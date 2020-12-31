/*
	Description:
	Volumetric cloud rendering, by compute shader.
	该shader使用的距离单位是米，所以以千米为单位的数据会 * 1000
*/


#version 450 core
#define INTERSECTION_ANALTICALLY

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout (binding = 0, rgba32f) readonly uniform image2D renderFBO;
//layout (binding = 1, rgba32f) readonly uniform image3D cloudBaseTexture;
//layout (binding = 2, rgba32f) readonly uniform image3D cloudDetailTexture;
//ayout (binding = 3, rgba8) readonly uniform image2D weatherMap;
//layout (binding = 4, r32f) readonly uniform image2D blueNoiseTexture;
//Result Image
layout (binding = 5, rgba32f) writeonly uniform image2D cloudTex;
layout (binding = 6) uniform sampler3D cloudBaseTexture;
layout (binding = 7) uniform sampler3D cloudDetailTexture;
layout (binding = 8) uniform sampler2D weatherMap;
layout (binding = 9) uniform sampler2D blueNoiseTexture;

//View ray parameters
uniform vec3 cameraPos;
uniform mat4 viewMatrix;
uniform vec2 projFactor;
uniform vec2 resolution;

//Ray marching parameters
uniform int sampleSteps;
uniform int lightSampleSteps;

//Cloud modeling parameters
uniform float globalCoverage;
uniform float globalDensity;
uniform float cloudScale;
uniform float cloudOffset;

//Lighting model parameters
uniform vec3 sunDirection;
uniform float firstRayMarchingFactor = 1.f;
uniform float secondRayMarchingFactor = 1.f;
vec3 lightRay = normalize(vec3(10.f, 10.f, 10.f));
vec3 lightColor = vec3(1.f, 1.f, 1.f);
float eccentricityG = 0.2f;

//Geometrical parameters
uniform vec3 earthCenter;
uniform float cloudHeightAboveGround;
uniform float cloudLayerLength;
float cloudInnerRadius = -earthCenter.y * 1000 + cloudHeightAboveGround;
float cloudOuterRadius = cloudInnerRadius + cloudLayerLength;

//Solve ray-sphere intersection analytically, rayDir must be normalized
float RaySphereIntersection(vec3 rayDir, float radius, out float t0, out float t1) {

	vec3 camRayVector = cameraPos * 1000 - earthCenter * 1000;
	//using discriminant of quadratic equation to solve the intersection.
	float a = dot(rayDir, rayDir);
	float b = 2 * dot(rayDir, camRayVector);
	float c = dot(camRayVector, camRayVector) - radius * radius; // the square of radius can be precalculated

	float discr = b * b - 4 * a * c;
	if (discr < 0) return discr;
	else if (discr == 0) t0 = t1 = - 0.5 * b / a;
	else {
		float q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
		t0 = q / a;
		t1 = c / q;
	}

	if( t0 > t1 ) { //make sure t0 < t1
		float temp = t1;
		t1 = t0;
		t0 = temp;
	}
	
	return discr;
}

//Camera can be placed below, inside or above the cloud layer.
//Each case has one or more possible intersections when looking
//at different angles.
//Currently there's a thorny bug when camera is inside or above
//the cloud layer, that the space above horizon appears to be empty.
bool cloudLayerIntersection( vec3 rayDir, out vec3 startingPoint, out float marchingLength){

	float cloudEntry = 0;	//The entry point of cloud layer
	float cloudExit = 0;	//The exit

	float t0Outer, t1Outer;
	float deltaOuter = RaySphereIntersection(rayDir, cloudOuterRadius, t0Outer, t1Outer);
	float t0Inner, t1Inner;
	float deltaInner = RaySphereIntersection(rayDir, cloudInnerRadius, t0Inner, t1Inner);

	if( deltaOuter > 0 && (t0Outer > 0 || t1Outer > 0) ){
		if( t0Outer > 0 && t1Outer > 0 ){  //Above cloud layer
			cloudEntry = t0Outer;
			if( deltaInner > 0 ){ //Hit inner cloud layer
				cloudExit = t0Inner;
			}else{ //Not hit
				cloudExit = t1Outer;
			}
		}else{  //Inside or below cloud layer
			if( deltaInner > 0 && (t0Inner > 0 || t1Inner > 0) ){
				if( t0Inner > 0 && t1Inner > 0 ){
					//Inside cloud layer, looking downwards
					cloudEntry = 0;
					cloudExit = t0Inner;
				}else{
					//Below cloud layer
					float t0Earth, t1Earth;
					float deltaEarth = RaySphereIntersection(rayDir, -earthCenter.y * 1000, t0Earth, t1Earth);
					if( deltaEarth > 0 && (t0Earth > 0 && t1Earth > 0) ) return false; // ray blocked by the Earth.
					cloudEntry = t1Inner;
					cloudExit = t1Outer;
				}
			}else { //Inside cloud layer, looking either upwards, left or right.
				cloudEntry = 0;
				cloudExit = t1Outer;
			}
		}
	}

	startingPoint = cameraPos * 1000 + cloudEntry * rayDir;
	marchingLength = length((cloudExit - cloudEntry) * rayDir);

	return (marchingLength == 0) ? false : true;
}

//Remapping function
float map(float value, float min1, float max1, float min2, float max2) {
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

//Shape-altering height function.
//It makes bottom thick, and makes top thin.
float ShapeHeightFunc(float ph, float b) {
	float SRbottom = map(ph, 0.f, 0.3f, 0.f, 1.f);
	float SRtop = map(ph, 0.4f, 0.8f, 1.f, 0.f);
	return clamp(SRbottom * SRtop, 0, 1);
}

//Density-altering height function.
//It makes bottom less tranparent than the top.
float DensityHeightFunc(float ph, float a) {
	float DRbottom = ph * clamp(map(ph, 0.f, 0.15f, 0.f, 1.f), 0.f, 1.f);
	float DRtop = clamp(map(ph, 0.9f, 1.f, 1.f, 0.f), 0.f, 1.f);
	return globalDensity * DRbottom * DRtop * a * 2;
}

//Sample the FBM detail noise.
//The maximum influence it has is limited to 0.35, with a natural
//number to apply the effect of globalCoverage as well.
//The interpolation at return is for a more "rounded" shape.
float SampleDetailNoise(vec3 uvw, float pHeight) {
	vec3 rawDetailNoise = texture(cloudDetailTexture, uvw).rgb;
	float fbmDetail = rawDetailNoise.r * 0.625f + rawDetailNoise.g * 0.25f + rawDetailNoise.b * 0.125f;

	return 0.35f * exp(-globalCoverage * 0.75f) *
		mix(fbmDetail, 1.f - fbmDetail, clamp(pHeight * 5, 0.f, 1.f));
}

//Sample the base shape of the cloud
float SampleDensity(vec3 pos, float pHeight, bool sampleDetail) {

	vec3 uvw = pos  * 0.0001 * cloudScale + cloudOffset * 0.1;
	uvw.y = pHeight * 0.2 + 0.2 + cloudOffset * 0.1;

	vec3 weatherMap = texture(weatherMap, uvw.xz).xyz;
	weatherMap.z = 1.f;
	float wmAlpha = 1.f;

	//Sample the raw data from noise texture.
	vec4 rawData = texture(cloudBaseTexture, uvw);
	float fbmWorley = rawData.g * 0.625f + rawData.b * 0.25f + rawData.a * 0.125f;
	float baseNoise = map(rawData.r, -(1.f - fbmWorley), 1.f, 0.f, 1.f);

	float baseheightModified = baseNoise * ShapeHeightFunc(pHeight, weatherMap.b);

	float weatherMapFactor = globalCoverage * max(weatherMap.r, clamp(globalCoverage - 0.5f, 0.f, 1.f) * weatherMap.g * 2.f);
	float baseCloud = clamp(map(baseheightModified, 1.f - weatherMapFactor, 1.f, 0.f, 1.f), 0.f, 1.f);

	if (sampleDetail) {
		float detailedCloud = clamp(map(baseCloud, SampleDetailNoise(uvw, pHeight), 1.f, 0.f, 1.f), 0.f, 1.f);
		//Density-altering height function at last
		return detailedCloud;// * DensityHeightFunc(pHeight, wmAlpha);
	}

	return baseCloud;
}

float HenyeyGreenstein(float cosAngle, float g)
{
	float g2 = g * g;
	return ((1.0 - g2) / pow((1.0 + g2 - 2.0 * g * cosAngle), 1.5)) / 4.0 * 3.1415;
}

vec4 RayMarching(vec3 rayDir, vec3 pos, float marchingLength){

	float density = 0.f;
	float radiance = 0.f;
	float T = 1.f; //transmittance

	float stepLength = cloudLayerLength / sampleSteps;
	vec3 stepDir = stepLength * rayDir;
	int totalSteps = int(marchingLength / stepLength);

	float densityTest = 0.f;
	int zeroDensityCount = 0;
	float blueNoise = texture(blueNoiseTexture, pos.xz / 10.f).x;
	pos += (blueNoise - 0.5) * 2 * stepDir;

	for (int i = 0; i < totalSteps; i++) { //Primary raymarching

		//Calculate the height percentage of the sample point in cloud layer. (very important)
		float pHeight = (length(pos - earthCenter * 1000) - cloudInnerRadius) / cloudLayerLength;

		if (densityTest > 0.f) { //Full sampling, otherwise cheap sampling
			float sampleDensity = SampleDensity(pos, pHeight, true);
			if (sampleDensity == 0) {
				zeroDensityCount++;
			}

			if (zeroDensityCount != 6) { //early exit check
				if (sampleDensity != 0) {

					density += sampleDensity;
					T *= exp(-density * stepLength * firstRayMarchingFactor * 0.01);

					float lightRayDensity = 0.f;
					float Ts = 1.f; //transmittance for sample point and sun

					vec3 lightSamplePos = pos;
					float sunStepLength = 0.5f * cloudLayerLength / lightSampleSteps;
					vec3 sunStepSize = sunStepLength * sunDirection;

					for (int j = 0; j < lightSampleSteps; j++) { //Secondary raymarching
						lightRayDensity += SampleDensity(lightSamplePos, pHeight, true);
						Ts *= exp(-lightRayDensity * sunStepLength * secondRayMarchingFactor * 0.01);
						lightSamplePos += sunStepSize;
					}
					//Attenuation Term
					Ts = max(Ts, exp(-1 * sunStepLength * secondRayMarchingFactor * 0.01));// + blueNoise * 0.0003f;
					//ambient absorption
					float osa = 0.5f;
					float OSambient = 1 - clamp(osa * pow(sampleDensity, map(pHeight, 0.3, 0.9, 0.5, 1.0)), 0.f, 1.f) *
										  clamp(pow(map(pHeight, 0.0, 0.3, 0.8, 1.0), 0.8f) , 0.f, 1.f);

					float sunViewRayAngle = clamp(dot(sunDirection, rayDir), 0, 1);
					const float csi = 2.5;
					const float cse = 2;
					float ISA = csi * pow(clamp(sunViewRayAngle, 0.f, 1.f), cse);
					float io = mix( max(HenyeyGreenstein(sunViewRayAngle, 0.2), ISA) , HenyeyGreenstein(sunViewRayAngle, -0.1),0.5);

					//float inScatteringRadiance = Ts * HenyeyGreenstein(sunViewRayAngle, eccentricityG);
					float inScatteringRadiance = Ts * io;

					radiance += T * 0.05 * inScatteringRadiance * OSambient * stepLength;
					//radiance += T * 0.05 * inScatteringRadiance * stepLength;

					if (T < 0.01f) break; //Early exit if converging.
				}
				pos += stepDir;
			}
			else {
				densityTest = 0.f;
				zeroDensityCount = 0;
			}
		}
		else {
			densityTest = SampleDensity(pos, pHeight, false);
			if (densityTest == 0) pos += stepDir;
			else pos -= stepDir;
		}
	}

	return vec4(vec3(radiance), T);
}

void main(void)
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);//Current pixel
	//vec2 uv = (p - .5 * resolution.xy) / resolution.y;
	vec2 uv = vec2(p) / resolution - 0.5f;
	uv *= projFactor;
	vec3 rayDir = mat3(viewMatrix) * normalize(vec3(uv.x, -uv.y, -0.5)); //why 0.5????

	vec3 color = imageLoad(renderFBO, ivec2(p.x, resolution.y - p.y)).xyz;

	vec3 startingPoint;							//Intersection point
	float marchingLength;						//Length to march
	vec4 cloudColor;

	if (cloudLayerIntersection(rayDir, startingPoint, marchingLength)) {

		cloudColor = RayMarching(rayDir, startingPoint, marchingLength);

		cloudColor.rgb = lightColor * pow(vec3(1.f) - exp(-cloudColor.rgb), vec3(1.0/2.2));
		color = mix(cloudColor.rgb, color, cloudColor.a);
	}


	imageStore(cloudTex, p, vec4(color, 1.f));
}
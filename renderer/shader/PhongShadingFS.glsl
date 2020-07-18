#version 450 core

uniform int UseTexture = 0;
uniform sampler2D ourTexture;

uniform vec3 cameraPos;
uniform vec4 LightPos;
uniform vec4 LightColor;

in VS {
	vec3 color;
	vec2 texCoord;
	vec3 vPos;
	vec3 vNormal;
}IN;

out vec4 FragColor;

void main() {
	vec4 diffuse = texture(ourTexture, IN.texCoord) * vec4(IN.color, 1.0) * LightColor;

	vec3 n = normalize(IN.vNormal);
	vec3 v = normalize(cameraPos - IN.vPos);
	vec3 l = normalize(LightPos.xyz - IN.vPos);
	vec3 halfDir = normalize(l + v);

	//How much diffuse reflection
	float lambert = clamp(dot(n, l), 0.0, 1.0);
	//How much specular reflection
	float rFactor = clamp(dot(halfDir, n), 0.0, 1.0);
	float sFactor = pow(rFactor, 60.0); // configurable

	vec3 color	=  diffuse.rgb;							// diffuse
		 color	+= (LightColor.rgb * sFactor) * 0.4;	// specularity, the right number is configurable
		 color	*= lambert;								// No attenuation for now
		 color	+= diffuse.rgb * 0.1;					// ambience;
	
	FragColor = vec4(color, diffuse.a);
}
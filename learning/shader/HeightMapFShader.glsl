#version 450 core

uniform int UseTexture = 0;
uniform sampler2D ourTexture;

uniform vec3 cameraPos;
uniform vec4 LightPos;
uniform vec4 LightColor;

in VS
{
	vec2 texCoord;
	vec3 vPos;
	vec3 vNormal;
}IN;

out vec4 FragColor;

vec3 lit(vec3 l, vec3 n, vec3 v)
{
	vec3 r_l = reflect(-l, n);
	float s = clamp(100.0 * dot(r_l, v) - 97.0, 0.0, 1.0);
	vec3 highlightColor = vec3(2, 2, 2);
	vec3 uWarmColor = (0.3, 0.3, 0.0) + 0.25 * texture(ourTexture, IN.texCoord).xyz;
	return mix(uWarmColor, highlightColor, s);
}

void main()
{
	//Lightings
	vec3 n = normalize(IN.vNormal);
	vec3 v = normalize(cameraPos - IN.vPos);
	FragColor = ((0.0, 0.0, 0.55) + 0.25 * texture(ourTexture, IN.texCoord)) * 0.5; // The "cold" color (unlit)

	vec3 l = normalize(LightPos.xyz - IN.vPos);
	float NdL = clamp(dot(n, l), 0.0, 1.0);
	FragColor.rgb += NdL * LightColor.rgb * lit(l, n, v);
}
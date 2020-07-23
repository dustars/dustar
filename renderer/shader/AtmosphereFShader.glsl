/**
 * Copyright (c) 2017 Eric Bruneton
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#version 450 core
//#define USE_LUMINANCE
const float kLengthUnitInMeters = 1000.0;

uniform vec3 camera;
uniform float exposure;
uniform vec3 white_point;
uniform vec3 earth_center;
uniform vec3 sun_direction;
uniform vec2 sun_size;
in vec3 view_ray;
layout(location = 0) out vec4 color;

const float PI = 3.14159265;
const vec3 kSphereAlbedo = vec3(0.8);
const vec3 kGroundAlbedo = vec3(0.0, 0.0, 0.04);

#ifdef USE_LUMINANCE
#define GetSolarRadiance GetSolarLuminance
#define GetSkyRadiance GetSkyLuminance
#define GetSkyRadianceToPoint GetSkyLuminanceToPoint
#define GetSunAndSkyIrradiance GetSunAndSkyIlluminance
#endif

vec3 GetSolarRadiance();
vec3 GetSkyRadiance(vec3 camera, vec3 view_ray, float shadow_length,
    vec3 sun_direction, out vec3 transmittance);
vec3 GetSkyRadianceToPoint(vec3 camera, vec3 point, float shadow_length,
    vec3 sun_direction, out vec3 transmittance);
vec3 GetSunAndSkyIrradiance(
    vec3 p, vec3 normal, vec3 sun_direction, out vec3 sky_irradiance);


void main() {
    // Normalized view direction vector.
    vec3 view_direction = normalize(view_ray);
    // Tangent of the angle subtended by this fragment.
    float fragment_angular_size =
        length(dFdx(view_ray) + dFdy(view_ray)) / length(view_ray);

    // Hack to fade out light shafts when the Sun is very close to the horizon.
    float lightshaft_fadein_hack = smoothstep(
        0.02, 0.04, dot(normalize(camera - earth_center), sun_direction));    

    // Compute the distance between the view ray line and the Earth center,
    // and the distance between the camera and the intersection of the view
    // ray with the ground (or NaN if there is no intersection).
    vec3 p = camera - earth_center;
    float p_dot_v = dot(p, view_direction);
    float p_dot_p = dot(p, p);
    float ray_earth_center_squared_distance = p_dot_p - p_dot_v * p_dot_v;
    float distance_to_intersection = - p_dot_v - sqrt(
        earth_center.z * earth_center.z - ray_earth_center_squared_distance);

    // Compute the radiance reflected by the ground, if the ray intersects it.
    float ground_alpha = 0.0;
    vec3 ground_radiance = vec3(0.0);
    if (distance_to_intersection > 0.0) {
        vec3 point = camera + view_direction * distance_to_intersection;
        vec3 normal = normalize(point - earth_center);

        // Compute the radiance reflected by the ground.
        vec3 sky_irradiance;
        vec3 sun_irradiance = GetSunAndSkyIrradiance(
            point - earth_center, normal, sun_direction, sky_irradiance);
        ground_radiance = kGroundAlbedo * (1.0 / PI);

        vec3 transmittance;
        vec3 in_scatter = GetSkyRadianceToPoint(camera - earth_center,
            point - earth_center, 0, sun_direction, transmittance);
        ground_radiance = ground_radiance * transmittance + in_scatter;
        ground_alpha = 1.0;
    }

    // Compute the radiance of the sky.
    vec3 transmittance;
    vec3 radiance = GetSkyRadiance(
        camera - earth_center, view_direction, 0, sun_direction,
        transmittance);

    // If the view ray intersects the Sun, add the Sun radiance.
    if (dot(view_direction, sun_direction) > sun_size.y) {
        radiance = radiance + transmittance * GetSolarRadiance();
    }
    radiance = mix(radiance, ground_radiance, ground_alpha);
    color.rgb =
        pow(vec3(1.0) - exp(-radiance / white_point * exposure), vec3(1.0 / 2.2));
    color.a = 1.0;
}
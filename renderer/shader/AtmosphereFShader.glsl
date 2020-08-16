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

//uniform vec3 camera;
//Use hard-coded camera position to fix the atmospheric scattering.
vec3 camera = vec3(0, 0, 2.f);
uniform float exposure;
uniform vec3 white_point;
uniform vec3 earth_center;
uniform vec3 sun_direction;
uniform vec2 sun_size;
in vec3 view_ray;
layout(location = 0) out vec4 color;

const float PI = 3.14159265;
const vec3 kSphereCenter = vec3(0.0, 0.0, 1000.0) / kLengthUnitInMeters;
const float kSphereRadius = 1000.0 / kLengthUnitInMeters;
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
    vec3 view_direction = normalize(view_ray);

    //Compute the radiance of the sky.
    vec3 transmittance;
    vec3 radiance = GetSkyRadiance(
        camera - earth_center, view_direction, 0, sun_direction,
        transmittance);

    //If the view ray intersects the Sun, add the Sun radiance.
    if (dot(view_direction, sun_direction) > sun_size.y) {
        radiance = radiance + transmittance * GetSolarRadiance();
    }

    color.rgb = pow(vec3(1.0) - exp(-radiance / white_point * exposure), vec3(1.0 / 2.2));
    color.a = 1.0;
}
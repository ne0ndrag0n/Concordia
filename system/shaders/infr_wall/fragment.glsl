#version 330 core
in vec2 fragTexture;
in vec3 fragNormal;
in vec3 fragPos;
out vec4 color;

#include "system/shaders/common/material.glsl"
#include "system/shaders/common/directional_light.glsl"
#include "system/shaders/common/room.glsl"

uniform Material material;
uniform float wallBaseline;

void main() {
  if( fragPos.z < wallBaseline ) {
    discard;
  }

  DirectionalLight light = directionalLights[ 0 ];

  vec3 texResult = texture( material.diffuse0, fragTexture ).rgb;

  vec3 norm = normalize( fragNormal );
  vec3 lightDirection = normalize( -light.direction );
  float diffTheta = max( dot( norm, lightDirection ), 0.0 );

  vec3 ambient = light.ambient * texResult;
  vec3 diffuse = light.diffuse * diffTheta * texResult;

  color = vec4( ambient + diffuse, material.opacity );
}

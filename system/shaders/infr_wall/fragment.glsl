#version 450 core
in vec2 fragTexture;
in vec3 fragNormal;
in vec3 fragPos;
out vec4 color;

#include "system/shaders/common/material_texture.glsl"
#include "system/shaders/common/directional_light.glsl"
#include "system/shaders/common/room.glsl"

uniform Material material;
uniform float wallBaseline;

void main() {
  if( fragPos.z < ( wallBaseline * 4.0f ) ) {
    discard;
  }

  DirectionalLight light = getRoomLight( fragPos );

  vec3 texResult = texture( material.diffuse0, fragTexture ).rgb;

  vec3 norm = normalize( fragNormal );
  vec3 lightDirection = normalize( -light.direction.xyz );
  float diffTheta = max( dot( norm, lightDirection ), 0.0 );

  vec3 ambient = light.ambient.xyz * texResult;
  vec3 diffuse = light.diffuse.xyz * diffTheta * texResult;

  color = vec4( ambient + diffuse, material.opacity );
}

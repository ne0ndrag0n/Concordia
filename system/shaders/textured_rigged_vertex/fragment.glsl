#version 450 core
in vec2 fragTexture;
in vec3 fragNormal;
in vec3 fragPos;
out vec4 color;

#include "system/shaders/common/directional_light.glsl"
#include "system/shaders/common/material.glsl"
#include "system/shaders/common/camera.glsl"

uniform Material material;
uniform vec4 highlight;

void main() {
  vec3 cameraPos = cameraPos.xyz;

  vec3 texResult = texture( material.diffuse0, fragTexture ).xyz;

  vec3 norm = normalize( fragNormal );
  vec3 viewDirection = normalize( cameraPos - fragPos );

  vec3 lightDirection = normalize( -directionalLights[ 0 ].direction.xyz );
  float diffTheta = max( dot( norm, lightDirection ), 0.0 );

  vec3 reflectDirection = reflect( -lightDirection, norm );
  float specTheta = pow( max( dot( viewDirection, reflectDirection ), 0.0 ), material.shininess );

  vec3 ambient = directionalLights[ 0 ].ambient.xyz * texResult;
  vec3 diffuse = directionalLights[ 0 ].diffuse.xyz * diffTheta * texResult;
  vec3 specular = directionalLights[ 0 ].specular.xyz * specTheta * texResult;

  color = vec4( ambient + diffuse + specular, material.opacity ) + highlight;
}

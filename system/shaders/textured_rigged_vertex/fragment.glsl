#version 450 core
in vec2 fragTexture;
in vec3 fragNormal;
in vec3 fragPos;
out vec4 color;

#include "system/shaders/common/directional_light.glsl"

struct Material {
  sampler2D diffuse0;
  float shininess;
  float opacity;
};

uniform vec3 cameraPos;
uniform Material material;
uniform vec4 highlight;

void main() {
  vec3 texResult = texture( material.diffuse0, fragTexture ).xyz;

  vec3 norm = normalize( fragNormal );
  vec3 viewDirection = normalize( cameraPos - fragPos );

  vec3 lightDirection = normalize( -directionalLights[ 0 ].direction );
  float diffTheta = max( dot( norm, lightDirection ), 0.0 );

  vec3 reflectDirection = reflect( -lightDirection, norm );
  float specTheta = pow( max( dot( viewDirection, reflectDirection ), 0.0 ), material.shininess );

  vec3 ambient = directionalLights[ 0 ].ambient * texResult;
  vec3 diffuse = directionalLights[ 0 ].diffuse * diffTheta * texResult;
  vec3 specular = directionalLights[ 0 ].specular * specTheta * texResult;

  color = vec4( ambient + diffuse + specular, material.opacity ) + highlight;
}

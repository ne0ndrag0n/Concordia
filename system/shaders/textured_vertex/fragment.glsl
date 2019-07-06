#version 450 core
in vec2 fragTexture;
in vec3 fragNormal;
in vec3 fragPos;
out vec4 color;

/*
struct Material {
  sampler2D diffuse0;
  float shininess;
  float opacity;
};

struct DirectionalLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

#include "system/shaders/common/camera.glsl"
uniform Material material;
uniform DirectionalLight directionalLight;
*/

void main() {
  /*
  vec3 cameraPos = cameraPos.xyz;

  vec3 texResult = texture( material.diffuse0, fragTexture ).xyz;

  vec3 norm = normalize( fragNormal );
  vec3 viewDirection = normalize( cameraPos - fragPos );

  vec3 lightDirection = normalize( -directionalLight.direction );
  float diffTheta = max( dot( norm, lightDirection ), 0.0 );

  vec3 reflectDirection = reflect( -lightDirection, norm );
  float specTheta = pow( max( dot( viewDirection, reflectDirection ), 0.0 ), material.shininess );

  vec3 ambient = directionalLight.ambient * texResult;
  vec3 diffuse = directionalLight.diffuse * diffTheta * texResult;
  vec3 specular = directionalLight.specular * specTheta * texResult;

  color = vec4( ambient + diffuse + specular, material.opacity );
  */
  color = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
}

#version 330 core
out vec4 fragColor;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec3 fragPos;
in vec3 fragNormal;

uniform vec3 cameraPos;
uniform Material material;
uniform Light light;

void main() {
  // ambient
  vec3 ambient = light.ambient * material.ambient;

  // diffuse
  vec3 norm = normalize( fragNormal );
  vec3 lightDirection = normalize( light.position - fragPos );
  float difference = max( dot( norm, lightDirection ), 0.0 );
  vec3 diffuse = light.diffuse * ( difference * material.diffuse );

  // specular
}

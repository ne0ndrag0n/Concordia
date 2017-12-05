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

  // specular
}

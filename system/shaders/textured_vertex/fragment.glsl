#version 330 core
in vec2 fragTexture;
in vec3 fragNormal;
in vec3 fragPos;
out vec4 color;

struct Material {
  vec3 ambient;
  sampler2D diffuse0;
  vec3 specular;
  float shininess;
};

uniform vec3 cameraPos;
uniform Material material;

void main() {
  color = texture( material.diffuse0, fragTexture );
}

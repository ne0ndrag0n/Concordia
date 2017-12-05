#version 330 core
in vec2 fragTexture;
in vec3 fragNormal;
in vec3 fragPos;

out vec4 color;

uniform vec3 cameraPos;
uniform sampler2D diffuse0;

void main() {
  color = texture( diffuse0, fragTexture );
}

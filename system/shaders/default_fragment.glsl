#version 330 core
in vec2 fragTexture;

out vec4 color;

uniform sampler2D diffuse0;

void main() {
  color = texture( diffuse0, fragTexture );
}

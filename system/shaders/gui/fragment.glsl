#version 450 core
in vec2 fragTexture;
out vec4 color;

uniform sampler2D surface;

void main() {
  color = texture( surface, fragTexture );
}

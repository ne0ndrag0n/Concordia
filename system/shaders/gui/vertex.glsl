#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture;

out vec2 fragTexture;

uniform mat4 orthoProjection;
uniform mat4 translation;

void main() {
  gl_Position = orthoProjection * translation * vec4( position, 1.0f );
  fragTexture = texture;
}

#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture;

out vec2 fragTexture;
uniform mat4 orthoProjection;

void main() {
  fragTexture = texture;
  gl_Position = orthoProjection * vec4( position, 1.0f );
}

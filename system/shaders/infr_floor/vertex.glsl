#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture;

out vec2 fragTexture;
out vec3 fragNormal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4( position, 1.0f );
  fragTexture = texture;
  fragNormal = mat3( transpose( inverse( model ) ) ) * normal;
  fragPos = vec3( model * vec4( position, 1.0 ) );
}

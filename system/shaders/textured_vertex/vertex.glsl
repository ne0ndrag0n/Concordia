#version 330 core
layout (location = 0) in vec3 position; // The position variable has attribute position 0
layout (location = 1) in vec3 normal; // This is currently unused
layout (location = 2) in vec2 texture;

out vec2 fragTexture;
out vec3 fragNormal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  mat4 mvp = projection * view * model;

  gl_Position = mvp * vec4( position, 1.0f );
  fragTexture = texture;
  fragNormal = normal;
  fragPos = vec3( model * vec4( position, 1.0 ) );
}

#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 fragNormal;
out vec3 fragPos;

uniform mat4 model;
layout (std140) uniform Camera {
  vec4 cameraPos;
  mat4 view;
  mat4 projection;
};

void main() {
  gl_Position = projection * view * model * vec4( position, 1.0f );
  fragNormal = mat3( transpose( inverse( model ) ) ) * normal;
  fragPos = vec3( model * vec4( position, 1.0 ) );
}

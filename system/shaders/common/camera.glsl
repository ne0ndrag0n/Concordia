layout (std140, binding = 0) uniform Camera {
  vec4 cameraPos;
  mat4 view;
  mat4 projection;
};
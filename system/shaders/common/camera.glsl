layout (std140) uniform Camera {
  vec4 cameraPos;
  mat4 view;
  mat4 projection;
};
#include "system/shaders/common/ubo_bindings.glsl"

layout (std140, binding = BLUEBEAR_CAMERA_BINDING) uniform Camera {
  vec4 cameraPos;
  mat4 view;
  mat4 projection;
};
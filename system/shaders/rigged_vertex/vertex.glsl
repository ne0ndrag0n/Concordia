#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in ivec4 boneIDs;
layout (location = 3) in vec4 boneWeights;

out vec3 fragNormal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 bones[ 16 ];

void main() {
  mat4 boneTransform =
    ( bones[ boneIDs[ 0 ] ] * boneWeights[ 0 ] ) +
    ( bones[ boneIDs[ 1 ] ] * boneWeights[ 1 ] ) +
    ( bones[ boneIDs[ 2 ] ] * boneWeights[ 2 ] ) +
    ( bones[ boneIDs[ 3 ] ] * boneWeights[ 3 ] );

  gl_Position = projection * view * model * boneTransform * vec4( position, 1.0f );
  fragNormal = mat3( transpose( inverse( model ) ) ) * mat3( boneTransform ) * normal;
  fragPos = vec3( model * vec4( position, 1.0 ) );
}

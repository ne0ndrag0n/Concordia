#version 330 core
layout (location = 0) in vec3 position; // The position variable has attribute position 0
layout (location = 1) in vec3 normal; // This is currently unused
layout (location = 2) in vec2 texture;
layout (location = 3) in ivec4 boneIDs;
layout (location = 4) in vec4 boneWeights;

out vec2 fragTexture;
out vec3 fragNormal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 bones[ 16 ];

void main()
{
  mat4 boneTransform =
    ( bones[ boneIDs[ 0 ] ] * boneWeights[ 0 ] ) +
    ( bones[ boneIDs[ 1 ] ] * boneWeights[ 1 ] ) +
    ( bones[ boneIDs[ 2 ] ] * boneWeights[ 2 ] ) +
    ( bones[ boneIDs[ 3 ] ] * boneWeights[ 3 ] );

  gl_Position = projection * view * boneTransform * model * vec4( position, 1.0f );
  fragTexture = texture;
  fragNormal = normal;
  fragPos = vec3( model * vec4( position, 1.0 ) );
}

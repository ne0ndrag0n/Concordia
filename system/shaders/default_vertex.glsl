#version 330 core
layout (location = 0) in vec3 position; // The position variable has attribute position 0
layout (location = 1) in vec3 normal; // This is currently unused
layout (location = 2) in vec2 texture;
layout (location = 3) in ivec4 boneIDs;
layout (location = 4) in vec4 boneWeights;

out vec2 fragTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    fragTexture = texture;
}

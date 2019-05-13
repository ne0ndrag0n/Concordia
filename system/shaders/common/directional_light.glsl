struct DirectionalLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform DirectionalLight directionalLights[ 16 ];
uniform uint numDirectionalLights;
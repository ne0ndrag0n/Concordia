#version 330 core
in vec3 fragNormal;
in vec3 fragPos;
out vec4 color;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
  float opacity;
};

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform vec3 cameraPos;
uniform Material material;
uniform uint numLights;
uniform Light lights[ 16 ];

void main() {
  vec3 result = vec3( 0.0, 0.0, 0.0 );

  for( uint i = 0u; i != numLights; i++ ) {
    vec3 ambient = lights[ i ].ambient * material.ambient;

    vec3 normal = normalize( fragNormal );
    vec3 lightDirection = normalize( lights[ i ].position - fragPos );
    float theta = max( dot( normal, lightDirection ), 0.0 );
    vec3 diffuse = lights[ i ].diffuse * ( theta * material.diffuse );

    vec3 viewDirection = normalize( cameraPos - fragPos );
    vec3 reflectDirection = reflect( -lightDirection, normal );
    float spec = pow( max( dot( viewDirection, reflectDirection ), 0.0 ), material.shininess );
    vec3 specular = lights[ i ].specular * ( spec * material.specular );

    result += ambient + diffuse + specular;
  }

  color = vec4( result, material.opacity );
}

#version 330

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float     shininess;
}; 

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

out vec4 frag_col;

in vec3 frag_3Dpos;
in vec3 normal;
in vec2 vs_tex_coord;

uniform Material material;
uniform Light light;  
uniform Light light2;  
uniform vec3 view_pos;

void main() {

  // Ambient
  vec3 ambient = light.ambient * texture(material.diffuse, vs_tex_coord).rgb;
  vec3 light_dir = normalize(light.position - frag_3Dpos);

  vec3 ambient2 = light2.ambient * texture(material.diffuse, vs_tex_coord).rgb;
  vec3 light_dir2 = normalize(light2.position - frag_3Dpos);

  // Diffuse
  float diff = max(dot(normal, light_dir), 0.0);
  vec3 diffuse = light.diffuse * diff * texture(material.diffuse, vs_tex_coord).rgb;

  float diff2 = max(dot(normal, light_dir2), 0.0);
  vec3 diffuse2 = light2.diffuse * diff2 * texture(material.diffuse, vs_tex_coord).rgb;
  
  // Specular
  vec3 view_dir = normalize(view_pos - frag_3Dpos);

  vec3 reflect_dir = reflect(-light_dir, normal);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
  vec3 specular = light.specular * spec * texture(material.specular, vs_tex_coord).rgb;

  vec3 reflect_dir2 = reflect(-light_dir2, normal);
  float spec2 = pow(max(dot(view_dir, reflect_dir2), 0.0), material.shininess);
  vec3 specular2 = light2.specular * spec2 * texture(material.specular, vs_tex_coord).rgb;

  vec3 result = ambient + diffuse + specular + ambient2 + diffuse2 + specular2;
  frag_col = vec4(result, 1.0);
}

#version 330

in vec3 v_pos;
in vec3 v_normal;
in vec2 v_textura;

out vec3 frag_3Dpos;
out vec3 normal;
out vec2 vs_tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normal_matrix;

void main() {
  gl_Position = projection * view * model * vec4(v_pos,1.0f);
  frag_3Dpos = vec3(model * vec4(v_pos,1.0));
  normal = normalize(mat3(normal_matrix) * v_normal);
  vs_tex_coord = v_textura;
}

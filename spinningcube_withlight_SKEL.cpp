// Copyright (C) 2020 Emilio J. Padrón
// Released as Free Software under the X11 License
// https://spdx.org/licenses/X11.html

//Ejecucion:
//gcc spinningcube_withlight_SKEL.cpp textfile.c -lGL -lGLEW -lglfw -lm -o spinningcube_withlight_SKEL


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

// GLM library to deal with matrix operations
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::perspective
#include <glm/gtc/type_ptr.hpp>

#include "textfile_ALT.h"

int gl_width = 640;
int gl_height = 480;

void glfw_window_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void render(double);

GLuint shader_program = 0; // shader program to set render pipeline
GLuint vao = 0; // Vertext Array Object to set input data
GLuint vao2 = 0; 
GLint model_location, view_location, proj_location; // Uniforms for transformation matrices
GLint normal_location;
GLint light_position_location, light_ambient_location, light_diffuse_location, light_specular_location;
GLint light_position_location2, light_ambient_location2, light_diffuse_location2, light_specular_location2;

GLint material_ambient_location, material_diffuse_location, material_specular_location, material_shininess_location;  
GLint camera_position_location;

// Shader names
const char *vertexFileName = "spinningcube_withlight_vs_SKEL.glsl";
const char *fragmentFileName = "spinningcube_withlight_fs_SKEL.glsl";

// Camera
glm::vec3 camera_pos(0.0f, 0.0f, 3.0f);

// Lighting
glm::vec3 light_pos(10.0f, 1.0f, 0.5f);
glm::vec3 light_ambient(0.2f, 0.2f, 0.2f);
glm::vec3 light_diffuse(0.5f, 0.5f, 0.5f);
glm::vec3 light_specular(1.0f, 1.0f, 1.0f);

// Lighting Tetraedro
glm::vec3 light_pos2(-10.0f, 1.0f, 0.5f);

// Material
glm::vec3 material_ambient(1.0f, 0.5f, 0.31f);
glm::vec3 material_diffuse(1.0f, 0.5f, 0.31f);
glm::vec3 material_specular(0.5f, 0.5f, 0.5f);

//GLint material_diffuse = 0;
//GLint material_specular = 1;
const GLfloat material_shininess = 32.0f;

int main() {
  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return 1;
  }

  //  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  //  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  //  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(gl_width, gl_height, "My spinning cube", NULL, NULL);
  if (!window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwSetWindowSizeCallback(window, glfw_window_size_callback);
  glfwMakeContextCurrent(window);

  // start GLEW extension handler
  // glewExperimental = GL_TRUE;
  glewInit();

  // get version info
  const GLubyte* vendor = glGetString(GL_VENDOR); // get vendor string
  const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
  const GLubyte* glversion = glGetString(GL_VERSION); // version as a string
  const GLubyte* glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION); // version as a string
  printf("Vendor: %s\n", vendor);
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", glversion);
  printf("GLSL version supported %s\n", glslversion);
  printf("Starting viewport: (width: %d, height: %d)\n", gl_width, gl_height);

  // Enable Depth test: only draw onto a pixel if fragment closer to viewer
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); // set a smaller value as "closer"

  // Vertex Shader
  char* vertex_shader = textFileRead(vertexFileName);

  // Fragment Shader
  char* fragment_shader = textFileRead(fragmentFileName);

  // Shaders compilation
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, NULL);
  free(vertex_shader);
  glCompileShader(vs);

  int  success;
  char infoLog[512];
  glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vs, 512, NULL, infoLog);
    printf("ERROR: Vertex Shader compilation failed!\n%s\n", infoLog);

    return(1);
  }

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, NULL);
  free(fragment_shader);
  glCompileShader(fs);

  glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fs, 512, NULL, infoLog);
    printf("ERROR: Fragment Shader compilation failed!\n%s\n", infoLog);

    return(1);
  }

  // Create program, attach shaders to it and link it
  shader_program = glCreateProgram();
  glAttachShader(shader_program, fs);
  glAttachShader(shader_program, vs);
  glLinkProgram(shader_program);

  glValidateProgram(shader_program);
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
    printf("ERROR: Shader Program linking failed!\n%s\n", infoLog);

    return(1);
  }

  // Release shader objects
  glDeleteShader(vs);
  glDeleteShader(fs);

  // Vertex Array Object
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Cube to be rendered
  //
  //          0        3
  //       7        4 <-- top-right-near
  // bottom
  // left
  // far ---> 1        2
  //       6        5
  //
  
  const GLfloat vertex_positions[] = {

    //positions                   //Normals
    -0.25f, -0.25f, -0.25f,     0.0f, 0.0f, -1.0f,    // 1  
    -0.25f,  0.25f, -0.25f,     0.0f, 0.0f, -1.0f,    // 0
     0.25f, -0.25f, -0.25f,     0.0f, 0.0f, -1.0f,    // 2

     0.25f,  0.25f, -0.25f,     0.0f, 0.0f, -1.0f,    // 3
     0.25f, -0.25f, -0.25f,     0.0f, 0.0f, -1.0f,    // 2
    -0.25f,  0.25f, -0.25f,     0.0f, 0.0f, -1.0f,    // 0

     0.25f, -0.25f, -0.25f,     1.0f, 0.0f, 0.0f,    // 2
     0.25f,  0.25f, -0.25f,     1.0f, 0.0f, 0.0f,    // 3
     0.25f, -0.25f,  0.25f,     1.0f, 0.0f, 0.0f,    // 5

     0.25f,  0.25f,  0.25f,     1.0f, 0.0f, 0.0f,    // 4
     0.25f, -0.25f,  0.25f,     1.0f, 0.0f, 0.0f,    // 5
     0.25f,  0.25f, -0.25f,     1.0f, 0.0f, 0.0f,    // 3

     0.25f, -0.25f,  0.25f,     0.0f, 0.0f, 1.0f,    // 5
     0.25f,  0.25f,  0.25f,     0.0f, 0.0f, 1.0f,    // 4
    -0.25f, -0.25f,  0.25f,     0.0f, 0.0f, 1.0f,    // 6

    -0.25f,  0.25f,  0.25f,     0.0f, 0.0f, 1.0f,   // 7
    -0.25f, -0.25f,  0.25f,     0.0f, 0.0f, 1.0f,   // 6
     0.25f,  0.25f,  0.25f,     0.0f, 0.0f, 1.0f,   // 4

    -0.25f, -0.25f,  0.25f,     -1.0f, 0.0f, 0.0f,   // 6
    -0.25f,  0.25f,  0.25f,     -1.0f, 0.0f, 0.0f,   // 7
    -0.25f, -0.25f, -0.25f,     -1.0f, 0.0f, 0.0f,   // 1

    -0.25f,  0.25f, -0.25f,     -1.0f, 0.0f, 0.0f,    // 0
    -0.25f, -0.25f, -0.25f,     -1.0f, 0.0f, 0.0f,    // 1
    -0.25f,  0.25f,  0.25f,     -1.0f, 0.0f, 0.0f,    // 7

     0.25f, -0.25f, -0.25f,     0.0f, -1.0f, 0.0f,    // 2
     0.25f, -0.25f,  0.25f,     0.0f, -1.0f, 0.0f,  // 5
    -0.25f, -0.25f, -0.25f,     0.0f, -1.0f, 0.0f,    // 1

    -0.25f, -0.25f,  0.25f,     0.0f, -1.0f, 0.0f,    // 6
    -0.25f, -0.25f, -0.25f,     0.0f, -1.0f, 0.0f,    // 1
     0.25f, -0.25f,  0.25f,     0.0f, -1.0f, 0.0f,    // 5

     0.25f,  0.25f,  0.25f,     0.0f, 1.0f, 0.0f,    // 4
     0.25f,  0.25f, -0.25f,     0.0f, 1.0f, 0.0f,    // 3
    -0.25f,  0.25f,  0.25f,     0.0f, 1.0f, 0.0f,    // 7

    -0.25f,  0.25f, -0.25f,     0.0f, 1.0f, 0.0f,     // 0
    -0.25f,  0.25f,  0.25f,     0.0f, 1.0f, 0.0f,    // 7
     0.25f,  0.25f, -0.25f,     0.0f, 1.0f, 0.0f,    // 3
  };

  // Tetraedro to be rendered
  // Cube to be rendered
  //
  //           3
  //
  //           1   
  //     
  //        6       5
  //
  const GLfloat vertex_positions_tetraedro[] = {

    //positions                   //Normals
    0.0f,  0.25f, 0.0f,        0.0f, -1.0f, 0.0f,    // 3
    0.0f,  -0.25f,  0.5f,      0.0f, -1.0f, 0.0f,    // 6
    -0.25f, -0.25f, 0.25f,     0.0f, -1.0f, 0.0f,    // 1

    0.0f,  0.25f, 0.0f,        0.0f, -1.0f, 0.0f,    // 3
    -0.25f, -0.25f, 0.25f,     0.0f, -1.0f, 0.0f,    // 1
    0.25f, -0.25f,  0.25f,      0.0f, -1.0f, 0.0f,   // 5

    0.0f,  0.25f, 0.0f,        0.0f, -1.0f, 0.0f,    // 3
    0.0f,  -0.25f,  0.5f,      0.0f, -1.0f, 0.0f,    // 6
    0.25f, -0.25f,  0.25f,     0.0f, -1.0f, 0.0f,    // 5

    0.0f,  -0.25f,  0.5f,         0.0f, -1.0f, 0.0f,   // 6
    -0.25f, -0.25f, 0.25f,     0.0f, -1.0f, 0.0f,    // 1
    0.25f, -0.25f,  0.25f,     0.0f, -1.0f, 0.0f,    // 5

  };

  // Vertex Buffer Object (for vertex coordinates)
  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);

  // Vertex attributes
  // 0: vertex position (x, y, z)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // 1: vertex normals (x, y, z)
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

  // Unbind vbo (it was conveniently registered by VertexAttribPointer)
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Unbind vao
  glBindVertexArray(0);

// Crear y vincular el Vertex Array Object (VAO) para el tetraedro
GLuint vao2 = 0;
glGenVertexArrays(1, &vao2);

// Crear y vincular el Vertex Buffer Object (VBO) para el tetraedro
GLuint vbo2 = 0;
glGenBuffers(1, &vbo2);
glBindBuffer(GL_ARRAY_BUFFER, vbo2);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions_tetraedro), vertex_positions_tetraedro, GL_STATIC_DRAW);

// Especificar los atributos de vértice para el tetraedro
// 0: posición del vértice (x, y, z)
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

// 1: normales de vértice (x, y, z)
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);

// Desvincular el VBO y el VAO
glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindVertexArray(0);

// ...

// Para renderizar el tetraedro

  // Uniforms
  // - Model matrix
  // - View matrix
  // - Projection matrix
  // - Normal matrix: normal vectors from local to world coordinates
  // - Camera position
  // - Light data
  // - Material data
  model_location = glGetUniformLocation(shader_program, "model");
  view_location = glGetUniformLocation(shader_program, "view");
  proj_location = glGetUniformLocation(shader_program, "projection");
  normal_location = glGetUniformLocation(shader_program, "normal_matrix");


  light_position_location = glGetUniformLocation(shader_program, "light.position");
  light_ambient_location = glGetUniformLocation(shader_program, "light.ambient");
  light_diffuse_location = glGetUniformLocation(shader_program, "light.diffuse");
  light_specular_location = glGetUniformLocation(shader_program, "light.specular");


  material_shininess_location = glGetUniformLocation(shader_program, "material.shininess");
  material_ambient_location = glGetUniformLocation(shader_program, "material.ambient");
  material_diffuse_location = glGetUniformLocation(shader_program, "material.diffuse");
  material_specular_location = glGetUniformLocation(shader_program, "material.specular");


 // Tetraedro:
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions_tetraedro), vertex_positions_tetraedro, GL_STATIC_DRAW);

  light_position_location2 = glGetUniformLocation(shader_program, "light2.position");
  light_ambient_location2 = glGetUniformLocation(shader_program, "light2.ambient");
  light_diffuse_location2 = glGetUniformLocation(shader_program, "light2.diffuse");
  light_specular_location2 = glGetUniformLocation(shader_program, "light2.specular");

  camera_position_location = glGetUniformLocation(shader_program, "view_pos");

// Render loop
  while(!glfwWindowShouldClose(window)) {

    processInput(window);

    render(glfwGetTime());

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}

void render(double currentTime) {
  float f = (float)currentTime * 0.3f;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, gl_width, gl_height);

  glUseProgram(shader_program);
  glBindVertexArray(vao);

  glm::mat4 model_matrix, view_matrix, proj_matrix;

  glm::mat3 normal_matrix;

  view_matrix = glm::lookAt(                 camera_pos,  // pos
                            glm::vec3(0.0f, 0.0f, 0.0f),  // target
                            glm::vec3(0.0f, 1.0f, 0.0f)); // up

  // Moving cube
  // model_matrix = glm::rotate(model_matrix,
  //   [...]
  model_matrix = glm::mat4(1.f);
  model_matrix = glm::translate(model_matrix, glm::vec3(.75f, 0.0f, 0.0f));
  //model_matrix = glm::translate(model_matrix,
  //                           glm::vec3(sinf(2.1f * f) * 0.5f,
  //                                     cosf(1.7f * f) * 0.5f,
  //                                     sinf(1.3f * f) * cosf(1.5f * f) * 2.0f));
  model_matrix = glm::rotate(model_matrix,
                      glm::radians((float)currentTime * 20.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));
  model_matrix = glm::rotate(model_matrix,
                      glm::radians((float)currentTime * 40.0f),
                      glm::vec3(1.0f, 0.0f, 0.0f));

  //glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model_matrix));

  //glUniformMatrix4fv(model_location, 1, GL_FALSE, &model_matrix[0][0]);

  // Projection
  // proj_matrix = glm::perspective(glm::radians(50.0f),
  //   [...]
  proj_matrix = glm::perspective(glm::radians(50.0f),
                                 (float) gl_width / (float) gl_height,
                                 0.1f, 1000.0f);


  // Normal matrix: normal vectors to world coordinates
  normal_matrix = glm::transpose(glm::inverse(glm::mat3(model_matrix)));

  //glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));
  
 
  glUniform3f(light_ambient_location, light_ambient.x, light_ambient.y, light_ambient.z);
  glUniform3f(light_position_location, light_pos.x, light_pos.y, light_pos.z);
  glUniform3f(light_diffuse_location, light_diffuse.x, light_diffuse.y, light_diffuse.z);
  glUniform3f(light_specular_location, light_specular.x, light_specular.y, light_specular.z);

  glUniform3f(light_ambient_location2, light_ambient.x, light_ambient.y, light_ambient.z);
  glUniform3f(light_position_location2, light_pos2.x, light_pos2.y, light_pos2.z);
  glUniform3f(light_diffuse_location2, light_diffuse.x, light_diffuse.y, light_diffuse.z);
  glUniform3f(light_specular_location2, light_specular.x, light_specular.y, light_specular.z);

  glUniform1f(material_shininess_location, material_shininess);
  glUniform3f(material_ambient_location, material_ambient.x, material_ambient.y, material_ambient.z);
  glUniform3f(material_diffuse_location, material_diffuse.x, material_diffuse.y, material_diffuse.z);
  glUniform3f(material_specular_location, material_specular.x, material_specular.y, material_specular.z);

  glUniform3f(camera_position_location, camera_pos.x, camera_pos.y, camera_pos.z);


  glUniformMatrix4fv(model_location, 1, GL_FALSE, &model_matrix[0][0]);
  glUniformMatrix4fv(view_location, 1, GL_FALSE, &view_matrix[0][0]);
  glUniformMatrix4fv(proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

  glUniformMatrix4fv(normal_location, 1, GL_FALSE, &normal_matrix[0][0]);

  glDrawArrays(GL_TRIANGLES, 0, 36);

  // tetraedro

  model_matrix = glm::mat4(1.f);
  model_matrix = glm::translate(model_matrix, glm::vec3(-.75f, 0.0f, 0.0f));

  proj_matrix = glm::perspective(glm::radians(50.0f),
                                 (float) gl_width / (float) gl_height,
                                 0.1f, 1000.0f);

  model_matrix = glm::rotate(model_matrix,
                      glm::radians((float)currentTime * 20.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));
  model_matrix = glm::rotate(model_matrix,
                      glm::radians((float)currentTime * 40.0f),
                      glm::vec3(1.0f, 0.0f, 0.0f));
  // Normal matrix: normal vectors to world coordinates
  normal_matrix = glm::transpose(glm::inverse(glm::mat3(model_matrix)));

  glUniform1f(material_shininess_location, material_shininess);

  glUniformMatrix4fv(model_location, 1, GL_FALSE, &model_matrix[0][0]);
  glUniformMatrix4fv(view_location, 1, GL_FALSE, &view_matrix[0][0]);
  glUniformMatrix4fv(proj_location, 1, GL_FALSE, &proj_matrix[0][0]);
  glUniformMatrix4fv(normal_location, 1, GL_FALSE, &normal_matrix[0][0]);

  glBindVertexArray(vao2);
  glDrawArrays(GL_TRIANGLES, 0, 12);
}

void processInput(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);
}

// Callback function to track window size and update viewport
void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
  gl_width = width;
  gl_height = height;
  printf("New viewport: (width: %d, height: %d)\n", width, height);
}

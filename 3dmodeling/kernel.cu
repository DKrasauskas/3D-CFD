
//generic c/c++ libs
#include <stdio.h>
#include <vector>
#include <fstream>
#include <iostream>
//opengl / glm
#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
//cuda
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cuda_fp16.h>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <thrust/random.h>
#include <thrust/remove.h>
#include <thrust/gather.h>
#include <thrust/iterator/zip_iterator.h>
#include <cuda_gl_interop.h>
//code-specific libraries
#include "camera/transformations.h"
#include "camera/camera.h"
#include "camera/Features.h"
#include "settings/settings.h"
#include "shaders/Shader.h"
#include "shaders/Buffer.h"
#include "settings/controls.h"
#include "lbm/meshSetup.h"
#include "inputGeometry/geometry.h"
#include "render/renderSetup.h"
#include "render/visualize.h"
#include "lbm/meshSetup.h"


int main()
{
    // glfw window creation
    window = createWindow();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 400.0f);
    camera = Features::GetCamera();
    //shaders
    Shader vertex("shaders/vertex.glsl", "shaders/fragment.glsl");
    Shader vertex2("shaders/vertex2.glsl", "shaders/fragment.glsl");
    //geometry
    VertexList v = read_verticesV(path);
    Buffer buff(v.vertices, nullptr, 4 * v.size, 1);
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, NX*NY*NZ*16, nullptr, GL_DYNAMIC_DRAW);
    // Specify the vertex attribute layout
    // current vertex array stores 3 vec3s for vertices and 1 float for color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * 4));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    cudaGraphicsResource* cuda_vbo_resource;
    cudaGraphicsGLRegisterBuffer(&cuda_vbo_resource, vbo, cudaGraphicsMapFlagsWriteDiscard);
    size_t num_bytes;
    cudaGraphicsMapResources(1, &cuda_vbo_resource, 0);
    cudaGraphicsResourceGetMappedPointer((void**)&dptr, &num_bytes, cuda_vbo_resource);
    //initialize marching cubes for rendering
    mcr::begin();
    __half* sc = (__half*)calloc((NX + 1) * (NY + 1) * (NZ + 1), sizeof(__half));
    cudaMalloc(&sc_field, sizeof(__half) * (NX + 1) * (NY + 1) * (NZ + 1));
    cudaMemcpy(sc_field, sc, sizeof(__half) * (NX + 1) * (NY + 1) * (NZ + 1), cudaMemcpyHostToDevice);
   // mcr::marchCubes(sc_field, dptr);
    cudaGraphicsUnmapResources(1, &cuda_vbo_resource, 0);
    free(sc);
    //initialize LBM cfd 
    lbm::begin(v.vertices, v.size);  
    while (!glfwWindowShouldClose(window)){
        //generic camera calculations for translation/rotation
        glm::mat4 inverse = glm::mat4(1);
        inverse[0] = camera.camera_basis.basis[0];
        inverse[1] = camera.camera_basis.basis[1];
        inverse[2] = camera.camera_basis.basis[2];
        inverse[3] = glm::vec4(0, 0, 0, 1);
        inverse = glm::transpose(inverse);
        if (launched) {    
            cudaGraphicsMapResources(1, &cuda_vbo_resource, 0); // Map the VBO resource
            cudaGraphicsResourceGetMappedPointer((void**)&dptr, &num_bytes, cuda_vbo_resource);
            compute();//perform LBM calculations
            mcr::marchCubes(sc_field, dptr); //render the scalar field
            cudaGraphicsUnmapResources(1, &cuda_vbo_resource, 0);          
            processInput(window);
            glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glUseProgram(vertex.ID);
            glBindVertexArray(buff.VAO);
            glUniformMatrix4fv(glGetUniformLocation(vertex2.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
            glUniform3fv(glGetUniformLocation(vertex2.ID, "camera_global"), 1, glm::value_ptr(glm::vec3(camera.camera_basis.basis[3].x, camera.camera_basis.basis[3].y, camera.camera_basis.basis[3].z)));
            glUniformMatrix4fv(glGetUniformLocation(vertex2.ID, "view_direction"), 1, GL_FALSE, glm::value_ptr(inverse));
            glDrawArrays(GL_TRIANGLES, 0, 4 * v.size);   //render the mesh        
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glUseProgram(vertex2.ID);
            glBindVertexArray(vao);
            glUniformMatrix4fv(glGetUniformLocation(vertex2.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
            glUniform3fv(glGetUniformLocation(vertex2.ID, "camera_global"), 1, glm::value_ptr(glm::vec3(camera.camera_basis.basis[3].x, camera.camera_basis.basis[3].y, camera.camera_basis.basis[3].z)));
            glUniformMatrix4fv(glGetUniformLocation(vertex2.ID, "view_direction"), 1, GL_FALSE, glm::value_ptr(inverse));
            glDrawArrays(GL_TRIANGLES, 0, (*ntriangle_count)); //render the field
            //read to file if desired
            if (COMPUTATION_MODE) {
                glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, image);
                fwrite(image, 1, SCR_WIDTH * SCR_HEIGHT * 4, ffmpeg);
            }                                   
        }
        else {
            processInput(window);        
            glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(vertex.ID);
            glBindVertexArray(buff.VAO);
            glUniformMatrix4fv(glGetUniformLocation(vertex2.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
            glUniform3fv(glGetUniformLocation(vertex2.ID, "camera_global"), 1, glm::value_ptr(glm::vec3(camera.camera_basis.basis[3].x, camera.camera_basis.basis[3].y, camera.camera_basis.basis[3].z)));
            glUniformMatrix4fv(glGetUniformLocation(vertex2.ID, "view_direction"), 1, GL_FALSE, glm::value_ptr(inverse));
            glDrawArrays(GL_TRIANGLES, 0, 4 * v.size);
        }      
        glfwPollEvents();
        HandleSettings(window, camera);
        PollControls(camera, window);
        glfwSwapBuffers(window);          
    }
    _pclose(ffmpeg);
    lbm::__terminate__();
    mcr::__terminate__();
    return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


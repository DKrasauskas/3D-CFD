#pragma once

#include <iostream>
#include <string>
#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"
#include "Shader.h"

#define uint unsigned int

class Buffer {
public:
	uint VAO, VBO, EBO;
	Buffer(void* vertex_memory, void* indice_memory, int vertex_size, int indice_size);
	~Buffer();
};
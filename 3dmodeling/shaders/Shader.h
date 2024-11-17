#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include<sstream>
#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"



using namespace std;

class Shader {
public:
    unsigned int ID;
    Shader(const char* inputVertex, const char* inputFragment);
    Shader() = default;
    ~Shader();
    char* Parse(string input);
    void BuildShaders(unsigned int& shader, const char* source, uint32_t shader_type);
    void Link(unsigned int& program);
};
class Cshader {
public:
    unsigned int ID;
    Cshader(const char* inputCompute);
    ~Cshader();
    char* Parse(string input);
    void BuildShaders(unsigned int& shader, const char* source, uint32_t shader_type);
    void Link(unsigned int& program);
};
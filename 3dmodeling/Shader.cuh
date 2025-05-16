#include "shaders/Shader.h"

Shader::Shader(const char* inputVertex, const char* inputFragment) {
    unsigned int vertex, fragment;
    char* source = Shader::Parse(inputVertex);
    char* source2 = Shader::Parse(inputFragment);
    BuildShaders(vertex, source, GL_VERTEX_SHADER);
    BuildShaders(fragment, source2, GL_FRAGMENT_SHADER);
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    Link(ID);
    free(source);
    free(source2);
}
Shader::~Shader() {
    glDeleteProgram(ID);
}

char* Shader::Parse(string input) {
    cout << input << "\n";
    ifstream in(input);
    !in.is_open() ? (throw std::invalid_argument("ERR: FILE NOT FOUND")) : NULL;
    std::stringstream buffer;
    buffer << in.rdbuf();
    string s = buffer.str();
    char* out = (char*)malloc(sizeof(char) * (s.length() + 1));
    out == NULL ? throw "malloc failed" : 0;
    for (int i = 0; i < s.length(); i++) {
        out[i] = s[i];
    }
    out[s.length()] = '\0';
    return out;
}
void Shader::BuildShaders(unsigned int& shader, const char* source, uint32_t shader_type) {
    shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, NULL);
    int success;
    char err[512];
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, err);
        std::cout << "shader Compilation failed " << ((shader_type == GL_VERTEX_SHADER) ? "GL_VERTEX_SHADER " : "GL_FRAGMENT_SHADER ") << err << "\n";
        throw;
    }
}
void Shader::Link(unsigned int& program) {
    glLinkProgram(program);
    int success;
    char err[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, err);
        std::cout << "Linking failed" << err << "\n";
        throw;
    }
}

Cshader::Cshader(const char* inputCompute) {
    unsigned int compute;
    char* source = Cshader::Parse(inputCompute);   
    BuildShaders(compute, source, GL_COMPUTE_SHADER);
    ID = glCreateProgram();
    glAttachShader(ID, compute);
    Link(ID);
    free(source);
}
Cshader::~Cshader() {
    glDeleteProgram(ID);
}

char* Cshader::Parse(string input) {
    cout << input << "\n";
    ifstream in(input);
    !in.is_open() ? (throw std::invalid_argument("ERR: FILE NOT FOUND")) : NULL;
    std::stringstream buffer;
    buffer << in.rdbuf();
    string s = buffer.str();
    char* out = (char*)malloc(sizeof(char) * (s.length() + 1));
    out == NULL ? throw "malloc failed" : 0;
    for (int i = 0; i < s.length(); i++) {
        out[i] = s[i];
    }
    out[s.length()] = '\0';
    return out;
}
void Cshader::BuildShaders(unsigned int& shader, const char* source, uint32_t shader_type) {
    shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, NULL);
    int success;
    char err[512];
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, err);
        std::cout << "shader Compilation failed " << ((shader_type == GL_VERTEX_SHADER) ? "GL_VERTEX_SHADER " : "GL_FRAGMENT_SHADER ") << err << "\n";
        throw;
    }
}
void Cshader::Link(unsigned int& program) {
    glLinkProgram(program);
    int success;
    char err[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, err);
        std::cout << "Linking failed" << err << "\n";
        throw;
    }
}

#pragma once
namespace Features {
    struct View {
        glm::mat4 perspective;
        glm::mat4 view;
        glm::mat4 projection;
    };
    struct free_delete
    {
        void operator()(void* x) { free(x); }
    };
    //Initializes Camera with predefined settings
    Camera GetCamera() {
        return { glm::vec4(0, 0, -1, 0) };
    }
    //initializes selected items  
    //Creates view and perspective matrices
    View CreateViewModel() {
        glm::mat4 view = glm::mat4(1.0f);
        view[3][2] = -0.0f;
        return { glm::mat4(1.0f), view,  glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 20.0f) };
    }
}
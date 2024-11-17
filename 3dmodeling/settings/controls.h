#pragma once

float rot = 0.001f;
float times = 0;
int warp_speed = 10000;
bool state = false;
bool launched = false;
int launch = -1;
float map = 500.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

GLFWwindow* createWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* win = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Orbits", NULL, NULL);
    win == NULL ? throw std::exception() : NULL;
    glfwMakeContextCurrent(win);
    !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) ? throw std::exception() : NULL;
    return win;
}

void HandleSettings(GLFWwindow* window, Camera& camera) {
    float tr = 0.01f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.Translate(camera.camera_basis.basis[2] * tr);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.Translate(camera.camera_basis.basis[2] * (-tr));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.Translate(camera.camera_basis.basis[0] * -tr);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.Translate(camera.camera_basis.basis[0] * (tr));
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glm::vec4 vec = glm::vec4(camera.camera_basis.basis[2]);
        vec.w = 0.01;
        camera.Rotate(vec);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        glm::vec4 vec = glm::vec4(camera.camera_basis.basis[2]);
        vec.w = -0.01;
        camera.Rotate(vec);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera.Translate(camera.camera_basis.basis[1] * (-tr));
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.Translate(camera.camera_basis.basis[1] * (tr));
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        map += 10.0f;
        *HMP = map;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        map -= 10.0f;
        *HMP = map;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        launched = true;
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        launched = false;
    }
}

void PollControls(Camera& cam, GLFWwindow* window) {
    float sensitivity = .5;
    double xpos, ypos;
    float angle;
    glfwGetCursorPos(window, &xpos, &ypos);
    angle = (ypos - 500) / 1940 * sensitivity;
    glm::vec4 axisT = cam.camera_basis.basis * glm::vec4(1, 0, 0, 0);
    axisT.w = angle;
    cam.Rotate(glm::vec4(axisT));
    angle = (xpos - 500) / 1940 * sensitivity;
    glm::vec4 axisB = cam.camera_basis.basis * glm::vec4(0, 1, 0, 0);
    axisB.w = angle;
    cam.Rotate(axisB);
    glfwSetCursorPos(window, 500, 500);
}

void processInput(GLFWwindow* window)
{
    float tr = 0.01f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        state = 1;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        state = 0;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && launch != true)
        launch = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && launch != true)
        launch = 2;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && launch != true)
        launch = 3;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && launch != true)
        launch = 4;
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && launch != true)
        launch = 5;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && launch != true)
        warp_speed += 10;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && launch != true)
        warp_speed -= 10;
}
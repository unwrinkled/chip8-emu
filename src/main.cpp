// #include <glad/gl.h>

// #include <GLFW/glfw3.h>

#include <fstream>
#include <iomanip>
#include <iostream>

#include "common.h"
#include "memory.h"

// #define WIDTH 800
// #define HEIGHT 600

// void glfw_error_callback(int, const char *desc) {
//     std::cerr << "GLFW Error: " << desc << std::endl;
// }

// void glfw_framebuffer_size_callback(GLFWwindow *, int width, int height) {
//     glViewport(0, 0, width, height);
// }
int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "USAGE: chip8 filename" << std::endl;
        return -1;
    }

    Memory memory;
    if (!memory.load_rom(argv[1])) {
        std::cerr << "Can't read whole ROM file to memory. Exiting..." << std::endl;
        return -1;
    }
    memory.load_font_default();

    u16 pc = 512;
    Opcode opcode = memory.fetch(pc);
    opcode.print();

    FontCharacter character = memory.get_font_char(0x0a);
    character.print();

    // u8 screen[64][32];

    // Initializing graphics
    // GLFWwindow *window;

    // glfwSetErrorCallback(glfw_error_callback);
    // if (!glfwInit()) {
    //     return -1;
    // }

    // window = glfwCreateWindow(WIDTH, HEIGHT, "CHIP-8 Interpreter", nullptr, nullptr);
    // if (!window) {
    //     glfwTerminate();
    //     return -1;
    // }

    // glfwMakeContextCurrent(window);
    // int version = gladLoadGL(glfwGetProcAddress);
    // if (version == 0) {
    //     std::cerr << "Failed to initialize OpenGL context" << std::endl;
    //     return -1;
    // }
    // std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;
    // glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);

    // glClearColor(0.0, 0.0, 0.0, 1.0);
    // while (!glfwWindowShouldClose(window)) {
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //     glfwSwapBuffers(window);
    //     glfwPollEvents();
    // }

    // glfwTerminate();
    return 0;
}
#include <chip8/display.h>

#include <gsl/gsl_assert>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

void terminate_glfw(GLFWwindow *window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

Display::Display() : m_window(nullptr, terminate_glfw), m_shader(0), m_pixel_vao(0), m_vbo(0), m_ebo(0), m_model_loc(0) {
    if (!glfwInit()) {
        throw std::runtime_error("There was a problem with glfw");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = GLFWwindow_smart(glfwCreateWindow(window_width, window_height, "CHIP8 Emulator", nullptr, nullptr), terminate_glfw);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(m_window.get());
    int version = gladLoadGL(glfwGetProcAddress);
    std::cout << "OpenGL Version: " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << "\n";

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(Display::message_callback, nullptr);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    load_shaders();
    create_pixel_vao();

    glUseProgram(m_shader);
    m_model_loc = glGetUniformLocation(m_shader, "model");

    glBindVertexArray(m_pixel_vao);
}

Display::~Display() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_pixel_vao);
    glDeleteProgram(m_shader);
}

void Display::load_shaders() {
    auto shaders_path = fs::path("shaders");

    std::ifstream vertex_file(shaders_path / "vertex.vert");
    std::stringstream vertex_stream;
    vertex_stream << vertex_file.rdbuf();
    std::string v_str = vertex_stream.str();
    const char *vertex_source = v_str.c_str();

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
    glCompileShader(vertex_shader);

    int success = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        constexpr size_t log_size = 512;
        std::string info_log;
        info_log.reserve(log_size);
        glGetShaderInfoLog(vertex_shader, log_size, nullptr, info_log.data());
        // std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
        //           << info_log << std::endl;
        throw std::runtime_error(std::format("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{}", info_log));
    }

    std::ifstream fragment_file(shaders_path / "fragment.frag");
    std::stringstream fragment_stream;
    fragment_stream << fragment_file.rdbuf();
    std::string f_str = fragment_stream.str();
    const char *fragment_source = f_str.c_str();

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        constexpr size_t log_size = 512;
        std::string info_log;
        info_log.reserve(log_size);
        glGetShaderInfoLog(fragment_shader, log_size, nullptr, info_log.data());
        // std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
        //           << info_log << std::endl;
        throw std::runtime_error(std::format("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{}", info_log));
    }

    m_shader = glCreateProgram();
    glAttachShader(m_shader, vertex_shader);
    glAttachShader(m_shader, fragment_shader);
    glLinkProgram(m_shader);

    glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
    if (!success) {
        constexpr size_t log_size = 512;
        std::string info_log;
        info_log.reserve(log_size);
        glGetProgramInfoLog(m_shader, log_size, nullptr, info_log.data());
        // std::cerr << "ERROR::SHADER::LINK_FAILED\n"
        //           << info_log << std::endl;
        throw std::runtime_error(std::format("ERROR::SHADER::LINK_FAILED\n{}", info_log));
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Display::create_pixel_vao() noexcept {
    const auto cube_vertices = std::array<float, 12>{
        1.0f, -1.0f, 0.0f,  // bottom right
        1.0f, 1.0f, 0.0f,   // top right
        -1.0f, -1.0f, 0.0f, // bottom left
        -1.0f, 1.0f, 0.0f   // top left
    };

    const auto cube_indices = std::array<unsigned int, 6>{
        0, 1, 2,
        2, 1, 3
    };

    glGenVertexArrays(1, &m_pixel_vao);
    glBindVertexArray(m_pixel_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, cube_vertices.size() * sizeof(cube_vertices.data()), cube_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube_indices.size() * sizeof(cube_indices.data()), cube_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Display::clear_screen() const noexcept {
    Expects(m_window);
    glClear(GL_COLOR_BUFFER_BIT);
}
// -1 + w * 0.5 + x * w = -1 + w(x + 0.5)
void Display::draw_pixel(const u16 x, const u16 y) const noexcept {
    auto pos_x = -1.0f + pixel_width * (static_cast<float>(x) * 2 + 1);
    auto pos_y = 1.0f - pixel_height * (static_cast<float>(y) * 2 + 1);
    auto model = glm::translate(glm::mat4(1.0f), glm::vec3(pos_x, pos_y, 0.0f));
    model = glm::scale(model, glm::vec3(pixel_width, pixel_height, 0.0f));
    glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, m_num_of_indices, GL_UNSIGNED_INT, nullptr);
}

bool Display::should_close() const noexcept {
    Expects(m_window);
    return static_cast<bool>(glfwWindowShouldClose(m_window.get()));
}

void Display::swap_buffers() const noexcept {
    Expects(m_window);
    glfwSwapBuffers(m_window.get());
}

void Display::poll_events() const noexcept {
    Expects(m_window);
    glfwPollEvents();
}

void Display::message_callback([[maybe_unused]] GLenum source, GLenum type,
                               [[maybe_unused]] GLuint id,
                               GLenum severity, [[maybe_unused]] GLsizei length,
                               const GLchar *msg, [[maybe_unused]] const void *user_param) noexcept {
    std::cerr << std::format("GL CALLBACK: {} type = {:#x}, severity = {:#x}, message = {}\n",
                             (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                             type, severity, msg);
}
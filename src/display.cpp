#include "display.h"

#include <GLFW/glfw3.h>
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
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

namespace {
enum class Chip8Key : u8 {
    Key0 = 0x0,
    Key1 = 0x1,
    Key2 = 0x2,
    Key3 = 0x3,
    Key4 = 0x4,
    Key5 = 0x5,
    Key6 = 0x6,
    Key7 = 0x7,
    Key8 = 0x8,
    Key9 = 0x9,
    KeyA = 0xa,
    KeyB = 0xb,
    KeyC = 0xc,
    KeyD = 0xd,
    KeyE = 0xe,
    KeyF = 0xf
};

void process_keys(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
    static std::map<int, Chip8Key> key_map{
        { GLFW_KEY_1, Chip8Key::Key1 },
        { GLFW_KEY_2, Chip8Key::Key2 },
        { GLFW_KEY_3, Chip8Key::Key3 },
        { GLFW_KEY_4, Chip8Key::KeyC },
        { GLFW_KEY_Q, Chip8Key::Key4 },
        { GLFW_KEY_W, Chip8Key::Key5 },
        { GLFW_KEY_E, Chip8Key::Key6 },
        { GLFW_KEY_R, Chip8Key::KeyD },
        { GLFW_KEY_A, Chip8Key::Key7 },
        { GLFW_KEY_S, Chip8Key::Key8 },
        { GLFW_KEY_D, Chip8Key::Key9 },
        { GLFW_KEY_F, Chip8Key::KeyE },
        { GLFW_KEY_Z, Chip8Key::KeyA },
        { GLFW_KEY_X, Chip8Key::Key0 },
        { GLFW_KEY_C, Chip8Key::KeyB },
        { GLFW_KEY_V, Chip8Key::KeyF }
    };

    auto &display = *static_cast<Display *>(glfwGetWindowUserPointer(window));

    if (key_map.contains(key) && (action == GLFW_PRESS || action == GLFW_RELEASE)) {
        // if (action == GLFW_PRESS) {
        //     std::cout << std::format("PRESSED {:#x}\n", static_cast<u8>(key_map.at(key)));
        // } else if (action == GLFW_RELEASE) {
        //     std::cout << std::format("RELEASED {:#x}\n", static_cast<u8>(key_map.at(key)));
        // }

        display.toggle_key(static_cast<u8>(key_map.at(key)));
    } else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void terminate_glfw(GLFWwindow *window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void message_callback([[maybe_unused]] GLenum source, GLenum type,
                      [[maybe_unused]] GLuint id,
                      GLenum severity, [[maybe_unused]] GLsizei length,
                      const GLchar *msg, [[maybe_unused]] const void *user_param) noexcept {
    std::cerr << std::format("GL CALLBACK: {} type = {:#x}, severity = {:#x}, message = {}\n",
                             (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                             type, severity, msg);
}
} // namespace

Display::Display() : m_window(nullptr, terminate_glfw), m_shader(0), m_pixel_vao(0), m_vbo(0), m_ebo(0), m_model_loc(0), m_color_loc(0), m_display{ { { 0 } } }, m_keys_pressed{ false } {
    if (!glfwInit()) {
        throw std::runtime_error("There was a problem with glfw");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);

    m_window = GLFWwindow_smart(glfwCreateWindow(window_width, window_height, "CHIP8 Emulator", nullptr, nullptr), terminate_glfw);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(m_window.get());
    int version = gladLoadGL(glfwGetProcAddress);
    std::cout << std::format("OpenGL Version: {}.{}\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    glfwSetWindowUserPointer(m_window.get(), this);
    glfwSetKeyCallback(m_window.get(), process_keys);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, nullptr);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    load_shaders();
    create_pixel_vao();

    glUseProgram(m_shader);
    m_model_loc = glGetUniformLocation(m_shader, "model");
    m_color_loc = glGetUniformLocation(m_shader, "color");

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

void Display::clear_screen() noexcept {
    Expects(m_window);
    for (auto &row : m_display) {
        row.fill(0);
    }
    glClear(GL_COLOR_BUFFER_BIT);
}

bool Display::draw_sprite(std::vector<u8> &&sprite, u8 x, u8 y) noexcept {
    constexpr u8 byte_in_bits = 8;
    x %= chip8_width;
    y %= chip8_height;

    u8 n = sprite.size();
    u8 current_sprite = 0;
    bool has_flipped = false;
    for (u8 i = 0; i < n && y + i < chip8_height; ++i) {
        current_sprite = sprite.at(i);
        for (u8 j = 0; j < byte_in_bits && x + j < chip8_width; ++j) {
            u8 set = current_sprite >> (byte_in_bits - 1 - j) & 1;
            if (set) {
                if (m_display.at(y + i).at(x + j) == 1) {
                    draw_pixel(x + j, y + i, false);
                    m_display.at(y + i).at(x + j) = 0;
                    has_flipped = true;
                } else {
                    draw_pixel(x + j, y + i, true);
                    m_display.at(y + i).at(x + j) = 1;
                }
            }
        }
    }

    return has_flipped;
}

// -1 + w * 0.5 + x * w = -1 + w(x + 0.5)
void Display::draw_pixel(const u16 x, const u16 y, bool is_colored) const noexcept {
    auto pos_x = -1.0f + pixel_width * (static_cast<float>(x) * 2 + 1);
    auto pos_y = 1.0f - pixel_height * (static_cast<float>(y) * 2 + 1);
    auto model = glm::translate(glm::mat4(1.0f), glm::vec3(pos_x, pos_y, 0.0f));
    model = glm::scale(model, glm::vec3(pixel_width, pixel_height, 0.0f));
    glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model));

    if (!is_colored) {
        glUniform3fv(m_color_loc, 1, glm::value_ptr(m_black_color));
    } else {
        glUniform3fv(m_color_loc, 1, glm::value_ptr(m_white_color));
    }

    glDrawElements(GL_TRIANGLES, m_num_of_indices, GL_UNSIGNED_INT, nullptr);
}

bool Display::should_close() const noexcept {
    Expects(m_window);
    return static_cast<bool>(glfwWindowShouldClose(m_window.get()));
}

void Display::swap_buffers() const noexcept {
    Expects(m_window);
    // glfwSwapBuffers(m_window.get());
    glFlush();
}

void Display::poll_events() const noexcept {
    Expects(m_window);
    glfwPollEvents();
}

void Display::toggle_key(u8 key) noexcept {
    m_keys_pressed.at(key) = !m_keys_pressed.at(key);
}

bool Display::is_pressed(u8 key) const noexcept {
    return m_keys_pressed.at(key);
}

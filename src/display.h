#ifndef C8_DISPLAY_H
#define C8_DISPLAY_H

#include "common.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>

#include <array>
#include <memory>
#include <vector>

constexpr u8 gl_version_major = 4;
constexpr u8 gl_version_minor = 6;

constexpr u8 chip8_width = 64;
constexpr u8 chip8_height = 32;
constexpr u8 display_scaling = 16;
constexpr u16 window_width = chip8_width * display_scaling;
constexpr u16 window_height = chip8_height * display_scaling;

constexpr float pixel_width = 1.0f / chip8_width;
constexpr float pixel_height = 1.0f / chip8_height;

// void terminate_glfw(GLFWwindow *window);
using GLFWwindow_smart = std::unique_ptr<GLFWwindow, void (*)(GLFWwindow *)>; /* decltype(&terminate_glfw) >;*/

class Display {
  public:
    Display();
    ~Display();

    Display(const Display &) = delete;
    Display operator=(const Display &) = delete;

    Display(Display &&) = delete;
    Display operator=(Display &&) = delete;

    void clear_screen() noexcept;
    [[nodiscard]] bool draw_sprite(std::vector<u8> &&sprite, u8 x, u8 y) noexcept;

    [[nodiscard]] bool should_close() const noexcept;
    void swap_buffers() const noexcept;
    void poll_events() const noexcept;

    void toggle_key(u8 key) noexcept;
    [[nodiscard]] bool is_pressed(u8 key) const noexcept;

  private:
    void load_shaders();
    void create_pixel_vao() noexcept;

    void draw_pixel(u16 x, u16 y, bool is_colored) const noexcept;

    GLFWwindow_smart m_window;
    unsigned int m_shader;
    unsigned int m_pixel_vao;
    unsigned int m_vbo, m_ebo;

    int m_model_loc;
    int m_color_loc;

    std::array<std::array<u8, chip8_width>, chip8_height> m_display;

    static constexpr u8 m_num_of_keys = 16;
    std::array<bool, m_num_of_keys> m_keys_pressed;

    static constexpr GLsizei m_num_of_indices = 6;
    static constexpr glm::vec3 m_black_color = { 0.0f, 0.0f, 0.0f };
    static constexpr glm::vec3 m_white_color = { 1.0f, 1.0f, 1.0f };
};

#endif

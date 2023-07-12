#ifndef C8_DISPLAY_H
#define C8_DISPLAY_H

#include <chip8/common.h>

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <memory>

constexpr u8 gl_version_major = 4;
constexpr u8 gl_version_minor = 6;

constexpr u8 chip8_width = 64;
constexpr u8 chip8_height = 32;
constexpr u8 display_scaling = 16;
constexpr u16 window_width = chip8_width * display_scaling;
constexpr u16 window_height = chip8_height * display_scaling;

constexpr float pixel_width = 1.0f / chip8_width;
constexpr float pixel_height = 1.0f / chip8_height;

void terminate_glfw(GLFWwindow *window);
using GLFWwindow_smart = std::unique_ptr<GLFWwindow, decltype(&terminate_glfw)>;

class Display {
  public:
    Display();
    ~Display();

    Display(const Display &) = delete;
    Display operator=(const Display &) = delete;

    Display(Display &&) = delete;
    Display operator=(Display &&) = delete;

    void clear_screen() const noexcept;
    void draw_pixel(u16 x, u16 y) const noexcept;

    [[nodiscard]] bool should_close() const noexcept;
    void swap_buffers() const noexcept;
    void poll_events() const noexcept;

  private:
    static void message_callback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *, const void *) noexcept;

    void load_shaders();
    void create_pixel_vao() noexcept;

    GLFWwindow_smart m_window;
    unsigned int m_shader;
    unsigned int m_pixel_vao;
    unsigned int m_vbo, m_ebo;

    int m_model_loc;

    static constexpr GLsizei m_num_of_indices = 6;
};

#endif

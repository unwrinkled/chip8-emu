#include <chip8/display.h>

#include <gsl/gsl>

#include <iostream>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "USAGE: chip8 filename" << std::endl;
        return -1;
    }

    [[maybe_unused]] auto args = gsl::make_span(argv, argc);

    try {
        Display display;

        while (!display.should_close()) {
            display.clear_screen();

            for (auto x = 0; x < chip8_width; ++x) {
                for (auto y = 0; y < chip8_height; ++y) {
                    if ((x + y) % 2 == 0) {
                        display.draw_pixel(x, y);
                    }
                }
            }

            display.swap_buffers();
            display.poll_events();
        }

    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
    }

    // while (!glfwWindowShouldClose(window)) {
    //     glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT);

    //     glUseProgram(shader);
    //     auto model_loc = glGetUniformLocation(shader, "model");
    //     glBindVertexArray(vao);
    //     glm::mat4 model;
    //     for (auto i = 0; i < chip8_width; ++i) {
    //         for (auto j = 0; j < chip8_height; ++j) {
    //             if ((i + j) % 2 == 0) {
    //                 float pixel_pos_x = -1.0f + pixel_width * (static_cast<float>(i) * 2 + 1);
    //                 float pixel_pos_y = 1.0f - pixel_height * (static_cast<float>(j) * 2 + 1);
    //                 model = glm::translate(glm::mat4(1.0f), glm::vec3(pixel_pos_x, pixel_pos_y, 0.0f));
    //                 model = glm::scale(model, glm::vec3(pixel_width, pixel_height, 0.0f));
    //                 glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    //                 glDrawElements(GL_TRIANGLES, cube_indices.size(), GL_UNSIGNED_INT, nullptr);
    //             }
    //         }
    //     }

    //     glfwSwapBuffers(window);
    //     glfwPollEvents();
    // }

    return 0;
}
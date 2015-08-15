#include <iostream>
#include <GLFW/glfw3.h>

#include "chip.h"
#include "renderer.h"

#define WIN_WIDTH 640
#define WIN_HEIGHT 320

int main(int argc, char* argv[]) {
    Chip chip;
    Renderer renderer;

    chip.Initialize();
    if (0 != chip.LoadGame("./roms/PONG")) {
        std::cerr << "Failed to load roms" << std::endl;
        exit(1);
    } else {
        std::cout << "Game PONG" << std::endl;
    }

    renderer.Setup();

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Chip8 vm", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetErrorCallback([] (int error, const char* desc) {
            std::cerr << "Glfw error: " << desc << std::endl;
    });

    glfwSetKeyCallback(
        window, 
        [&chip](GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
            return;
        }

        if (action == GLFW_PRESS) {
        }

        if (action == GLFW_RELEASE) {
        }
    });

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        chip.EmulateCycle();
        /* Render here */
        renderer.Draw();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

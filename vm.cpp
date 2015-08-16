#include <iostream>
#include <chrono>
#include <unistd.h>

#include <GLFW/glfw3.h>

#include "chip.h"
#include "renderer.h"

#define WIN_WIDTH 640
#define WIN_HEIGHT 320

#define CPU_SPEED 100 //Hz
#define CYCLE_TIME_MS (1000/CPU_SPEED)
Chip g_chip;
void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }

    if (action == GLFW_PRESS) {
        // left side of keyboard (QWER) is mapped to hex layout:
        // |1|2|3|C|        =>      |1|2|3|4|
        // |4|5|6|D|        =>      |Q|W|E|R|
        // |7|8|9|E|        =>      |A|S|D|F|
        // |A|0|B|F|        =>      |Z|X|C|V|
        switch (key) {
            case  GLFW_KEY_1:
                g_chip.OnKey(0x0001);
                break;
            case GLFW_KEY_2:
                g_chip.OnKey(0x0002);
                break;
            case GLFW_KEY_3:
                g_chip.OnKey(0x0003);
                break;
            case GLFW_KEY_4:
                g_chip.OnKey(0x000C);
                break;
            case GLFW_KEY_Q:
                g_chip.OnKey(0x0004);
                break;
            case GLFW_KEY_W:
                g_chip.OnKey(0x0005);
                break;
            case GLFW_KEY_E:
                g_chip.OnKey(0x0006);
                break;
            case GLFW_KEY_R:
                g_chip.OnKey(0x000D);
                break;
            case GLFW_KEY_A:
                g_chip.OnKey(0x0007);
                break;
            case GLFW_KEY_S:
                g_chip.OnKey(0x0008);
                break;
            case GLFW_KEY_D:
                g_chip.OnKey(0x0009);
                break;
            case GLFW_KEY_F:
                g_chip.OnKey(0x000E);
                break;
            case GLFW_KEY_Z:
                g_chip.OnKey(0x000A);
                break;
            case GLFW_KEY_X:
                g_chip.OnKey(0x0000);
                break;
            case GLFW_KEY_C:
                g_chip.OnKey(0x000B);
                break;
            case GLFW_KEY_V:
                g_chip.OnKey(0x000F);
                break;
            default:
                break;
        }
    }

    if (action == GLFW_RELEASE) {
        g_chip.OnKey(0x00FF);
    }
}

int main(int argc, char* argv[]) {
    Renderer renderer(WIN_WIDTH, WIN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, g_chip);

    g_chip.Initialize();
    if (0 != g_chip.LoadGame("./roms/PONG")) {
        std::cerr << "Failed to load roms" << std::endl;
        exit(1);
    } else {
        std::cout << "Game PONG" << std::endl;
    }

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Chip8 vm", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    glfwSetErrorCallback([] (int error, const char* desc) {
            std::cerr << "Glfw error: " << desc << std::endl;
    });

    glfwSetKeyCallback(window, keyboard_callback);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, WIN_WIDTH, WIN_HEIGHT, 0.0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // clears screen color
    glClearColor(0.f, 0.f, 0.f, 1.f);

    //auto last = std::chrono::system_clock::now();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        g_chip.EmulateCycle();
        /* Render here */
        renderer.Draw(window);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        /*
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = now - last;
        auto elapse_ms = diff.count() * 1000;
        auto sleep_time = (CYCLE_TIME_MS - elapse_ms) * 1000ULL;
        if (sleep_time > 0) {
            usleep(sleep_time);
        }
         
        last  = std::chrono::system_clock::now();
        */
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

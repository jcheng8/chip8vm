#include <GLFW/glfw3.h>
#include "renderer.h"
#include "chip.h"

Renderer::Renderer(int winWidth, int winHeight, int screenWidth, int screenHeight, Chip& chip):
    winWidth_(winWidth),
    winHeight_(winHeight),
    screenWidth_(screenWidth),
    screenHeight_(screenHeight),
    chip_(chip)
{
    scaleX_ = (winWidth_ * 1.0 / screenWidth_);
    scaleY_ = (winHeight_ * 1.0 / screenHeight_);
}

void Renderer::Draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    const VisualMemory& display = chip_.GetDisplay();

    for (int i = 0; i < screenWidth_; ++i) {
        for (int j = 0; j < screenHeight_; ++j) {
            if (display[i][j] == 1) drawSprite(i * scaleX_, j * scaleY_);
        }
    }
}

void Renderer::drawSprite(float x, float y)
{
    glBegin(GL_QUADS);
        glColor3f(0.0f, 1.0f, 1.0f);
        glVertex2f(x, y);
        glVertex2f(x + scaleX_, y);
        glVertex2f(x + scaleX_, y + scaleY_);
        glVertex2f(x, y + scaleY_);
    glEnd();

}

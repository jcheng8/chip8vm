#ifndef chip8vm_renderer_h
#define chip8vm_renderer_h
class GLFWwidnow;
class Chip;

class Renderer {
public:
    Renderer(int winWidth, int winHeight, int screenWidth, int screenHeight, Chip& chip);
    void Draw();
private:
    int winWidth_, winHeight_, screenWidth_, screenHeight_;
    float scaleX_, scaleY_;
    Chip& chip_;
    
    void drawSprite(float x, float y);
};
#endif

#ifndef chip8vm_chip_h
#define chip8vm_chip_h
#include <cstdint>
#include <array>
#include <stack>

#define PIXELS (64*32)
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define MEMORY_OFFSET 0x200
#define FONTSET_SIZE 80

using VisualMemory = std::array<std::array<uint8_t, SCREEN_HEIGHT>, SCREEN_WIDTH>;
 
const uint8_t fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,       // 0
    0x20, 0x60, 0x20, 0x20, 0x70,       // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,       // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,       // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,       // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,       // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,       // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,       // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,       // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,       // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,       // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,       // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,       // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,       // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,       // E
    0xF0, 0x80, 0xF0, 0x80, 0x80        // F
};

class Chip {
private:
    std::array<uint8_t,  4096>   memory_;
    std::array<uint8_t,  16>     V_;
    std::array<uint16_t, 16>     stack_;
    uint8_t                      key_;

    // std::array<uint8_t, PIXELS>  gfx_; 
    uint16_t                     I_;
    uint16_t                     PC_;
    uint16_t                     SP_;

    uint8_t                      delay_timer_;
    uint8_t                      sound_timer_;

    VisualMemory display_;
     
public:
    void Initialize();
    int  LoadGame(const char* filename);
    void EmulateCycle();
    void OnKey(uint8_t key) { key_ = key; }

    const VisualMemory& GetDisplay() const {
        return display_;
    }
private:
    void SkipNextInstruction() { PC_ += 2; }
};
#endif

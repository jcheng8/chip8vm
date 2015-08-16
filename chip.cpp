#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "chip.h"

#define SPRITE_WIDTH 8

void Chip::Initialize() {
    PC_ = MEMORY_OFFSET;
    I_  = 0;
    SP_ = 0;
    
    key_ = 0;
    for (int i = 0; i < FONTSET_SIZE; ++i) {
        memory_[i] = fontset[i];
    }
}

int Chip::LoadGame(const char* filename) {
    int retVal = -1;

    std::ifstream is(filename, std::ifstream::binary);
    if (is) {
        is.seekg(0, is.end);
        int length = is.tellg();
        is.seekg(0, is.beg);
    
        char buffer[length];
        is.read(buffer, length);

        if (is) {
            for (int i = 0; i < length; ++i) {
                memory_[i + MEMORY_OFFSET] = buffer[i];
            }
            retVal = 0;
        } else {
            std::cerr << "error: only " << is.gcount() << " could be read" << std::endl;
        }
        is.close();
    } 

    return retVal;
}

void Chip::EmulateCycle() {
    if (delay_timer_ > 0) --delay_timer_;
    if (sound_timer_ > 0) --sound_timer_;

    uint16_t instruction = memory_[PC_] << 8 | memory_[PC_ + 1]; // big endian
    PC_ += 2;

    uint16_t op  = instruction & 0xF000;
    uint16_t x   = (instruction & 0x0F00) >> 8;
    uint16_t y   = (instruction & 0x00F0) >> 4;
    uint16_t kk  = instruction & 0x00FF;
    uint16_t nnn = instruction & 0x0FFF;
    uint16_t n   = instruction & 0x000F;

    switch (op) {
        case 0x0000:
            if (instruction == 0x00E0) { // 00E0: clear the screen
                for (auto& row: display_) row.fill(0);
            }
            if (instruction == 0x00EE) { // 00EE: return from a subroutine
               PC_ = stack_[SP_];
               if (SP_ >= 1) {
                   --SP_;
               }
            }
            break;

        case 0x1000: // 1NNN: jump to address NNN
            PC_ = nnn;
            break;
            
        case 0x2000: // 2NNN: Execute subroutines starting at address NNN
            stack_[++SP_] = PC_;
            PC_ = nnn;
            break;

        case 0x3000: // 3XNN: Skip the following instruction if the value of
                     // register VX equals to NN
            if (V_[x] == kk) {
                SkipNextInstruction();
            }
            break;

        case 0x4000: // 4XNN: Skip the following instruction if the value of
                     // register VX is not equal to NN
            if (V_[x] != kk) {
                SkipNextInstruction();
            }

            break;

        case 0x5000: // 5XY0: Skip the following instruction if the value of 
                     // register VX is not equal to the value of register VY
            if (V_[x] == V_[y]) {
                SkipNextInstruction();
            }
            break;

        case 0x6000: // 6XNN: Store number NN in register VX
            V_[x] = kk;
            break;

        case 0x7000: // 7XNN: Add the value NN to register VX
            V_[x] += kk;
            break;

        case 0x8000:
            if (n == 0) { // 8XY0: Store the value of register VY in register VX
                V_[x] = V_[y];
            }
            if (n == 1) { // 8XY1: Set VX to VX OR VY
                V_[x] = V_[x] | V_[y];
            }
            if (n == 2) { // 8XY2: Set VX to VX AND VY
                V_[x] = V_[x] & V_[y]; 
            }
            if (n == 3) { // 8XY3: Set VX to VX XOR VY
                V_[x] = V_[x] ^ V_[y];
            }
            if (n == 4) { // 8XY4: Add the value of register VY to register VX
                          // Set VF to 01 if a carry occurs
                          // Set VF to 00 if a carry does not occur
                if (V_[x] + V_[y] > 255) {
                    V_[0xF] = 1;
                } else {
                    V_[0xF] = 0;
                }
                V_[x] += V_[y];     
            }
            if (n == 5) { // 8XY5: Subtract the value of register VY
                          // from register VX. Set VF to 0 if a borrow occurs
                          // Set VF to 1 if a borrow does not occur
                if (V_[x] < V_[y]) {
                    V_[0xF] = 0;
                } else {
                    V_[0xF] = 1;
                }
                V_[x] -= V_[y];
            }
            if (n == 6) { // 8XY6: Store the value of register VY shifted right
                          // one bit in register VX. Set register VF to the least
                          // significant bit prior to the shift
                V_[0xF] = V_[x] & 0x01;
                V_[x] = V_[y] >> 1;
            }
            if (n == 7) { // 8XY7: Set register VX to the value of VY minus VX
                          // Set VF to 00 if a borrow occurs
                          // Set VF to 01 if a borrow does not occur
                if (V_[y] < V_[x]) {
                    V_[0xF] = 0;
                } else {
                    V_[0xF] = 1;
                }
                V_[x] = V_[y] - V_[x];
            } 
            if (n == 0xE) { // 8XYE: Store the value of register VY shifted left
                            // one bit in register VX. Set register VF to the most 
                            // significant bit prior to the shift
                V_[0xF] = V_[y] & 0x80;
                V_[x] = V_[y] << 1;
            }
            break;

        case 0x9000: // 9XY0: Skip the following instruction if the value of
                     // register VX is not equal to the value of register VY
            if (V_[x] != V_[y]) {
                SkipNextInstruction();
            }
            break;

        case 0xA000: // ANNN: Store memory address NNN in register I
            I_ = nnn;
            break;

        case 0xB000: // BNNN: Jump to address NNN + V0
            PC_ = V_[0] + nnn;
            break;

        case 0xC000: // CXNN: Set VX to a random number with a mask of NN
                     // Draw a sprite at position VX, VY with N bytes of sprite
                     // data starting at the address stored in I
            V_[x] = (rand() % 255) & kk;
            break;

        case 0xD000: // DXYN:  Display n-byte sprite starting at memory location I 
                     // at (Vx, Vy), set VF = collision
           { 
                int x_pos = V_[x], y_pos = V_[y];
                int ands[8] = {128, 64, 32, 16, 8, 4, 2, 1};
                bool collision = false;
                for (int i = 0; i < n; ++i) {
                    for (int j = 0; j < SPRITE_WIDTH; ++j) {
                       if (x_pos + j == SCREEN_WIDTH) {
                            x_pos = -j;
                        } 
                        if (y_pos + i == SCREEN_HEIGHT) {
                            y_pos = -i;
                        }
                        uint8_t oldVal = display_[x_pos + j][y_pos + i];
                        uint8_t newVal = (memory_[I_ + i] & ands[j]) >> (8 - j - 1);
                        if (oldVal == 1 && newVal == 1) { collision = true; }

                        display_[x_pos + j][y_pos + i] ^= newVal;
                    }
                    x_pos = V_[x];
                    y_pos = V_[y];
                }

                V_[0xF] = collision ? 1 : 0;
            }
            break;

        case 0xE000:
            if (kk == 0x009E) { // EX9E: Skip the following instruction if the key
                                // corresponding to the hex value currently stored
                                // in register VX is pressed
                if (key_ == V_[x]) {
                    SkipNextInstruction();
                }

            }

            if (kk == 0x00A1) { // EX9E: Skip the following instruction if the key
                                // corresponding to the hex value currently stored
                                // in register VX is not pressed
                if (key_ != V_[x]) {
                    SkipNextInstruction();
                }
            }

            break;

        case 0xF000:
            if (kk == 0x07) { // FX07: Store the current value of the delay timer
                              // in register VX
                V_[x] = delay_timer_;
            }
            if (kk == 0x0A) { // FX0A: Wait for a keypress adn store the result 
                             // in register VX

            }
            if (kk == 0x15) { // FX15: Set the delay timer to the value of
                              // register VX
                delay_timer_ = V_[x];
            }
            if (kk == 0x18) { // FX18: Set the sound timer to the value of 
                             // register VX
                sound_timer_ = V_[x];
            }
            if (kk == 0x1E) { // FX1E: Add the value stored in register VX to
                              // register I
                I_ += V_[x];
            }
            if (kk == 0x29) { // FX29: Set I to the memory address of the sprite
                              // data corresponding to the hex digit stored in
                              // register VX
                I_ = V_[x] * 5; // 5 uint8_t per character (0 - F)
            }
            if (kk == 0x33) { // FX33: Store the binary-coded decimal equivalent
                              // of the value stored in register VX at address
                              // I, I+1, and I+2
                memory_[I_] = V_[x] / 100;
                memory_[I_ + 1] = V_[x] /10 % 10;
                memory_[I_ + 2] = V_[x] % 10;
            }
            if (kk == 0x55) { // FX55: Store the value of registers V0 to VX 
                              // inclusive in memory starting at address I
                              // I is set to I+X+1 after operation
                for (int i = 0; i <= x; ++i) {
                    memory_[I_ + i] = V_[i];
                }
                I_ += (x + 1);

            }
            if (kk == 0x65) { // FX65: Fill registers V0 to VX inclusive with 
                              // the values stored in memory starting at address I
                              // I is set to I+X+1 after operation
                for (int i = 0; i <= x; ++i) {
                    V_[i] = memory_[I_ + i];
                }
                I_ += (x + 1);
            }
            break;

        default:
            break;
    }

    if (delay_timer_ > 0) {
        --delay_timer_;
    }

    if (sound_timer_ > 0) {
        if (sound_timer_ == 1) {
            // beep
        }
        --sound_timer_;
    }
}

#include "emulator.h"

#include <iostream>
#include <chrono>
#include <thread>

#include "SDL2/SDL.h"

SDL_Keycode Keys[16] = 
{
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_r,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_f,
    SDLK_z,
    SDLK_x,
    SDLK_c,
    SDLK_v
};

int main(int argc, char* argv[])
{
    emulator ch8;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;

    if (ch8.load_program("C:\\Users\\brandona\\Desktop\\white_noise\\games\\PONG2"))
    {
        if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) != 0)
        {
            std::cerr << "Unable to init SDL" << std::endl;
            return -1;
        }

        window = SDL_CreateWindow("Chip 8",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  640, 320,
                                  0);

        if (window == nullptr)
        {
            std::cerr << "Unable to create SDL window" << std::endl;
            return -1;
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        if (renderer == nullptr)
        {
            std::cerr << "Unable to create SDL renderer" << std::endl;
            return -1;
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STREAMING, 
                                    EMULATOR_DISPLAY_WIDTH,
                                    EMULATOR_DISPLAY_HEIGHT);

        if (texture == nullptr)
        {
            std::cerr << "Unable to create a SDL texture for rendering" << std::endl;
            return -1;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_Event e;
        bool quit = false;

        // Initialize to a value that will cause the first frame to cycle
        float time_elapsed_us = 2000;

        auto begin = std::chrono::high_resolution_clock::now();
        auto end = std::chrono::high_resolution_clock::now();

        while (!quit)
        {
            begin = std::chrono::high_resolution_clock::now();
            auto duration = begin - end;
            time_elapsed_us += std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }

                if (e.type == SDL_KEYDOWN)
                {
                    for (int i = 0; i < 16; i++)
                    {
                        if (e.key.keysym.sym == Keys[i])
                        {
                            ch8.set_key_down(i);
                        }
                    }
                }

                if (e.type == SDL_KEYUP)
                {
                    for (int i = 0; i < 16; i++)
                    {
                        if (e.key.keysym.sym == Keys[i])
                        {
                            ch8.set_key_up(i);
                        }
                    }
                }
            }

            if (!ch8.error_flag())
            {
                if (time_elapsed_us >= 1667)
                {
                    ch8.cycle();
                    //ch8.print_opcode();
                    //ch8.print_display();
                    time_elapsed_us = 0;
                }
            }
            else
            {
                std::cerr << "Emulator error, shutting down" << std::endl;
                return -1;
            }

            if (ch8.needs_draw())
            {
                SDL_RenderClear(renderer);

                uint32_t pixels[2048]; 
                int pixel_indx = 0;
                for (int r = 0; r < EMULATOR_DISPLAY_HEIGHT; r++)
                {
                    for (int c = 0; c < EMULATOR_DISPLAY_WIDTH; c++)
                    {
                        if (ch8.get_display_pixel(c, r))
                        {
                            pixels[pixel_indx] = 0xFFFFFFFF; 
                        }
                        else
                        {
                            pixels[pixel_indx] = 0xFF000000;
                        }

                        pixel_indx++;
                    }
                }

                SDL_UpdateTexture(texture, nullptr, pixels, (64*4)); 
                SDL_RenderCopy(renderer, texture, nullptr, nullptr);
                SDL_RenderPresent(renderer);
            }
            end = std::chrono::high_resolution_clock::now();
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

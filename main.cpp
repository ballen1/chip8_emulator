#include "emulator.h"

#include <iostream>

#include "SDL2/SDL.h"

int main(int argc, char* argv[])
{
    emulator ch8;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;

    if (ch8.load_program("C:\\Users\\brandona\\Desktop\\white_noise\\games\\tetris.c8"))
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

        while (!quit)
        {
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
            }

            if (!ch8.error_flag())
            {
                ch8.cycle();
                ch8.tick_timers();
                ch8.print_opcode();
                //ch8.print_display();
            }
            else
            {
                std::cerr << "Emulator error, shutting down" << std::endl;
                return -1;
            }

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
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>

class Pipe{
    public:
        int posX;
        int posY;
};


void draw_bird(SDL_Renderer *renderer, double y)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect r = {114, int(y), -64, 64};
    SDL_RenderFillRect(renderer, &r);
}

void draw_pipe(SDL_Renderer *renderer, Pipe ceiling, Pipe ground, int mode){

    // Ground pipe
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect top = {ground.posX, ground.posY, 64, 1080};
    SDL_RenderFillRect(renderer, &top);  

    // Ceiling pipe
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect bottom = {ceiling.posX, ceiling.posY, 64, -1080};
    SDL_RenderFillRect(renderer, &bottom);    
}

int main(int argc, char *argv[])
{
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;

    // Initial variables
    int done = 0, birdPosY = 80;
    float birdSpeed = 0;

    // Generate pipes
    Pipe groundPipes[8];
    Pipe ceilingPipes[8];

    for(int i = 0; i < 8; i++){
        groundPipes[i].posX = 512 * i + 512;
        groundPipes[i].posY = rand() % (1080 - 540 + 1) + 540;
        ceilingPipes[i].posX = groundPipes[i].posX;
        ceilingPipes[i].posY = groundPipes[i].posY - 384; 
    }


    // mandatory at least on switch, else gfx is not properly closed
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        SDL_Log("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    // create an SDL window (OpenGL ES2 always enabled)
    // when SDL_FULLSCREEN flag is not set, viewport is automatically handled by SDL (use SDL_SetWindowSize to "change resolution")
    // available switch SDL2 video modes :
    // 1920 x 1080 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
    // 1280 x 720 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
    window = SDL_CreateWindow("sdl2_gles2", 0, 0, 1920, 1080, 0);
    if (!window) {
        SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // create a renderer (OpenGL ES2)
    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // open CONTROLLER_PLAYER_1 and CONTROLLER_PLAYER_2
    // when railed, both joycons are mapped to joystick #0,
    // else joycons are individually mapped to joystick #0, joystick #1, ...
    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L45
    for (int i = 0; i < 2; i++) {
        if (SDL_JoystickOpen(i) == NULL) {
            SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
            SDL_Quit();
            return -1;
        }
    }



    while (!done) {
        // Input must be taken in before drawing
        while (SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_JOYBUTTONDOWN:

                    if(event.jbutton.which == 0){
                        if(event.jbutton.button == 0){
                            birdSpeed = -20;
                        } else if (event.jbutton.button == 10){
                            done = 1;
                        }
                    }
            }
        }



        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_RenderClear(renderer);

        // All game functions

        // Check if player is dead
        if(birdPosY >= 1017 || birdPosY <= 0){
            // Show death screen, for now this just stops the game (just dont die Kappa)
            done = 1;
        }

        birdPosY += birdSpeed;
        birdSpeed += 1.05;

        draw_bird(renderer, birdPosY);

        for(int i = 0; i < 8; i++){

            // Draw the pipes and move them to the sides
            draw_pipe(renderer, ceilingPipes[i], groundPipes[i], 0);
            ceilingPipes[i].posX -= 3;
            groundPipes[i].posX -= 3;

            // Generate new pipes
            if(groundPipes[i].posX <= 0){
                ceilingPipes[i].posX = 4320 - 128;
                groundPipes[i].posX = 4320 - 128;
                groundPipes[i].posY = rand() % (1080 - 540 + 1) + 540;
                ceilingPipes[i].posY = groundPipes[i].posY - 384;
            }

            // Hit detection
            if(ceilingPipes[i].posX <= 114 && ceilingPipes[i].posX >= 20){
                if(ceilingPipes[i].posY >= birdPosY || groundPipes[i].posY <= birdPosY){
                    done = 1;
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#include "SDL2/SDL.h"
#include <stdio.h>

#define ASSERT(_e, ...)               \
    if (!(_e))                        \
    {                                 \
        fprintf(stderr, __VA_ARGS__); \
        exit(1);                      \
    }

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 180

typedef float f32;
typedef double f64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

struct
{
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    bool quit;
} state;

int main(int argc, char *argv[])
{
    ASSERT(
        !SDL_Init(SDL_INIT_VIDEO),
        "SDL Failed to iniitalize: %s\n",
        SDL_GetError());

    state.window = SDL_CreateWindow(
        "DEMO",
        SDL_WINDOWPOS_CENTERED_DISPLAY(1),
        SDL_WINDOWPOS_CENTERED_DISPLAY(1),
        1280,
        720,
        SDL_WINDOW_ALLOW_HIGHDPI);

    ASSERT(state.window, "Failed to create SDL Window: %s\n", SDL_GetError());

    state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_PRESENTVSYNC);
    ASSERT(state.renderer, "Failed to Create SDL Renderer: %s\n", SDL_GetError());
    state.texture = SDL_CreateTexture(
        state.renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);
    ASSERT(state.texture, "Failed to Create SDL Renderer: %s\n", SDL_GetError());
    while (!state.quit)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
            case SDL_QUIT:
                state.quit = true;
                break;
            }
        }
        state.pixels[(10 * SCREEN_WIDTH) + 5] = 0xFFFF00FF;
        SDL_UpdateTexture(state.texture, NULL, state.pixels, SCREEN_WIDTH * 4);
        SDL_RenderCopyEx(
            state.renderer,
            state.texture,
            NULL,
            NULL,
            0.0,
            NULL,
            SDL_FLIP_VERTICAL);
        SDL_RenderPresent(state.renderer);
    }
    SDL_DestroyWindow(state.window);
    return 0;
}
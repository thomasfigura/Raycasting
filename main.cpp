#include <stdio.h>
#include <stdint.h>

#include "SDL2/SDL.h"

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

#define ASSERT(_e, ...)               \
    if (!(_e)) {                      \
        fprintf(stderr, __VA_ARGS__); \
        exit(1);                      \
    }

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 180

typedef struct v2_s {
    f32 x, y;
} v2;
typedef struct v2i_s {
    i32 x, y;
} v2i;

#define dot(v0, v1)                        \
    ({                                     \
        const v2 _v0 = (v0), _v1 = (v1);   \
        (_v0.x * _v1.x) + (_v0.y * _v1.y); \
    })
#define length(v)           \
    ({                      \
        const v2 _v = (v);  \
        sqrtf(dot(_v, _v)); \
    })
#define normalize(u)              \
    ({                            \
        const v2 _u = (u);        \
        const f32 l = length(u);  \
        (v2){_u.x / l, _u.y / l}; \
    })
#define min(a, b)                         \
    ({                                    \
        __typeof__(a) _a = (a), _b = (b); \
        _a < _b ? _a : _b;                \
    })
#define max(a, b)                         \
    ({                                    \
        __typeof__(a) _a = (a), _b = (b); \
        _a > _b ? _a : _b;                \
    })

#define sign(a) ({                                   \
    __typeof__(a) _a = (a);                          \
    (__typeof__(a))(_a < 0 ? -1 : (_a > 0 ? 1 : 0)); \
})
#define MAP_SIZE 8
static u8 MAPDATA[MAP_SIZE * MAP_SIZE] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 0, 4, 0, 0, 0, 1,
    1, 0, 3, 0, 0, 2, 0, 1,
    1, 0, 0, 0, 2, 2, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1};

struct {
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    bool quit;
    v2 pos, dir, plane;

} state;

static void verline(int x, int y0, int y1, u32 color) {
    for (int y = y0; y <= y1; y++) {
        state.pixels[(y * SCREEN_WIDTH) + x] = color;
    }
}
static void render() {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        const f32 xcam = (2 * (x / (f32)(SCREEN_WIDTH))) - 1;

        const v2 dir = {state.dir.x + state.plane.x * xcam,
                        state.dir.y + state.plane.y * xcam};

        v2 pos = state.pos;
        v2 ipos = {(int)pos.x, (int)pos.y};

        const v2 deltadist = {fabsf(dir.x) < 1e-10 ? 1e30 : fabsf(1.0f / dir.x),
                              fabsf(dir.y) < 1e-10 ? 1e30 : fabsf(1.0f / dir.y)};

        v2 sidedist = {
            deltadist.x * (dir.x < 0 ? (pos.x - ipos.x) : (ipos.x + 1 - pos.x)),
            deltadist.y * (dir.y < 0 ? (pos.y - ipos.y) : (ipos.x + 1 - pos.y))};
        const v2i step = {(int)sign(dir.x),
                          (int)sign(dir.y)};

        struct {
            int val, side;
            v2 pos;
        } hit = {0, 0, {0.0f, 0.0f}};

        while (!hit.val) {
            if (sidedist.x < sidedist.y) {
                sidedist.x += deltadist.x;
                ipos.x += step.x;
                hit.side = 0;
            } else {
                sidedist.y += deltadist.y;
                ipos.y += step.y;
                hit.side = 1;
            }
            ASSERT(
                ipos.x >= 0 && ipos.x < MAP_SIZE && ipos.y >= 0 && ipos.y < MAP_SIZE,
                "DDA out of bounds");

            hit.pos = (v2) { pos.x + sidedist.x, pos.y + sidedist.y};
        }
    }
}

int main(int argc, char *argv[]) {
    ASSERT(!SDL_Init(SDL_INIT_VIDEO), "SDL Failed to iniitalize: %s\n",
           SDL_GetError());

    state.window = SDL_CreateWindow("DEMO", SDL_WINDOWPOS_CENTERED_DISPLAY(1),
                                    SDL_WINDOWPOS_CENTERED_DISPLAY(1), 1280, 720,
                                    SDL_WINDOW_ALLOW_HIGHDPI);

    ASSERT(state.window, "Failed to create SDL Window: %s\n", SDL_GetError());

    state.renderer =
        SDL_CreateRenderer(state.window, -1, SDL_RENDERER_PRESENTVSYNC);
    ASSERT(state.renderer, "Failed to Create SDL Renderer: %s\n", SDL_GetError());
    state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888,
                                      SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH,
                                      SCREEN_HEIGHT);
    ASSERT(state.texture, "Failed to Create SDL Renderer: %s\n", SDL_GetError());

    state.pos = (v2){5, 5};
    state.dir = normalize(((v2){-1.0f, 0.0f}));
    state.plane = (v2){0.0f, 0.66f};

    while (!state.quit) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
            case SDL_QUIT:
                state.quit = true;
                break;
            }
        }
        render();
        SDL_UpdateTexture(state.texture, NULL, state.pixels, SCREEN_WIDTH * 4);
        SDL_RenderCopyEx(state.renderer, state.texture, NULL, NULL, 0.0, NULL,
                         SDL_FLIP_VERTICAL);
        SDL_RenderPresent(state.renderer);
    }

    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    return 0;
}
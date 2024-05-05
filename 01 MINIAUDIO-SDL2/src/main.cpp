// https://miniaud.io/docs/examples/engine_sdl.html
// ^^ code is drawn from here :)

#define MA_NO_DEVICE_IO
#define MINIAUDIO_IMPLEMENTATION

#include <iostream>
#include <SDL.h>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include "../libs/glm/glm.hpp" // not included in git repo, download from glm official repo and add to libs
#else
    #include <glm/glm.hpp>
#endif

#include "../libs/miniaudio.h"

#include "my_data_source.h"
#include "entity.h"

#define SAMPLE_RATE   44100

#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480

#ifdef __EMSCRIPTEN__
    #define FPS 1.0f
    bool userInteracted = false;
#else
    #define FPS 144.0f
#endif

// ------------------------------------------------------------
bool running = true;
double NOW = SDL_GetPerformanceCounter();
double LAST = 0;
double deltaTime = 0;

struct {
    glm::vec2 position = glm::vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    glm::vec2 direction = glm::vec2(1.0, 0.0);
    float speed = 0.1;
    unsigned short size = 8;
} ball;

// --- ma setup
static ma_engine engine;
static ma_sound chirp;
static my_data_source test;

// --- sdl setup
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Event e;

void data_callback(void* pUserData, ma_uint8* pBuffer, int bufferSizeInBytes)
{
    /* Reading is just a matter of reading straight from the engine. */
    ma_uint32 bufferSizeInFrames = (ma_uint32)bufferSizeInBytes / ma_get_bytes_per_frame(ma_format_f32, ma_engine_get_channels(&engine));
    ma_engine_read_pcm_frames(&engine, pBuffer, bufferSizeInFrames, NULL);
}

static void input() {
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            #ifdef __EMSCRIPTEN__
                emscripten_cancel_main_loop();
            #endif
        }
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            #ifdef __EMSCRIPTEN__
            if (!userInteracted) {
                userInteracted = true;
                return;
            }
            #endif
            int x, y;
            SDL_GetMouseState(&x, &y);

            ball.direction = glm::normalize(glm::vec2(x, y) - ball.position);
        }
    }
}

static void update() {
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();

    deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency() );

    ball.position += ball.direction * (ball.speed * (float)deltaTime);

    if (ball.position.x > WINDOW_WIDTH || ball.position.x < 0 - ball.size / 2) {
        ball.direction.x = -ball.direction.x;
        ma_sound_start(&chirp);
    } else if (ball.position.y > WINDOW_HEIGHT || ball.position.y < 0 - ball.size / 2) {
        ball.direction.y = -ball.direction.y;
        ma_sound_start(&chirp);
    }

}

static void draw() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // set colour to pure white
    SDL_RenderClear(renderer);                            // clear the renderer with current colour

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    for (int y = 0; y < ball.size; y++) {
        for (int x = 0; x < ball.size; x++) {
            SDL_RenderDrawPoint(renderer, ball.position.x + x - ball.size/2, ball.position.y + y - ball.size/2);
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderDrawLine(renderer, ball.position.x, ball.position.y, ball.position.x + ball.direction.x * 10, ball.position.y + ball.direction.y * 10);

    SDL_RenderPresent(renderer);                          // present renderer to the end user
}

static void mainloop(void) {
    input();

    #ifdef __EMSCRIPTEN__
        if (!userInteracted) {
            return;
        }
    #endif

    update();
    draw();
}

int main() {
    // --- ma
    ma_result result;
    ma_engine_config engineConfig;

    engineConfig = ma_engine_config_init();
    engineConfig.noDevice = MA_TRUE;
    engineConfig.channels = 2;
    engineConfig.sampleRate = SAMPLE_RATE;

    result = ma_engine_init(&engineConfig, &engine);
    if (result != MA_SUCCESS) {
        printf("Failed to initialise audio engine.\n");
        return -1;
    }

    result = my_data_source_init(&test);
    if (result != MA_SUCCESS) {
        printf("Failed to initialise my_data_source.\n");
        return -1;
    }

    result = ma_sound_init_from_data_source(&engine, &test, 0, NULL, &chirp);
    if (result != MA_SUCCESS) {
        printf("Failed to initialise sound.\n");
        return -1;
    }

    // --- sdl
    SDL_AudioSpec desiredSpec;
    SDL_AudioSpec obtainedSpec;
    SDL_AudioDeviceID deviceID;

    MA_ZERO_OBJECT(&desiredSpec);
    desiredSpec.freq     = ma_engine_get_sample_rate(&engine);
    desiredSpec.format   = AUDIO_F32;
    desiredSpec.channels = ma_engine_get_channels(&engine);
    desiredSpec.samples  = 512;
    desiredSpec.callback = data_callback;
    desiredSpec.userdata = NULL;

    SDL_Init(SDL_INIT_VIDEO); // also initialises events

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        printf("Failed to initialise SDL subsystem.\n");
        return -1;
    }

    deviceID = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &obtainedSpec, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (deviceID == 0) {
        printf("Failed to open SDL audio device.\n");
        return -1;
    }

    SDL_PauseAudioDevice(deviceID, 0); // make sure device is not paused

    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "SDL Miniaudio");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(mainloop, 0, 1);
    #else
        while (running) mainloop();
    #endif

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

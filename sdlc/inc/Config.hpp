#pragma once

#ifdef TTF_INIT
#include <SDL3_ttf/SDL_ttf.h>
#endif

#ifndef AUDIO_NUM_STREAMS
#define AUDIO_NUM_STREAMS 1
#endif

#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH 640
#endif

#ifndef WINDOW_HEIGHT
#define WINDOW_HEIGHT 480
#endif

#ifndef WINDOW_FLAGS
#define WINDOW_FLAGS 0
#endif

#ifndef WINDOW_TITLE
#define WINDOW_TITLE "SDL3 Application"
#endif

#ifndef ORG_NAME
#define ORG_NAME "Unamed org"
#endif

#ifndef APP_NAME
#define APP_NAME "Unnamed app"
#endif 

#ifndef RENDER_LOGICAL_WIDTH
#define RENDER_LOGICAL_WIDTH 0
#endif

#ifndef  RENDER_LOGICAL_HEIGHT
#define RERENDER_LOGICAL_HEIGHT 0
#endif

#ifndef RENDER_VSYNC
#define RERENDER_VSYNC 0
#endif

struct AppConfig {
    int width{WINDOW_WIDTH};
    int height{WINDOW_HEIGHT};
    int logicalWidth{RENDER_LOGICAL_WIDTH};
    int logicalHeight{RENDER_LOGICAL_HEIGHT};
    int vSync{RENDER_VSYNC};
};
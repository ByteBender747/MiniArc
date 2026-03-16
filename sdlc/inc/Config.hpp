#pragma once

#ifdef TTF_INIT
#include <SDL3_ttf/SDL_ttf.h>
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

#ifndef APP_INIT
#define APP_INIT
#warning Application entry point not defined
#endif

#ifndef APP_EXIT
#define APP_EXIT
#warning Application exit point not defined
#endif
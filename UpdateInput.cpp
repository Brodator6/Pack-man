#include <SDL3/SDL_events.h>
#include <map>

static void UpdateInput(std::map<SDL_Scancode, bool>& control)
{
    const bool *keyStates = SDL_GetKeyboardState(NULL);//reading control from keyboard (maybe better to read only necessary keys)

    for(auto key : control){//pasting keystate from keyboard into control
        control[key.first] = keyStates[key.first];
    }
}
#include "stdafx.h"
#include "Engine.h"
#include "Input/Detail/InputManager.h"
#include <SDL.h>

namespace {

    using namespace slon::input;

    KeyboardEvent* craeteKeyboardEvent(const SDL_Event& event)
    {
        assert( (event.type == SDL_KEYUP || event.type == SDL_KEYDOWN)
                && "Trying to construct keyboard event from non-keyboard SDL event" );

        KeyboardEvent* keyboardEvent = new KeyboardEvent();
        keyboardEvent->state    = event.type == SDL_KEYDOWN ? KeyboardEvent::DOWN : KeyboardEvent::UP;
        keyboardEvent->key      = key_sym(event.key.keysym.sym);
        keyboardEvent->modifier = event.key.keysym.mod;

        return keyboardEvent;
    }

    MouseEvent* createMouseMotionEvent(const SDL_Event& event)
    {
        assert( event.type == SDL_MOUSEMOTION
                && "Trying to construct mouse motion event from non-mousemotion SDL event" );

        MouseEvent* mouseEvent = new MouseEvent();
        mouseEvent->button = MBUTTON_NONE;
        mouseEvent->x      = event.motion.x;
        mouseEvent->y      = event.motion.y;
        mouseEvent->xrel   = event.motion.xrel;
        mouseEvent->yrel   = event.motion.yrel;

        return mouseEvent;
    }

    MouseEvent* createMouseButtonEvent(const SDL_Event& event)
    {
        assert( (event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEBUTTONDOWN)
                && "Trying to construct mouse motion event from non-mousebutton SDL event" );

        MouseEvent* mouseEvent = new MouseEvent();
        mouseEvent->button      = mouse_button(event.button.button);
        mouseEvent->buttonState = event.type == SDL_MOUSEBUTTONDOWN ? MouseEvent::DOWN : MouseEvent::UP;
        mouseEvent->x           = event.button.x;
        mouseEvent->y           = event.button.y;

        return mouseEvent;
    }

} // anonymouse namespace

namespace slon {
namespace input {
namespace detail {

InputManager::InputManager() :
    ignoreSetCursorPositionEvent(false),
    cursorPositionFixed(false)
{
}

void InputManager::handleEvent(const SDL_Event* event)
{
    assert(event && "event must be not 0");

    boost::shared_ptr<InputEvent> inputEvent;
    switch (event->type)
    {
    case SDL_KEYUP:
    case SDL_KEYDOWN:
        inputEvent.reset( craeteKeyboardEvent(*event) );
        break;

    case SDL_MOUSEMOTION:
        // Need to filter event from SetCursorPosition
        if ( !ignoreSetCursorPositionEvent
             || event->motion.x != fixedCursorPosition.x
             || event->motion.y != fixedCursorPosition.y )
        {
            inputEvent.reset( createMouseMotionEvent(*event) );
            if (cursorPositionFixed) {
                setCursorPosition(fixedCursorPosition.x, fixedCursorPosition.y);
            }
        }
        else if ( event->motion.x == fixedCursorPosition.x
                  && event->motion.y == fixedCursorPosition.y )
        {
            ignoreSetCursorPositionEvent = false;
        }
        else {
            setCursorPosition(fixedCursorPosition.x, fixedCursorPosition.y);
        }

        break;

    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
        inputEvent.reset( createMouseButtonEvent(*event) );
        break;

    default:
        break;
    }

    if (inputEvent)
    {
        for(size_t i = 0; i<inputHandlers.size(); ++i) {
            inputHandlers[i]->handleEvent(*inputEvent);
        }
    }
}

void InputManager::handleEvents()
{
    SDL_Event event;
    while( SDL_PollEvent(&event) && Engine::Instance()->isRunning() ) {
        handleEvent(&event);
    }

    for(size_t i = 0; i<inputHandlers.size(); ++i) {
        inputHandlers[i]->update();
    }
}

void InputManager::setCursorPosition(unsigned int x, unsigned int y, bool generateEvent)
{
    SDL_WarpMouse(x, y);
    fixedCursorPosition.x = x;
    fixedCursorPosition.y = y;
    ignoreSetCursorPositionEvent = !generateEvent;
}

void InputManager::fixCursorPosition(unsigned int x, unsigned int y)
{
    cursorPositionFixed = true;
    fixedCursorPosition.x = x;
    fixedCursorPosition.y = y;
}

void InputManager::showCursor(bool toggle)
{
    SDL_ShowCursor(toggle);
}

bool InputManager::isCursorVisible() const
{
    return SDL_ShowCursor(SDL_QUERY) == 1;
}

} // namespace detail
} // namespace input
} // namespace slon
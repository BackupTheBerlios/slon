#ifndef __SLON_ENGINE_INPUT_DETAIL_INPUT_MANAGER__
#define __SLON_ENGINE_INPUT_DETAIL_INPUT_MANAGER__

#include "../InputManager.h"
#include <sgl/Math/Matrix.hpp>

// Forward decl
union SDL_Event;

namespace slon {
namespace input {
namespace detail {

/** Main class of the SlonEngine. The manager of the hole graphics,
 * physics, sound and etc in the scene.
 */
class InputManager :
    public input::InputManager
{
public:
    InputManager();

    /** Get first input_handler iterator */
    virtual input_handler_iterator firstInputHandler() { return inputHandlers.begin(); }

    /** Get first input_handler iterator */
    virtual const_input_handler_iterator firstInputHandler() const { return inputHandlers.begin(); }

    /** Get first input_handler iterator */
    virtual input_handler_iterator endInputHandler() { return inputHandlers.end(); }

    /** Get first input_handler iterator */
    virtual const_input_handler_iterator endInputHandler() const { return inputHandlers.end(); }

    /** Add input handler to the engine input_handlers vector. Input handlers will
     * handle corresponding events consequently, in the order you have them added.
     * You should care about the uniqueness if the input handlers.
     * Duplicate input handlers will handle events twice, so take care
     * of it, if such behaviour is not your goal.
     */
    virtual void addInputHandler(InputHandler* inputHandler) { inputHandlers.push_back(inputHandler); }

    /** Setup the position of the cursor.
     * @param x - x window coordinate of the cursor.
     * @param y - y window coordinate of the cursor.
     * @param generateEvent - generate mouse motion event.
     */
    virtual void setCursorPosition(unsigned int x, unsigned int y, bool generateEvent = false);

    /** Fix mouse cursor position. Mouse motion will be generated still, but
     * after each mouse cursor will return to the specified position.
     */
    virtual void fixCursorPosition(unsigned int x, unsigned int y);

    /** Allow cursor to travel along the window */
    virtual void freeCursorPosition() { cursorPositionFixed = false; }

    /** Check whether cursor is fixed */
    virtual bool isCursorPositionFixed() const { return cursorPositionFixed; }

    /** Show/Hide cursor */
    virtual void showCursor(bool toggle);

    /** Check whether cursor is shown */
    virtual bool isCursorVisible() const;

    // Called by engine
    void handleEvents();

    void handleEvent(const SDL_Event* event);

private:
    input_handler_vector    inputHandlers;
    bool                    ignoreSetCursorPositionEvent;
    bool                    cursorPositionFixed;
    math::Vector2ui    		fixedCursorPosition;
};

} // namespace detail
} // namespace input
} // namespace slon

#endif // __SLON_ENGINE_INPUT_DETAIL_INPUT_MANAGER__

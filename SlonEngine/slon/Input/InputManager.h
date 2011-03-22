#ifndef __SLON_ENGINE_INPUT_INPUT_MANAGER__
#define __SLON_ENGINE_INPUT_INPUT_MANAGER__

#include "InputHandler.h"
#include <boost/intrusive_ptr.hpp>
#include <vector>

namespace slon {
namespace input {

/** Main class of the SlonEngine. The manager of the hole graphics,
 * physics, sound and etc in the scene.
 */
class InputManager
{
public:
    typedef std::vector<input_handler_ptr>          input_handler_vector;
    typedef input_handler_vector::iterator          input_handler_iterator;
    typedef input_handler_vector::const_iterator    const_input_handler_iterator;

public:
    /** Get first input_handler iterator */
    virtual input_handler_iterator firstInputHandler() = 0;

    /** Get first input_handler iterator */
    virtual const_input_handler_iterator firstInputHandler() const = 0;

    /** Get first input_handler iterator */
    virtual input_handler_iterator endInputHandler() = 0;

    /** Get first input_handler iterator */
    virtual const_input_handler_iterator endInputHandler() const = 0;

    /** Add input handler to the engine input_handlers vector. Input handlers will
     * handle corresponding events consequently, in the order you have them added.
     * You should care about the uniqueness if the input handlers.
     * Duplicate input handlers will handle events twice, so take care
     * of it, if such behaviour is not your goal.
     */
    virtual void addInputHandler(InputHandler* inputHandler) = 0;

    /** Setup the position of the cursor.
     * @param x - x window coordinate of the cursor.
     * @param y - y window coordinate of the cursor.
     * @param generateEvent - generate mouse motion event.
     */
    virtual void setCursorPosition(unsigned int x, unsigned int y, bool generateEvent = false) = 0;

    /** Fix mouse cursor position. Mouse motion will be generated still, but
     * after each mouse cursor will return to the specified position.
     */
    virtual void fixCursorPosition(unsigned int x, unsigned int y) = 0;

    /** Allow cursor to travel along the window */
    virtual void freeCursorPosition() = 0;

    /** Check whether cursor is fixed */
    virtual bool isCursorPositionFixed() const = 0;

    /** Show/Hide cursor */
    virtual void showCursor(bool toggle) = 0;

    /** Check whether cursor is shown */
    virtual bool isCursorVisible() const = 0;

    virtual ~InputManager() {}
};

/** Get current input manager used by engine. */
InputManager& currentInputManager();

} // namespace input
} // namespace slon

#endif // __SLON_ENGINE_INPUT_INPUT_MANAGER__

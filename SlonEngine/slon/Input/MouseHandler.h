#ifndef __SLON_ENGINE_INPUT_MOUSE_HANDLER_H__
#define __SLON_ENGINE_INPUT_MOUSE_HANDLER_H__

#include <boost/signals.hpp>
#include "InputHandler.h"

namespace slon {
namespace input {

/** MouseHandler uses signals to handle mice events.
 */
class MouseHandler :
    public InputHandler
{
public:
    typedef boost::signal<void (mouse_button)>  mouse_button_signal;
    typedef boost::signal<void (int, int)>      mouse_motion_signal;
    typedef boost::signals::connection          connection_type;

public:
    MouseHandler();

    /** Connect handler of the button down event.
     * @param button - pressed button.
     * @param handler - event handler.
     * @return connection of the handler used to disconnect it.
     */
    connection_type connectMouseButtonDownEventHandler(mouse_button button, mouse_button_signal::slot_type handler);

    /** Connect handler of the any key down event.
     * @param handler - event handler.
     * @return connection of the handler used to disconnect it.
     */
    connection_type connectMouseButtonDownEventHandler(mouse_button_signal::slot_type handler);

    /** Connect handler of the button up event.
     * @param button - released button.
     * @param handler - event handler.
     * @return connection of the handler used to disconnect it.
     */
    connection_type connectMouseButtonUpEventHandler(mouse_button button, mouse_button_signal::slot_type handler);

    /** Connect handler of the any button up event.
     * @param handler - event handler.
     * @return connection of the handler used to disconnect it.
     */
    connection_type connectMouseButtonUpEventHandler(mouse_button_signal::slot_type handler);

    /** Connect handler of the mouse motion event.
     * @param handler - event handler. Accepts (x,y) - relative mouse motion in pixels.
     * @return connection of the handler used to disconnect it.
     */
    connection_type connectRelativeMouseMotionEventHandler(mouse_motion_signal::slot_type handler);

    /** Connect handler of the mouse motion event.
     * @param handler - event handler. Accepts (x,y) - screen position of the cursor in pixels.
     * @return connection of the handler used to disconnect it.
     */
    connection_type connectAbsoluteMouseMotionEventHandler(mouse_motion_signal::slot_type handler);

    /** Remove all handlers */
    void disconnectAllHandlers();

    // Override InputHandler
    void    update();
    void    handleEvent(const InputEvent& event);
    double  getLastEventTime() const { return lastEventTime; }

private:
    // signals
    mouse_button_signal anyMouseButtonDownSignal;
    mouse_button_signal anyMouseButtonUpSignal;
    mouse_button_signal mouseButtonDownSignal[MBUTTON_LAST];
    mouse_button_signal mouseButtonUpSignal[MBUTTON_LAST];

    mouse_motion_signal relativeMouseMotionSignal;
    mouse_motion_signal absoluteMouseMotionSignal;

    // settings
    mutable double lastEventTime;
};

typedef boost::intrusive_ptr<MouseHandler>          mouse_handler_ptr;
typedef boost::intrusive_ptr<const MouseHandler>    const_mouse_handler_ptr;

} // namespace input
} // namespace slon

#endif // __SLON_ENGINE_INPUT_MOUSE_HANDLER_H__

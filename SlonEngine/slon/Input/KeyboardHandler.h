#ifndef __SLON_ENGINE_INPUT_KEYBOARD_HANDLER_H__
#define __SLON_ENGINE_INPUT_KEYBOARD_HANDLER_H__

#include <boost/signals.hpp>
#include <set>
#include "InputHandler.h"

namespace slon {
namespace input {

/** KeyboardHandler uses signals to handle key events.
 */
class SLON_PUBLIC KeyboardHandler :
    public InputHandler
{
private:
    typedef std::set<key_sym>                           key_set;

public:
    typedef boost::signal<void (key_sym, key_modifier)> keyboard_signal;
    typedef boost::signals::connection                  connection_type;

public:
    KeyboardHandler();

    /** Connect handler of the key down event. Calls handler every frame.
     * @param key - pressed key.
     * @param handler - event handler.
     * @return connection of the handler used to disconnect it.
     */
    connection_type connectKeyDownHandler(key_sym key, keyboard_signal::slot_type handler);

    /** Connect handler of the any key down event. Calls handler every frame.
     * @param handler - event handler.
     * @return connection of the handler used to disconnect it.
     */
    connection_type connectKeyDownHandler(keyboard_signal::slot_type handler);

    /** Connect handler of the key down event.
     * @param key - pressed key.
     * @param handler - event handler.
     * @return connection of the handler used to disconnect it.
     */
    connection_type connectKeyDownEventHandler(key_sym key, keyboard_signal::slot_type handler);

    /** Connect handler of the any key down event.
     * @param handler - event handler.
     * @return connection of the handler used to disconnect it.
     */
    connection_type connectKeyDownEventHandler(keyboard_signal::slot_type handler);

    /** Connect handler of the key press event.
     * @param key - pressed key.
     * @param handler - event handler.
     * @return connection of the handler used to disconnect it.
     * @see setMaxKeyPressDuration, getMassKeyPressDuration
     */
    connection_type connectKeyPressEventHandler(key_sym key, keyboard_signal::slot_type handler);

    /** Connect handler of the any key press event.
     * @param handler - event handler.
     * @return connection of the handler used to disconnect it.
     * @see setMaxKeyPressDuration, getMassKeyPressDuration
     */
    connection_type connectKeyPressEventHandler(keyboard_signal::slot_type handler);

    /** Connect handler of the key up event.
     * @param key - released key.
     * @param handler - event handler.
     * @return connection of the handler used to disconnect it.
     */
    connection_type connectKeyUpEventHandler(key_sym key, keyboard_signal::slot_type handler);

    /** Connect handler of the any key up event.
     * @param handler - event handler.
     * @return connection of the handler used to disconnect it.
     */
    connection_type connectKeyUpEventHandler(keyboard_signal::slot_type handler);

    /** Remove all handlers */
    void disconnectAllHandlers();

    /** Set maximum time interval beetween KEY_DOWN, KEY_UP events in the case
     * they generate KEY_PRESS event. Default is 1 second.
     * @param _keyPressDuration - time in seconds.
     */
    void setMaxKeyPressDuration(double _maxKeyPressDuration) { maxKeyPressDuration = _maxKeyPressDuration; }

    /** Get maximum time interval beetween KEY_DOWN, KEY_UP events in the case
     * they generate KEY_PRESS event. Default is 1 second.
     * @return time in seconds.
     */
    double getMaxKeyPressDuration() const { return maxKeyPressDuration; }

    /** Get time whether specified key was pressed.
     * @return time the key is pressed from the simulation beginning in seconds. -1.0f means key is not pressed now.
     */
    double getKeyPressTime(key_sym key) const { return keyPressTime[key]; }

    /** Check wether specified key is pressed */
    bool isKeyDown(key_sym key) const { return keyPressTime[key] > 0.0f; }

    /** Get current key modifier */
    key_modifier getKeyModifier() const { return activeModifier; }

    // Override InputHandler
    void    update();
    void    handleEvent(const InputEvent& event);
    double  getLastEventTime() const { return lastEventTime; }

private:
    // signals
    keyboard_signal     anyKeyDownContiniousSignal;
    keyboard_signal     anyKeyDownSignal;
    keyboard_signal     anyKeyUpSignal;
    keyboard_signal     anyKeyPressSignal;
    keyboard_signal     keyDownContiniousSignal[KEY_LAST];
    keyboard_signal     keyPressSignal[KEY_LAST];
    keyboard_signal     keyDownSignal[KEY_LAST];
    keyboard_signal     keyUpSignal[KEY_LAST];

    // settings
    double          maxKeyPressDuration;
    double          lastEventTime;
    double          keyPressTime[KEY_LAST];
    key_modifier    activeModifier;
    key_set         activeKeys;
};

typedef boost::intrusive_ptr<KeyboardHandler>          keyboard_handler_ptr;
typedef boost::intrusive_ptr<const KeyboardHandler>    const_keyboard_handler_ptr;

} // namespace input
} // namespace slon

#endif // __SLON_ENGINE_INPUT_KEYBOARD_HANDLER_H__

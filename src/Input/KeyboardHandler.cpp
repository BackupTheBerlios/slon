#include "stdafx.h"
#include "Engine.h"
#include "Input/KeyboardHandler.h"

using namespace slon;
using namespace input;

KeyboardHandler::KeyboardHandler() :
    maxKeyPressDuration(1.0f),
    lastEventTime(0.0f)
{
    std::fill(keyPressTime, keyPressTime + KEY_LAST, -1.0f);
}

KeyboardHandler::connection_type KeyboardHandler::connectKeyDownHandler(key_sym key, keyboard_signal::slot_type handler)
{
    return keyDownContiniousSignal[key].connect(handler);
}

KeyboardHandler::connection_type KeyboardHandler::connectKeyDownHandler(keyboard_signal::slot_type handler)
{
    return anyKeyDownContiniousSignal.connect(handler);
}

KeyboardHandler::connection_type KeyboardHandler::connectKeyDownEventHandler(key_sym key, keyboard_signal::slot_type handler)
{
    return keyDownSignal[key].connect(handler);
}

KeyboardHandler::connection_type KeyboardHandler::connectKeyDownEventHandler(keyboard_signal::slot_type handler)
{
    return anyKeyDownSignal.connect(handler);
}

KeyboardHandler::connection_type KeyboardHandler::connectKeyPressEventHandler(key_sym key, keyboard_signal::slot_type handler)
{
    return keyPressSignal[key].connect(handler);
}

KeyboardHandler::connection_type KeyboardHandler::connectKeyPressEventHandler(keyboard_signal::slot_type handler)
{
    return anyKeyPressSignal.connect(handler);
}

KeyboardHandler::connection_type KeyboardHandler::connectKeyUpEventHandler(key_sym key, keyboard_signal::slot_type handler)
{
    return keyUpSignal[key].connect(handler);
}

KeyboardHandler::connection_type KeyboardHandler::connectKeyUpEventHandler(keyboard_signal::slot_type handler)
{
    return anyKeyUpSignal.connect(handler);
}

void KeyboardHandler::disconnectAllHandlers()
{
    anyKeyUpSignal.disconnect_all_slots();
    anyKeyDownSignal.disconnect_all_slots();
    anyKeyPressSignal.disconnect_all_slots();
    std::for_each( keyDownSignal,  keyDownSignal  + KEY_LAST, boost::mem_fn(&keyboard_signal::disconnect_all_slots) );
    std::for_each( keyUpSignal,    keyUpSignal    + KEY_LAST, boost::mem_fn(&keyboard_signal::disconnect_all_slots) );
    std::for_each( keyPressSignal, keyPressSignal + KEY_LAST, boost::mem_fn(&keyboard_signal::disconnect_all_slots) );
}

void KeyboardHandler::update()
{
    for (key_set::iterator iter  = activeKeys.begin();
                           iter != activeKeys.end();
                           ++iter)
    {
        anyKeyDownContiniousSignal(*iter, activeModifier);
        keyDownContiniousSignal[*iter](*iter, activeModifier);
    }
}

void KeyboardHandler::handleEvent(const InputEvent& event)
{
    if ( const KeyboardEvent* keyboardEvent = event.asKeyboardEvent() )
    {
        lastEventTime = Engine::Instance()->getSimulationTimer().getTime();

        activeModifier = keyboardEvent->modifier;
        if (keyboardEvent->state == KeyboardEvent::DOWN)
        {
            activeKeys.insert(keyboardEvent->key);
            keyPressTime[keyboardEvent->key] = lastEventTime;
            anyKeyDownSignal(keyboardEvent->key, keyboardEvent->modifier);
            keyDownSignal[keyboardEvent->key](keyboardEvent->key, keyboardEvent->modifier);
        }
        else
        {
            activeKeys.erase(keyboardEvent->key);
            if ( keyPressTime[keyboardEvent->key] - lastEventTime < maxKeyPressDuration )
            {
                anyKeyPressSignal(keyboardEvent->key, keyboardEvent->modifier);
                keyPressSignal[keyboardEvent->key](keyboardEvent->key, keyboardEvent->modifier);
            }
            keyPressTime[keyboardEvent->key] = -1.0f;
            anyKeyUpSignal(keyboardEvent->key, keyboardEvent->modifier);
            keyUpSignal[keyboardEvent->key](keyboardEvent->key, keyboardEvent->modifier);
        }
    }
}

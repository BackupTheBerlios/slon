#include "stdafx.h"
#include "Engine.h"
#include "Input/MouseHandler.h"

using namespace slon;
using namespace input;

MouseHandler::MouseHandler() :
    lastEventTime(0.0f)
{
}

MouseHandler::connection_type MouseHandler::connectMouseButtonDownEventHandler(mouse_button button, mouse_button_signal::slot_type handler)
{
    return mouseButtonDownSignal[button].connect(handler);
}

MouseHandler::connection_type MouseHandler::connectMouseButtonDownEventHandler(mouse_button_signal::slot_type handler)
{
    return anyMouseButtonDownSignal.connect(handler);
}

MouseHandler::connection_type MouseHandler::connectMouseButtonUpEventHandler(mouse_button button, mouse_button_signal::slot_type handler)
{
    return mouseButtonUpSignal[button].connect(handler);
}

MouseHandler::connection_type MouseHandler::connectMouseButtonUpEventHandler(mouse_button_signal::slot_type handler)
{
    return anyMouseButtonUpSignal.connect(handler);
}

MouseHandler::connection_type MouseHandler::connectRelativeMouseMotionEventHandler(mouse_motion_signal::slot_type handler)
{
    return relativeMouseMotionSignal.connect(handler);
}

MouseHandler::connection_type MouseHandler::connectAbsoluteMouseMotionEventHandler(mouse_motion_signal::slot_type handler)
{
    return absoluteMouseMotionSignal.connect(handler);
}

void MouseHandler::disconnectAllHandlers()
{
    anyMouseButtonDownSignal.disconnect_all_slots();
    anyMouseButtonDownSignal.disconnect_all_slots();
    relativeMouseMotionSignal.disconnect_all_slots();
    absoluteMouseMotionSignal.disconnect_all_slots();
    std::for_each( mouseButtonDownSignal, mouseButtonDownSignal + MBUTTON_LAST, boost::mem_fn(&mouse_button_signal::disconnect_all_slots) );
    std::for_each( mouseButtonUpSignal, mouseButtonUpSignal + MBUTTON_LAST, boost::mem_fn(&mouse_button_signal::disconnect_all_slots) );
}

void MouseHandler::update()
{
}

void MouseHandler::handleEvent(const InputEvent& event)
{
    if ( const MouseEvent* mouseEvent = event.asMouseEvent() )
    {
        lastEventTime = Engine::Instance()->getSimulationTimer().getTime();

        if (mouseEvent->button != MBUTTON_NONE)
        {
            if (mouseEvent->buttonState == MouseEvent::DOWN)
            {
                anyMouseButtonDownSignal(mouseEvent->button);
                mouseButtonDownSignal[mouseEvent->button](mouseEvent->button);
            }
            else
            {
                anyMouseButtonUpSignal(mouseEvent->button);
                mouseButtonUpSignal[mouseEvent->button](mouseEvent->button);
            }
        }
        else
        {
            absoluteMouseMotionSignal(mouseEvent->x, mouseEvent->y);
            relativeMouseMotionSignal(mouseEvent->xrel, mouseEvent->yrel);
        }
    }
}

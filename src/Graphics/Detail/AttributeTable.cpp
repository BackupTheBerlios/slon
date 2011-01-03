#include "stdafx.h"
#include "Graphics/Detail/GraphicsManager.h"

namespace slon {
namespace graphics {
namespace detail {

AttributeTable::AttributeTable()
{
    std::fill(bindings, bindings + MAX_NUM_ATTRIBUTES, (binding*)0);
}

AttributeTable::binding_ptr AttributeTable::queryAttribute(unique_string name)
{
    // find attribute
    for (int i = 0; i<MAX_NUM_ATTRIBUTES; ++i)
    {
        if (bindings[i] && bindings[i]->name == name) {
            return binding_ptr(bindings[i]);
        }
    }

    // reques new attribute
    for (int i = 0; i<MAX_NUM_ATTRIBUTES; ++i)
    {
        if (!bindings[i]) 
        {
            bindings[i] = new binding(name, i, this);
            return binding_ptr(bindings[i]);
        }
    }

    // can't request attribute
    return binding_ptr();
}

void AttributeTable::removeBinding(binding* attr)
{
    assert(attr);
    bindings[attr->index] = 0;
}

AttributeTable& currentAttributeTable()
{
    return static_cast<detail::GraphicsManager&>( currentGraphicsManager() ).getAttributeTable();
}

} // namespace detail
} // namespace graphics
} // namespace slon

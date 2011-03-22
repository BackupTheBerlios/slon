#ifndef __SLON_ENGINE_GRAPHICS_STATE_TABLE_H__
#define __SLON_ENGINE_GRAPHICS_STATE_TABLE_H__

#include "../../Utility/referenced.hpp"
#include <boost/intrusive_ptr.hpp>

namespace slon {
namespace graphics {

class StateTable :
    public Referenced
{

};

typedef boost::intrusive_ptr<StateTable>        state_table_ptr;
typedef boost::intrusive_ptr<const StateTable>  const_state_table_ptr;

/** Get AttributeTable used by current GraphicsManager. */
StateTable& currentStateTable();

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_STATE_TABLE_H__

#ifndef __SLON_ENGINE_GRAPHICS_STATE_TABLE_H__
#define __SLON_ENGINE_GRAPHICS_STATE_TABLE_H__

#include "../../Realm/Object.h"
#include "../../Scene/Node.h"
#include "../../Scene/Visitors/NodeVisitor.h"
#include "../../Utility/callback.hpp"

namespace slon {
namespace graphics {
namespace detail {

class RenderableGatherer :
	public realm::object_const_callback
{
public:
	bool operator () (const realm::Object& object)
	{
		cv.traverse(*object.getRoot());
		return false;
	}

public:
	scene::CullVisitor cv;
};

} // namespace detail
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_STATE_TABLE_H__

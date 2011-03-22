#include "stdafx.h"
#include "Graphics/Detail/GraphicsManager.h"

namespace slon {
namespace graphics {
namespace detail {

ParameterTable& currentParameterTable()
{
    return static_cast<detail::GraphicsManager&>( currentGraphicsManager() ).getParameterTable();
}

} // namespace detail
} // namespace graphics
} // namespace slon

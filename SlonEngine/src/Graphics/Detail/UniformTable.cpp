#include "stdafx.h"
#include "Graphics/Detail/GraphicsManager.h"

namespace slon {
namespace graphics {
namespace detail {

UniformTable& currentUniformTable()
{
    return static_cast<detail::GraphicsManager&>( currentGraphicsManager() ).getUniformTable();
}

} // namespace detail
} // namespace graphics
} // namespace slon

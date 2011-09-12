#ifndef __SLON_ENGINE_GRAPHICS_COMMON_H__
#define __SLON_ENGINE_GRAPHICS_COMMON_H__

#include "../Config.h"
#include <sgl/Device.h>

namespace slon {

// Forward decl
class Engine;

namespace graphics {

/** Get sgl device used by engine */
SLON_PUBLIC sgl::Device* currentDevice();

/** Check error based on the sgl result */
void checkShaderError(const std::string& fileName, sgl::SGL_HRESULT err);

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_COMMON_H__

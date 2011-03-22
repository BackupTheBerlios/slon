#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/GraphicsManager.h"
#include "Utility/error.hpp"

using namespace slon;

namespace {
    log::Logger logger("graphics");
}

namespace slon {
namespace graphics {

sgl::Device* currentDevice()
{
    return currentGraphicsManager().sglDevice();
}

void checkShaderError(const std::string& fileName, sgl::SGL_HRESULT err)
{
    switch(err)
    {
    case sgl::SGLERR_FILE_NOT_FOUND:
        throw file_not_found_error(logger, "Couldn't find file: " + fileName);
    case sgl::SGLERR_INVALID_CALL:
        throw shader_error(logger, fileName + ":" + sglGetErrorMsg() );
    default:
        return;
    }
}

} // namespac graphics
} // namespace slon

#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/GraphicsManager.h"
#include "Utility/error.hpp"

using namespace slon;

DECLARE_AUTO_LOGGER("graphics")

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
        throw file_error(AUTO_LOGGER, "Couldn't find file: " + fileName);
    case sgl::SGLERR_INVALID_CALL:
        throw shader_error(AUTO_LOGGER, fileName + ":" + sglGetErrorMsg() );
    default:
        return;
    }
}

} // namespace graphics
} // namespace slon

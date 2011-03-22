#ifndef SLON_ENGINE_GRAPHICS_GUI_PROGRESS_BAR_H
#define SLON_ENGINE_GRAPHICS_GUI_PROGRESS_BAR_H

#include <memory>
#include <string>
#include <sgl/Device.h>
#include <sgl/Utility/StateSet.h>
#include <sgl/Math/Matrix.hpp>

namespace slon {
namespace graphics {
namespace gui {

class ProgressBar
{
public:
    ProgressBar(sgl::Device* _pDevice);
    virtual ~ProgressBar();

    void SetPosition(int x, int y);
    void SetSize(int width, int height);

    /** Set color of the bar
     * @param color of the bar. The last parameter is opacity
     */
    void SetColor(float r, float g, float b, float a);

    /** Color of the border */
    void SetBorderColor(float r, float g, float b, float a);

    /** Set font of the button */
    void SetFont(sgl::Font* _font);

    /** Set text of the progresss bar */
    void SetText(const std::string& text);

    /** Set the value of the progress bar in the [0,1] segment */
    void SetValue(float value);

    void Draw() const;

protected:
private:
    // graphics
    sgl::ref_ptr<sgl::Device>               pDevice;
    sgl::ref_ptr<sgl::VertexBuffer>         vbo;
    sgl::ref_ptr<sgl::BlendState>           blendState;
    sgl::ref_ptr<sgl::VertexColorState>     colorState;
    sgl::ref_ptr<sgl::TransformState>       projectionTransform;
    sgl::ref_ptr<sgl::TransformState>       modelviewTransform;
    sgl::ref_ptr<sgl::DepthState>           depthTestState;
    sgl::StateSet                           stateSet;

    // properties
    math::Vector2i     topLeft;
    math::Vector2i     size;
    math::Vector4f     color;
    math::Vector4f     borderColor;
    float               value;
    std::string         text;

    // font of the bar
    sgl::ref_ptr<sgl::Font>     font;
};

} // namesapce gui
} // namesapce graphics
} // namesapce slon

#endif // SLON_ENGINE_GRAPHICS_GUI_PROGRESS_BAR_H

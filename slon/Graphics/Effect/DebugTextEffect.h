#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_DEBUG_TEXT_EFFECT_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_DEBUG_TEXT_EFFECT_H__

#include "../../Utility/Memory/aligned_allocator.hpp"
#include "../../Utility/Memory/object_in_pool.hpp"
#include "../Effect.h"
#include "Pass.h"
#include <sgl/Font.h>

namespace slon {
namespace graphics {

/** Effect for rendering debug information */
class DebugTextEffect :
    public object_in_pool<DebugTextEffect, Effect, aligned_allocator<0x10> >
{
public:
    DebugTextEffect( const sgl::Font*      font  = 0, 
                     const math::Vector2i& size  = math::Vector2i(10, 12),
                     const math::Vector4f& color = math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f) );

    /** Setup model matrix for the effect */
    void setFont(const sgl::Font* font_) { font.reset(font_); }

    /** Setup projection matrix for the effect. */
    const sgl::Font* getFont() const { return font.get(); }

    /** Setup rendering color */
    void setColor(const math::Vector4f& color_) { color = color_; }

    /** Get render color */
    const math::Vector4f& getColor() const { return color; }

    /** Setup font size */
    void setSize(const math::Vector2i& size_) { size = size_; }

    /** Get render color */
    const math::Vector2i& getSize() const { return size; }

    // Override Effect
    int                               present(render_group_handle renderGroup, render_pass_handle renderPass, Pass** passes);
    int                               queryAttribute(unique_string /*name*/) { return -1; }
    const abstract_parameter_binding* getParameter(unique_string /*name*/) const { return 0; }
    bool                              bindParameter(unique_string                        /*name*/,
                                                    const abstract_parameter_binding*    /*binding*/) { return false; }

private:
    sgl::ref_ptr<const sgl::Font>   font;
    math::Vector2i                  size;
    math::Vector4f                  color;
    pass_ptr                        pass;
};

typedef boost::intrusive_ptr<DebugTextEffect>       debug_text_effect_ptr;
typedef boost::intrusive_ptr<const DebugTextEffect> const_debug_text_effect_ptr;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_EFFECT_DEBUG_TEXT_EFFECT_H__

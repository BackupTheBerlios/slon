#include "stdafx.h"
#include "Graphics/Effect/DebugTextEffect.h"
#include "Graphics/Renderer/ForwardRenderer.h"
#include "Graphics/Renderer/FixedPipelineRenderer.h"
#include "Log/Logger.h"

__DEFINE_LOGGER__("graphics.DebugTextEffect")

namespace {

    using namespace slon;
    using namespace graphics;

    static const slon::hash_string DebugPass = slon::hash_string("DebugPass");

    class TextPass :
        public Pass
    {
    public:
        TextPass(DebugTextEffect* debugEffect_) :
            debugEffect(debugEffect_)
        {
        }

        // Override Pass
        long long getPriority() const 
        { 
            return (long long)(this); 
        }
        
        void begin() const
        {
            debugEffect->getFont()->Bind( debugEffect->getSize().x,
                                          debugEffect->getSize().y,
                                          debugEffect->getColor() );
        }

        void end() const
        {
            debugEffect->getFont()->Unbind();
        }

    private:
        DebugTextEffect* debugEffect;
    };

} // anonymous namespace

namespace slon {
namespace graphics {

// Debug effect
DebugTextEffect::DebugTextEffect(const sgl::Font*      font_, 
                                 const math::Vector2i& size_,
                                 const math::Vector4f& color_) :
    font(font_),
    size(size_),
    color(color_)
{
    pass.reset( new TextPass(this) );
}

int DebugTextEffect::present(render_group_handle /*renderGroup*/, render_pass_handle renderPass, Pass** passes)
{
    if (renderPass == detail::ForwardRenderer::RP_DEBUG || renderPass == detail::FixedPipelineRenderer::RP_DEBUG)
    {
        passes[0] = pass.get();
        return 1;
    }

    return 0;
}

} // namesapce slon
} // namespace graphics

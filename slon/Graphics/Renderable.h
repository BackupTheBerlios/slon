#ifndef SLON_ENGINE_GRAPHICS_RENDERABLE_H
#define SLON_ENGINE_GRAPHICS_RENDERABLE_H

namespace slon {
namespace graphics {

class Effect;

/** Base for drawable objects.
 * Renderable contains geometry that will be rendered to the device
 * and effect. Effect usually contains state set and shaders. During
 * render pass renderables will be sorted to reduce effect changes.
 */
class Renderable
{
public:
    /** Render object to the device. In the SlonEngine this means only
     * to render the geometry and set up only necessary settings.
     * Effects, render targets, states, etc must be set somewhere else.
     */
    virtual void render() const = 0;

    /** Get effect(material) of the surface */
    virtual graphics::Effect* getEffect() const = 0;

    virtual ~Renderable() {}
};

} // namespace graphics
} // namespace slon

#endif // SLON_ENGINE_GRAPHICS_RENDERABLE_H

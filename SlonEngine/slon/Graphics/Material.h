#ifndef __SLON_ENGINE_GRAPHICS_MATERIAL_H__
#define __SLON_ENGINE_GRAPHICS_MATERIAL_H__

#include "Effect.h"

namespace slon {
namespace graphics {

/** Materials provide mechanism for mapping same material
 * onto different geometry types. For example, you can make wood material
 * and map it onto rigid and onto skinned geometry. 
 */
class Material :
    public Referenced
{
public:
    /** Create effect for attaching onto geometry. Make different effect
     * for every geometry.
     * @return effect for rendering geometry with this material.
     */
    virtual Effect* createEffect() const = 0;

    virtual ~Material() {}
};

typedef boost::intrusive_ptr<Material>          material_ptr;
typedef boost::intrusive_ptr<const Material>    const_material_ptr;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_MATERIAL_H__

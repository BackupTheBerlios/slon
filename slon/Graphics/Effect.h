#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_H__

#include "../Utility/unique_string.hpp"
#include "ParameterBinding.h"

namespace slon {
namespace graphics {

// Forward
class Pass;

typedef unique_string render_group_handle;
typedef unique_string render_pass_handle;
typedef unique_string parameter_handle;
typedef unique_string attribute_handle;

/** Effect unify effects that share states and(or) shaders.
 * Traditionally rendering can be represented as followed:
 * \code
 * if ( Pass* pass = effect->present(renderPassName) )
 * {
 *     pass->begin();      // begin using effect, remember states
 *     someDrawable->render();
 *     pass->end();        // restore states
 * }
 * \uncode
 */
class Effect :
    public Referenced
{
public:
    static const int MAX_NUM_PASSES = 5; /// maximum number of passes allowed for effect per render pass

public:
    /** Present effect in the render pass. Get technique used to render objects. 
     * Render passes are grouped in render groups for convinience and some performance speed up.
     * E.g. ForwardRenderer have render group "Reflect" for rendering scene into reflect texture with
     * simplified states and front face culling.
     * @param renderGroup - current render pass group.
     * @param renderPass - current render pass.
     * @param passes [out] - array of the techniques for rendering object
     * @return number of passes for rendering object.
     */
    virtual int present(render_group_handle renderGroup, render_pass_handle renderPass, Pass** passes) = 0;

    /** Get parameter binding.
     * @see bindParameter
     */
    virtual const abstract_parameter_binding* getParameter(parameter_handle name) const = 0;

    /** Bind material parameter parameter to arbitrary get function.
     * @param name - parameter name.
     * @param binder - parameter binding.
     * List of default semantics:
     * \li \c ("worldMatrix", Matrix4x4f&) - object world matrix.
	 * \li \c ("boneMatrices", Matrix4x4f*) - bone transformation matrices.
	 * \li \c ("boneRotations", Vector4f*) - bone rotation quaternions.
	 * \li \c ("boneTranslations", Vector3f*) - bone translations.
     * \li \c ("projectedGridCorners", Vector3f*) - four corners projected on the plane using projected grid.
     */
    virtual bool bindParameter(parameter_handle                     name,
                               const abstract_parameter_binding*    binding) = 0;

    /** Get index of the named attribute. You must bind all the queried attributes,
     * otherwise effect behaviour will be undefined. May change effect behaviour so
     * it is not const.
     * @return attribute index or -1 if effect doesn't support queried attribute.
     * List of default attributes:
     * \li \c ("position", Vector4f)
	 * \li \c ("weights", Vector4f)
     * \li \c ("normal", Vector3f)
     * \li \c ("tangent", Vector3f)
     * \li \c ("binormal", Vector3f)
     * \li \c ("texcoord", Vector2f)
     */
    virtual int queryAttribute(attribute_handle name) = 0;

    virtual ~Effect() {}
};

typedef boost::intrusive_ptr<Effect>            effect_ptr;
typedef boost::intrusive_ptr<const Effect>      const_effect_ptr;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_EFFECT_H__

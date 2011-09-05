#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_H__

#include "../Database/Serializable.h"
#include "../Utility/hash_string.hpp"
#include "ParameterBinding.h"

namespace slon {
namespace graphics {

// Forward
class Pass;

typedef unsigned	render_group_handle;
typedef unsigned	render_pass_handle;
typedef hash_string parameter_handle;
typedef hash_string attribute_handle;

/** Effect unify effects that share states and(or) shaders.
 * Traditionally rendering can be represented as followed:
 * \code
 * Pass* passes[Effect::MAX_NUM_PASSES];
 * int nPasses = effect->present(renderGroup, renderPass, passes);
 * for (int i = 0; i<nPasses; ++i)
 * {
 *     passes[i]->begin();      // begin using effect, remember states
 *     someDrawable->render();
 *     passes[i]->end();        // restore states
 * }
 * \uncode
 */
class SLON_PUBLIC Effect :
    public Referenced,
    public database::Serializable
{
public:
    static const int MAX_NUM_PASSES = 5; /// maximum number of passes allowed for effect per render pass

public:
    // Override serializable
    const char* serialize(database::OArchive& ar) const { return "Effect"; }
    void deserialize(database::IArchive& ar)            {}

    /** Present effect in the render pass. Get technique used to render objects. 
     * Render passes are grouped in render groups for convenience and some performance speed up.
     * E.g. ForwardRenderer have render group "Reflect" for rendering scene into reflect texture with
     * simplified states and front face culling.
     * @param renderGroup - current render group index.
     * @param renderPass - current render pass index.
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

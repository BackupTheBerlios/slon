#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_DETAIL_EFFECT_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_DETAIL_EFFECT_H__

#include <map>
#include "../Effect.h"
#include "Pass.h"

namespace slon {
namespace graphics {
namespace detail {

/* Technique performing object lighting using shaders */
class Effect :
    public graphics::Effect
{
private:
    typedef std::vector<graphics::pass_ptr>			pass_vector;
    typedef std::map<hash_string, binding_ptr>      parameter_map;
    typedef parameter_map::iterator                 parameter_iterator;
    typedef parameter_map::const_iterator           parameter_const_iterator;

public:
    /** Add parameter to the technique. If parameter exist - replace it,
     * rebind uniforms to that parameter.
     */
    void setParameter(hash_string name, abstract_parameter_binding* parameter);

    /** Remove parameter from the technique. Unbind uniforms from that parameter.
     * @return true - if parameter removed, false - if not found.
     */
    bool removeParameter(hash_string name);

    /** Find technique parameter by the name */
    const abstract_parameter_binding* getParameter(hash_string name) const;

    /** Add pass to the technique. If pass with same name already exist -
     * replace that pass.
     */
    void addPass(hash_string name, graphics::Pass* pass);

    /** Remove pass from the technique.
     * @return true - if pass suyccesfully removed, false - if not found.
     */
    bool removePass(hash_string name);

    // Override Effect
    int present(render_group_handle renderGroup, render_pass_handle renderPass, graphics::Pass** passes);

private:
    parameter_map	parameters;
	pass_vector		passes;
};

} // namespace detail
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_EFFECT_DETAIL_EFFECT_H__

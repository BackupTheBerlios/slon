#include "stdafx.h"
#include "Graphics/Effect/Detail/Effect.h"

namespace slon {
namespace graphics {
namespace detail {

void Effect::setParameter(unique_string name, abstract_parameter_binding* parameter)
{
    assert(0);
}

bool Effect::removeParameter(unique_string name)
{
    assert(0);
    return false;
}

const abstract_parameter_binding* Effect::getParameter(unique_string name) const
{
    parameter_const_iterator paramIter = parameters.find(name);
    if ( paramIter != parameters.end() ) {
        return paramIter->second.get();
    }

    return 0;
}

void Effect::addPass(unique_string name, graphics::Pass* pass)
{
    pass_iterator passIter = passes.find(name);
    if ( passIter != passes.end() ) {
        passIter->second = graphics::pass_ptr(pass);
    }
    else {
        passes.insert( pass_map::value_type(name, graphics::pass_ptr(pass)) );
    }
}

bool Effect::removePass(unique_string name)
{
    return passes.erase(name) != 0;
}

int Effect::present(render_group_handle /*renderGroup*/, render_pass_handle renderPass, graphics::Pass** p)
{
    pass_iterator passIter = passes.find(renderPass);
    if ( passIter != passes.end() )
    {
        p[0] = passIter->second.get();
        return 1;
    }

    return 0;
}

} // namespace detail
} // namespace graphics
} // namespace slon

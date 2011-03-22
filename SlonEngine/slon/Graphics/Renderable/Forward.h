#ifndef __SLON_ENGINE_RENDERABLE_FORWARD_H__
#define __SLON_ENGINE_RENDERABLE_FORWARD_H__

namespace slon {
namespace graphics {

/** For convinience engine renderables store attributes in the
 * stages defined by their semantic.
 */
enum ATTRIBUTE_SEMANTIC 
{
    ATTRIBUTE_POSITION = 0,
    ATTRIBUTE_NORMAL   = 1,
    ATTRIBUTE_TANGENT  = 2,
    ATTRIBUTE_BINORMAL = 3,
    ATTRIBUTE_COLOR    = 4,
    ATTRIBUTE_WEIGHTS  = 5,
};

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_RENDERABLE_FORWARD_H__
#ifndef __SLON_ENGINE_SCENE_VISITORS_TRAVERSE_HPP_
#define __SLON_ENGINE_SCENE_VISITORS_TRAVERSE_HPP_

#include "../../Utility/if_then_else.hpp"
#include "../Group.h"
#include <boost/type_traits/is_convertible.hpp>

namespace slon {
namespace scene {

/** traverse scene graph using depth first search. Call func for every traversed node. */
template<typename NodeT, typename Func>
void traverseDFS(NodeT& root, 
                 Func   func,
                 typename boost::is_convertible<NodeT&, const scene::Node&>::type* tag = 0)
{
    typedef typename if_then_else< boost::is_const<NodeT>::value,
                                   const Group,
                                   Group >::type group_type;

    NodeT* node = &root;
    while (node)
    {
        func(*node);

        if (node->getNodeType() & scene::Node::GROUP) {
            node = static_cast<group_type*>(node)->getChild();
        }
        else if ( node->getRight() ) {
            node = node->getRight();
        }
        else
        {
            node = node->getParent();
            if (!node) {
                return;
            }

            while ( node != &root && !node->getRight() ) {
                node = node->getParent();
            }

            if (node == &root) {
                return;
            }
            else {
                node = node->getRight();
            }
        }
    }
}

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_VISITORS_TRAVERSE_HPP_

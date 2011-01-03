#include "../Config.h"

namespace boost
{
    template<typename T>
    class intrusive_ptr;
} // namespace boost

namespace slon {
namespace scene {

    // forward types
    class Entity;
    class Geode;
    class Group;
    class Light;
    class MatrixTransform;
    class Node;
    class Transform;

    class CullVisitor;
    class NodeVisitor;
    class TraverseVisitor;

    // common ptr typedefs
    typedef boost::intrusive_ptr<Entity>                entity_ptr;
    typedef boost::intrusive_ptr<const Entity>          const_entity_ptr;
    typedef boost::intrusive_ptr<Geode>                 geode_ptr;
    typedef boost::intrusive_ptr<const Geode>           const_geode_ptr;
    typedef boost::intrusive_ptr<Group>                 group_ptr;
    typedef boost::intrusive_ptr<const Group>           const_group_ptr;
    typedef boost::intrusive_ptr<Light>                 light_ptr;
    typedef boost::intrusive_ptr<const Light>           const_light_ptr;
    typedef boost::intrusive_ptr<MatrixTransform>       matrix_transform_ptr;
    typedef boost::intrusive_ptr<const MatrixTransform> const_matrix_transform_ptr;
    typedef boost::intrusive_ptr<Node>                  node_ptr;
    typedef boost::intrusive_ptr<const Node>            const_node_ptr;
    typedef boost::intrusive_ptr<Transform>             transform_ptr;
    typedef boost::intrusive_ptr<const Transform>       const_transform_ptr;

    typedef boost::intrusive_ptr<CullVisitor>           cull_visitor_ptr;
    typedef boost::intrusive_ptr<const CullVisitor>     const_cull_visitor_ptr;
    typedef boost::intrusive_ptr<NodeVisitor>           node_visitor_ptr;
    typedef boost::intrusive_ptr<const NodeVisitor>     const_node_visitor_ptr;   
    typedef boost::intrusive_ptr<TraverseVisitor>       traverse_visitor_ptr;
    typedef boost::intrusive_ptr<const TraverseVisitor> const_traverse_visitor_ptr;

} // namespace scene
} // namespace slon

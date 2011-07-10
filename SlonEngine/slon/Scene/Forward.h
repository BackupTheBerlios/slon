#ifndef __SLON_ENGINE_SCENE_FORWARD_H__
#define __SLON_ENGINE_SCENE_FORWARD_H__

#include "../Config.h"

namespace boost
{
    template<typename T>
    class intrusive_ptr;
} // namespace boost

namespace slon {
namespace scene {

    // forward types
    class Camera;
    class Entity;
    class Geode;
    class Group;
    class Joint;
    class Light;
    class MatrixTransform;
    class Node;
    class Skeleton;
    class Transform;
    
    template<typename Visitor>
    class AcceptVisitor;

    template<typename Visitor>
    class ConstAcceptVisitor;

    class Visitor;
    class ConstVisitor;

    template<typename Derived, typename Base>
    class VisitorImpl;

    class CullVisitor;
    class TransformVisitor;

    // common ptr typedefs
    typedef boost::intrusive_ptr<Camera>                camera_ptr;
    typedef boost::intrusive_ptr<const Camera>          const_camera_ptr;
    typedef boost::intrusive_ptr<Entity>                entity_ptr;
    typedef boost::intrusive_ptr<const Entity>          const_entity_ptr;
    typedef boost::intrusive_ptr<Geode>                 geode_ptr;
    typedef boost::intrusive_ptr<const Geode>           const_geode_ptr;
    typedef boost::intrusive_ptr<Group>                 group_ptr;
    typedef boost::intrusive_ptr<const Group>           const_group_ptr;
    typedef boost::intrusive_ptr<Joint>                 joint_ptr;
    typedef boost::intrusive_ptr<const Joint>           const_joint_ptr;
    typedef boost::intrusive_ptr<Light>                 light_ptr;
    typedef boost::intrusive_ptr<const Light>           const_light_ptr;
    typedef boost::intrusive_ptr<MatrixTransform>       matrix_transform_ptr;
    typedef boost::intrusive_ptr<const MatrixTransform> const_matrix_transform_ptr;
    typedef boost::intrusive_ptr<Node>                  node_ptr;
    typedef boost::intrusive_ptr<const Node>            const_node_ptr;
    typedef boost::intrusive_ptr<Skeleton>              skeleton_ptr;
    typedef boost::intrusive_ptr<const Skeleton>        const_skeleton_ptr;
    typedef boost::intrusive_ptr<Transform>             transform_ptr;
    typedef boost::intrusive_ptr<const Transform>       const_transform_ptr;

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_FORWARD_H__

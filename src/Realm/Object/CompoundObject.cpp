#include "stdafx.h"
#include "Log/Logger.h"
#include "Physics/RigidBodyTransform.h"
#include "Realm/Object/CompoundObject.h"
#include "Scene/Visitors/TraverseVisitor.h"

__DEFINE_LOGGER__("realm.CompoundObject")

namespace {

	using namespace slon;

	class DecomposeTransformVisitor :
		public scene::TraverseVisitor
	{
	public:
		void visitAbsoluteTransform(scene::Transform& transform)
		{
			physics::RigidBodyTransform* rbTransform = dynamic_cast<physics::RigidBodyTransform*>(&transform);
			if (rbTransform)
			{
				math::Matrix4f T( getLocalToWorldTransform() );
				math::Matrix4f R( rbTransform->getRigidBody()->getTransform() );

				switch ( rbTransform->getRigidBody()->getDynamicsType() )
				{
					case physics::RigidBody::DT_DYNAMIC:
						rbTransform->setTransform( math::invert(R) * T );
						break;
						
					case physics::RigidBody::DT_STATIC:
					case physics::RigidBody::DT_KINEMATIC:
						rbTransform->setTransform( math::invert(T) * R );
						break;

					default:
						assert(!"Can't get here");
				}
			}

			// Let traverse visitor to do rest of the work
			TraverseVisitor::visitAbsoluteTransform(transform);
		}
	};

} // anonymous namespace

namespace slon {
namespace realm {

CompoundObject::CompoundObject(scene::Node*             root_, 
                               bool                     dynamic_
#ifdef SLON_ENGINE_USE_PHYSICS
                               , physics::PhysicsModel* physicsModel_
#endif
                               ) :
    dynamic(dynamic_),
    alwaysUpdate(false)
{
    world       = 0;
    location    = 0;
    spatialNode = 0;

    root.reset(root_);
    if (root)
    {
        scene::TraverseVisitor visitor;
        visitor.traverse(*root);
        aabb = visitor.getBounds();
    }
    else {
        aabb.reset_max();
    }

#ifdef SLON_ENGINE_USE_PHYSICS
    setPhysicsModel(physicsModel_);
#endif
}

void CompoundObject::traverse(scene::NodeVisitor& nv)
{
    if (root)
    {
        if ( scene::TraverseVisitor* tv = dynamic_cast<scene::TraverseVisitor*>(&nv) )
        {
            tv->traverse(*root);
            aabb = tv->getBounds();
        }
        else
        {
            nv.traverse(*root);
        }
    }
}

void CompoundObject::setRoot(scene::Node* root_)
{
#ifdef SLON_ENGINE_USE_PHYSICS
    if (root && physicsModel) {
        clearPhysics( root.get() );
    }
#endif

    root.reset(root_);
    if (root)
    {
        scene::TraverseVisitor visitor;
        visitor.traverse(*root);
        aabb = visitor.getBounds();
    
    #ifdef SLON_ENGINE_USE_PHYSICS
        if (physicsModel) {
            setPhysicsModel( physicsModel.get() );
        }
    #endif
    }
    else {
        aabb.reset_max();
    }
}

#ifdef SLON_ENGINE_USE_PHYSICS
void CompoundObject::clearPhysics(scene::Node* node)
{
    if ( scene::Group* group = dynamic_cast<scene::Group*>(node) )
    {
        scene::Group::node_vector         children( group->firstChild(), group->endChild() );
        physics::rigid_body_transform_ptr rbTransform( dynamic_cast<physics::RigidBodyTransform*>(node) );
        if (rbTransform)
        {
            if ( scene::Group* parent = rbTransform->getParent() )
            {
                // relink children
                parent->removeChild(*rbTransform);
                while ( rbTransform->getNumChildren() > 0 ) {
                    rbTransform->moveChild( *parent, rbTransform->firstChild() );
                }
            }
            else
            {
                scene::Group* newParent = new scene::Group();
                while ( rbTransform->getNumChildren() > 0 ) {
                    rbTransform->moveChild( *newParent, rbTransform->firstChild() );
                }

                root.reset(newParent);
            }
        }

        // remove all rigid body transforms further
        for (scene::Group::node_iterator iter  = children.begin();
                                         iter != children.end();
                                         ++iter)
        {
            clearPhysics( iter->get() );
        }
    }
}

void CompoundObject::setPhysicsModel(physics::PhysicsModel* physicsModel_)
{
    using namespace physics;

    physicsModel.reset(physicsModel_);
    if (root)
    {
        clearPhysics( root.get() );
        
        // set transforms
        if (physicsModel)
        {
            for (PhysicsModel::rigid_body_iterator iter  = physicsModel->firstRigidBody();
                                                   iter != physicsModel->endRigidBody();
                                                   ++iter)
            {
                // insert rigid body into scene graph
                scene::node_ptr targetNode( findNamedNode( *root, (*iter)->getTarget() ) );
                if (targetNode)
                {
                    physics::RigidBodyTransform* rbTransform = (*iter)->getMotionState();
                    if ( scene::Group* group = dynamic_cast<scene::Group*>( targetNode.get() ) )
                    {
                        // relink children
                        while ( group->getNumChildren() > 0 ) {
                            group->moveChild( *rbTransform, group->firstChild() );
                        }

                        group->addChild(*rbTransform);
                    }
                    else if ( targetNode->getParent() != 0 )
                    {
                        targetNode->getParent()->addChild(*rbTransform);
                        targetNode->getParent()->moveChild(*rbTransform, *targetNode);
                    }
                    else 
                    {
                        rbTransform->addChild(*targetNode);
                        root.reset(rbTransform);
                    }
                }
                else {
                    logger << log::WL_WARNING << "Can't find node corresponding rigid body: " << (*iter)->getTarget() << std::endl;
                }
            }

            // apply local transform to the shapes
            DecomposeTransformVisitor visitor;
            visitor.traverse(*root);
        }
    }
}
#endif // SLON_ENGINE_USE_PHYSICS

} // namesapce realm
} // namespace slon
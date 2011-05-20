#include "stdafx.h"
#include "Log/Logger.h"
#include "Log/LogVisitor.h"
#include "Physics/PhysicsModel.h"
#include "Physics/RigidBodyTransform.h"
#include "Realm/Detail/Object.h"
#include "Scene/Visitors/DFSNodeVisitor.h"
#include "Scene/Visitors/FilterVisitor.h"
#include "Scene/Visitors/TransformVisitor.h"

DECLARE_AUTO_LOGGER("realm.Object")

namespace {

	using namespace slon;

	class DecomposeTransformVisitor :
        public scene::FilterVisitor<scene::DFSNodeVisitor, physics::RigidBodyTransform>
	{
	public:
        DecomposeTransformVisitor(scene::Node& node)
        {
            traverse(node);
        }

		void visit(physics::RigidBodyTransform& rbTransform)
		{
			math::Matrix4f T( rbTransform.getLocalToWorld() );
			math::Matrix4f R( rbTransform.getRigidBody()->getTransform() );

			rbTransform.setTransform( math::invert(R) * T ); // physics -> graphics 
			switch ( rbTransform.getRigidBody()->getDynamicsType() )
			{
			case physics::RigidBody::DT_STATIC:
			case physics::RigidBody::DT_DYNAMIC:
                rbTransform.setAbsolute(true);
				break;
				
			case physics::RigidBody::DT_KINEMATIC:
                rbTransform.setAbsolute(false);
				break;

			default:
				assert(!"Can't get here");
			}
		}
	};

} // anonymous namespace

namespace slon {
namespace realm {
namespace detail {

Object::Object(scene::Node*             root_, 
               bool                     dynamic_
#ifdef SLON_ENGINE_USE_PHYSICS
               , physics::PhysicsModel* physicsModel_
#endif
               ) :
    world(0),
    infinite(false),
	location(0),
	locationData(0),
    dynamic(dynamic_)
{
    AUTO_LOGGER_MESSAGE(log::S_FLOOD, "Creating compound object" << LOG_FILE_AND_LINE);
    setRoot(root_);
#ifdef SLON_ENGINE_USE_PHYSICS
    setPhysicsModel(physicsModel_);
#endif
}

Object::~Object()
{
    setRoot(0);
}

void Object::traverse(scene::NodeVisitor& nv)
{
    if (root)
    {
        if ( scene::TransformVisitor* tv = dynamic_cast<scene::TransformVisitor*>(&nv) )
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

void Object::traverse(scene::ConstNodeVisitor& nv) const
{
    if (root) {
        nv.traverse(*root);
    }
}

void Object::setRoot(scene::Node* root_)
{
    if (root)
    {
        root->setObject(0);
    #ifdef SLON_ENGINE_USE_PHYSICS
        if (physicsModel) 
        {
            physicsModel->toggleSimulation(false);
            clearPhysics( root.get() );
        }
    #endif
    }

    AUTO_LOGGER_MESSAGE(log::S_FLOOD, "Resetting root for compound object" << LOG_FILE_AND_LINE);
    root.reset(root_);
    if (root)
    {
        root->setObject(this);

        scene::TransformVisitor visitor(*root);
        aabb = visitor.getBounds();
    
        // print scene graph
        log::LogVisitor v(AUTO_LOGGER, log::S_FLOOD, *root);

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
void Object::clearPhysics(scene::Node* node)
{
    if ( scene::Group* group = dynamic_cast<scene::Group*>(node) )
    {
        physics::rigid_body_transform_ptr rbTransform( dynamic_cast<physics::RigidBodyTransform*>(node) );
        if (rbTransform)
        {
            if ( scene::Group* parent = rbTransform->getParent() )
            {
                // relink children
                parent->removeChild(rbTransform.get());
                while ( scene::Node* child = rbTransform->getChild() ) {
                    parent->addChild(child);
                }

				for ( scene::Node* i = parent->getChild(); i; i = i->getRight() ) {
					clearPhysics(i);
				}
            }
            else
            {
                parent = new scene::Group();
                while ( scene::Node* child = rbTransform->getChild() ) {
                    parent->addChild(child);
                }

                root.reset(parent);
            }
        }
    }
}

void Object::setPhysicsModel(physics::PhysicsModel* physicsModel_)
{
    using namespace physics;

    if (physicsModel) 
    {
        physicsModel->toggleSimulation(false);
        clearPhysics( root.get() );
    }

    physicsModel.reset(physicsModel_);
    if (root)
    {
        AUTO_LOGGER_MESSAGE(log::S_FLOOD, "Setting physics model for compound object" << LOG_FILE_AND_LINE); 
        
        // set transforms
        if (physicsModel)
        {
            for (PhysicsModel::rigid_body_iterator iter  = physicsModel->firstRigidBody();
                                                   iter != physicsModel->endRigidBody();
                                                   ++iter)
            {
                // insert rigid body into scene graph
                scene::node_ptr targetNode( findNamedNode( *root, hash_string((*iter)->getTarget()) ) );
                if (targetNode)
                {
                    physics::RigidBodyTransform* rbTransform = (*iter)->getMotionState();
                    if ( scene::Group* group = dynamic_cast<scene::Group*>( targetNode.get() ) )
                    {
                        // relink children
                        while ( scene::Node* child = group->getChild() ) {
                            rbTransform->addChild(child);
                        }

                        group->addChild(rbTransform);
                    }
                    else if ( targetNode->getParent() != 0 )
                    {
                        targetNode->getParent()->addChild(rbTransform);
                        rbTransform->addChild(targetNode.get());
                    }
                    else 
                    {
                        rbTransform->addChild(targetNode.get());
                        root.reset(rbTransform);
                    }
                }
                else {
                    AUTO_LOGGER_MESSAGE(log::S_WARNING, "Can't find node corresponding rigid body: " << (*iter)->getTarget() << std::endl);
                }
            }

            // enable simulation
            physicsModel->toggleSimulation(true);

            // apply local transform to the shapes
            scene::TransformVisitor tv(*root);
            DecomposeTransformVisitor dv(*root);
        
            // print scene graph
            log::LogVisitor lv(AUTO_LOGGER, log::S_FLOOD, *root);
        }
    }
}
#endif // SLON_ENGINE_USE_PHYSICS

} // namespace detail
} // namespace realm
} // namespace slon
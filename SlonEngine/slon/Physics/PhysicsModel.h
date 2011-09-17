#ifndef __SLON_ENGINE_PHYSICS_PHYSICS_MODEL_H__
#define __SLON_ENGINE_PHYSICS_PHYSICS_MODEL_H__

#include "../Database/Serializable.h"
#include "../Utility/referenced.hpp"
#include "Constraint.h"
#include "RigidBody.h"
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <map>
#include <set>

namespace slon {
namespace physics {

/** Container for rigid bodies, constraints and other physics entities. */
class SLON_PUBLIC PhysicsModel :
    public Referenced,
    public database::Serializable
{
private:
    template <typename RigidBody, typename Iterator>
    class rigid_body_iterator_impl :
        public boost::iterator_facade
        <
            rigid_body_iterator_impl<RigidBody, Iterator>,
            RigidBody,
            boost::bidirectional_traversal_tag
        >
    {
    friend class boost::iterator_core_access;
    private:
        void increment()
        {
            while (++iter != end)
            {
                item = dynamic_cast<RigidBody*>( iter->first.get() );
                if (item) {
                    break;
                }
            }
        }

        void decrement()
        {
            while (--iter != begin)
            {
                item = dynamic_cast<RigidBody*>( iter->first.get() );
                if (item) {
                    break;
                }
            }
        }

        bool equal(const rigid_body_iterator_impl& other) const
        {
            return iter == other.iter;
        }

        RigidBody& dereference() const 
        { 
            assert(item);
            return *item; 
        }

    public:
        rigid_body_iterator_impl(Iterator iter_,
                                 Iterator begin_,
                                 Iterator end_)
        :   iter(iter_)
        ,   begin(begin_)
        ,   end(end_)
        ,   item(0)
        {
            if (iter != end) 
            {
                item = dynamic_cast<RigidBody*>( iter->first.get() );
                if (!item) {
                    increment();
                }
            }
        }

    private:
        Iterator   iter;
        Iterator   begin;
        Iterator   end;
        RigidBody* item;
    };

    template<typename T>
    struct dereference :
        public std::unary_function<boost::intrusive_ptr<T>, T&>
    {
        T& operator () (const boost::intrusive_ptr<T>& value) const { return *value; }
    };

    typedef dereference<Constraint>         constraint_dereference;
    typedef dereference<const Constraint>   const_constraint_dereference;

private: 
    typedef std::map<collision_object_ptr, std::string>                 collision_object_map;
    typedef std::set<constraint_ptr>                                    constraint_set;

public:
    typedef collision_object_map::iterator                              collision_object_iterator;
    typedef collision_object_map::const_iterator                        collision_object_const_iterator;

    typedef rigid_body_iterator_impl<RigidBody, 
                                     collision_object_iterator>         rigid_body_iterator;
    typedef rigid_body_iterator_impl<const RigidBody, 
                                     collision_object_const_iterator>   rigid_body_const_iterator;

    typedef boost::transform_iterator<constraint_dereference, 
                                      constraint_set::iterator>         constraint_iterator;
    typedef boost::transform_iterator<const_constraint_dereference, 
                                      constraint_set::const_iterator>   constraint_const_iterator;

public:
    PhysicsModel();
    ~PhysicsModel();

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

	/** Get name of the physics model */
	const std::string& getName() const { return name; }

	/** Set name of the physics model */
	void setName(const std::string& name_) { name = name_; }

    /** Add rigid body to the physics model. 
     * @param rigidBody - rigid body to store in the model.
     * @param target - name of the target node for rigid body.
     */
    void addCollisionObject(const collision_object_ptr& rigidBody, const std::string& target = "");

    /** Remove rigid body from the model. */
    bool removeCollisionObject(const collision_object_ptr& rigidBody);

    /** Add constraint to the physics model */
    void addConstraint(const constraint_ptr& constraint);

    /** Remove constraint from the physics model */
    bool removeConstraint(const constraint_ptr& constraint);

	/** Find collision object by name */
	collision_object_iterator findCollisionObjectByName(const std::string& name);
	
	/** Find collision object by target */
	collision_object_iterator findCollisionObjectByTarget(const std::string& target);

	/** Find constraint by name */
	constraint_iterator findConstraintByName(const std::string& name);

    /** Get iterator addressing first collision object. */
    collision_object_iterator firstCollisionObject() { return collisionObjects.begin(); }

    /** Get iterator addressing first collision object. */
    collision_object_const_iterator firstCollisionObject() const { return collisionObjects.begin(); }

    /** Get iterator addressing end of collision objects. */
    collision_object_iterator endCollisionObject() { return collisionObjects.end(); }

    /** Get iterator addressing end of collision objects. */
    collision_object_const_iterator endCollisionObject() const { return collisionObjects.end(); }

    /** Get iterator addressing first rigid body. */
    rigid_body_iterator firstRigidBody();

    /** Get iterator addressing first rigid body. */
    rigid_body_const_iterator firstRigidBody() const;

    /** Get iterator addressing end of rigid bodies. */
    rigid_body_iterator endRigidBody();

    /** Get iterator addressing end of rigid bodies. */
    rigid_body_const_iterator endRigidBody() const;

    /** Get iterator addressing first constraint. */
    constraint_iterator firstConstraint();

    /** Get iterator addressing first constraint. */
    constraint_const_iterator firstConstraint() const;

    /** Get iterator addressing end of constraints. */
    constraint_iterator endConstraint();

    /** Get iterator addressing end of constraints. */
    constraint_const_iterator endConstraint() const;

private:
	std::string          name;
    collision_object_map collisionObjects;
    constraint_set       constraints;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_PHYSICS_MODEL_H__

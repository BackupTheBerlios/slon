#include "stdafx.h"
#include "Detail/Engine.h"
#include "Realm/Detail/BVHLocation.h"
#include "Scene/Visitors/NodeVisitor.h"
#include "Utility/Algorithm/algorithm.hpp"
#include "Utility/math.hpp"
#include <boost/thread/locks.hpp>
#include <sgl/Math/Intersection.hpp>

namespace {

    // manhattan distance
    inline float proximity(const math::AABBf& a, const math::AABBf& b)
    {
        const math::AABBf::vec_type d = (a.minVec + a.maxVec) - (b.minVec + b.maxVec); // vector between centers
	    return fabs(d.x) + fabs(d.y) + fabs(d.z);
    }

}

namespace slon {
namespace realm {
namespace detail {
		
template<typename World, typename Callback>
class WorldVisitor : 
	public boost::static_visitor<void>
{
public:
	WorldVisitor(World& world_, Callback& cb_)
	:	world(world_)
	,	cb(cb_)
	{}

	void operator () (const math::Ray3f& body) const
	{
        world.visit(body, cb);
	}
    
	void operator () (const math::Sphere3f& body) const
	{
		//world.visit<math::Sphere3f>(body, cb);
	}

	void operator () (const math::AABBf& body) const
	{
        world.visit(body, cb);
	}
		
	void operator () (const math::Frustumf& body) const
	{
        world.visit(body, cb);
	}

private:
	World&    world;
	Callback& cb;
};

template<typename World, typename Callback>
WorldVisitor<World, Callback> makeWorldVisitor(World& world, Callback& cb)
{
	return WorldVisitor<World, Callback>(world, cb);
}

World::World()
{
    // create default location
    addLocation(new BVHLocation);
}

void World::addLocation(Location* location)
{
    assert(location);
    locations.push_back( location_ptr(location) );
}

bool World::removeLocation(Location* location)
{
    return quick_remove( locations, location_ptr(location) );
}

bool World::haveLocation(Location* location) const
{
    return std::find( locations.begin(), locations.end(), location ) != locations.end();
}

void World::visit(const body_variant& body, object_callback& cb)
{
	boost::apply_visitor(makeWorldVisitor(*this, cb), body);
}

void World::visit(const body_variant& body, object_const_callback& cb) const
{
	boost::apply_visitor(makeWorldVisitor(*this, cb), body);
}

void World::visitVisible(const math::Frustumf& frustum, object_callback& cb)
{
	visit(frustum, cb);
}

void World::visitVisible(const math::Frustumf& frustum, object_const_callback& cb) const
{
	visit(frustum, cb);
}

realm::Object* World::createObject() const
{
	return new detail::Object;
}

bool World::update(realm::Object* object_)
{
	detail::Object* object = static_cast<detail::Object*>(object_);
    assert(object);

    if ( object->getBounds() == bounds<math::AABBf>::infinite() ) {
        return true;
    }

    if ( Location* location = object->getLocation() ) {
        return location->update(object);
    }

    return false;
}

bool World::remove(realm::Object* object_)
{
    detail::Object* object = static_cast<detail::Object*>(object_);
    assert(object && object->world == this);

    object->world = 0;
    if ( Location* location = object->getLocation() ) {
        return location->remove(object);
    }
    else {
        return quick_remove( infiniteObjects, object_ptr(object) );
    }
}

void World::add(realm::Object* object_)
{
    detail::Object* object = static_cast<detail::Object*>(object_);
    assert(object && object->world == 0);

    object->world = this;
    if ( object->getBounds() == bounds<math::AABBf>::infinite() ) {
        infiniteObjects.push_back( object_ptr(object) );
    }
    else
    {
		assert( !locations.empty() );

        // find nearest location
        size_t nearestLoc = 0;
        float  minDist    = proximity( object->getBounds(), locations[0]->getBounds() );
        for (size_t i = 1; i<locations.size(); ++i)
        {
            float d = proximity( object->getBounds(), locations[i]->getBounds() );
            if (d < minDist) 
            {
                minDist    = d;
                nearestLoc = i;
            }
        }

        locations[nearestLoc]->add(object);
    }
}

realm::Object* World::add(scene::Node*				node,
				          bool						dynamic
#ifdef SLON_ENGINE_USE_PHYSICS
				          , physics::PhysicsModel*	physicsModel
#endif             
				          )
{
#ifdef SLON_ENGINE_USE_PHYSICS
	detail::Object* object = new detail::Object(node, dynamic, physicsModel);
#else
	detail::Object* object = new detail::Object(node, dynamic);
#endif
	add(object);
	return object;
}

thread::lock_ptr World::lockForReading() const
{
    return thread::create_lock( new boost::shared_lock<boost::shared_mutex>(accessMutex) );
}

thread::lock_ptr World::lockForWriting()
{
    return thread::create_lock( new boost::unique_lock<boost::shared_mutex>(accessMutex) );
}

} // namespace detail

World& currentWorld()
{
	return Engine::Instance()->getWorld();
}

} // namespace realm 
} // namespace slon

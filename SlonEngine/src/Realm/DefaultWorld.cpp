#include "stdafx.h"
#include "Database/Archive.h"
#include "Detail/Engine.h"
#include "Realm/DefaultWorld.h"
#include "Realm/EventVisitor.h"
#include "Scene/Visitor.h"
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

DECLARE_AUTO_LOGGER("realm.DefaultWorld")

namespace slon {
namespace realm {
		
template<typename World, typename Visitor>
class WorldVisitor : 
	public boost::static_visitor<void>
{
public:
	WorldVisitor(World& world_, Visitor& visitor_)
	:	world(world_)
	,	visitor(visitor_)
	{}

	void operator () (const math::Ray3f& body) const
	{
        world.visit(body, visitor);
	}
    
	void operator () (const math::Sphere3f& body) const
	{
		//world.visit<math::Sphere3f>(body, cb);
	}

	void operator () (const math::AABBf& body) const
	{
        world.visit(body, visitor);
	}
		
	void operator () (const math::Frustumf& body) const
	{
        world.visit(body, visitor);
	}

private:
	World&    world;
	Visitor&  visitor;
};

template<typename World, typename Callback>
WorldVisitor<World, Callback> makeWorldVisitor(World& world, Callback& cb)
{
	return WorldVisitor<World, Callback>(world, cb);
}

DefaultWorld::DefaultWorld()
{
}

const char* DefaultWorld::serialize(database::OArchive& ar) const
{
    ar.openChunk("locations");
    {
        for (size_t i = 0; i<locations.size(); ++i) {
            ar.writeSerializable(locations[i].get());
        }
    }
    ar.closeChunk();

    ar.openChunk("infiniteObjects");
    {
        for (size_t i = 0; i<infiniteObjects.size(); ++i) {
            ar.writeSerializable(infiniteObjects[i].get());
        }
    }
    ar.closeChunk();

    return "World";
}

void DefaultWorld::deserialize(database::IArchive& ar)
{
    database::IArchive::chunk_info info;
    if ( !ar.openChunk("locations", info) ) {
        throw database::serialization_error(AUTO_LOGGER, "Missing locations chunk");
    }
    while ( realm::Location* location = ar.readSerializable<realm::Location>(false, true) ) {
        locations.push_back( location_ptr(location) );
    }
    ar.closeChunk();

    if ( !ar.openChunk("infiniteObjects", info) ) {
        throw database::serialization_error(AUTO_LOGGER, "Missing locations chunk");
    }
    while ( scene::Node* object = ar.readSerializable<scene::Node>(false, true) ) {
        infiniteObjects.push_back( scene::node_ptr(object) );
    }
    ar.closeChunk();
}

void DefaultWorld::addLocation(const location_ptr& location)
{
    assert(location);
    locations.push_back(location);
}

bool DefaultWorld::removeLocation(const location_ptr& location)
{
    return quick_remove(locations, location);
}

bool DefaultWorld::haveLocation(const location_ptr& location) const
{
    return std::find( locations.begin(), locations.end(), location ) != locations.end();
}

void DefaultWorld::visit(const body_variant& body, scene::Visitor& nv)
{
	boost::apply_visitor(makeWorldVisitor(*this, nv), body);
}

void DefaultWorld::visit(const body_variant& body, scene::ConstVisitor& nv) const
{
	boost::apply_visitor(makeWorldVisitor(*this, nv), body);
}

void DefaultWorld::visitVisible(const math::Frustumf& frustum, scene::Visitor& nv)
{
	// visit infinite objects
	for (size_t i = 0; i<infiniteObjects.size(); ++i) {
		nv.traverse(*infiniteObjects[i]);
	}

	// visit others
	for (size_t i = 0; i<locations.size(); ++i)
	{
		if ( test_intersection(locations[i]->getBounds(), frustum) ) {
			locations[i]->visitVisible(frustum, nv);
		}
	}
}

void DefaultWorld::visitVisible(const math::Frustumf& frustum, scene::ConstVisitor& nv) const
{
	// visit infinite objects
	for (size_t i = 0; i<infiniteObjects.size(); ++i) {
		nv.traverse(*infiniteObjects[i]);
	}

	// visit others
	for (size_t i = 0; i<locations.size(); ++i)
	{
		if ( test_intersection(locations[i]->getBounds(), frustum) ) {
			locations[i]->visitVisible(frustum, nv);
		}
	}
}

bool DefaultWorld::removeInfiniteNode(const scene::node_ptr& node)
{
    if ( quick_remove(infiniteObjects, node) ) 
    {
        EventVisitor ev(EventVisitor::WORLD_ADD, this, 0, *node);
        return true;
    }

    return false;
}

void DefaultWorld::addInfiniteNode(const scene::node_ptr& node)
{
	infiniteObjects.push_back(node);
    EventVisitor ev(EventVisitor::WORLD_ADD, this, 0, *node);
}

bool DefaultWorld::haveInfiniteNode(const scene::node_ptr& node) const
{
    return std::find( infiniteObjects.begin(), infiniteObjects.end(), node ) != infiniteObjects.end();
}

thread::lock_ptr DefaultWorld::lockForReading() const
{
    return thread::create_lock( new boost::shared_lock<boost::shared_mutex>(accessMutex) );
}

thread::lock_ptr DefaultWorld::lockForWriting()
{
    return thread::create_lock( new boost::unique_lock<boost::shared_mutex>(accessMutex) );
}

World* currentWorld()
{
	return Engine::Instance()->getWorld();
}

} // namespace realm 
} // namespace slon

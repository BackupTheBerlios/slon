#include "stdafx.h"
#include "Realm/World/ScalableWorld.h"
#include "Realm/Location/DbvtLocation.h"
#include "Utility/Algorithm/algorithm.hpp"
#include "Utility/math.hpp"
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

ScalableWorld::ScalableWorld()
{
    // create default location
    addLocation(new DbvtLocation);
}

void ScalableWorld::addLocation(Location* location)
{
    assert(location);
    locations.push_back( location_ptr(location) );
}

bool ScalableWorld::removeLocation(Location* location)
{
    return quick_remove( locations, location_ptr(location) );
}

bool ScalableWorld::haveLocation(Location* location) const
{
    return std::find( locations.begin(), locations.end(), location ) != locations.end();
}

void ScalableWorld::visit(const math::Ray3f& ray, scene::NodeVisitor& nv)
{
    // visit infinite objects
    for (size_t i = 0; i<infiniteObjects.size(); ++i) {
        nv.traverse( *object_core_access::get_node(*infiniteObjects[i]) );
    }

    // visit others
    for (size_t i = 0; i<locations.size(); ++i)
    {
        if ( test_intersection(locations[i]->getBounds(), ray) ) {
            locations[i]->visit(ray, nv);
        }
    }
}

void ScalableWorld::visit(const math::Ray3f& ray, scene::ConstNodeVisitor& nv) const
{
    // visit infinite objects
    for (size_t i = 0; i<infiniteObjects.size(); ++i) {
        nv.traverse( *object_core_access::get_node(*infiniteObjects[i]) );
    }

    // visit others
    for (size_t i = 0; i<locations.size(); ++i)
    {
        if ( test_intersection(locations[i]->getBounds(), ray) ) {
            locations[i]->visit(ray, nv);
        }
    }
}

void ScalableWorld::visit(const math::AABBf& area, scene::NodeVisitor& nv)
{
    // visit infinite objects
    for (size_t i = 0; i<infiniteObjects.size(); ++i) {
        nv.traverse( *object_core_access::get_node(*infiniteObjects[i]) );
    }

    // visit others
    for (size_t i = 0; i<locations.size(); ++i)
    {
        if ( test_intersection(area, locations[i]->getBounds()) ) {
            locations[i]->visit(area, nv);
        }
    }
}

void ScalableWorld::visit(const math::AABBf& area, scene::ConstNodeVisitor& nv) const
{
    // visit infinite objects
    for (size_t i = 0; i<infiniteObjects.size(); ++i) {
        nv.traverse( *object_core_access::get_node(*infiniteObjects[i]) );
    }

    // visit others
    for (size_t i = 0; i<locations.size(); ++i)
    {
        if ( test_intersection(area, locations[i]->getBounds()) ) {
            locations[i]->visit(area, nv);
        }
    }
}

void ScalableWorld::visit(const math::Frustumf& frustum, scene::NodeVisitor& nv)
{
    // visit infinite objects
    for (size_t i = 0; i<infiniteObjects.size(); ++i) {
        nv.traverse( *object_core_access::get_node(*infiniteObjects[i]) );
    }

    // visit others
    for (size_t i = 0; i<locations.size(); ++i)
    {
        if ( test_intersection(frustum, locations[i]->getBounds()) ) {
            locations[i]->visit(frustum, nv);
        }
    }
}

void ScalableWorld::visit(const math::Frustumf& frustum, scene::ConstNodeVisitor& nv) const
{
    // visit infinite objects
    for (size_t i = 0; i<infiniteObjects.size(); ++i) {
        nv.traverse( *object_core_access::get_node(*infiniteObjects[i]) );
    }

    // visit others
    for (size_t i = 0; i<locations.size(); ++i)
    {
        if ( test_intersection(frustum, locations[i]->getBounds()) ) {
            locations[i]->visit(frustum, nv);
        }
    }
}

bool ScalableWorld::update(Object* object)
{
    assert(object);
    if (object_core_access::get_world(*object) != this) {
        return false;
    }
    else if ( object->getBounds() == bounds<math::AABBf>::infinite() ) {
        return true;
    }

    if ( Location* location = object_core_access::get_location(*object) ) {
        return location->update(object);
    }

    return false;
}

bool ScalableWorld::remove(Object* object)
{
    assert(object);
    if (object_core_access::get_world(*object) != this) {
        return false;
    }

    object_core_access::set_world(*object, 0);
    if ( Location* location = object_core_access::get_location(*object) ) {
        return location->remove(object);
    }
    else {
        return quick_remove( infiniteObjects, object_ptr(object) );
    }
}

void ScalableWorld::add(Object* object)
{
    assert(object);

    object_core_access::set_world(*object, this);
    if ( object->getBounds() == bounds<math::AABBf>::infinite() ) {
        infiniteObjects.push_back( object_ptr(object) );
    }
    else if ( !locations.empty() )
    {
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

} // namespace realm 
} // namespace slon
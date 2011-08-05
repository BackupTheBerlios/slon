#include "stdafx.h"
#include "PyLibrary.h"
#include "Database/Library.h"
#include "Animation/Animation.h"
#include "Graphics/Effect.h"
#ifdef SLON_ENGINE_USE_PHYSICS
#   include "Physics/PhysicsModel.h"
#endif
#include "Scene/Node.h"
#include <boost/intrusive_ptr.hpp>
#include <boost/python.hpp>
#include <sgl/Texture.h>

using namespace boost::python;
using namespace slon::database;

void KeyError() { PyErr_SetString(PyExc_KeyError, "Invalid key"); }

// wrappers
template<class T>
struct map
{
    typedef T                               map_type;
    typedef typename map_type::key_type     key_type;
    typedef typename map_type::mapped_type  mapped_type;

    static mapped_type& get(map_type& cont, const key_type& key)
    {
        if ( cont.find(key) != cont.end() ) {
            return cont[key];
        }

        KeyError();
        return cont[key_type()];
    }

    static void set(map_type& cont, const key_type& key, const mapped_type& val)
    {
        cont[key] = val;
    }

    static void del(map_type& cont, const key_type& key)
    {
        if ( cont.find(key) != cont.end() ) {
            cont.erase(key);
        }
        else {
            KeyError();
        }
    }
};

template<typename Container>
void exportMapContainer(const char* name)
{
    class_<Container>(name, init<>())
        .def("__len__", &Container::size)
        .def("clear", &Container::clear)
        .def("__getitem__", &map<Container>::get, return_value_policy<copy_non_const_reference>())
        .def("__setitem__", &map<Container>::set, with_custodian_and_ward<1,2>()) 
        .def("__delitem__", &map<Container>::del);
}

void exportLibrary()
{
    exportMapContainer<Library::key_visual_scene_map>("key_visual_scene_map");
    class_<Library, boost::intrusive_ptr<Library>, boost::noncopyable>("Library", init<>())
        .def_readwrite("visualScenes", &Library::visualScenes);
}
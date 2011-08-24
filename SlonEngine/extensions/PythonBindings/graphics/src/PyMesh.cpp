#include "stdafx.h"
#include "Graphics/MeshConstructor.h"
#include "Graphics/StaticMesh.h"
#include "PyMesh.h"
#include <boost/python.hpp>
#include <sgl/Math/Containers.hpp>

using namespace boost::python;
using namespace slon::graphics;
using namespace slon;

// wrappers
void setAttributes2f(MeshConstructor& mesh, const std::string& name, unsigned index, const math::vector_of_vector2f& arr) 
{ 
    mesh.setAttributes(name, index, 2, arr.size(), sgl::FLOAT, &arr[0]); 
}

void setAttributes3f(MeshConstructor& mesh, const std::string& name, unsigned index, const math::vector_of_vector3f& arr) 
{ 
    mesh.setAttributes(name, index, 3, arr.size(), sgl::FLOAT, &arr[0]); 
}

void setAttributes4f(MeshConstructor& mesh, const std::string& name, unsigned index, const math::vector_of_vector4f& arr) 
{ 
    mesh.setAttributes(name, index, 4, arr.size(), sgl::FLOAT, &arr[0]); 
}

void setIndices(MeshConstructor& mesh, unsigned attribute, const std::vector<unsigned>& arr) 
{ 
    mesh.setIndices(attribute, arr.size(), &arr[0]); 
}

void exportMesh()
{
    // MeshConstructor
    {
        scope classScope = class_<MeshConstructor, boost::intrusive_ptr<MeshConstructor>, boost::noncopyable>("MeshConstructor", init<>())
            .def("setAttributes", setAttributes2f)
            .def("setAttributes", setAttributes3f)
            .def("setAttributes", setAttributes4f)
            .def("setIndices", setIndices)
            .def("createMesh", &MeshConstructor::createMesh);
    }

    // Mesh
    {
        scope classScope = class_<Mesh, boost::intrusive_ptr<Mesh>, boost::noncopyable>("Mesh", init<const Mesh::DESC&>())
            .def("addIndexedSubset", &Mesh::addIndexedSubset, return_value_policy<reference_existing_object>())
            .def("addPlainSubset", &Mesh::addPlainSubset, return_value_policy<reference_existing_object>());
        
        class_<Mesh::DESC>("DESC", init<>());
        class_<Mesh::indexed_subset>("indexed_subset", no_init);
        class_<Mesh::plain_subset>("plain_subset", no_init);
    }

    // StaticMesh
    {
        class_<StaticMesh, bases<scene::Node>, boost::intrusive_ptr<StaticMesh>, boost::noncopyable>("StaticMesh", init<>())
            .def(init<const mesh_ptr&>());

        implicitly_convertible< boost::intrusive_ptr<StaticMesh>, boost::intrusive_ptr<scene::Node> >();
    }
}
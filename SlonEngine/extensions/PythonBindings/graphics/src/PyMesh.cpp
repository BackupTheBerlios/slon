#include "stdafx.h"
#include "Graphics/CPUSideTriangleMesh.h"
#include "Graphics/StaticMesh.h"
#include "PyMesh.h"
#include <boost/python.hpp>
#include <sgl/Math/Containers.hpp>

using namespace boost::python;
using namespace slon::graphics;
using namespace slon;

// wrappers
void setAttributes2f(CPUSideMesh& mesh, const std::string& name, unsigned index, const math::vector_of_vector2f& arr) 
{ 
    mesh.setAttributes(name, index, 2, arr.size(), sgl::FLOAT, &arr[0]); 
}

void setAttributes3f(CPUSideMesh& mesh, const std::string& name, unsigned index, const math::vector_of_vector3f& arr) 
{ 
    mesh.setAttributes(name, index, 3, arr.size(), sgl::FLOAT, &arr[0]); 
}

void setAttributes4f(CPUSideMesh& mesh, const std::string& name, unsigned index, const math::vector_of_vector4f& arr) 
{ 
    mesh.setAttributes(name, index, 4, arr.size(), sgl::FLOAT, &arr[0]); 
}

void setIndices(CPUSideMesh& mesh, unsigned attribute, const std::vector<unsigned>& arr) 
{ 
    mesh.setIndices(attribute, arr.size(), &arr[0]); 
}

void exportMesh()
{
    // CPUSideMesh
    {
        class_<CPUSideMesh, boost::intrusive_ptr<CPUSideMesh>, boost::noncopyable>("CPUSideMesh", init<>())
            .def("setAttributes",       setAttributes2f)
            .def("setAttributes",       setAttributes3f)
            .def("setAttributes",       setAttributes4f)
            .def("setIndices",          setIndices)
            .def("createGPUSideMesh",   &CPUSideMesh::createGPUSideMesh);

        class_<CPUSideTriangleMesh, bases<CPUSideMesh>, boost::intrusive_ptr<CPUSideTriangleMesh>, boost::noncopyable>("CPUSideTriangleMesh", init<>())
            .def("getNumFaces",             &CPUSideTriangleMesh::getNumFaces)
            .def("getFaceVertex",           &CPUSideTriangleMesh::getFaceVertex)
            .def("haveFaceAdjacency",       &CPUSideTriangleMesh::haveFaceAdjacency)
            .def("generateFaceAdjacency",   &CPUSideTriangleMesh::generateFaceAdjacency)
            .def("getNumAdjacentFaces",     &CPUSideTriangleMesh::getNumAdjacentFaces)
            //.def("getAdjacentFaces",        &CPUSideTriangleMesh::getAdjacentFaces, return_value_policy<reference_existing_object>())
            .def("haveFaceNormals",         &CPUSideTriangleMesh::haveFaceNormals)
            .def("generateFaceNormals",     &CPUSideTriangleMesh::generateFaceNormals)
            .def("getFaceNormal",           &CPUSideTriangleMesh::getFaceNormal, return_value_policy<reference_existing_object>())
            .def("haveSmoothingGroups",     &CPUSideTriangleMesh::haveSmoothingGroups)
            .def("generateSmoothingGroups", &CPUSideTriangleMesh::generateSmoothingGroups)
            .def("getSmoothingGroup",       &CPUSideTriangleMesh::getSmoothingGroup)
            .def("generatePhongNormals",    &CPUSideTriangleMesh::generatePhongNormals);
    }

    // Mesh
    {
        scope classScope = class_<GPUSideMesh, boost::intrusive_ptr<GPUSideMesh>, boost::noncopyable>("GPUSideMesh", init<const GPUSideMesh::DESC&>())
            .def("addIndexedSubset", &GPUSideMesh::addIndexedSubset, return_value_policy<reference_existing_object>())
            .def("addPlainSubset",   &GPUSideMesh::addPlainSubset,   return_value_policy<reference_existing_object>());
        
        class_<GPUSideMesh::DESC>("DESC", init<>());
        class_<GPUSideMesh::indexed_subset>("indexed_subset", no_init);
        class_<GPUSideMesh::plain_subset>("plain_subset", no_init);
    }

    // StaticMesh
    {
        class_<StaticMesh, bases<scene::Node>, boost::intrusive_ptr<StaticMesh>, boost::noncopyable>("StaticMesh", init<>())
            .def(init<const gpu_side_mesh_ptr&>());

        implicitly_convertible< boost::intrusive_ptr<StaticMesh>, boost::intrusive_ptr<scene::Node> >();
    }
}
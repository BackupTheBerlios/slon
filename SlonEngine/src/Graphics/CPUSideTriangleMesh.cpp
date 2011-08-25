#include "stdafx.h"
#include "Graphics/CPUSideTriangleMesh.h"
#include "Log/Logger.h"
#include <boost/unordered_map.hpp>

DECLARE_AUTO_LOGGER("graphics.CPUSideTriangleMesh")

namespace 
{
    template<typename T>
    unsigned get_first_nonzero_bit(T value)
    {
        int i = 1;
        while (value > 0) 
        {
            if (value & 1) {
                return i;
            }
            value >>= 1;
            ++i;
        }

        return 0;
    }
} // anonymous namespace

namespace slon {
namespace graphics {

void CPUSideTriangleMesh::generateFaceAdjacency()
{
    assert(!indicesArrays[0].empty() && "No index data specified");

    typedef std::pair<unsigned, unsigned>                               edge;
    typedef boost::unordered_multimap<edge, unsigned>                   edge_face_map;
    typedef std::pair<edge_face_map::iterator, edge_face_map::iterator> edge_face_map_range;

    // fill up edge map
    edge_face_map edgeFaceMap;
    for (unsigned iFace = 0; iFace < getNumFaces(); ++iFace)
    {
        for (int j = 0; j<3; ++j)
        {
            edge e( getFaceVertex(iFace, j), getFaceVertex(iFace, (j + 1) % 3) );
            if (e.first > e.second) {
                std::swap(e.first, e.second);
            }
            edgeFaceMap.insert( std::make_pair(e, iFace) );
        }
    }

    // add neighbor to the face if it shares edge with some other
    faceNeighborsStarts.resize(getNumFaces() + 1);
    for (unsigned iFace = 0; iFace < getNumFaces(); ++iFace)
    {
        faceNeighborsStarts[iFace] = faceNeighbors.size();
        for (int j = 0; j<3; ++j)
        {
            edge e( getFaceVertex(iFace, j), getFaceVertex(iFace, (j + 1) % 3) );
            if (e.first > e.second) {
                std::swap(e.first, e.second);
            }
            
            // add neighbor faces which share same edge
            edge_face_map_range edgeFacesRange = edgeFaceMap.equal_range(e);
            for (edge_face_map::iterator it  = edgeFacesRange.first;
                                         it != edgeFacesRange.second;
                                         ++it)
            {
                if (it->second != iFace) {
                    faceNeighbors.push_back(it->second);
                }
            }
        }
    }

    // fake end element
    faceNeighborsStarts[getNumFaces()] = faceNeighbors.size();
}

void CPUSideTriangleMesh::generateFaceNormals()
{
    assert(!attributeArrays[0].empty() && "No vertex data specified");
    assert(!indicesArrays[0].empty() && "No index data specified");

    if ( const math::Vector3f* vertices = queryAttributeData<math::Vector3f>(0) )
    {
        faceNormals.resize( getNumFaces() );
        for (size_t i = 0; i<getNumFaces(); ++i) 
        {
            const math::Vector3f& A = vertices[ getFaceVertex(i, 0) ];
            const math::Vector3f& B = vertices[ getFaceVertex(i, 1) ];
            const math::Vector3f& C = vertices[ getFaceVertex(i, 2) ];

            faceNormals[i] = math::cross(B - A, C - A);
        }
    }
    else if ( const math::Vector4f* vertices = queryAttributeData<math::Vector4f>(0) )
    {
        faceNormals.resize( getNumFaces() );
        for (size_t i = 0; i<getNumFaces(); ++i) 
        {
            const math::Vector4f& A = vertices[ getFaceVertex(i, 0) ];
            const math::Vector4f& B = vertices[ getFaceVertex(i, 1) ];
            const math::Vector4f& C = vertices[ getFaceVertex(i, 2) ];

            faceNormals[i] = math::cross(math::xyz(B - A), math::xyz(C - A));
        }
    }
    else {
        assert(!"Invalid number of vertex components");
    }

    // normalize
    for (size_t i = 0; i<faceNormals.size(); ++i)
    {
        float len = math::length(faceNormals[i]);
        if (len < 0.0001f) {
            faceNormals[i] = math::Vector3f(0.0f); // in case of degenerate triangle
        }
        else {
            faceNormals[i] /= len;
        }
    }
}

void CPUSideTriangleMesh::generateSmoothingGroups(float maxSmoothAngle,
                                                  bool  regenerateNormals,
                                                  bool  regenerateAdjacency)
{
    if (regenerateNormals) {
        generateFaceNormals();
    }

    if (regenerateAdjacency) {
        generateFaceAdjacency();
    }

    // split faces
    float                 cSMAngle = cos(maxSmoothAngle);
    unsigned              smGroup  = 0;
    std::vector<unsigned> faceSMGroup(getNumFaces(), 0);
    //std::vector<unsigned> smGroupRepresentative(1);
    for (unsigned iFace = 0; iFace < getNumFaces(); ++iFace)
    {
        // sm group for face already set
        if (faceSMGroup[iFace] != 0) {
            continue;
        }

        // make new smoothing group
        faceSMGroup[iFace] = ++smGroup;
        //smGroupRepresentative.push_back(iFace);

        // run DFS to gather faces belonging to same smoothing group
        std::stack<unsigned> toVisit; toVisit.push(iFace);
        while ( !toVisit.empty() ) 
        {
            unsigned        curFace  = toVisit.top(); toVisit.pop();
            const unsigned* adjFaces = getAdjacentFaces(curFace);
            for (unsigned i = 0; i<getNumAdjacentFaces(curFace); ++i)
            {
                // if angle between normals is less than maxSmoothAngle put adjacent face into same smoothing group
                unsigned adjFace = adjFaces[i];
                if ( faceSMGroup[adjFace] == 0 && math::dot(faceNormals[curFace], faceNormals[adjFace]) > cSMAngle ) 
                {
                    faceSMGroup[adjFace] = smGroup;
                    toVisit.push(adjFace);
                }
            }
        }
    }

    // once smoothing groups are set try to reduce their quantity to 32 using greedy algorithm
    smoothingGroups.resize( getNumFaces() );
    for (size_t i = 0; i<getNumFaces(); ++i) {
        smoothingGroups[i] = faceSMGroup[i] % 32;
    }
}

void CPUSideTriangleMesh::generatePhongNormals()
{
    assert( haveFaceNormals() && haveSmoothingGroups() && "Mesh should have smoothing groups - use generateSmoothingGroups");

    // vertex normals
    math::vector_of_vector3f normals(attributeArrays[0].count);
    // vertex normals indices
    std::vector<unsigned> indices(indicesArrays[0].indices.get(), indicesArrays[0].indices.get() + indicesArrays[0].count);
    // vertex smoothing groups
    std::vector<int> vertexSMGroup(attributeArrays[0].count, -1);

    // average normals against faces sharing same smoothing group
    for (size_t iFace = 0; iFace < faceNormals.size(); ++iFace)
    {
        for (size_t iVertInd = iFace * 3; iVertInd < iFace * 3 + 3; ++iVertInd)
        {
            unsigned iVert = indices[iVertInd];
            if (vertexSMGroup[iVert] == -1) 
            {
                vertexSMGroup[iVert] = get_first_nonzero_bit(smoothingGroups[iFace]);
                normals[iVert] = faceNormals[iFace];
            }
            else if ( (smoothingGroups[iFace] != 0) && (smoothingGroups[iFace] & (1 << (vertexSMGroup[iVert] - 1))) ) {
                normals[iVert] += faceNormals[iFace];
            }
            else 
            {
                indices[iVertInd] = normals.size();
                normals.push_back(faceNormals[iFace]);
                vertexSMGroup.push_back( get_first_nonzero_bit(smoothingGroups[iFace]) );
            }
        }
    }

    // normalize all
    for (size_t i = 0; i<normals.size(); ++i) {
        normals[i] = math::normalize(normals[i]);
    }

    // find attribute slot for normals
    int normalAttrIndex = getAttributeIndex("normal");
    if (normalAttrIndex == -1) {
        normalAttrIndex = getFreeAttributeIndex();
    }
    assert(normalAttrIndex != -1 && "No free slot for normals");

    // setup normals
    setAttributes("normal", normalAttrIndex, 3, normals.size(), sgl::FLOAT, &normals[0]);
    setIndices(normalAttrIndex, indices.size(), &indices[0]);
}

} // namespace graphics
} // namespace slon

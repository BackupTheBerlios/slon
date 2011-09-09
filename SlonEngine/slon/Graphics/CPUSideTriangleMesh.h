#ifndef __SLON_ENGINE_GRAPHICS_CPU_SIDE_TRIANGLE_MESH_H__
#define __SLON_ENGINE_GRAPHICS_CPU_SIDE_TRIANGLE_MESH_H__

#include "CPUSideMesh.h"
#include <sgl/Math/Containers.hpp>

namespace slon {
namespace graphics {
	
/** CPU side mesh. Treats mesh as indexed triangle surface, contains some
 * geometry processing utilities.
 */
class SLON_PUBLIC CPUSideTriangleMesh :
    public CPUSideMesh
{
public:
    /** Get number of triangles in mesh. */
    unsigned getNumFaces() const { return indicesArrays[0].count / 3; }

    /** Get face vertex index. */
    unsigned getFaceVertex(unsigned iFace, unsigned iVert) const { return indicesArrays[0].indices[iFace*3 + iVert]; }

    /** Check whether mesh have adjacency information. */
    bool haveFaceAdjacency() const { return !faceNeighbors.empty(); }

    /** Generate face adjacency information. */
    void generateFaceAdjacency();

    /** Get number of adjacent faces */
    unsigned getNumAdjacentFaces(int face) const { return faceNeighborsStarts[face + 1] - faceNeighborsStarts[face]; }

    /** Get adjacent faces array */
    const unsigned* getAdjacentFaces(int face) const { return &faceNeighbors[faceNeighborsStarts[face]]; }

    /** Check whether mesh have face normals. Doesn't check they could expire. */
    bool haveFaceNormals() const { return !faceNormals.empty(); }

    /** Generate face normals (regenerate if ones already exist). */
    void generateFaceNormals();

    /** Get face normal */
    const math::Vector3f& getFaceNormal(unsigned i) { return faceNormals[i]; }

    /** Check whether mesh have smoothing groups. Doesn't check they could expire. */
    bool haveSmoothingGroups() const { return !smoothingGroups.empty(); }

    /** Generate smoothing groups for faces accordingly to the maximum angle between smoothed faces (regenerate if ones already exist). 
     * @param maxSmoothAngle - maximum angle between face normals to belong same smoothing group.
     * @param regenerateNormals - regenerate face normals.
     * @param regenerateNormals - regenerate face adjacency information.
     */
    void generateSmoothingGroups(float maxSmoothAngle = math::PI * 0.3f,
                                 bool  regenerateNormals = true,
                                 bool  regenerateAdjacency = true);

    /** Get face smoothing group. Returned integer is mask whether i'th nonzero bit means face
     * belongs to i'th smoothing group. Zero mask means face should not be smoothed with any other.
     */
    unsigned getSmoothingGroup(unsigned i) const { return smoothingGroups[i]; }

    /** Generate vertices phong normals (replace existing if presented). Vertex phong normal is normalized average
     * between face normals containing this vertex and sharing same smoothing group.
     * @see generateSmoothingGroups
     */
    void generatePhongNormals();

private:
    // geometry processing information
    std::vector<unsigned>       smoothingGroups;
    math::vector_of_vector3f    faceNormals;
    std::vector<unsigned>       faceNeighbors;
    std::vector<unsigned>       faceNeighborsStarts;
};

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_CPU_SIDE_TRIANGLE_MESH_H__

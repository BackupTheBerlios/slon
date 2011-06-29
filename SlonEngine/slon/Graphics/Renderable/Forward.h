#ifndef __SLON_ENGINE_RENDERABLE_FORWARD_H__
#define __SLON_ENGINE_RENDERABLE_FORWARD_H__

namespace boost
{
    template<typename T>
    class intrusive_ptr;
}

namespace slon {
namespace graphics {

/** For convinience engine renderables store attributes in the
 * stages defined by their semantic.
 */
enum ATTRIBUTE_SEMANTIC 
{
    ATTRIBUTE_POSITION = 0,
    ATTRIBUTE_NORMAL   = 1,
    ATTRIBUTE_TANGENT  = 2,
    ATTRIBUTE_BINORMAL = 3,
    ATTRIBUTE_COLOR    = 4,
    ATTRIBUTE_WEIGHTS  = 5,
};

class Mesh;
class MeshData;
class StaticMesh;

typedef boost::intrusive_ptr<StaticMesh>		static_mesh_ptr;
typedef boost::intrusive_ptr<const StaticMesh>	const_static_mesh_ptr;

typedef boost::intrusive_ptr<MeshData>			mesh_data_ptr;
typedef boost::intrusive_ptr<const MeshData>	const_mesh_data_ptr;

typedef boost::intrusive_ptr<Mesh>				mesh_ptr;
typedef boost::intrusive_ptr<const Mesh>		const_mesh_ptr;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_RENDERABLE_FORWARD_H__
#ifndef __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_MESH_H__
#define __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_MESH_H__

#include <sgl/Math/Containers.hpp>
#include <sgl/Font.h>
#include <sgl/IndexBuffer.h>
#include <sgl/VertexBuffer.h>
#include <sgl/Program.h>
#include "../../Scene/Entity.h"
#include "../Effect/DebugEffect.h"
#include "../Effect/DebugTextEffect.h"
#include "../Renderable.h"

namespace slon {
namespace graphics {

/** Debug mesh provides basic functionality for drawing debug information */
class DebugMesh :
    public scene::Entity
{
private:
    typedef Renderable renderable;

public:
    struct text_subset
        : public renderable
    {
        std::string             text;
        math::Vector2f          position;
        debug_text_effect_ptr   debugEffect;

        // Override renderable
        void                render() const;
        graphics::Effect*   getEffect() const { return debugEffect.get(); }
    };

    struct subset
        : public renderable
    {
        DebugMesh*              debugMesh;
        debug_effect_ptr        debugEffect;
        math::Matrix4f          transform;
        sgl::PRIMITIVE_TYPE     primitiveType;
        unsigned int            vertexOffset;
        unsigned int            numVertices;
        bool                    skip;

        subset() :
            debugMesh(0),
            primitiveType(sgl::POINTS),
            vertexOffset(0),
            numVertices(0),
            skip(false)
        {}

        // Override renderable
        void                render() const;
        graphics::Effect*   getEffect() const { return debugEffect.get(); }
    };

    // vector typedefs
    typedef math::vector_of_vector3f                                vertices_vector;
    typedef std::vector<unsigned int>                               indices_vector;
    typedef std::vector<text_subset>                                text_subset_vector;
    typedef sgl::vector< subset, sgl::aligned_allocator<subset> >   subset_vector;

public:
    DebugMesh();

    // Override Entity
    using Entity::accept;

    void accept(scene::CullVisitor& visitor) const;
    void accept(scene::TransformVisitor& visitor);

    // Override Entity
    const math::AABBf& getBounds() const;

    /** Clear data in the mesh */
    void clear(bool clearBuffers = false);

    virtual ~DebugMesh() {}

public:
    void dirty();
    void pushPrimitive( sgl::PRIMITIVE_TYPE primType,
                        size_t              numVertices );
    void pushTextPrimitive(const std::string& text);

public:
    // states
    unsigned                        totalNumVertices;
    bool                            stateChanged;
    bool                            infiniteBounds;
    bool                            useCameraProjection;
    math::Matrix4f                  projection;
    math::Matrix4f                  transform;
    math::Matrix4f                  baseTransform;
    math::Vector4f                  color;
    math::Vector2i                  textSize;
    sgl::ref_ptr<const sgl::Font>   font;

    // data
    bool                geometryDirty;
    bool                depthTest;
    bool                wireframe;
    math::AABBf         aabb;
	text_subset_vector  textSubsets;
	subset_vector       subsets;
    vertices_vector     vertices;
    indices_vector      indices;

	// graphics
	sgl::ref_ptr<sgl::VertexBuffer> vertexBuffer;
    sgl::ref_ptr<sgl::VertexLayout> vertexLayout;
	sgl::ref_ptr<sgl::IndexBuffer>  indexBuffer;
};

typedef boost::intrusive_ptr<DebugMesh>         debug_mesh_ptr;
typedef boost::intrusive_ptr<const DebugMesh>   const_debug_mesh_ptr;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_MESH_H__

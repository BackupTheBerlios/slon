#ifndef __SLON_ENGINE_GRAPHICS_FORWARD_H__
#define __SLON_ENGINE_GRAPHICS_FORWARD_H__

#include "../Config.h"

namespace sgl
{
    class Device;
    class Image;
    class Texture;
    class Texture2D;
    class Texture3D;
    class TextureCube;
    class Shader;
    class Program;
}

namespace std
{
    template<typename T>
    class allocator;

    template<typename T, typename A = allocator<T> >
    class list;
}

namespace boost
{
    template<typename T>
    class intrusive_ptr;
}

namespace slon {
namespace graphics {

    // forward types
    class Effect;
    class Renderable;
    class Mesh;
    class MeshConstructor;
    class PostProcessFilter;
    class SkinnedMesh;
    class SkyBox;
    class StaticMesh;

    // export types into graphics namespace
    typedef sgl::Device                Device;
    typedef sgl::Image                 Image;
    typedef sgl::Texture               Texture;
    typedef sgl::Texture2D             Texture2D;
    typedef sgl::Texture3D             Texture3D;
    typedef sgl::TextureCube           TextureCube;
    typedef sgl::Shader                Shader;
    typedef sgl::Program               Program;

    // common ptr typedefs
    typedef boost::intrusive_ptr<Device>        device_ptr;
    typedef boost::intrusive_ptr<Image>         image_ptr;
    typedef boost::intrusive_ptr<Texture>       texture_ptr;
    typedef boost::intrusive_ptr<Texture2D>     texture_2d_ptr;
    typedef boost::intrusive_ptr<Texture3D>     texture_3d_ptr;
    typedef boost::intrusive_ptr<TextureCube>   texture_cube_ptr;
    typedef boost::intrusive_ptr<Shader>        shader_ptr;
    typedef boost::intrusive_ptr<Program>       program_ptr;

    typedef boost::intrusive_ptr<Effect>                    effect_ptr;
    typedef boost::intrusive_ptr<Renderable>                renderable_ptr;
    typedef boost::intrusive_ptr<Mesh>                      mesh_ptr;
    typedef boost::intrusive_ptr<MeshConstructor>           mesh_constructor_ptr;
    typedef boost::intrusive_ptr<const MeshConstructor>     const_mesh_constructor_ptr;
    typedef boost::intrusive_ptr<SkinnedMesh>               skinned_mesh_ptr;
    typedef boost::intrusive_ptr<const SkinnedMesh>         const_skinned_mesh_ptr;
    typedef boost::intrusive_ptr<SkyBox>                    sky_box_ptr;
    typedef boost::intrusive_ptr<const SkyBox>              const_sky_box_ptr;
    typedef boost::intrusive_ptr<StaticMesh>                static_mesh_ptr;
    typedef boost::intrusive_ptr<const StaticMesh>          const_static_mesh_ptr;

    typedef boost::intrusive_ptr<PostProcessFilter>        post_process_filter_ptr;
    typedef boost::intrusive_ptr<const PostProcessFilter>  const_post_process_filter_ptr;
    typedef std::list<const_post_process_filter_ptr>       post_process_filter_chain;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_FORWARD_H__
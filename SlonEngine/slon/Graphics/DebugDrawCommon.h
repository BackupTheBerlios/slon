#ifndef __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_MESH_COMMON_H__
#define __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_MESH_COMMON_H__

#include <cstring>
#include "../Utility/Memory/aligned.hpp"
#include "DebugMesh.h"

namespace slon {
namespace graphics {
namespace debug {

/** Debug primitive to setup font for the debug text */
struct SLON_PUBLIC font :
    public aligned<0x10>
{
    explicit font(const sgl::Font* textFont_ = 0) :
        textFont(textFont_)
    {}

    sgl::ref_ptr<const sgl::Font> textFont;
};

/** Debug primitive to setup debug text size */
struct SLON_PUBLIC text_size :
    public aligned<0x10>
{
    text_size(int width, int height) :
        size(width, height)
    {}

    explicit text_size(const math::Vector2i& size_) :
        size(size_)
    {}

    math::Vector2i size;
};

/** Debug primitive to print debug text */
struct SLON_PUBLIC text :
    public aligned<0x10>
{
    explicit text(const std::string& str_) :
        str(str_)
    {}

    template<typename T0>
    text(const std::string&  format, 
         T0                  value0)
    {
        str.resize(256);
        sprintf_s(&str[0], 256, format.c_str(), value0);
        str.resize( strlen( str.c_str() ) );
    }

    template<typename T0, typename T1>
    text(const std::string&  format, 
         T0                  value0, 
         T1                  value1)
    {
        str.resize(256);
        sprintf_s(&str[0], 256, format.c_str(), value0, value1);
        str.resize( strlen( str.c_str() ) );
    }

    template<typename T0, typename T1, typename T2>
    text(const std::string&  format, 
         T0                  value0, 
         T1                  value1, 
         T2                  value2)
    {
        str.resize(256);
        sprintf_s(&str[0], 256, format.c_str(), value0, value1, value2);
        str.resize( strlen( str.c_str() ) );
    }

    std::string str;
};

/** Debug primitive to setup color of the polygons */
struct SLON_PUBLIC color :
    public aligned<0x10>
{
    color(float r, float g, float b, float a = 1.0f) :
        colorVal(r, g, b, a)
    {}

    explicit color(const math::Vector4f& _colorVal) :
        colorVal(_colorVal)
    {}

    math::Vector4f colorVal;
};

/** Debug primitive to setup projection transform */
struct SLON_PUBLIC projection :
    public aligned<0x10>
{
    projection() :
        useCameraProjection(true)
    {}

    explicit projection(const math::Matrix4f& _matrix) :
        useCameraProjection(false),
        matrix(_matrix)
    {}

    bool            useCameraProjection;
    math::Matrix4f  matrix;
};

/** Debug primitive to setup model transform */
struct SLON_PUBLIC transform :
    public aligned<0x10>
{
    explicit transform(const math::Matrix4f& _matrix) :
        matrix(_matrix)
    {}

    math::Matrix4f matrix;
};

/** Debug primitive to toggle depth test */
struct SLON_PUBLIC depth_test
{
    depth_test(bool _toggle) :
        toggle(_toggle)
    {}

    bool toggle;
};

/** Debug primitive to toggle wireframe drawing */
struct SLON_PUBLIC wireframe
{
    wireframe(bool _toggle) :
        toggle(_toggle)
    {}

    bool toggle;
};

/** Debug primitive - line */
struct SLON_PUBLIC line
{
    line(const math::Vector2f& a_, const math::Vector2f& b_) :
        a(a_.x, a_.y, 0.0f),
        b(b_.x, b_.y, 0.0f)
    {}

    line(const math::Vector3f& a_, const math::Vector3f& b_) :
        a(a_),
        b(b_)
    {}

    math::Vector3f a,b;
};

/** Debug primitive - sector */
struct SLON_PUBLIC sector
{
    sector(const    math::Vector3f& up_, 
           const    math::Vector3f& cross_, 
           float    loLimit_, 
           float    hiLimit_,
           bool     filled_ = false,
           unsigned fullCircleSplits_ = 60)
    :   up(up_)
    ,   cross(cross_)
    ,   loLimit(loLimit_)
    ,   hiLimit(hiLimit_)
    ,   fullCircleSplits(fullCircleSplits_)
    ,   filled(filled_)
    {}

    math::Vector3f  up, cross;
    float           loLimit, hiLimit;
    bool            filled;
    unsigned        fullCircleSplits;
};

/** Debug primitive - cone(up axis is Y) */
struct SLON_PUBLIC cone
{
    cone(float  radius_,
         float  height_,
         bool   wired_ = false,
         size_t numVertices_ = 20)
    :   radius(radius_)
    ,   height(height_)
    ,   wired(wired_)
    ,   numVertices(numVertices_)
    {}

    float   radius;
    float   height;
    bool    wired;
    size_t  numVertices; /// number of vertices in the base; total_num_vertices = numVertices + 1
};

/** Debug primitive - cylinder(up axis is Y) */
struct SLON_PUBLIC cylinder
{
    cylinder(float  radius_,
             float  height_,
             bool   wired_ = false,
             size_t numVertices_ = 20)
    :   radius(radius_)
    ,   height(height_)
    ,   wired(wired_)
    ,   numVertices(numVertices_)
    {}

    float   radius;
    float   height;
    bool    wired;
    size_t  numVertices; /// number of vertices in the caps; total_num_vertices = numVertices * 2 
};

/** Setup debug mesh draw color. */
SLON_PUBLIC DebugMesh& operator << (DebugMesh& mesh, const font& f);

/** Setup debug mesh draw color. */
SLON_PUBLIC DebugMesh& operator << (DebugMesh& mesh, const text_size& ts);

/** Setup debug mesh draw color. */
SLON_PUBLIC DebugMesh& operator << (DebugMesh& mesh, const text& t);

/** Setup debug mesh draw color. */
SLON_PUBLIC DebugMesh& operator << (DebugMesh& mesh, const color& c);

/** Setup debug mesh projection transform. */
SLON_PUBLIC DebugMesh& operator << (DebugMesh& mesh, const projection& t);

/** Setup debug mesh transform. */
SLON_PUBLIC DebugMesh& operator << (DebugMesh& mesh, const transform& t);

/** Toggle depth test for debug mesh */
SLON_PUBLIC DebugMesh& operator << (DebugMesh& mesh, const depth_test& d);

/** Toggle wireframe for debug mesh */
SLON_PUBLIC DebugMesh& operator << (DebugMesh& mesh, const wireframe& w);

/** Add another debug mesh to the debug mesh */
SLON_PUBLIC DebugMesh& operator << (DebugMesh& mesh, const DebugMesh& other);

/** Add line to the debug mesh */
SLON_PUBLIC DebugMesh& operator << (DebugMesh& mesh, const line& l);

/** Add sector to the debug mesh */
SLON_PUBLIC DebugMesh& operator << (DebugMesh& mesh, const sector& s);

/** Add cone to the debug mesh */
SLON_PUBLIC DebugMesh& operator << (DebugMesh& mesh, const cone& c);

/** Add cylinder to the debug mesh */
SLON_PUBLIC DebugMesh& operator << (DebugMesh& mesh, const cylinder& c);

/** Add line to the debug mesh */
SLON_PUBLIC DebugMesh& operator << (DebugMesh& mesh, const math::AABBf& a);

} // namespace debug
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_MESH_COMMON_H__

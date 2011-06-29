#ifndef __SLON_ENGINE_DATABASE_DETAIL_SGL_SERIALIZATION_H__
#define __SLON_ENGINE_DATABASE_DETAIL_SGL_SERIALIZATION_H__

#include <sgl/Device.h>
#include <sgl/Math/AABB.hpp>
#include "../Archive.h"

namespace slon {
namespace database {

// math serialization
template<typename T, int n>
void serialize(OArchive& ar, const char* name, const math::AABB<T,n>& aabb)
{
    ar.openChunk(name);
    ar.writeChunk("minVec", aabb.minVec.arr, aabb.minVec.num_elements);
    ar.writeChunk("maxVec", aabb.maxVec.arr, aabb.minVec.num_elements);
    ar.closeChunk();
}

template<typename T, int n>
void deserialize(IArchive& ar, const char* name, math::AABB<T,n>& aabb)
{
    IArchive::chunk_info info;
    if (!ar.openChunk(name, info)) {
        throw serialization_error("Can't open AABB chunk");
    }

    ar.readChunk("minVec", aabb.minVec.arr, aabb.minVec.num_elements);
    ar.readChunk("maxVec", aabb.maxVec.arr, aabb.minVec.num_elements);
    ar.closeChunk();
}

} // namespace slon
} // namespace database

#endif // __SLON_ENGINE_DATABASE_DETAIL_SGL_SERIALIZATION_H__
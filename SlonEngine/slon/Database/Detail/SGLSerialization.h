#ifndef __SLON_ENGINE_DATABASE_DETAIL_SGL_SERIALIZATION_H__
#define __SLON_ENGINE_DATABASE_DETAIL_SGL_SERIALIZATION_H__

#include <sgl/Device.h>
#include "../Serializable.h"

namespace slon {
namespace database {

// math serialization
template<typename T, int n>
void serialize(OArchive& ar, const char* name, const math::AABB<T,n>& aabb)
{
    ar.openChunk(name);
    ar.writeChunk("minVec", &aabb.minVec[0], aabb.minVec.num_elements);
    ar.writeChunk("maxVec", &aabb.maxVec[0], aabb.minVec.num_elements);
    ar.closeChunk();
}

template<typename T, int n>
void deserialize(IArchive& ar, const char* name, math::AABB<T,n>& aabb)
{
    if (!ar.openChunk(name)) {
        throw serialization_error(log::logger_ptr(), "Can't open AABB chunk");
    }

    ar.writeChunk("minVec", &aabb.minVec[0], aabb.minVec.num_elements);
    ar.writeChunk("maxVec", &aabb.maxVec[0], aabb.minVec.num_elements);
    ar.closeChunk();
}

} // namespace slon
} // namespace database

#endif // __SLON_ENGINE_DATABASE_DETAIL_SGL_SERIALIZATION_H__
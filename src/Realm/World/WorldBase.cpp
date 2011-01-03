#include "stdafx.h"
#include "Realm/World/WorldBase.h"
#include <boost/thread/locks.hpp>

namespace slon {
namespace realm {

thread::lock_ptr WorldBase::lockForReading() const
{
    return thread::create_lock( new boost::shared_lock<boost::shared_mutex>(accessMutex) );
}

thread::lock_ptr WorldBase::lockForWriting()
{
    return thread::create_lock( new boost::unique_lock<boost::shared_mutex>(accessMutex) );
}

} // namespace realm
} // namespace slon
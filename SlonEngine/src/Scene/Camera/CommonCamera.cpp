#include "stdafx.h"
#include "Scene/Camera/CommonCamera.h"
#include <boost/thread/locks.hpp>

using namespace slon;
using namespace scene;
using namespace graphics;

CommonCamera::CommonCamera()
{
}

void CommonCamera::setRenderTarget(sgl::RenderTarget* renderTarget_)
{
    renderTarget.reset(renderTarget_);
}

void CommonCamera::setViewport(const sgl::rectangle& viewport_)
{
    viewport = viewport_;
}

const math::AABBf& CommonCamera::getBounds() const
{
    // FIXME:
    //updateArea = math::AABBf( getPosition() - math::Vector3f(100.0f, 100.0f, 100.0f),
    //                          getPosition() + math::Vector3f(100.0f, 100.0f, 100.0f) );
    updateArea = math::AABBf(-1000.0f, -1000.0f, -1000.0f, 1000.0f, 1000.0f, 1000.0f);
    return updateArea;
}

thread::lock_ptr CommonCamera::lockForReading() const
{
    return thread::create_lock( new boost::shared_lock<boost::shared_mutex>(accessMutex) );
}

thread::lock_ptr CommonCamera::lockForWriting()
{
    return thread::create_lock( new boost::unique_lock<boost::shared_mutex>(accessMutex) );
}
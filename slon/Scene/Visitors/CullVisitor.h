#ifndef __SLON_ENGINE_SCENE_GRAPH_CULL_VISITOR_H__
#define __SLON_ENGINE_SCENE_GRAPH_CULL_VISITOR_H__

#include "NodeVisitor.h"

namespace slon {

// forward
namespace graphics {
    class Renderable;
}

namespace scene {

class Camera;
class Light;

/** CullVisitor performs some function on the geode renderable
 * that is not culled by the cull function.
 */
class CullVisitor :
    public NodeVisitor
{
public:
    typedef std::vector<const graphics::Renderable*>    renderable_vector;
    typedef renderable_vector::iterator                 renderable_iterator;
    typedef renderable_vector::const_iterator           renderable_const_iterator;

    typedef std::vector<const Light*>                   light_vector;
    typedef light_vector::iterator                      light_iterator;
    typedef light_vector::const_iterator                light_const_iterator;

public:
    CullVisitor(const Camera* camera_ = 0) :
        camera(camera_)
    {}

    /** Get camera of the CullVisitor */
    const Camera* getCamera() const { return camera; }

    /** Set camera for culling */
    void setCamera(const Camera* camera_) { camera = camera_; }

    /** Collect Renderable */
    void addRenderable(const graphics::Renderable* renderable) { renderables.push_back(renderable); }

    /** Collect Light */
    void addLight(const Light* light) { lights.push_back(light); }

    /** Get begin iterator of collected lights vector. */
    light_iterator beginLight() { return lights.begin(); }

    /** Get begin iterator of collected lights vector. */
    light_const_iterator beginLight() const { return lights.begin(); }

    /** Get end iterator of collected lights vector. */
    light_iterator endLight() { return lights.end(); }

    /** Get end iterator of collected lights vector. */
    light_const_iterator endLight() const { return lights.end(); }

    /** Get begin iterator of collected renderable vector. */
    renderable_iterator beginRenderable() { return renderables.begin(); }

    /** Get begin iterator of collected renderable vector. */
    renderable_const_iterator beginRenderable() const { return renderables.begin(); }

    /** Get end iterator of collected renderable vector. */
    renderable_iterator endRenderable() { return renderables.end(); }

    /** Get end iterator of collected renderable vector. */
    renderable_const_iterator endRenderable() const { return renderables.end(); }

    /** Remove all collected lights and renderables */
    void clear();

    // Override NodeVisitor
    virtual void acceptBy(Node& node) { node.accept(*this); }
    virtual void visitEntity(Entity& entity);

    virtual ~CullVisitor() {}

protected:
    const Camera*       camera;
    light_vector        lights;
    renderable_vector   renderables;
};

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_GRAPH_CULL_VISITOR_H__

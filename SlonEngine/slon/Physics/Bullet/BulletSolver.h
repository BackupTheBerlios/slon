#ifndef __SLON_ENGINE_PHYSICS_BULLET_SOLVER_H__
#define __SLON_ENGINE_PHYSICS_BULLET_SOLVER_H__

#include "../Forward.h"
#include <boost/intrusive/slist.hpp>

namespace slon {
namespace physics {

class BulletSolverCollector;

class BulletSolver
{
friend class BulletSolverCollector;
public:
    /** Resolve some physics */
    virtual void solve(real dt) = 0;

    /** Accept collector */
    virtual void accept(BulletSolverCollector& collector) = 0;

private:
    boost::intrusive::slist_member_hook<> collectorHook;
};

} // namespace slon
} // namespace physics

#endif // __SLON_ENGINE_PHYSICS_BULLET_SOLVER_H__
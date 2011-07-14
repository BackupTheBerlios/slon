#ifndef __SLON_ENGINE_PHYSICS_BULLET_SOLVER_H__
#define __SLON_ENGINE_PHYSICS_BULLET_SOLVER_H__

#include "../Forward.h"

namespace slon {
namespace physics {

/** Base class for solvers which should be processed every frame. Organized in list. */
class BulletSolver
{
public:
	BulletSolver()
	:	next(0)
	,	prev(0)
	{}

    /** Do some physics*/
    virtual void solve(real dt) = 0;

protected:
    ~BulletSolver() {}

public:
    BulletSolver* next;
	BulletSolver* prev;
};

} // namespace slon
} // namespace physics

#endif // __SLON_ENGINE_PHYSICS_BULLET_SOLVER_H__
#ifndef __SLON_ENGINE_PHYSICS_BULLET_SOLVER_COLLECTOR_H__
#define __SLON_ENGINE_PHYSICS_BULLET_SOLVER_COLLECTOR_H__

#include "BulletSolver.h"

namespace slon {
namespace physics {

class BulletSolverCollector
{
private:
    typedef boost::intrusive::member_hook< BulletSolver, 
                                           boost::intrusive::slist_member_hook<>, 
                                           &BulletSolver::collectorHook >    solver_option;

    typedef boost::intrusive::slist<BulletSolver, solver_option>             solver_list;

public:
    void clear()                         
    { 
        solvers.clear(); 
    }

    void addSolver(BulletSolver& solver) 
    { 
        solvers.push_front(solver);
    }

    void solve(btScalar dt);

private:
    solver_list solvers;
};

} // namespace slon
} // namespace physics

#endif // __SLON_ENGINE_PHYSICS_BULLET_SOLVER_COLLECTOR_H__
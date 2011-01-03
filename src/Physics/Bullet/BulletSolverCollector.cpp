#include "stdafx.h"
#include "Physics/Bullet/BulletSolverCollector.h"
#include <boost/bind.hpp>

namespace slon {
namespace physics {

void BulletSolverCollector::solve(btScalar dt)                         
{ 
    std::for_each( solvers.begin(), solvers.end(), boost::bind(&BulletSolver::solve, _1, dt) ); 
}

} // namespace slon
} // namespace physics

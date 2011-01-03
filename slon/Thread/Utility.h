#ifndef __SLON_ENGINE_THREAD_UTILITY_H__
#define __SLON_ENGINE_THREAD_UTILITY_H__

namespace slon {
namespace thread {

/** Sleep calling process. Use windows Sleep or unix usleep.
 * @param time - sleep time in seconds. Must be greater or equal
 * to 0.0 and less or equal to 1.0.
 */
void sleep(double time);


} // namespace thread
} // namespace slon

#endif // __SLON_ENGINE_THREAD_UTILITY_H__

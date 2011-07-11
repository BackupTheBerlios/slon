#ifndef __SLON_ENGINE_GRAPHICS_POST_PROCESS_COMMON_H__
#define __SLON_ENGINE_GRAPHICS_POST_PROCESS_COMMON_H__

namespace slon {
namespace graphics {
namespace ppu {

/** Stores most common ppu objects */
struct DrawUtilities
{
    /** Setup post process rendering chain: save viewport, bind
     * screen quad, disable culling.
     */
    static void beginChain();

    /** Restore states saved using beginChain. */
    static void endChain();
};

} // namespace ppu
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_POST_PROCESS_COMMON_H__

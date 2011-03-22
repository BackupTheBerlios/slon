#include "Network.h"

using namespace net;

#ifdef __GNUC__ 

// We must define static vars. For MSVC compilers it produces errors 
const int object_desc::header;
const int mesh_desc::header;
const int sky_box_desc::header;
const int object_transform_desc::header;
const int player_state_desc::header;
const int canon_desc::header;
const int kernel_desc::header;
const int fire_canon_desc::header;
const int player_damage_desc::header;

#endif
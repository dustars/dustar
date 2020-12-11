#pragma once

//Size of terrain
constexpr auto MAPWIDTH = 512;
constexpr auto MAPLENGTH = 512;

//Resolutoin of shadow map
constexpr auto SHADOWWIDTH = 1024;
constexpr auto SHADOWHEIGHT = 1024;

//#define OFFLINE
#define THREADING

//#define RENDER_CLOUD
//Optional: Using Compute Shader to render cloud in a texture
#ifdef RENDER_CLOUD
#define RENDER_CLOUD_CS
#endif

//#define ATMOSPHERE
//#define SQUARE_OBJECT
#define IMGUI
//#define SHADOW_MAPPING

// Macro POST_PROCESSING must be defined if
// there's any other post processing effect!
//#define POST_PROCESSING

//#define DEPTH_OF_FIELD

#ifndef SHADER_CONFIG
#define SHADER_CONFIG

/* Has to match, with which is defined inside the .c code. */
#define MSAA_SAMPLES 1

#define MAX_LIGHTS_POWER 10
#define MAX_LIGHTS (1 << MAX_LIGHTS_POWER)

/* This determines the tile size for light binning and associated tradeoffs */
#define CSHADER_TILE_GROUP_DIM 16
#define CSHADER_TILE_GROUP_SIZE (CSHADER_TILE_GROUP_DIM*CSHADER_TILE_GROUP_DIM)

#endif /* SHADER_CONFIG */


//
// Created by HXYP on 2023/4/15.
//

#ifndef ISLAND_DEFS_H
#define ISLAND_DEFS_H
#include <limits>
#include "glad/glad.h"

typedef unsigned int uint;
typedef long long Long;
typedef GLuint GLObject;

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#define DEFAULT_TEXTURE_DIFF    31
#define DEFAULT_TEXTURE_SPEC    30
#define DEFAULT_TEXTURE_REFL    29
#define DEFAULT_TEXTURE_NORM    28
#define DEFAULT_TEXTURE_PARA    27
#define DEFAULT_TEXTURE_AO      26
#define DEFAULT_TEXTURE_METAL   25
#define DEFAULT_TEXTURE_ROUGH   24

#define ENVIRONMENT_MAP_TEX     15

#define POINT_SHADOW_MAP0       16
#define POINT_SHADOW_MAP1       17
#define POINT_SHADOW_MAP2       18
#define POINT_SHADOW_MAP3       19
#define CSM_SHADOW_MAP0         20
#define CSM_SHADOW_MAP1         21
#define CSM_SHADOW_MAP2         22
#define CSM_SHADOW_MAP3         23

constexpr GLfloat FLOAT_INF = std::numeric_limits<GLfloat>::infinity();




#endif //ISLAND_DEFS_H

#ifndef BLUEBEAR_TYPES
#define BLUEBEAR_TYPES

#include <chrono>
#include <cstdint>

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;

typedef char				s8;
typedef short				s16;
typedef int					s32;

typedef std::chrono::duration< uint32_t, std::milli > millisecond_t;

#endif

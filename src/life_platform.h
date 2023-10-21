#if !defined(LIFE_PLATFORM_H)

// Header file that interfaces between win32 and app layers.
// Contains services that the app provides to the platform layer.


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef int8_t		s8;
typedef int16_t 	s16;
typedef int32_t 	s32;
typedef int64_t 	s64;
typedef s32			b32;

typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef float		f32;

#if APP_SLOW
#define ASSERT(expression) if (!(expression)) {*(int *)0 = 0;}
#else
#define ASSERT(expression)
#endif

#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))

#define KILOBYTES(kilobyte_count) 1024 * kilobyte_count
#define MEGABYTES(megabyte_count) 1024 * KILOBYTES(megabyte_count)
#define GIGABYTES(gigabyte_count) 1024 * MEGABYTES(gigabyte_count)
#define TERABYTES(terabyte_count) 1024 * GIGABYTES(terabyte_count)

#define HALF_ROOT_TWO 0.707106780f
#define PI32 3.1415926535897f

#define internal		static
#define local_persist	static
#define global_variable static

#define ABS(x) (((x) > 0) ? (x) : -(x))
#define MIN(a, b) ((a < b) ? (a) : (b))
#define MAX(a, b) ((a < b) ? (b) : (a))
#define SQUARE(x) ((x) * (x))
#define CUBE(x) ((x) * (x) * (x))

typedef struct
{
	void *contents;
	u32 size;
} debug_file_read;

internal void platform_file_free_memory(void *file_memory);
internal debug_file_read platform_file_read_entire(char *filename);

typedef struct
{
	void *memory;
	int width;
	int height;
	int bytes_per_pixel;
	int stride;

} back_buffer;

typedef struct
{
	u32 half_transition_count;
	b32 ended_down;
} app_button_state;

typedef struct
{
	union
	{
		app_button_state Buttons[6];
		struct
		{
			app_button_state Up;
			app_button_state Down;
			app_button_state Left;
			app_button_state Right;
			app_button_state Space;
			app_button_state Shift;
			app_button_state ArrowUp;
			app_button_state ArrowDown;
		};
	};
} app_controller_input;



typedef struct
{
	app_controller_input Controller;
	f32 dt_for_frame;
} app_input;

typedef struct
{
	u64 total_size;
	b32 is_initialized;
	void *permanent_storage;

} app_memory;

#ifdef __cplusplus
}
#endif
#define LIFE_PLATFORM_H
#endif

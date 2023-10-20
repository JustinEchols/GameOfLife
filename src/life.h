#if !defined(LIFE_H)

#include "life_platform.h"

struct string_u8
{
	u8 *data;
	u64 length;
};

#include "life_intrinsics.h"
#include "life_math.h"

enum cell_state
{
	CELL_DEAD,
	CELL_ALIVE

};

struct cell
{
	v2i Pos;
	b32 state;
	u32 neighbor_count;
};

struct grid
{
	u32 *cell_states;
	u32 *state_buffer;
	u32 cell_count;
	v2i CellDim;
	s32 cell_total_count_x;
	s32 cell_total_count_y;
};

struct app_state
{
	// TODO(Justin): Cells should be part of grid. Also is_alive and cell states
	// seems to copy info?
	grid Grid;

	b32 is_initialized;
};

#define LIFE_H
#endif

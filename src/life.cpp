
/*
 * Conway's Game of Life Rules:
 *	(1) A cell is alive or dead (rendered black and white, respectively.)
 *
 * If n is the number of alive neighbors of an alive or dead cell, then
 *
 *	(2) If alive and n = 2 or n = 3 it propagates. Otherwise it dies.
 *	(3) If dead and n = 3 it comes to life.
*/

/*
 * TODO(Justin):
 * Multi threading
 * SIMD
 * Cell update list?
 * Acive Region?
 * Cell File I/O
 *
 *
 *
*/

#include "life.h"

internal void 
rectangle_draw(back_buffer *BackBuffer, v2i Min, v2i Max, f32 r, f32 g, f32 b)
{
	__m128 ValueA = _mm_set1_ps(1.0f);
	__m128 ValueB = _mm_set1_ps(2.0f);
	__m128 Sum1 = _mm_add_ps(ValueA, ValueB);

	//__m128 ValueC = _mm_set1_ps(1.0f);
	//__m128 ValueD = _mm_set1_ps(2.0f);
	//__m128 Sum2 = _mm_add_ps(ValueA, ValueB);

	s32 x_min = Min.x;
	s32 y_min = Min.y;
	s32 x_max = Max.x;
	s32 y_max = Max.y;

	if(x_min < 0)
	{
		x_min += BackBuffer->width;
	}
	if(x_max > BackBuffer->width)
	{
		x_max -= BackBuffer->width;
	}
	if(y_min < 0)
	{
		y_min += BackBuffer->height;
	}
	if(y_max > BackBuffer->height)
	{
		y_max -= BackBuffer->height;
	}

	u32 red = f32_round_to_u32(255.0f * r);
	u32 green = f32_round_to_u32(255.0f * g);
	u32 blue = f32_round_to_u32(255.0f * b);
	u32 color = ((red << 16) | (green << 8) | (blue << 0));

	u8 *pixel_row = (u8 *)BackBuffer->memory + BackBuffer->stride * y_min + BackBuffer->bytes_per_pixel * x_min ;
	for(int row = y_min; row < y_max; row++)
	{
		u32 *pixel = (u32 *)pixel_row;
		for(int col = x_min; col < x_max; col++)
		{
			*pixel++ = color;
		}
		pixel_row += BackBuffer->stride;
	}
}

internal void
pixel_set(back_buffer *BackBuffer, v2f ScreenXY, u32 color)
{
	s32 screen_x = f32_round_to_s32(ScreenXY.x);
	s32 screen_y = f32_round_to_s32(ScreenXY.y);

	if(screen_x >= BackBuffer->width)
	{
		screen_x = screen_x - BackBuffer->width;
	}
	if(screen_x < 0)
	{ 
		screen_x = screen_x + BackBuffer->width;
	}
	if(screen_y >= BackBuffer->height)
	{
		screen_y = screen_y - BackBuffer->height;
	}
	if(screen_y < 0)
	{
		screen_y = screen_y + BackBuffer->height;
	}
	u8 *start = (u8 *)BackBuffer->memory + BackBuffer->stride * screen_y + BackBuffer->bytes_per_pixel * screen_x;
	u32 *pixel = (u32 *)start;
	*pixel = color;
}

internal u32
str_length(char *str)
{
	u32 Result = 0;
	for(char *c = str; *c != '\0'; c++)
	{
		Result++;
	}
	return(Result);
}

internal b32
str_are_same(char *str1, char *str2)
{
	// NOTE(Justin): Assume the strings are the same from the
	// outset, if we find that the lengths are in fact different, then they
	// are not the same and set Result to 0. If the lengths are the same
	// but the characters are not all the same, then the strings are
	// different and again set Result to 0.

	b32 Result = 1;

	u32 str1_length = str_length(str1);
	u32 str2_length = str_length(str2);
	if(str1_length == str2_length)
	{
		char *c1 = str1;
		char *c2 = str2;
		for(u32 char_index = 0; char_index < str1_length; char_index++)
		{
			if(*c1++ != *c2++)
			{
				Result = 0;
				break;
			}
		}
	}
	else
	{
		Result = 0;
	}
	return(Result);
}

internal string_u8
str_u8(char *str)
{
	string_u8 Result;
	Result.data = (u8 *)str;
	for(char *c = str; *c != '\0'; c++)
	{
		Result.length++;
	}
	return(Result);
}

internal void
line_vertical_draw(back_buffer *BackBuffer, f32 x, f32 r, f32 g, f32 b)
{
	s32 x_col = f32_round_to_s32(x);

	if(x_col < 0)
	{
		x_col = 0;
	}
	if(x_col >= BackBuffer->width)
	{
		x_col = BackBuffer->width - 1;
	}

	u32 red = f32_round_to_u32(255.0f * r);
	u32 green = f32_round_to_u32(255.0f * g);
	u32 blue = f32_round_to_u32(255.0f * b);
	u32 color = ((red << 16) | (green << 8) | (blue << 0));

	u8 *pixel_row = (u8 *)BackBuffer->memory + x_col * BackBuffer->bytes_per_pixel;
	for(s32 y = 0; y < BackBuffer->height; y++)
	{
		u32 *pixel = (u32 *)pixel_row;
		*pixel = color;
		pixel_row += BackBuffer->stride;
	}
}

internal void
line_horizontal_draw(back_buffer *BackBuffer, f32 y, f32 r, f32 g, f32 b) 
{
	s32 y_row = f32_round_to_s32(y);

	if(y_row < 0)
	{
		y_row = 0;
	}
	if(y_row >= BackBuffer->height)
	{
		y_row = BackBuffer->height - 1;
	}

	u32 red = f32_round_to_u32(255.0f * r);
	u32 green = f32_round_to_u32(255.0f * g);
	u32 blue = f32_round_to_u32(255.0f * b);
	u32 color = ((red << 16) | (green << 8) | (blue << 0));

	u8 *pixel_row = (u8 *)BackBuffer->memory + y_row * BackBuffer->stride;
	u32 *pixel = (u32 *)pixel_row;
	for(s32 x = 0; x < BackBuffer->width; x++)
	{
		*pixel++ = color;
	}
}

inline u32
colors_alpha_blend(u32 color_a, u32 color_b, f32 alpha)
{
	u32 Result;

	u32 color_a_red = ((color_a & 0xFF0000) >> 16);
	u32 color_a_green = ((color_a & 0xFF00) >> 8);
	u32 color_a_blue = (color_a & 0xFF);

	u32 color_b_red = ((color_b & 0xFF0000) >> 16);
	u32 color_b_green = ((color_b & 0xFF00) >> 8);
	u32 color_b_blue = (color_b & 0xFF);

	u32 result_red = f32_round_to_u32((1.0f - alpha) * color_a_red + alpha * color_b_red);
	u32 result_green = f32_round_to_u32((1.0f - alpha) * color_a_green + alpha * color_b_green);
	u32 result_blue = f32_round_to_u32((1.0f - alpha) * color_a_blue + alpha * color_b_blue);

	Result = ((result_red << 16) | (result_green << 8) | (result_blue << 0));
	return(Result);
}

internal void
rectangle_transparent_draw(back_buffer *BackBuffer, v2f Min, v2f Max, f32 r, f32 g, f32 b, f32 alpha)
{
	s32 x_min = f32_round_to_s32(Min.x);
	s32 y_min = f32_round_to_s32(Min.y);
	s32 x_max = f32_round_to_s32(Max.x);
	s32 y_max = f32_round_to_s32(Max.y);

	// Check bounds
	if(x_min < 0)
	{
		x_min = 0;
	}
	if(x_max > BackBuffer->width)
	{
		x_max = BackBuffer->width;
	}
	if(y_min < 0)
	{
		y_min = 0;
	}
	if(y_max > BackBuffer->height)
	{
		y_max = BackBuffer->height;
	}

	u32 red = f32_round_to_u32(255.0f * r);
	u32 green = f32_round_to_u32(255.0f * g);
	u32 blue = f32_round_to_u32(255.0f * b);
	u32 color = ((red << 16) | (green << 8) | (blue << 0));

	u8 *pixel_row = (u8 *)BackBuffer->memory + BackBuffer->stride * y_min + BackBuffer->bytes_per_pixel * x_min;
	for(int row = y_min; row < y_max; row++)
	{
		u32 *pixel = (u32 *)pixel_row;
		for(int col = x_min; col < x_max; col++)
		{
			*pixel++ = colors_alpha_blend(*pixel, color, alpha);
		}
		pixel_row += BackBuffer->stride;
	}
}

internal void
grid_cell_state_set(grid *Grid, v2i CellPos, u32 state)
{
	if((CellPos.x >= 0) && (CellPos.x < Grid->cell_total_count_x) &&
	   (CellPos.y >= 0) && (CellPos.y < Grid->cell_total_count_y))
	{
		Grid->cell_states[CellPos.y * Grid->cell_total_count_x + CellPos.x] = state;
	}
}

internal u32
grid_cell_state_get(grid *Grid, v2i CellPos)
{
	u32 Result = 0;
	if((CellPos.x >= 0) && (CellPos.x < Grid->cell_total_count_x) &&
	   (CellPos.y >= 0) && (CellPos.y < Grid->cell_total_count_y))
	{
		Result = Grid->cell_states[CellPos.y * Grid->cell_total_count_x + CellPos.x];
	}
	return(Result);
}

internal void
state_buffer_state_set(grid *Grid, v2i CellPos, u32 state)
{
	if((CellPos.x >= 0) && (CellPos.x < Grid->cell_total_count_x) &&
	   (CellPos.y >= 0) && (CellPos.y < Grid->cell_total_count_y))
	{
		Grid->state_buffer[CellPos.y * Grid->cell_total_count_x + CellPos.x] = state;
	}
}

internal u32
state_buffer_state_get(grid *Grid, v2i CellPos)
{
	u32 Result = 0;

	if((CellPos.x >= 0) && (CellPos.x < Grid->cell_total_count_x) &&
	   (CellPos.y >= 0) && (CellPos.y < Grid->cell_total_count_y))
	{
		Result = Grid->state_buffer[CellPos.y * Grid->cell_total_count_x + CellPos.x];
	}
	return(Result);
}

internal u32
cell_get_neighbor_count(grid *Grid, v2i CellPos)
{
	ASSERT((CellPos.x >= 0) && (CellPos.x < Grid->cell_total_count_x) &&
		   (CellPos.y >= 0) && (CellPos.y < Grid->cell_total_count_y));


	u32 Result = 0;

	if(CellPos.x == 0)
	{
		if(CellPos.y == 0)
		{
			Result += grid_cell_state_get(Grid, CellPos + V2I(0, 1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(1, 1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(1, 0));
		}
		else if((CellPos.y + 1) == Grid->cell_total_count_y)
		{
			Result += grid_cell_state_get(Grid, CellPos + V2I(0, -1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(1, -1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(1, 0));
		}
		else
		{
			Result += grid_cell_state_get(Grid, CellPos + V2I(0, 1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(1, 1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(1, 0));
			Result += grid_cell_state_get(Grid, CellPos + V2I(1, -1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(0, -1));
		}
	}
	else if((CellPos.x + 1) == Grid->cell_total_count_x)
	{
		if(CellPos.y == 0)
		{
			Result += grid_cell_state_get(Grid, CellPos + V2I(0, 1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(-1, 1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(-1, 0));
		}
		else if((CellPos.y + 1) == Grid->cell_total_count_y)
		{
			Result += grid_cell_state_get(Grid, CellPos + V2I(0, -1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(-1, -1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(-1, 0));
		}
		else
		{
			Result += grid_cell_state_get(Grid, CellPos + V2I(0, 1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(-1, 1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(-1, 0));
			Result += grid_cell_state_get(Grid, CellPos + V2I(-1, -1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(0, -1));
		}
	}
	else
	{
		if(CellPos.y == 0)
		{
			Result += grid_cell_state_get(Grid, CellPos + V2I(-1, 0));
			Result += grid_cell_state_get(Grid, CellPos + V2I(-1, 1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(0, 1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(1, 1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(1, 0));
		}
		else if((CellPos.y + 1) == Grid->cell_total_count_y)
		{
			Result += grid_cell_state_get(Grid, CellPos + V2I(-1, 0));
			Result += grid_cell_state_get(Grid, CellPos + V2I(-1, -1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(0, -1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(1, -1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(1, 0));
		}
		else
		{

			Result += grid_cell_state_get(Grid, CellPos + V2I(-1, 0));
			Result += grid_cell_state_get(Grid, CellPos + V2I(-1, 1));

			Result += grid_cell_state_get(Grid, CellPos + V2I(0, 1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(1, 1));

			Result += grid_cell_state_get(Grid, CellPos + V2I(1, 0));
			Result += grid_cell_state_get(Grid, CellPos + V2I(1, -1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(0, -1));
			Result += grid_cell_state_get(Grid, CellPos + V2I(-1, -1));
		}

	}
	return(Result);

}

//
// NOTE(Justin): Still lifes.
//

internal void
grid_create_block(grid *Grid, v2i CellPos)
{
	// NOTE(Justin): The position of the block is bottom left cell of the block.
	grid_cell_state_set(Grid, CellPos, CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, 1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(0, 1), CELL_ALIVE);
}

internal void
grid_create_tub(grid *Grid, v2i CellPos)
{
	// NOTE(Justin): The position of the tub is the center cell surrounded by the
	// four still life cells
	grid_cell_state_set(Grid, CellPos + V2I(-1, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(0, 1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(0, -1), CELL_ALIVE);
}

internal void
grid_create_boat(grid *Grid, v2i CellPos)
{
	// NOTE(Justin): The position of the boat is the center dead cell.
	grid_cell_state_set(Grid, CellPos + V2I(-1, -1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(-1, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(0, 1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(0, -1), CELL_ALIVE);
}

internal void
grid_create_ship(grid *Grid, v2i CellPos)
{
	// NOTE(Justin): The position of the ship is the center dead cell.
	grid_create_boat(Grid, CellPos);
	grid_cell_state_set(Grid, CellPos + V2I(1, 1), CELL_ALIVE);
	
}

internal void
grid_create_beehive(grid *Grid, v2i CellPos)
{
	grid_cell_state_set(Grid, CellPos, CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, 1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(2, 1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(3, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, -1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(2, -1), CELL_ALIVE);

}

internal void
grid_create_loaf()
{
}

internal void
grid_create_pond()
{
}

//
// NOTE(Justin): Begining non still lifes.
//

internal void
grid_create_horizontal_blinker(grid *Grid, v2i CellPos)
{
	// NOTE(Justin): The CellPos is the middle cell in the blinker configuration.
	grid_cell_state_set(Grid, CellPos + V2I(-1, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos, CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, 0), CELL_ALIVE);
}

internal void
grid_create_vertical_blinker(grid *Grid, v2i CellPos)
{
	// NOTE(Justin): The CellPos is the middle cell in the blinker configuration.
	grid_cell_state_set(Grid, CellPos + V2I(0, -1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos, CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(0, 1), CELL_ALIVE);
}

internal void
grid_create_traffic_light(grid *Grid, v2i CellPos)
{
	grid_create_horizontal_blinker(Grid, CellPos + V2I(0, 3));
	grid_create_horizontal_blinker(Grid, CellPos + V2I(0, -3));

	grid_create_vertical_blinker(Grid, CellPos + V2I(3, 0));
	grid_create_vertical_blinker(Grid, CellPos + V2I(-3, 0));
	
}

internal void
grid_create_glider(grid *Grid, v2i CellPos)
{
	grid_cell_state_set(Grid, CellPos + V2I(-1, -1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(0, -1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, -1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(0, 1), CELL_ALIVE);

}

internal void
grid_create_toad(grid *Grid, v2i CellPos)
{
	grid_cell_state_set(Grid, CellPos, CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(2, 0), CELL_ALIVE);

	grid_cell_state_set(Grid, CellPos + V2I(1, 1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(2, 1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(3, 1), CELL_ALIVE);
}

internal void
grid_create_pentadecathlon(grid *Grid, v2i CellPos)
{
	grid_cell_state_set(Grid, CellPos, CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(-1, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(-3, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(-4, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(-2, 1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(-2, -1), CELL_ALIVE);

	grid_cell_state_set(Grid, CellPos + V2I(1, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(2, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(4, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(5, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(3, 1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(3, -1), CELL_ALIVE);
}

internal void
grid_create_t_tetromino(grid *Grid, v2i CellPos)
{
	grid_cell_state_set(Grid, CellPos, CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(-1, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(0, 1), CELL_ALIVE);
}

internal void
grid_create_honey_farm(grid *Grid, v2i CellPos)
{

	grid_cell_state_set(Grid, CellPos, CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, -1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, 1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(2, -1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(2, 1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(3, 1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(3, 0), CELL_ALIVE);
}

internal void
grid_create_pi_heptomino(grid *Grid, v2i CellPos)
{
	grid_cell_state_set(Grid, CellPos + V2I(-1, -1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(-1, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(-1, 1), CELL_ALIVE);

	grid_cell_state_set(Grid, CellPos + V2I(0, 1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, 1), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, 0), CELL_ALIVE);
	grid_cell_state_set(Grid, CellPos + V2I(1, -1), CELL_ALIVE);
}

internal void
grid_create_stairstep_hexomino(grid *Grid, v2i CellPos)
{
}


internal void
grid_create_gosper_glider_gun(grid *Grid, v2i CellPos)
{
}


internal void
update_and_render(app_memory *AppMemory, back_buffer *BackBuffer, app_input *AppInput)
{

	BEGIN_TIMED_BLOCK(update_and_render);

	ASSERT(sizeof(app_state) <= AppMemory->total_size);
	app_state *AppState = (app_state *)AppMemory->permanent_storage;
	if(!AppMemory->is_initialized)
	{
		grid *Grid = &AppState->Grid;

		debug_file_read CellFile = platform_file_read_entire("1beacon.cells");

		Grid->CellDim = {4, 4};
		Grid->cell_total_count_x = BackBuffer->width / Grid->CellDim.x; 
		Grid->cell_total_count_y = BackBuffer->height / Grid->CellDim.y; 

		u8 *GridMemoryArenaBaseAddress = (u8 *)AppMemory->permanent_storage + sizeof(app_state);
		u64 GridMemoryArenaSize = Grid->cell_total_count_x * Grid->cell_total_count_y * sizeof(u32);

		Grid->cell_states = (u32 *)GridMemoryArenaBaseAddress;

		u8 *StateBufferMemoryArenaBaseAddress = (u8 *)GridMemoryArenaBaseAddress + GridMemoryArenaSize;
		u64 StateBufferMemoryArenaSize = GridMemoryArenaSize;

		Grid->state_buffer = (u32 *)StateBufferMemoryArenaBaseAddress;

		v2i GridMiddle = {Grid->cell_total_count_x / 2, Grid->cell_total_count_y / 2};
		v2i Offset = {10, 0};
		v2i Pos = GridMiddle;


		grid_create_boat(Grid, Pos);
		
		Pos = GridMiddle + Offset;
		grid_create_ship(Grid, Pos);

		Pos = GridMiddle + 2 * Offset;
		grid_create_beehive(Grid, Pos);

		Pos = GridMiddle + 3 * Offset;
		grid_create_traffic_light(Grid, Pos);

		Pos = GridMiddle + 4 * Offset;
		grid_create_glider(Grid, Pos);

		Offset = V2I(0, -50);
		Pos = GridMiddle + Offset;
		grid_create_stairstep_hexomino(Grid, Pos);


		AppMemory->is_initialized = true;
	}

	//
	// NOTE(Justin): Render
	//
	

	grid *Grid = &AppState->Grid;
	for(s32 cell_y = 0; cell_y < Grid->cell_total_count_y; cell_y++)
	{
		for(s32 cell_x = 0; cell_x < Grid->cell_total_count_x; cell_x++)
		{
			if((cell_x % 10 == 0) || (cell_y % 10 == 0))
			{
				line_vertical_draw(BackBuffer, (f32)(cell_x * (Grid->CellDim.x)), 0.0f, 0.0f, 0.0f);
				line_horizontal_draw(BackBuffer, (f32)(cell_y * (Grid->CellDim.y)), 0.0f, 0.0f, 0.0f);
			}
			else
			{
				line_vertical_draw(BackBuffer, (f32)(cell_x * (Grid->CellDim.x)), 0.5f, 0.5f, 0.5f);
				line_horizontal_draw(BackBuffer, (f32)(cell_y * (Grid->CellDim.y)), 0.5f, 0.5f, 0.5f);
			}


			v2i CellPos = V2I(cell_x, cell_y);
			u32 cell_state = grid_cell_state_get(Grid, CellPos);
			u32 neighbor_count = cell_get_neighbor_count(Grid, CellPos);

			v2i Min = CellPos;
			Min.x *= Grid->CellDim.x;
			Min.y *= Grid->CellDim.y;

			v2i Max = Min + Grid->CellDim;

			if(cell_state == CELL_ALIVE)
			{	

				rectangle_draw(BackBuffer, Min, Max, 0.0f, 0.0f, 0.0f);
			}
			else
			{
				rectangle_draw(BackBuffer, Min, Max, 1.0f, 1.0f, 1.0f);
			}
		}
	}

	// NOTE(Justin): We have to determine for each cell its state change
	// before updating any one of the cells. If we change the state of
	// a cell as we look at it, then this breaks the game becuase the
	// behavior of cells that we look at later will behave differently
	// because we changed the state of a previous cell. To avoid this
	// first determine the state change for each cell based on the current
	// state, remember the state change in a buffer, then change the state of each cell
	// using the state change we calculated and stored into the buffer.


	// NOTE(Justin): For each cell, calculate and store the new state in the state buffer.
	BEGIN_TIMED_BLOCK(render_grid);
	for(s32 cell_y = 0; cell_y < Grid->cell_total_count_y; cell_y++)
	{
		for(s32 cell_x = 0; cell_x < Grid->cell_total_count_x; cell_x++)
		{
			v2i CellPos = V2I(cell_x, cell_y);
			u32 cell_state = grid_cell_state_get(Grid, CellPos);
			u32 neighbor_count = cell_get_neighbor_count(Grid, CellPos);
			if(cell_state == CELL_ALIVE)
			{
				if((neighbor_count == 2) || (neighbor_count == 3))
				{
					state_buffer_state_set(Grid, CellPos, CELL_ALIVE);
				}
				else
				{
					state_buffer_state_set(Grid, CellPos, CELL_DEAD);
				}
			}
			else
			{
				if(neighbor_count == 3)
				{
					state_buffer_state_set(Grid, CellPos, CELL_ALIVE);
				}
			}
		}
	}
	END_TIMED_BLOCK(render_grid);

	// NOTE(Justin): For each cell, copy state from the state buffer to the grid memory arena.
	for(s32 cell_y = 0; cell_y < Grid->cell_total_count_y; cell_y++)
	{
		for(s32 cell_x = 0; cell_x < Grid->cell_total_count_x; cell_x++)
		{
			v2i CellPos = V2I(cell_x, cell_y);
			u32 cell_state = state_buffer_state_get(Grid, CellPos);
			grid_cell_state_set(Grid, CellPos, cell_state);
		}
	}

	if(AppInput->MouseController.Left.ended_down)
	{
		int mouse_x = AppInput->MouseController.Pos.x;
		int mouse_y = AppInput->MouseController.Pos.y;

		v2i OffsetInPixels = Grid->CellDim;
		v2i Min = V2I(mouse_x, mouse_y);
		v2i Max = Min + OffsetInPixels;
		Min = Min + -0.5f * OffsetInPixels;

		rectangle_draw(BackBuffer, Min, Max, 1.0f, 0.0f, 0.0f);
	}
	if(AppInput->MouseController.Right.ended_down)
	{
		int mouse_x = AppInput->MouseController.Pos.x;
		int mouse_y = AppInput->MouseController.Pos.y;

		v2i OffsetInPixels = Grid->CellDim;
		v2i Min = V2I(mouse_x, mouse_y);
		v2i Max = Min + OffsetInPixels;
		Min = Min + -0.5f * OffsetInPixels;

		rectangle_draw(BackBuffer, Min, Max, 1.0f, 1.0f, 0.0f);
	}

	END_TIMED_BLOCK(update_and_render);
}

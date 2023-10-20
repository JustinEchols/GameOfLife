#if !defined(LIFE_INTRINSICS_H)

inline f32
absolute_value(f32 x)
{
	f32 Result = fabs(x);
	return(Result);
}

inline s16
f32_round_to_s16(f32 x)
{
	s16 Result = (s16)floorf(x);
	return(Result);
}

inline s32
f32_round_to_s32(f32 x)
{
	s32 result = (s32)roundf(x);
	return(result);
}

inline u32
f32_round_to_u32(f32 x)
{
	u32 result = (u32)(x + 0.5f);
	return(result);
}

inline s32
f32_truncate_to_s32(f32 x)
{
	s32 Result = (s32)x;
	return(Result);
}

inline f32
f32_fractional_part(f32 x)
{
	f32 Result = 0.0f;
	if (x > 0) {
		Result = x - f32_round_to_s32(x);
	} else {
		Result = x - f32_round_to_s32(x + 1.0f);
	}
	return(Result);
}

inline s32
f32_floor_to_s32(f32 x)
{
	s32 Result = (s32)floorf(x);
	return(Result);
}

inline f32
f32_sin(f32 angle)
{
	f32 Result = sinf(angle);
	return(Result);
}

inline f32
f32_sqrt(f32 x)
{
	f32 Result = sqrtf(x);
	return(Result);
}

inline u32 
f32_ceil_to_u32(f32 x)
{
}

struct bit_scan_result
{
	u32 index;
	b32 found;
};

inline bit_scan_result
find_first_bit_set_u32(u32 x)
{
	bit_scan_result Result = {};
#if COMPILER_MSVC
	Result.found = _BitScanForward((unsigned long *)&Result.index, x);
#else
	for (u32 scan_index = 0; scan_index < 32; scan_index++) {
		if ((1 << scan_index) & x) {
			Result.index = scan_index;
			Result.found = true;
			break;
		}
	}
#endif
	return(Result);
}


#define LIFE_INTRINSICS_H
#endif

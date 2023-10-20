#if !defined(WIN32_LIFE_H)

typedef struct
{
	int width;
	int height;

} win32_client_dimensions;

typedef struct
{
	BITMAPINFO Info;
	int width;
	int height;
	int bytes_per_pixel;
	int stride;
	void *memory;

} win32_back_buffer;


#define WIN32_LIFE_H
#endif

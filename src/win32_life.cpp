#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include <dsound.h>

#include <math.h>

#include "life.cpp"
#include "win32_life.h"

global_variable b32					Win32GlobalRunning;
global_variable win32_back_buffer	Win32GlobalBackBuffer;
global_variable LARGE_INTEGER		Win32GlobalTickFrequency;
global_variable WINDOWPLACEMENT		GlobalWindowPosition = {sizeof(GlobalWindowPosition)};


internal void
win32_toggle_full_screen(HWND WindowHandle)
{
	DWORD window_style = GetWindowLong(WindowHandle, GWL_STYLE);
	if(window_style & WS_OVERLAPPEDWINDOW)
	{
		MONITORINFO MonitorInfo = {sizeof(MonitorInfo)};
		if(GetWindowPlacement(WindowHandle, &GlobalWindowPosition) && GetMonitorInfo(MonitorFromWindow(WindowHandle, MONITOR_DEFAULTTOPRIMARY), &MonitorInfo))
		{
			SetWindowLong(WindowHandle, GWL_STYLE,
					window_style & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(WindowHandle, HWND_TOP,
					MonitorInfo.rcMonitor.left, MonitorInfo.rcMonitor.top,
					MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left,
					MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
					SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}
	else
	{
		SetWindowLong(WindowHandle, GWL_STYLE, window_style | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(WindowHandle, &GlobalWindowPosition);
		SetWindowPos(WindowHandle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}	
}

internal void
win32_back_buffer_resize(win32_back_buffer *Win32GlobalBackBuffer, int width, int height)
{
	if(Win32GlobalBackBuffer->memory)
	{
		VirtualFree(Win32GlobalBackBuffer->memory, 0, MEM_RELEASE);
	}

	Win32GlobalBackBuffer->width = width;
	Win32GlobalBackBuffer->height = height;
	Win32GlobalBackBuffer->bytes_per_pixel = 4;
	Win32GlobalBackBuffer->stride = Win32GlobalBackBuffer->width * Win32GlobalBackBuffer->bytes_per_pixel;

	Win32GlobalBackBuffer->Info.bmiHeader.biSize = sizeof(Win32GlobalBackBuffer->Info.bmiHeader); // Necessary?
	Win32GlobalBackBuffer->Info.bmiHeader.biWidth = Win32GlobalBackBuffer->width;
	Win32GlobalBackBuffer->Info.bmiHeader.biHeight = Win32GlobalBackBuffer->height;
 
	Win32GlobalBackBuffer->Info.bmiHeader.biPlanes = 1;
	Win32GlobalBackBuffer->Info.bmiHeader.biBitCount = 32;
	Win32GlobalBackBuffer->Info.bmiHeader.biCompression = BI_RGB;

	Win32GlobalBackBuffer->Info.bmiHeader.biSizeImage = 0;
	Win32GlobalBackBuffer->Info.bmiHeader.biXPelsPerMeter = 0;
	Win32GlobalBackBuffer->Info.bmiHeader.biYPelsPerMeter = 0;
	Win32GlobalBackBuffer->Info.bmiHeader.biClrUsed = 0;
	Win32GlobalBackBuffer->Info.bmiHeader.biClrImportant = 0;

	int bitmap_memory_size =  
		Win32GlobalBackBuffer->width * Win32GlobalBackBuffer->height * Win32GlobalBackBuffer->bytes_per_pixel;

	Win32GlobalBackBuffer->memory = VirtualAlloc((LPVOID)0, bitmap_memory_size, 
			MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

LRESULT CALLBACK
WndProc(HWND WindowHandle, UINT Message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch(Message)
	{
		case WM_CLOSE:
		{
			Win32GlobalRunning = FALSE;
		} break;
		case WM_DESTROY:
		{
			// TODO: Handle as an error. Why?
		} break;
		case WM_QUIT:
		{
		} break;
		case WM_SIZE:
		{
			RECT ClientRect;
			GetClientRect(WindowHandle, &ClientRect);
			int client_width = ClientRect.right - ClientRect.left;
			int client_height = ClientRect.bottom - ClientRect.top;
			win32_back_buffer_resize(&Win32GlobalBackBuffer, client_width, client_height);
		} break;
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			// TODO(Justin): Assert macro
		} break;
		case WM_PAINT:
		{
			PAINTSTRUCT PaintStruct;
			HDC DeviceContext = BeginPaint(WindowHandle, &PaintStruct);

			StretchDIBits(DeviceContext,
					0, 0, Win32GlobalBackBuffer.width, Win32GlobalBackBuffer.height,
					0, 0, Win32GlobalBackBuffer.width, Win32GlobalBackBuffer.height,
					Win32GlobalBackBuffer.memory, &Win32GlobalBackBuffer.Info,
					DIB_RGB_COLORS, SRCCOPY);

			EndPaint(WindowHandle, &PaintStruct);
		} break;
		default:
		{
			result = DefWindowProc(WindowHandle, Message, wParam, lParam);
		} break;
	}
	return(result);
}

internal void
win32_process_pending_messgaes(app_controller_input *KeyboardController, app_mouse_input *MouseController)
{
	MSG Message; 
	while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
	{
		switch(Message.message)
		{
			case WM_QUIT:
			{
				Win32GlobalRunning = FALSE;
			} break;
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
				WPARAM vk_code = Message.wParam;
				s32 key_was_down = ((Message.lParam & (1 << 30)) != 0);
				s32 key_is_down = ((Message.lParam & (1 << 31)) == 0);
				if(key_was_down != key_is_down)
				{
					switch(vk_code)
					{
						case VK_LBUTTON:
						{
						} break;
						case VK_RBUTTON:
						{
						} break;
						case VK_LEFT:
						{
						} break;
						case VK_UP:
						{
						} break;
						case VK_DOWN:
						{
						} break;
						case VK_RIGHT:
						{
						} break;
						case VK_SPACE:
						{
							KeyboardController->Space.ended_down = key_is_down;
							KeyboardController->Space.half_transition_count++;
						} break;
						case 'W':
						{
							KeyboardController->Up.ended_down = key_is_down;
							KeyboardController->Up.half_transition_count++;
						} break;
						case 'A':
						{
							KeyboardController->Left.ended_down = key_is_down;
							KeyboardController->Left.half_transition_count++;
						} break;
						case 'S':
						{
							KeyboardController->Down.ended_down = key_is_down;
							KeyboardController->Down.half_transition_count++;
						} break;
						case 'D':
						{
							KeyboardController->Right.ended_down = key_is_down;
							KeyboardController->Right.half_transition_count++;
						} break;
						case VK_SHIFT:
						{
							KeyboardController->Shift.ended_down = key_is_down;
							KeyboardController->Shift.half_transition_count++;
						} break;
						case VK_ESCAPE:
						{
							OutputDebugStringA("Escape");
							Win32GlobalRunning = FALSE;
						} break;

					}
					if(key_is_down)
					{
						b32 alt_key_was_down = (Message.lParam & (1 << 29));
						if((vk_code == VK_RETURN) && (alt_key_was_down))
						{
							if(Message.hwnd)
							{
								win32_toggle_full_screen(Message.hwnd);
							}
						}
					}
				}
			}
			case WM_MOUSEMOVE:
			{
				MouseController->Pos.x = (Message.lParam & 0xFFFF);
				MouseController->Pos.y = (Win32GlobalBackBuffer.height - ((Message.lParam & (0xFFFF << 16)) >> 16));
			} break;
			case WM_LBUTTONDOWN:
			{
				MouseController->Left.ended_down = true;
				MouseController->Left.half_transition_count++;
			} break;
			case WM_LBUTTONUP:
			{
				MouseController->Left.ended_down = false;
				MouseController->Left.half_transition_count++;
			} break;

			case WM_RBUTTONDOWN:
			{
				MouseController->Right.ended_down = true;
				MouseController->Right.half_transition_count++;
			} break;
			case WM_RBUTTONUP:
			{
				MouseController->Right.ended_down = false;
				MouseController->Right.half_transition_count++;
			} break;
			default:
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			} break;
		}
	}
}

inline LARGE_INTEGER
win32_get_wall_clock(void)
{
	LARGE_INTEGER Result;
	QueryPerformanceCounter(&Result);
	return(Result);
}


internal void
platform_file_free_memory(void *file_memory)
{
	if(file_memory)
	{
		VirtualFree(file_memory, 0, MEM_RELEASE);
	}
}

internal debug_file_read
platform_file_read_entire(char *filename)
{
	debug_file_read  Result = {0};
	HANDLE FileHandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if(FileHandle != INVALID_HANDLE_VALUE)
	{
		DWORD FileSize = GetFileSize(FileHandle, &FileSize);
		if(FileSize != 0)
		{
			Result.contents = VirtualAlloc(0, FileSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			if(Result.contents)
			{
				DWORD BytesRead;
				ReadFile(FileHandle, Result.contents, FileSize, &BytesRead, 0);
				if(FileSize == BytesRead)
				{
					Result.size = FileSize;
				}
				else
				{
					platform_file_free_memory(Result.contents);
					Result.contents = 0;
				}
			}
			else
			{
			}
		}
		else
		{
		}
	}
	else
	{
		CloseHandle(FileHandle);
	}
	return(Result);
}

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR CmdLine, int nCmdShow)
{
	const char window_class_name[]	= "AppWindowClass";
	const char window_title[]		= "Life";

	WNDCLASSEX WindowClass;
	HWND WindowHandle;

	WindowClass.cbSize			= sizeof(WindowClass);
	WindowClass.style			= CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc		= WndProc;
	WindowClass.cbClsExtra		= 0;
	WindowClass.cbWndExtra		= 0;
	WindowClass.hInstance		= hInstance;
	WindowClass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	WindowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
	WindowClass.hbrBackground	= NULL;
	WindowClass.lpszMenuName	= NULL;
	WindowClass.lpszClassName	= window_class_name;
	WindowClass.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);


	// f	= 10MHz,
	// 1/f	= 1e-7  = 0.1 µs
	QueryPerformanceFrequency(&Win32GlobalTickFrequency);
	s64 ticks_per_second = Win32GlobalTickFrequency.QuadPart;
	f32 time_for_each_tick = 1.0f / (f32)ticks_per_second;

	u32 min_miliseconds_to_sleep = 1;
	MMRESULT sleep_is_granular = timeBeginPeriod(min_miliseconds_to_sleep);

	int monitor_hz = 60;
	int app_hz = monitor_hz / 2;
	f32 seconds_per_frame_target = 1.0f / (f32)app_hz;

	if(RegisterClassEx(&WindowClass))
	{
		WindowHandle = CreateWindowEx(0,
				window_class_name,
				window_title,
				WS_OVERLAPPEDWINDOW | WS_VISIBLE,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				0, 0, hInstance, 0);

		if(WindowHandle)
		{
			HDC DeviceContext = GetDC(WindowHandle);

			win32_back_buffer_resize(&Win32GlobalBackBuffer, 960, 540);

			app_memory AppMemory = {};
			AppMemory.total_size = MEGABYTES(32);
			AppMemory.permanent_storage = VirtualAlloc((LPVOID)0, AppMemory.total_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);


			if(AppMemory.permanent_storage)
			{
				app_input AppInput[2] = {};
				app_input *NewInput = &AppInput[0];
				app_input *OldInput = &AppInput[1];

				Win32GlobalRunning = TRUE;

				u64 cycle_count_last = __rdtsc();
				LARGE_INTEGER tick_count_last;

				QueryPerformanceCounter(&tick_count_last);

				//
				// NOTE(Justin): App loop.
				//

				f32 seconds_per_frame_actual = 0.0f;
				while(Win32GlobalRunning)
				{
					app_controller_input *NewKeyboardController = &NewInput->Controller;
					app_controller_input *OldKeyboardController = &OldInput->Controller;
					app_controller_input EmptyKeyboardController = {}; 
					*NewKeyboardController = EmptyKeyboardController;

					app_mouse_input *NewMouseController = &NewInput->MouseController;
					app_mouse_input *OldMouseController = &OldInput->MouseController;
					app_mouse_input EmptyMouseController = {}; 

					// NOTE(Justin): The mouse's cursor position needs to
					// persist from frame to frame. If the curosr position is
					// cleared each time, the position jumps back and forth from
					// (0, 0) to (x, y) which is not desired. Therefore when
					// when constructing new moues input, set the new input to
					// the empty controller, then set the new position to the
					// old position.

					*NewMouseController = EmptyMouseController;
					NewMouseController->Pos = OldMouseController->Pos;

					// TODO(Justin): Collapse keyboard/mouse input into one
					// loop.
					for(u32 button_index = 0;
							button_index < ARRAY_COUNT(NewKeyboardController->Buttons);
								button_index++)
					{
						NewKeyboardController->Buttons[button_index].ended_down =
							OldKeyboardController->Buttons[button_index].ended_down;
					}

					for(u32 button_index = 0;
							button_index < ARRAY_COUNT(NewMouseController->Buttons);
								button_index++)
					{
						NewMouseController->Buttons[button_index].ended_down =
							OldMouseController->Buttons[button_index].ended_down;
					}

					win32_process_pending_messgaes(NewKeyboardController, NewMouseController);


					back_buffer BackBuffer = {NULL, 0, 0, 0};
					BackBuffer.memory = Win32GlobalBackBuffer.memory;
					BackBuffer.width = Win32GlobalBackBuffer.width;
					BackBuffer.height = Win32GlobalBackBuffer.height;
					BackBuffer.bytes_per_pixel = Win32GlobalBackBuffer.bytes_per_pixel;
					BackBuffer.stride = Win32GlobalBackBuffer.stride;

					update_and_render(&AppMemory, &BackBuffer, NewInput);

					u64 cycle_count_end = __rdtsc();

					LARGE_INTEGER tick_count_end;
					QueryPerformanceCounter(&tick_count_end);

					s64 cycles_elapsed = cycle_count_end - cycle_count_last;
					s64 ticks_elapsed = tick_count_end.QuadPart - tick_count_last.QuadPart;

					seconds_per_frame_actual = (f32)ticks_elapsed / (f32)ticks_per_second;

					if(seconds_per_frame_actual < seconds_per_frame_target)
					{
						while(seconds_per_frame_actual < seconds_per_frame_target)
						{
							if(sleep_is_granular)
							{
								DWORD ms_to_sleep =
									(DWORD)(1000 *(seconds_per_frame_target - seconds_per_frame_actual));
								Sleep(ms_to_sleep);
							}
							LARGE_INTEGER tick_count_after_sleep;
							QueryPerformanceCounter(&tick_count_after_sleep);
							ticks_elapsed = tick_count_after_sleep.QuadPart - tick_count_last.QuadPart;
							seconds_per_frame_actual = (f32)ticks_elapsed / (f32)ticks_per_second;
						}
					}
					else
					{
						// NOTE(Justin): Missed frame
					}

					StretchDIBits(DeviceContext,
							0, 0, Win32GlobalBackBuffer.width, Win32GlobalBackBuffer.height,
							0, 0, Win32GlobalBackBuffer.width, Win32GlobalBackBuffer.height,
							Win32GlobalBackBuffer.memory, 
							&Win32GlobalBackBuffer.Info,
							DIB_RGB_COLORS, SRCCOPY);

					f32 fps = (f32)ticks_per_second / (f32)ticks_elapsed;
					f32 miliseconds_elapsed = (((1000 * (f32)ticks_elapsed) * time_for_each_tick));
					f32 megacycles_per_frame = (f32)(cycles_elapsed / (1000.0f * 1000.0f));

					char textbuffer[256];
					sprintf_s(textbuffer, sizeof(textbuffer),
							"%0.2fms/f %0.2ff/s %0.2fMc/f\n", miliseconds_elapsed, fps, megacycles_per_frame);

					tick_count_last = tick_count_end;
					cycle_count_last = cycle_count_end;

					NewInput->dt_for_frame = seconds_per_frame_actual;
					app_input *Temp = NewInput;
					NewInput = OldInput;
					OldInput = Temp;
				}
			}
		}
		else
		{
		}
	}
	else
	{
	}
	return 0;
} 

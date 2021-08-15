#include "universal.h"

#include "game.h"

#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../libs/stb/stb_image_write.h"

#include <windows.h>
#include <windowsx.h>
#include <xinput.h>


#ifndef UNICODE
#define UNICODE
#endif 

int window_width = 1600;
int window_height = 900;

int win32_running = 0;

// ============================================================================
// USER INPUT

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_GET_STATE(x_input_get_state);
typedef X_INPUT_SET_STATE(x_input_set_state);

X_INPUT_GET_STATE(XInputGetStateStub) {
	return ERROR_DEVICE_NOT_CONNECTED;
}
X_INPUT_SET_STATE(XInputSetStateStub) {
	return ERROR_DEVICE_NOT_CONNECTED;
}

// The assigments are in win32_LoadXInput because C doesn't like the assignments
static x_input_get_state* XInputGetState_; // = XInputGetStateStub;
static x_input_set_state* XInputSetState_; // = XInputSetStateStub;
#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_

static void win32_LoadXInput() {
	XInputGetState_ = XInputGetStateStub;
	XInputSetState_ = XInputSetStateStub;

	// If we don't have xinput1.4 check for xinput1.3
	HMODULE XInputLibrary;
	XInputLibrary = LoadLibraryA("xinput1_4.dll");
	if (!XInputLibrary) {
		XInputLibrary = LoadLibraryA("xinput9_1_0.dll");
	}
	if (!XInputLibrary) {
		XInputLibrary = LoadLibraryA("xinput1_3.dll");
	}
	if (XInputLibrary) {
		XInputGetState = (x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
		XInputSetState = (x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
	}
}

// // something for controllers?
// internal void win32_ProcessXInputDigitalButton(ButtonState* oldstate, ButtonState* newstate, DWORD buttonBit, DWORD xinputButtonState) {
//  newstate->ended_down = ((xinputButtonState & buttonBit) == buttonBit);
//  newstate->transition_count = (oldstate->ended_down != newstate->ended_down) ? 1 : 0;
//}

static void win32_ProcessKeyboardMessage(ButtonState* newState, b32 isDown) {
	assert(newState->ended_down != isDown);
	newState->ended_down = isDown;
	newState->transition_count += 1;
}

static void win32_UpdateInput(Input* gameInput, HWND window) {
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
		switch (message.message) {
			case WM_QUIT: win32_running = false; break;

			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
				u32 vkCode = (u32)message.wParam;
				b32 wasDown = ((message.lParam & (1 << 30)) != 0);
				b32 isDown = ((message.lParam & (1 << 31)) == 0);

				if (isDown != wasDown) {
					switch (vkCode) {
						case 0x30: win32_ProcessKeyboardMessage(&gameInput->keyboard.zero, isDown); break;
						case 0x31: win32_ProcessKeyboardMessage(&gameInput->keyboard.one, isDown); break;
						case 0x32: win32_ProcessKeyboardMessage(&gameInput->keyboard.two, isDown); break;
						case 0x33: win32_ProcessKeyboardMessage(&gameInput->keyboard.three, isDown); break;
						case 0x34: win32_ProcessKeyboardMessage(&gameInput->keyboard.four, isDown); break;
						case 0x35: win32_ProcessKeyboardMessage(&gameInput->keyboard.five, isDown); break;
						case 0x36: win32_ProcessKeyboardMessage(&gameInput->keyboard.six, isDown); break;
						case 0x37: win32_ProcessKeyboardMessage(&gameInput->keyboard.seven, isDown); break;
						case 0x38: win32_ProcessKeyboardMessage(&gameInput->keyboard.eight, isDown); break;
						case 0x39: win32_ProcessKeyboardMessage(&gameInput->keyboard.nine, isDown); break;

						case 'A': win32_ProcessKeyboardMessage(&gameInput->keyboard.a, isDown); break;
						case 'B': win32_ProcessKeyboardMessage(&gameInput->keyboard.b, isDown); break;
						case 'C': win32_ProcessKeyboardMessage(&gameInput->keyboard.c, isDown); break;
						case 'D': win32_ProcessKeyboardMessage(&gameInput->keyboard.d, isDown); break;
						case 'E': win32_ProcessKeyboardMessage(&gameInput->keyboard.e, isDown); break;
						case 'F': win32_ProcessKeyboardMessage(&gameInput->keyboard.f, isDown); break;
						case 'G': win32_ProcessKeyboardMessage(&gameInput->keyboard.g, isDown); break;
						case 'H': win32_ProcessKeyboardMessage(&gameInput->keyboard.h, isDown); break;
						case 'I': win32_ProcessKeyboardMessage(&gameInput->keyboard.i, isDown); break;
						case 'J': win32_ProcessKeyboardMessage(&gameInput->keyboard.j, isDown); break;
						case 'K': win32_ProcessKeyboardMessage(&gameInput->keyboard.k, isDown); break;
						case 'L': win32_ProcessKeyboardMessage(&gameInput->keyboard.l, isDown); break;
						case 'M': win32_ProcessKeyboardMessage(&gameInput->keyboard.m, isDown); break;
						case 'N': win32_ProcessKeyboardMessage(&gameInput->keyboard.n, isDown); break;
						case 'O': win32_ProcessKeyboardMessage(&gameInput->keyboard.o, isDown); break;
						case 'P': win32_ProcessKeyboardMessage(&gameInput->keyboard.p, isDown); break;
						case 'Q': win32_ProcessKeyboardMessage(&gameInput->keyboard.q, isDown); break;
						case 'R': win32_ProcessKeyboardMessage(&gameInput->keyboard.r, isDown); break;
						case 'S': win32_ProcessKeyboardMessage(&gameInput->keyboard.s, isDown); break;
						case 'T': win32_ProcessKeyboardMessage(&gameInput->keyboard.t, isDown); break;
						case 'U': win32_ProcessKeyboardMessage(&gameInput->keyboard.u, isDown); break;
						case 'V': win32_ProcessKeyboardMessage(&gameInput->keyboard.v, isDown); break;
						case 'W': win32_ProcessKeyboardMessage(&gameInput->keyboard.w, isDown); break;
						case 'X': win32_ProcessKeyboardMessage(&gameInput->keyboard.x, isDown); break;
						case 'Y': win32_ProcessKeyboardMessage(&gameInput->keyboard.y, isDown); break;
						case 'Z': win32_ProcessKeyboardMessage(&gameInput->keyboard.z, isDown); break;

						case VK_LBUTTON: win32_ProcessKeyboardMessage(&gameInput->mouse.left, isDown); break;
						case VK_RBUTTON: win32_ProcessKeyboardMessage(&gameInput->mouse.right, isDown); break;
						case VK_MBUTTON: win32_ProcessKeyboardMessage(&gameInput->mouse.middle, isDown); break;
						case VK_XBUTTON1: win32_ProcessKeyboardMessage(&gameInput->mouse.x1, isDown); break;
						case VK_XBUTTON2: win32_ProcessKeyboardMessage(&gameInput->mouse.x2, isDown); break;

						case VK_UP: win32_ProcessKeyboardMessage(&gameInput->keyboard.up, isDown); break;
						case VK_DOWN: win32_ProcessKeyboardMessage(&gameInput->keyboard.down, isDown); break;
						case VK_LEFT: win32_ProcessKeyboardMessage(&gameInput->keyboard.left, isDown); break;
						case VK_RIGHT: win32_ProcessKeyboardMessage(&gameInput->keyboard.right, isDown); break;

						case VK_SPACE: win32_ProcessKeyboardMessage(&gameInput->keyboard.space, isDown); break;
						case VK_ESCAPE: win32_ProcessKeyboardMessage(&gameInput->keyboard.escape, isDown); break;
					}
				}

				b32 altKeyDown = ((message.lParam & (1 << 29)) != 0);
				if ((vkCode == VK_F4) && altKeyDown) {
					win32_running = false;
				}
			} break;

			case WM_LBUTTONDOWN:
			{
				gameInput->mouse.left.ended_down = 1;
				gameInput->mouse.left.transition_count = 1;
			} break;
			case WM_LBUTTONUP:
			{
				gameInput->mouse.left.ended_down = 0;
				gameInput->mouse.left.transition_count = 1;
			} break;
			case WM_MBUTTONDOWN:
			{
				gameInput->mouse.middle.ended_down = 1;
				gameInput->mouse.middle.transition_count = 1;
			} break;
			case WM_MBUTTONUP:
			{
				gameInput->mouse.middle.ended_down = 0;
				gameInput->mouse.middle.transition_count = 1;
			} break;
			case WM_RBUTTONDOWN:
			{
				gameInput->mouse.right.ended_down = 1;
				gameInput->mouse.right.transition_count = 1;
			} break;
			case WM_RBUTTONUP:
			{
				gameInput->mouse.right.ended_down = 0;
				gameInput->mouse.right.transition_count = 1;
			} break;
			case WM_XBUTTONDOWN:
			{
				DWORD fwButton = GET_XBUTTON_WPARAM(message.wParam);
				if (fwButton == XBUTTON1) {
					gameInput->mouse.x1.ended_down = 1;
					gameInput->mouse.x1.transition_count = 1;
				}
				else {
					gameInput->mouse.x2.ended_down = 1;
					gameInput->mouse.x2.transition_count = 1;
				}
			} break;
			case WM_XBUTTONUP:
			{
				DWORD fwButton = GET_XBUTTON_WPARAM(message.wParam);
				if (fwButton == XBUTTON1) {
					gameInput->mouse.x1.ended_down = 0;
					gameInput->mouse.x1.transition_count = 1;
				}
				else {
					gameInput->mouse.x2.ended_down = 0;
					gameInput->mouse.x2.transition_count = 1;
				}
			} break;

			case WM_MOUSEMOVE:
			{
				// mouse move is for only when action is needed during movement

				//int32_t xPos = GET_X_LPARAM(message.lParam);
				//int32_t yPos = GET_Y_LPARAM(message.lParam);

				/*
				// Check for if modifer keys are pressed while moving mouse
				uint32_t modifierKeyFlags = message.wParam;
				if (modifierKeyFlags & MK_CONTROL) {}
				if (modifierKeyFlags & MK_LBUTTON) {}
				if (modifierKeyFlags & MK_RBUTTON) {}
				if (modifierKeyFlags & MK_MBUTTON) {}
				if (modifierKeyFlags & MK_SHIFT){}
				if (modifierKeyFlags & MK_XBUTTON1){}
				if (modifierKeyFlags & MK_XBUTTON2) {}
				*/

				//gameInput->mouse.x = xPos;
				//gameInput->mouse.y = yPos;
			} break;

			default:
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			} break;
		}
	}
	POINT cursor_point;
	if (GetCursorPos(&cursor_point)) {
		if (ScreenToClient(window, &cursor_point)) {
			gameInput->mouse.x = cursor_point.x;
			gameInput->mouse.y = cursor_point.y;
		}
	}
}

// end USER INPUT
// ============================================================================
// FILE IO

debug_ReadFileResult debug_ReadFile(char* filename) {
	debug_ReadFileResult result = {};
	HANDLE file = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	if (file != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER fileSize;

		if (GetFileSizeEx(file, &fileSize)) {
			assert(fileSize.QuadPart <= 0xFFFFFFFF);
			result.data = VirtualAlloc(0, (SIZE_T)fileSize.QuadPart, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

			if (result.data) {
				DWORD bytesRead;
				if (ReadFile(file, result.data, (i32)fileSize.QuadPart, &bytesRead, 0) && (bytesRead == fileSize.QuadPart)) {
					result.size = fileSize.QuadPart;
				}
				else {
					debug_FreeFile(result.data);
					result.data = 0;
				}
			}
		}
		CloseHandle(file);
	}
	return result;
}

b32 debug_WriteFile(char* filename, u32 memorySize, void* memory) {
  b32 result = 0;
  HANDLE file = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

  if (file != INVALID_HANDLE_VALUE) {
	DWORD bytesWritten;

	if (WriteFile(file, memory, memorySize, &bytesWritten, 0)) {
	  result = (bytesWritten == memorySize);
	}
	else {
	  // error
	}
	CloseHandle(file);
  }
  return result;
}

void debug_FreeFile(void* memory) {
	if (memory) {
		VirtualFree(memory, 0, MEM_RELEASE);
	}
}

// end FILE IO
// ============================================================================
// Memory

PermanentResourceAllocator::PermanentResourceAllocator(i64 size) {
	this->size = size;
	this->offset = 0;

	this->backing_buffer = (uchar*)VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void* PermanentResourceAllocator::Allocate(i64 alloc_size) {
	if (offset + alloc_size < size) {
		void* temp = &backing_buffer[offset];
		offset += alloc_size;
		memset(temp, 0, alloc_size);
		return temp;
	}
	OutputDebugStringA("PermanentResourceAllocator ran out of memory\n");
	return NULL;
}

void PermanentResourceAllocator::Free() {
	offset = 0;
}

void PermanentResourceAllocator::FreeBackingBuffer() {
	if (backing_buffer) {
		VirtualFree(backing_buffer, 0, MEM_RELEASE);
	}
	size = 0;
	offset = 0;
}

// end Memory
// ============================================================================

void DebugPrint(char* str) {
	OutputDebugStringA(str);
}

struct win32_WindowDimension {
	i32 width;
	i32 height;
};

static win32_WindowDimension win32_GetWindowDimension(HWND window) {
	RECT client_rect;
	GetClientRect(window, &client_rect);

	win32_WindowDimension window_dimension = {};
	window_dimension.width = client_rect.right - client_rect.left;
	window_dimension.height = client_rect.bottom - client_rect.top;
	return window_dimension;
}

void PrepareRenderData(Memory* game_memory, RenderData* render_data) {
	GameState* gs = (GameState*)game_memory->data;

	int iter = 0;
	render_data->entities[iter++] = { gs->blackGuyHead.render_pos, gs->blackGuyHead.render_scale, gs->blackGuyHead.render_rot_axis, gs->blackGuyHead.render_rot_angle, 0, 0 };
	render_data->entities[iter++] = { gs->blackGuyHead2.render_pos, gs->blackGuyHead2.render_scale, gs->blackGuyHead2.render_rot_axis, gs->blackGuyHead2.render_rot_angle, 0, 0 };
	
	render_data->entities[iter++] = { gs->bunnyTest.render_pos, gs->bunnyTest.render_scale, gs->bunnyTest.render_rot_axis, gs->bunnyTest.render_rot_angle, 2, 2 };
	render_data->entities[iter++] = { gs->bunnyTest2.render_pos, gs->bunnyTest2.render_scale, gs->bunnyTest2.render_rot_axis, gs->bunnyTest2.render_rot_angle, 2, 2 };
	
	for (int i = 0; i < 7; i++) {
		render_data->entities[iter++] = { gs->cubes[i].render_pos, gs->cubes[i].render_scale, gs->cubes[i].render_rot_axis, gs->cubes[i].render_rot_angle, 4, 3 + i };
	}
	render_data->entities[iter++] = { gs->quad.render_pos, gs->quad.render_scale, gs->quad.render_rot_axis, gs->quad.render_rot_angle, 5, 0 };

	render_data->num_entities = iter;

	for (int i = 0; i < render_data->num_entities; i++) {
		render_data->entities[i].texture_index = 10;
	}
}

void PrepareText(char* str, int str_len, int* num_chars_visible, int xpos, int ypos, Font* font, TextVertex* verts, win32_WindowDimension scr) {
	/* calculate the origin of the string */
	f32 scale = stbtt_ScaleForPixelHeight(&(font->info), font->size);

	i32 ascent, descent, linegap;
	stbtt_GetFontVMetrics(&(font->info), &ascent, &descent, &linegap);

	i32 first_char_lsb;
	stbtt_GetCodepointHMetrics(&(font->info), str[0], 0, &first_char_lsb);

	f32 horz_pos = xpos + (scale * first_char_lsb) + 2;
	f32 baseline = ypos + (scale * ((f32)ascent - (f32)descent + (f32)linegap));

	int vi = 0;
	int num_new_lines = 1;
	for (i32 i = 0; i < str_len; i++) {
		if (str[i] == '\n') {
			/* if we hit a new line indicator, we need to adjust the baseline */
			num_new_lines++;
			stbtt_GetFontVMetrics(&(font->info), &ascent, &descent, &linegap);
			baseline = ypos + num_new_lines * (scale * ((f32)ascent - (f32)descent + (f32)linegap));
			horz_pos = xpos + (scale * first_char_lsb) + 2;
			continue;
		}
		
		stbtt_aligned_quad vd; // vertex_data, holds position and texcoord data
		stbtt_GetPackedQuad(
			font->chardata_for_range, 256, 128,
			str[i], &horz_pos, &baseline, &vd, 1
		);

		/* vd is 0,0 top left and scr_wid, scr_hgt bottom right */
		/* so we have to scale it to the NDC */
		vd.x0 = (vd.x0 / (f32)scr.width);
		vd.x1 = (vd.x1 / (f32)scr.width);

		vd.y0 = (vd.y0 / (f32)scr.height);
		vd.y1 = (vd.y1 / (f32)scr.height);

		/* scale x's and y's from [0,1] to [-1,1] */
		/* y's are inverted to try to solve stb using different coordinates from DX */

		vd.x0 = vd.x0 * 2.0f - 1.0f;
		vd.x1 = vd.x1 * 2.0f - 1.0f;

		vd.y0 = -(vd.y0 * 2.0f - 1.0f);
		vd.y1 = -(vd.y1 * 2.0f - 1.0f);
		
		verts[vi + 0] = { vd.x0, vd.y0, 0, 1, 1, 1, 1, vd.s0, vd.t0 };
		verts[vi + 1] = { vd.x1, vd.y0, 0, 1, 1, 1, 1, vd.s1, vd.t0 };
		verts[vi + 2] = { vd.x0, vd.y1, 0, 1, 1, 1, 1, vd.s0, vd.t1 };
		verts[vi + 3] = { vd.x1, vd.y1, 0, 1, 1, 1, 1, vd.s1, vd.t1 };
		vi += 4;

		// --------------------------------------- 
		// TODO: re add kerning? 
		i32 kern = stbtt_GetCodepointKernAdvance(&(font->info), str[i], str[i + 1]);
		horz_pos += (f32)kern * scale;
	}

	*num_chars_visible = vi / 4;
}

// ============================================================================


LRESULT CALLBACK win32_WindowCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	switch (message) {
		case WM_CREATE:
		{

			OutputDebugStringW(L"WM_CREATE\n");
		} break;
		case WM_SIZE: 
		{
			//if (FAILED(renderer.RendererResize(hwnd, LOWORD(lParam), HIWORD(lParam)))) {
			//	DestroyWindow(hwnd);
			//}
			OutputDebugStringW(L"WM_SIZE\n");
		} break;
		case WM_DESTROY: { win32_running = 0; OutputDebugStringW(L"WM_DESTROY\n"); } break;
		case WM_CLOSE: { win32_running = 0; OutputDebugStringW(L"WM_CLOSE\n"); } break;
		case WM_ACTIVATEAPP: OutputDebugStringW(L"WM_ACTIVATEAPP\n"); break;
		
		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
		{
			OutputDebugStringW(L"Keyboard Event in windows callback function\n");
			assert(0);
		} break;
		default:
		{
			result = DefWindowProcW(hwnd, message, wParam, lParam);
		} break;
	}

	return result;
}


/*
struct AppThreadInput {
	HWND window;
};

DWORD WINAPI AppThread(LPVOID param) {
	// AppThreadInput* input = (AppThreadInput*)param;
	
	OutputDebugStringA((char*)"APP THREAD REPORTING IN\n");
	
	return 0;
}
*/

int WINAPI wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hprevinstance, _In_ LPWSTR pCmdLine, _In_ int nCmdShow) {
	srand(0);
	
	win32_LoadXInput();
	
	stbi_set_flip_vertically_on_load(1);

	// Timing Info
	LARGE_INTEGER perftimerfreqresult;
	QueryPerformanceFrequency(&perftimerfreqresult);
	i64 perftimerfreq = perftimerfreqresult.QuadPart;

	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = win32_WindowCallback;
	window_class.hInstance = hinstance;
	// window_class.hIcon = ;
	window_class.lpszClassName = L"EngineWindowClass";

	if (RegisterClass(&window_class)) {
		HWND window = CreateWindowExW(
			0, window_class.lpszClassName, L"Game",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT,
			window_width, window_height, 0, 0, hinstance, 0
		);

		if (window) {
			//DWORD app_thread_handle;
			//AppThreadInput app_thread_input = {};
			//HANDLE handle = CreateThread(NULL, 0, AppThread, &app_thread_input, 0, &app_thread_handle);
			
			//WaitForSingleObject(handle, INFINITE);

			//CloseHandle(handle);

			win32_WindowDimension dim = win32_GetWindowDimension(window);
			
			win32_running = 1;

			LARGE_INTEGER lasttimer;
			QueryPerformanceCounter(&lasttimer);
			u64 lastcyclecount = __rdtsc();

			Input game_input[2] = {};
			Input* new_input = &game_input[0];
			Input* old_input = &game_input[1];

			Memory game_memory = {};
			game_memory.is_initialized = 0;
			game_memory.size = Kilobytes((u64)1);
			game_memory.data = VirtualAlloc(0, (SIZE_T)game_memory.size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

			InitGameState(&game_memory, Vec2((f32)dim.width, (f32)dim.height));

			VertexBuffer vertex_buffer;
			IndexBuffer index_buffer;
			ModelBuffer model_buffer;
			PermanentResourceAllocator renderer_allocator(Gigabytes((u64)4));
			FrameAllocator frame_allocator(Megabytes((u64)64));

			InitRenderer(&vertex_buffer, &index_buffer, &model_buffer, &renderer_allocator);
			
			LoadOBJ((char*)"test_assets/african_head.obj", &vertex_buffer, &index_buffer, &model_buffer, &frame_allocator);
			LoadOBJ((char*)"test_assets/deer.obj", &vertex_buffer, &index_buffer, &model_buffer, &frame_allocator);
			LoadOBJ((char*)"test_assets/bunny.obj", &vertex_buffer, &index_buffer, &model_buffer, &frame_allocator);
			LoadOBJ((char*)"test_assets/tree_default.obj", &vertex_buffer, &index_buffer, &model_buffer, &frame_allocator);
			LoadOBJ((char*)"test_assets/cube.obj", &vertex_buffer, &index_buffer, &model_buffer, &frame_allocator);
			LoadOBJ((char*)"test_assets/quad.obj", &vertex_buffer, &index_buffer, &model_buffer, &frame_allocator);
			
			char game_debug_text[1024];
			const i32 FONT_SIZE = 20;
			Image font_image;
			Font font;
			{
				/* TODO: Decide if fontsize should be made negative or not */
				font.size = FONT_SIZE;

				/* load font from file */
				FILE* font_file;
				fopen_s(&font_file, "C:/Windows/Fonts/Arial.ttf", "rb");
				fseek(font_file, 0, SEEK_END);
				i32 font_buf_size = ftell(font_file);
				fseek(font_file, 0, SEEK_SET);
				font.font_buf = (uchar*)malloc(font_buf_size); // malloc

				fread(font.font_buf, font_buf_size, 1, font_file);
				fclose(font_file);

				/* prepare font */
				if (!stbtt_InitFont(&(font.info), font.font_buf, 0)) {
					DebugPrint((char*)"Failed to init font\n");
				}

				/* Packing fonts ---------------------------------------*/
				/* packed font bitmap init */
				font_image.width = 256, font_image.height = 128;
				uchar* pack_font_1bpp = (uchar*)calloc(font_image.width * font_image.height, sizeof(uchar));

				/* stride is 0 because we are tightly packed */
				/* 1 for padding because filtering */
				/* NULL because no allocator */
				stbtt_pack_context spc;
				stbtt_PackBegin(
					&spc, pack_font_1bpp,
					font_image.width, font_image.height,
					0, 1, NULL
				);

				/* 0 because i only want the first font */
				i32 which_font_in_the_ttf = 0;

				/* 0 and 256 because i want all of ascii */
				i32 first_unicode_char_to_pull = 0;
				i32 num_chars_in_range = 256;

				i32 res = stbtt_PackFontRange(
					&spc, font.font_buf, which_font_in_the_ttf, font.size,
					first_unicode_char_to_pull, num_chars_in_range, font.chardata_for_range
				);
				if (res != 1) {
					DebugPrint((char*)"possible font packing error\n");
				}

				stbtt_PackEnd(&spc);
				/* done with the packing context -------------------------*/

				font_image.data = (uchar*)calloc(font_image.width * font_image.height, 4 * sizeof(uchar));
				for (i32 i = 0; i < font_image.width * font_image.height; i++) {
					font_image.data[4 * i + 0] = 255;
					font_image.data[4 * i + 1] = 255;
					font_image.data[4 * i + 2] = 255;
					font_image.data[4 * i + 3] = pack_font_1bpp[i];
				}
			}

			int n, iter = 0;
			Image* images = (Image*)renderer_allocator.Allocate(sizeof(Image) * 16);

			images[iter].data = stbi_load((char*)"test_assets/african_head_diffuse.tga", &images[iter].width, &images[iter].height, &n, 4);
			iter += 1;
			images[iter].data = stbi_load((char*)"test_assets/grass.png", &images[iter].width, &images[iter].height, &n, 4);
			iter += 1;
			images[iter].data = stbi_load((char*)"test_assets/bunny.png", &images[iter].width, &images[iter].height, &n, 4);
			iter += 1;
			images[iter].data = stbi_load((char*)"test_assets/red.png", &images[iter].width, &images[iter].height, &n, 4);
			iter += 1;
			images[iter].data = stbi_load((char*)"test_assets/orange.png", &images[iter].width, &images[iter].height, &n, 4);
			iter += 1;
			images[iter].data = stbi_load((char*)"test_assets/yellow.png", &images[iter].width, &images[iter].height, &n, 4);
			iter += 1;
			images[iter].data = stbi_load((char*)"test_assets/green.png", &images[iter].width, &images[iter].height, &n, 4);
			iter += 1;
			images[iter].data = stbi_load((char*)"test_assets/cyan.png", &images[iter].width, &images[iter].height, &n, 4);
			iter += 1;
			images[iter].data = stbi_load((char*)"test_assets/blue.png", &images[iter].width, &images[iter].height, &n, 4);
			iter += 1;
			images[iter].data = stbi_load((char*)"test_assets/gray.png", &images[iter].width, &images[iter].height, &n, 4);
			iter += 1;
			images[iter].data = stbi_load((char*)"test_assets/no_tex.png", &images[iter].width, &images[iter].height, &n, 4);
			iter += 1;
			images[iter].data = font_image.data;
			images[iter].width = font_image.width;
			images[iter].height = font_image.height;
			iter += 1;

			stbi_write_png("test.png", font_image.width, font_image.height, 4, font_image.data, 0);

			const int NUM_TEXT_VERTS = Renderer::MAX_NUM_TEXT_CHARS * 4;
			TextVertex text_verts[NUM_TEXT_VERTS];
			for (int i = 0; i < NUM_TEXT_VERTS; i += 4) {
				text_verts[i + 0] = { 0, 0, 0, 1, 1, 1, 1, 0, 0 };
				text_verts[i + 1] = { 1, 0, 0, 1, 1, 1, 1, 1, 0 };
				text_verts[i + 2] = { 0, 1, 0, 1, 1, 1, 1, 0, 1 };
				text_verts[i + 3] = { 1, 1, 0, 1, 1, 1, 1, 1, 1 };
			}

			Renderer renderer = {};
			renderer.InitD3D11(window, dim.width, dim.height, &vertex_buffer, &index_buffer, images, iter, text_verts, NUM_TEXT_VERTS);

			for (int i = 0; i < iter; i++) {
				stbi_image_free(images[iter].data);
			}

			RenderData render_data = {};
			render_data.entities = (RenderEntity*)renderer_allocator.Allocate(sizeof(RenderEntity) * (i64)(((GameState*)game_memory.data)->num_entities));

			win32_WindowDimension old_dim = dim;
			bool client_area_updated = false;

			while (win32_running) {
				// Timing
				f32 dt = 0.0f;
				{
					LARGE_INTEGER endtimer;
					QueryPerformanceCounter(&endtimer);
					int64_t timerelapsed = endtimer.QuadPart - lasttimer.QuadPart;
					f32 msperframe = (f32)((1000.0f * (f32)timerelapsed) / (f32)perftimerfreq);
					int64_t fps = (int64_t)(perftimerfreq / timerelapsed);

					uint64_t endcyclecount = __rdtsc();
					uint64_t cycleselapsed = endcyclecount - lastcyclecount;

					char str_buffer[256];
					sprintf_s(str_buffer, "ms / frame: %f, fps: %I64d, %I64u\n", msperframe, fps, cycleselapsed);
					//OutputDebugStringA(str_buffer);

					dt = msperframe;

					lasttimer = endtimer;
					lastcyclecount = endcyclecount;
				}

				// Update Input
				{
					*new_input = {};
					for (int i = 0; i < NUM_KEYBOARD_BUTTONS; i++) {
						new_input->keyboard.buttons[i].ended_down = old_input->keyboard.buttons[i].ended_down;
					}
					win32_UpdateInput(new_input, window);
				}

				{
					// Check if the window has been resized since last frame.
					// if it has been resized, signal to the game and renderer that
					// they need to update their client area size dependencies

					dim = win32_GetWindowDimension(window);
					if (dim.width != old_dim.width || dim.height != old_dim.height) {
						client_area_updated = true;
					}
				}

				// Update Game
				{
					if (client_area_updated) {
						UpdateCamera(&game_memory, dim.width, dim.height);
					}

					GameUpdate(&game_memory, new_input, dt, game_debug_text);
				}

				// Render Game
				{
					if (client_area_updated) {
						renderer.RendererResize(window, dim.width, dim.height, old_dim.width, old_dim.height);
					}

					GameState* gs = (GameState*)game_memory.data;
					renderer.UpdateViewport(gs->main_camera.viewport);

					PrepareRenderData(&game_memory, &render_data);
					PrepareText(
						game_debug_text, strlen(game_debug_text), &renderer.NUM_CHARS_TO_RENDER, 10, 10,
						&font, text_verts, dim
					);
					renderer.RenderFrame(&game_memory, &model_buffer, &render_data, text_verts);
					if (FAILED(renderer.RenderPresent(window))) {
						break;
					}
				}

				client_area_updated = false;
				old_dim = dim;

				// Swap Input structs
				{
					Input* temp = new_input;
					new_input = old_input;
					old_input = temp;
				}
			}
		}
	}

	return 0;
}
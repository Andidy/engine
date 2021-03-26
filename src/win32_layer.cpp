#include "universal.h"

#include "game.h"

#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb/stb_image.h"

#include <windows.h>
#include <windowsx.h>
#include <xinput.h>


#ifndef UNICODE
#define UNICODE
#endif 

int window_width = 1200;
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
//  newstate->endedDown = ((xinputButtonState & buttonBit) == buttonBit);
//  newstate->transitionCount = (oldstate->endedDown != newstate->endedDown) ? 1 : 0;
//}

static void win32_ProcessKeyboardMessage(ButtonState* newState, b32 isDown) {
	assert(newState->endedDown != isDown);
	newState->endedDown = isDown;
	newState->transitionCount += 1;
}

static void win32_UpdateInput(Input* gameInput) {
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
	POINT cursorPoint;
	if (GetCursorPos(&cursorPoint)) {
		gameInput->mouse.x = cursorPoint.x;
		gameInput->mouse.y = cursorPoint.y;
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
	RECT clientRect;
	GetClientRect(window, &clientRect);

	win32_WindowDimension windowDimension = {};
	windowDimension.width = clientRect.right - clientRect.left;
	windowDimension.height = clientRect.bottom - clientRect.top;
	return windowDimension;
}

/*
struct win32_OffScreenBuffer {
	// Pixels are alwasy 32-bits wide, Memory Order BB GG RR XX
	BITMAPINFO info;
	void* memory;
	i32 width;
	i32 height;
	i32 pitch;
	i32 bytesPerPixel;
};
win32_OffScreenBuffer globalBackBuffer;

static void win32_ResizeDIBSection(win32_OffScreenBuffer* buffer, i32 width, i32 height) {
	if (buffer->memory) {
		VirtualFree(buffer->memory, 0, MEM_RELEASE);
	}

	buffer->width = width;
	buffer->height = height;

	int bytesPerPixel = 4;
	buffer->bytesPerPixel = bytesPerPixel;

	// NOTE(casey): When the biHeight field is negative, this is the clue to
	// Windows to treat this bitmap as top-down, not bottom-up, meaning that
	// the first three bytes of the image are the color for the top left pixel
	// in the bitmap, not the bottom left!
	buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
	buffer->info.bmiHeader.biWidth = buffer->width;
	buffer->info.bmiHeader.biHeight = -buffer->height;
	buffer->info.bmiHeader.biPlanes = 1;
	buffer->info.bmiHeader.biBitCount = 32;
	buffer->info.bmiHeader.biCompression = BI_RGB;

	// NOTE(casey): Thank you to Chris Hecker of Spy Party fame
	// for clarifying the deal with StretchDIBits and BitBlt!
	// No more DC for us.
	int bitmapMemorySize = (buffer->width * buffer->height) * bytesPerPixel;
	buffer->memory = VirtualAlloc(0, bitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	buffer->pitch = width * bytesPerPixel;
}

static void Win32DisplayBufferInWindow(win32_OffScreenBuffer* buffer,
	HDC hdc, int window_wid, int window_hgt) {
	// NOTE(casey): For prototyping purposes, we're going to always blit
	// 1-to-1 pixels to make sure we don't introduce artifacts with
	// stretching while we are learning to code the renderer!
	StretchDIBits(hdc,
		0, 0, buffer->width, buffer->height,
		0, 0, buffer->width, buffer->height,
		buffer->memory,
		&buffer->info,
		DIB_RGB_COLORS, SRCCOPY);
}

*/

void PrepareRenderData(Memory* gameMemory, RenderData* renderData) {
	GameState* gs = (GameState*)gameMemory->data;

	int iter = 0;
	renderData->entities[iter++] = { gs->blackGuyHead.renderPos, gs->blackGuyHead.renderScale, gs->blackGuyHead.renderRotAxis, gs->blackGuyHead.renderRotAngle, 0, 0 };
	renderData->entities[iter++] = { gs->bunnyTest.renderPos, gs->bunnyTest.renderScale, gs->bunnyTest.renderRotAxis, gs->bunnyTest.renderRotAngle, 2, 2 };
	renderData->entities[iter++] = { gs->treeTest.renderPos, gs->treeTest.renderScale, gs->treeTest.renderRotAxis, gs->treeTest.renderRotAngle, 3, 1 };
	renderData->entities[iter++] = { gs->deerTest.renderPos, gs->deerTest.renderScale, gs->deerTest.renderRotAxis, gs->deerTest.renderRotAngle, 1, 0 };
	for (int i = 0; i < 7; i++) {
		renderData->entities[iter++] = { gs->cubes[i].renderPos, gs->cubes[i].renderScale, gs->cubes[i].renderRotAxis, gs->cubes[i].renderRotAngle, 4, 3 + i };
	}
	renderData->entities[iter++] = { gs->quad.renderPos, gs->quad.renderScale, gs->quad.renderRotAxis, gs->quad.renderRotAngle, 5, 0 };

	/*
	renderData->entities[iter++] = { gs->gameMap.ent.renderPos, gs->gameMap.ent.renderScale, gs->gameMap.ent.renderRotAxis, gs->gameMap.ent.renderRotAngle, 6, 1 };
	renderData->entities[iter++] = { gs->waterQuad.renderPos, gs->waterQuad.renderScale, gs->waterQuad.renderRotAxis, gs->waterQuad.renderRotAngle, 5, 8 };
	int width = gs->gameMap.mapWidth;
	int height = gs->gameMap.mapHeight;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Tile t = gs->gameMap.tiles[x + y * width];
			if (t.feature == TileFeatures::FOREST) {
				renderData->entities[iter++] = { { (f32)x + 0.5f, (f32)t.elevation, (f32)y + 0.5f }, OneVec(), UpVec(), 0.0f, 3, 1 };
			}
			if (t.resources[0] == TileResource::GRAINS) {
				renderData->entities[iter++] = { { (f32)x + 0.5f, (f32)t.elevation, (f32)y + 0.5f }, Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f, 4, 5 };
			}
			if (t.resources[0] == TileResource::FRUITS) {
				renderData->entities[iter++] = { { (f32)x + 0.5f, (f32)t.elevation, (f32)y + 0.5f }, Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f, 4, 3 };
			}
			if (t.resources[0] == TileResource::STONE) {
				renderData->entities[iter++] = { { (f32)x + 0.5f, (f32)t.elevation, (f32)y + 0.5f }, Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f, 4, 9 };
			}
			if (t.resources[0] == TileResource::VEGETABLES) {
				renderData->entities[iter++] = { { (f32)x + 0.5f, (f32)t.elevation, (f32)y + 0.5f }, Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f, 4, 6 };
			}
			if (t.resources[0] == TileResource::SMALL_GAME) {
				renderData->entities[iter++] = { { (f32)x + 0.5f, (f32)t.elevation, (f32)y + 0.5f }, Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f, 2, 2 };
			}
			if (t.resources[0] == TileResource::LARGE_GAME) {
				renderData->entities[iter++] = { { (f32)x + 0.5f, (f32)t.elevation, (f32)y + 0.5f }, Vec3(0.5f, 0.5f, 0.5f), UpVec(), 0.0f, 1, 0 };
			}
		}
	}
	*/
	renderData->entities[iter++] = { gs->terrainMap.ent.renderPos, gs->terrainMap.ent.renderScale, gs->terrainMap.ent.renderRotAxis, gs->terrainMap.ent.renderRotAngle, 7, 1 };

	renderData->num_entities = iter;
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

int WINAPI wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hprevinstance, _In_ LPWSTR pCmdLine, _In_ int nCmdShow) {
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
			win32_WindowDimension dim = win32_GetWindowDimension(window);
			// win32_ResizeDIBSection(&globalBackBuffer, dim.width, dim.height);
			
			win32_running = 1;

			LARGE_INTEGER lasttimer;
			QueryPerformanceCounter(&lasttimer);
			u64 lastcyclecount = __rdtsc();

			Input gameInput[2] = {};
			Input* newInput = &gameInput[0];
			Input* oldInput = &gameInput[1];

			Memory gameMemory = {};
			gameMemory.isInitialized = 0;
			gameMemory.size = Kilobytes((u64)1);
			gameMemory.data = VirtualAlloc(0, (SIZE_T)gameMemory.size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

			InitGameState(&gameMemory, Vec2((f32)dim.width, (f32)dim.height));

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

			GenerateTerrainModel(&((GameState*)gameMemory.data)->gameMap, &vertex_buffer, &index_buffer, &model_buffer);
			GenerateTerrainMapModel(&((GameState*)gameMemory.data)->terrainMap, &vertex_buffer, &index_buffer, &model_buffer);
			
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

			Renderer renderer = {};
			renderer.InitD3D11(window, dim.width, dim.height, &vertex_buffer, &index_buffer, images, iter);

			RenderData renderData = {};
			const int dynamicEntitiesMax = 1000000;
			renderData.entities = (RenderEntity*)renderer_allocator.Allocate(sizeof(RenderEntity) * (i64)(((GameState*)gameMemory.data)->numEntities + dynamicEntitiesMax));

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
					*newInput = {};
					for (int i = 0; i < NUM_KEYBOARD_BUTTONS; i++) {
						newInput->keyboard.buttons[i].endedDown = oldInput->keyboard.buttons[i].endedDown;
					}
					win32_UpdateInput(newInput);
				}

				// Update Game
				{
					GameUpdate(&gameMemory, newInput, dt);
				}

				// Render Game
				{
					PrepareRenderData(&gameMemory, &renderData);
					renderer.RenderFrame(&gameMemory, &model_buffer, &renderData);
					if (FAILED(renderer.RenderPresent(window))) {
						break;
					}
				}

				// Swap Input structs
				{
					Input* temp = newInput;
					newInput = oldInput;
					oldInput = temp;
				}
			}
		}
	}

	return 0;
}
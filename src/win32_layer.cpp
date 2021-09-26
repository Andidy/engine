#include "universal.h"

#include "game.h"

#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../libs/stb/stb_image_write.h"

#include "../libs/json11-master/json11.hpp"

#include <windows.h>
#include <windowsx.h>

#ifndef UNICODE
#define UNICODE
#endif 

int window_width = 1200;
int window_height = 900;

int win32_running = 0;

void DebugPrint(char* str) {
	OutputDebugStringA(str);
}

// ============================================================================
// USER INPUT

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

void PremultiplyAlpha(Image* image) {
	for (int row = 0; row < image->height; row++) {
		for (int col = 0; col < image->width; col++) {
			uchar alpha = image->data[4 * (col + row * image->width) + 3];
			uchar red = image->data[4 * (col + row * image->width) + 0];
			uchar green = image->data[4 * (col + row * image->width) + 1];
			uchar blue = image->data[4 * (col + row * image->width) + 2];

			float color = ((float)red / 255.0f) * ((float)alpha / 255.0f);
			image->data[4 * (col + row * image->width) + 0] = (uchar)(color * 255.0f);
			color = ((float)green / 255.0f) * ((float)alpha / 255.0f);
			image->data[4 * (col + row * image->width) + 1] = (uchar)(color * 255.0f);
			color = ((float)blue / 255.0f) * ((float)alpha / 255.0f);
			image->data[4 * (col + row * image->width) + 2] = (uchar)(color * 255.0f);
		}
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
// Rendering

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

void PrepareRenderData(GameState* gs, RenderData* render_data) {
	Entity* crosshair = &gs->entities[gs->crosshair_entity];
	cRenderable* ch = &gs->c_renderables[crosshair->renderable];

	int num_render_entities = 0;
	for (int iter = 0; iter < gs->entities.size(); iter++) {
		Entity* e = &gs->entities[iter];
		cTransform* t = &gs->c_transforms[e->transform];
		cRenderable* r = &gs->c_renderables[e->renderable];

		if (r->should_render) {
			vec3 pos = { t->game_pos.x, 0.0f, t->game_pos.y };
			pos += r->offset;
			render_data->entities[num_render_entities++] = { pos, r->scale, r->rot_axis, r->rot_angle, r->h_model.handle };

			if (e->unit >= 0) {
				cUnit* u = &gs->c_units[e->unit];
				if (u->waypoint_active) {
					pos = { u->waypoint_pos.x, 0.0f, u->waypoint_pos.y };
					pos += ch->offset;
					render_data->entities[num_render_entities++] = { pos, ch->scale, ch->rot_axis, ch->rot_angle, ch->h_model.handle };
				}
			}
		}
	}
	render_data->num_entities = num_render_entities;
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

		i32 kern = stbtt_GetCodepointKernAdvance(&(font->info), str[i], str[i + 1]);
		horz_pos += (f32)kern * scale;
	}

	*num_chars_visible = vi / 4;
}

// end Rendering
// ============================================================================
// Game

void LoadGameAssets(GameState* gs, AssetHandle* asset_handles) {
	debug_ReadFileResult file = debug_ReadFile((char*)"test_assets/entities.json");
	if (file.data != NULL && file.size >= 0) {
		std::string json_err_str;
		json11::Json json = json11::Json::parse((char*)file.data, json_err_str);

		int entity_index = 0;
		while (json[entity_index].is_object()) {
			json11::Json::object je = json[entity_index].object_items();

			std::string name = je["name"].string_value();

			auto j_pos = je["pos"].array_items();
			vec2 pos = Vec2(j_pos[0].number_value(), j_pos[1].number_value());

			auto j_ro = je["render_offset"].array_items();
			vec3 render_offset = Vec3(j_ro[0].number_value(), j_ro[1].number_value(), j_ro[2].number_value());

			auto j_scale = je["scale"].array_items();
			vec3 render_scale = Vec3(j_scale[0].number_value(), j_scale[1].number_value(), j_scale[2].number_value());

			auto j_ra = je["rotation_axis"].array_items();
			vec3 render_rot_axis = Vec3(j_ra[0].number_value(), j_ra[1].number_value(), j_ra[2].number_value());

			auto j_rang = je["rotation_angle"].number_value();
			float render_rot_angle = (float)j_rang;

			AssetHandle h_model = {};
			std::string asset_name = je["model"].string_value();
			for (int i = 0; i < 256; i++) {
				if (asset_name.compare(asset_handles[i].name) == 0) {
					h_model = asset_handles[i];
					break;
				}
			}

			bool should_render = true;
			bool is_unit = je["unit"].bool_value();
			bool waypoint_active = false;
			vec2 waypoint_pos = { 0.0f, 0.0f };
			bool is_pickup = je["pickup"].bool_value();
			bool is_food = false;
			bool is_active = true;

			if (name.compare((char*)"crosshair_0") == 0) {
				should_render = false;
				gs->crosshair_entity = entity_index;
			}

			// ================================================================
			// Create components and store them in the associated vectors
			// then Create and initialize an entity with the components
			// and store the entity in the entities array

			int transforms_index = gs->c_transforms.size();
			gs->c_transforms.push_back(cTransform(pos));

			int renderables_index = gs->c_renderables.size();
			gs->c_renderables.push_back(cRenderable(should_render, render_offset, render_scale, render_rot_axis, render_rot_angle, h_model));
			
			Entity ent = {};

			if (is_unit) {
				int units_index = gs->c_units.size();
				gs->c_units.push_back(cUnit(waypoint_active, waypoint_pos));
				
				ent.InitUnit(entity_index, is_active, transforms_index, renderables_index, units_index);
			}
			else if (is_pickup) {
				int items_index = gs->c_items.size();
				gs->c_items.push_back(cItem(1, 0));

				ent.InitItem(entity_index, is_active, transforms_index, renderables_index, items_index);
			}
			else if (is_food) {
				int items_index = gs->c_items.size();
				gs->c_items.push_back(cItem(1, 0));

				int foods_index = gs->c_foods.size();
				gs->c_foods.push_back(cFood(1));

				ent.InitFood(entity_index, is_active, transforms_index, renderables_index, items_index, foods_index);
			}
			else {
				ent.InitGeneric(entity_index, is_active, transforms_index, renderables_index);
			}

			gs->entities.push_back(ent);

			entity_index++;
		}
	}
}

// end Game
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
	srand(0);
	
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
			
			win32_running = 1;

			LARGE_INTEGER old_time;
			QueryPerformanceCounter(&old_time);
			u64 old_cycle_count = __rdtsc();

			Input game_input[2] = {};
			Input* new_input = &game_input[0];
			Input* old_input = &game_input[1];

			GameState game_state = {};
			GameState* gs = &game_state;

			VertexBuffer vertex_buffer;
			IndexBuffer index_buffer;
			MeshBuffer model_buffer;
			PermanentResourceAllocator renderer_allocator(Gigabytes((u64)4));
			FrameAllocator frame_allocator(Megabytes((u64)64));

			InitRenderer(&vertex_buffer, &index_buffer, &model_buffer, &renderer_allocator);
			
			// experimental Asset Loading

			int n, iter = 0;
			Image* images = (Image*)renderer_allocator.Allocate(sizeof(Image) * 128);

			const int MAX_ASSET_HANDLES = 256;
			AssetHandle asset_handles[MAX_ASSET_HANDLES];
			int asset_index = 0;

			std::string asset_folder_path = "test_assets/";
			std::string asset_path = "";
			const int MAX_MODELS = 16;
			Model models[MAX_MODELS];
			debug_ReadFileResult file = debug_ReadFile((char*)"test_assets/models.json");
			if (file.data != NULL && file.size >= 0) {
				std::string json_err_str;
				json11::Json json = json11::Json::parse((char*)file.data, json_err_str);

				int model_index = 0;
				while (json[model_index].is_object()) {
					json11::Json::object jm = json[model_index].object_items();

					Model m = {};

					std::string model_name = jm["name"].string_value();
					m.name.assign(model_name);

					std::string mesh_name = jm["mesh"].string_value();
					bool found_mesh = false;
					for (int i = 0; i < asset_index; i++) {
						if (mesh_name.compare(asset_handles[i].name) == 0) {
							m.h_mesh = asset_handles[i];
							found_mesh = true;
							break;
						}
					}
					if (!found_mesh) {
						// generate asset handle
						asset_handles[asset_index] = { mesh_name, AssetType::MESH, model_buffer.num_meshes };
						// load the mesh
						asset_path = asset_folder_path;
						LoadOBJ((char*)asset_path.append(mesh_name).c_str(), &vertex_buffer, &index_buffer, &model_buffer, &frame_allocator);
						asset_path.clear();
						// assign asset handle to model
						m.h_mesh = asset_handles[asset_index++];
					}

					std::string texture_name = jm["texture"].string_value();
					bool found_texture = false;
					for (int i = 0; i < asset_index; i++) {
						if (texture_name.compare(asset_handles[i].name) == 0) {
							m.h_texture = asset_handles[i];
							found_texture = true;
							break;
						}
					}
					if (!found_texture) {
						// load the texture
						asset_path = asset_folder_path;
						images[iter].data = stbi_load((char*)asset_path.append(texture_name).c_str(), &images[iter].width, &images[iter].height, &n, 4);
						PremultiplyAlpha(&images[iter]);
						asset_path.clear();
						// generate asset handle
						asset_handles[asset_index] = { texture_name.c_str(), AssetType::TEXTURE, iter };
						iter += 1;
						// assign asset handle to model
						m.h_texture = asset_handles[asset_index++];
					}

					bool opaque = jm["opaque"].bool_value();
					m.opaque = opaque;

					bool billboard = jm["billboard"].bool_value();
					m.billboard = billboard;

					asset_handles[asset_index++] = { model_name, AssetType::MODEL, model_index };
					models[model_index++] = m;
				}
			}

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
				font.font_buf = (uchar*)calloc(font_buf_size, 1u);

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
			
			// put the font atlas into the images
			images[iter].data = font_image.data;
			images[iter].width = font_image.width;
			images[iter].height = font_image.height;
			int text_image_index = iter;
			iter += 1;

			const int NUM_TEXT_VERTS = Renderer::MAX_NUM_TEXT_CHARS * 4;
			TextVertex text_verts[NUM_TEXT_VERTS];
			for (int i = 0; i < NUM_TEXT_VERTS; i += 4) {
				text_verts[i + 0] = { 0, 0, 0, 1, 1, 1, 1, 0, 0 };
				text_verts[i + 1] = { 1, 0, 0, 1, 1, 1, 1, 1, 0 };
				text_verts[i + 2] = { 0, 1, 0, 1, 1, 1, 1, 0, 1 };
				text_verts[i + 3] = { 1, 1, 0, 1, 1, 1, 1, 1, 1 };
			}

			Renderer renderer = {};
			renderer.InitD3D11(window, dim.width, dim.height, &vertex_buffer, &index_buffer, images, iter, text_verts, NUM_TEXT_VERTS, text_image_index);

			for (int i = 0; i < iter; i++) {
				stbi_image_free(images[iter].data);
			}

			InitGameState(&game_state, Vec2((f32)dim.width, (f32)dim.height), asset_handles);

			// end experimental asset loading

			RenderData render_data = {};
			render_data.entities = (RenderEntity*)renderer_allocator.Allocate(sizeof(RenderEntity) * (i64)game_state.entities.size());

			win32_WindowDimension old_dim = dim;
			bool client_area_updated = false;

			while (win32_running) {
				// Timing
				f32 dt = 0.0f;
				{
					LARGE_INTEGER new_time;
					QueryPerformanceCounter(&new_time);
					int64_t timer_elapsed = new_time.QuadPart - old_time.QuadPart;
					dt = (f32)timer_elapsed / (f32)perftimerfreq;
					f32 ms_per_frame = 1000.0f * dt;
					int64_t fps = (int64_t)(perftimerfreq / timer_elapsed);

					uint64_t new_cycle_count = __rdtsc();
					uint64_t cycles_elapsed = new_cycle_count - old_cycle_count;

					char str_buffer[256];
					sprintf_s(str_buffer, "ms / frame: %f, fps: %I64d, %I64u\n", ms_per_frame, fps, cycles_elapsed);
					//OutputDebugStringA(str_buffer);

					old_time = new_time;
					old_cycle_count = new_cycle_count;
				}

				// Update Input
				{
					*new_input = {};
					for (int i = 0; i < NUM_KEYBOARD_BUTTONS; i++) {
						new_input->keyboard.buttons[i].ended_down = old_input->keyboard.buttons[i].ended_down;
					}
					for (int i = 0; i < NUM_MOUSE_BUTTONS; i++) {
						new_input->mouse.buttons[i].ended_down = old_input->mouse.buttons[i].ended_down;
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
						Camera* main_camera = &gs->main_camera;
						SetCameraViewportAndProjMat(main_camera, dim.width, dim.height);
					}

					GameUpdate(&game_state, new_input, dt, game_debug_text);
				}

				// Render Game
				{
					if (client_area_updated) {
						renderer.RendererResize(window, dim.width, dim.height, old_dim.width, old_dim.height);
					}

					renderer.UpdateViewport(gs->main_camera.viewport);

					PrepareRenderData(gs, &render_data);
					PrepareText(
						game_debug_text, (int)strlen(game_debug_text), &renderer.NUM_CHARS_TO_RENDER, 10, 10,
						&font, text_verts, dim
					);
					renderer.RenderFrame(gs, &model_buffer, models, &render_data, text_verts);
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
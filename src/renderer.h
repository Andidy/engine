#ifndef RENDERER_H
#define RENDERER_H

#include "universal.h"

// fix this?
#include "game.h"

#include "../libs/stb/stb_rect_pack.h"
#include "../libs/stb/stb_truetype.h"

struct Vertex {
	f32 x, y, z;
	f32 nx, ny, nz;
	f32 tx, ty;
};

Vertex CreateVertex(vec3 pos, vec3 norm, vec2 tex);

struct TextVertex {
	f32 x, y, z;
	f32 r, g, b, a;
	f32 tx, ty;
};

struct VertexBuffer {
	i32 buffer_length;
	i32 num_vertices;
	Vertex* vertices;
};

struct IndexBuffer {
	i32 buffer_length;
	i32 num_indices;
	i32* indices;
};

struct Mesh {
	i32 start_index;
	i32 length;
};

struct MeshBuffer {
	i32 buffer_length;
	i32 num_meshes;
	Mesh* meshes;
};

struct RenderEntity {
	vec3 pos;
	vec3 scale;
	vec3 rot_axis;
	f32 rot_angle;

	int32_t model_index;
};

struct RenderData {
	int32_t num_entities;
	RenderEntity* entities;
};

struct Model {
	std::string name;
	AssetHandle h_mesh;
	AssetHandle h_texture;
	// AssetHandle h_normal_map;
	bool opaque;
};

struct Font {
	stbtt_fontinfo info;
	uchar* font_buf;
	i32 size;

	stbtt_packedchar chardata_for_range[256];
};



struct Image {
	i32 width;
	i32 height;
	uchar* data;
};

void InitRenderer(VertexBuffer* v_buf, IndexBuffer* i_buf, MeshBuffer* meshes, PermanentResourceAllocator* allocator);

void LoadOBJ(char* filename, VertexBuffer* v_buffer, IndexBuffer* i_buffer, MeshBuffer* m_buffer, PermanentResourceAllocator* allocator);

#include <d3d11_1.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

struct Constants {
	mat4 m;
	mat4 mvp;
	vec3 camera_pos;
};

struct Renderer {
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11DeviceContext1* context1;

	ID3D11RasterizerState* rasterizer_state;
	ID3D11DepthStencilState* depth_stencil_state;
	ID3D11DepthStencilState* no_depth_write_stencil_state;
	ID3D11BlendState* blend_state;
	ID3D11BlendState* transparency_blend_state;

	IDXGISwapChain3* swapchain;

	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11InputLayout* input_layout;

	ID3D11Buffer* vertex_buffer;
	ID3D11Buffer* index_buffer;
	ID3D11Buffer* constant_buffer;
	
	int NUM_CHARS_TO_RENDER = 0;
	static const int MAX_NUM_TEXT_CHARS = 1024;
	ID3D11VertexShader* text_vs;
	ID3D11PixelShader* text_ps;
	ID3D11InputLayout* text_il;

	ID3D11Buffer* text_vertex_buffer;
	ID3D11Buffer* text_index_buffer;

	ID3D11RenderTargetView* window_rt_view;
	ID3D11DepthStencilView* window_ds_view;

	ID3D11SamplerState* sampler_state;

	ID3D11Texture2D* textures[16];
	ID3D11ShaderResourceView* texture_views[16];

	b32 renderer_occluded = 0;

	void InitD3D11(HWND window, i32 swapchain_width, i32 swapchain_height, VertexBuffer* v_buf, IndexBuffer* i_buf, Image* images, int num_images, TextVertex* text_vertex_buffer, int num_text_verts);
	void RendererResize(HWND window, i32 swapchain_width, i32 swapchain_height, i32 old_width, i32 old_height);
	HRESULT RenderPresent(HWND window);
	void RenderFrame(Memory* game_memory, MeshBuffer* m_buffer, Model* models, RenderData* render_data, TextVertex* verts);
	void UpdateViewport(Viewport viewport);
};

#endif

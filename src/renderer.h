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

struct Model {
	i32 start_index;
	i32 length;
};

struct ModelBuffer {
	i32 buffer_length;
	i32 num_models;
	Model* models;
};

struct RenderEntity {
	vec3 pos;
	vec3 scale;
	vec3 rot_axis;
	f32 rot_angle;

	int32_t model_index;
	int32_t texture_index;
};

struct RenderData {
	int32_t num_entities;
	RenderEntity* entities;
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

void InitRenderer(VertexBuffer* v_buf, IndexBuffer* i_buf, ModelBuffer* models, PermanentResourceAllocator* allocator);

void LoadOBJ(char* filename, VertexBuffer* v_buffer, IndexBuffer* i_buffer, ModelBuffer* m_buffer, PermanentResourceAllocator* allocator);

void GenerateTerrainModel(GameMap* gameMap, VertexBuffer* v_buf, IndexBuffer* i_buf, ModelBuffer* models);
void GenerateTerrainMapModel(TerrainMap* terrMap, VertexBuffer* v_buf, IndexBuffer* i_buf, ModelBuffer* models);

#include <d3d11_1.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

struct Constants {
	mat4 mvp;
};

struct Renderer {
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11DeviceContext1* context1;

	ID3D11RasterizerState* rasterizerState;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11BlendState* blendState;
	ID3D11BlendState* transparencyBlendState;

	IDXGISwapChain3* swapchain;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constantBuffer;
	
	int NUM_CHARS_TO_RENDER = 0;
	static const int MAX_NUM_TEXT_CHARS = 1024;
	ID3D11VertexShader* textVS;
	ID3D11PixelShader* textPS;
	ID3D11InputLayout* textIL;

	ID3D11Buffer* textVertexBuffer;
	ID3D11Buffer* textIndexBuffer;

	ID3D11RenderTargetView* windowRTView;
	ID3D11DepthStencilView* windowDPView;

	ID3D11SamplerState* samplerState;

	ID3D11Texture2D* textures[16];
	ID3D11ShaderResourceView* textureViews[16];

	b32 renderer_occluded = 0;

	void InitD3D11(HWND window, i32 swapchainWidth, i32 swapchainHeight, VertexBuffer* v_buf, IndexBuffer* i_buf, Image* images, int numImages, TextVertex* textVertBuffer, int numTextVerts);
	HRESULT RendererResize(HWND window, i32 swapchainWidth, i32 swapchainHeight);
	HRESULT RenderPresent(HWND window);
	void RenderFrame(Memory* gameMemory, ModelBuffer* m_buffer, RenderData* renderData, TextVertex* verts);
};

#endif

#ifndef RENDERER_H
#define RENDERER_H

#include "universal.h"

// fix this?
#include "game.h"

struct Vertex {
	f32 x, y, z;
	f32 nx, ny, nz;
	f32 tx, ty;
};

Vertex CreateVertex(vec3 pos, vec3 norm, vec2 tex);

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
	i32 num_models;
	i32 buffer_length;
	Model* models;
};









struct Image {
	i32 width;
	i32 height;
	uchar* data;
};

void InitRenderer(VertexBuffer* v_buf, IndexBuffer* i_buf, ModelBuffer* models, PermanentResourceAllocator* allocator);

void LoadOBJ(char* filename, VertexBuffer* v_buffer, IndexBuffer* i_buffer, ModelBuffer* m_buffer, PermanentResourceAllocator* allocator);

void GenerateTerrainModel(GameMap* gameMap, VertexBuffer* v_buf, IndexBuffer* i_buf, ModelBuffer* models);

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

	IDXGISwapChain3* swapchain;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;

	ID3D11RenderTargetView* windowRTView;
	ID3D11DepthStencilView* windowDPView;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constantBuffer;

	ID3D11SamplerState* samplerState;

	ID3D11Texture2D* blackguyface_texture;
	ID3D11ShaderResourceView* blackguyface_textureView;

	ID3D11Texture2D* grass_texture;
	ID3D11ShaderResourceView* grass_textureView;

	b32 renderer_occluded = 0;

	void InitD3D11(HWND window, i32 swapchainWidth, i32 swapchainHeight, VertexBuffer* v_buf, IndexBuffer* i_buf, Image* my_image, Image* grass_image);
	HRESULT RendererResize(HWND window, i32 swapchainWidth, i32 swapchainHeight);
	HRESULT RenderPresent(HWND window);
	void RenderFrame(Memory* gameMemory, ModelBuffer* m_buffer);
};

#endif

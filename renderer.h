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









struct Color {
	u8 r, g, b, a;
};

struct Image {
	i32 width;
	i32 height;
	uchar* data;
};

void InitRenderer(VertexBuffer* v_buf, IndexBuffer* i_buf, ModelBuffer* models, PermanentResourceAllocator* allocator);

void OutputDebugFromRenderer(char* string);

void LoadOBJ(char* filename, VertexBuffer* v_buffer, IndexBuffer* i_buffer, ModelBuffer* m_buffer, PermanentResourceAllocator* allocator);

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

	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* textureView;

	b32 renderer_occluded = 0;

	void InitD3D11(HWND window, i32 swapchainWidth, i32 swapchainHeight, VertexBuffer* v_buf, IndexBuffer* i_buf, Image* my_image);
	HRESULT RendererResize(HWND window, i32 swapchainWidth, i32 swapchainHeight);
	HRESULT RenderPresent(HWND window);
	void RenderFrame(Memory* gameMemory, ModelBuffer* m_buffer);
};

/* // D3D12 Renderer
struct Renderer {
	ID3D12Debug* debugController;

	static const u32 frameCount = 3;
	u32 frameIndex;
	HANDLE fenceEvent;
	ID3D12Fence* fence[frameCount];
	u64 fenceValue[frameCount];

	ID3D12Device* device;

	ID3D12CommandAllocator* commandAllocator[frameCount];
	ID3D12CommandQueue* commandQueue;
	ID3D12CommandList* commandList;
	IDXGISwapChain3* swapchain;

	ID3D12DescriptorHeap* rtvHeap;
	u32 rtvHeapDescSize;
	ID3D12Resource* renderTargets[frameCount];

	// should be made into arrays in future
	ID3D12RootSignature* rootSignature;
	ID3D12PipelineState* pipelineStateObject;

	// need to look at "carnegie-9eca4b7839b4a09badcf5dced58e366b07f9f172" to implement D3D12 init code
	void InitD3D12(WindowHandle window, i32 swapchainWidth, i32 swapchainHeight);
	void CreateRootSig();
	void CreatePSO();
	void UploadModelData();
};
*/
#endif

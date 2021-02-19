#include "renderer.h"

void InitRenderer(VertexBuffer* v_buf, IndexBuffer* i_buf, ModelBuffer* models, PermanentResourceAllocator* allocator) {
	v_buf->buffer_length = 1000000;
	v_buf->num_vertices = 0;
	v_buf->vertices = (Vertex*)allocator->Allocate(sizeof(Vertex) * v_buf->buffer_length);

	i_buf->buffer_length = 1000000;
	i_buf->num_indices = 0;
	i_buf->indices = (i32*)allocator->Allocate(sizeof(i32) * i_buf->buffer_length);

	models->buffer_length = 100;
	models->num_models = 0;
	models->models = (Model*)allocator->Allocate(sizeof(Model) * models->buffer_length);
}



b32 VertexCompare(Vertex v1, Vertex v2) {
	b32 a = v1.x == v2.x;
	b32 b = v1.y == v2.y;
	b32 c = v1.z == v2.z;

	b32 d = v1.nx == v2.nx;
	b32 e = v1.ny == v2.ny;
	b32 f = v1.nz == v2.nz;
	
	b32 g = v1.tx == v2.tx;
	b32 h = v1.ty == v2.ty;
	return a && b && c && d && e && f && g && h;
}

struct FaceInt3 {
	int v, n, t;
};

struct Face {
	FaceInt3 f[3];
};

void LoadOBJ(char* filename, VertexBuffer* v_buffer, IndexBuffer* i_buffer, ModelBuffer* m_buffer, PermanentResourceAllocator* allocator) {
	// load obj
	debug_ReadFileResult obj_file = debug_ReadFile(filename);
	uchar* ptr = (uchar*)obj_file.data;

	// find out how many faces and indices we will need for temp storage and model data
	i32 num_indices = 0;
	i32 num_faces = 0;
	u64 i = 0;
	while (i < obj_file.size) {
		if (ptr[i] == 'f') {
			num_faces += 1;
		}
		while (ptr[i] != '\n') { i += 1; }
		i += 1;
	}
	num_indices = 3 * num_faces;

	// set this model's information to model buffer
	m_buffer->models[m_buffer->num_models].start_index = i_buffer->num_indices;
	m_buffer->models[m_buffer->num_models].length = num_indices;
	m_buffer->num_models += 1;


	// allocate temporary space
	vec3* vertices = (vec3*)allocator->Allocate(sizeof(vec3) * num_indices);
	vec3* normals = (vec3*)allocator->Allocate(sizeof(vec3) * num_indices);
	vec2* texcoords = (vec2*)allocator->Allocate(sizeof(vec2) * num_indices);

	Face* faces = (Face*)allocator->Allocate(sizeof(Face) * num_faces);

	// extract obj data and place it into temporary storage
	i32 vi = 0;
	i32 ni = 0;
	i32 ti = 0;
	i32 fi = 0;

	i = 0;
	while (i < obj_file.size) {
		if (ptr[i] == '#' || ptr[i] == ' ' || ptr[i] == '\n') { // skip lines which have the comment operator #, and skip blank lines
			while (ptr[i] != '\n') { i += 1; }
			i += 1;
		}
		else if (ptr[i] == 'm') { // we ignore materials for now
			while (ptr[i] != '\n') { i += 1; }
			i += 1;
		}
		else if (ptr[i] == 's' || ptr[i] == 'o' || ptr[i] == 'g' || ptr[i] == 'u') { // ignore soft shading, objects, groups, and usematerial for now
			while (ptr[i] != '\n') { i += 1; }
			i += 1;
		}
		else if (ptr[i] == 'v') { // we found vertex information
			i += 1;

			if(ptr[i] == ' ') { // we found a vertex
				while (ptr[i] == ' ') { i += 1; }
				vertices[vi].x = (float)atof((char*)&ptr[i]);
				while (ptr[i] != ' ') {
					i += 1;
				}
				i += 1;
				vertices[vi].y = (float)atof((char*)&ptr[i]);
				while (ptr[i] != ' ') {
					i += 1;
				}
				i += 1;
				vertices[vi].z = (float)atof((char*)&ptr[i]);
				while (ptr[i] != '\n') {
					i += 1;
				}
				i += 1;

				vi += 1;
			}
			else if (ptr[i] == 't') { // we found a texcoord
				i += 1;
				while (ptr[i] == ' ') { i += 1; }
				texcoords[ti].x = (float)atof((char*)&ptr[i]);
				while (ptr[i] != ' ') {
					i += 1;
				}
				i += 1;
				texcoords[ti].y = (float)atof((char*)&ptr[i]);
				while (ptr[i] != '\n') {
					i += 1;
				}
				i += 1;

				ti += 1;
			}
			else if (ptr[i] == 'n') { // we found a normal
				i += 1;
				while (ptr[i] == ' ') { i += 1; }
				normals[ni].x = (float)atof((char*)&ptr[i]);
				while (ptr[i] != ' ') {
					i += 1;
				}
				i += 1;
				normals[ni].y = (float)atof((char*)&ptr[i]);
				while (ptr[i] != ' ') {
					i += 1;
				}
				i += 1;
				normals[ni].z = (float)atof((char*)&ptr[i]);
				while (ptr[i] != '\n') {
					i += 1;
				}
				i += 1;

				ni += 1;
			}
			else if (ptr[i] == 'p') { // ignore parametric curve stuff
				while (ptr[i] != '\n') { i += 1; }
				i += 1;
			}
		}
		else if (ptr[i] == 'f') { // faces
			i += 2;

			faces[fi].f[0].v = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != '/') { i += 1; }
			i += 1;
			faces[fi].f[0].t = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != '/') { i += 1; }
			i += 1;
			faces[fi].f[0].n = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != ' ') { i += 1; }
			i += 1;

			faces[fi].f[1].v = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != '/') { i += 1; }
			i += 1;
			faces[fi].f[1].t = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != '/') { i += 1; }
			i += 1;
			faces[fi].f[1].n = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != ' ') { i += 1; }
			i += 1;

			faces[fi].f[2].v = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != '/') { i += 1; }
			i += 1;
			faces[fi].f[2].t = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != '/') { i += 1; }
			i += 1;
			faces[fi].f[2].n = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != '\n') { i += 1; }
			i += 1;
			fi += 1;
		}
	}


	i32 start_v_buf = v_buffer->num_vertices;


	// compose temporary storage into vertices and build vertex and index buffers
	for (fi = 0; fi < num_faces; fi++) { // compose vertices 
		for (i32 fii = 0; fii < 3; fii++) {
			vec3 pos = vertices[faces[fi].f[fii].v];
			vec3 norm = normals[faces[fi].f[fii].n];
			vec2 tex = texcoords[faces[fi].f[fii].t];

			Vertex v = { pos.x, pos.y, pos.z, norm.x, norm.y, norm.z, tex.x, tex.y };

			b32 found_vertex = 0;
			for (int i = start_v_buf; i < v_buffer->num_vertices; i++) {
				if (VertexCompare(v, v_buffer->vertices[i])) {
					i_buffer->indices[i_buffer->num_indices] = i;
					i_buffer->num_indices += 1;
					found_vertex = 1;
					break;
				}
			}

			if (!found_vertex) {
				v_buffer->vertices[v_buffer->num_vertices] = v;
				i_buffer->indices[i_buffer->num_indices] = v_buffer->num_vertices;
				v_buffer->num_vertices += 1;
				i_buffer->num_indices += 1;
			}
		}
	}

	allocator->Free();
}



// ============================================================================
// D3D11

void Renderer::InitD3D11(HWND window, i32 swapchainWidth, i32 swapchainHeight, VertexBuffer* v_buf, IndexBuffer* i_buf, Image* my_image) {
	HRESULT hr;

	// device
	{
		UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
		flags |= D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL level;

		hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, NULL, 0, D3D11_SDK_VERSION,
			&device, &level, &context);
		if (FAILED(hr)) {
			hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, NULL, flags, NULL, 0, D3D11_SDK_VERSION,
				&device, &level, &context);
			if (FAILED(hr)) {
				__debugbreak();
			}
		}

		hr = context->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&context1);
		if (SUCCEEDED(hr)) {
			context->Release();
			context = (ID3D11DeviceContext*)context1;
		}
	}

	// swapchain
	{
		IDXGIFactory2* factory = {};
		if (FAILED(hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory))) {
			__debugbreak();
		}

		DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
		swapchainDesc.Width = swapchainWidth;
		swapchainDesc.Height = swapchainHeight;
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.BufferCount = 2;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.SampleDesc.Count = 1;
		swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		IDXGISwapChain1* swapchain1;
		if (FAILED(factory->CreateSwapChainForHwnd((IUnknown*)device, window, &swapchainDesc, NULL, NULL, &swapchain1))) {
			// Windows 8.1
			swapchainDesc.BufferCount = 2;
			swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
			hr = factory->CreateSwapChainForHwnd((IUnknown*)device, window, &swapchainDesc, NULL, NULL, &swapchain1);
			if (FAILED(hr)) { __debugbreak(); }
		}
		swapchain = (IDXGISwapChain3*)swapchain1;
		factory->Release();
	}

	// rasterizer
	{
		D3D11_RASTERIZER_DESC desc = {};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.FrontCounterClockwise = FALSE;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = TRUE;
		desc.ScissorEnable = FALSE;
		desc.MultisampleEnable = 0;
		desc.AntialiasedLineEnable = FALSE;

		hr = device->CreateRasterizerState(&desc, &rasterizerState);
		if (FAILED(hr)) { __debugbreak(); }
	}

	// depth stencil state
	{
		D3D11_DEPTH_STENCIL_DESC desc = {};
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilEnable = FALSE;
		desc.StencilReadMask = 0;
		desc.StencilWriteMask = 0;
		hr = device->CreateDepthStencilState(&desc, &depthStencilState);
		if (FAILED(hr)) { __debugbreak(); }
	}

	// blend state
	{
		D3D11_RENDER_TARGET_BLEND_DESC blendDesc = {};
		blendDesc.BlendEnable = FALSE;
		blendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendDesc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		D3D11_BLEND_DESC desc = {};
		desc.AlphaToCoverageEnable = FALSE;
		desc.IndependentBlendEnable = FALSE;
		desc.RenderTarget[0] = blendDesc;

		hr = device->CreateBlendState(&desc, &blendState);
		if (FAILED(hr)) { __debugbreak(); }
	}

	/*
	#if !USE_PRECOMPILED_SHADERS
	UINT shader_flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS
		#if USE_DEBUG_MODE
		  D3DCOMPILE_OPTIMIZATION_LEVEL0 | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
		#else
		  D3DCOMPILE_OPTIMIZATION_LEVEL3;
		#endif
	#endif
	*/
	UINT shader_flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_OPTIMIZATION_LEVEL0 | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;

	// vertex shader
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, x), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, nx), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, tx), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ID3DBlob* code = NULL;
		const void* vshader;
		size_t vshader_size;

		/*
		#if USE_PRECOMPILED_SHADERS
		vshader = d3d11_vshader;
		vshader_size = sizeof(d3d11_vshader);
		#else
		*/

		ID3DBlob* errorBuff;
		hr = D3DCompileFromFile(L"VertexShader.hlsl", 0, 0, "main", "vs_5_0",
			shader_flags, 0, &code, &errorBuff);
		if (FAILED(hr)) { 
			OutputDebugFromRenderer((char*)errorBuff->GetBufferPointer());
			__debugbreak();
		}
		vshader = code->GetBufferPointer();
		vshader_size = code->GetBufferSize();

		//#endif

		hr = device->CreateVertexShader(vshader, vshader_size, NULL, &vertexShader);
		if (FAILED(hr)) { __debugbreak(); }

		hr = device->CreateInputLayout(layout, _countof(layout), vshader, vshader_size, &inputLayout);
		if (FAILED(hr)) { __debugbreak(); }
	}

	// pixel shader
	{
		ID3DBlob* code = NULL;
		const void* pshader;
		size_t pshader_size;

		/*
		#if USE_PRECOMPILED_SHADERS
		pshader = d3d11_pshader;
		pshader_size = sizeof(d3d11_pshader);
		#else
		*/

		ID3DBlob* errorBuff;
		hr = D3DCompileFromFile(L"PixelShader.hlsl", 0, 0, "main", "ps_5_0",
			shader_flags, 0, &code, &errorBuff);
		if (FAILED(hr)) { __debugbreak(); }
		pshader = code->GetBufferPointer();
		pshader_size = code->GetBufferSize();

		//#endif

		hr = device->CreatePixelShader(pshader, pshader_size, NULL, &pixelShader);
		if (FAILED(hr)) { __debugbreak(); }
	}

	// vertex buffer
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = v_buf->num_vertices * sizeof(Vertex);
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = v_buf->vertices;

		hr = device->CreateBuffer(&desc, &data, &vertexBuffer);
		if (FAILED(hr)) { __debugbreak(); }
	}

	// index buffer
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = i_buf->num_indices * sizeof(i32);
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = i_buf->indices;

		hr = device->CreateBuffer(&desc, &data, &indexBuffer);
		if (FAILED(hr)) { __debugbreak(); }
	}

	// constant buffer
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(Constants) + 0xf & 0xfffffff0;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		device->CreateBuffer(&desc, NULL, &constantBuffer);
	}

	// texture
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = my_image->width;
		textureDesc.Height = my_image->height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA textureData = {};
		textureData.pSysMem = my_image->data;
		textureData.SysMemPitch = my_image->width * 4; // 4 bytes per pixel

		device->CreateTexture2D(&textureDesc, &textureData, &texture);
		device->CreateShaderResourceView(texture, nullptr, &textureView);
	}

	// sampler
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		device->CreateSamplerState(&samplerDesc, &samplerState);
	}

	/*
	UINT flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	hr = swapchain->ResizeBuffers(0, swapchainWidth, swapchainHeight, DXGI_FORMAT_UNKNOWN, flags);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET || hr == DXGI_ERROR_DRIVER_INTERNAL_ERROR) {
		// if (failed (recreate device()) )
		__debugbreak();
	}
	else {
		__debugbreak();
	}
	*/

	ID3D11Texture2D* windowBuffer = {};
	hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&windowBuffer);
	if (FAILED(hr)) { __debugbreak(); }

	hr = device->CreateRenderTargetView((ID3D11Resource*)windowBuffer, NULL, &windowRTView);
	windowBuffer->Release();
	if (FAILED(hr)) { __debugbreak(); }

	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = swapchainWidth;
		desc.Height = swapchainHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ID3D11Texture2D* depthStencil = {};
		hr = device->CreateTexture2D(&desc, NULL, &depthStencil);
		if (FAILED(hr)) { __debugbreak(); }

		hr = device->CreateDepthStencilView((ID3D11Resource*)depthStencil, NULL, &windowDPView);
		depthStencil->Release();
		if (FAILED(hr)) { __debugbreak(); }
	}

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = (f32)swapchainWidth;
	viewport.Height = (f32)swapchainHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);
}

HRESULT Renderer::RendererResize(HWND window, i32 swapchainWidth, i32 swapchainHeight) {
	if (swapchainWidth == 0 || swapchainHeight == 0) {
		return S_OK;
	}

	if (windowRTView) {
		context->OMSetRenderTargets(0, NULL, NULL);
		windowRTView->Release();
		windowRTView = NULL;
	}

	if (windowDPView) {
		windowDPView->Release();
		windowDPView = NULL;
	}

	UINT flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	HRESULT hr = swapchain->ResizeBuffers(0, swapchainWidth, swapchainHeight, DXGI_FORMAT_UNKNOWN, flags);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET || hr == DXGI_ERROR_DRIVER_INTERNAL_ERROR) {
		// if (failed (recreate device()) )
		__debugbreak();
	}
	else {
		__debugbreak();
	}

	ID3D11Texture2D* windowBuffer;
	hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&windowBuffer);
	if (FAILED(hr)) { __debugbreak(); }

	hr = device->CreateRenderTargetView((ID3D11Resource*)windowBuffer, NULL, &windowRTView);
	windowBuffer->Release();
	if (FAILED(hr)) { __debugbreak(); }

	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = swapchainWidth;
		desc.Height = swapchainHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ID3D11Texture2D* depthStencil = {};
		hr = device->CreateTexture2D(&desc, NULL, &depthStencil);
		if (FAILED(hr)) { __debugbreak(); }

		hr = device->CreateDepthStencilView((ID3D11Resource*)depthStencil, NULL, &windowDPView);
		depthStencil->Release();
		if (FAILED(hr)) { __debugbreak(); }
	}

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = (f32)swapchainWidth;
	viewport.Height = (f32)swapchainHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);
	return S_OK;
}

HRESULT Renderer::RenderPresent(HWND window) {
	HRESULT hr = S_OK;
	if (renderer_occluded) {
		hr = swapchain->Present(0, DXGI_PRESENT_TEST);
		if (SUCCEEDED(hr) && hr != DXGI_STATUS_OCCLUDED) {
			// DXGI window is back to normal, resuming rendering
			renderer_occluded = 0;
		}
	}

	if (!renderer_occluded) {
		hr = swapchain->Present(0, 0);
	}

	if (hr == DXGI_ERROR_DEVICE_RESET || hr == DXGI_ERROR_DEVICE_REMOVED) {
		//if (FAILED(RecreateDevice(wnd))) {
		//	return FatalDeviceLostError();
		//}

		//RECT rect;
		//if (!GetClientRect(wnd, &rect)) {
		//	LogWin32LastError("GetClientRect failed");
		//}
		//else {
		//	RenderResize(wnd, rect.right - rect.left, rect.bottom - rect.top);
		//}
		__debugbreak();
	}
	else if (hr == DXGI_STATUS_OCCLUDED) {
		// DXGI window is occluded, skipping rendering
		renderer_occluded = 1;
	}
	else if (hr == S_OK) {
		// do nothing because its not an error
	}
	else {
		__debugbreak();
		// LOG_AND_RETURN_ERROR(hr, "IDXGISwapChain::Present failed");
	}

	if (renderer_occluded) {
		Sleep(10);
	}
	else {
		if (context1) {
			context1->DiscardView((ID3D11View*)windowRTView);
		}
	}

	return S_OK;
}

void Renderer::RenderFrame(Memory* gameMemory, ModelBuffer* m_buffer) {
	if (!renderer_occluded) {
		//if (render_frame_latency_wait) {
		//	WaitForSingleObjectEx(render_frame_latency_wait, INFINITE, TRUE);
		//}

		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

		Constants* constants = (Constants*)(mappedSubresource.pData);


		GameState* gameState = (GameState*)gameMemory->data;
		mat4 translate = TranslateMat(Vec3(0.0f, 0.0f, 1.0f));
		mat4 scale = ScaleMat(OneVec());
		mat4 rotate = RotateMat(180, UpVec());

		mat4 view = gameState->mainCamera.view;
		mat4 proj = gameState->mainCamera.proj;

		constants->mvp = MulMat(proj, MulMat(view, MulMat(rotate, MulMat(scale, translate))));
		// constants->LightVector = { 1.0f, -1.0f, 1.0f };

		context->Unmap(constantBuffer, 0);

		context->OMSetRenderTargets(1, &windowRTView, windowDPView);
		context->OMSetDepthStencilState(depthStencilState, 0);
		context->ClearDepthStencilView(windowDPView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		// clear background
		FLOAT clear_color[] = { 100.f / 255.f, 149.f / 255.f, 237.f / 255.f, 1.f };
		context->ClearRenderTargetView(windowRTView, clear_color);

		// draw a triangle
		const UINT stride = sizeof(struct Vertex);
		const UINT offset = 0;
		context->IASetInputLayout(inputLayout);
		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		context->RSSetState(rasterizerState);
		
		context->VSSetShader(vertexShader, NULL, 0);
		
		context->PSSetShader(pixelShader, NULL, 0);
		context->PSSetShaderResources(0, 1, &textureView);
		context->PSSetSamplers(0, 1, &samplerState);

		context->OMSetBlendState(blendState, NULL, ~0U);
		
		for (i32 i = 0; i < m_buffer->num_models; i++) {
			context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, m_buffer->models[i].start_index * sizeof(i32));
			context->VSSetConstantBuffers(0, 1, &constantBuffer);
			context->DrawIndexed(m_buffer->models[i].length, 0, 0);
		}	
	}
}
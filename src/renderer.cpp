#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "renderer.h"

Vertex CreateVertex(vec3 pos, vec3 norm, vec2 tex) {
	Vertex result = {};
	result.x = pos.x;
	result.y = pos.y;
	result.z = pos.z;
	result.nx = norm.x;
	result.ny = norm.y;
	result.nz = norm.z;
	result.tx = tex.x;
	result.ty = tex.y;
	return result;
}

void InitRenderer(VertexBuffer* v_buf, IndexBuffer* i_buf, MeshBuffer* meshes, PermanentResourceAllocator* allocator) {
	v_buf->buffer_length = 100000000;
	v_buf->num_vertices = 0;
	v_buf->vertices = (Vertex*)allocator->Allocate(sizeof(Vertex) * v_buf->buffer_length);

	i_buf->buffer_length = 100000000;
	i_buf->num_indices = 0;
	i_buf->indices = (i32*)allocator->Allocate(sizeof(i32) * i_buf->buffer_length);

	meshes->buffer_length = 100;
	meshes->num_meshes = 0;
	meshes->meshes = (Mesh*)allocator->Allocate(sizeof(Mesh) * meshes->buffer_length);
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

void LoadOBJ(char* filename, VertexBuffer* v_buffer, IndexBuffer* i_buffer, MeshBuffer* m_buffer, PermanentResourceAllocator* allocator) {
	// load obj
	debug_ReadFileResult obj_file = debug_ReadFile(filename);
	uchar* ptr = (uchar*)obj_file.data;

	// find out how many faces and indices we will need for temp storage and mesh data
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
	m_buffer->meshes[m_buffer->num_meshes].start_index = i_buffer->num_indices;
	m_buffer->meshes[m_buffer->num_meshes].length = num_indices;
	m_buffer->num_meshes += 1;


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

void Renderer::InitD3D11(HWND window, i32 swapchain_width, i32 swapchain_height, VertexBuffer* v_buf, IndexBuffer* i_buf, Image* images, int num_images, TextVertex* text_vert_buffer, int num_text_verts) {
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

		DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
		swapchain_desc.Width = swapchain_width;
		swapchain_desc.Height = swapchain_height;
		swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchain_desc.BufferCount = 2;
		swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchain_desc.SampleDesc.Count = 1;
		swapchain_desc.Scaling = DXGI_SCALING_STRETCH;
		swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		IDXGISwapChain1* swapchain1;
		if (FAILED(factory->CreateSwapChainForHwnd((IUnknown*)device, window, &swapchain_desc, NULL, NULL, &swapchain1))) {
			// Windows 8.1
			swapchain_desc.BufferCount = 2;
			swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
			hr = factory->CreateSwapChainForHwnd((IUnknown*)device, window, &swapchain_desc, NULL, NULL, &swapchain1);
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
		desc.FrontCounterClockwise = TRUE;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = TRUE;
		desc.ScissorEnable = FALSE;
		desc.MultisampleEnable = 0;
		desc.AntialiasedLineEnable = FALSE;

		hr = device->CreateRasterizerState(&desc, &rasterizer_state);
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
		hr = device->CreateDepthStencilState(&desc, &depth_stencil_state);
		if (FAILED(hr)) { __debugbreak(); }
	}

	// blend state
	{
		D3D11_RENDER_TARGET_BLEND_DESC blend_desc = {};
		blend_desc.BlendEnable = FALSE;
		blend_desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blend_desc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		D3D11_BLEND_DESC desc = {};
		desc.AlphaToCoverageEnable = FALSE;
		desc.IndependentBlendEnable = FALSE;
		desc.RenderTarget[0] = blend_desc;

		hr = device->CreateBlendState(&desc, &blend_state);
		if (FAILED(hr)) { __debugbreak(); }
		

		blend_desc = {};
		blend_desc.BlendEnable = TRUE;
		blend_desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blend_desc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		desc = {};
		desc.AlphaToCoverageEnable = FALSE;
		desc.IndependentBlendEnable = FALSE;
		desc.RenderTarget[0] = blend_desc;

		hr = device->CreateBlendState(&desc, &transparency_blend_state);
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

		ID3DBlob* error_buffer;
		hr = D3DCompileFromFile(L"shaders/GeneralVS.hlsl", 0, 0, "main", "vs_5_0",
			shader_flags, 0, &code, &error_buffer);
		if (FAILED(hr)) { 
			DebugPrint((char*)error_buffer->GetBufferPointer());
			__debugbreak();
		}
		vshader = code->GetBufferPointer();
		vshader_size = code->GetBufferSize();

		//#endif

		hr = device->CreateVertexShader(vshader, vshader_size, NULL, &vertex_shader);
		if (FAILED(hr)) { __debugbreak(); }

		hr = device->CreateInputLayout(layout, _countof(layout), vshader, vshader_size, &input_layout);
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

		ID3DBlob* error_buffer;
		hr = D3DCompileFromFile(L"shaders/GeneralPS.hlsl", 0, 0, "main", "ps_5_0",
			shader_flags, 0, &code, &error_buffer);
		if (FAILED(hr)) { 
			DebugPrint((char*)error_buffer->GetBufferPointer());
			__debugbreak(); 
		}
		pshader = code->GetBufferPointer();
		pshader_size = code->GetBufferSize();

		//#endif

		hr = device->CreatePixelShader(pshader, pshader_size, NULL, &pixel_shader);
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

		hr = device->CreateBuffer(&desc, &data, &vertex_buffer);
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

		hr = device->CreateBuffer(&desc, &data, &index_buffer);
		if (FAILED(hr)) { __debugbreak(); }
	}

	// constant buffer
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(Constants) + 0xf & 0xfffffff0;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		device->CreateBuffer(&desc, NULL, &constant_buffer);
	}

	// textures
	{
		for (int i = 0; i < num_images; i++) {
			D3D11_TEXTURE2D_DESC texture_desc = {};
			texture_desc.Width = images[i].width;
			texture_desc.Height = images[i].height;
			texture_desc.MipLevels = 1;
			texture_desc.ArraySize = 1;
			texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			texture_desc.SampleDesc.Count = 1;
			texture_desc.Usage = D3D11_USAGE_IMMUTABLE;
			texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

			D3D11_SUBRESOURCE_DATA texture_data = {};
			texture_data.pSysMem = images[i].data;
			texture_data.SysMemPitch = images[i].width * 4; // 4 bytes per pixel

			device->CreateTexture2D(&texture_desc, &texture_data, &textures[i]);
			device->CreateShaderResourceView(textures[i], nullptr, &texture_views[i]);
		}
	}

	// sampler
	{
		D3D11_SAMPLER_DESC sampler_desc = {};
		sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampler_desc.MaxAnisotropy = 16;

		device->CreateSamplerState(&sampler_desc, &sampler_state);
	}

	// text vertex shader
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(TextVertex, x), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(TextVertex, r), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(TextVertex, tx), D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

		ID3DBlob* error_buffer;
		hr = D3DCompileFromFile(L"shaders/TextVS.hlsl", 0, 0, "main", "vs_5_0",
			shader_flags, 0, &code, &error_buffer);
		if (FAILED(hr)) {
			DebugPrint((char*)error_buffer->GetBufferPointer());
			__debugbreak();
		}
		vshader = code->GetBufferPointer();
		vshader_size = code->GetBufferSize();

		//#endif

		hr = device->CreateVertexShader(vshader, vshader_size, NULL, &text_vs);
		if (FAILED(hr)) { __debugbreak(); }

		hr = device->CreateInputLayout(layout, _countof(layout), vshader, vshader_size, &text_il);
		if (FAILED(hr)) { __debugbreak(); }
	}

	// text pixel shader
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

		ID3DBlob* error_buffer;
		hr = D3DCompileFromFile(L"shaders/TextPS.hlsl", 0, 0, "main", "ps_5_0",
			shader_flags, 0, &code, &error_buffer);
		if (FAILED(hr)) {
			DebugPrint((char*)error_buffer->GetBufferPointer());
			__debugbreak();
		}
		pshader = code->GetBufferPointer();
		pshader_size = code->GetBufferSize();

		//#endif

		hr = device->CreatePixelShader(pshader, pshader_size, NULL, &text_ps);
		if (FAILED(hr)) { __debugbreak(); }
	}

	// text vertex buffer
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = Renderer::MAX_NUM_TEXT_CHARS * 4 * sizeof(TextVertex);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = text_vert_buffer;

		hr = device->CreateBuffer(&desc, &data, &text_vertex_buffer);
		if (FAILED(hr)) { __debugbreak(); }
	}

	// text index buffer
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = Renderer::MAX_NUM_TEXT_CHARS * 6 * sizeof(i32);
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		i32 indices[6144];
		int index = 0;
		for (int i = 0; i < Renderer::MAX_NUM_TEXT_CHARS * 4; i += 4) {
			indices[index + 0] = i + 0;
			indices[index + 1] = i + 2;
			indices[index + 2] = i + 1;
			indices[index + 3] = i + 3;
			indices[index + 4] = i + 1;
			indices[index + 5] = i + 2;
			index += 6;
		}

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = indices;

		hr = device->CreateBuffer(&desc, &data, &text_index_buffer);
		if (FAILED(hr)) { __debugbreak(); }
	}

	ID3D11Texture2D* windowBuffer = {};
	hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&windowBuffer);
	if (FAILED(hr)) { __debugbreak(); }

	hr = device->CreateRenderTargetView((ID3D11Resource*)windowBuffer, NULL, &window_rt_view);
	windowBuffer->Release();
	if (FAILED(hr)) { __debugbreak(); }

	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = swapchain_width;
		desc.Height = swapchain_height;
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

		hr = device->CreateDepthStencilView((ID3D11Resource*)depthStencil, NULL, &window_ds_view);
		depthStencil->Release();
		if (FAILED(hr)) { __debugbreak(); }
	}

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = (f32)swapchain_width;
	viewport.Height = (f32)swapchain_height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);
}

void Renderer::RendererResize(HWND window, i32 swapchain_width, i32 swapchain_height, i32 old_width, i32 old_height) {
	if (window_rt_view == NULL || swapchain_width != old_width || swapchain_height != old_height) {
		if (window_rt_view) {
			context->OMSetRenderTargets(0, NULL, NULL);
			context->ClearState();
			window_rt_view->Release();
			window_rt_view = NULL;
			window_ds_view->Release();
			window_ds_view = NULL;
		}

		if (swapchain_width != 0 && swapchain_height != 0) {
			UINT flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
			HRESULT hr = swapchain->ResizeBuffers(0, swapchain_width, swapchain_height, DXGI_FORMAT_UNKNOWN, flags);
			if (FAILED(hr)) {
				OutputDebugStringA((char*)"Failed to Resize the Swapchain\n");
				__debugbreak();
			}

			D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};
			rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

			ID3D11Texture2D* window_buffer;
			hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&window_buffer);
			if (FAILED(hr)) { __debugbreak(); }

			hr = device->CreateRenderTargetView((ID3D11Resource*)window_buffer, &rtv_desc, &window_rt_view);
			if (FAILED(hr)) { __debugbreak(); }
			window_buffer->Release();

			{
				D3D11_TEXTURE2D_DESC desc = {};
				desc.Width = swapchain_width;
				desc.Height = swapchain_height;
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

				hr = device->CreateDepthStencilView((ID3D11Resource*)depthStencil, NULL, &window_ds_view);
				if (FAILED(hr)) { __debugbreak(); }
				depthStencil->Release();
			}
		}
	}
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
		hr = swapchain->Present(1, 0);
	}

	if (hr == DXGI_STATUS_OCCLUDED) {
		renderer_occluded = 1;
	}

	if (renderer_occluded) {
		Sleep(10);
	}
	else {
		if (context1) {
			context1->DiscardView((ID3D11View*)window_rt_view);
		}
	}

	return S_OK;
}

void Renderer::RenderFrame(Memory* game_memory, MeshBuffer* m_buffer, Model* models, RenderData* render_data, TextVertex* text_vert_buffer) {
	if (!renderer_occluded) {
		//if (render_frame_latency_wait) {
		//	WaitForSingleObjectEx(render_frame_latency_wait, INFINITE, TRUE);
		//}

		context->OMSetRenderTargets(1, &window_rt_view, window_ds_view);
		context->OMSetDepthStencilState(depth_stencil_state, 0);
		context->ClearDepthStencilView(window_ds_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// clear background
		FLOAT clear_color[] = { 100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f };
		context->ClearRenderTargetView(window_rt_view, clear_color);

		context->RSSetState(rasterizer_state);
		context->OMSetBlendState(transparency_blend_state, NULL, ~0U);

		context->VSSetShader(vertex_shader, NULL, 0);
		context->PSSetShader(pixel_shader, NULL, 0);
		context->PSSetSamplers(0, 1, &sampler_state);
		
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		context->IASetInputLayout(input_layout);
		context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

		GameState* gs = (GameState*)game_memory->data;
		mat4 view = gs->main_camera.view;
		mat4 proj = gs->main_camera.proj;

		int distances_count = 0;
		float distances[100];
		int indexes[100];
		for (int i = 0; i < 100; i++) {
			distances[i] = -1.0f;
			indexes[i] = -1;
		}

		for (int i = 0; i < render_data->num_entities; i++) {
			RenderEntity re = render_data->entities[i];
			Model m = models[re.model_index];

			if (m.opaque) {
				mat4 translate, scale, rotate;
				translate = TranslateMat(re.pos);
				scale = ScaleMat(re.scale);
				rotate = RotateMat(re.rot_angle, re.rot_axis);

				mat4 world = MulMat(translate, MulMat(rotate, scale));

				D3D11_MAPPED_SUBRESOURCE mappedSubresource;
				context->Map(constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
				Constants* constants = (Constants*)(mappedSubresource.pData);
				constants->m = world;
				constants->mvp = MulMat(proj, MulMat(view, world));
				constants->camera_pos = gs->main_camera.pos;
				context->Unmap(constant_buffer, 0);
				context->VSSetConstantBuffers(0, 1, &constant_buffer);
			
				context->PSSetShaderResources(0, 1, &texture_views[m.h_texture.handle]);
			
				context->DrawIndexed(m_buffer->meshes[m.h_mesh.handle].length, m_buffer->meshes[m.h_mesh.handle].start_index, 0);
			}
			else {
				int current_index = i;
				float distance_to_cam = Distance(gs->main_camera.pos, re.pos);
				for (int j = 0; j < 100; j++) {
					if (distance_to_cam > distances[j]) {
						float temp = distances[j];
						distances[j] = distance_to_cam;
						distance_to_cam = temp;

						int temp2 = indexes[j];
						indexes[j] = current_index;
						current_index = temp2;
					}
					else if (distance_to_cam == -1.0f && distances[j] == -1.0f) {
						break;
					}
				}

				distances_count += 1;
			}
		}

		for (int i = 0; i < distances_count; i++) {
			RenderEntity re = render_data->entities[indexes[i]];
			Model m = models[re.model_index];

			mat4 translate, scale, rotate;
			translate = TranslateMat(re.pos);
			scale = ScaleMat(re.scale);
			rotate = RotateMat(re.rot_angle, re.rot_axis);

			mat4 world = MulMat(translate, MulMat(rotate, scale));

			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			context->Map(constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
			Constants* constants = (Constants*)(mappedSubresource.pData);
			constants->m = world;
			constants->mvp = MulMat(proj, MulMat(view, world));
			constants->camera_pos = gs->main_camera.pos;
			context->Unmap(constant_buffer, 0);
			context->VSSetConstantBuffers(0, 1, &constant_buffer);

			context->PSSetShaderResources(0, 1, &texture_views[m.h_texture.handle]);

			context->DrawIndexed(m_buffer->meshes[m.h_mesh.handle].length, m_buffer->meshes[m.h_mesh.handle].start_index, 0);
		}

		context->VSSetShader(text_vs, NULL, 0);
		context->PSSetShader(text_ps, NULL, 0);
		context->PSSetSamplers(0, 1, &sampler_state);

		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		context->Map(text_vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		memcpy(mappedSubresource.pData, text_vert_buffer, sizeof(TextVertex) * Renderer::MAX_NUM_TEXT_CHARS * 4);
		context->Unmap(text_vertex_buffer, 0);

		stride = sizeof(TextVertex);
		offset = 0;
		context->IASetInputLayout(text_il);
		context->IASetVertexBuffers(0, 1, &text_vertex_buffer, &stride, &offset);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetIndexBuffer(text_index_buffer, DXGI_FORMAT_R32_UINT, 0);

		context->PSSetShaderResources(0, 1, &texture_views[5]);
		context->DrawIndexed(NUM_CHARS_TO_RENDER * 6, 0, 0);
	}
}

void Renderer::UpdateViewport(Viewport game_viewport) {
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = game_viewport.pos.x;
	viewport.TopLeftY = game_viewport.pos.y;
	viewport.Width = game_viewport.size.x;
	viewport.Height = game_viewport.size.y;
	viewport.MinDepth = game_viewport.depth.x;
	viewport.MaxDepth = game_viewport.depth.y;

	context->RSSetViewports(1, &viewport);
}
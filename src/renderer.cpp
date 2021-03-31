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

void InitRenderer(VertexBuffer* v_buf, IndexBuffer* i_buf, ModelBuffer* models, PermanentResourceAllocator* allocator) {
	v_buf->buffer_length = 100000000;
	v_buf->num_vertices = 0;
	v_buf->vertices = (Vertex*)allocator->Allocate(sizeof(Vertex) * v_buf->buffer_length);

	i_buf->buffer_length = 100000000;
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

void GenerateTerrainModel(GameMap* gameMap, VertexBuffer* v_buf, IndexBuffer* i_buf, ModelBuffer* models) {
	models->models[models->num_models].start_index = i_buf->num_indices;
	models->models[models->num_models].length = 24 * gameMap->mapWidth * gameMap->mapHeight;
	models->num_models += 1;

	vec3 norm = ZeroVec();
	i32 width = gameMap->mapWidth;
	i32 height = gameMap->mapHeight;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			f32 my_elevation = (f32)gameMap->tiles[x + y * gameMap->mapWidth].elevation;

			f32 west_neighbor_elevation = 0;
			f32 south_neighbor_elevation = 0;
			f32 southwest_neighbor_elevation = 0;
			if ((x - 1) >= 0) {
				west_neighbor_elevation = (f32)gameMap->tiles[(x - 1) + y * gameMap->mapWidth].elevation;
			}
			if ((y - 1) >= 0) {
				south_neighbor_elevation = (f32)gameMap->tiles[x + (y - 1) * gameMap->mapWidth].elevation;
			}
			if (((x - 1) >= 0) && ((y - 1) >= 0)) {
				southwest_neighbor_elevation = (f32)gameMap->tiles[(x - 1) + (y - 1) * gameMap->mapWidth].elevation;
			}

			vec3 a0 = { (f32)x + 0.0f, west_neighbor_elevation, (f32)y + 1.0f };
			vec3 a1 = { (f32)x + 0.25f, my_elevation, (f32)y + 1.0f };
			vec3 a2 = { (f32)x + 0.0f, west_neighbor_elevation, (f32)y + 0.25f };
			norm = Cross(SubVec(a1, a0), SubVec(a2, a0));
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(a0, norm, Vec2(0, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(a1, norm, Vec2(1, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(a2, norm, Vec2(0, 0));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;

			vec3 b0 = { (f32)x + 0.25f, my_elevation, (f32)y + 1.0f };
			vec3 b1 = { (f32)x + 0.0f, west_neighbor_elevation, (f32)y + 0.25f };
			vec3 b2 = { (f32)x + 0.25f, my_elevation, (f32)y + 0.25f };
			norm = Cross(SubVec(b2, b0), SubVec(b1, b0));
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(b0, norm, Vec2(1, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(b2, norm, Vec2(0, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(b1, norm, Vec2(0, 0));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;

			vec3 c0 = { (f32)x + 0.25f, my_elevation, (f32)y + 1.0f };
			vec3 c1 = { (f32)x + 1.0f, my_elevation, (f32)y + 1.0f };
			vec3 c2 = { (f32)x + 0.25f, my_elevation, (f32)y + 0.25f };
			norm = Cross(SubVec(c1, c0), SubVec(c2, c0));
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(c0, norm, Vec2(0, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(c1, norm, Vec2(1, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(c2, norm, Vec2(1, 0));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;

			vec3 d0 = { (f32)x + 1.0f, my_elevation, (f32)y + 1.0f };
			vec3 d1 = { (f32)x + 0.25f, my_elevation, (f32)y + 0.25f };
			vec3 d2 = { (f32)x + 1.0f, my_elevation, (f32)y + 0.25f };
			norm = Cross(SubVec(d2, d0), SubVec(d1, d0));
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(d0, norm, Vec2(1, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(d2, norm, Vec2(0, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(d1, norm, Vec2(0, 0));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;

			vec3 e0 = { (f32)x + 0.0f, west_neighbor_elevation, (f32)y + 0.25f };
			vec3 e1 = { (f32)x + 0.25f, my_elevation, (f32)y + 0.25f };
			vec3 e2 = { (f32)x + 0.0f, southwest_neighbor_elevation, (f32)y + 0.0f };
			norm = Cross(SubVec(e1, e0), SubVec(e2, e0));
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(e0, norm, Vec2(0, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(e1, norm, Vec2(1, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(e2, norm, Vec2(0, 0));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;

			vec3 f0 = { (f32)x + 0.25f, my_elevation, (f32)y + 0.25f };
			vec3 f1 = { (f32)x + 0.0f, southwest_neighbor_elevation, (f32)y + 0.0f };
			vec3 f2 = { (f32)x + 0.25f, south_neighbor_elevation, (f32)y + 0.0f };
			norm = Cross(SubVec(f2, f0), SubVec(f1, f0));
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(f0, norm, Vec2(1, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(f2, norm, Vec2(0, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(f1, norm, Vec2(0, 0));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;

			vec3 g0 = { (f32)x + 0.25f, my_elevation, (f32)y + 0.25f };
			vec3 g1 = { (f32)x + 1.0f, my_elevation, (f32)y + 0.25f };
			vec3 g2 = { (f32)x + 0.25f, south_neighbor_elevation, (f32)y + 0.0f };
			norm = Cross(SubVec(g1, g0), SubVec(g2, g0));
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(g0, norm, Vec2(0, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(g1, norm, Vec2(1, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(g2, norm, Vec2(0, 0));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;

			vec3 h0 = { (f32)x + 1.0f, my_elevation, (f32)y + 0.25f };
			vec3 h1 = { (f32)x + 0.25f, south_neighbor_elevation, (f32)y + 0.0f };
			vec3 h2 = { (f32)x + 1.0f, south_neighbor_elevation, (f32)y + 0.0f };
			norm = Cross(SubVec(h2, h0), SubVec(h1, h0));
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(h0, norm, Vec2(1, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(h2, norm, Vec2(0, 1));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
			v_buf->vertices[v_buf->num_vertices] = CreateVertex(h1, norm, Vec2(0, 0));
			i_buf->indices[i_buf->num_indices++] = v_buf->num_vertices++;
		}
	}
}

void GenerateTerrainMapModel(TerrainMap* terrMap, VertexBuffer* v_buf, IndexBuffer* i_buf, ModelBuffer* models) {
	models->models[models->num_models].start_index = i_buf->num_indices;
	models->models[models->num_models].length = 6 * (terrMap->width-1) * (terrMap->height-1);
	models->num_models += 1;

	// Generate vertices
	i32 width = terrMap->width;
	i32 height = terrMap->height;
	Vertex* vertices = (Vertex*)malloc(sizeof(Vertex) * width * height);

	vec3 norm = ZeroVec();
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			f32 center = terrMap->elevation[x + y * width];
			
			f32 right, left, botleft, bot, top, topright;
			if (x + 1 < width) {
				right = terrMap->elevation[(x + 1) + y * width];
			}
			else {
				right = center;
			}
			if (x - 1 >= 0) {
				left = terrMap->elevation[(x-1) + y * width];
			}
			else {
				left = center;
			}
			if (x - 1 >= 0 && y - 1 >= 0) {
				botleft = terrMap->elevation[(x-1) + (y-1) * width];
			}
			else {
				botleft = center;
			}
			if (y - 1 >= 0) {
				bot = terrMap->elevation[x + (y-1) * width];
			}
			else {
				bot = center;
			}
			if (x + 1 < width && y + 1 < height) {
				topright = terrMap->elevation[(x+1) + (y+1) * width];
			}
			else {
				topright = center;
			}
			if (y+1 < height) {
				top = terrMap->elevation[x + (y + 1) * width];
			}
			else {
				top = center;
			}

			vec3 c = Vec3(x, center, y);
			vec3 l = Vec3(x-1, left, y);
			vec3 b = Vec3(x, bot, y-1);
			vec3 t = Vec3(x, top, y+1);
			vec3 r = Vec3(x+1, right, y);
			vec3 tr = Vec3(x+1, topright, y+1);
			vec3 bl = Vec3(x-1, botleft, y-1);

			vec3 to_l = SubVec(l, c);
			vec3 to_b = SubVec(b, c);
			vec3 to_t = SubVec(t, c);
			vec3 to_r = SubVec(r, c);
			vec3 to_tr = SubVec(tr, c);
			vec3 to_bl = SubVec(bl, c);

			vec3 norm_lbl = Cross(to_bl, to_l);
			vec3 norm_blb = Cross(to_b, to_bl);
			vec3 norm_br = Cross(to_r, to_b);
			vec3 norm_rtr = Cross(to_tr, to_r);
			vec3 norm_trt = Cross(to_t, to_tr);
			vec3 norm_tl = Cross(to_l, to_t);

			norm = AddVec(norm_lbl, norm_blb);
			norm = AddVec(norm, norm_br);
			norm = AddVec(norm, norm_rtr);
			norm = AddVec(norm, norm_trt);
			norm = AddVec(norm, norm_tl);
			norm = NormVec(norm);

			Vertex v;
			v.x = x;
			v.y = terrMap->elevation[x + y * width];
			v.z = y;
			v.nx = norm.x;
			v.ny = norm.y;
			v.nz = norm.z;
			v.tx = 0.0f;
			v.ty = 0.0f;
			vertices[x + y * width] = v;
		}
	}

	for (int y = 0; y < height-1; y++) {
		for (int x = 0; x < width-1; x++) {
			int32_t tl = v_buf->num_vertices;
			int32_t tr = tl + 1;
			int32_t bl = tr + 1;
			int32_t br = bl + 1;
			
			v_buf->vertices[v_buf->num_vertices++] = vertices[x + y * width];
			v_buf->vertices[v_buf->num_vertices++] = vertices[(x+1) + y * width];
			v_buf->vertices[v_buf->num_vertices++] = vertices[x + (y+1) * width];
			v_buf->vertices[v_buf->num_vertices++] = vertices[(x+1) + (y+1) * width];
			
			i_buf->indices[i_buf->num_indices++] = tl;
			i_buf->indices[i_buf->num_indices++] = bl;
			i_buf->indices[i_buf->num_indices++] = tr;
			i_buf->indices[i_buf->num_indices++] = bl;
			i_buf->indices[i_buf->num_indices++] = br;
			i_buf->indices[i_buf->num_indices++] = tr;
		}
	}

	free(vertices);
}

// ============================================================================
// D3D11

void Renderer::InitD3D11(HWND window, i32 swapchainWidth, i32 swapchainHeight, VertexBuffer* v_buf, IndexBuffer* i_buf, Image* images, int numImages, TextVertex* textVertBuffer, int numTextVerts) {
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
		

		blendDesc = {};
		blendDesc.BlendEnable = TRUE;
		blendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendDesc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		desc = {};
		desc.AlphaToCoverageEnable = FALSE;
		desc.IndependentBlendEnable = FALSE;
		desc.RenderTarget[0] = blendDesc;

		hr = device->CreateBlendState(&desc, &transparencyBlendState);
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
		hr = D3DCompileFromFile(L"shaders/GeneralVS.hlsl", 0, 0, "main", "vs_5_0",
			shader_flags, 0, &code, &errorBuff);
		if (FAILED(hr)) { 
			DebugPrint((char*)errorBuff->GetBufferPointer());
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
		hr = D3DCompileFromFile(L"shaders/GeneralPS.hlsl", 0, 0, "main", "ps_5_0",
			shader_flags, 0, &code, &errorBuff);
		if (FAILED(hr)) { 
			DebugPrint((char*)errorBuff->GetBufferPointer());
			__debugbreak(); 
		}
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

	// textures
	{
		for (int i = 0; i < numImages; i++) {
			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = images[i].width;
			textureDesc.Height = images[i].height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

			D3D11_SUBRESOURCE_DATA textureData = {};
			textureData.pSysMem = images[i].data;
			textureData.SysMemPitch = images[i].width * 4; // 4 bytes per pixel

			device->CreateTexture2D(&textureDesc, &textureData, &textures[i]);
			device->CreateShaderResourceView(textures[i], nullptr, &textureViews[i]);
		}
	}

	// sampler
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		device->CreateSamplerState(&samplerDesc, &samplerState);
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

		ID3DBlob* errorBuff;
		hr = D3DCompileFromFile(L"shaders/TextVS.hlsl", 0, 0, "main", "vs_5_0",
			shader_flags, 0, &code, &errorBuff);
		if (FAILED(hr)) {
			DebugPrint((char*)errorBuff->GetBufferPointer());
			__debugbreak();
		}
		vshader = code->GetBufferPointer();
		vshader_size = code->GetBufferSize();

		//#endif

		hr = device->CreateVertexShader(vshader, vshader_size, NULL, &textVS);
		if (FAILED(hr)) { __debugbreak(); }

		hr = device->CreateInputLayout(layout, _countof(layout), vshader, vshader_size, &textIL);
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

		ID3DBlob* errorBuff;
		hr = D3DCompileFromFile(L"shaders/TextPS.hlsl", 0, 0, "main", "ps_5_0",
			shader_flags, 0, &code, &errorBuff);
		if (FAILED(hr)) {
			DebugPrint((char*)errorBuff->GetBufferPointer());
			__debugbreak();
		}
		pshader = code->GetBufferPointer();
		pshader_size = code->GetBufferSize();

		//#endif

		hr = device->CreatePixelShader(pshader, pshader_size, NULL, &textPS);
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
		data.pSysMem = textVertBuffer;

		hr = device->CreateBuffer(&desc, &data, &textVertexBuffer);
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

		hr = device->CreateBuffer(&desc, &data, &textIndexBuffer);
		if (FAILED(hr)) { __debugbreak(); }
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
		hr = swapchain->Present(1, 0);
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

void Renderer::RenderFrame(Memory* gameMemory, ModelBuffer* m_buffer, RenderData* renderData, TextVertex* textVertBuffer) {
	if (!renderer_occluded) {
		//if (render_frame_latency_wait) {
		//	WaitForSingleObjectEx(render_frame_latency_wait, INFINITE, TRUE);
		//}

		context->OMSetRenderTargets(1, &windowRTView, windowDPView);
		context->OMSetDepthStencilState(depthStencilState, 0);
		context->ClearDepthStencilView(windowDPView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// clear background
		FLOAT clear_color[] = { 100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f };
		context->ClearRenderTargetView(windowRTView, clear_color);

		context->RSSetState(rasterizerState);
		context->OMSetBlendState(blendState, NULL, ~0U);
		
		context->VSSetShader(vertexShader, NULL, 0);
		context->PSSetShader(pixelShader, NULL, 0);
		context->PSSetSamplers(0, 1, &samplerState);
		
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		context->IASetInputLayout(inputLayout);
		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		GameState* gameState = (GameState*)gameMemory->data;
		mat4 view = gameState->mainCamera.view;
		mat4 proj = gameState->mainCamera.proj;

		for (int i = 0; i < renderData->num_entities; i++) {
			RenderEntity re = renderData->entities[i];
			
			context->PSSetShaderResources(0, 1, &textureViews[re.texture_index]);
			
			mat4 translate, scale, rotate;
			translate = TranslateMat(re.pos);
			scale = ScaleMat(re.scale);
			rotate = RotateMat(re.rot_angle, re.rot_axis);

			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
			Constants* constants = (Constants*)(mappedSubresource.pData);
			constants->mvp = MulMat(proj, MulMat(view, MulMat(translate, MulMat(rotate, scale))));
			context->Unmap(constantBuffer, 0);

			context->VSSetConstantBuffers(0, 1, &constantBuffer);
			context->DrawIndexed(m_buffer->models[re.model_index].length, m_buffer->models[re.model_index].start_index, 0);
		}

		context->OMSetBlendState(transparencyBlendState, NULL, ~0U);

		context->VSSetShader(textVS, NULL, 0);
		context->PSSetShader(textPS, NULL, 0);
		context->PSSetSamplers(0, 1, &samplerState);

		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		context->Map(textVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		memcpy(mappedSubresource.pData, textVertBuffer, sizeof(TextVertex) * Renderer::MAX_NUM_TEXT_CHARS * 4);
		context->Unmap(textVertexBuffer, 0);

		stride = sizeof(TextVertex);
		offset = 0;
		context->IASetInputLayout(textIL);
		context->IASetVertexBuffers(0, 1, &textVertexBuffer, &stride, &offset);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetIndexBuffer(textIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		context->PSSetShaderResources(0, 1, &textureViews[10]);
		context->DrawIndexed(NUM_CHARS_TO_RENDER * 6, 0, 0);
	}
}
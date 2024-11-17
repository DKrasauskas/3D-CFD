#pragma once
#include "table.h"
#define thr .5f

//global variables for marching cubes
__global__ void  march_begin(__half* __restrict__ scalar, __half* color_field, int* n_triangles, int* indices, uint8_t* code) {
	uint idx = threadIdx.x + blockDim.x * blockIdx.x;
	uint idy = threadIdx.y + blockDim.y * blockIdx.y;
	uint idz = threadIdx.z + blockDim.z * blockIdx.z;
	uint index = idx + idy * (NX) + idz * (NX) * (NY);
	if (idx == NX - 1 || idy == NY - 1 || idz == NZ - 1)return;
	uint8_t hashcode = 0;

	__half2float(scalar[index])>= thr ? hashcode ^= 1 << 0 : NULL;
	__half2float(scalar[index + 1]) >= thr ? hashcode ^= 1 << 1 : NULL;
	__half2float(scalar[index + NX]) >= thr ? hashcode ^= 1 << 2 : NULL;
	__half2float(scalar[index + NX + 1]) >= thr ? hashcode ^= 1 << 3 : NULL;
	__half2float(scalar[index + (NX) * (NY)]) >= thr ? hashcode ^= 1 << 4 : NULL;
	__half2float(scalar[index + (NX) * (NY) + 1]) >= thr ? hashcode ^= 1 << 5 : NULL;
	__half2float(scalar[index + (NX) * (NY) + NX ]) >= thr ? hashcode ^= 1 << 6 : NULL;
	__half2float(scalar[index + (NX) * (NY) + NX + 1]) >= thr ? hashcode ^= 1 << 7 : NULL;
	//hashcode = 3;
	float col = __half2float(scalar[index]) + __half2float(scalar[index + 1]) + __half2float(scalar[index + NX]) + __half2float(scalar[index + NX + 1]) + __half2float(scalar[index + (NX) * (NY)]);
	col += __half2float(scalar[index + (NX) * (NY)+1]) + __half2float(scalar[index + (NX) * (NY)+NX]) + __half2float(scalar[index + (NX) * (NY)+NX + 1]);
	color_field[idx + idy * (NX - 1) + idz * (NX - 1) * (NY - 1)] = __float2half(col);
	code[idx + idy * (NX - 1) + idz * (NX - 1) * (NY - 1)] = hashcode;
	indices[idx + idy * (NX - 1) + idz * (NX - 1) * (NY - 1)] = (n_triangles[hashcode] - 1) * 4;
	//now average out the colors:

}


__global__ void compute_triangles(__half* color_field,  int* offset, int* count, uint8_t* code, vec3* in_vertices, int** lookup, float* output, int* triangle_count) {
	uint idx = threadIdx.x + blockDim.x * blockIdx.x;
	uint idy = threadIdx.y + blockDim.y * blockIdx.y;
	uint idz = threadIdx.z + blockDim.z * blockIdx.z;
	uint index = idx + idy * (NX - 1) + idz * (NX - 1) * (NY - 1);
	__shared__ vec3 vertices[12];
	if (threadIdx.x == 0 && threadIdx.y == 0 && threadIdx.z == 0) {
		for (int i = 0; i < 12; i++) {
			vertices[i] = in_vertices[i];
		}
	}
	__syncthreads();
	if (idx == NX - 1 || idy == NY - 1 || idz == NZ - 1)return;
	uint8_t hashcode = code[index];
	int begin = offset[index];
	for (int j = 0; j < 16; j++) {
		if (lookup[hashcode][j] == -1)break;
		vec3 vertex = vertices[lookup[hashcode][j]];
		output[begin] = vertex.x + idx * 1.0f;
		output[begin + 1] = vertex.y + idy * 1.0f;
		output[begin + 2] = vertex.z + idz * 1.0f;
		output[begin + 3] = color_field[idx + idy * (NX - 1) +idz * (NX - 1) * (NY - 1)];
		begin += 4;
	}
	if (idx == NX - 2 && idy == NY - 2 && idz == NZ - 2) {
		(*triangle_count) = offset[index] / 4 + begin / 4;
	}
	//debug[index] = begin;
}

__global__ void compute_trianglesINTERP(__half* field, int* offset, int* count, uint8_t* id, vec3** in_vertices, int** lookup, float* output, int* triangle_count) {
	uint idx = threadIdx.x + blockDim.x * blockIdx.x;
	uint idy = threadIdx.y + blockDim.y * blockIdx.y;
	uint idz = threadIdx.z + blockDim.z * blockIdx.z;
	uint index = idx + idy * (NX - 1) + idz * (NX - 1) * (NY - 1);
	__shared__ vec3 sides[12][2];
	if (threadIdx.x == 0 && threadIdx.y == 0 && threadIdx.z == 0) {
		for (int i = 0; i < 12; i++) {
			sides[i][0] = in_vertices[i][0];
			sides[i][1] = in_vertices[i][1];
		}
	}
	__syncthreads();
	if (idx == NX - 1 || idy == NY - 1 || idz == NZ - 1)return;
	uint8_t hashcode = id[index];
	int begin = offset[index];
	for (int j = 0; j < 16; j++) {
		if (lookup[hashcode][j] == -1)break;
		int side = lookup[hashcode][j];
		vec3 start = sides[side][0];
		vec3 end = sides[side][1];
		int start_index = idx + start.x + (idy + start.y) * NX + (idz + start.z) * NX * NY;
		int end_index = idx + end.x + (idy + end.y) * NX + (idz + end.z) * NX * NY;

		vec3 dir = end - start;

		float tvalue = __half2float(field[end_index]) - __half2float(field[start_index]);
		vec3 final_pos = start;
		if (tvalue > 10e-5) {
			tvalue = (thr - __half2float(field[start_index])) / (tvalue);

			vec3 final_pos = dir * tvalue + start;
		}
		
		output[begin] = final_pos.x + idx * 1.0f;
		output[begin + 1] = final_pos.y + idy * 1.0f;
		output[begin + 2] = final_pos.z + idz * 1.0f;
		output[begin + 3] =  sc_field[idx + idy * NX + idz * NX * NY];
		begin += 4;
	}
	if (idx == NX - 2 && idy == NY - 2 && idz == NZ - 2) {
		(*triangle_count) = offset[index] / 4 + begin / 4;
	}
	//debug[index] = begin;
}

namespace mcr {

	void begin() {
		int* triangle_count = (int*)malloc(sizeof(int) * 256);
		cudaMalloc(&n_triangles, sizeof(int) * 256);
		cudaMalloc(&indices, sizeof(int*) * 256);
		cudaMalloc(&vertices, sizeof(vec3) * 12);
		cudaMalloc(&codes, sizeof(uint8_t) * NX * NY * NZ);
		cudaMallocManaged(&color_field, sizeof(__half) * (NX - 1) * (NY - 1) * (NZ - 1));
		cudaMallocManaged(&ntriangle_count, sizeof(int));
		cudaMallocManaged(&sdDEV, sizeof(vec3*) * 12);
		for (int i = 0; i < 256; i++) {
			int cnt = 1;
			for (int j = 0; j < 16; j++) {
				if (TriangleTable[i][j] == -1) break;
				cnt++;
			}
			cudaMalloc(&host_indices[i], sizeof(int) * cnt);
			cudaMemcpy(host_indices[i], TriangleTable[i], sizeof(int) * cnt, cudaMemcpyHostToDevice);
			triangle_count[i] = cnt;
		}
		for (int j = 0; j < 12; j++) {
			cudaMallocManaged(&sd[j], sizeof(vec3) * 2);
			cudaMemcpy(sd[j], lookupSides[j], sizeof(vec3) * 2, cudaMemcpyDefault);
		}
		cudaMemcpy(sdDEV, sd, sizeof(vec3*) * 12, cudaMemcpyDefault);
		cudaMemcpy(n_triangles, triangle_count, sizeof(int) * 256, cudaMemcpyHostToDevice);
		cudaMemcpy(indices, host_indices, sizeof(int*) * 256, cudaMemcpyHostToDevice);
		cudaMemcpy(vertices, verticesHost, sizeof(vec3) * 12, cudaMemcpyHostToDevice);
		free(triangle_count);
		offsets = thrust::device_vector<int>(NX * NY * NZ);
		for (int i = 0; i < 12; i++) {
			std::cout << sdDEV[i][0].x << lookupSides[i][0].x << "\n";
		}
	}

	void marchCubes(__half* field, float* dptr) {
		march_begin << <block, thread >> > (field, color_field, n_triangles, thrust::raw_pointer_cast(offsets.data()), codes);
		cudaDeviceSynchronize();
		thrust::plus<int> binary_op;
		thrust::exclusive_scan(offsets.begin(), offsets.end(), offsets.begin(), 0, binary_op);		
		compute_trianglesINTERP << <block, thread >> > (color_field, thrust::raw_pointer_cast(offsets.data()), n_triangles, codes, sdDEV, indices, dptr, ntriangle_count);
		cudaDeviceSynchronize();		
	}
	void __terminate__() {
		cudaFree(n_triangles);
		cudaFree(indices);
		cudaFree(vertices);
		for (int i = 0; i < 256; i++) {
			cudaFree(host_indices[i]);
		}
	}
}

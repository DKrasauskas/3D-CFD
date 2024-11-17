#pragma once
#include "lbm_settings.h"
#include "mesh.h"

namespace lbm {
    
    mesh generate_normalized_mesh(int split) {
        float d = 1 / (float)split;
        int index = 0;
        cell* source = (cell*)malloc(sizeof(cell) * split * split * split);
        for (int i = 0; i < split; i++) {
            for (int j = 0; j < split; j++) {
                for (int k = 0; k < split; k++) {
                    float x = -0.5 + d * i;
                    float y = -0.5 + d * j;
                    float z = -0.5 + d * k;
                    index = i + j * split + k * split * split;
                    source[index].x = x;
                    source[index].y = y;
                    source[index].z = z;
                    source[index].rho = .00001;
                    source[index].v.x = 0;
                    source[index].v.y = 0;
                    source[index].v.z = 0;
                }
            }
        }
        return { source };
    }

    //creates a sphere mask
    float* mask(float R, int split) {
        float* mask = (float*)malloc(sizeof(float) * split * split * split);
        for (int i = 0; i < split; i++) {
            for (int j = 0; j < split; j++) {
                for (int k = 0; k < split; k++) {
                    float r = (i - split / 2 + split / 4) * (i - split / 4) + (j - split / 2) * (j - split / 2) + (k - split / 2) * (k - split / 2);
                    if (r < R) {
                        mask[i + j * NX + k * NX * NY] = 1;
                    }
                    else {
                        mask[i + j * NX + k * NX * NY] = 0;
                    }
                }
            }
        }
        return mask;
    }
    //creates a mask based on the mesh extracted
    bool* mask(float* vertices, int v_size) {
        bool* mask = (bool*)calloc(NX * NY * NZ, sizeof(bool));
        int index = 3;
        for (int i = 1; i < v_size; i++) {
            int idx = (int)((vertices[index] * 1.0f + 0.5f) * 256);
            int idy = (int)((vertices[index + 1] * 1.0f + 0.5f) * NY);
            int idz = (int)((vertices[index + 2] * 1.0f + 0.5f) * NZ);
            index += 3;
            if (idx < 0 || idy < 0 || idz < 0 || idx > NX - 1 || idy > NY - 1 || idz > NZ - 1) continue;
            mask[idx + idy * NX + idz * NX * NY] = 1;
        }
        return mask;
    }
    cell* __init__() {
        cudaMalloc(&wDEV, sizeof(float) * 19);
        cudaMalloc(&eDEV, sizeof(vec3) * 19);
        cudaMallocManaged(&HMP, sizeof(float) * 1);
        cudaError_t er = cudaMalloc(&v, sizeof(vec3) * NX * NY * NZ);
        cudaGetErrorName(er);
        std::cout << cudaMalloc(&m_obj, sizeof(bool) * NX * NY * NZ) << "\n";
        mesh ms = generate_normalized_mesh(1);
        vec3* eHOST = (vec3*)malloc(sizeof(vec3) * 19);
        float* wHOST = (float*)malloc(sizeof(float) * 19);
        for (int i = 0; i < 19; i++) {
            eHOST[i].x = e[i].x;
            eHOST[i].y = e[i].y;
            eHOST[i].z = e[i].z;
            wHOST[i] = w[i];
            cudaMalloc(&hostptr[i], sizeof(__half) * NX * NY * NZ);
            cudaMalloc(&hostbuffer[i], sizeof(__half) * NX * NY * NZ);
        }
        cudaMalloc(&devptr, sizeof(__half*) * 19);
        cudaMalloc(&devbuffer, sizeof(__half*) * 19);
        cudaMemcpy(devptr, hostptr, sizeof(__half*) * 19, cudaMemcpyHostToDevice);
        cudaMemcpy(devbuffer, hostbuffer, sizeof(__half*) * 19, cudaMemcpyHostToDevice);
        cudaMemcpy(eDEV, eHOST, sizeof(vec3) * 19, cudaMemcpyHostToDevice);         
        cudaMemcpy(wDEV, wHOST, sizeof(float) * 19, cudaMemcpyHostToDevice);
        return ms.cells;
        
    }
    void begin(float* verticesV, int sizeV) {
        __init__();
        bool* m = mask(verticesV, sizeV);
        cudaMemcpy(m_obj, m, sizeof(bool) * NX*NY*NZ, cudaMemcpyHostToDevice);
        free(m);
        eq << <block, thread >> > (devptr, eDEV, wDEV);
        cudaDeviceSynchronize();
        compute();
    }
    void __terminate__() {
        cudaFree(wDEV);
        cudaFree(eDEV);
        cudaFree(output);
        cudaFree(v);
    }
}

#pragma once

struct vec3H {
    __half x, y, z;
};
__global__ void eq(__half** fdev, vec3* e, float* w) {
    uint idx = threadIdx.x + blockDim.x * blockIdx.x;
    uint idy = threadIdx.y + blockDim.y * blockIdx.y;
    uint idz = threadIdx.z + blockDim.z * blockIdx.z;
    uint index = idx + idy * NX + idz * NX * NY;
    float vx = max_vx;// current.v.x;
    float vy = max_vy;
    float vz = 0;
    if (idx == NX-1) {
        float vx = max_vx;// current.v.x;
        float vy = max_vy;
        float vz = 0;
    }
    float udotu = vx * vx + vy * vy + vz * vz;
    float fe;
    for (int i = 0; i < 19; i++) {
        float cdotu = vx * e[i].x + vy * e[i].y + vz * e[i].z;
        fe = 1.0 * w[i] * (1.0f + 3.0f * cdotu + 4.5f * cdotu * cdotu  - 1.5f * udotu );
        //immediatelly correct the distribution functions w/o launching another kernel (collide):
        fdev[i][index] = __float2half(fe);
    }
}
__global__ void compute_macros(__half** cells, __half** buffer, vec3* e_global, float* w_global, bool* m, vec3* v) {
    uint idx = threadIdx.x + blockDim.x * blockIdx.x;
    uint idy = threadIdx.y + blockDim.y * blockIdx.y;
    uint idz = threadIdx.z + blockDim.z * blockIdx.z;
    uint index = idx + idy * NX + idz * NX * NY;
    __shared__ vec3 e[19];
    __shared__ float w[19];
    __half f[19];
    if (threadIdx.x == 1 && threadIdx.y == 1 && threadIdx.z == 1) {
        for (int i = 0; i < 19; i++) {
            e[i] = e_global[i];
            w[i] = w_global[i];
            f[i] = cells[i][index];
        }
    }
    else {
        for (int i = 0; i < 19; i++) {
            f[i] = cells[i][index];
        }
    }
    SYNC();
    //compute macros:
    __half rho = 0;
    __half vx = 0;
    __half vy = 0;
    __half vz = 0;
    #pragma unroll
    for (int i = 0; i < 19; i++) {
        __half fi = f[i];
        rho += fi;
        vx += __hmul(fi, __float2half(e[i].x));
        vy += __hmul(fi, __float2half(e[i].y));
        vz += __hmul(fi, __float2half(e[i].z));
    }
    //to avoid potential division by 0
    __half inverse = rho != (__half)0 ? (__half)1 / rho : 0; 
    vx *= inverse;
    vy *= inverse;
    vz *= inverse;
    //write to the global v array
    v[index].x = __half2float(vx);
    v[index].y = __half2float(vy);
    v[index].z = __half2float(vz);

    float udotu = __half2float(vx * vx + vy * vy + vz * vz);
    for (int i = 0; i < 19; i++) {
        float cdotu = __half2float(vx) * e[i].x + __half2float(vy) * e[i].y + __half2float(vz) * e[i].z;
        float fe = __half2float(rho) * w[i] * (1.0f + 3.0f * cdotu + 4.5f * cdotu * cdotu - 1.5f * udotu);
        //immediatelly correct the distribution functions w/o launching another kernel (collide):
        f[i] -=  __float2half(omega * (-fe + __half2float(f[i]))); //write the updated values efficiently 
    }
    if (m[index]){
        __half temp = f[1];
        f[1] = f[2];
        f[2] = temp;
        temp = f[3];
        f[3]= f[4];
        f[4] = temp;
        temp = f[5];
        f[5] = f[6];
        f[6] = temp;
        temp = f[7];
        f[7] = f[10];
        f[10] = temp;
        temp = f[8];
        f[9] = f[9];
        f[8] = temp;
        temp = f[11];
        f[11] = f[14];
        f[14] = temp;
        temp = f[12];
        f[12] = f[13];
        f[13] = temp;
        temp = f[15];
        f[15] = f[18];
        f[18] = temp;
        temp = f[16];
        f[16] = f[17];
        f[17] = temp;      
        udotu = -100000000;
    }
   // color_buffer[index] = max_v * max_v - udotu ;
    for (int i = 0; i < 19; i++) {
        uint write_x = idx + e[i].x;
        uint write_y = idy + e[i].y;
        uint write_z = idz + e[i].z;
        //handle boundary
        if (write_x == -1 || write_y == -1 || write_z == -1 || write_x == NX || write_y == NY || write_z == NZ) continue;
        buffer[i][write_x + write_y * NX + write_z * NX * NY] = f[i];
    }
}

__global__ void walls(__half** fdev, vec3* e, float* w) {
    uint idx = threadIdx.x + blockDim.x * blockIdx.x;
    uint idy = threadIdx.y + blockDim.y * blockIdx.y;
    uint idz = threadIdx.z + blockDim.z * blockIdx.z;
    uint index = idx + idy * NX + idz * NX * NY;
    if (idx == 0 || idy == 0 || idz == 0 || idx == NX - 1 || idy == NY - 1 || idz == NZ - 1) {
        float vx = max_vx;// current.v.x;
        float vy = max_vy;
        float vz = 0;
        float rho = 1.0;
        float udotu = vx * vx + vy * vy + vz * vz;
        float fe;
        for (int i = 0; i < 19; i++) {
            float cdotu = vx * e[i].x + vy * e[i].y + vz * e[i].z;
            fe = rho * w[i]*(1.0f + 3.0f * cdotu + 4.5f * cdotu * cdotu - 1.5f * udotu);
            //immediatelly correct the distribution functions w/o launching another kernel (collide):
            fdev[i][index] = __float2half(fe);
        }
    }
}

__global__ void gradient(vec3* buffer, __half* output, float* scale) {
    uint idx = threadIdx.x + blockDim.x * blockIdx.x + 1;
    uint idy = threadIdx.y + blockDim.y * blockIdx.y + 1;
    uint idz = threadIdx.z + blockDim.z * blockIdx.z + 1;
    if (idx >= NX - 1 || idy >= NY - 1 || idz >= NZ - 1)return;
    float dx1 = buffer[idx + 1 + idy * NX + idz * NX * NY].y - buffer[idx - 1 + idy * NX + idz * NX * NY].y; //dvy/dx
    float dy1 = buffer[idx + (idy + 1) * NX + idz * NX * NY].z - buffer[idx + (idy - 1) * NX + idz * NX * NY].z;
    float dz1 = buffer[idx + (idy)*NX + (idz + 1) * NX * NY].x - buffer[idx + (idy)*NX + (idz - 1) * NX * NY].x;
    float dx2 = buffer[idx + 1 + idy * NX + idz * NX * NY].z - buffer[idx - 1 + idy * NX + idz * NX * NY].z; //dvy/dx
    float dy2 = buffer[idx + (idy + 1) * NX + idz * NX * NY].x - buffer[idx + (idy - 1) * NX + idz * NX * NY].x;
    float dz2 = buffer[idx + (idy)*NX + (idz + 1) * NX * NY].y - buffer[idx + (idy)*NX + (idz - 1) * NX * NY].y;
    //float vx = buffer[idx + idy * NX + idz * NX * NY].x;
    //float vy = buffer[idx + idy * NX + idz * NX * NY].y;
    //float vz = buffer[idx + idy * NX + idz * NX * NY].z;
    float len = (dy1 - dz2) * (dy1 - dz2) + (dz1 - dx2) * (dz1 - dx2) + (dx1 - dy2) * (dx1 - dy2);
    output[idx + idy * NX + idz * NX * NY] =  __float2half(((*scale))* len);
}


void compute() {
    compute_macros<<<block, thread>>>(devptr, devbuffer,  eDEV, wDEV, m_obj, v);
    cudaDeviceSynchronize();
    __half** aux = devptr;
    devptr = devbuffer;
    devbuffer = aux;
    walls <<<block, thread >>> (devptr, eDEV, wDEV);
    cudaDeviceSynchronize();
    gradient <<<block, thread >>> (v, sc_field, HMP);
    cudaDeviceSynchronize();
}



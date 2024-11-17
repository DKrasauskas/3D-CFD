#pragma once

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1200;

const char* path = "inputGeometry/airbus.txt";
struct vec2 {
    float x, y;
};

struct vec3 {
    float x, y, z;
    __host__ __device__
        vec3 operator -(vec3 lhs) {
        return { x - lhs.x, y - lhs.y, z - lhs.z };
    }
    __host__ __device__
        vec3 operator +(vec3 lhs) {
        return { x + lhs.x, y + lhs.y, z + lhs.z };
    }
    __host__ __device__
        vec3 operator *(float lhs) {
        return { x * lhs, y * lhs, z * lhs };
    }
};

struct cell {
    float x, y, z;
    float rho;
    vec3 v;
};

struct mesh {
    cell* cells;
};
/*___________________________Program properties____________________________*/

//launch macros
#define uint unsigned int
#define SYNC() __syncthreads()
#define COMPUTATION_MODE 1 // set 1 to write dv data to file

//launch settings
#define thread_count 256
dim3 block(80, 64, 64);
dim3 thread(8, 4, 4);

// LBM parameters
#define NX 640
#define NY 256
#define NZ 256
#define omega 1.96f
#define max_vx -0.06
#define max_vy 0.00

//LBM Globals
__device__ vec3* eDEV, * vels;
__device__ float* wDEV;
__managed__ bool* m_obj;
float* output;
__managed__ __half* hostptr[19], * hostbuffer[19]; // store ptr to the begining of the array
__device__ __half** devptr, ** devbuffer;
__device__ vec3* v;

//marching cubes globals
float* dptr;
__device__ int* n_triangles;
__device__ uint8_t *codes;
thrust::device_vector<int> offsets;
__device__ int** indices;
__device__ vec3* vertices;
__managed__ __half* sc_field, * color_field;
__managed__ int* host_indices[256];
__managed__ int* ntriangle_count;
__managed__ float* HMP;
__managed__ vec3* sd[12];
__managed__ vec3** sdDEV;

//OpenGL globals
GLFWwindow* window;
Camera camera;
glm::mat4 projection = glm::mat4(1);
glm::mat4 view = glm::mat4(1);
glm::mat4 model = glm::mat4(1);
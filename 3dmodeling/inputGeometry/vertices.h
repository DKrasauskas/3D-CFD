#pragma once
struct VertexList {
    float* vertices;
    int size;
    points point;
};

VertexList read_verticesV(const char* source) {
    std::ifstream in(source);
    int size = 0;
    in >> size;
    std::cout << size << "\n";
    float* output = (float*)malloc(sizeof(float) * size * 3);
    points pt;
    int u = 0;
    thrust::host_vector<com> pos(size);
    thrust::host_vector<Key> keys(size);
    float max = -0xFFFFFF;
    float min = 0xFFFFFF;
    for (int i = 0; i < size * 3; i++) {
        float a;
        in >> a;
        output[i] = (a * 0.01f + .5f) * 16;
        if (output[i] > max) max = output[i];
        if (output[i] < min) min = output[i];
    }
    std::cout << "max" << max << "min" << min << "\n";
    for (int i = 0; i < size * 3; i += 3) {
        pos[i / 3] = { output[i], output[i + 1], output[i + 2] };
        keys[i / 3] = { 0, 0 };
    }
    pt.data = pos;
    pt.keys = keys;
    return { output, size, pt };
}

VertexList VAO_rng(points pt) {
    float* vao = (float*)malloc(sizeof(float) * 3 * (pt.data.end() - pt.data.begin()));
    int index = 0;
    for (auto it = pt.data.begin(); it != pt.data.end(); it++) {
        com a = (*it);
        vao[index] = a.x;
        vao[index + 1] = a.y;
        vao[index + 2] = a.z;
        index += 3;
    }
    return { vao, (int)(3 * (pt.data.end() - pt.data.begin())), pt };
}

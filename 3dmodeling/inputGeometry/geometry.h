#pragma once

struct VertexList {
    float* vertices;
    int size;
};

VertexList read_verticesV(const char* source) {
    std::ifstream in(source);
    int size = 0;
    in >> size;
    std::cout << size << "\n";
    float* output = (float*)calloc(size * 3, sizeof(float));
    int u = 0;
    float max = -0xFFFFFF;
    float min = 0xFFFFFF;
    float scale = 4.5f;
    for (int i = 0; i < size * 3; i += 3) {
        float a;
       /* in >> a;
        output[i + 2] = scale *(-a * 0.01f - 0.14f);
        in >> a;
        output[i + 1] = scale *(a * 0.01f - 0.05f);
        in >> a;
        output[i] = scale * (a * 0.01f + 0.14f);*/
        in >> a;
        output[i + 2] = scale * (-a * 0.01f);
        in >> a;
        output[i] = scale * (-a * 0.01f + .05);
        in >> a;
        output[i + 1] = scale * (a * 0.01f - 0.05);
        if (output[i] > max) max = output[i];
        if (output[i] < min) min = output[i];
    }
    std::cout << "max" << max << "min" << min << "\n";
    return { output, size};
}
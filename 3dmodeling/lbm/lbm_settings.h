#pragma once
//lookup tables
vec3 e[19] = {
    { 0,  0, 0},
    { 1,  0, 0},
    {-1,  0, 0},
    { 0,  1, 0},
    { 0, -1, 0},
    { 0,  0, 1},
    { 0,  0,-1},
    { 1,  1, 0},
    {-1,  1, 0},
    { 1, -1, 0},
    {-1, -1, 0},
    { 1,  0, 1},
    {-1,  0, 1},
    { 1,  0,-1},
    {-1,  0,-1},
    { 0,  1, 1},
    { 0, -1, 1},
    { 0,  1,-1},
    { 0, -1,-1},
};
float w[19] = {
    (float)1 / (float)3,
    (float)1 / (float)18,
    (float)1 / (float)18,
    (float)1 / (float)18,
    (float)1 / (float)18,
    (float)1 / (float)18,
    (float)1 / (float)18,
    (float)1 / (float)36,
    (float)1 / (float)36,
    (float)1 / (float)36,
    (float)1 / (float)36,
    (float)1 / (float)36,
    (float)1 / (float)36,
    (float)1 / (float)36,
    (float)1 / (float)36,
    (float)1 / (float)36,
    (float)1 / (float)36,
    (float)1 / (float)36,
    (float)1 / (float)36
};
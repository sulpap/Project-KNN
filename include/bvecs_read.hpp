#ifndef BVECS_READ_HPP
#define BVECS_READ_HPP

#include <vector>
#include <iostream>
#include <cstdint>

using namespace std;

vector<vector<uint8_t>> bvecs_read(const char *filename, size_t a = 1, size_t b = 0);

#endif
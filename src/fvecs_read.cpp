#include "../include/fvecs_read.hpp"
#include <iostream>
#include <fstream>
#include <cassert>

vector<vector<float>> fvecs_read(const char* filename, size_t a, size_t b) 
{
    // open the file and count the number of descriptors
    FILE* fid = fopen(filename, "rb");
    if (!fid) {
        cerr << "I/O error: Unable to open the file " << filename << endl;
        exit(EXIT_FAILURE);
    }

    // Read dimension of vectors (first integer in the file)
    int d;
    fread(&d, sizeof(int), 1, fid);
    size_t vecsizeof = sizeof(int) + d * sizeof(float);

    // Get the number of vectors by checking the file size
    fseek(fid, 0, SEEK_END);  // go to end of file
    size_t file_size = ftell(fid);
    size_t bmax = file_size / vecsizeof;

    if (b == 0) {
        b = bmax;  // if we are not given an upper bound (b), read until the end of file
    }

    assert(a >= 1 && "Starting index must be >= 1");
    if (b > bmax) {
        b = bmax;  // we can't read more than the available vectors
    }
    if (b < a) {
        fclose(fid);
        return {};  // return empty vector if the range is invalid
    }

    // compute the number of vectors to read
    size_t n = b - a + 1;

    // go to the position of the first vector to read
    fseek(fid, (a - 1) * vecsizeof, SEEK_SET);

    // read n vectors into a buffer (each vector includes its dimension as the first element)
    vector<float> buffer((d + 1) * n);
    fread(buffer.data(), sizeof(float), (d + 1) * n, fid);

    // reshape the buffer into a 2D vector - ignore thr first column = dimension of the vectors
    vector<vector<float>> vectors(n, vector<float>(d));
    for (size_t i = 0; i < n; ++i) {
        for (auto j = 0; j < d; j++) {
            vectors[i][j] = buffer[i * (d + 1) + j + 1];  // first element of each vector --> ignored
        }
    }

    fclose(fid);
    return vectors;
}
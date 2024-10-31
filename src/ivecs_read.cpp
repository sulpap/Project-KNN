#include "../include/ivecs_read.hpp"
#include <iostream>
#include <fstream>
#include <cassert>

vector<vector<int>> ivecs_read(const char* filename, size_t a, size_t b) 
{
    // Open the file and count the number of descriptors
    FILE* fid = fopen(filename, "rb");
    if (!fid) {
        cerr << "I/O error: Unable to open the file " << filename << endl;
        throw runtime_error("File open error");
    }

    // Read the vector size (first integer in the file)
    int d;
    if (fread(&d, sizeof(int), 1, fid) != 1) {
        cerr << "Error reading vector dimension" << endl;
        fclose(fid);
        throw runtime_error("File read error");
    }
    size_t vecsizeof = sizeof(int) * (d + 1);  // size of each vector in bytes

    // Determine the total number of vectors by checking file size
    fseek(fid, 0, SEEK_END);
    size_t file_size = ftell(fid);
    size_t bmax = file_size / vecsizeof;

    // Adjust bounds if needed
    if (b == 0) {
        b = bmax;  // If no upper bound is provided, read to end of file
    }

    assert(a >= 1 && "Starting index must be >= 1");
    if (b > bmax) {
        b = bmax;  // Limit to available vectors
    }
    if (b < a) {
        fclose(fid);
        return {};  // Return empty vector if range is invalid
    }

    // Calculate the number of vectors to read
    size_t n = b - a + 1;

    // Move to the position of the first vector to read
    fseek(fid, (a - 1) * vecsizeof, SEEK_SET);

    // Read n vectors into a buffer (each vector includes its dimension as the first element)
    vector<int> buffer((d + 1) * n);
    if (fread(buffer.data(), sizeof(int), (d + 1) * n, fid) != (d + 1) * n) {
        cerr << "Error reading vector data" << endl;
        fclose(fid);
        throw runtime_error("File read error");
    }

    // Reshape the buffer into a 2D vector, ignoring the first element (dimension) of each vector
    vector<vector<int>> vectors(n, vector<int>(d));
    for (size_t i = 0; i < n; ++i) {
        for (auto j = 0; j < d; ++j) {
            vectors[i][j] = buffer[i * (d + 1) + j + 1];  // Skip the first element (dimension)
        }
    }

    fclose(fid);
    return vectors;
}

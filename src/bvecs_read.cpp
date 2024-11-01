#include "../include/bvecs_read.hpp"
#include <fstream>
#include <cassert>

vector<vector<uint8_t>> bvecs_read(const char* filename, size_t a, size_t b) 
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
    size_t vecsizeof = sizeof(int) + d * sizeof(uint8_t);

    // Determine the total number of vectors by checking the file size
    fseek(fid, 0, SEEK_END);  // Go to end of file
    size_t file_size = ftell(fid);
    size_t bmax = file_size / vecsizeof;

    // Adjust bounds if needed
    if (b == 0) {
        b = bmax;  // If no upper bound is provided, read to the end of file
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
    vector<uint8_t> buffer((d + 4) * n);
    if (fread(buffer.data(), sizeof(uint8_t), (d + 4) * n, fid) != (d + 4) * n) {
        cerr << "Error reading vector data" << endl;
        fclose(fid);
        throw runtime_error("File read error");
    }

    // Reshape the buffer into a 2D vector, ignoring the first 4 bytes (dimension) of each vector
    vector<vector<uint8_t>> vectors(n, vector<uint8_t>(d));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < static_cast<size_t>(d); ++j) {
            vectors[i][j] = buffer[i * (d + 4) + j + 4];  // Skip the first 4 bytes (dimension)
        }
    }

    fclose(fid);
    return vectors;
}

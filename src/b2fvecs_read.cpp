#include "../include/b2fvecs_read.hpp"
#include <iostream>
#include <fstream>
#include <cassert>

using namespace std;

vector<vector<float>> b2fvecs_read(const char* filename, size_t a, size_t b) 
{
    // Open the file and check for errors
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
        throw runtime_error("File read error1");
    }
    size_t vecsizeof = sizeof(int) + d * sizeof(float);

    // Determine the total number of vectors by checking file size
    fseek(fid, 0, SEEK_END);  // Go to end of file
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

    vector<vector<float>> vectors(n, vector<float>(d)); // Directly create the 2D vector
    for(size_t i = 0; i < n; ++i) {
        int dim_read;
        fread(&dim_read, sizeof(int), 1, fid);
        if (dim_read != d) {
            cerr << "Dimension mismatch!" << endl;
            fclose(fid);
            throw runtime_error("Dimension error");
        }
        fread(vectors[i].data(), sizeof(float), d, fid);
    }

    fclose(fid);
    return vectors;
}

#include "../include/b2fvecs_read.hpp"
#include <iostream>
#include <fstream>
#include <cassert>

using namespace std;

vector<vector<float>> b2fvecs_read(const char *filename, size_t a, size_t b)
{
    FILE *fid = fopen(filename, "rb");
    if (!fid)
    {
        cerr << "I/O error: Unable to open the file " << filename << endl;
        throw runtime_error("File open error");
    }

    int d;
    if (fread(&d, sizeof(int), 1, fid) != 1)
    {
        cerr << "Error reading vector dimension" << endl;
        fclose(fid);
        throw runtime_error("File read error1");
    }
    size_t vecsizeof = sizeof(int) + d * sizeof(float);

    fseek(fid, 0, SEEK_END);
    size_t file_size = ftell(fid);
    size_t bmax = file_size / vecsizeof;

    if (b == 0)
    {
        b = bmax;
    }

    assert(a >= 1 && "Starting index must be >= 1");
    if (b > bmax)
    {
        b = bmax;
    }
    if (b < a)
    {
        fclose(fid);
        return {};
    }

    size_t n = b - a + 1;
    fseek(fid, static_cast<long>((a - 1) * vecsizeof), SEEK_SET);

    vector<vector<float>> vectors(n, vector<float>(d));
    for (size_t i = 0; i < n; ++i)
    {
        int dim_read = 0; // Initialize dim_read to avoid warning
        if (fread(&dim_read, sizeof(int), 1, fid) != 1)
        {
            cerr << "Error reading vector dimension" << endl;
            fclose(fid);
            throw runtime_error("File read error2");
        }
        if (dim_read != d)
        {
            cerr << "Dimension mismatch!" << endl;
            fclose(fid);
            throw runtime_error("Dimension error");
        }
        if (fread(vectors[i].data(), sizeof(float), static_cast<size_t>(d), fid) != static_cast<size_t>(d))
        {
            cerr << "Error reading data" << endl;
            fclose(fid);
            throw runtime_error("File read error3");
        }
    }

    fclose(fid);
    return vectors;
}

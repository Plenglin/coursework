#pragma once

#include <iostream>

#include "sync.hpp"

/**
 * Outputs the rows assigned to this process, start inclusive and end exclusive.
 */
template <int rows>
void get_row_range(int &start, int &end) {
    static int _start = -1;
    static int _end;

    if (_start == -1) {
        float slices_size = (float)rows / __sync_n;
        _start = slices_size * __sync_i;
        _end = slices_size * (__sync_i + 1);
        if (_end > rows) {
            _end = rows;
        }
    }

    start = _start;
    end = _end;
}

template <int mrows, int mcols>
class MatrixPatch {
public:
    float *nums[mcols];
    int rows;
    int row_offset;

    MatrixPatch(int row_offset, int rows) : row_offset(row_offset), rows(rows) {
        nums = new float[rows][mcols];
    }
    ~MatrixPatch() {
        delete nums;
    }
};

template <int rows, int cols>
class Matrix {
public:
    float nums[rows][cols];

    Matrix() {}

    template <int ocols>
    Matrix<rows, ocols> operator *(const Matrix<cols, ocols> &other) {
        int rows_start, rows_end;
        get_row_range<rows>(rows_start, rows_end);

        // The following technique ensures that you can use *= when
        // multiple procs are performing this one operation without 
        // overwriting each others' results.

        Matrix<rows, ocols> out;
        for (int i = rows_start; i < rows_end; i++) {
            // Write into a temporary array
            float row_result[cols];
            for (int j = 0; j < ocols; j++) {
                float sum = 0;
                for (int k = 0; k < cols; k++) {
                    sum += nums[i][k] * other.nums[k][j];
                }
                out.nums[i][j] = sum;
            }
        }
        return out;
    }

    Matrix<rows, cols>& operator =(Matrix<rows, cols> src) {
        int start, end;
        get_row_range<rows>(start, end);
        
        for (int i = start; i < end; i++) {
            for (int j = 0; j < cols; j++) {
                nums[i][j] = src.nums[i][j];
            }
        }
    }

    /**
     * Returns a Python expression that, assuming that `import numpy as np`
     * has been called, creates an exact copy of this matrix, but in Python.
     */
    void print_numpy(std::ostream &os) {
        os << "np.array([";
        for (int i = 0; i < rows; i++) {
            os << "[";
            for (int j = 0; j < cols; j++) {
                os << nums[i][j];
                if (j != cols - 1) os << ",";
            }
            os << "]";
            if (i != rows - 1) os << ",";
        }
        os << "], np.float32)";
    }

    void print_pretty(std::ostream &os) {
        for (int j = 0; j < cols; j++) {
            for (int i = 0; i < rows; i++) {
                os << nums[i][j] << "\n";
            }
        }        
    }

    void fill(float x) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                nums[i][j] = x;
            }
        }
    }
};

// Multi-process matrix randomization
template <int rows, int cols>
void mp_randomize(Matrix<rows, cols> &m) {
    int start, end;
    get_row_range<rows>(start, end);

    for (int i = start; i < end; i++) {
        for (int j = 0; j < cols; j++) {
            m.nums[i][j] = (float)(rand() % 20);
        }
    }
}

template <int rows, int cols>
std::ostream& operator <<(std::ostream& os, Matrix<rows, cols>& m) {
    #ifdef MATRIX_PRINT_NUMPY
        m.print_numpy(os);
    #else
        m.print_pretty(os);
    #endif
    return os;
}
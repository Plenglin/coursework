#include <iostream>

#include <stdlib.h>
#include <time.h>

int __sync_i, __sync_n;
void mp_initialize(int i, int n) {
    __sync_i = i;
    __sync_n = n;
}

void mp_seed_random() {
    srand(time(NULL) * (__sync_n + __sync_i) / __sync_n);
}

template <int rows>
void get_row_range(int &start, int &end) {
    int slices_size = rows / __sync_n;
    start = slices_size * __sync_i;
    end = slices_size * (__sync_i + 1);
    if (end > rows) {
        end = rows;
    }
}

template <int rows, int cols>
class Matrix {
public:
    float nums[rows][cols];

    Matrix() {}

    template <int ocols>
    Matrix<rows, ocols> operator *(const Matrix<cols, ocols> &other) {

        // Calculate which row we're doing
        int rows_start, rows_end;
        get_row_range<rows>(rows_start, rows_end);

        Matrix<rows, ocols> out;
        for (int i = rows_start; i < rows_end; i++) {
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

    /**
     * Call it a semi-manual unit test.
     */
    void print_wolfram_alpha(std::ostream &os) {
        os << "[";
        for (int i = 0; i < rows; i++) {
            os << "[";
            for (int j = 0; j < cols; j++) {
                os << nums[i][j];
                if (j != cols - 1) os << ", ";
            }
            os << "]";
            if (i != rows - 1) os << ", ";
        }
        os << "]";
    }

    void print_pretty(std::ostream &os) {
        for (int i = 0; i < rows; i++) {
            bool is_first = i == 0;
            bool is_last = i == rows - 1;

            os << (is_first ? "┌" : is_last ? "└" : "|");
            for (int j = 0; j < cols; j++) {
                os << nums[i][j] << "\t";
            }

            os << (is_first ? "┐" : is_last ? "┘" : "|"); 
            if (!is_last) {
                os << std::endl;
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

template <int rows, int cols>
void mp_randomize(Matrix<rows, cols> &m) {
    int start, end;
    get_row_range<rows>(start, end);

    for (int i = start; i < end; i++) {
        for (int j = 0; j < cols; j++) {
            m.nums[i][j] = (float)(rand() % 3);
        }
    }
}

template <int rows, int cols>
std::ostream& operator <<(std::ostream& os, Matrix<rows, cols>& m) {
    #ifdef MATRIX_PRINT_WOLFRAM_ALPHA
        m.print_wolfram_alpha(os);
    #else
        m.print_pretty(os);
    #endif
    return os;
}
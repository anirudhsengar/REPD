// --- Matrix.cpp ---
// A simple, robust, and generic 2D Matrix class.
// It demonstrates proper resource management, const-correctness,
// operator overloading, and bounds checking.

#include <vector>
#include <stdexcept>
#include <iostream>

template <typename T>
class Matrix {
public:
    // Constructor initializes a matrix of given dimensions with a default value.
    Matrix(size_t rows, size_t cols, const T& initialValue = T{})
        : m_rows(rows), m_cols(cols), m_data(rows * cols, initialValue) {
        if (rows == 0 || cols == 0) {
            throw std::invalid_argument("Matrix dimensions must be non-zero.");
        }
    }

    // Const-correct access operator for reading elements.
    // Throws std::out_of_range if indices are invalid.
    const T& operator()(size_t row, size_t col) const {
        if (row >= m_rows || col >= m_cols) {
            throw std::out_of_range("Matrix access out of range.");
        }
        return m_data[row * m_cols + col];
    }

    // Non-const access operator for writing elements.
    // Throws std::out_of_range if indices are invalid.
    T& operator()(size_t row, size_t col) {
        if (row >= m_rows || col >= m_cols) {
            throw std::out_of_range("Matrix access out of range.");
        }
        return m_data[row * m_cols + col];
    }

    // Accessors for dimensions.
    size_t getRows() const { return m_rows; }
    size_t getCols() const { return m_cols; }

    // No need to define destructor, copy/move constructors, or assignment operators.
    // The std::vector member handles all resource management correctly (Rule of Zero).

private:
    size_t m_rows;
    size_t m_cols;
    std::vector<T> m_data; // Using std::vector for automatic memory management.
};

void testMatrixOperations() {
    try {
        Matrix<int> mat(3, 4, 1); // 3x4 matrix, all elements are 1
        
        // Write to the matrix using the non-const operator()
        mat(1, 2) = 99;

        // Read from the matrix using the const operator()
        const Matrix<int>& const_mat_ref = mat;
        std::cout << "Element at (1, 2): " << const_mat_ref(1, 2) << std::endl; // Prints 99
        std::cout << "Element at (0, 0): " << const_mat_ref(0, 0) << std::endl; // Prints 1

        // This will trigger the exception
        // std::cout << const_mat_ref(5, 5) << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Caught an exception: " << e.what() << std::endl;
    }
}
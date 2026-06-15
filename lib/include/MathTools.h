#ifndef MATHTOOLS_H
#define MATHTOOLS_H

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

class BinaryPolynomial {
private:
    int m_degree;
    std::string m_coefficients;
    
    void updateData(const std::string& input) {
        if (input.find_first_not_of("01") != std::string::npos) {
            throw std::invalid_argument("Binary strings only!");
        }

        size_t firstOne = input.find_first_of('1');
        
        if (firstOne == std::string::npos) {
            m_coefficients = "0";
            m_degree = -1; // Standard convention: zero polynomial has degree -1 or 0
        } else {
            m_coefficients = input.substr(firstOne);
            m_degree = static_cast<int>(m_coefficients.length()) - 1;
        }
    }

public:
    BinaryPolynomial(const std::string& input) {
        updateData(input);
    }
    void setCoefficients(const std::string& input) {
        updateData(input);
    }

    int getDegree() const { return m_degree; }

    // --- UPDATED FUNCTION ---
    // Returns the string padded with leading zeros to the specified width
    std::string getCoefficients(int width = 0) const {
        if (width <= (int)m_coefficients.length()) {
            return m_coefficients;
        }
        return std::string(width - m_coefficients.length(), '0') + m_coefficients;
    }

    int evaluate() const {
        int result = 0;
        for (char bit : m_coefficients) {
            result = (result << 1) + (bit - '0');
        }
        return result;
    }

    friend BinaryPolynomial operator+(const BinaryPolynomial& p1, const BinaryPolynomial& p2);
    friend BinaryPolynomial operator-(const BinaryPolynomial& p1, const BinaryPolynomial& p2);
    friend BinaryPolynomial operator*(const BinaryPolynomial& p1, const BinaryPolynomial& p2);
    friend BinaryPolynomial operator&(const BinaryPolynomial& p1, const BinaryPolynomial& p2);
    friend std::vector<BinaryPolynomial> operator/(const BinaryPolynomial& p1, const BinaryPolynomial& p2);
};

BinaryPolynomial addBinaryPolynomials(const BinaryPolynomial& p1, const BinaryPolynomial& p2);
std::vector<BinaryPolynomial> divideBinaryPolynomials(const BinaryPolynomial& dividend, const BinaryPolynomial& divisor);

// Prototypes for the operators
BinaryPolynomial operator+(const BinaryPolynomial& p1, const BinaryPolynomial& p2);
BinaryPolynomial operator-(const BinaryPolynomial& p1, const BinaryPolynomial& p2);
BinaryPolynomial operator*(const BinaryPolynomial& p1, const BinaryPolynomial& p2);
BinaryPolynomial operator&(const BinaryPolynomial& p1, const BinaryPolynomial& p2);
std::vector<BinaryPolynomial> operator/(const BinaryPolynomial& p1, const BinaryPolynomial& p2);

BinaryPolynomial multiplyBinaryPolynomials(const BinaryPolynomial& p1, const BinaryPolynomial& p2);
BinaryPolynomial multiplyBinaryPolynomials(const BinaryPolynomial& p1, const BinaryPolynomial& p2, const BinaryPolynomial& modulus);

BinaryPolynomial andBinaryPolynomials(const BinaryPolynomial& p1, const BinaryPolynomial& p2);

BinaryPolynomial GCD(const BinaryPolynomial& p1, const BinaryPolynomial& p2);
std::vector<BinaryPolynomial> extendedGCD(const BinaryPolynomial& p1, const BinaryPolynomial& p2);


class Matrix {
private:
    int m_rows;
    int m_cols;
    std::vector<std::vector<std::string>> m_data;

public:
    Matrix(int rows, int cols) : m_rows(rows), m_cols(cols) {    
	    m_data.resize(rows, std::vector<std::string>(cols)); 
    }

    // New Constructor: Create a matrix from existing 2D vector
    Matrix(const std::vector<std::vector<std::string>>& data) 
        : m_data(data), m_rows(data.size()), m_cols(data.empty() ? 0 : data[0].size()) {}

    // Getters
    int getRows() const { return m_rows; }
    int getCols() const { return m_cols; }
    std::vector<std::vector<std::string>> getData() const { return m_data; }

    // Setter to update the matrix data
    void setData(const std::vector<std::vector<std::string>>& data) {
        m_data = data;
        m_rows = data.size();
        m_cols = data.empty() ? 0 : data[0].size();
    }
};

Matrix shiftRow(const Matrix& mat, int rowIndex, int shiftAmount);
Matrix shiftRows(const Matrix& mat, std::vector<int> shiftAmounts);
//Matrix multiplyMatrices(const Matrix& mat1, const Matrix& mat2);
BinaryPolynomial affineTransform(const BinaryPolynomial& poly, const bool inverse = false);
Matrix linearTransformMatrix(Matrix& inputMatrix, Matrix& transformationMatrix, BinaryPolynomial& modulo);

#endif // MATHTOOLS_H

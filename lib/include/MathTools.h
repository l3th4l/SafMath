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

    // Internal logic to clean zeros and update degree
    void updateData(const std::string& input) {
        if (input.find_first_not_of("01") != std::string::npos) {
            throw std::invalid_argument("Binary strings only!");
        }

        size_t firstOne = input.find_first_of('1');
        
        if (firstOne == std::string::npos) {
            m_coefficients = "0";
            m_degree = 0;
        } else {
            m_coefficients = input.substr(firstOne);
            m_degree = static_cast<int>(m_coefficients.length()) - 1;
        }
    }

public:
    // Constructor
    BinaryPolynomial(const std::string& input) {
        updateData(input);
    }

    // Setter: Every time coefficients are changed, degree is updated automatically
    void setCoefficients(const std::string& input) {
        updateData(input);
    }

    // Getters: Read-only access
    int getDegree() const { return m_degree; }
    std::string getCoefficients() const { return m_coefficients; }

    // Your evaluation logic
    int evaluate() const {
        int result = 0;
        // Note: For string "1101", coefficients[0] is '1' (the MSB)
        // So we process from left to right
        for (char bit : m_coefficients) {
            result = (result << 1) + (bit - '0');
        }
        return result;
    }
};

BinaryPolynomial addBinaryPolynomials(const BinaryPolynomial& p1, const BinaryPolynomial& p2);
std::vector<BinaryPolynomial> divideBinaryPolynomials(const BinaryPolynomial& dividend, const BinaryPolynomial& divisor);

#endif // MATHTOOLS_H
#include <iostream>
#include <vector>

#include "include/MathTools.h"
#include "include/NumberSystems.h"
#include "include/XorTools.h"

#include <string>
#include <stdexcept>
#include <algorithm>

BinaryPolynomial addBinaryPolynomials(const BinaryPolynomial& p1, const BinaryPolynomial& p2) {

    std::string coeffs1 = p1.getCoefficients();
    std::string coeffs2 = p2.getCoefficients();

    // Pad the shorter one with leading zeros
    if (coeffs1.length() < coeffs2.length()) {
        coeffs1 = std::string(coeffs2.length() - coeffs1.length(), '0') + coeffs1;
    } else if (coeffs2.length() < coeffs1.length()) {
        coeffs2 = std::string(coeffs1.length() - coeffs2.length(), '0') + coeffs2;
    }

    return xorBinaryStrings(coeffs1, coeffs2);
    
}

std::vector<BinaryPolynomial> divideBinaryPolynomials(const BinaryPolynomial& dividend, const BinaryPolynomial& divisor)
{
    std::string divisor_coeffs = divisor.getCoefficients();
    std::string dividend_coeffs = dividend.getCoefficients();

    // Initialize quotient with as many zeros as the degree difference (max possible degree of quotient)
    int max_quotient_degree = dividend.getDegree() - divisor.getDegree();
    std::string quotient_coeffs = std::string(max_quotient_degree + 1, '0');

    //std::cout << "Initial quotient coefficients: " << quotient_coeffs << std::endl;

    BinaryPolynomial quotient("0"); // Start with zero, will update coefficients later

    BinaryPolynomial remainder(dividend_coeffs);

    if (divisor.getDegree() == 0 && divisor.getCoefficients() == "0") {
        throw std::invalid_argument("Division by zero polynomial is not allowed.");
    }else if (divisor.getDegree() > dividend.getDegree())
    {
        // Quotient is zero, remainder is dividend
        quotient.setCoefficients("0");
        return {quotient, remainder};
    }else if (divisor.getDegree() == 0 && divisor.getCoefficients() == "1")
    {
        quotient.setCoefficients(dividend_coeffs);
        return {quotient, BinaryPolynomial("0")};
    }
    
    
    while (remainder.getDegree() >= divisor.getDegree())
    {
        int degree_diff = remainder.getDegree() - divisor.getDegree();

        // set the term at index degree_diff in the quotient to 1
        // step 1. set the bit at degree_diff to '1'
        quotient_coeffs[quotient_coeffs.size() - 1 - degree_diff] = '1';

        //std::cout << "Current Quotient Coefficients: " << quotient_coeffs << std::endl;

        // Create the term to subtract (which is divisor shifted left by degree_diff)
        std::string term_coeffs = divisor_coeffs + std::string(degree_diff, '0');
        BinaryPolynomial term(term_coeffs);
        remainder.setCoefficients(xorBinaryStrings(remainder.getCoefficients(), term.getCoefficients()));
    }

    // step 2. update the quotient with the new coefficients
    quotient.setCoefficients(quotient_coeffs);

    return {quotient, remainder};
    
}

BinaryPolynomial multiplyBinaryPolynomials(const BinaryPolynomial& p1, const BinaryPolynomial& p2) {
    std::string coeffs1 = p1.getCoefficients();
    std::string coeffs2 = p2.getCoefficients();

    // The degree of the product is at most the sum of the degrees
    int max_degree = p1.getDegree() + p2.getDegree();
    std::string product_coeffs(max_degree + 1, '0');

    for (size_t i = 0; i < coeffs1.length(); ++i) {
        if (coeffs1[coeffs1.length() - 1 - i] == '1') { // If the term in p1 is present
            for (size_t j = 0; j < coeffs2.length(); ++j) {
                if (coeffs2[coeffs2.length() - 1 - j] == '1') { // If the term in p2 is present
                    // XOR the corresponding term in the product
                    int product_index = max_degree - (i + j);
                    product_coeffs[product_index] = (product_coeffs[product_index] == '0') ? '1' : '0';
                }
            }
        }
    }

    return BinaryPolynomial(product_coeffs);
}

BinaryPolynomial multiplyBinaryPolynomials(const BinaryPolynomial& p1, const BinaryPolynomial& p2, const BinaryPolynomial& modulus) {
    BinaryPolynomial product = multiplyBinaryPolynomials(p1, p2);
    std::vector<BinaryPolynomial> division_result = divideBinaryPolynomials(product, modulus);
    return division_result[1]; // Return the remainder as the result of multiplication modulo the modulus
}
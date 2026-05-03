#include <iostream>
#include <vector>

#include "include/MathTools.h"
#include "include/NumberSystems.h"
#include "include/XorTools.h"
#include "include/Utils.h"

#include <string>
#include <stdexcept>
#include <algorithm>

BinaryPolynomial addBinaryPolynomials(const BinaryPolynomial& p1, const BinaryPolynomial& p2) {

    std::string coeffs1 = p1.getCoefficients(8);
    std::string coeffs2 = p2.getCoefficients(8);

    // Pad the shorter one with leading zeros
    if (coeffs1.length() < coeffs2.length()) {
        coeffs1 = std::string(coeffs2.length() - coeffs1.length(), '0') + coeffs1;
    } else if (coeffs2.length() < coeffs1.length()) {
        coeffs2 = std::string(coeffs1.length() - coeffs2.length(), '0') + coeffs2;
    }

    return xorBinaryStrings(coeffs1, coeffs2);
    
}

// Addition wrapper
BinaryPolynomial operator+(const BinaryPolynomial& p1, const BinaryPolynomial& p2) {
    return addBinaryPolynomials(p1, p2);
}

// Subtraction wrapper
BinaryPolynomial operator-(const BinaryPolynomial& p1, const BinaryPolynomial& p2) {
    // In GF(2), addition and subtraction are both XOR
    return addBinaryPolynomials(p1, p2);
}

std::vector<BinaryPolynomial> divideBinaryPolynomials(const BinaryPolynomial& dividend, const BinaryPolynomial& divisor)
{
    std::string divisor_coeffs = divisor.getCoefficients(8);
    std::string dividend_coeffs = dividend.getCoefficients(8);

    // Initialize quotient with as many zeros as the degree difference (max possible degree of quotient)
    int max_quotient_degree = dividend.getDegree() - divisor.getDegree();
    std::string quotient_coeffs = std::string(max_quotient_degree + 1, '0');

    //std::cout << "Initial quotient coefficients: " << quotient_coeffs << std::endl;

    BinaryPolynomial quotient("0"); // Start with zero, will update coefficients later

    BinaryPolynomial remainder(dividend_coeffs);

    if (divisor.getDegree() == 0 && divisor.getCoefficients(8) == "0") {
        throw std::invalid_argument("Division by zero polynomial is not allowed.");
    }else if (divisor.getDegree() > dividend.getDegree())
    {
        // Quotient is zero, remainder is dividend
        quotient.setCoefficients("0");
        return {quotient, remainder};
    }else if (divisor.getDegree() == 0 && divisor.getCoefficients(8) == "1")
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
        remainder.setCoefficients(xorBinaryStrings(remainder.getCoefficients(8), term.getCoefficients(8)));
    }

    // step 2. update the quotient with the new coefficients
    quotient.setCoefficients(quotient_coeffs);

    return {quotient, remainder};
    
}

// Division wrapper
std::vector<BinaryPolynomial> operator/(const BinaryPolynomial& p1, const BinaryPolynomial& p2) {
    return divideBinaryPolynomials(p1, p2);
}

BinaryPolynomial multiplyBinaryPolynomials(const BinaryPolynomial& p1, const BinaryPolynomial& p2) {
    std::string coeffs1 = p1.getCoefficients(8);
    std::string coeffs2 = p2.getCoefficients(8);

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

// Multiplication wrapper
BinaryPolynomial operator*(const BinaryPolynomial& p1, const BinaryPolynomial& p2) {
    return multiplyBinaryPolynomials(p1, p2);
}

BinaryPolynomial multiplyBinaryPolynomials(const BinaryPolynomial& p1, const BinaryPolynomial& p2, const BinaryPolynomial& modulus) {
    BinaryPolynomial product = multiplyBinaryPolynomials(p1, p2);
    std::vector<BinaryPolynomial> division_result = divideBinaryPolynomials(product, modulus);
    return division_result[1]; // Return the remainder as the result of multiplication modulo the modulus
}

BinaryPolynomial operator&(const BinaryPolynomial& p1, const BinaryPolynomial& p2) {
    return andBinaryPolynomials(p1, p2);
}

BinaryPolynomial andBinaryPolynomials(const BinaryPolynomial& p1, const BinaryPolynomial& p2) {
    // Step 1: Perform bitwise AND on the coefficients
    std::string coeffs1 = p1.getCoefficients(8);
    std::string coeffs2 = p2.getCoefficients(8);

    // Pad the shorter one with leading zeros
    if (coeffs1.length() < coeffs2.length()) {
        coeffs1 = std::string(coeffs2.length() - coeffs1.length(), '0') + coeffs1;
    } else if (coeffs2.length() < coeffs1.length()) {
        coeffs2 = std::string(coeffs1.length() - coeffs2.length(), '0') + coeffs2;
    }

    // Perform bitwise AND operation
    std::string result_coeffs = "";
    for (size_t i = 0; i < coeffs1.length(); ++i) {
        result_coeffs += (coeffs1[i] == '1' && coeffs2[i] == '1') ? '1' : '0';
    }

    return BinaryPolynomial(result_coeffs);

}

/// GCD of two binary polynomials using the Euclidean algorithm
BinaryPolynomial GCD(const BinaryPolynomial& p1, const BinaryPolynomial& p2){
    BinaryPolynomial a = p1; 
    BinaryPolynomial b = p2;

    do
    {
        BinaryPolynomial temp = b;
        temp = divideBinaryPolynomials(a, b)[1]; // Get the remainder of a divided by b
        a = b; // Update a to b
        b = temp; // Update b to the remainder
    } while (b.getDegree() >= 0 && b.getCoefficients(8) != "0");

    return a; // The last non-zero remainder is the GCD
}

std::vector<BinaryPolynomial> extendedGCD(const BinaryPolynomial& p1, const BinaryPolynomial& p2) {
    BinaryPolynomial r0 = p1, r1 = p2;
    BinaryPolynomial s0("1"), s1("0");
    BinaryPolynomial t0("0"), t1("1");

    // Check condition BEFORE entering to avoid division by zero
    while (r1.getDegree() >= 0 && r1.getCoefficients(8) != "00000000") 
    {
        BinaryPolynomial temp_r = (r0 / r1)[1]; 
        BinaryPolynomial temp_q = ((r0 - temp_r) / r1)[0]; 

        BinaryPolynomial temp_s = s0 - temp_q * s1; 
        BinaryPolynomial temp_t = t0 - temp_q * t1; 

        r0 = r1;
        r1 = temp_r;
        s0 = s1;
        s1 = temp_s;
        t0 = t1;
        t1 = temp_t;
    }

    return {r0, s0, t0}; 
}

Matrix shiftRow(const Matrix& mat, int rowIndex, int shiftAmount) {
    // 1. Get a copy of the data to work on
    std::vector<std::vector<std::string>> data = mat.getData();
    
    // Safety check for valid row index
    if (rowIndex < 0 || rowIndex >= mat.getRows()) {
        return mat; 
    }

    std::vector<std::string>& row = data[rowIndex];
    int n = row.size();

    if (n == 0) return mat; // Avoid division by zero

    // 2. Normalize the shift
    // This handles negative shifts and shifts larger than the row size
    int effectiveShift = ((shiftAmount % n) + n) % n;

    if (effectiveShift != 0) {
        // Using your rbegin() logic for a right shift:
        std::rotate(row.rbegin(), row.rbegin() + effectiveShift, row.rend());
    }

    // 3. Return a new Matrix object with the updated data
    return Matrix(data);
}

Matrix shiftRows(const Matrix& mat, std::vector<int> shiftAmounts){
	
	Matrix shiftedMatrix = mat;

	for (int i = 0; i < shiftAmounts.size(); i++){
		shiftedMatrix = shiftRow(shiftedMatrix, i, shiftAmounts[i]); 
	}

	return shiftedMatrix;
}

BinaryPolynomial affineTransform(const BinaryPolynomial& poly) {
    BinaryPolynomial constant("11000110"); // additive constant for the affine transformation

    std::string affineCoeffs = "10001111"; 

    std::string intermediateCoeffs = "00000000"; // Initialize with zeros

    for (int i = 0; i < 8; i++) {
        // Rotate the polynomial to the right by 1 bit
        BinaryPolynomial rotated = shiftRight(affineCoeffs, i);

        //std::cout << "Rotated Coefficients for bit " << i << ": " << rotated.getCoefficients(8) << std::endl;

        // take the bitwise AND of the rotated polynomial and XOR all of the elements of the result together to get a single bit
        BinaryPolynomial andResult = poly & rotated;

        //std::cout << "AND Result for bit " << i << ": " << andResult.getCoefficients(8) << std::endl;

        // XOR the resulting bit with the corresponding bit in the constant to get the final bit for the output polynomial
        intermediateCoeffs[i] = xorAll(andResult.getCoefficients(8));
    }

    return BinaryPolynomial(intermediateCoeffs) + constant; // Add the constant to get the final output polynomial
}

BinaryPolynomial inverseAffineTransform(const BinaryPolynomial& poly) {
    // This is a placeholder implementation. You would need to implement the actual logic for the inverse affine transformation.
    return poly; // Return the modified polynomial after the inverse affine transformation
}

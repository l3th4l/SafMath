#include <iostream>
#include <vector>

#include <vector>
#include <iostream>

class Polynomial {

    public:
    
        int degree;
        std::vector<int> coefficients;

        // Constructor: Initializes the degree and the vector size
        Polynomial(int deg) : degree(deg), coefficients(deg + 1, 0) {}

        // Setter: Logic can be added here to validate input
        void setCoefficient(int power, int value) {
            if (power >= 0 && power <= degree) {
                coefficients[power] = value;
            }
        }

        // Getter: Access data safely
        int getCoefficient(int power) const {
            if (power >= 0 && power <= degree) {
                return coefficients[power];
            }
            return 0;
        }

        int getDegree() const {
            return degree;
        }

        // A helper method to print the polynomial nicely
        void display() const {
            for (int i = degree; i >= 0; --i) {
                std::cout << coefficients[i] << "x^" << i << (i > 0 ? " + " : "");
            }
            std::cout << std::endl;
        }
};

Polynomial addPolynomials(const Polynomial& p1, const Polynomial& p2, const int Modulo){
    Polynomial result(std::max(p1.degree, p2.degree));
    result.degree = std::max(p1.degree, p2.degree);
    result.coefficients.resize(result.degree + 1, 0);

    for (int i = 0; i <= result.degree; ++i) {
        int coeff1 = (i <= p1.degree) ? p1.coefficients[i] : 0;
        int coeff2 = (i <= p2.degree) ? p2.coefficients[i] : 0;
        result.coefficients[i] = (coeff1 + coeff2) % Modulo;
    }

    return result;
}
// Polynomial Modulo 

// Polynomial Multiplication 
Polynomial multiplyPolynomialsModulo(const Polynomial& p1, const Polynomial& p2, const Polynomial irreduciblePolynomial, const int Modulo) {
    Polynomial result(irreduciblePolynomial.degree); 
    Polynomial tempPoly(p1.degree + p2.degree); // Temporary polynomial to hold the multiplication result

    for (int i = 0; i <= p1.degree; i++){
        for (int j = 0; j <= p2.degree; j++){
            tempPoly.coefficients[j + i] = (p1.coefficients[i] * p2.coefficients[j]) % Modulo;
        }
    }

    // Now we need to reduce tempPoly modulo the irreducible polynomial
    return tempPoly; // Placeholder, the actual reduction logic needs to be implemented
}

// Polynomial Division 

// Polynomial GCD (Euclidean Algorithm) 

// PolynomialInverse (Extended Euclidean Algorithm)

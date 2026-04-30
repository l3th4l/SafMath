#include "include/AES.h"
#include "include/Utils.h"
#include "include/MathTools.h"
#include <vector>


class AESLayer{
private:
	BinaryPolynomial AESPolynomial = BinaryPolynomial("100011011");
public:
	AESLayer* input; 
	Matrix GetOutput(){
		// Get the input valaues first 
		//
		Matrix inputMatrix = input->GetOutput(); 

		// S-box layer 
		for(int i = 0;  i < 4; i++){
			for(int j = 0; j < 4; j++){

				BinaryPolynomial byte(inputMatrix.getData()[i][j]);

				// --- Inverse in GF(2^8)
				std::vector<BinaryPolynomial> EGCDResult = extendedGCD(AESPolynomial, byte);

				// --- Affine Transformation 
			}
		}		

		// Diffusion Layer 
		// Mix Column Layer 
		// Key Addition Layer 
		return inputMatrix; 
	};
};

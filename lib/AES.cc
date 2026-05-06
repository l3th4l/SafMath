#include "include/AES.h"

#include "include/MathTools.h"
#include "include/NumberSystems.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>


class Layer {
public:
    virtual ~Layer() = default;
    
    // Every layer must implement this to process data from the previous layer
    virtual Matrix GetOutput() = 0;

protected:
    Layer* inputLayer = nullptr; 
};


class InputLayer : public Layer {
private:
    Matrix staticData;

public:
    // Initialize with the starting matrix (e.g., the plaintext)
    InputLayer(Matrix initialMatrix) : staticData(initialMatrix) {}

    // Simply returns the data it was initialized with
    Matrix GetOutput() override {
        return staticData;
    }
};


class AESLayer : public Layer {
private:
    BinaryPolynomial AESPolynomial = BinaryPolynomial("100011011");

public:
    // Constructor to link this layer to the previous one
    AESLayer(Layer* previous) {
        this->inputLayer = previous;
    }

    Matrix GetOutput() override {
        // 1. Get the input values from the previous layer
        Matrix inputMatrix = inputLayer->GetOutput();
        std::vector<std::vector<std::string>> outputData = inputMatrix.getData();

        // 2. S-box logic
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
		std::cout<< "at " << i << " , " << j << std::endl;
                BinaryPolynomial byte(inputMatrix.getData()[i][j]);

                // --- Inverse in GF(2^8)
                std::vector<BinaryPolynomial> EGCDResult = extendedGCD(AESPolynomial, byte);

                // --- Affine Transformation
                std::string invCoeffs = EGCDResult[2].getCoefficients(8);
                std::reverse(invCoeffs.begin(), invCoeffs.end());

                BinaryPolynomial AESInv = BinaryPolynomial(invCoeffs);

                // Add to the data matrix 
                outputData[i][j] = affineTransform(AESInv).getCoefficients(8);
		std::reverse(outputData[i][j].begin(), outputData[i][j].end());
		std::cout << "resulting S-box value: " << outputData[i][j] << std::endl;

            }
        }

	// 3. Shift Rows 
	Matrix outputMatrix =  Matrix(outputData);
	outputMatrix = shiftRows(outputMatrix, {0, 1, 2, 3, 4});

	//4. Mix Columns 
	std::vector<std::vector<std::string>> transformationMatrixData = 
	{ 
		{"02", "03", "01", "01"}, 
		{"01", "02", "03", "01"}, 
		{"01", "01", "02", "03"}, 
		{"03", "01", "01", "02"}, 
	};

	transformationMatrixData = hex2DArrayToBinary2DArray(transformationMatrixData);

	Matrix transformationMatrix = Matrix(transformationMatrixData);
	
	outputMatrix = linearTransformMatrix(outputMatrix, transformationMatrix);

	//5. Key Addition Layer 
	//TODO Implement this 

        return outputMatrix; 
    }
};

int main() {

	
    //
    std::cout << "What is happening?" << std::endl;
    // 1. Prepare the 4x4 data (all 0s)
    std::vector<std::vector<std::string>> testData(4, std::vector<std::string>(4, "11000010"));
    
    // 2. Set the first byte to 1100 0010
    testData[0][0] = "11000010";
    testData[1][0] = "11000010";


    // 3. Create the Matrix object
    Matrix initialMatrix(testData);

    // 4. Set up the layer chain
    InputLayer* input = new InputLayer(initialMatrix);
    AESLayer* subBytes = new AESLayer(input);

    // 5. Execute and retrieve output
    Matrix resultMatrix = subBytes->GetOutput();
    std::vector<std::vector<std::string>> resultData = resultMatrix.getData();

    std::cout << "We're on it!"<< std::endl;

    // 6. Print the results (First row)
    std::cout << "SubBytes Output for first row:" << std::endl;
    for(int j = 0; j < 4; j++) {
        std::cout << "[" << resultData[0][j] << "] ";
    }
    
    // Clean up memory
    delete subBytes;
    delete input;

    return 0;
}

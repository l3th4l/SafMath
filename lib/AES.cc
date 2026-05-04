#include "include/AES.h"

#include "include/MathTools.h"
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
	
	


        return outputMatrix; 
    }
};



Matrix linearTransformMatrix(Matrix& inputMatrix, Matrix& transformationMatrix){

	std::vector<std::vector<std::string>> inputMatrixData = inputMatrix.getData();
	std::vector<std::vector<std::string>> outputMatrixData; 

	for (int inputColumn = 0; inputColumn < inputMatrix.getCols(); inputColumn++){

		//first get all elements of the row 
		std::vector<BinaryPolynomial> columnVector;
		std::vector<BinaryPolynomial> outputColumnVector(transformationMatrix.getRows(), BinaryPolynomial("0"));

		for(int inputRow = 0; inputRow < inputMatrix.getRows(); inputRow++){
			columnVector.emplace_back(inputMatrixData[inputRow][inputColumn]); 
		}

		//perform the linear transformation with the transformation matrix 
		for(int transRow = 0; transRow < transformationMatrix.getRows(); transRow++)
		{
			for(int transColumn = 0; transColumn < transformationMatrix.getCols(); transColumn++)
			{
				outputColumnVector[transRow] =  outputColumnVector[transRow] + transformationMatrix.getData()[transRow][transColumn] * columnVector[transColumn];
			} 
		}

		//convert polynomial vector to string vector 
		std::vector<std::string> outputStringVector; 
		for(int i =0; i < outputColumnVector.size(); i++){
			outputStringVector.push_back(outputColumnVector[i].getCoefficients(8));
		}

		//
		outputMatrixData.push_back(outputStringVector); 
		
	}
	return Matrix(outputMatrixData); 
}

int main() {

	
    //
    std::cout << "What is happening?" << std::endl;
    // 1. Prepare the 4x4 data (all 0s)
    std::vector<std::vector<std::string>> testData(4, std::vector<std::string>(4, "00000000"));
    
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

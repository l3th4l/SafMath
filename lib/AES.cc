#include "include/AES.h"

#include "include/MathTools.h"
#include "include/NumberSystems.h"
#include "include/Utils.h"
#include <bitset>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <algorithm>


class Layer {
public:
    virtual ~Layer() = default;
    
    // Every layer must implement this to process data from the previous layer
    virtual Matrix GetOutput() = 0;
    virtual Matrix GetKey() = 0; 
    virtual int GetRound() = 0;

protected:
    Layer* inputLayer = nullptr; 
};


class InputLayer : public Layer {
private:

    BinaryPolynomial AESPolynomial = BinaryPolynomial("100011011");

    Matrix staticData;
    Matrix staticKey; 

public:
    // Initialize with the starting matrix (e.g., the plaintext)
    InputLayer(Matrix initialMatrix, Matrix initialKey) : staticData(initialMatrix) , staticKey(initialKey){}

    // Simply returns the data it was initialized with
    Matrix GetOutput() override {
        //return staticData;

	std::vector<std::vector<std::string>> intermediateOutputData(staticData.getRows(), std::vector<std::string>(staticData.getCols(), "0")); 
	
	for(int i = 0; i < staticData.getRows(); i++){
		for(int j = 0; j < staticData.getCols(); j++){
			intermediateOutputData[i][j] = (BinaryPolynomial(staticKey.getData()[i][j]) + BinaryPolynomial(staticData.getData()[i][j])).getCoefficients(8);
		}
	}

	
	Matrix outputMatrix = Matrix(intermediateOutputData);

        return outputMatrix; 


    }

    Matrix GetKey() override {
	    //return staticKey;
	   int roundCoeffecient = RoundCoeffecient(0);
	   std::string RCString = std::bitset<8>(roundCoeffecient).to_string();
	   BinaryPolynomial RCPolynomial = BinaryPolynomial(RCString);

	   return keyScheduleStep(staticKey, AESPolynomial, RCPolynomial);

	     
    }

    int GetRound() override {
	    return 0;
    }
};


class AESLayer : public Layer {
private:
    BinaryPolynomial AESPolynomial = BinaryPolynomial("100011011");
    int round; 
    Matrix roundKey = Matrix(4, 4);

public:
    // Constructor to link this layer to the previous one
    AESLayer(Layer* previous) {
        this->inputLayer = previous;
	//this->round = inputLayer->GetRound();
	round = inputLayer->GetRound(); 
	round = round + 1; 
	roundKey = inputLayer->GetKey(); 
    }

    int GetRound() override{
	    return round;
    }

    Matrix GetKey() override{
	   int roundCoeffecient = RoundCoeffecient(round);
	   std::string RCString = std::bitset<8>(roundCoeffecient).to_string();
	   BinaryPolynomial RCPolynomial = BinaryPolynomial(RCString);

	   return keyScheduleStep(roundKey, AESPolynomial, RCPolynomial);
	   
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

                outputData[i][j] = SBoxTransform(AESPolynomial, byte).getCoefficients(8);

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
	
	// Remember that the linear transformation is done in GF(2^8)/AES 
	outputMatrix = linearTransformMatrix(outputMatrix, transformationMatrix, AESPolynomial);

	//5. Key Addition Layer 
	//TODO Implement this. Make sure to transpose the key matrix
	
	std::vector<std::vector<std::string>> intermediateOutputData(outputMatrix.getRows(), std::vector<std::string>(outputMatrix.getCols(), "0")); 
	
	for(int i = 0; i < outputMatrix.getRows(); i++){
		for(int j = 0; j < outputMatrix.getCols(); j++){
			intermediateOutputData[i][j] = (BinaryPolynomial(roundKey.getData()[i][j]) + BinaryPolynomial(outputMatrix.getData()[i][j])).getCoefficients(8);
		}
	}

	outputMatrix = Matrix(intermediateOutputData);

        return outputMatrix; 
    }
};

BinaryPolynomial SBoxTransform(BinaryPolynomial AESPolynomial, BinaryPolynomial byte){

	 	// --- Inverse in GF(2^8)
                std::vector<BinaryPolynomial> EGCDResult = extendedGCD(AESPolynomial, byte);

                // --- Affine Transformation
                std::string invCoeffs = EGCDResult[2].getCoefficients(8);
                std::reverse(invCoeffs.begin(), invCoeffs.end());

                BinaryPolynomial AESInv = BinaryPolynomial(invCoeffs);

                // Add to the data matrix 
                return affineTransform(AESInv);


};

Matrix keyScheduleStep(Matrix& key, BinaryPolynomial& AESPolynomial, BinaryPolynomial& RC){
	std::vector<std::vector<std::string>> keyData = key.getData(); 
	std::vector<std::string> W0 = keyData[0];

	std::vector<std::string> Wn = keyData[keyData.size() -1];
	std::vector<std::string> g = {Wn[1], Wn[2], Wn[3], Wn[0]};

	for(int i=0; i<g.size(); i++){
		g[i] = SBoxTransform(AESPolynomial, BinaryPolynomial(g[i])).getCoefficients(8);
	}

	g[0] = (BinaryPolynomial(g[0]) + RC).getCoefficients(8); 

	std::vector<std::vector<std::string>> outputKeyData(4, std::vector<std::string>(4, "00000000"));

	for(int W = 0; W < keyData.size(); W++){
		if(W == 0){
			for(int i=0; i < outputKeyData[0].size(); i++){
				outputKeyData[W][i] = (BinaryPolynomial(g[i]) + BinaryPolynomial(keyData[W][i])).getCoefficients(8);
			}
		}else{
			for(int i=0; i < outputKeyData[0].size(); i++){
				outputKeyData[W][i] = (BinaryPolynomial(keyData[W-1][i]) + BinaryPolynomial(keyData[W][i])).getCoefficients(8);
			}
		}
	}

	return Matrix(outputKeyData);


}

//TODO Add a function to calculate the RoundKeys
// It's easier to work with hexadecimal numbers here for calculating the round keys 
// A critical design flaw of the current implementations is the usage of strings to perform all operations 

int RoundCoeffecient(int j){

	int i = j+1;

	std::cout << i << std::endl; 
	// Base case 
	if (i == 1){
		return 1;
	}

	int previousRoundCoeffecient = RoundCoeffecient(i-1);	

	std::cout << previousRoundCoeffecient << std::endl;


	if(previousRoundCoeffecient < 0x80){
		return 2 * previousRoundCoeffecient;
	} else if (previousRoundCoeffecient >= 0x80) {
		return (2 * previousRoundCoeffecient) ^ 0x11B; 
	}
	else {
		return 0; 
	}
}


int main() {

	
    
    //
    //std::cout << "What is happening?" << std::endl;
    //
    std::string keyHex = "2B7E151628AED2A6ABF7158809CF4F3C";
    std::string binaryKeyString = hexStringToBinary(keyHex);
    Matrix keyMatrix = AESMatrixFromBinaryString(binaryKeyString); 


    std::string inputPlaintext = "6BC1BEE22E409F96E93D7E117393172A";
    //std::string inputPlaintext = "C2C2C2C2C2C2C2C2C2C2C2C2C2C2C2C2";
    std::string inputBinaryString = hexStringToBinary(inputPlaintext);
    Matrix initialMatrix = AESMatrixFromBinaryString(inputBinaryString);

    /*
    // 1. Prepare the 4x4 data (all 0s)
    std::vector<std::vector<std::string>> testData(4, std::vector<std::string>(4, "11000010"));
    
    // 2. Set the first byte to 1100 0010
    testData[0][0] = "11000010";
    testData[1][0] = "11000010";


    // 3. Create the Matrix object
    Matrix initialMatrix(testData);
    */

    // 4. Set up the layer chain
    InputLayer* input = new InputLayer(initialMatrix, keyMatrix);
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
    
    /*
	BinaryPolynomial testAESPoly = BinaryPolynomial("100011011");

    std::cout << "========================================\n";
    std::cout << "      RUNNING MIXCOLUMNS UNIT TESTS     \n";
    std::cout << "========================================\n\n";

    // 1. Setup the Fixed AES Transformation Matrix
    std::vector<std::vector<std::string>> transformationMatrixData = { 
        {"02", "03", "01", "01"}, 
        {"01", "02", "03", "01"}, 
        {"01", "01", "02", "03"}, 
        {"03", "01", "01", "02"}, 
    };
    
    // Convert hex identifiers to binary string representation used by your library
    transformationMatrixData = hex2DArrayToBinary2DArray(transformationMatrixData);
    Matrix transformationMatrix = Matrix(transformationMatrixData);


    // ---------------------------------------------------------
    // TEST CASE 1: Standard NIST Test Vector (From FIPS-197 Appendix B)
    // ---------------------------------------------------------
    std::cout << "--- Test Case 1: NIST Standard Column ---\n";
    
    // NIST Input column: D4, BF, 5D, 30 (filled out as a full matrix for your system)
    std::vector<std::vector<std::string>> testInputHex1 = {
        {"D4", "00", "00", "00"},
        {"BF", "00", "00", "00"},
        {"5D", "00", "00", "00"},
        {"30", "00", "00", "00"}
    };
    
    // Expected Output column: 04, 66, 81, E5
    std::vector<std::vector<std::string>> expectedOutputHex1 = {
        {"04", "00", "00", "00"},
        {"66", "00", "00", "00"},
        {"81", "00", "00", "00"},
        {"E5", "00", "00", "00"}
    };

    Matrix inputMatrix1 = Matrix(hex2DArrayToBinary2DArray(testInputHex1));
    Matrix expectedMatrix1 = Matrix(hex2DArrayToBinary2DArray(expectedOutputHex1));

    // Execute transformation
    Matrix resultMatrix1 = linearTransformMatrix(inputMatrix1, transformationMatrix, testAESPoly);

    // Verify first column
    bool pass1 = true;
    std::cout << "Index | Expected (Bin) | Result (Bin) | Status\n";
    std::cout << "----------------------------------------------\n";
    for(int i = 0; i < 4; i++) {
        std::string expected = expectedMatrix1.getData()[i][0];
        std::string result = resultMatrix1.getData()[i][0];
        bool match = (expected == result);
        if(!match) pass1 = false;
        
        std::cout << " [" << i << ",0] | " << expected << " | " << result 
                  << " | " << (match ? "PASS" : "FAIL") << "\n";
    }
    std::cout << "Conclusion: Test Case 1 " << (pass1 ? "PASSED 🎉" : "FAILED ❌") << "\n\n";


    // ---------------------------------------------------------
    // TEST CASE 2: Simple Sequential Test Vector
    // ---------------------------------------------------------
    std::cout << "--- Test Case 2: Sequential Vector ---\n";

    // Input Column: 01, 02, 03, 04
    std::vector<std::vector<std::string>> testInputHex2 = {
        {"49", "00", "00", "00"},
        {"DB", "00", "00", "00"},
        {"87", "00", "00", "00"},
        {"3B", "00", "00", "00"}
    };

    // Expected Output Column: 04, 01, 02, 09
    std::vector<std::vector<std::string>> expectedOutputHex2 = {
        {"58", "00", "00", "00"},
        {"4D", "00", "00", "00"},
        {"CA", "00", "00", "00"},
        {"F1", "00", "00", "00"}
    };

    Matrix inputMatrix2 = Matrix(hex2DArrayToBinary2DArray(testInputHex2));
    Matrix expectedMatrix2 = Matrix(hex2DArrayToBinary2DArray(expectedOutputHex2));

    // Execute transformation
    Matrix resultMatrix2 = linearTransformMatrix(inputMatrix2, transformationMatrix, testAESPoly);

    // Verify first column
    bool pass2 = true;
    std::cout << "Index | Expected (Bin) | Result (Bin) | Status\n";
    std::cout << "----------------------------------------------\n";
    for(int i = 0; i < 4; i++) {
        std::string expected = expectedMatrix2.getData()[i][0];
        std::string result = resultMatrix2.getData()[i][0];
        bool match = (expected == result);
        if(!match) pass2 = false;
        
        std::cout << " [" << i << ",0] | " << expected << " | " << result 
                  << " | " << (match ? "PASS" : "FAIL") << "\n";
    }
    std::cout << "Conclusion: Test Case 2 " << (pass2 ? "PASSED 🎉" : "FAILED ❌") << "\n\n";

    return 0;
    */
}

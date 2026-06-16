#include "include/AES.h"

#include "include/MathTools.h"
#include "include/NumberSystems.h"
#include "include/Utils.h"
#include <bitset>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>
#include <vector>
#include <algorithm>


class Layer {
public:
    virtual ~Layer() = default;
    
    // Every layer must implement this to process data from the previous layer
    virtual Matrix GetOutput() = 0;
    virtual Matrix GetKey() = 0;
    virtual std::vector<Matrix> GetAllKeys() = 0;
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
	//
	    //std::cout << "Round 1 Input :" << std::endl; 

	std::vector<std::vector<std::string>> intermediateOutputData(staticData.getRows(), std::vector<std::string>(staticData.getCols(), "0")); 
	
	for(int i = 0; i < staticData.getRows(); i++){
		for(int j = 0; j < staticData.getCols(); j++){
			intermediateOutputData[i][j] = (BinaryPolynomial(staticKey.getData()[i][j]) + BinaryPolynomial(staticData.getData()[i][j])).getCoefficients(8);
			//std::cout << binaryToHexString(intermediateOutputData[i][j])<< " ";
		}
		//std::cout << std::endl; 
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

    std::vector<Matrix> GetAllKeys() override{
	    // Do nothing hehe 
    };
};

// Decryption Input
class DecryptionInputLayer : public Layer {
private:

    BinaryPolynomial AESPolynomial = BinaryPolynomial("100011011");

    Matrix staticData;
    Matrix staticKey; 

    std::vector<Matrix> allKeys; 

public:
    // Initialize with the starting matrix (e.g., the plaintext)
    DecryptionInputLayer(Matrix initialMatrix, Matrix initialKey): staticData(initialMatrix), staticKey(initialKey){
	    
	    allKeys.push_back(staticKey);

	    int maxRounds = 10;

	    for(int r = 0; r < maxRounds; r++){

		//calculate all keys starting from initial key 
		int roundCoeffecient = RoundCoeffecient(r);

		//std::cout << "Round Coefficient " << r << " ";

	   	std::string RCString = std::bitset<8>(roundCoeffecient).to_string();
	   	BinaryPolynomial RCPolynomial = BinaryPolynomial(RCString);

		//std::cout << binaryToHexString(RCString) << std::endl;

	   	Matrix tempRoundKey = keyScheduleStep(allKeys[r], AESPolynomial, RCPolynomial);

		allKeys.push_back(tempRoundKey);

		    
	    }
    };

    // Simply returns the data it was initialized with
    Matrix GetOutput() override {
        //return staticData;
	//
	    //std::cout << "Round 1 (Decrypt) Key: " << std::endl;
	    //

	for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
		//std::cout << binaryToHexString(allKeys[GetRound()].getData()[i][j]) << " ";
            }
	    //std::cout << std::endl; 
        }

	std::vector<std::vector<std::string>> intermediateOutputData(staticData.getRows(), std::vector<std::string>(staticData.getCols(), "0")); 
	
	for(int i = 0; i < staticData.getRows(); i++){
		for(int j = 0; j < staticData.getCols(); j++){
			intermediateOutputData[i][j] = (BinaryPolynomial(allKeys[GetRound()].getData()[i][j]) + BinaryPolynomial(staticData.getData()[i][j])).getCoefficients(8);
			//std::cout << binaryToHexString(intermediateOutputData[i][j])<< " ";
		}
		//std::cout << std::endl; 
	}

	
	Matrix outputMatrix = Matrix(intermediateOutputData);

        return outputMatrix; 


    }

    Matrix GetKey() override {

	   return allKeys[GetRound()];
	     
    }

    std::vector<Matrix> GetAllKeys() override
    {
	    return allKeys;
    }

    int GetRound() override {
	    return 10;
    }
};


class AESLayer : public Layer {
private:
    BinaryPolynomial AESPolynomial = BinaryPolynomial("100011011");
    int round; 
    Matrix roundKey = Matrix(4, 4);
    bool disableMixColumn; 

public:
    // Constructor to link this layer to the previous one
    AESLayer(Layer* previous, bool _disableMixColumn = false) {
	this->disableMixColumn = _disableMixColumn; 
        this->inputLayer = previous;
	//this->round = inputLayer->GetRound();
	round = inputLayer->GetRound(); 
	round = round + 1; 
	roundKey = inputLayer->GetKey(); 
    }

    std::vector<Matrix> GetAllKeys() override{
	    // Also do nothing hehe 
    };

    int GetRound() override{
	    //std::cout << " round : " << round << std::endl; 
	    return round;
    };

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

	//std::cout << "Round [" << round << "] " << std::endl; 

        // 2. S-box logic
	//std::cout << "After Sub Bytes : " << std::endl; 
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
		//std::cout<< "at " << i << " , " << j << std::endl;
                BinaryPolynomial byte(inputMatrix.getData()[i][j]);

                outputData[i][j] = SBoxTransform(AESPolynomial, byte).getCoefficients(8);
		std::reverse(outputData[i][j].begin(), outputData[i][j].end());

		//std::cout << binaryToHexString(outputData[i][j]) << " ";

            }
	    //std::cout << std::endl; 
        }

	// 3. Shift Rows 
	Matrix outputMatrix =  Matrix(outputData);
	outputMatrix = shiftRows(outputMatrix, {0, -1, -2, -3, -4});

	//std::cout << "After Shift Rows : " << std::endl; 
  
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
		//std::cout << binaryToHexString(outputMatrix.getData()[i][j]) << " ";
            }
	    //std::cout << std::endl; 
        }

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
	
	if(!disableMixColumn)
	{
	// Remember that the linear transformation is done in GF(2^8)/AES 
		outputMatrix = linearTransformMatrix(outputMatrix, transformationMatrix, AESPolynomial);
	}

	//std::cout << "After Mix Columns : " << std::endl; 

        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
		//std::cout << binaryToHexString(outputMatrix.getData()[i][j]) << " ";
            }
	    //std::cout << std::endl; 
        }

	//5. Key Addition Layer 
	//TODO Implement this. Make sure to transpose the key matrix
	//
	//std::cout << "Round " << round << " key : " << std::endl; 
	for(int i = 0; i < roundKey.getRows(); i ++)
	{
		for(int j = 0; j < roundKey.getCols(); j++)
		{
			//std::cout << binaryToHexString(roundKey.getData()[i][j]) << " "; 
		}

		//std::cout << std::endl; 
	}
	
	std::vector<std::vector<std::string>> intermediateOutputData(outputMatrix.getRows(), std::vector<std::string>(outputMatrix.getCols(), "0")); 

	//std::cout << "Output " << std::endl;
	for(int i = 0; i < outputMatrix.getRows(); i++){
		for(int j = 0; j < outputMatrix.getCols(); j++){
			intermediateOutputData[i][j] = (BinaryPolynomial(roundKey.getData()[i][j]) + BinaryPolynomial(outputMatrix.getData()[i][j])).getCoefficients(8);
			//std::cout << binaryToHexString(intermediateOutputData[i][j])<< " ";

		}
		//std::cout << std::endl; 

	}

	outputMatrix = Matrix(intermediateOutputData);

        return outputMatrix; 
    }
};

class AESDecryptionLayer : public Layer
{
private:
    BinaryPolynomial AESPolynomial = BinaryPolynomial("100011011");
    int round; 
    std::vector<Matrix> allKeys;
    Matrix roundKey = Matrix(4, 4);
    bool disableMixColumn; 

public:
    // Constructor to link this layer to the previous one
    AESDecryptionLayer(Layer* previous, bool _disableMixColumn = false) {
	this->disableMixColumn = _disableMixColumn; 
        this->inputLayer = previous;
	//this->round = inputLayer->GetRound();
	round = inputLayer->GetRound(); 
	round = round - 1; 
	allKeys = inputLayer->GetAllKeys();
	roundKey = allKeys[round]; 
    };

    int GetRound() override{
	    return round;
    };

    Matrix GetKey() override{
	    // hehe
    };
    
    std::vector<Matrix> GetAllKeys() override{
	    return allKeys;
    };

    Matrix GetOutput() override{

	// 1. Get the input values from the previous layer
        Matrix inputMatrix = inputLayer->GetOutput();
        std::vector<std::vector<std::string>> outputData = inputMatrix.getData();
	Matrix outputMatrix =  Matrix(outputData);

	//std::cout << "Round [" << round << "] " << std::endl; 
	
	//4. Mix Columns [1.]
	std::vector<std::vector<std::string>> transformationMatrixData = 
	{ 
		{"0E", "0B", "0D", "09"}, 
		{"09", "0E", "0B", "0D"}, 
		{"0D", "09", "0E", "0B"}, 
		{"0B", "0D", "09", "0E"}, 
	};

	transformationMatrixData = hex2DArrayToBinary2DArray(transformationMatrixData);

	Matrix transformationMatrix = Matrix(transformationMatrixData);
	
	if(!disableMixColumn)
	{
	// Remember that the linear transformation is done in GF(2^8)/AES 
		outputMatrix = linearTransformMatrix(outputMatrix, transformationMatrix, AESPolynomial);
	}

	//std::cout << "After Mix Columns : " << std::endl; 

        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
		//std::cout << binaryToHexString(outputMatrix.getData()[i][j]) << " ";
            }
	    //std::cout << std::endl; 
        }

	// 3. Shift Rows [2.]
	outputMatrix = shiftRows(outputMatrix, {0, 1, 2, 3, 4});

	//std::cout << "After Shift Rows : " << std::endl; 
  
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
		//std::cout << binaryToHexString(outputMatrix.getData()[i][j]) << " ";
            }
	    //std::cout << std::endl; 
        }

        // 2. S-box logic [3.]
	//std::cout << "After Sub Bytes : " << std::endl; 
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
		//std::cout<< "at " << i << " , " << j << std::endl;
                BinaryPolynomial byte(outputMatrix.getData()[i][j]);

                outputData[i][j] = SBoxTransform(AESPolynomial, byte, true).getCoefficients(8);
		std::reverse(outputData[i][j].begin(), outputData[i][j].end());

		//std::cout << binaryToHexString(outputData[i][j]) << " ";

            }
	    //std::cout << std::endl; 
        }

	outputMatrix = Matrix(outputData);
	
	//Key Addition 
		    //std::cout << "Round "<< GetRound() -1 <<" (Decrypt) Key: " << std::endl;
	    //

	for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
		//std::cout << binaryToHexString(allKeys[GetRound()].getData()[i][j]) << " ";
            }
	    //std::cout << std::endl; 
        }

	std::vector<std::vector<std::string>> intermediateOutputData(outputMatrix.getRows(), std::vector<std::string>(outputMatrix.getCols(), "0")); 
	
	//std::cout << "Output for round " << GetRound() << std::endl; 

	for(int i = 0; i < outputMatrix.getRows(); i++){
		for(int j = 0; j < outputMatrix.getCols(); j++){
			intermediateOutputData[i][j] = (BinaryPolynomial(allKeys[GetRound()].getData()[i][j]) + BinaryPolynomial(outputMatrix.getData()[i][j])).getCoefficients(8);
			//std::cout << binaryToHexString(intermediateOutputData[i][j])<< " ";
		}
		//std::cout << std::endl; 
	}
	
	outputMatrix = Matrix(intermediateOutputData); 
 

	return outputMatrix; 

    }

};

BinaryPolynomial SBoxTransform(BinaryPolynomial AESPolynomial, BinaryPolynomial byte, bool inverse){

		BinaryPolynomial tempByte = byte;

		if(inverse){
			std::string tbString = tempByte.getCoefficients(8);
			std::reverse(tbString.begin(), tbString.end());
			tempByte = affineTransform(tempByte, inverse);
			tbString = tempByte.getCoefficients(8);
			//std::reverse(tbString.begin(), tbString.end());
			tempByte = BinaryPolynomial(tbString);

			//std::cout << "Inv affine step : " << tempByte.getCoefficients(8) << std::endl;
		}

	 	// --- Inverse in GF(2^8)
                std::vector<BinaryPolynomial> EGCDResult = extendedGCD(AESPolynomial, tempByte);

                // --- Affine Transformation
                std::string invCoeffs = EGCDResult[2].getCoefficients(8);
                std::reverse(invCoeffs.begin(), invCoeffs.end());

                BinaryPolynomial AESInv = BinaryPolynomial(invCoeffs);

		if(inverse){
			//std::cout << "GF Inv step : " << invCoeffs << std::endl;
			return AESInv; 
		}
		else {
                	// Add to the data matrix 
                	return affineTransform(AESInv, inverse);
		}


};

Matrix keyScheduleStep(Matrix& key, BinaryPolynomial& AESPolynomial, BinaryPolynomial& RC){
	std::vector<std::vector<std::string>> keyDataTemp = key.getData();
	std::vector<std::vector<std::string>> keyData = key.getData();
	std::vector<std::vector<std::string>> keyDataOut = key.getData();
	//transpose key data before processing
	//NOTE : This is hardcoded to 4x4 key 
	for(int i = 0; i < 4; i ++)
	{
		for(int j = 0; j < 4; j ++)
		{
			keyData[i][j] = keyDataTemp[j][i];
		}
	}

	std::vector<std::string> W0 = keyData[0];

	std::vector<std::string> Wn = keyData[keyData.size() -1];

	//std::cout << "Key Schedule " << std::endl; 
	//std::cout << "Temp W[3] : "; 

	for(int n = 0; n < 4; n++){
		//std::cout << binaryToHexString(Wn[n]); 
	}



	std::vector<std::string> g = {Wn[1], Wn[2], Wn[3], Wn[0]};

	//std::cout << std::endl << "After Rotword: "; 

	for(int n = 0; n < 4; n++){
		//std::cout << binaryToHexString(g[n]); 
	}

	//std::cout << std::endl << "After Subword: "; 

	for(int i=0; i<g.size(); i++){
		BinaryPolynomial giPoly = BinaryPolynomial(g[i]);		
		g[i] = SBoxTransform(AESPolynomial, giPoly).getCoefficients(8);
		std::reverse(g[i].begin(), g[i].end());
		//std::cout << binaryToHexString(g[i]);
	}

	
	//std::cout << std::endl << "Rcon: " << binaryToHexString(RC.getCoefficients(8)) << std::endl; 
	
	//std::cout << std::endl << "After XOR with Rcon WeWe : " << std::endl; 

	g[0] = (BinaryPolynomial(g[0]) + RC).getCoefficients(8);

	for(int n = 0; n < 4; n++){
		//std::cout << binaryToHexString(g[n]); 
	}

	//std::cout << "XOR with temp(s) : " << std::endl; 

	std::vector<std::vector<std::string>> outputKeyData(4, std::vector<std::string>(4, "00000000"));

	for(int W = 0; W < keyData.size(); W++){
		if(W == 0){
			for(int i=0; i < outputKeyData[0].size(); i++){
				//std::cout << binaryToHexString(keyData[W][i]);
				outputKeyData[W][i] = (BinaryPolynomial(g[i]) + BinaryPolynomial(keyData[W][i])).getCoefficients(8);
			}
		}else{
			for(int i=0; i < outputKeyData[0].size(); i++){
				//std::cout << binaryToHexString(keyData[W][i]);
				outputKeyData[W][i] = (BinaryPolynomial(outputKeyData[W-1][i]) + BinaryPolynomial(keyData[W][i])).getCoefficients(8);
			}
		}
		//std::cout << std::endl;
	}

	//transpose before returning 
	
	for(int i = 0; i < 4; i ++)
	{
		for(int j = 0; j < 4; j ++)
		{
			keyDataOut[i][j] = outputKeyData[j][i];
		}
	}

	return Matrix(keyDataOut);
	//return Matrix(outputKeyData);


}

//TODO Add a function to calculate the RoundKeys
// It's easier to work with hexadecimal numbers here for calculating the round keys 
// A critical design flaw of the current implementations is the usage of strings to perform all operations 

int RoundCoeffecient(int j){

	int i = j+1;

	//std::cout << i << std::endl; 
	// Base case 
	if (i == 1){
		return 1;
	}

	int previousRoundCoeffecient = RoundCoeffecient(j-1);	

	//std::cout << previousRoundCoeffecient << std::endl;


	if(previousRoundCoeffecient < 0x80){
		return 2 * previousRoundCoeffecient;
	} else if (previousRoundCoeffecient >= 0x80) {
		return (2 * previousRoundCoeffecient) ^ 0x11B; 
	}
	else {
		return 0; 
	}
}

std::string ECBDecrypt(const std::string& key, const std::string& plainText)
{
	std::string binaryKeyString = plainTextToBinaryString(key); 
	Matrix keyMatrix = AESMatrixFromBinaryString(binaryKeyString);

	std::string inputBinaryString = base64ToBinary(plainText);
	int numBlocks = inputBinaryString.size() / 128; 

	std::string outputString = ""; 

	for(int block = 0; block < numBlocks; block ++)
	{
		std::string binaryStringBlock = inputBinaryString.substr(block * 128, 128);
		Matrix initialMatrix = AESMatrixFromBinaryString(binaryStringBlock);

		DecryptionInputLayer* deInput = new DecryptionInputLayer(initialMatrix, keyMatrix); 
    		AESDecryptionLayer* y = new AESDecryptionLayer(deInput, true);
    		y = new AESDecryptionLayer(y);
    		y = new AESDecryptionLayer(y);
    		y = new AESDecryptionLayer(y);
    		y = new AESDecryptionLayer(y);
    		y = new AESDecryptionLayer(y);
    		y = new AESDecryptionLayer(y);
    		y = new AESDecryptionLayer(y);
    		y = new AESDecryptionLayer(y);
   		y = new AESDecryptionLayer(y);
    		Matrix plainTextResultMatrix = y->GetOutput(); 

		std::string outBlock = ""; 

		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				outBlock.append(binaryStringToPlainText(plainTextResultMatrix.getData()[j][i]));
			}
		}

		outputString.append(outBlock);

	}

	return outputString;

}

int main() {

	
    //std::cout << "What is happening?" << std::endl;
    //
    //
	
    std::string key = "YELLOW SUBMARINE"; 

    std::string plainText64 = "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"; 

    std::string output = ECBDecrypt(key, plainText64); 

    std::cout << output << std::endl;

    /*
    std::string keyHex = "2B7E151628AED2A6ABF7158809CF4F3C";
    std::string binaryKeyString = hexStringToBinary(keyHex);
    Matrix keyMatrix = AESMatrixFromBinaryString(binaryKeyString); 

    std::string inputPlaintext = "3243F6A8885A308D313198A2E0370734";
    //std::string inputPlaintext = "6BC1BEE22E409F96E93D7E117393172A";
    std::string inputBinaryString = hexStringToBinary(inputPlaintext);
    Matrix initialMatrix = AESMatrixFromBinaryString(inputBinaryString);
    
    BinaryPolynomial _AESPolynomial = BinaryPolynomial("100011011");

    BinaryPolynomial SBOXTest = BinaryPolynomial("00100101");
    std::string SBR = SBoxTransform(_AESPolynomial, SBOXTest, true).getCoefficients(8); 
    std::reverse(SBR.begin(), SBR.end());
    std::cout << "SBOX test " << SBR << std::endl;
    std::cout << "SBOX HEX " << binaryToHexString(SBR) << std::endl;

    //Check that the inverse affine transformation works 
    //

    std::string aff = affineTransform(SBOXTest).getCoefficients(8);
    std::reverse(aff.begin(), aff.end());
    BinaryPolynomial invaff = affineTransform(BinaryPolynomial(aff), true);
    

    std::cout << "Inverse affine test " << invaff.getCoefficients(8) << std::endl;

    
    // --- Inverse in GF(2^8)

    std::vector<BinaryPolynomial> EGCDResult = extendedGCD(_AESPolynomial, SBOXTest);

    // --- Affine Transformation
        
    std::string invCoeffs = EGCDResult[2].getCoefficients(8);

    
    EGCDResult = extendedGCD(_AESPolynomial, EGCDResult[2]);


    std::cout << "Inverse GF test " << EGCDResult[2].getCoefficients(8) << std::endl;

    InputLayer* input = new InputLayer(initialMatrix, keyMatrix);
    AESLayer* x = new AESLayer(input);
    x = new AESLayer(x);
    x = new AESLayer(x);
    x = new AESLayer(x);
    x = new AESLayer(x);
    x = new AESLayer(x);
    x = new AESLayer(x);
    x = new AESLayer(x);
    x = new AESLayer(x);
    x = new AESLayer(x, true);

    // 5. Execute and retrieve output
    Matrix resultMatrix = x->GetOutput();
    std::vector<std::vector<std::string>> resultData = resultMatrix.getData();

    DecryptionInputLayer* deInput = new DecryptionInputLayer(resultMatrix, keyMatrix); 
    AESDecryptionLayer* y = new AESDecryptionLayer(deInput, true);
    y = new AESDecryptionLayer(y);
    y = new AESDecryptionLayer(y);
    y = new AESDecryptionLayer(y);
    y = new AESDecryptionLayer(y);
    y = new AESDecryptionLayer(y);
    y = new AESDecryptionLayer(y);
    y = new AESDecryptionLayer(y);
    y = new AESDecryptionLayer(y);
    y = new AESDecryptionLayer(y);
    Matrix plainTextResultMatrix = y->GetOutput(); 


    // 6. Print the results (First row)
    std::cout << "SubBytes Output for first row:" << std::endl;
    for(int i = 0; i < 4; i++){
    	for(int j = 0; j < 4; j++) {
        	std::cout <<  binaryToHexString(resultData[j][i]);
    	} 
    }
    std::cout << std::endl;
    // Clean up memory
    delete x;
    delete input;

    return 0; 

    */
    
   }

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


// Encryption Input 
InputLayer::InputLayer(Matrix initialMatrix, Matrix initialKey) : staticData(initialMatrix) , staticKey(initialKey){}

Matrix InputLayer::GetOutput() {

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

Matrix InputLayer::GetKey(){
    
    int roundCoeffecient = RoundCoeffecient(0);
    std::string RCString = std::bitset<8>(roundCoeffecient).to_string();
    BinaryPolynomial RCPolynomial = BinaryPolynomial(RCString);

    return keyScheduleStep(staticKey, AESPolynomial, RCPolynomial);	     
}

int InputLayer::GetRound(){
    return 0;
}

std::vector<Matrix> InputLayer::GetAllKeys(){
    // Do nothing hehe 
}

// Decryption Input 

// Initialize with the starting matrix (e.g., the plaintext)
DecryptionInputLayer::DecryptionInputLayer(Matrix initialMatrix, Matrix initialKey): staticData(initialMatrix), staticKey(initialKey){
	    
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
Matrix DecryptionInputLayer::GetOutput() {

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

Matrix DecryptionInputLayer::GetKey(){

	return allKeys[GetRound()];
	     
}

std::vector<Matrix> DecryptionInputLayer::GetAllKeys(){
	return allKeys;
}

int DecryptionInputLayer::GetRound(){
	return 10;
}

// AES Layer 

AESLayer::AESLayer(Layer* previous, bool _disableMixColumn) {
	this->disableMixColumn = _disableMixColumn; 
        this->inputLayer = previous;
	//this->round = inputLayer->GetRound();
	round = inputLayer->GetRound(); 
	round = round + 1; 
	roundKey = inputLayer->GetKey(); 
}

std::vector<Matrix> AESLayer::GetAllKeys(){
	    // Also do nothing hehe 
};

int AESLayer::GetRound(){
	    //std::cout << " round : " << round << std::endl; 
	return round;
};

Matrix AESLayer::GetKey(){
	int roundCoeffecient = RoundCoeffecient(round);
	std::string RCString = std::bitset<8>(roundCoeffecient).to_string();
	BinaryPolynomial RCPolynomial = BinaryPolynomial(RCString);

	return keyScheduleStep(roundKey, AESPolynomial, RCPolynomial);
	   
}

Matrix AESLayer::GetOutput(){
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


// AES Decryption Layer 
AESDecryptionLayer::AESDecryptionLayer(Layer* previous, bool _disableMixColumn) {
	this->disableMixColumn = _disableMixColumn; 
        this->inputLayer = previous;
	//this->round = inputLayer->GetRound();
	round = inputLayer->GetRound(); 
	round = round - 1; 
	allKeys = inputLayer->GetAllKeys();
	roundKey = allKeys[round]; 
    };

int AESDecryptionLayer::GetRound(){
	    return round;
};

Matrix AESDecryptionLayer::GetKey() {
	    // hehe
};
    
std::vector<Matrix> AESDecryptionLayer::GetAllKeys(){
	    return allKeys;
};

Matrix AESDecryptionLayer::GetOutput(){

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


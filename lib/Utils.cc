#include "include/NumberSystems.h"
#include "include/XorTools.h"
#include "include/Utils.h"
#include "include/MathTools.h"

#include <charconv>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

float HammingDistanceBinaryString(const std::string& string1, const std::string& string2, bool normalized = false){
    std::string differing_bits = xorBinaryStrings(string1, string2);

    float distance = 0.0f; 

    for(unsigned char c : differing_bits){
        if (c == '1'){
            distance += 1.0f;
        }
    }

    if (normalized){
        distance = distance / string1.size();
    }

    return distance;

};

std::vector<std::string> transposedBlocksFromString(const std::string& inputString, int numGroups, size_t blockSize){
    std::vector<std::string> groups(numGroups);

    int groupId = 0;
    for(size_t i = 0; i < (inputString.size() + blockSize - 1) / blockSize; i++){
        //process string 
        std::string blockString = inputString.substr(i * blockSize, blockSize);
        //pad if ends with less characters than block size 
        if(blockString.size() < blockSize){
            std::string padding = ""; 
            for(int j = 1; j <= blockSize - blockString.size(); j++){
                padding += '0';
            }
            groups[groupId] += blockString + padding;
        }
        else{
            groups[groupId] += blockString;
        }
        groupId = (groupId + 1) % (numGroups); 
    }

    return groups;
};


std::string stringFromTransposedBlocks(const std::vector<std::string>& groups, size_t blockSize) {
    if (groups.empty() || groups[0].empty()) return "";

    std::string originalString = "";
    size_t numGroups = groups.size();
    
    // Calculate how many blocks are in each group. 
    // All groups should have the same length due to the original function's logic.
    size_t totalBlocksInFirstGroup = groups[0].size() / blockSize;

    for (size_t i = 0; i < totalBlocksInFirstGroup; ++i) {
        for (size_t g = 0; g < numGroups; ++g) {
            // Check if this group actually has a block at this index 
            // (In case the groups aren't perfectly uniform)
            if (i * blockSize < groups[g].size()) {
                originalString += groups[g].substr(i * blockSize, blockSize);
            }
        }
    }

    return originalString;
}

std::string readFileToString(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }   
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return content;
};

std::string shiftRight(const std::string& input, int shiftAmount) {
    if (input.empty()) return input; // Handle empty string case

    int n = input.size();
    // Normalize the shift amount to be within the bounds of the string length
    int effectiveShift = ((shiftAmount % n) + n) % n;

    if (effectiveShift == 0) return input; // No shift needed

    // Perform the right shift using string slicing
    return input.substr(n - effectiveShift) + input.substr(0, n - effectiveShift);
};


Matrix AESMatrixFromBinaryString(const std::string& binaryString){
	std::vector<std::vector<std::string>> dataMatrix(4, std::vector<std::string>(4, "00000000"));

	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			dataMatrix[j][i] = binaryString.substr(i * 32 + j * 8, 8);
		}
	}
	
	return Matrix(dataMatrix);
};

std::vector<std::string> splitByDelimiter(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    for (char c : str) {
        if (c == delimiter) {
            if (!token.empty()) {
                tokens.push_back(token);
                //std::cout << "Token : " << token << "\n";
                token.clear();
            }
        } else {
            token += c;
        }

    //print token for debugging 
    
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
};

std::vector<std::string> splitByChunks(const std::string& inputString, const int& chunkSize){    
    
    std::vector<std::string> outputChunks; 

    for (size_t i = 0; i < inputString.size(); i += chunkSize){
        std::string chunk = inputString.substr(i, chunkSize);

	outputChunks.push_back(chunk);
    }

    return outputChunks;
}

std::string PKCS7Pad(const std::string& plainText, const int& byteLength){

	std::string binaryInput = plainTextToBinaryString(plainText); 
	int padLength = (byteLength * 8 - (binaryInput.size() % (byteLength * 8))) / 8;
	
	std::string padding = ""; 

	std::stringstream ss; 
	ss << "\\0x" << std::hex << padLength; 
	std::string hexString = ss.str();

	for(int i = 0; i < padLength; i ++){
		padding += hexString; 
	}

	std::cout << padLength << std::endl; 

	return (plainText + padding); 

}

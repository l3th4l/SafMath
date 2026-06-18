#include <algorithm>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <climits>
#include "lib/include/AES.h"
#include "lib/include/Utils.h"
#include "lib/include/MathTools.h"
#include "lib/include/NumberSystems.h"

int main()
{
	// Step 1. Read file content 
    	std::string filename; 
    	std::cout << "Enter the filename:";
    	std::cin >> filename;

	std::string fileContent = readFileToString(filename);

	std::vector<std::string> lines = splitByDelimiter(fileContent, '\n');

	// Convert each lines to binary 
	
	std::vector<std::string> binaryLines;

	int maxNumHits = 0;
	int bestLine = 0; 

	for(int lineNum = 0; lineNum < lines.size(); lineNum++)
	{
		std::string binaryLine = hexStringToBinary(lines[lineNum]); 

		std::vector<std::string> chunks = splitByChunks(binaryLine, 128);

		int numHits = 0; 

		for(int i = 0; i < chunks.size(); i++){
			for(int j = 0; j < chunks.size(); j++){
				if(chunks[i] == chunks[j]){
					numHits += 1; 
				}
			}
		}

		std::cout << numHits - chunks.size() << std::endl; 

		if(numHits > maxNumHits)
		{
			maxNumHits = numHits; 
			bestLine = lineNum;
		}
	}
		// split into blocks of 128bits
	
	// calculate hamming distance between blocks 
	
	// the line with the least inter block hamming distance is likely the one encrypted in ecb mode 
	
	// i.e. there would be identical blocks 
	//
	std::cout << "Possible ECB : " << std::endl; 
	std::cout << "Line " << bestLine + 1 << " : " << lines[bestLine] << std::endl;
};

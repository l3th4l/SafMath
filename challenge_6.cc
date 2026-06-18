#include "lib/include/Utils.h"
#include "lib/include/NumberSystems.h"
#include "lib/include/XorTools.h"
#include <iostream>

int main(){
    // Step 1. Read file content 
    std::string filename; 
    std::cout << "Enter the filename:";
    std::cin >> filename; 

    std::string fileContent = readFileToString(filename);

    // step 2. Convert to binary string 
    std::string binaryString = base64ToBinary(fileContent); 

    //std::cout << "num bits " << binaryString.size() << std::endl;


    // step 3. guess key size by calculating the hamming distance 
    // between the first 4 blocks of the binary string for key sizes between 2 and 40.
    // Store the top key size with the smallest hamming distance.
    int bestKeySize = 0; 
    float bestDistance = static_cast<float>(INT_MAX);

    //std::cout << "Calculating best key size..." << std::endl;

    for(int keySize = 2; keySize <= binaryString.size() / 8 && keySize <= 40; keySize++){
        // get the first 4 blocks of the binary string for the current key size
        std::string block1 = binaryString.substr(0, keySize * 8);
        std::string block2 = binaryString.substr(keySize * 8, keySize * 8);
        std::string block3 = binaryString.substr(2 * keySize * 8, keySize * 8);
        std::string block4 = binaryString.substr(3 * keySize * 8, keySize * 8);

        // calculate the hamming distance between the blocks
        float distance12 = HammingDistanceBinaryString(block1, block2, true);
        float distance13 = HammingDistanceBinaryString(block1, block3, true);
        float distance14 = HammingDistanceBinaryString(block1, block4, true);
        float distance23 = HammingDistanceBinaryString(block2, block3, true);
        float distance24 = HammingDistanceBinaryString(block2, block4, true);
        float distance34 = HammingDistanceBinaryString(block3, block4, true);

        // average the distances
        float averageDistance = (distance12 + distance13 + distance14 + distance23 + distance24 + distance34) / 6.0f;

        // store the key size and its average distance
        if(averageDistance < bestDistance){
            bestDistance = averageDistance;
            bestKeySize = keySize;
        }

    }

    std::cout << "Best Key Size: " << bestKeySize << " with average normalized Hamming distance: " << bestDistance << "\n \n";
    // Step 4. split the binary string into blocks of the best key size and transpose the blocks.
    std::vector<std::string> transposedBlocks = transposedBlocksFromString(binaryString, bestKeySize, 8);
    // Step 5. solve each transposed block as if it was single-character XOR.
    std::vector<std::string> decryptedBlocks;
    for(const std::string& block : transposedBlocks){
        DecryptionResult decryptedBlock = frequencyAnalysis(block);
        decryptedBlocks.push_back(decryptedBlock.text);
    }
    //std::cout << "Decrypted blocks generated." << std::endl;
    // Step 6. Reconstruct original binary string from decrypted transposed blocks and convert to plaintext. 

    std::string decryptedPlaintext = stringFromTransposedBlocks(decryptedBlocks, 1);

    std::cout << "Decrypted Plaintext: \n \n " << decryptedPlaintext << std::endl;
}

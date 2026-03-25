#include "include/XorTools.h"
#include "include/NumberSystems.h"
#include <iostream>
//filesystem ops 
#include <fstream>
#include <sstream>
// error handling
#include <stdexcept>
#include <algorithm> // For sorting if you need it
#include <cfloat>    // For FLT_MAX

int main() {
    std::string plainText;
    std::string charsKey; 

    std::cout << "Enter the text to be encrypted: ";
    std::getline(std::cin, plainText); // Reads the whole line until you hit Enter
    std::cout << "Enter the key: ";
    std::cin >> charsKey; 

    //Convert PlainText and Characters to Binary 
    std::string textBinary = plainTextToBinaryString(plainText);
    std::string keysBinary = plainTextToBinaryString(charsKey);

    //Encrypt with repeating key XOR 

    std::string encryptedTextBinary = xorChunksWithMultipleKeys(textBinary, keysBinary, 8, true);
    std::string encryptedTextHex = binaryToHexString(encryptedTextBinary);

    std::cout << "Output Cypher in Hex : " << encryptedTextHex; 

    return 0;
}
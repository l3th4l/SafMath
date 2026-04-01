#ifndef NUMBERSYSTEMS_H
#define NUMBERSYSTEMS_H

#include <string>
#include <bitset>
#include <iostream>
#include <string>
#include <algorithm> // for transform

//Hex to Binary Conversion
std::string hexCharsToBinary(char c);
std::string hexStringToBinary(const std::string& hex);
std::string binaryToHexString(const std::string& binary);

//Binary to Base64 Conversion
std::string binaryToBase64(const std::string& binary);
std::string base64ToBinary(const std::string& base64);

//Plaintext to Binary 
std::string plainTextToBinaryString(const std::string& plainText);
std::string binaryStringToPlainText(const std::string& binaryString);

#endif // NUMBERSYSTEMS_H
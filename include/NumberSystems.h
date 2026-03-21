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

//Binary to Hex Conversion
std::string binaryToHexString(const std::string& binary);

//Binary to Base64 Conversion
std::string binaryToBase64(const std::string& binary);

//Base64 to Binary Conversion
std::string base64ToBinary(const std::string& base64);

#endif // NUMBERSYSTEMS_H
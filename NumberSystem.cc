#include "include/NumberSystems.h"

//Hex to Binary Conversion
std::string hexCharsToBinary(char c) {
    switch (toupper(c)) {
        case '0': return "0000"; case '1': return "0001";
        case '2': return "0010"; case '3': return "0011";
        case '4': return "0100"; case '5': return "0101";
        case '6': return "0110"; case '7': return "0111";
        case '8': return "1000"; case '9': return "1001";
        case 'A': return "1010"; case 'B': return "1011";
        case 'C': return "1100"; case 'D': return "1101";
        case 'E': return "1110"; case 'F': return "1111";
        default:  return "[Invalid]"; // Handle non-hex characters
    }
}

std::string hexStringToBinary(const std::string& hex) {
    std::string binaryResult = "";
    for (char c : hex) {
        binaryResult += hexCharsToBinary(c);
    }
    return binaryResult;
}

std::string binaryToHexString(const std::string& binary) {
    const std::string hexChars = "0123456789abcdef";
    std::string hexResult = "";

    // Process 4 bits at a time 
    for (size_t i = 0; i < binary.size(); i += 4) {
        std::string fourBits = binary.substr(i, 4);
        int hexValue = 0;
        for (char c : fourBits) {
            hexValue = hexValue * 2 + (c - '0');
        }
        hexResult += hexChars[hexValue];
    }

    return hexResult;
}

//Binary to Base64 Conversion
std::string binaryToBase64(const std::string& binary) {
    const std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string base64Result = "";
    
    for (size_t i = 0; i < binary.size(); i += 6) {
        std::string sixBits = binary.substr(i, 6);
        while (sixBits.size() < 6) {
            sixBits += "0"; // Pad with zeros if less than 6 bits
        }
        int index = std::stoi(sixBits, nullptr, 2); // Convert 6 bits to an integer
        base64Result += base64Chars[index]; // Map to Base64 character
    }

    return base64Result;
}

std::string base64ToBinary(const std::string& base64) {

    // Throw a not implemented error for now, as this function is not fully implemented yet
    // You can implement the actual conversion logic later, but for now, it will just return

    return "Not implemented yet";
}

std::string plainTextToBinaryString(const std::string& plainText) {
    std::string binary = "";
    for (unsigned char c : plainText) {
        binary += std::bitset<8>(c).to_string();
    }
    return binary;
}

std::string binaryStringToPlainText(const std::string& binaryString) {
    // Convert decryptedBinary to text (assuming ASCII)
    std::string plainText = "";
    for (size_t i = 0; i < binaryString.size(); i += 8) {
        std::string byte = binaryString.substr(i, 8);
        char character = static_cast<char>(std::bitset<8>(byte).to_ulong());
        plainText += character;
    }
    return plainText; 
}
#ifndef XOR_TOOLS_H
#define XOR_TOOLS_H

#include <string>
#include <bitset>
#include <vector>

std::string xorBinaryStrings(const std::string& bin1, const std::string& bin2);
std::string xorChunksWithSingleKey(const std::string& binary, const std::string& key, size_t chunkSize);
std::string xorChunksWithMultipleKeys(const std::string& binary, const std::string& keys, size_t chunkSize, bool repeatKeys);

// 1. Define the struct first
struct DecryptionResult {
    std::string text;
    float distance;
};

// 2. Declare the function using that struct as the return type
DecryptionResult frequencyAnalysis(const std::string& binary);

std::string decryptWithKnownPlaintext(const std::string& binary, const std::string& knownPlaintext);

#endif // XOR_TOOLS_H
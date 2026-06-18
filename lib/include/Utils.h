#ifndef UTILS_H
#define UTILS_H 

#include "MathTools.h"
#include <string>
#include <vector>


float HammingDistanceBinaryString(const std::string& string1, const std::string& string2, bool normalized);

std::vector<std::string> transposedBlocksFromString(const std::string& inputString, int numGroups, size_t blockSize);
std::string stringFromTransposedBlocks(const std::vector<std::string>& groups, size_t blockSize);

std::string readFileToString(const std::string& filename);
std::string shiftRight(const std::string& input, int shiftAmount);

Matrix AESMatrixFromBinaryString(const std::string& binaryString);
std::vector<std::string> splitByDelimiter(const std::string& str, char delimiter);
std::vector<std::string> splitByChunks(const std::string&, const int& chunkSize);

std::string PKCS7Pad(const std::string& plainText, const int& byteLength);

#endif

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

std::vector<std::string> splitByDelimiter(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    for (char c : str) {
        if (c == delimiter) {
            if (!token.empty()) {
                tokens.push_back(token);
                std::cout << "Token : " << token << "\n";
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


int main() {
    std::string filename;
    std::cout << "Enter the filename: ";
    std::cin >> filename;

    std::string fileContent = readFileToString(filename);
    std::vector<std::string> lines = splitByDelimiter(fileContent, '\n');

    std::vector<DecryptionResult> results;

    // 1. Process each line
    for (const std::string& line : lines) {
        if (line.empty()) continue; // Skip empty lines

        std::string binary = hexStringToBinary(line);
        
        // This now returns the struct {text, distance}
        DecryptionResult result = frequencyAnalysis(binary);
        
        // Append to vector
        results.push_back(result);
    }

    // 2. Find the overall best result (lowest distance)
    DecryptionResult* overallBest = nullptr;
    float minDistance = FLT_MAX;

    std::cout << "\n--- Analysis Results ---\n";
    for (auto& res : results) {
        // Optional: Print every line's best guess
        // std::cout << "Score: " << res.distance << " | Text: " << res.text << "\n";

        if (res.distance < minDistance) {
            minDistance = res.distance;
            overallBest = &res;
        }
    }

    // 3. Print the winner
    if (overallBest) {
        std::cout << "\n[ WINNER FOUND ]\n";
        std::cout << "Best Score: " << overallBest->distance << "\n";
        std::cout << "Decrypted Message: " << overallBest->text << "\n";
    } else {
        std::cout << "No valid results found.\n";
    }

    return 0;
}
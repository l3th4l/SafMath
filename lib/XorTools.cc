#include "include/XorTools.h"
#include "include/NumberSystems.h"

std::string xorBinaryStrings(const std::string& bin1, const std::string& bin2) {
    std::string result = "";
    for (size_t i = 0; i < bin1.size() && i < bin2.size(); ++i) {
        // XOR the bits and append to result
        // (0 or 1) ^ (0 or 1) + 48
        char xor_result = ((bin1[i] - '0') ^ (bin2[i] - '0')) + '0'; // XOR operation on characters '0' and '1' 
        result += xor_result;
    }
    return result;
};

std::string xorChunksWithSingleKey(const std::string& binary, const std::string& key, size_t chunkSize) {
    std::string result = "";
    for (size_t i = 0; i < binary.size(); i += chunkSize) {
        std::string chunk = binary.substr(i, chunkSize);
        result += xorBinaryStrings(chunk, key);
    }
    return result;
};

std::string xorChunksWithMultipleKeys(const std::string& binary, const std::string& keys, size_t chunkSize, bool repeatKeys) {
    std::string result = "";
    size_t keyIndex = 0;
    for (size_t i = 0; i < binary.size(); i += chunkSize) {
        std::string chunk = binary.substr(i, chunkSize);

        // get the chunk of key from the big string of keys 
        std::string keyChunk = keys.substr(keyIndex, chunkSize);

        result += xorBinaryStrings(chunk, keyChunk);

        if (repeatKeys) {
            keyIndex = (keyIndex + chunkSize) % keys.size(); // Move to the next key and wrap around if needed
        } else {
            keyIndex += chunkSize; // Move to the next key without wrapping
            if (keyIndex >= keys.size()) {
                break; // Stop if we run out of keys
            }
        }
    }
    return result;
};

// Letter frequency analysis for XOR cipher decryption, to assist in breaking simple XOR ciphers
// by analyzing the frequency of letters in the resulting plaintext.
DecryptionResult frequencyAnalysis(const std::string& binary) 
{   
    // Letters and their frequencies in English text (approximate)
    std::string letters = "ETAOINSHRDLUCMWFGYPBVKJXQZ";
    std::vector<float> englishMonograms(26, 0);
    // Fill the vector with known frequencies 
    englishMonograms[0] = 12.02;
    englishMonograms[1] = 9.10; 
    englishMonograms[2] = 8.12; 
    englishMonograms[3] = 7.68;
    englishMonograms[4] = 7.31;
    englishMonograms[5] = 6.95; 
    englishMonograms[6] = 6.28; 
    englishMonograms[7] = 6.02;
    englishMonograms[8] = 5.92; 
    englishMonograms[9] = 4.32; 
    englishMonograms[10] = 3.98;
    englishMonograms[11] = 2.88;
    englishMonograms[12] = 2.71;
    englishMonograms[13] = 2.61;
    englishMonograms[14] = 2.30;
    englishMonograms[15] = 2.11;
    englishMonograms[16] = 2.09;
    englishMonograms[17] = 2.03;
    englishMonograms[18] = 1.82;
    englishMonograms[19] = 1.49;
    englishMonograms[20] = 1.11;
    englishMonograms[21] = 0.69;
    englishMonograms[22] = 0.17;
    englishMonograms[23] = 0.11;
    englishMonograms[24] = 0.10;
    englishMonograms[25] = 0.07;
    
    // store top 5 decrypted texts with the least distance to the frequency distribution
    std::string bestDecryptedText = ""; // Placeholder for decrypted text after XOR operation
    float leastDistance = 0.0; // Placeholder for the least distance found in frequency analysis

    // Get the xor results for each possible key (single byte keys from 0 to 255)
    for (int key = 0; key < 256; ++key) {
        std::string keyBinary = std::bitset<8>(key).to_string();
        // TODO - Implement this for arbitrary key sizes, not just single byte keys
        std::string decryptedBinary = xorChunksWithSingleKey(binary, keyBinary, 8);

        // Convert decryptedBinary to text (assuming ASCII)
        std::string decryptedText = "";
        for (size_t i = 0; i < decryptedBinary.size(); i += 8) {
            std::string byte = decryptedBinary.substr(i, 8);
            char character = static_cast<char>(std::bitset<8>(byte).to_ulong());
            decryptedText += character;
        }

        // Find the frequency of letters in decryptedBinary (Only the ones in the 'letters' string)
        std::vector<float> decryptedFrequency(26, 0);
        for (char c : decryptedText) {
            char upperC = toupper(c);
            for (size_t j = 0; j < letters.size(); ++j) {
                if (letters[j] == upperC) {
                    decryptedFrequency[j]++;
                    break;
                }
            }
        }

         // 1. Calculate Monogram distance 
        float monogramDistance = 0.0;
        for (size_t j = 0; j < 26; ++j) {

            //ignore non-letter characters in the frequency analysis
            float decryptedRatio = (float)decryptedFrequency[j] / decryptedText.size();
             // Use a weighted distance metric, giving more weight to more common letters 
            // Use a weighted distance metric, giving more weight to more common letters
            monogramDistance += ((decryptedRatio - englishMonograms[j]) * (decryptedRatio - englishMonograms[j])) / englishMonograms[j];
        }

        // 2. Penalty/Reward System
        float penalty = 0.0;
        for (char c : decryptedText) {
            // Reward spaces!
            if (c == ' ') {
                penalty -= 10.0; 
            }
            // Heavily penalize non-printable "garbage" characters
            else if (!isprint(c) && !isspace(c)) {
                penalty += 50.0;
            }
        }
        
        float distance = (monogramDistance * 0.4f) + penalty;

        if (key == 0 || distance < leastDistance) {
            leastDistance = distance;
            bestDecryptedText = decryptedText; // Store the decrypted text with the least distance
        }
    }
    // Return the struct initialized with both values
    return {bestDecryptedText, leastDistance};
};


// this time, we know the first k characters of the plaintext, so we can use that 
// information to find the key more efficiently and accurately, by directly comparing the known plaintext with the decrypted text for each key.

std::string decryptWithKnownPlaintext(const std::string& binary, const std::string& knownPlaintext) {
    std::string bestDecryptedText = "";
    float leastDistance = 0.0;

    for (int key = 0; key < 256; ++key) {
        std::string keyBinary = std::bitset<8>(key).to_string();
        std::string decryptedBinary = xorChunksWithSingleKey(binary, keyBinary, 8);

        // Convert decryptedBinary to text (assuming ASCII)
        std::string decryptedText = "";
        for (size_t i = 0; i < decryptedBinary.size(); i += 8) {
            std::string byte = decryptedBinary.substr(i, 8);
            char character = static_cast<char>(std::bitset<8>(byte).to_ulong());
            decryptedText += character;
        }

        // Compare the known plaintext with the beginning of the decrypted text
        if (decryptedText.substr(0, knownPlaintext.size()) == knownPlaintext) {
            return decryptedText; // Return immediately if we find a match
        }
    }

    return bestDecryptedText; // Return the best decrypted text if no exact match is found
};
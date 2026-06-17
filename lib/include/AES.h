#ifndef AES_H 
#define AES_H

#include "MathTools.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>


class Layer {
public:
    virtual ~Layer() = default;
    
    // Every layer must implement this to process data from the previous layer
    virtual Matrix GetOutput() = 0;
    virtual Matrix GetKey() = 0;
    virtual std::vector<Matrix> GetAllKeys() = 0;
    virtual int GetRound() = 0;

    BinaryPolynomial AESPolynomial = BinaryPolynomial("100011011");

protected:
    Layer* inputLayer = nullptr; 
};

class InputLayer : public Layer {
private:

    Matrix staticData;
    Matrix staticKey; 

public:
    // Initialize with the starting matrix (e.g., the plaintext)
    InputLayer(Matrix initialMatrix, Matrix initialKey);
    Matrix GetOutput() override;
    Matrix GetKey() override;
    int GetRound() override;
    std::vector<Matrix> GetAllKeys() override;
};


// Decryption Input
class DecryptionInputLayer : public Layer {
private:

    Matrix staticData;
    Matrix staticKey; 

    std::vector<Matrix> allKeys; 

public:
    // Initialize with the starting matrix (e.g., the plaintext)
    DecryptionInputLayer(Matrix initialMatrix, Matrix initialKey);
    Matrix GetOutput() override;
    Matrix GetKey() override;
    int GetRound() override;
    std::vector<Matrix> GetAllKeys() override;
};


// AES Layer 
class AESLayer : public Layer {
private:
    int round; 
    Matrix roundKey = Matrix(4, 4);
    bool disableMixColumn; 

public:
    // Constructor to link this layer to the previous one
    AESLayer(Layer* previous, bool _disableMixColumn = false);
    Matrix GetOutput() override;
    Matrix GetKey() override;
    int GetRound() override;
    std::vector<Matrix> GetAllKeys() override;
};

// AES Decryption Layer 
class AESDecryptionLayer : public Layer
{
private: 
    int round; 
    std::vector<Matrix> allKeys;
    Matrix roundKey = Matrix(4, 4);
    bool disableMixColumn; 

public:
    // Constructor to link this layer to the previous one
    AESDecryptionLayer(Layer* previous, bool _disableMixColumn = false);
    Matrix GetOutput() override;
    Matrix GetKey() override;
    int GetRound() override;
    std::vector<Matrix> GetAllKeys() override;
};



BinaryPolynomial SBoxTransform(BinaryPolynomial AESPolynomial, BinaryPolynomial byte, bool inverse = false);
Matrix keyScheduleStep(Matrix& key, BinaryPolynomial& AESPolynomial, BinaryPolynomial& RC);
int RoundCoeffecient(int i);

std::string ECBDecrypt(const std::string& key, const std::string& plainText);

#endif

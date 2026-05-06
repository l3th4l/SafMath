#ifndef AES_H 
#define AES_H

#include "MathTools.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

BinaryPolynomial SBoxTransform(BinaryPolynomial AESPolynomial, BinaryPolynomial byte); 

#endif

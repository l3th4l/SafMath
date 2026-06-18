#include "lib/include/Utils.h"

int main(){

	std::string unpadded = "YELLOW ";
	std::string padded = PKCS7Pad(unpadded, 20);

	std::cout << "Unpadded string : " << unpadded << " Padded string : " << padded << std::endl; 
}

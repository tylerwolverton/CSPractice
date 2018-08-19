#include "Encoder.h"
#include "Decoder.h"
#include <string>
#include <iostream>

int main() 
{
    Encoder::Encode("./Test.txt", "./EncodedTest.txt");

	return 0;
}
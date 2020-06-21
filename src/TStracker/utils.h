#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <fstream>
#include <random>
#include "TiffWriter.h"
using namespace  std;
#define DEFAULT_FILENAME_LENGTH 5

// Generate a random string with given size
string random_string(size_t length = DEFAULT_FILENAME_LENGTH);


// Check if a file is already exist
inline bool exists_test(const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}

// Get name of a provided file with no duplication
string getNoNExistFileName(string fileName);

#endif // !_UTILS_H_

#include "utils.h"

// Generate a random string with given size
string random_string(size_t length)
{
	const string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	random_device random_device;
	mt19937 generator(random_device());
	uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

	string random_string;

	for (size_t i = 0; i < length; ++i)
	{
		random_string += CHARACTERS[distribution(generator)];
	}

	return random_string;
}


// Get name of a provided file with no duplication
string getNoNExistFileName(string fileName)
{
	if (exists_test(fileName + DEFAULT_EXTENSION))
	{
		// Already exist
		int counter = -1;
		while (exists_test(fileName + string("_") + to_string(++counter) + DEFAULT_EXTENSION));
		return fileName + string("_") + to_string(counter);
	}
	else
	{
		// Not exist
		return fileName;
	}
}
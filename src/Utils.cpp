#include <algorithm>
#include "Utils.h"

bool Utils::isNumeric(const string & s)
{
	return !s.empty() && find_if(s.begin(), s.end(), [](char c) { return !isdigit(c); }) == s.end();
}

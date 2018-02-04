#ifndef INCLUDE_UTILS_H
#define INCLUDE_UTILS_H

#include <limits>    // for std::numeric_limits
#include <stdexcept> // for std::overflow_error
#include <string>
#include <sstream>

using namespace std;

// convert usigned int to signed
int size_to_int(size_t u);
string intToString(int x);

#endif
#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <limits>    // for std::numeric_limits
#include <stdexcept> // for std::overflow_error
#include <string>
#include <sstream>

using namespace std;

// convert usigned int to signed
int size_to_int(size_t u);
string intToString(int x);

#endif /* UTILS_HPP_ */
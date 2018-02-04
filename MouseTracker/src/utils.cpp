#include <utils.hpp>

using namespace std;

// convert usigned int to signed
int size_to_int(size_t u)
{
	if (u > static_cast<size_t>(numeric_limits<int>::max()))
    {
        throw overflow_error(
            "size_t value cannot be stored in a variable of type int.");
    }

    return static_cast<int>(u);
}

string intToString(int x) {
	stringstream ss;
	ss << x;
	return ss.str();
}

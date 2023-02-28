// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

// Common #includes for convience.
#include <algorithm>
#include <cassert>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "TableEntry.h"
#include "Silly.h"

using namespace std;

int main(int argc, char* argv[]) {
	ios_base::sync_with_stdio(false); // you should already have this
	cin >> std::boolalpha; // add these two lines
	cout << std::boolalpha;

	Silly sql;
	sql.getMode(argc, argv);
	sql.start();
	cout << "Thanks for being silly!" << endl;
}
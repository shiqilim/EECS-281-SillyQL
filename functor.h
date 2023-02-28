// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include "TableEntry.h"
#include <iostream>
#include <string>

using namespace std;

template<typename T>
struct EqualFunc {
	int ind;
	T val;
	//constructor
	EqualFunc(int ind, T val) :ind(ind),  val(val) {};

	bool operator()(vector<TableEntry>& target) {
		return val == target[ind];
	}
};

template<typename T>
struct GreaterThan {
	int ind;
	T val;
	//constructor
	GreaterThan(int ind, T val) :ind(ind), val(val) {};

	bool operator()(vector<TableEntry>& target) {
		return target[ind] > val;
	}
};


template<typename T>
struct SmallerThan {
	int ind;
	T val;
	//constructor
	SmallerThan(int ind, T val) :ind(ind), val(val) {};

	bool operator()(vector<TableEntry>& target) {
		return target[ind] < val;
	}
};
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
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <getopt.h>

// header file created
#include "TableEntry.h"
#include "functor.h"

using namespace std;

struct Entry_type_index {
	EntryType type;
	uint32_t ind = 0;
};

struct Table {
	bool indexExist = false; //if it has index
	string index_col;
	vector<vector<TableEntry>> data; // vector store each tuples
	vector<EntryType> coltypes;
	unordered_map<string, Entry_type_index> entry; // colname to coltype & colindex
	map<TableEntry, vector<uint32_t>> bst;
	unordered_map<TableEntry, vector<uint32_t>> hash;
};

class Silly {
public:
	/*****************VARIABLES********************/
	bool quiet = false;
	string cmd;
	unordered_map<string, Table> database;

	/*****************GETMODE**********************/

	void getMode(int argc, char* argv[]) {
		opterr = false; // Let us handle all error output for command line options
		int choice;
		int option_index = 0;
		option long_options[] = {
			{"help",no_argument,nullptr,'h'},
			{"quiet",no_argument,nullptr,'q'},
			{ nullptr, 0, nullptr, '\0' }
		};

		while ((choice = getopt_long(argc, argv, "hq", long_options, &option_index)) != -1) {
			switch (choice) {
			case 'h':
				cout << "Usage of Commands:" << endl;
				cout << "CREATE: Creates a new table with <N> columns (where N > 0)." << endl;
				cout << "INSERT INTO: Inserts <N> new rows (where N > 0) into the table specified by <tablename>. " << endl;
				cout << "DELETE FROM: Deletes all rows from the table specified by <tablename> where the value of "
					<< "the entry in <colname> satisfies the operation <OP> with the given value <value>. " << endl;
				cout << "GENERATE INDEX: Directs the program to create an index of the type "
					<< "<indextype> on the column <colname> in the table< tablename > " << endl;
				cout << "PRINT: Directs the program to print the columns specified by <print_colname1>, <print_colname2>, ..."
					<< "< print_colnameN > from some / all rows in <tablename>. " << endl;
				cout << "JOIN: Directs the program to print the data in <N> columns, specified by <print_colname1>,"
					<< "< print_colname2 > , ... <print_colnameN>." << endl;
				cout << "REMOVE: Removes the table specified by <tablename> and all associated data from the database, including any created index." << endl;
				cout << "QUIT: Cleans up all internal data (i.e. no memory leaks) and exits the program. " << endl;
				exit(0);
				break;
			case 'q':
				quiet = true;
				break;
			default:
				cerr << "Error: Unknown command line option" << endl;
				exit(1);
			} // switch
		} // while
	}

	/******************COMMANDS********************/
	void create() {
		int num_col;
		string table_name;
		vector<string> colnames;
		cin >> table_name >> num_col;
		if (database.find(table_name) == database.end()) {
			for (int i = 0; i < num_col; ++i) {
				string coltype;
				cin >> coltype;
				if (coltype.at(0) == 's') {
					database[table_name].coltypes.push_back(EntryType::String);
				}
				else if (coltype.at(0) == 'i') {
					database[table_name].coltypes.push_back(EntryType::Int);
				}
				else if (coltype.at(0) == 'd') {
					database[table_name].coltypes.push_back(EntryType::Double);
				}
				else {
					database[table_name].coltypes.push_back(EntryType::Bool);
				}
			}
			for (int i = 0; i < num_col; ++i) {
				string colname;
				cin >> colname;
				colnames.push_back(colname);
				database[table_name].entry[colname].type = database[table_name].coltypes[i];
				database[table_name].entry[colname].ind = i;
			}

			database[table_name].coltypes.resize(num_col);
			cout << "New table " << table_name << " with column(s) ";
			for (string i: colnames) {
				cout << i << " ";
			}
			cout << "created\n";
		}
		else {
			cout << "Error during CREATE: Cannot create already existing table " << table_name << endl;
			getline(cin, cmd);
			return;
		}
	}

	void insert_into() {
		int num_rows;
		string table_name;
		cin >> cmd >> table_name >> num_rows >> cmd;

		if (database.find(table_name) != database.end()) {

			int start = static_cast<int>(database[table_name].data.size());
			int end = static_cast<int>(database[table_name].data.size() + num_rows - 1);
			int size = static_cast<int>(database[table_name].coltypes.size());

			database[table_name].data.reserve(size + num_rows);

			for (int i = 0; i < num_rows; ++i) {
				vector<TableEntry> v;
				v.reserve(size);
				for (int j = 0; j < size; ++j) {
					switch (database[table_name].coltypes[j]) {
					case EntryType::Bool: {
						bool d;
						cin >> d;
						v.emplace_back(d);
						break;
					}
					case EntryType::Double: {
						double d;
						cin >> d;
						v.emplace_back(d);
						break;
					}
					case EntryType::Int: {
						int d;
						cin >> d;
						v.emplace_back(d);
						break;
					}
					case EntryType::String: 
						string d;
						cin >> d;
						v.emplace_back(d);
						break;
					}
				}
				database[table_name].data.push_back(v);
			}
			cout << "Added " << num_rows << " rows to " << table_name << " from position " << start << " to " << end << endl;

			if (database[table_name].indexExist) {
				update_index(table_name, database[table_name].index_col);
			}
		}
		else {
			cout << "Error during INSERT: " << table_name << " does not name a table in the database" << endl;
			int size = static_cast<int>(database[table_name].coltypes.size());
			for (int i = 0; i < num_rows; ++i) {
				for (int j = 0; j < size; ++j) {
					string trash;
					cin >> trash;
				}
			}
			return;
		}
	}

	void remove_table() {
		string table_name;
		cin >> table_name;
		if (database.find(table_name) != database.end()) {
			database.erase(table_name);
			cout << "Table " << table_name << " deleted\n";
		}
		else {
			cout << "Error during REMOVE: " << table_name << " does not name a table in the database" << endl;
			return;
		}
	}

	void print() {
		//PRINT FROM <tablename> <N> <print_colname1> <print_colname2> ... <print_colnameN> [WHERE <colname> <OP> <value> | ALL ]
		int n;
		string table_name;
		vector<string> print_col_names;
		cin >> cmd >> table_name >> n;

		//check error
		if (database.find(table_name) == database.end()) {
			cout << "Error during PRINT: " << table_name << " does not name a table in the database" << endl;
			getline(cin, cmd);
			return;
		}
		//done checking

		print_col_names.resize(n);
		for (int i = 0; i < n; ++i) {
			cin >> cmd;
			print_col_names[i] = cmd;
			//check error
			if (database[table_name].entry.find(cmd) == database[table_name].entry.end()) {
				cout << "Error during PRINT: " << cmd << " does not name a column in " << table_name << endl;
				getline(cin, cmd);
				return;
			}
		}

		cin >> cmd;
		if (cmd == "WHERE") {
			char op;
			string col_name;
			cin >> col_name >> op;
			if (database[table_name].entry.find(col_name) == database[table_name].entry.end()) {
				cout << "Error during PRINT: " << col_name << " does not name a column in " << table_name << endl;
				getline(cin, cmd);
				return;
			}

			switch (database[table_name].entry[col_name].type) {
			case EntryType::Bool: {
				bool val;
				cin >> val;
				TableEntry temp(val);
				print_condition(table_name, print_col_names, op, col_name, temp);
				break;
			}
			case EntryType::Double: {
				double val;
				cin >> val;
				TableEntry temp(val);
				print_condition(table_name, print_col_names, op, col_name, temp);
				break;
			}
			case EntryType::Int: {
				int val;
				cin >> val;
				TableEntry temp(val);
				print_condition(table_name, print_col_names, op, col_name, temp);
				break;
			}
			case EntryType::String:
				string val;
				cin >> val;
				TableEntry temp(val);
				print_condition(table_name, print_col_names, op, col_name, temp);
				break;
			}
		}
		else if (cmd == "ALL") {
			print_all(table_name, print_col_names, n);
		}
	}

	void delete_row() {
		char op;
		string table_name, colname;
		cin >> cmd >> table_name; 

		//check error
		if (database.find(table_name) == database.end()) {
			cout << "Error during DELETE: " << table_name << " does not name a table in the database" << endl;
			getline(cin, cmd);
			return;
		}

		cin >> cmd >> colname;

		if (database[table_name].entry.find(colname) == database[table_name].entry.end()) {
			cout << "Error during DELETE: " << colname << " does not name a column in " << table_name << endl;
			getline(cin, cmd);
			return;
		}

		cin >> op;

		uint32_t size_before = static_cast<uint32_t>(database[table_name].data.size());

		EntryType target_type = database[table_name].entry[colname].type;
		int target_ind = database[table_name].entry[colname].ind;

		vector<vector<TableEntry>>::iterator trash;

		switch (target_type) {
		case EntryType::Bool: {
			bool val;
			cin >> val;
			if (op == '=') {
				EqualFunc<bool> comparator(target_ind, val);
				trash = remove_if(database[table_name].data.begin(), database[table_name].data.end(), comparator);
			}
			else if (op == '<') {
				SmallerThan<bool> comparator(target_ind, val);
				trash = remove_if(database[table_name].data.begin(), database[table_name].data.end(), comparator);
			}
			else if (op == '>') {
				GreaterThan<bool> comparator(target_ind, val);
				trash = remove_if(database[table_name].data.begin(), database[table_name].data.end(), comparator);
			}
			break;
		}
		case EntryType::Double: {
			double val;
			cin >> val;
			if (op == '=') {
				EqualFunc<double> comparator(target_ind, val);
				trash = remove_if(database[table_name].data.begin(), database[table_name].data.end(), comparator);
			}
			else if (op == '<') {
				SmallerThan<double> comparator(target_ind, val);
				trash = remove_if(database[table_name].data.begin(), database[table_name].data.end(), comparator);
			}
			else if (op == '>') {
				GreaterThan<double> comparator(target_ind, val);
				trash = remove_if(database[table_name].data.begin(), database[table_name].data.end(), comparator);
			}
			break;
		}
		case EntryType::Int: {
			int val;
			cin >> val;
			if (op == '=') {
				EqualFunc<int> comparator(target_ind, val);
				trash = remove_if(database[table_name].data.begin(), database[table_name].data.end(), comparator);
			}
			else if (op == '<') {
				SmallerThan<int> comparator(target_ind, val);
				trash = remove_if(database[table_name].data.begin(), database[table_name].data.end(), comparator);
			}
			else if (op == '>') {
				GreaterThan<int> comparator(target_ind, val);
				trash = remove_if(database[table_name].data.begin(), database[table_name].data.end(), comparator);
			}
			break;
		}
		case EntryType::String:
			string val;
			cin >> val;
			if (op == '=') {
				EqualFunc<string> comparator(target_ind, val);
				trash = remove_if(database[table_name].data.begin(), database[table_name].data.end(), comparator);
			}
			else if (op == '<') {
				SmallerThan<string> comparator(target_ind, val);
				trash = remove_if(database[table_name].data.begin(), database[table_name].data.end(), comparator);
			}
			else if (op == '>') {
				GreaterThan<string> comparator(target_ind, val);
				trash = remove_if(database[table_name].data.begin(), database[table_name].data.end(), comparator);
			}
			break;
		}
		database[table_name].data.erase(trash, database[table_name].data.end());
		uint32_t size_after = static_cast<uint32_t>(database[table_name].data.size());

		cout << "Deleted " << size_before - size_after << " rows from " << table_name << endl;

		if (database[table_name].indexExist) {
			update_index(table_name, database[table_name].index_col);
		}
	}

	void generate_index() {
		string table_name, index_type, col_name;
		cin >> cmd >> table_name;

		//check error - table name
		if (database.find(table_name) == database.end()) {
			cout << "Error during GENERATE: " << table_name << " does not name a table in the database" << endl;
			getline(cin, cmd);
			return;
		}

		cin >> index_type >> cmd >> cmd >> col_name;

		//check error - col name
		if (database[table_name].entry.find(col_name) == database[table_name].entry.end()) {
			cout << "Error during GENERATE: " << col_name << " does not name a column in " << table_name << endl;
			return;
		}

		//If an index is requested on a table that already has one, 
		//discard the old index before building the new one.
		if (database[table_name].indexExist) {
			database[table_name].bst.clear();
			database[table_name].hash.clear();
		}

		uint32_t target_ind = find_index(table_name, col_name);
		int size = static_cast<int>(database[table_name].data.size());
		database[table_name].index_col = col_name;

		if (index_type == "bst") {
			for (int i = 0; i < size; i++) {
				database[table_name].bst[database[table_name].data[i][target_ind]].push_back(i);
			}
		}
		else {
			for (int i = 0; i < size; i++) {
				database[table_name].hash[database[table_name].data[i][target_ind]].push_back(i);
			}
		}

		database[table_name].indexExist = true;
		cout << "Created " << index_type << " index for table " << table_name << " on column " << col_name << endl;
	}

	void join() {
		//JOIN <tablename1> AND <tablename2> WHERE <colname1> = <colname2> AND PRINT <N> <print_colname1> <1 | 2> <print_colname2> <1 | 2> ... <print_colnameN> <1 | 2>
		int n;
		string table_name_1, table_name_2, colname1, colname2;
		vector<string> print_names_vec;
		vector<uint32_t> table_num_vec;
		
		cin >> table_name_1;
		//check if table 1 exists
		if (database.find(table_name_1) == database.end()) {
			cout << "Error during JOIN: " << table_name_1 << " does not name a table in the database" << endl;
			getline(cin, cmd);
			return;
		}//done checking

		cin >> cmd >> table_name_2;
		//check if table 2 exist
		if (database.find(table_name_2) == database.end()) {
			cout << "Error during JOIN: " << table_name_2 << " does not name a table in the database" << endl;
			getline(cin, cmd);
			return;
		}//done checking

		cin >> cmd >> colname1;
		//check if colname exist in table 1
		if (database[table_name_1].entry.find(colname1) == database[table_name_1].entry.end()) {
			cout << "Error during JOIN: " << colname1 << " does not name a column in " << table_name_1 << endl;
			getline(cin, cmd);
			return;
		}//done checking

		cin >> cmd >> colname2;
		//check if colname exist in table 2
		if (database[table_name_2].entry.find(colname2) == database[table_name_2].entry.end()) {
			cout << "Error during JOIN: " << colname2 << " does not name a column in " << table_name_2 << endl;
			getline(cin, cmd);
			return;
		}//done checking

		cin >> cmd >> cmd >> n;
		for (int i = 0; i < n; i++) {
			uint32_t table_num;
			string print_name;
			cin >> print_name >> table_num;

			//check if print column exist in the table
			if (table_num == 1) {
				if (database[table_name_1].entry.find(print_name) == database[table_name_1].entry.end()) {
					cout << "Error during JOIN: " << print_name << " does not name a column in " << table_name_1 << endl;
					getline(cin, cmd);
					return;
				}
			}
			else {
				if (database[table_name_2].entry.find(print_name) == database[table_name_2].entry.end()) {
					cout << "Error during JOIN: " << print_name << " does not name a column in " << table_name_2 << endl;
					getline(cin, cmd);
					return;
				}
			}//done checking

			print_names_vec.push_back(print_name);
			table_num_vec.push_back(table_num);
		}
		print_names_vec.resize(n);
		table_num_vec.resize(n);

		//if not quiet, print colnames
		if (!quiet) {
			for (int i = 0; i < static_cast<int>(print_names_vec.size()); i++) {
				cout << print_names_vec[i] << " ";
			}
			cout << endl;
		}

		uint32_t target_col_ind_1 = find_index(table_name_1, colname1);
		uint32_t target_col_ind_2 = find_index(table_name_2, colname2);

		uint32_t count = 0;

		//create hash index for table 2
		unordered_map<TableEntry, vector<uint32_t>> hash_table_2;
		for (int i = 0; i < static_cast<int>(database[table_name_2].data.size()); i++) {
			hash_table_2[database[table_name_2].data[i][target_col_ind_2]].push_back(i);
		}

		for (vector<TableEntry>& table : database[table_name_1].data) {
			for (uint32_t& hash : hash_table_2[table[target_col_ind_1]]) {
				if (!quiet) {
					for (int i = 0; i < static_cast<int>(print_names_vec.size()); i++) {
						if (table_num_vec[i] == 1) {
							cout << table[find_index(table_name_1, print_names_vec[i])] << " ";
						}
						else {
							cout << database[table_name_2].data[hash][find_index(table_name_2, print_names_vec[i])] << " ";
						}
					}
					cout << endl;
				}
				count++;
			}
		}

		cout << "Printed " << count << " rows from joining " << table_name_1 << " to " << table_name_2 << endl;
	}

	void start() {
		do {
			cout << "% ";
			cin >> cmd;
			if (cmd == "CREATE") {
				create();
			}
			else if (cmd == "INSERT") {
				insert_into();
			}
			else if (cmd == "DELETE") {
				delete_row();
			}
			else if (cmd == "GENERATE") {
				generate_index();
			}
			else if (cmd == "PRINT") {
				print();
			}
			else if (cmd == "JOIN") {
				join();
			}
			else if (cmd == "REMOVE") {
				remove_table();
			}
			else if (cmd.at(0) == '#') {
				getline(cin, cmd);
			}
			else if (cmd == "QUIT") {
				return;
			}
			else {
				getline(cin, cmd);
				cout << "Error: unrecognized command" << endl;
			}
		} while (cmd != "QUIT");
	}

	/*******************Helper function******************/

	uint32_t find_index(string& table_name, string& col_name) {
		return database[table_name].entry[col_name].ind;
	}

	vector<uint32_t>& get_row(string& table_name, const TableEntry& val) {
		if (!database[table_name].bst.empty()) {
			return database[table_name].bst[val];
		}
		else {
			return database[table_name].hash[val];
		}
	}

	void print_all(string& table_name, vector<string>& print_col_names, int n) {
		vector<uint32_t> index;
		if (!quiet) {
			for (string i : print_col_names) {
				cout << i << " ";
				uint32_t ind = find_index(table_name, i);
				index.push_back(ind);
			}
			cout << endl;

			for (int i = 0; i < static_cast<int>(database[table_name].data.size()); i++) {
				for (int j = 0; j < n; j++) {
					cout << database[table_name].data[i][index[j]] << " ";
				}
				cout << endl;
			}
		}

		cout << "Printed " << database[table_name].data.size() << " matching rows from " << table_name << endl;
	}

	void print_condition(string& table_name, vector<string>& print_col_names, char& op, string& target_col, const TableEntry& val) {
		vector<uint32_t> index;
		if (!quiet) {
			for (string i : print_col_names) {
				cout << i << " ";
				uint32_t ind = find_index(table_name, i);
				index.push_back(ind);
			}
			cout << endl;
		}

		int target_ind = find_index(table_name, target_col);

		switch (op) {
		case'=':
			if (database[table_name].indexExist && !database[table_name].bst.empty() && database[table_name].index_col == target_col) {
				print_condition_equal_bst(table_name, index, val);
			}
			else {
				print_condition_equal(table_name, index, target_ind, val);
			}
			break;
		case'<':
			if (database[table_name].indexExist && !database[table_name].bst.empty() && database[table_name].index_col == target_col ) {
				print_condition_lessThan_bst(table_name, val, index);
			}
			else {
				print_condition_lessThan(table_name, index, target_ind, val);
			}
			break;
		case'>':
			if (database[table_name].indexExist && !database[table_name].bst.empty() && database[table_name].index_col == target_col) {
				print_condition_greaterThan_bst(table_name, val, index);
			}
			else {
				print_condition_greaterThan(table_name, index, target_ind, val);
			}
			break;
		}
	}

	void print_condition_equal_bst(string& table_name, vector<uint32_t>& print_index, const TableEntry& val) {
		vector<uint32_t> row_ind_to_print = get_row(table_name, val);
		int row_size = static_cast<int>(row_ind_to_print.size());
		int col_size = static_cast<int>(print_index.size());

		if (!quiet) {
			for (int i = 0; i < row_size; i++) {
				int row = row_ind_to_print[i];
				for (int j = 0; j < col_size; j++) {
					int col = print_index[j];
					cout << database[table_name].data[row][col] << " ";
				}
				cout << endl;
			}
		}
		cout << "Printed " << row_size << " matching rows from " << table_name << endl;
	}

	void print_condition_equal(string& table_name, vector<uint32_t>& print_index, int target_ind, const TableEntry& val) {
		int row_size = static_cast<int>(database[table_name].data.size());
		int print_size = static_cast<int>(print_index.size());
		int count = 0;

		for (int i = 0; i < row_size; i++) {
			if (database[table_name].data[i][target_ind] == val) {
				if (!quiet) {
					for (int j = 0; j < print_size; j++) {
						cout << database[table_name].data[i][print_index[j]] << " ";
					}
					cout << endl;
				}
				count++;
			}
		}
		cout << "Printed " << count << " matching rows from " << table_name << endl;
	}

	void print_condition_lessThan_bst(string& table_name, const TableEntry& val, vector<uint32_t>& print_index) {
		uint32_t count = 0;
		map<TableEntry, vector<uint32_t>>::iterator lower_b = database[table_name].bst.lower_bound(val);

		for (auto it = database[table_name].bst.begin(); it != lower_b; it++) {
			for (int i = 0; i < static_cast<int>(it->second.size()); i++) {
				if (!quiet) {
					int row = static_cast<int>(it->second[i]);
					for (int j = 0; j < static_cast<int>(print_index.size()); j++) {
						int col = static_cast<int>(print_index[j]);
						cout << database[table_name].data[row][col] << " ";
					}
					cout << endl;
				}
				count++;
			}
		}

		cout << "Printed " << count << " matching rows from " << table_name << endl;
	}

	void print_condition_lessThan(string& table_name, vector<uint32_t>& print_index, int target_ind, const TableEntry& val) {
		int row_size = static_cast<int>(database[table_name].data.size());
		int print_size = static_cast<int>(print_index.size());
		int count = 0;

		for (int i = 0; i < row_size; i++) {
			if (database[table_name].data[i][target_ind] < val) {
				if (!quiet) {
					for (int j = 0; j < print_size; j++) {
						cout << database[table_name].data[i][print_index[j]] << " ";
					}
					cout << endl;
				}
				count++;
			}
		}
		cout << "Printed " << count << " matching rows from " << table_name << endl;
	}

	void print_condition_greaterThan_bst(string& table_name, const TableEntry& val, vector<uint32_t>& print_index) {
		uint32_t count = 0;
		map<TableEntry, vector<uint32_t>>::iterator upper_b = database[table_name].bst.upper_bound(val);

		for (auto it = upper_b; it != database[table_name].bst.end(); it++) {
			for (int i = 0; i < static_cast<int>(it->second.size()); i++) {
				if (!quiet) {
					int row = static_cast<int>(it->second[i]);
					for (int j = 0; j < static_cast<int>(print_index.size()); j++) {
						int col = static_cast<int>(print_index[j]);
						cout << database[table_name].data[row][col] << " ";
					}
					cout << endl;
				}
				count++;
			}
		}

		cout << "Printed " << count << " matching rows from " << table_name << endl;
	}

	void print_condition_greaterThan(string& table_name, vector<uint32_t>& print_index, int target_ind, const TableEntry& val) {
		int row_size = static_cast<int>(database[table_name].data.size());
		int print_size = static_cast<int>(print_index.size());
		int count = 0;

		for (int i = 0; i < row_size; i++) {
			if (database[table_name].data[i][target_ind] > val) {
				if (!quiet) {
					for (int j = 0; j < print_size; j++) {
						cout << database[table_name].data[i][print_index[j]] << " ";
					}
					cout << endl;
				}
				count++;
			}
		}
		cout << "Printed " << count << " matching rows from " << table_name << endl;
	}

	void update_index(string& table_name, string& col_name) {
		if (!database[table_name].bst.empty()) {
			database[table_name].bst.clear();

			int size = static_cast<int>(database[table_name].data.size());
			uint32_t target_ind = find_index(table_name, col_name);

			database[table_name].index_col = col_name;
			for (int i = 0; i < size; i++) {
				database[table_name].bst[database[table_name].data[i][target_ind]].push_back(i);
			}
		}
		else {
			database[table_name].hash.clear();

			int size = static_cast<int>(database[table_name].data.size());
			uint32_t target_ind = find_index(table_name, col_name);

			database[table_name].index_col = col_name;
			for (int i = 0; i < size; i++) {
				database[table_name].hash[database[table_name].data[i][target_ind]].push_back(i);
			}
		}
	}
}; 

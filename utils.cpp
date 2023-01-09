#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "utils.h"

using std::cerr;
using std::cout;
using std::ifstream;
using std::string;
using std::vector;
using std::endl;
using std::ios_base;

void die(const string& msg) {
	cerr << "Error: " << msg << endl;
#ifdef _DEBUG
	abort();
#else
	exit(1);
#endif
}

void load_file(const string& file, string& str) {
	//Helps to the program to locate the file regardless of whether it's in the same folder or not via the specified directory
	std::string dir = "";
	for (int i = 0; i < 3; i++) //Tries 3 times if it doesn't work throws the error below
	{
		ifstream f(dir + file, ios_base::binary);
		if (!f.good()) {
			dir = "../" + dir;
			continue; //Breaks this iteration to try again
		}

		// Looks for the end of the program in order to find the length
		f.seekg(0, ios_base::end);
		const size_t filelen = f.tellg();

		vector<char> buffer(filelen);

		//Go to start
		f.seekg(0);

		//Read from the file
		f.read(buffer.data(), filelen);

		//Assign beggining and end to the string via the buffer
		str.assign(buffer.begin(), buffer.end());

		return;
	}

	die("Unable to find " + file); //If unable to find the file throw an error
}

void Look_File(string& str) {
	//Read File into str
	load_file("UserHash.txt", str);
}

void show_context(const string& str, Position pos) {
	const int width = 76;
	int end = 0;
	int leftpos;
	int rightpos;

	Position len = str.size();
	Position i = pos;

	//Show context shows all characters within the three colons either side of each line

	while (end < 3)
	{
		i--;
		char c = str[i];
		if (c == 58) //58 is the acii number for the colon
		{
			end++;
		}
		leftpos = i;
	}
	Position left = leftpos;

	end = 0;
	i = pos;

	while (end < 3)
	{
		i++;
		char c = str[i];
		if (c == 58)
		{
			end++;
		}
		rightpos = i+1;
	}
	Position right = rightpos;

	for (Position i = left; i < right; ++i) 
	{
		if (i < 0 || i >= len) {
			cout << ' ';
			continue;
		}
		char c = str[i];
		if (c >= 32 && c < 128) //Readable/normal characters 32-128
		{ 
			cout << c; //Print that character
		}
		else
		{
			left++; 
			//Accounts for missing characters so the pointer to the
			//first character in the pattern is still in the right place, see below
		}
	}
	cout << endl;
	for (Position i = left; i < right; ++i) {
		if (i < pos) {
			cout << ' ';
		}
		else if (i == pos) {
			cout << "^ " << pos;
		}
	}
	cout << endl;
}
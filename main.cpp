#include <cassert>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "utils.h"
#include <stdio.h>
#include <windows.h>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <thread>

//Init values
#define INIT_A 0x67452301
#define INIT_B 0xefcdab89
#define INIT_C 0x98badcfe
#define INIT_D 0x10325476

#define SQRT_2 0x5a827999
#define SQRT_3 0x6ed9eba1

unsigned int nt_buffer[16];
unsigned int output[4];
char hex_format[33];
char itoa16[17] = "0123456789ABCDEF";

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using namespace std;
typedef std::chrono::steady_clock the_clock;

string NTLM(char* key)
{
	/*
	------------------------------------------------------------------------------------------------------------------------------------------------
	| The code in this function is not mine, all credit goes to [Mustafa Chelik] - https://www.codeproject.com/Articles/328761/NTLM-Hash-Generator |
	------------------------------------------------------------------------------------------------------------------------------------------------
	
	Some minor alterations have been made but otherwise this function is entirely his work

	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 Prepare the string for hash calculation
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	*/
	int i = 0;
	int length = strlen(key);
	memset(nt_buffer, 0, 16 * 4);
	//The length of key need to be <= 27
	for (; i < length / 2; i++)
		nt_buffer[i] = key[2 * i] | (key[2 * i + 1] << 16);

	//padding
	if (length % 2 == 1)
		nt_buffer[i] = key[length - 1] | 0x800000;
	else
		nt_buffer[i] = 0x80;
	//put the length
	nt_buffer[14] = length << 4;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// NTLM hash calculation
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	unsigned int a = INIT_A;
	unsigned int b = INIT_B;
	unsigned int c = INIT_C;
	unsigned int d = INIT_D;

	/* Round 1 */
	a += (d ^ (b & (c ^ d))) + nt_buffer[0]; a = (a << 3) | (a >> 29);
	d += (c ^ (a & (b ^ c))) + nt_buffer[1]; d = (d << 7) | (d >> 25);
	c += (b ^ (d & (a ^ b))) + nt_buffer[2]; c = (c << 11) | (c >> 21);
	b += (a ^ (c & (d ^ a))) + nt_buffer[3]; b = (b << 19) | (b >> 13);

	a += (d ^ (b & (c ^ d))) + nt_buffer[4]; a = (a << 3) | (a >> 29);
	d += (c ^ (a & (b ^ c))) + nt_buffer[5]; d = (d << 7) | (d >> 25);
	c += (b ^ (d & (a ^ b))) + nt_buffer[6]; c = (c << 11) | (c >> 21);
	b += (a ^ (c & (d ^ a))) + nt_buffer[7]; b = (b << 19) | (b >> 13);

	a += (d ^ (b & (c ^ d))) + nt_buffer[8]; a = (a << 3) | (a >> 29);
	d += (c ^ (a & (b ^ c))) + nt_buffer[9]; d = (d << 7) | (d >> 25);
	c += (b ^ (d & (a ^ b))) + nt_buffer[10]; c = (c << 11) | (c >> 21);
	b += (a ^ (c & (d ^ a))) + nt_buffer[11]; b = (b << 19) | (b >> 13);

	a += (d ^ (b & (c ^ d))) + nt_buffer[12]; a = (a << 3) | (a >> 29);
	d += (c ^ (a & (b ^ c))) + nt_buffer[13]; d = (d << 7) | (d >> 25);
	c += (b ^ (d & (a ^ b))) + nt_buffer[14]; c = (c << 11) | (c >> 21);
	b += (a ^ (c & (d ^ a))) + nt_buffer[15]; b = (b << 19) | (b >> 13);

	/* Round 2 */
	a += ((b & (c | d)) | (c & d)) + nt_buffer[0] + SQRT_2; a = (a << 3) | (a >> 29);
	d += ((a & (b | c)) | (b & c)) + nt_buffer[4] + SQRT_2; d = (d << 5) | (d >> 27);
	c += ((d & (a | b)) | (a & b)) + nt_buffer[8] + SQRT_2; c = (c << 9) | (c >> 23);
	b += ((c & (d | a)) | (d & a)) + nt_buffer[12] + SQRT_2; b = (b << 13) | (b >> 19);

	a += ((b & (c | d)) | (c & d)) + nt_buffer[1] + SQRT_2; a = (a << 3) | (a >> 29);
	d += ((a & (b | c)) | (b & c)) + nt_buffer[5] + SQRT_2; d = (d << 5) | (d >> 27);
	c += ((d & (a | b)) | (a & b)) + nt_buffer[9] + SQRT_2; c = (c << 9) | (c >> 23);
	b += ((c & (d | a)) | (d & a)) + nt_buffer[13] + SQRT_2; b = (b << 13) | (b >> 19);

	a += ((b & (c | d)) | (c & d)) + nt_buffer[2] + SQRT_2; a = (a << 3) | (a >> 29);
	d += ((a & (b | c)) | (b & c)) + nt_buffer[6] + SQRT_2; d = (d << 5) | (d >> 27);
	c += ((d & (a | b)) | (a & b)) + nt_buffer[10] + SQRT_2; c = (c << 9) | (c >> 23);
	b += ((c & (d | a)) | (d & a)) + nt_buffer[14] + SQRT_2; b = (b << 13) | (b >> 19);

	a += ((b & (c | d)) | (c & d)) + nt_buffer[3] + SQRT_2; a = (a << 3) | (a >> 29);
	d += ((a & (b | c)) | (b & c)) + nt_buffer[7] + SQRT_2; d = (d << 5) | (d >> 27);
	c += ((d & (a | b)) | (a & b)) + nt_buffer[11] + SQRT_2; c = (c << 9) | (c >> 23);
	b += ((c & (d | a)) | (d & a)) + nt_buffer[15] + SQRT_2; b = (b << 13) | (b >> 19);

	/* Round 3 */
	a += (d ^ c ^ b) + nt_buffer[0] + SQRT_3; a = (a << 3) | (a >> 29);
	d += (c ^ b ^ a) + nt_buffer[8] + SQRT_3; d = (d << 9) | (d >> 23);
	c += (b ^ a ^ d) + nt_buffer[4] + SQRT_3; c = (c << 11) | (c >> 21);
	b += (a ^ d ^ c) + nt_buffer[12] + SQRT_3; b = (b << 15) | (b >> 17);

	a += (d ^ c ^ b) + nt_buffer[2] + SQRT_3; a = (a << 3) | (a >> 29);
	d += (c ^ b ^ a) + nt_buffer[10] + SQRT_3; d = (d << 9) | (d >> 23);
	c += (b ^ a ^ d) + nt_buffer[6] + SQRT_3; c = (c << 11) | (c >> 21);
	b += (a ^ d ^ c) + nt_buffer[14] + SQRT_3; b = (b << 15) | (b >> 17);

	a += (d ^ c ^ b) + nt_buffer[1] + SQRT_3; a = (a << 3) | (a >> 29);
	d += (c ^ b ^ a) + nt_buffer[9] + SQRT_3; d = (d << 9) | (d >> 23);
	c += (b ^ a ^ d) + nt_buffer[5] + SQRT_3; c = (c << 11) | (c >> 21);
	b += (a ^ d ^ c) + nt_buffer[13] + SQRT_3; b = (b << 15) | (b >> 17);

	a += (d ^ c ^ b) + nt_buffer[3] + SQRT_3; a = (a << 3) | (a >> 29);
	d += (c ^ b ^ a) + nt_buffer[11] + SQRT_3; d = (d << 9) | (d >> 23);
	c += (b ^ a ^ d) + nt_buffer[7] + SQRT_3; c = (c << 11) | (c >> 21);
	b += (a ^ d ^ c) + nt_buffer[15] + SQRT_3; b = (b << 15) | (b >> 17);

	output[0] = a + INIT_A;
	output[1] = b + INIT_B;
	output[2] = c + INIT_C;
	output[3] = d + INIT_D;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Convert the hash to hex (for being readable)
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	for (i = 0; i < 4; i++)
	{
		int j = 0;
		unsigned int n = output[i];
		//iterate the bytes of the integer		
		for (; j < 4; j++)
		{
			unsigned int convert = n % 256;
			hex_format[i * 8 + j * 2 + 1] = itoa16[convert % 16];
			convert = convert / 16;
			hex_format[i * 8 + j * 2 + 0] = itoa16[convert % 16];
			n = n / 256;
		}
	}

	string hash = hex_format; //hex_format contains the NTLM hash

	for_each(hash.begin(), hash.end(), [](char& x)
		{
			x = ::tolower(x);
		}
	);
	return hash;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// End of the function, all code below is mine
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Position find_bm_multi(const string& pat, const string& text, std::vector<int>& positions) {
	Position pat_len = pat.size();
	Position text_len = text.size();

	int skip[256];

	for (int i = 0; i < 256; i++)
	{
		skip[i] = pat_len; //Set all values to pattern length (Not in the pattern)
	}

	for (int i = 0; i < pat_len; i++)
	{
		skip[int(pat[i])] = (pat_len - 1) - i; //Tells us which characters are in the pattern and how far to jump based on which characters
	}

	for (int i = 0; i < text_len - pat_len; ++i)
	{

		int s = skip[int(text[i + pat_len - 1])];
		if (s != 0)
		{
			i += s - 1; //Skip ahead by value in skip array
		}

		int j;

		for (j = 0; j < pat_len; j++) //Check to see that the pattern matches the text
		{
			if (text[i + j] != pat[j])
			{
				break; 
			}
		}

		if (j == pat_len) //If j is the same length as the pattern they matched
		{
			positions.push_back(i); //Matches then add position to the postion vector
		}
	}
	if (positions.empty())
	{
		return -1; //No positions found
	}
	else
	{
		return 0;
	}
}

Position find_RK_multi(const string& pat, const string& text, std::vector<int>& positions) {
	Position pat_len = pat.size();
	Position text_len = text.size();

	const int p = 11; //Prime number for modulo
	int h = 1;
	int ht = 0; //Text hash
	int hp = 0; //Pattern hash
	
	int i, j;


	for (i = 0; i < pat_len - 1; i++)
	{
		h = (h * 256) % p; //Calculating the value of h(Modulo helps to prevent integer overflow)
	}

	for (i = 0; i < pat_len; i++)
	{
		hp = (256 * hp + pat[i]) % p; //Stores the hash of the pattern
		ht = (256 * ht + text[i]) % p; //Stores the hash of the Text
	}
	
	for (i = 0; i <= text_len - pat_len; i++) //Slides along the text
	{
		if (hp == ht) //If both hashes are equal
		{
			for (j = 0; j < pat_len; j++) //Check to see that the pattern matches the text
			{
				if (text[i + j] != pat[j])
				{
					break;
				}
			}
			if (j == pat_len) //If j is the same length as the pattern they matched
			{
				positions.push_back(i); //Matches then add position to the postion vector
			}
		}

		if (i < text_len - pat_len)
		{
			ht = (256 * (ht - text[i] * h) + text[i + pat_len]) % p; //Removes the trailer character of the hash and adds the leading character

			//If the value of ht is negative then change it to positive

			if (ht < 0)
				ht = (ht + p);
		}
	}

	if (positions.empty())
	{
		return -1; // Not found.
	}
	else
	{
		return 0;
	}
}

int main() {
	double Dif;
	double val1;
	double val2;
	double total_Dif{};
	double total_val1{};
	double total_val2{};
	string str;
	string hash;
	int x;
	long long time_taken; //Hold value for the time taken to complete the Rabbin Karp Algorithm
	long long time_taken2; //Hold value for the time taken to complete the Boore Moyre Algorithm

	for (int i = 0; i < 10; i++)
	{

		for (x = 0; x < 2; x++) //Iterates both methods
		{
			ifstream file("cain.txt"); //Loads file containing common passwords
			if (!file.good())
			{
				die("Unable to Locate cain.txt"); //Breaks this iteration to try again
			}

			string text; //Created to hold the contents of the .txt file
			Look_File(text); //Looks for the file
			load_file("UserHash.txt", text); //Loads the file into text string

			the_clock::time_point start = the_clock::now(); //Starts the timer

			while (getline(file, str))//Iterates the password dictionary line by line
			{

				char* char_arr; //Char pointer

				char_arr = &str[0]; //Sets char pointer to string so that it can be used in the NTLM parameters

				hash = NTLM(char_arr); //Converts the string of text into an NTLM haSH



				vector<int> positions; //Vector holds the postions of the hashes that match

				if (x == 0)
				{
					find_RK_multi(hash, text, positions); //Runs the Rabbin Karp Method on the first iteration
				}
				if (x == 1) //No else if to make overhead consistent
				{
					find_bm_multi(hash, text, positions); //Runs the Boyer Moore Method on the second iteration
				}

				for (int i = 0; i < positions.size(); i++) //Iterates through all the positions found for that particular hash
				{
					cout << "Found '" << hash << "' at position " << positions[i] << endl; //Prints the hash
					show_context(text, positions[i]); //Shows context
					cout << "Password = " << str << endl << endl; //Prints the corresponding password
				}
			}

			the_clock::time_point end = the_clock::now(); //Ends the timer

			if (x == 0)
			{
				time_taken = duration_cast<milliseconds>(end - start).count(); //Puts the RB time into time taken
			}
			if (x == 1)
			{
				time_taken2 = duration_cast<milliseconds>(end - start).count(); //Puts the BM time into time taken 2
			}
		}
		//Prints time taken for both algorithms
		cout << "It took " << time_taken << "ms/" << time_taken / 1000 << "s for the Rabbin Karp method" << endl;
		cout << "It took " << time_taken2 << "ms/" << time_taken2 / 1000 << "s for the Roger Boore method" << endl;

		//Intialiases some variables to hold values (Convert to double) to allow for division
		val1 = time_taken;
		val2 = time_taken2;

		if (time_taken < time_taken2)
		{
			//Calculates percentage
			Dif = (val2 / val1) * 100.00;

			cout << "The Rabbin karp method was " << Dif << "% faster" << endl;
		}
		else
		{
			//Calculates percentage
			Dif = (val1 / val2) * 100.00;
			cout << "The Boyer Moore method was " << Dif << "% faster" << endl;
		}

		total_Dif = total_Dif + Dif;
		total_val1 = total_val1 + val1;
		total_val2 = total_val2 + val2;
	}

	cout << "It took " << total_val1/10.00 << "ms/" << (total_val1 / 1000)/10.00 << "s for the Rabbin Karp method" << endl;
	cout << "It took " << total_val2/10.00 << "ms/" << (total_val2 / 1000)/10.00 << "s for the Roger Boore method" << endl;

	if (total_val1 < total_val2)
	{
		cout << "The Rabbin karp method was " << total_Dif/10.00 << "% faster" << endl;
	}
	else
	{
		//Calculates percentage
		cout << "The Boyer Moore method was " << total_Dif/10.00 << "% faster" << endl;
	}


	return 0;
}
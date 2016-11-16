
#ifndef function_hpp
#define function_hpp
#include<unordered_set>
#include<unordered_map>
#include <stdio.h>
#include <string>
#include<vector>
#include<sstream>
#include <fstream>
#include<iostream>
#include <curl/curl.h>
#include<algorithm>
using namespace std;
bool isValidWord(const string& word, const unordered_set<char>& notInLetter, const unordered_map<int, char>& pos_inLetter, const int wordLen);
string getNextLetter(const unordered_map<char, double>& letter_freq);
void buildDicts(vector<string>& dicts, vector<long long>& counts, vector<double>& freq);
void fetchInitialEvi(vector<int>& e_wordLen, int& life, const string initialUrl, string& token);
string getNextGuess(const vector<unordered_set<char> >& e_notInLetter, const vector<unordered_map<int, char> >& e_pos_inLetter, const vector<string>& dicts, const vector<double>& freq, const vector<int>& e_wordLen);
string getJson(const string& all, const string& name);
void pushGuessUpDateEvi(const string& nextUrl, vector<unordered_set<char> >& e_notInLetter, vector<unordered_map<int, char> >& e_pos_inLetter, vector<int>& e_wordLen, int& life, bool& success, const string& guess);
#endif /* function_hpp */

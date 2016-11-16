#include <iostream>
#include<sstream>
#include <vector>
#include <fstream>
#include <tuple>
#include <math.h>
#include<vector>
#include<limits.h>
#include<set>
#include<math.h>
#include<queue>
#include<stack>
#include"function.hpp"
using namespace std;

int main(int argc, const char * argv[]) {
    
    vector<string> dicts;
    vector<long long> counts;
    vector<double> freq;
    buildDicts(dicts, counts, freq);
    while(1) {
        string initialUrl = "http://gallows.hulu.com/play?code=liyinan1001@gmail.com";
        string token;
        vector<int> e_wordLen;
        int life;
        fetchInitialEvi(e_wordLen, life, initialUrl, token);
        vector<unordered_set<char> > e_notInLetter(e_wordLen.size());
        vector<unordered_map<int, char> > e_pos_inLetter(e_wordLen.size());
        while(life > 0) {
            string guess = getNextGuess(e_notInLetter, e_pos_inLetter, dicts, freq, e_wordLen);
            string nextUrl = "http://gallows.hulu.com/play?code=liyinan1001@gmail.com&token=" + token + "&guess=" + guess;
            bool success = false;
            pushGuessUpDateEvi(nextUrl, e_notInLetter, e_pos_inLetter, e_wordLen, life, success, guess);
            if(success || life == 0) {
                cout << "success? " << success << endl;
                break;
            }
        }
    }
    return 0;
}



#include "function.hpp"
using namespace std;
void buildDicts(vector<string>& dicts, vector<long long>& counts, vector<double>& freq) {
    string filePath = "./count_1w.txt";
    ifstream fin(filePath);
    long long sum = 0;
    while(!fin.eof()) {
        string word;
        long long count;
        fin >> word >> count;
        sum += count;
        transform(word.begin(), word.end(), word.begin(), ::toupper);
        dicts.push_back(word);
        counts.push_back(count);
    }
    long long n = dicts.size();
    for(long long i = 0; i < n; i++) {
        freq.push_back(counts[i] * 1.0 / sum);
    }
    fin.close();
}
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
void downloadUrl(const string& url, string& content) {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}
string getJson(const string& all, const string& name) {
    if(name == "remaining_guesses") {
        size_t nameEndPos = all.find(name) + name.length();
        size_t valStart = all.find(":", nameEndPos + 1) + 1;
        size_t valEnd = all.find(",", nameEndPos + 1) - 1;
        return all.substr(valStart, valEnd - valStart + 1);
    }
    else {
        size_t nameEndPos = all.find(name) + name.length();
        size_t valStart = all.find("\"", nameEndPos + 1) + 1;
        size_t valEnd = all.find("\"", valStart) - 1;
        return all.substr(valStart, valEnd - valStart + 1);
    }
}
void fetchInitialEvi(vector<int>& e_wordLen, int& life, const string initialUrl, string& token) {
    string content;
    downloadUrl(initialUrl, content);
    if(getJson(content, "status") == "DEAD") {life = 0; return;}
    token = getJson(content, "token");
    life = stoi(getJson(content, "remaining_guesses"));
    string sentence = getJson(content, "state");
    int wordLen = 0;
    for(int i = 0; i < sentence.length(); i++) {
        if(sentence[i] == '_') {
            wordLen++;
            if(i + 1 == sentence.length()) {e_wordLen.push_back(wordLen);}
        }
        else if(sentence[i] != '_' && wordLen != 0) {
            e_wordLen.push_back(wordLen);
            wordLen = 0;
        }/*
        else {  //punctuation
            if(sentence[i] == '\'' && i + 1 < sentence.length() && sentence[i + 1] == '_') {hasS = true;}  // I assume When ' appear, only s or nothing is followed.
            if(wordLen != 0) {
                e_wordLen.push_back(wordLen);
                wordLen = 0;
                i++;
            }
        }*/
        
    }
}
string getNextGuess(const vector<unordered_set<char> >& e_notInLetter, const vector<unordered_map<int, char> >& e_pos_inLetter, const vector<string>& dicts, const vector<double>& freq, const vector<int>& e_wordLen) {
    unordered_map<char, double> letter_freq;
    for(int i = 0; i < e_wordLen.size(); i++) {
        double validWordFreqSum = 0;
        for(int idict = 0; idict < dicts.size(); idict++) {
            validWordFreqSum += isValidWord(dicts[idict], e_notInLetter[i], e_pos_inLetter[i], e_wordLen[i]) ? freq[i] : 0;
        }
        for(int idict = 0; idict < dicts.size(); idict++) {
            string dictWord = dicts[idict];
            if(isValidWord(dictWord, e_notInLetter[i], e_pos_inLetter[i], e_wordLen[i])) {
                for(int wordPos = 0; wordPos < dictWord.length(); wordPos++) {
                    letter_freq[dictWord[wordPos]] += e_pos_inLetter[i].find(wordPos) == e_pos_inLetter[i].end() ? freq[idict] / validWordFreqSum : 0;
                }
            }
        }
    }
    return getNextLetter(letter_freq);
}
void pushGuessUpDateEvi(const string& nextUrl, vector<unordered_set<char> >& e_notInLetter, vector<unordered_map<int, char> >& e_pos_inLetter, vector<int>& e_wordLen, int& life, bool& success, const string& guess) {
    string content;
    for(int j = 0; j < e_wordLen.size(); j++) {
        e_notInLetter[j].insert(guess[0]);
    }
    downloadUrl(nextUrl, content);
    string sentence = getJson(content, "state");
    if(getJson(content, "status") == "DEAD") {cout << sentence << endl; life = 0; return;}
    life = stoi(getJson(content, "remaining_guesses"));
    
    
    if(getJson(content, "status") == "FREE") {cout << sentence << endl; success = true; return;}
    int pos = 0;
    int iWord = 0;
    while(pos < sentence.length()) {
        while(sentence[pos] != '_' && !(sentence[pos] <= 'Z' && sentence[pos] >= 'A')) {pos++;}
        for(int i = 0; i < e_wordLen[iWord]; i++) {
            if(sentence[pos] <= 'Z' && sentence[pos] >= 'A') {
                e_pos_inLetter[iWord][i] = sentence[pos];
            }
            pos++;
        }
        //while(pos < sentence.length() && sentence[pos] != ' ') {pos++;} // jump over possible 'S
        iWord++;
    }
}

bool isValidWord(const string& word, const unordered_set<char>& notInLetter, const unordered_map<int, char>& pos_inLetter, const int wordLen) {
    if(word.length() != wordLen) {return false;}
    for(int i = 0; i < word.size(); i++) {
        if(pos_inLetter.find(i) != pos_inLetter.end()) {
            if(word[i] != pos_inLetter.at(i)) {return false;}
        }
        else {
            if(notInLetter.find(word[i]) != notInLetter.end()) {return false;}
        }
    }
    return true;
}
string getNextLetter(const unordered_map<char, double>& letter_freq) {
    char maxLetter = 'A';
    double maxFreq = 0;
    for(auto pair : letter_freq) {
        if(pair.second > maxFreq) {
            maxLetter = pair.first;
            maxFreq = pair.second;
        }
    }
    string res;
    res += maxLetter;
    return res;
}

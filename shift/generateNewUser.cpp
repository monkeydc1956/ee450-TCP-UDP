#include <cstring>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
using namespace std;

char offsetChar(char c) {
    if (c >= 'a' && c <= 'z') {
        return 'a' + (c - 'a' + 5) % 26;
    } else if (c >= 'A' && c <= 'Z') {
        return 'A' + (c - 'A' + 5) % 26;
    } else if (c >= '0' && c <= '9') {
        return '0' + (c - '0' + 5) % 10;
    } else {
        return c;
    }
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n");
    if (first == string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n");
    return str.substr(first, last - first +1);
}

string confusionCipherEncrypt(const string& plaintext) {
    string ciphertext = plaintext;

    string allCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    unsigned int seed = 123; 
    std::random_device rd;
    std::mt19937 g(seed);
    std::shuffle(allCharacters.begin(), allCharacters.end(), g);


    std::unordered_map<char, char> charMap;
    for (size_t i = 0; i < allCharacters.length(); ++i) {
        charMap[allCharacters[i]] = allCharacters[(i + 1) % allCharacters.length()];
    }


    for (char& c : ciphertext) {
        if (charMap.find(c) != charMap.end()) {
            c = charMap[c];
        }
    }

    return ciphertext;
}


string encrypt(const string& input) {
    cout<<input.length()<<endl;
    string res;
    for (char c : input) {
        res += offsetChar(c);
    }
    cout<<res.length()<<endl;
    return res;
}

unordered_map<string, string> readUserInfo(string filename){
    ifstream file(filename);
    unordered_map<string, string> userInfo;
    string line;
    if (file.is_open()){
        while(getline(file, line)){
            if (!file.eof()) {
                stringstream ss(line);
                string username, passcode;
                getline(ss, username, ',');
                getline(ss, passcode);
                string tmpcode = passcode.erase(0, 1);
                tmpcode.erase(tmpcode.length()-1, 1);
                userInfo[username] = tmpcode;
            }else{
                stringstream ss(line);
                string username, passcode;
                getline(ss, username, ',');
                getline(ss, passcode);
                string tmpcode = passcode.erase(0, 1);
                userInfo[username] = tmpcode; 
            }
        }
        file.close();
    }else{
        cout<<"Unable to open "<<filename<<endl;
    }
    return userInfo;
}

int saveMap(unordered_map<string, string> info){
    ofstream outFile("member.txt");
    if (!outFile.is_open()) {
        cout << "Error opening file!" << endl;
        return 1;
    }
    for (const auto& pair : info) {
        outFile << pair.first << "," <<" "<< pair.second<<endl;
    }
    outFile.close();
    return 0;
}

int main(){
    unordered_map<string, string> userInfo = readUserInfo("member_unencrypted.txt");
    unordered_map<string, string> enuserInfo; 
    for (const auto& pair : userInfo) {
        cout << "Key: " << pair.first << ", Value: " << pair.second.length() << endl;
        string new_user = confusionCipherEncrypt(pair.first);
        string new_pass = confusionCipherEncrypt(pair.second);
        enuserInfo[new_user] = new_pass;
    }
    saveMap(enuserInfo);
    return 0;
}
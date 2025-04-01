#include <iostream>
#include <vector>
#include <string>

using namespace std;

void keyScramble(vector<int> &S, const string &key) {
    for (int i = 0; i < 256; i++) S[i] = i;

    int j = 0;
    for (int i = 0; i < 256; i++) {
        j = (j + S[i] + key[i % key.length()]) % 256;
        swap(S[i], S[j]);
    }
}

string rc4crypt(const string &input, const string &key) {
    vector<int> S(256);
    keyScramble(S, key);
    
    int i = 0, j = 0;
    string output;
    output.reserve(input.size());
    
    for (char c : input) {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        swap(S[i], S[j]);
        char keystreamByte = S[(S[i] + S[j]) % 256];
        output += c ^ keystreamByte;
    }
    
    return output;
}

int main() {
    string key, plaintext;
    
    cout << "Enter plaintext: ";
    getline(cin, plaintext);
    
    cout << "Enter key: ";
    getline(cin, key);
    
    string ciphertext = rc4crypt(plaintext, key);
    
    cout << "Ciphertext (hex): ";
    for (unsigned char c : ciphertext)
        cout << hex << (int)(unsigned char)c << " ";
    cout << endl;
    
    string decrypted = rc4crypt(ciphertext, key);
    cout << "Decrypted: " << decrypted << endl;
    
    return 0;
}

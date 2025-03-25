#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <cstdlib>

using namespace std;

struct Node {
    char ch;
    Node *left, *right;
    Node() : ch('\0'), left(nullptr), right(nullptr) {}
};

void insertCode(Node* root, char ch, const string &code) {
    Node* curr = root;
    for (char bit : code) {
        if (bit == '0') {
            if (!curr->left) curr->left = new Node();
            curr = curr->left;
        } else {
            if (!curr->right) curr->right = new Node();
            curr = curr->right;
        }
    }
    curr->ch = ch;
}

int main() {
    // Read Huffman codes from file to rebuild tree
    ifstream codeFile("huffman_codes.txt");
    if (!codeFile.is_open()) {
        cerr << "Error opening huffman_codes.txt for reading.\n";
        return EXIT_FAILURE;
    }
    
    Node* root = new Node();
    string line;
    while (getline(codeFile, line)) {
        istringstream iss(line);
        int ascii;
        string code;
        if (!(iss >> ascii >> code)) continue;
        char ch = char(ascii);
        insertCode(root, ch, code);
    }
    codeFile.close();

    // Read encoded message from file
    ifstream encodedFile("encoded.txt");
    if (!encodedFile.is_open()) {
        cerr << "Error opening encoded.txt for reading.\n";
        return EXIT_FAILURE;
    }
    string encodedMessage;
    getline(encodedFile, encodedMessage);
    encodedFile.close();
    
    // Decode the message
    string decodedMessage;
    Node* curr = root;
    for (char bit : encodedMessage) {
        if (bit == '0') {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
        if (!curr->left && !curr->right) { // leaf node
            decodedMessage.push_back(curr->ch);
            curr = root;
        }
    }
    
    // Write decoded message to file
    ofstream decodedFile("decoded.txt");
    if (decodedFile.is_open()) {
        decodedFile << decodedMessage;
        decodedFile.close();
    } else {
        cerr << "Unable to open decoded.txt for writing.\n";
    }
    
    cout << "Decoded Message: " << decodedMessage << "\n";
    cout << "Decoding completed. See decoded.txt for output.\n";
    
    return 0;
}
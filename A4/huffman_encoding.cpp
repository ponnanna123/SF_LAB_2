#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <sstream>

using namespace std;

struct Node {
    char ch;
    int freq;
    Node *left, *right;
    Node(char ch, int freq) : ch(ch), freq(freq), left(nullptr), right(nullptr) {}
    Node(Node *left, Node *right) : ch('\0'), freq(left->freq + right->freq), left(left), right(right) {}
};

struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

Node* buildHuffmanTree(unordered_map<char, int>& freq) {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }
    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* parent = new Node(left, right);
        pq.push(parent);
    }
    return pq.top();
}

void generateCodes(Node* root, const string &code, unordered_map<char, string>& codes) {
    if (!root) return;
    if (!root->left && !root->right) {
        codes[root->ch] = (code != "" ? code : "1"); // single-character message edge case
    }
    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

int main() {
    cout << "Enter the message to encode: ";
    string message;
    getline(cin, message);
    
    unordered_map<char, int> freq;
    for (char ch : message) {
        freq[ch]++;
    }

    Node* root = buildHuffmanTree(freq);
    unordered_map<char, string> codes;
    generateCodes(root, "", codes);

    stringstream encodedStream;
    for (char ch : message) {
        encodedStream << codes[ch];
    }
    string encodedString = encodedStream.str();
    
    // Write encoded message to file
    ofstream encodedFile("encoded.txt");
    if (encodedFile.is_open()) {
        encodedFile << encodedString;
        encodedFile.close();
    } else {
        cerr << "Unable to open file for writing encoded message.\n";
    }
    
    // Write Huffman codes (ASCII value and its code) to file
    ofstream codeFile("huffman_codes.txt");
    if (codeFile.is_open()) {
        for (auto pair : codes) {
            codeFile << int(pair.first) << " " << pair.second << "\n";
        }
        codeFile.close();
    } else {
        cerr << "Unable to open file for writing Huffman codes.\n";
    }
    
    cout << "Encoded Message: " << encodedString << "\n";
    cout << "Encoding completed. See encoded.txt and huffman_codes.txt for output.\n";
    
    return 0;
}
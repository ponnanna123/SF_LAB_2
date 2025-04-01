#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

int modExp(int base, int exp, int mod) {
    int result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

int modInverse(int num, int mod) {
    return modExp(num, mod - 2, mod);
}

int generateRandom(int max) {
    return rand() % (max - 1) + 1;
}

void elGamalKeyGen(int& p, int& g, int& x, int& y) {
    cout << "Enter a prime number (p): ";
    cin >> p;
    cout << "Enter a generator (g): ";
    cin >> g;

    srand(time(0));
    x = generateRandom(p - 1);
    y = modExp(g, x, p);
}

void elGamalEncrypt(int p, int g, int y, int m, int& c1, int& c2) {
    srand(time(0));
    int k = generateRandom(p - 1);
    c1 = modExp(g, k, p);
    c2 = (modExp(y, k, p) * m) % p;
}

int elGamalDecrypt(int p, int x, int c1, int c2) {
    int s = modExp(c1, x, p);
    int sInverse = modInverse(s, p);
    return (c2 * sInverse) % p;
}

int main() {
    int p, g, x, y;
    int m;
    int c1, c2;

    elGamalKeyGen(p, g, x, y);
    cout << "Public Key: (p=" << p << ", g=" << g << ", y=" << y << ")" << endl;
    cout << "Private Key: x=" << x << endl;

    cout << "Enter the message to encrypt (m): (m must br smaller than p)" << endl;
    cin >> m;

    if (m >= p) {
        cout << "Error: Message must be smaller than the prime number p." << endl;
        return 1;
    }

    elGamalEncrypt(p, g, y, m, c1, c2);
    cout << "Encrypted values: c1=" << c1 << ", c2=" << c2 << endl;

    int decryptedMessage = elGamalDecrypt(p, x, c1, c2);
    cout << "Decrypted message: " << decryptedMessage << endl;

    return 0;
}

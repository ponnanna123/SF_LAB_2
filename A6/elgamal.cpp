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

void elGamalKeyGen(int& p, int& g, int& a, int& h) {
    cout << "Enter a prime number (p): ";
    cin >> p;
    cout << "Enter a generator (g): ";
    cin >> g;

    srand(time(0));
    a = generateRandom(p - 1);
    h = modExp(g, a, p);
}

void elGamalEncrypt(int p, int g, int h, int m, int& c1, int& c2) {
    srand(time(0));
    int k = generateRandom(p - 1);
    c1 = modExp(g, k, p);
    c2 = (modExp(h, k, p) * m) % p;
}

int elGamalDecrypt(int p, int a, int c1, int c2) {
    int s = modExp(c1, a, p);
    int sInverse = modInverse(s, p);
    return (c2 * sInverse) % p;
}

int main() {
    int p, g, a, h;
    int m;
    int c1, c2;

    elGamalKeyGen(p, g, a, h);
    cout << "Public Key: (p=" << p << ", g=" << g << ", h=" << h << ")" << endl;
    cout << "Private Key: a=" << a << endl;

    cout << "Enter the message to encrypt (m): ";
    cin >> m;

    if (m >= p) {
        cout << "Error: Message must be smaller than the prime number p." << endl;
        return 1;
    }

    elGamalEncrypt(p, g, h, m, c1, c2);
    cout << "Encrypted values: c1=" << c1 << ", c2=" << c2 << endl;

    int decryptedMessage = elGamalDecrypt(p, a, c1, c2);
    cout << "Decrypted message: " << decryptedMessage << endl;

    return 0;
}

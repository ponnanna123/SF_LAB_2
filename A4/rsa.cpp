#include <bits/stdc++.h>

using namespace std;

int mod_exp(int base, int expo, int m)
{
    int res = 1;
    base = base % m;
    while (expo > 0)
    {
        if (expo & 1)
            res = (1LL * res * base) % m;
        base = (1LL * base * base) % m;
        expo = expo / 2;
    }
    return res;
}

int mod_inv(int e, int phi)
{
    for (int d = 2; d < phi; d++)
    {
        if ((e * d) % phi == 1)
            return d;
    }
    return -1;
}

void key_gen(int &e, int &d, int &n, int &p, int &q)
{
    n = p * q;
    int phi = (p - 1) * (q - 1);

    for (e = 2; e < phi; e++)
        if (__gcd(e, phi) == 1)
            break;

    d = mod_inv(e, phi);
}

void encrypt(vector<int> &M, vector<int> &C, int &e, int &n) {
    for (auto &c : M) {
        C.push_back(mod_exp(c, e, n));
    }
}

void decrypt(vector<int> &C, vector<int> &D, int &d, int &n) {
    for (auto &c : C) {
        D.push_back(mod_exp(c, d, n));
    }
}

int main(int argc, char *argv[])
{
    int e, d, n, p, q;
    cout << "Enter two prime numbers: ";
    cin >> p >> q;

    key_gen(e, d, n, p, q);

    cout << "\nPublic Key (n, e): (" << n << ", " << e << ")\n";
    cout << "Private Key (n, d): (" << n << ", " << d << ")\n\n";

    string msg;
    cout << "Enter the message: ";
    cin.ignore();
    getline(cin, msg);

    vector<int> M, C, D;
    for (auto &c : msg) {
        M.push_back(int(c));
    }

    encrypt(M, C, e, n);
    cout << "Encrypted Message: ";
    for (auto &c : C) cout << char(c);
    cout << endl;

    decrypt(C, D, d, n);
    cout << "Decrypted Message: ";
    for (auto &c : D) cout << char(c);
    cout << endl;

    return 0;
}

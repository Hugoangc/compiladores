#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

bool isInteger(const string& str) {
    size_t start = (str[0] == '-' ? 1 : 0);  // Começa no índice 1 se houver um sinal '-'
    for (size_t i = start; i < str.size(); ++i) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

bool isFloat(const string& str) {
    size_t start = (str[0] == '-' ? 1 : 0);
    bool pointFound = false;
    for (size_t i = start; i < str.size(); ++i) {
        if (str[i] == '.') {
            if (pointFound) return false;
            pointFound = true;
        } else if (!isdigit(str[i])) {
            return false;
        }
    }
    return pointFound;
}

bool isHexadecimal(const string& str) {
    if (str.size() < 3 || str[0] != '0' || (str[1] != 'x' && str[1] != 'X')) return false;
    for (size_t i = 2; i < str.size(); ++i) {
        if (!isxdigit(str[i])) return false;
    }
    return true;
}

string checkNumberType(const string& str) {
    size_t dotCount = count(str.begin(), str.end(), '.');
    if (dotCount > 1) {
        throw invalid_argument("Erro: Mais de um ponto na string.");
    }

    if (isHexadecimal(str)) return "Hexadecimal";
    if (isInteger(str)) return "Inteiro";
    if (isFloat(str)) return "Float";

    return "Tipo de número desconhecido";
}

int main() {
    string a = "10.5.5";
    string b = "17.5";
    string c = "10";
    string d = "0x1A3F";
    string e = "15a";

    try {
        cout << "String a: " << a << " é " << checkNumberType(a) << endl;
    } catch (const invalid_argument& e) {
        cout << e.what() << endl;
    }

    try {
        cout << "String b: " << b << " é " << checkNumberType(b) << endl;
    } catch (const invalid_argument& e) {
        cout << e.what() << endl;
    }

    try {
        cout << "String c: " << c << " é " << checkNumberType(c) << endl;
    } catch (const invalid_argument& e) {
        cout << e.what() << endl;
    }

    try {
        cout << "String d: " << d << " é " << checkNumberType(d) << endl;
    } catch (const invalid_argument& e) {
        cout << e.what() << endl;
    }
    
    try {
        cout << "String e: " << e << " é " << checkNumberType(e) << endl;
    } catch (const invalid_argument& e) {
        cout << e.what() << endl;
    }
    return 0;
}

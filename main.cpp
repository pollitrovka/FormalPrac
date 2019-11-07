// Даны α и буква x. Найти максимальное k, такое что в L есть слова, начинающиеся с x^k

#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>

using std::max;
using std::min;
using std::cin;
using std::cout;
using std::stack;
using std::string;
using std::vector;
using std::exception;

const string OPERATORS =  "+.*";
const string OPERANDS = "abc1";

class RegularExpression {
    stack<RegularExpression> parsingStack;
    char prefLtr;
    int maxDeg;
    int strDegLen; // длина подслова, в префиксе кт находится наиб степень буквы
    size_t minLen; // a+b+1 - minlen = 0
    void Disjunction();
    void KleeneStar(); // Klini star
    void Concatenation(); //sticking
    void ParsePostfixNotation(const string&);
public:
    RegularExpression(const char&);
    RegularExpression(const char&, const char&);
    RegularExpression(const char&, const string&);
    void operator=(const RegularExpression&);
    int MaxDegree();
    bool Finished();
};

RegularExpression::RegularExpression(const char& letter) {
    prefLtr = letter;
    maxDeg = 0;
    strDegLen = 0;
    minLen = 0;
}

RegularExpression::RegularExpression(const char& letter, const string& postfixNotation) {
    prefLtr = letter;
    maxDeg = 0;
    strDegLen = 0;
    minLen = 0;
    ParsePostfixNotation(postfixNotation);
}

// делаем регулярки из букв
RegularExpression::RegularExpression(const char& letter, const char& expr) {
    prefLtr = letter;
    maxDeg = 0;
    minLen = 1;
    strDegLen = 0;
    if (letter == expr) {
        maxDeg = 1;
        strDegLen = 1;
    }
    if (expr == '1') {
        minLen = 0;
    }
}

void RegularExpression::operator=(const RegularExpression& expr) {
    this->maxDeg = expr.maxDeg;
    this->prefLtr = expr.prefLtr;
    this->parsingStack = expr.parsingStack;
    this->minLen = expr.minLen;
    this->strDegLen = expr.strDegLen;
}

void RegularExpression::Disjunction() {
    if (parsingStack.size() < 2) {
        throw "Not enough arguments for +";
    }
    RegularExpression left = parsingStack.top();
    parsingStack.pop();
    RegularExpression right = parsingStack.top();
    parsingStack.pop();
    RegularExpression curr(prefLtr);
    if (left.maxDeg > right.maxDeg) {
        curr.maxDeg = left.maxDeg;
        curr.strDegLen = left.strDegLen;
    } else {
        curr.maxDeg = right.maxDeg;
        curr.strDegLen = right.strDegLen;
    }
    curr.minLen = min(left.minLen, right.minLen);
    size_t lTmp, rTmp;
    // если левый или правый состоит из степени нужной буквы
    lTmp = ((left.maxDeg == left.strDegLen && left.maxDeg != 0) ? left.maxDeg : curr.minLen);
    rTmp = ((right.maxDeg == right.strDegLen && right.maxDeg != 0) ? right.maxDeg : curr.minLen);
    size_t mx = max(lTmp, rTmp);
    curr.minLen = max(mx, curr.minLen);
    parsingStack.push(curr);
}

void RegularExpression::KleeneStar() {
    if (parsingStack.size() < 1) {
        throw "Not enough arguments for *";
    }
    RegularExpression curr = parsingStack.top();
    parsingStack.pop();
    curr.minLen = 0;
    if (curr.strDegLen == curr.maxDeg && curr.maxDeg != 0) {
        curr.maxDeg = -1;
        curr.strDegLen = -1;
    }
    parsingStack.push(curr);
}

void RegularExpression::Concatenation() {
    if (parsingStack.size() < 2) {
        throw "Not enough arguments for .";
    }
    RegularExpression left = parsingStack.top();
    parsingStack.pop();
    RegularExpression right = parsingStack.top();
    parsingStack.pop();
    RegularExpression curr(prefLtr);
    curr.maxDeg = left.maxDeg;
    curr.strDegLen = left.strDegLen;
    curr.minLen = left.minLen + right.minLen;
//    cout << "before (left.strDegLen = " << left.strDegLen << ") ";
//    cout << "(curr.maxDeg = " << curr.maxDeg << ") ";
//    cout << "(left.minLen = " << left.minLen << ") ";
//    cout << "(curr.minLen = " << curr.minLen << ")\n";
    if (left.maxDeg == -1 || (left.maxDeg == left.strDegLen && right.maxDeg == -1)) {
        curr.maxDeg = -1;
        curr.strDegLen = -1;
    } else if (left.maxDeg == left.strDegLen && left.maxDeg != 0) {
        curr.maxDeg += right.maxDeg;
        curr.strDegLen += right.minLen;
    } else if (left.maxDeg <= right.maxDeg && left.minLen < left.strDegLen) {
        curr.strDegLen = left.minLen + right.strDegLen;
        curr.maxDeg = left.minLen + right.maxDeg;
    }
//    cout << "after (curr.strDegLen = " << curr.strDegLen << ") ";
//    cout << "(curr.maxDeg = " << curr.maxDeg << ") ";
//    cout << "(curr.minLen = " << curr.minLen << ")\n";
    parsingStack.push(curr);
}

void RegularExpression::ParsePostfixNotation(const string& postfixNotation) {
    for (char symbol : postfixNotation) {
        if (OPERANDS.find(symbol) != std::string::npos) {
            RegularExpression expr(prefLtr, symbol);
//            cout << symbol << " degree = " << expr.maxDeg << "\n";
            parsingStack.push(expr);
        }
        if (OPERATORS.find(symbol) != std::string::npos) {
//            cout << "caught operator " << symbol << "\n";
            if (symbol == '+') {
                Disjunction();
            }
            if (symbol == '*') {
                KleeneStar();
            }
            if (symbol == '.') {
                Concatenation();
            }
        }
    }
}

int RegularExpression::MaxDegree() {
    return parsingStack.top().maxDeg;
}

bool RegularExpression::Finished() {
    return parsingStack.size() == 1;
}

int main() {
    string input;
    cin >> input;
    char x;
    cin >> x;
//    cout << input << "\n" << x;
    try {
        RegularExpression expression(x, input);
        if (!expression.Finished()) {
            throw "Unfinished expression";
        }
        cout << expression.MaxDegree();
    }
    catch(...) {
        cout << "ERROR";
    }
    return 0;
}

// ab+c.aba.*.bac.+.+* a
// a*abc.+*. a
// a*bbc.+*. a
// ab+c.aba.*.bac.+. a
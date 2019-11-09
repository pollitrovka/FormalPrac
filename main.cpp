#include <iostream>
#include <string>
#include <stack>

using std::max;
using std::cin;
using std::cout;
using std::stack;
using std::string;

const string OPERATORS =  "+.*";
const string OPERANDS = "abc1";
const string INCORRECT_REGULAR_EXPRESSION = "ERROR";
const string INFINITY = "INF";

class RegularExpression {
    int maxDeg;
    int maxLen;
    stack<RegularExpression> parsingStack;
    void Disjunction();
    void KleeneStar();
    void Concatenation();
    bool Finished();
public:
    RegularExpression();
    RegularExpression(const char&, const char&);
    RegularExpression(const char&, const string&);
    friend void OutputMaxDegree(const string&, const char&);
};

RegularExpression::RegularExpression() : maxDeg(0), maxLen(0) {}

RegularExpression::RegularExpression(const char& x, const char& expr) : maxDeg(0), maxLen(0) {
    if (expr == '1') {
        maxLen = -2;
    }
    if (expr == x) {
        maxDeg = 1;
        maxLen = 1;
    }
}

RegularExpression::RegularExpression(const char& x, const string& postfix) : maxDeg(0), maxLen(0) {
    for (char symbol : postfix) {
        if (OPERANDS.find(symbol) != std::string::npos) {
            RegularExpression expr(x, symbol);
            parsingStack.push(expr);
        }
        if (OPERATORS.find(symbol) != std::string::npos) {
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

void RegularExpression::Disjunction() {
    if (parsingStack.size() < 2) {
        throw "Not enough arguments for +";
    }
    RegularExpression left = parsingStack.top();
    parsingStack.pop();
    RegularExpression right = parsingStack.top();
    parsingStack.pop();
    RegularExpression curr;
    curr.maxDeg = (left.maxDeg == -1 || right.maxDeg == -1) ? -1 : max(left.maxDeg, right.maxDeg);
    // Если "левая" регулярка не имеет слов, состоящих только из x, то вне зависимости от значения maxLen для
    // "правой" регулярки макс. длина слов, сост. только их х, для их дизъюнкции будет равна right.maxLen (очевидно).
    if (left.maxLen == 0) {
        curr.maxLen = right.maxLen;
    } else if (right.maxLen == 0) {
        curr.maxLen = left.maxLen;
    // Если в "левой" или в "правой" регулярках есть слова, состоящие только из х, то в дизъюнкии будут слова,
    // состоящие только из х, причем их максимальная длина будет равна наибольшей из длин таких слов в "левой"
    // и "правой" регулярках. Если таких слов нет, то есть пустое слово (единица), поэтому сохраняем эту информацию.
    } else if (left.maxDeg != -1 && right.maxDeg != -1) {
        curr.maxLen = max(left.maxLen, right.maxLen);
    }
    parsingStack.push(curr);
}

void RegularExpression::KleeneStar() {
    if (parsingStack.size() < 1) {
        throw "Not enough arguments for *";
    }
    RegularExpression curr = parsingStack.top();
    parsingStack.pop();
    // Если есть слова, состоящие только из х, то при "навешивании" звезды, сможем получать такие слова какой
    // угодно длины, т.е. степень х будет бесконечной.
    if (curr.maxLen != -2 && curr.maxLen != 0) {
        curr.maxDeg = -1;
    // Если таких слов нет, то сможем получать пустое слово.
    } else {
        curr.maxLen = -2;
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
    RegularExpression curr;
    // Если в "левой" регулярке есть пустое слово, а в префиксах нет х, то конкатенируем пустое слово с "правой"
    // регуляркой (забываем, что в "левой" существовали другие слова, т.к. они нам не подходят). (1+b)(bac), x=a
    // Если в "левой" все таки есть х в префиксе какого-то слова, то пока считаем, что в конкатенации наибольшая
    // степень х в префиксе равна наибольшей степени х в префиксе в "левой" регулярке. (1+ab)(bac), x=a
    curr.maxDeg = (left.maxLen == -2 && left.maxDeg == 0) ? right.maxDeg : left.maxDeg;
    curr.maxLen = left.maxLen == -2 ? right.maxLen : left.maxLen;
    // Если в "левой" регулярке х встречается в префиксе бесконечное число раз или есть слово, состоящее только
    // из х, а в "правой" регулярке х в префиксе встречается бесконечное число раз, то и в конкатенации х в
    // префиксе будет встречаться бесконечное число раз. (a*)(bc) или (a+1+b)(a*bc), x=a
    if (left.maxDeg == -1 || (left.maxDeg == left.maxLen && left.maxDeg != 0 && right.maxDeg == -1)) {
        curr.maxDeg = -1;
    // Иначе, если в "левой" регулярке максимальная степень х в префиксе берется из слова, состоящего только
    // из х, в конкатенации максимальная степень х в префиксе будет суммой left.maxDeg и right.maxDeg.
    // Будет ли в конкатенации хоть одно слово, состоящее только из х, зависит от того, есть ли в "правой"
    // регулярке слова, состоящие только их х или, если таковых нет, есть ли пустое слово.
    // (1+a+ab)(aa+c), (a)(1), x=a
    } else if (left.maxDeg == left.maxLen && left.maxDeg != 0) {
        curr.maxDeg += right.maxDeg;
        curr.maxLen = right.maxLen == 0 ? 0 : (right.maxLen == -2 ? left.maxLen : left.maxLen + right.maxLen);
    // Если нет (т.е. максимальная степень х в префиксе берется из слова, в суффиксе которого присутсвуют
    // ненужные буквы) и максимальная степень х в ее префиксе не превосходит максимальной степени х в префиксе
    // в "правой" регулярке, то что будет в конкатенации зависит от того, что есть в "левой" регулярке:
    // если есть слово, состоящее только из х, то конкатенируем именно его с "правой" регуляркой (таким образом
    // получаем слово с наибольшей степенью х в префиксе), если нет, но его пустое слово, то конкатенируем его.
    // (aaabc+a)(aaa) или (aaabc+1)(aaa), x=a
    } else if ((left.maxDeg <= right.maxDeg || right.maxDeg == -1) && left.maxLen != 0) {
        curr.maxDeg = left.maxLen == -2 ? right.maxDeg : left.maxDeg + right.maxDeg;
        curr.maxLen = left.maxLen == -2 ? right.maxLen : (right.maxLen == 0 ? 0 : (right.maxLen == -2 ? left.maxLen : left.maxLen + right.maxLen));
    }
    parsingStack.push(curr);
}

bool RegularExpression::Finished() {
    return parsingStack.size() == 1;
}

void OutputMaxDegree(const string& input, const char& x) {
    try {
        RegularExpression expression(x, input);
        if (!expression.Finished()) {
            throw "Unfinished expression";
        }
        if (expression.parsingStack.top().maxDeg < 0) {
            throw -1;
        }
        cout << expression.parsingStack.top().maxDeg;
    }
    catch(char const*) {
        cout << INCORRECT_REGULAR_EXPRESSION;
    }
    catch(int) {
        cout << INFINITY;
    }
}

int main() {
    string input;
    cin >> input;
    char x;
    cin >> x;
    OutputMaxDegree(input, x);
    return 0;
}

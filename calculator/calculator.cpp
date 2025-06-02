#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <stack>

using namespace std;

enum class TokenType { Number, Operator, Function, LeftParen, RightParen };

struct Token {
    TokenType type;
    string value;
};

vector<Token> tokenize(const string& expr) {
    vector<Token> tokens;
    size_t i = 0;

    while (i < expr.size()) {
        if (expr[i] == ' ') {
            i++;
            continue;
        }

        if (isdigit(expr[i]) || expr[i] == '.' || (expr[i] == '-' && (i == 0 || expr[i - 1] == '('))) {
            string num;
            num += expr[i++];
            while (i < expr.size() && (isdigit(expr[i]) || expr[i] == '.')) {
                num += expr[i++];
            }
            tokens.push_back({TokenType::Number, num});
            continue;
        }

        if (isalpha(expr[i])) {
            string func;
            while (i < expr.size() && isalpha(expr[i])) {
                func += expr[i++];
            }
            tokens.push_back({TokenType::Function, func});
            continue;
        }

        if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
            tokens.push_back({TokenType::Operator, string(1, expr[i++])});
            continue;
        }

        if (expr[i] == '(') {
            tokens.push_back({TokenType::LeftParen, "("});
            i++;
            continue;
        }

        if (expr[i] == ')') {
            tokens.push_back({TokenType::RightParen, ")"});
            i++;
            continue;
        }

        i++;
    }
    return tokens;
}

vector<Token> shuntingYard(const vector<Token>& tokens) {
    vector<Token> output;
    stack<Token> opStack;
    map<string, int> precedence = {
        {"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}
    };

    for (const Token& token : tokens) {
        if (token.type == TokenType::Number) {
            output.push_back(token);
        } else if (token.type == TokenType::Function) {
            opStack.push(token);
        } else if (token.type == TokenType::Operator) {
            while (!opStack.empty() &&
                   opStack.top().type == TokenType::Operator &&
                   precedence[opStack.top().value] >= precedence[token.value])
            {
                output.push_back(opStack.top());
                opStack.pop();
            }
            opStack.push(token); // 2 * 3 - 7
        } else if (token.type == TokenType::LeftParen) {
            opStack.push(token);
        } else if (token.type == TokenType::RightParen) {
            while (!opStack.empty() && opStack.top().type != TokenType::LeftParen) {
                output.push_back(opStack.top());
                opStack.pop();
            }
            opStack.pop();
        }
    }

    while (!opStack.empty()) {
        output.push_back(opStack.top());
        opStack.pop();
    }

    return output;
}

double evaluateRPN(const vector<Token>& rpn) {
    stack<double> stack;

    for (const auto& token : rpn) {
        if (token.type == TokenType::Number) {
            stack.push(stod(token.value));
        } else if (token.type == TokenType::Operator) {
            double b = stack.top(); stack.pop();
            double a = stack.top(); stack.pop();
            if (token.value == "+") stack.push(a + b);
            else if (token.value == "-") stack.push(a - b);
            else if (token.value == "*") stack.push(a * b);
            else if (token.value == "/") stack.push(a / b);
        } else if (token.type == TokenType::Function) {
            double x = stack.top(); stack.pop();
            if (token.value == "sin") stack.push(sin(x));
            else if (token.value == "cos") stack.push(cos(x));
        }
    }
    return stack.top();
}

double calculate(const string& expr) {
    auto tokens = tokenize(expr);
    auto rpn = shuntingYard(tokens);
    return evaluateRPN(rpn);
}

int main() {
	cout << "Введите выражение\n";
	string expr;
	getline(cin, expr);
    cout << "Результат: " << calculate(expr) << endl;
    return 0;
}

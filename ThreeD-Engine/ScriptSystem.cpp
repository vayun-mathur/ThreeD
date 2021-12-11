#include "ScriptSystem.h"
#include <stack>
#include <iostream>
#include <vector>
#include <sstream>
#include "SceneObject.h"

ScriptSystem* ScriptSystem::m_system = nullptr;

ScriptSystem::ScriptSystem()
{
}

ScriptSystem::~ScriptSystem()
{
}

int precedence(std::string op) {
	if (op == "=" || op == "+=" || op == "-=" || op == "*=" || op == "/=")
		return 1;
	if (op == "+" || op == "-")
		return 2;
	if (op == "*" || op == "/")
		return 3;
	return 0;
}

// Function to perform arithmetic operations.
ScriptValue* applyOp(ScriptValue* a, ScriptValue* b, std::string op) {
	if (op == "+") return a->add(b);
	if (op == "-") return a->sub(b);
	if (op == "*") return a->mul(b);
	if (op == "/") return a->div(b);
	if (op == "=") return a->assign(b);
	if (op == "+=") return a->addassign(b);
	if (op == "-=") return a->subassign(b);
	if (op == "*=") return a->mulassign(b);
	if (op == "/=") return a->divassign(b);
	return nullptr;
}

bool isOperator(std::string c) {
	return c == "+" || c == "-" || c == "*" || c == "/" || c == "=" ||
		c == "+=" || c == "-=" || c == "*=" || c == "/=";
}

std::string trim(const std::string& str,
	const std::string& whitespace = " \t")
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

bool isFloat(std::string myString) {
	std::istringstream iss(myString);
	float f;
	iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
	// Check the entire string was consumed and if either failbit or badbit is set
	return iss.eof() && !iss.fail();
}

float evaluate(std::string str, SceneObject* object) {
	std::stack <ScriptValue*> values;
	std::stack <std::string> ops;

	std::vector<std::string> tokens;

	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] == ' ') continue;
		else if (str[i] == '(') {
			tokens.push_back("(");
		}
		else if (str[i] == ')') {
			tokens.push_back(")");
		}
		else if (i+1 < str.length() && (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/') && str[i+1] == '=') {
			tokens.push_back(std::string() + str[i] + str[i+1]);
			i++;
		}
		else if (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/' || str[i] == '=') {
			tokens.push_back(std::string()+str[i]);
		}
		else if (isdigit(str[i])) {
			size_t initial = i;
			while (i < str.length() && isdigit(str[i])) {
				i++;
			}
			if (i < str.length() && str[i] == '.') {
				i++;
				while (i < str.length() && isdigit(str[i])) {
					i++;
				}
			}
			tokens.push_back(str.substr(initial, i - initial));
			i--;
		}
		else {
			std::string s = "";
			while (i < str.length() && (isalnum(str[i]) || str[i] == '.')) {
				s += str[i];
				i++;
			}
			i--;
			tokens.push_back(s);
		}
	}

	for (int i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "(") {
			ops.push(tokens[i]);
		}
		else if (tokens[i] == ")")
		{
			while (!ops.empty() && ops.top() != "(")
			{
				ScriptValue* val2 = values.top();
				values.pop();

				ScriptValue* val1 = values.top();
				values.pop();

				std::string op = ops.top();
				ops.pop();

				values.push(applyOp(val1, val2, op));
			}
			if (!ops.empty())
				ops.pop();
		}
		else if (isOperator(tokens[i]))
		{
			while (!ops.empty() && precedence(ops.top())
				>= precedence(tokens[i])) {
				ScriptValue* val2 = values.top();
				values.pop();

				ScriptValue* val1 = values.top();
				values.pop();

				std::string op = ops.top();
				ops.pop();

				values.push(applyOp(val1, val2, op));
			}
			ops.push(tokens[i]);
		}
		else if (isFloat(tokens[i])) {

			values.push(new NumberScriptValue(new float(std::stof(tokens[i]))));
		}
		else {
			std::string x = tokens[i];
			std::string s = "";
			int i = 0;
			while (i < x.size() && isalnum(x[i])) {
				s += x[i];
				i++;
			}
			if (s != "this") throw std::exception("Invalid script");

			ScriptValue* val = object;
			while (i < x.length() && x[i] == '.') {
				i++;
				std::string s = "";
				while (i < x.length() && isalnum(x[i])) {
					s += x[i];
					i++;
				}
				val = val->dot(s);
			}
			i--;
			values.push(val);

		}
	}
	while (!ops.empty()) {
		ScriptValue* val2 = values.top();
		values.pop();

		ScriptValue* val1 = values.top();
		values.pop();

		std::string op = ops.top();
		ops.pop();

		values.push(applyOp(val1, val2, op));
	}
	return *((NumberScriptValue*)values.top())->v;
}

void ScriptSystem::exec(std::string cmd, SceneObject* object)
{
	std::cout << evaluate(cmd, object) << std::endl;
}

ScriptSystem* ScriptSystem::get()
{
	return m_system;
}

void ScriptSystem::create()
{
	if (ScriptSystem::m_system) throw std::exception("Audio System already created");
	ScriptSystem::m_system = new ScriptSystem();
}

void ScriptSystem::release()
{
	if (!ScriptSystem::m_system) return;
	delete ScriptSystem::m_system;
	ScriptSystem::m_system = nullptr;
}

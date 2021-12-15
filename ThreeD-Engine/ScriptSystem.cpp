#include "ScriptSystem.h"
#include <stack>
#include <iostream>
#include <vector>
#include <list>
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

bool at_index(std::string& str, std::string toFind, int index) {
	if (str.find(toFind.c_str(), index) == index) {
		return true;
	}
	return false;
}

std::list<std::string> tokenize(std::string str) {


	std::list<std::string> tokens;

	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\r') continue;
		else if (str[i] == '(') {
			tokens.push_back("(");
		}
		else if (str[i] == ')') {
			tokens.push_back(")");
		}
		else if (str[i] == ';') {
			tokens.push_back(";");
		}
		else if (at_index(str, "number", i)) {
			tokens.push_back("number");
			i += 5;
		}
		else if (at_index(str, "vec3", i)) {
			tokens.push_back("vec3");
			i += 3;
		}
		else if (at_index(str, "if", i)) {
			tokens.push_back("if");
			i += 1;
		}
		else if (str[i] == '{') {
			int cnt = 1;
			int initial = i;
			while (i < str.length() && cnt != 0) {
				i++;
				if (str[i] == '{') cnt++;
				if (str[i] == '}') cnt--;
			}
			tokens.push_back(str.substr(initial, i + 1 - initial));
		}
		else if (i + 1 < str.length() && (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/') && str[i + 1] == '=') {
			tokens.push_back(std::string() + str[i] + str[i + 1]);
			i++;
		}
		else if (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/' || str[i] == '=') {
			tokens.push_back(std::string() + str[i]);
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
	return tokens;
}

void check(std::list<std::string>& tokens, std::string against) {
	if (tokens.front() != against) {
		throw std::exception("invalid script");
	}
	tokens.pop_front();
}

ScriptValue* evaluate_assign(std::list<std::string>& tokens, std::map<std::string, ScriptValue*>& vars);

ScriptValue* evaluate_obj(std::list<std::string>& tokens, std::map<std::string, ScriptValue*>& vars) {
	std::string t = tokens.front();
	tokens.pop_front();
	if (t == "(") {
		ScriptValue* val = evaluate_assign(tokens, vars);
		check(tokens, ")");
		tokens.pop_front();
		return val;
	}
	else if (isFloat(t)) {
		return new NumberScriptValue(new float(std::stof(t)));
	}
	else if (t[0] == '{') {
		return new CodeValue(new Script(t.substr(1, t.length() - 2)));
	}
	else {
		std::string x = t;
		std::string s = "";
		int i = 0;
		while (i < x.size() && isalnum(x[i])) {
			s += x[i];
			i++;
		}
		if (vars.find(s) == vars.end()) throw std::exception("Invalid script");

		ScriptValue* val = vars[s];
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
		return val;
	}
}

ScriptValue* evaluate_mul(std::list<std::string>& tokens, std::map<std::string, ScriptValue*>& vars) {
	ScriptValue* o1 = evaluate_obj(tokens, vars);
	while (tokens.front() == "*" || tokens.front() == "/") {
		std::string op = tokens.front();
		tokens.pop_front();
		ScriptValue* o2 = evaluate_obj(tokens, vars);
		if (op == "*") {
			o1 = o1->mul(o2);
		}
		else {
			o1 = o1->div(o2);
		}
	}
	return o1;
}

ScriptValue* evaluate_add(std::list<std::string>& tokens, std::map<std::string, ScriptValue*>& vars) {
	ScriptValue* o1 = evaluate_mul(tokens, vars);
	while (tokens.front() == "+" || tokens.front() == "-") {
		std::string op = tokens.front();
		tokens.pop_front();
		ScriptValue* o2 = evaluate_mul(tokens, vars);
		if (op == "+") {
			o1 = o1->add(o2);
		}
		else {
			o1 = o1->sub(o2);
		}
	}
	return o1;
}

ScriptValue* evaluate_assign(std::list<std::string>& tokens, std::map<std::string, ScriptValue*>& vars) {
	ScriptValue* o1 = evaluate_add(tokens, vars);
	while (tokens.front() == "+=" || tokens.front() == "-=" || tokens.front() == "*=" || tokens.front() == "/=" || tokens.front() == "=") {
		std::string op = tokens.front();
		tokens.pop_front();
		ScriptValue* o2 = evaluate_add(tokens, vars);
		if (op == "+=") {
			o1 = o1->addassign(o2);
		}
		else if (op == "-=") {
			o1 = o1->subassign(o2);
		}
		else if (op == "*=") {
			o1 = o1->mulassign(o2);
		}
		else if (op == "/=") {
			o1 = o1->divassign(o2);
		}
		else {
			o1 = o1->assign(o2);
		}
	}
	return o1;
}

void evaluate(std::string str, SceneObject* object, std::map<std::string, ScriptValue*> var_in){
	std::stack <ScriptValue*> values;
	std::stack <std::string> ops;

	std::list<std::string> tokens = tokenize(str);

	std::map<std::string, ScriptValue*> vars;
	vars = var_in;
	vars["this"] = object;

	while(!tokens.empty()) {
		if (tokens.front() == "if") {
			tokens.pop_front();
			check(tokens, "(");
			ScriptValue* cond = evaluate_assign(tokens, var_in);
			check(tokens, ")");
			if(cond->bool_value()) {
				evaluate(tokens.front().substr(1, tokens.front().length() - 2), object, vars);
			}
			tokens.pop_front();
		}
		else if (tokens.front() == "number") {
			tokens.pop_front();
			vars[tokens.front()] = new NumberScriptValue(new float());
			tokens.pop_front();
			check(tokens, ";");
		}
		else if (tokens.front() == "vec3") {
			tokens.pop_front();
			vars[tokens.front()] = new Vec3ScriptValue(new vec3());
			tokens.pop_front();
			check(tokens, ";");
		}
		else {
			evaluate_assign(tokens, vars);
			check(tokens, ";");
		}
	}
}

void ScriptSystem::exec(std::string cmd, SceneObject* object, std::map<std::string, ScriptValue*>& var_in)
{
	evaluate(cmd, object, var_in);
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

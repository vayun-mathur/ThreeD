#include "ScriptSystem.h"
#include <stack>
#include <iostream>
#include "SceneObject.h"

ScriptSystem* ScriptSystem::m_system = nullptr;

ScriptSystem::ScriptSystem()
{
}

ScriptSystem::~ScriptSystem()
{
}

int precedence(char op) {
	if (op == '=')
		return 1;
	if (op == '+' || op == '-')
		return 2;
	if (op == '*' || op == '/')
		return 3;
	return 0;
}

// Function to perform arithmetic operations.
ScriptValue* applyOp(ScriptValue* a, ScriptValue* b, char op) {
	switch (op) {
	case '+': return a->add(b);
	case '-': return a->sub(b);
	case '*': return a->mul(b);
	case '/': return a->div(b);
	case '=': return a->assign(b);
	}
}

bool isOperator(char c) {
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '=';
}

float evaluate(std::string tokens, SceneObjectPtr object) {
	int i;
	std::stack <ScriptValue*> values;
	std::stack <char> ops;

	for (i = 0; i < tokens.length(); i++) {
		if (tokens[i] == ' ')
			continue;
		else if (tokens[i] == '(') {
			ops.push(tokens[i]);
		}
		else if (tokens[i] == ')')
		{
			while (!ops.empty() && ops.top() != '(')
			{
				ScriptValue* val2 = values.top();
				values.pop();

				ScriptValue* val1 = values.top();
				values.pop();

				char op = ops.top();
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

				char op = ops.top();
				ops.pop();

				values.push(applyOp(val1, val2, op));
			}
			ops.push(tokens[i]);
		}
		else if (isdigit(tokens[i])) {
			int val = 0;
			while (i < tokens.length() &&
				isdigit(tokens[i]))
			{
				val = (val * 10) + (tokens[i] - '0');
				i++;
			}

			values.push(new NumberScriptValue(new float(val)));
			i--;
		}
		else {
			std::string s = "";
			while (i < tokens.length() && isalnum(tokens[i])) {
				s += tokens[i];
				i++;
			}
			if (s != "this") throw std::exception("Invalid script");

			ScriptValue* val = object.get();
			while (i < tokens.length() && tokens[i] == '.') {
				i++;
				std::string s = "";
				while (i < tokens.length() && isalnum(tokens[i])) {
					s += tokens[i];
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

		char op = ops.top();
		ops.pop();

		values.push(applyOp(val1, val2, op));
	}
	return *((NumberScriptValue*)values.top())->v;
}

void ScriptSystem::exec(std::string cmd, SceneObjectPtr object)
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

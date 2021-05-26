#pragma once
#include <unordered_map>
#include <string>
#include <string>
#include <fstream>
#include "prerequisites.h"
#include "Resource.h"

class ResourceManager
{
public:
	ResourceManager();
	virtual ~ResourceManager();

	ResourcePtr createResourceFromFile(const wchar_t* file_path);

protected:
	virtual Resource* createResourceFromFileConcrete(const wchar_t* file_path) = 0;

private:
	std::unordered_map<std::wstring, ResourcePtr> m_map_resources;
};

inline void readString(std::ifstream& stream, std::wstring& out) {
	int len;
	stream >> len;
	char* str = new char[len];
	char c;
	stream.read(&c, 1);
	stream.read(str, len);
	std::string s = std::string(str, len);
	out = std::wstring(s.begin(), s.end());
}
inline void readString(std::ifstream& stream, std::string& out) {
	int len;
	stream >> len;
	char* str = new char[len];
	char c;
	stream.read(&c, 1);
	stream.read(str, len);
	out = std::string(str, len);
}
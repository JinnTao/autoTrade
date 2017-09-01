#pragma once
#include <vector>
#include <string>
using std::string;
using std::vector;
class IniFile
{
public:
	explicit IniFile(const string &fileName);
	~IniFile(void);
	bool ReadFile(void);
	string ReadString(const string &section, const string &key, const string &value);
	int    ReadInt(const string &section, const string &key, int value);
	bool WriteString(const string &section, const string &key, const string &value);
	bool WriteInt(const string &section, const string &key, int value);
	bool RemoveSection(const string &section);
	bool RemoveKey(const string &section, const string &key);
	bool WriteFile(void);
private:
	static string Trim(const string &str);
	static string LTrim(const string &str);
	static string RTrim(const string &str);
private:
	string m_fileName;
	vector<string> m_vctLine;
	bool m_modifyFlag;
};
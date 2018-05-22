#include "IniFile.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
/************************************************************************/
/* 字符串的迭代器定义                                                         */
/************************************************************************/
struct ci_char_traits : public char_traits<char>
{
    static bool eq(char c1, char c2)
    {
        return toupper(c1) == toupper(c2);
    }
    static bool ne(char c1, char c2)
    {
        return toupper(c1) != toupper(c2);
    }
    static bool lt(char c1, char c2)
    {
        return toupper(c1) < toupper(c2);
    }
    static bool compare(const char* s1, const char* s2, size_t n)
    {
#ifdef WIN32  
        return _memicmp(s1, s2, n) != 0;  //实现不区分大小写的串比较  
#else  
        //linux不支持memicmp，自定义版本  
        char *tmps1 = new char[n];
        char *tmps2 = new char[n];
        for (size_t i = 0; i < n; ++i)
        {
            tmps1[i] = toupper(s1[i]);
            tmps2[i] = toupper(s2[i]);
        }
        return memcmp(tmps1, tmps2, n) != 0;
#endif  
    }
    static const char* find(const char*s, int n, char a)
    {
        while (n-- > 0 && toupper(*s) != toupper(a))
            ++s;
        return s;
    }
};

IniFile::IniFile(const string &fileName)
    :m_fileName(fileName),
    m_modifyFlag(false)
{
    ReadFile();
}

IniFile::~IniFile(void)
{
    WriteFile();
}

bool IniFile::ReadFile(void)
{
    ifstream in(m_fileName.c_str());
    if (false == in.is_open())
        return false;

    string line;
    while (getline(in, line))
    {
        m_vctLine.push_back(line);
    }
    m_modifyFlag = false;
    return true;
}

bool IniFile::WriteFile(void)
{
    //check if is need to save  
    if (false == m_modifyFlag)
    {
        return true;
    }
    ofstream out(m_fileName.c_str());
    for (size_t i = 0; i < m_vctLine.size(); ++i)
    {
        out << m_vctLine[i] << endl;
    }
    m_modifyFlag = false;
    return true;
}

string IniFile::ReadString(const string &section, const string &key, const string &defval)
{
    for (size_t i = 0; i < m_vctLine.size(); ++i)
    {
        string section_line = LTrim(m_vctLine[i]);
        size_t sec_begin_pos = section_line.find('[');
        if (sec_begin_pos == string::npos || sec_begin_pos != 0)
        {
            continue;
        }
        size_t sec_end_pos = section_line.find(']', sec_begin_pos);
        if (sec_end_pos == string::npos)
        {
            continue;
        }

        //if (ci_string(section.c_str()) != Trim(section_line.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1)).c_str())
        if (strcmp(section.c_str() , Trim(section_line.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1)).c_str())   )
        {
            continue;
        }

        //find key  
        for (++i; i < m_vctLine.size(); ++i)
        {
            string key_line = LTrim(m_vctLine[i]);
            size_t sec_pos = key_line.find('[');
            if (sec_pos != string::npos && sec_pos == 0)
            {
                --i;  //reback a step,find again  
                break;//the line is section line  
            }

            if (key_line.find('#') != string::npos)
            {
                continue;//this is comment line  
            }
            size_t equal_pos = key_line.find('=');
            if (equal_pos == string::npos)
            {
                continue;
            }
            //if (ci_string(key.c_str()) != RTrim(key_line.substr(0, equal_pos)).c_str())
            if (strcmp(key.c_str(), RTrim(key_line.substr(0, equal_pos)).c_str()))
            {
                continue;
            }

            size_t comment_pos = key_line.find("#", equal_pos + 1);
            if (comment_pos != string::npos)
            {
                return Trim(key_line.substr(equal_pos + 1, comment_pos - equal_pos - 1));
            }

            return Trim(key_line.substr(equal_pos + 1));
        }
    }

    return defval;
}

bool IniFile::WriteString(const string &section, const string &key, const string &value)
{
    for (size_t i = 0; i < m_vctLine.size(); ++i)
    {
        string section_line = LTrim(m_vctLine[i]);
        size_t sec_begin_pos = section_line.find('[');
        if (sec_begin_pos == string::npos || sec_begin_pos != 0)
        {
            continue;
        }
        size_t sec_end_pos = section_line.find(']', sec_begin_pos);
        if (sec_end_pos == string::npos)
        {
            continue;
        }
        //if (ci_string(section.c_str()) != RTrim(section_line.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1)).c_str())
        if ((section.c_str()) != RTrim(section_line.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1)).c_str())
        {
            continue;
        }
        
        //find key  
        for (++i; i < m_vctLine.size(); ++i)
        {
            string key_line = LTrim(m_vctLine[i]);
            size_t sec_pos = key_line.find('[');
            if (sec_pos != string::npos && sec_pos == 0)
            {
                --i;  //reback a step,find again  
                break;//the line is section line  
            }

            if (key_line.find('#') != string::npos)
            {
                continue;//this is comment line  
            }
            size_t equal_pos = key_line.find('=');
            if (equal_pos == string::npos)
            {
                continue;
            }
            //if (ci_string(key.c_str()) != RTrim(key_line.substr(0, equal_pos)).c_str())
            if ((key.c_str()) != RTrim(key_line.substr(0, equal_pos)).c_str())
            {
                continue;
            }

            size_t comment_pos = key_line.find("#", equal_pos + 1);
            string new_line = key_line.substr(0, equal_pos + 1) + value;
            if (comment_pos != string::npos)
            {
                new_line += key_line.substr(comment_pos);
            }
            key_line = new_line;
            m_modifyFlag = true;
            return true;
        }

        //add a new key  
        m_vctLine.insert(m_vctLine.begin() + i, key + "=" + value);
        m_modifyFlag = true;
        return true;
    }

    //add a new section and a new key  
    m_vctLine.insert(m_vctLine.end(), "");
    m_vctLine.insert(m_vctLine.end(), "[" + section + "]");
    m_vctLine.insert(m_vctLine.end(), key + "=" + value);
    m_modifyFlag = true;
    return true;
}

bool IniFile::RemoveSection(const string &section)
{
    for (size_t i = 0; i < m_vctLine.size(); ++i)
    {
        string section_line = LTrim(m_vctLine[i]);
        size_t sec_begin_pos = section_line.find('[');
        if (sec_begin_pos == string::npos || sec_begin_pos != 0)
        {
            continue;
        }
        size_t sec_end_pos = section_line.find(']', sec_begin_pos);
        if (sec_end_pos == string::npos)
        {
            continue;
        }
        //if (ci_string(section.c_str()) != RTrim(section_line.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1)).c_str())
        if ((section.c_str()) != RTrim(section_line.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1)).c_str())
        {
            continue;
        }

        //  
        size_t del_begin = i;
        for (++i; i < m_vctLine.size(); ++i)
        {
            string next_section = LTrim(m_vctLine[i]);
            size_t next_pos = next_section.find('[');
            if (next_pos == string::npos || next_pos != 0)
            {
                continue;
            }

            break;
        }
        m_vctLine.erase(m_vctLine.begin() + del_begin, m_vctLine.begin() + i);
        return true;
    }
    return false;
}

bool IniFile::RemoveKey(const string &section, const string &key)
{
    for (size_t i = 0; i < m_vctLine.size(); ++i)
    {
        string section_line = m_vctLine[i];
        size_t sec_begin_pos = section_line.find('[');
        if (sec_begin_pos == string::npos || sec_begin_pos != 0)
        {
            continue;
        }
        size_t sec_end_pos = section_line.find(']', sec_begin_pos);
        if (sec_end_pos == string::npos)
        {
            continue;
        }
        //if (ci_string(section.c_str()) != Trim(section_line.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1)).c_str())
        if ((section.c_str()) != Trim(section_line.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1)).c_str())
        {
            continue;
        }

        //find key  
        for (++i; i < m_vctLine.size(); ++i)
        {
            string key_line = m_vctLine[i];
            key_line = Trim(key_line);
            if (key_line.find('#') == 0)
            {
                continue;
            }

            size_t key_pos = key_line.find('=');
            if (key_pos == string::npos)
            {
                continue;
            }

            //if (ci_string(key.c_str()) == Trim(key_line.substr(0, key_pos)).c_str())
            if ((key.c_str()) == Trim(key_line.substr(0, key_pos)).c_str())
            {
                m_vctLine.erase(m_vctLine.begin() + i);
                return true;
            }
        }
    }
    return false;
}

int IniFile::ReadInt(const string &section, const string &key, int value)
{
    string str = ReadString(section, key, "");
    if ("" == str)
    {
        return value;
    }
    istringstream in(str.c_str());
    int ret = 0;
    in >> ret;
    return ret;
}
double IniFile::ReadDouble(const string &section, const string &key, double value)
{
    string str = ReadString(section, key, "");
    if ("" == str)
    {
        return value;
    }
    istringstream in(str.c_str());
    double ret = 0;
    in >> ret;
    return ret;
}


bool IniFile::WriteInt(const string &section, const string &key, int value)
{
    ostringstream out;
    out << value;
    return WriteString(section, key, out.str());
}

string IniFile::LTrim(const string &str)
{
    size_t pos = 0;
    while (pos != str.size())
    {
        if (' ' == str[pos])
        {
            ++pos;
        }
        else
        {
            break;
        }
    }

    return str.substr(pos);
}

string IniFile::RTrim(const string &str)
{
    size_t pos = str.size() - 1;
    while (pos >= 0)
    {
        if (' ' == str[pos])
        {
            --pos;
        }
        else
        {
            break;
        }
    }

    return str.substr(0, pos + 1);
}

string IniFile::Trim(const string &str)
{
    return LTrim(RTrim(str));
}
#include <iostream>
#include <Windows.h>
#include <vector>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include "utils.h"

std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

std::vector<std::string> split(const std::string& txt, char ch)
{
    std::vector<std::string> strs = {};
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    while (pos != std::string::npos)
    {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;
        pos = txt.find(ch, initialPos);
    }

    strs.push_back(txt.substr(initialPos, min(pos, (txt.size() - initialPos + 1))));
    return strs;
}


std::string url_encode(std::string value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char)c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}
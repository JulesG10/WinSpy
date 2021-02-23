#pragma once
#include<iostream>
#include<vector>

std::wstring s2ws(const std::string& s);
std::vector<std::string> split(const std::string& txt, char ch);
std::string url_encode(std::string value);
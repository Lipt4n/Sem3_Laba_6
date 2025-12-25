#pragma once
#include <string>
#include <vector>

std::string get_time();

void ClearTerminal();

void ClearCin();

std::string ToUpper(const std::string& s);

std::vector<std::string> Split(const std::string& s);

void ShowHelp(bool e, bool a);
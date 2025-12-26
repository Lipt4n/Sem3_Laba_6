#pragma once
#include <string>
#include <vector>

void ClearCin();

void Confirm();


int InputInt(std::string hint, int min, int max);

std::string InputWord(std::string hint);

std::string InputLine(std::string hint);


std::string ToUpper(const std::string& s);

std::vector<std::string> SplitForTokens(const std::string& s);

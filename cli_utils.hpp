#pragma once

std::string GetTime();


void PrintTable(pqxx::result& res, bool rows, std::string name);

void PrintCard(pqxx::result& res);


void ClearTerminal();

void ClearAltScreen();

void EnterAltScreen();

void ExitAltScreen();


void PrintActiveUser();
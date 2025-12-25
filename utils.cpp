#include "utils.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;

string get_time() {
    time_t t = time(nullptr);
    tm* local = localtime(&t);

    ostringstream oss;
    oss << put_time(local, "%d.%m %H:%M");
    return oss.str();
}

void ClearTerminal() {
    system("clear");
}

void ClearCin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string ToUpper(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

vector<string> Split(const string& s) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, ' ')) {
        tokens.push_back(ToUpper(token));
    }
    return tokens;
}

void ShowHelp(bool e, bool a) {
    string help_message_engineer =
    "=== Engineer Commands ===\n"
    "/orders <-a(ctive)> <-m(y)> - Show list of orders\n"
    "/order [id] - Show and change order\n"
    "/new - Add new order\n\n";

    string help_message_admin =
    "=== Admin Commands ===\n"
    "/sql - SQL mode\n\n";

    string help_message_user =
    "=== System Commands ===\n"
    "/stats - Show stats\n"
    "/chu - Change user\n"
    "/help - Show this message\n"
    "/exit | /quit - Exit the program\n\n";


    cout << "=== HELP ===\n" << endl;
    cout << (e ? help_message_engineer : "");
    cout << (a ? help_message_admin : "");
    cout << help_message_user << endl;
    cout << "*Note: All commands support writing in both registers." << endl;
    cout << "=== ==== ===" << endl;
}
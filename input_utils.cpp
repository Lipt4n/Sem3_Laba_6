#include <iostream>
#include <sstream>

#include "input_utils.hpp"

using namespace std;

void ClearCin() {   
    cin.clear();

    string shit;
    getline(cin, shit);
}

void Confirm() {
    cin.clear();
    cout << "Press enter to continue...";
    string value;
    getline(cin, value);
}


int InputInt(string hint, int min, int max) {
    int value;
    while (true) {
        try {
            cout << "   " << hint << " ";
            if (!(cin >> value)) {
                throw invalid_argument("Invalid input!");
            } else if (value < min || value > max) {
                throw invalid_argument("Entered value must be from " + to_string(min) + " to " + to_string(max) + "!");
            }
            break;
        } catch (const exception& e) {
            cerr << "|" << endl;
            cerr << "Error: " << e.what() << endl;
            cerr << "|" << endl;
            ClearCin();
        }
    }
    ClearCin();
    return value;
}

string InputWord(string hint) {
    string value;
    while (true) {
        try {
            cout << "   " << hint << " ";
            if (!(cin >> value)) {
                throw invalid_argument("Invalid input!");
            }
            if (value.empty()) {
                throw invalid_argument("The input can't be empty!");
            }
            break;
        } catch (const exception& e) {
            cerr << "|" << endl;
            cerr << "Error: " << e.what() << endl;
            cerr << "|" << endl;
            ClearCin();
        }
    }
    ClearCin();
    return value;
}

string InputLine(string hint) {
    string value;
    while (true) {
        try {
            cout << "   " << hint << " ";
            if (!getline(cin, value)) {
                throw invalid_argument("Invalid input!");
            }
            if (value.empty()) {
                throw invalid_argument("The input can't be empty!");
            }
            break;
        } catch (const exception& e) {
            cerr << "|" << endl;
            cerr << "Error: " << e.what() << endl;
            cerr << "|" << endl;
            cin.clear();
        }
    }
    return value;
}


string ToUpper(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

vector<string> SplitForTokens(const string& s) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, ' ')) {
        tokens.push_back(ToUpper(token));
    }
    return tokens;
}
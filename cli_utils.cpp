#include <iostream>
#include <codecvt>
#include <locale>
#include <algorithm>
#include <pqxx/pqxx>

#include "globals.hpp"
#include "cli_utils.hpp"

using namespace std;

string GetTime() {
    time_t t = time(nullptr);
    tm* local = localtime(&t);

    ostringstream oss;
    oss << put_time(local, "%d.%m %H:%M");
    return oss.str();
}

size_t RealLength(const std::string& s) {
    size_t count = 0;
    for (size_t i = 0; i < s.size(); i++) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if ((c & 0xC0) != 0x80) {
            count++;
        }
    }
    return count;
}

string Setnw(size_t target, const std::string& text) {
    size_t current = RealLength(text);  
    if (current >= target) return text;
    return text + string(target - current, ' ');
}


void PrintTable(pqxx::result& res, bool rows, string name) {
    if (res.empty()) {
        cout << "Result is empty." << endl;
        return;
    }
    
    // 1. Собираем ширины колонок
    vector<size_t> widths(res.columns(), 0);

    for (size_t col = 0; col < res.columns(); col++) {
        string name = res.column_name(col);
        widths[col] = RealLength(name);
    }

    for (const auto& row : res) {
        for (size_t col = 0; col < res.columns(); col++) {
            string val = row[col].is_null() ? "<NULL>" : row[col].as<string>();
            widths[col] = max(widths[col], RealLength(val));
        }
    }

    // 2. Выводим заголовки
    cout << name << endl;
    for (size_t col = 0; col < res.columns(); col++) {
        cout << left << Setnw(widths[col] + 2, res.column_name(col));
    }
    cout << endl;

    // 3. Разделитель
    for (size_t col = 0; col < res.columns(); col++) {
        cout << string(widths[col] + 2, '-');
    }
    cout << endl;

    // 4. Строки данных
    for (const auto& row : res) {
        for (size_t col = 0; col < res.columns(); col++) {
            string val = row[col].is_null() ? "<NULL>" : row[col].as<string>();
            cout << Setnw(widths[col] + 2, val);
        }
        cout << endl;
    }

    if (rows) {
        cout << endl << "Rows: " << res.size() << endl;
    } else {
        cout << endl;
    }
}

void PrintCard(pqxx::result& res) {
    if (res.empty()) {
        cout << "Result is empty." << endl;
        return;
    }
    
    // 1. Собираем ширины колонок
    size_t width = 0;

    for (size_t col = 0; col < res.columns(); col++) {
        string name = res.column_name(col);
        width = max(RealLength(name), width);
    }

    for (size_t col = 0; col < res.columns(); col++) {
        string val = res[0][col].is_null() ? "<NULL>" : res[0][col].as<string>();
        cout << Setnw(width, res.column_name(col)) << " | " << val << endl;
    }
}


void ClearTerminal() {
    system("clear");
}

void ClearAltScreen() {
    std::cout << "\x1B[2J";  // Clear entire screen
    std::cout << "\x1B[H";   // Move cursor to top-left
    std::cout.flush();
}

void EnterAltScreen() {
    cout << "\x1B[?1049h";  // DEC Private Mode Set - Alternate Screen
    cout.flush();
}

void ExitAltScreen() {
    cout << "\x1B[?1049l";  // DEC Private Mode Reset - Main Screen
    cout.flush();
}


void PrintActiveUser() {
    cout << "Active user: " << active_user.name << " with id " << active_user.id << endl;
}
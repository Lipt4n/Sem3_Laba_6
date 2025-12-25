#include <iostream>
#include <algorithm>
#include <pqxx/pqxx>
#include "utils.hpp"
#include "db_utils.hpp"
#include "globals.hpp"

using namespace std;

void process_command() {
    vector<string> cmd;
    while (true) {
        try {
            string line;
            cout << "   >>> ";
            cin.clear();
            getline(cin, line);
            cmd.clear();
            if (!line.empty()) {
                cmd = Split(line);
            } else {
                throw invalid_argument("Command is empty!");
            }

            if (cmd[0] == "/ORDERS"){
                if (cmd.size() == 1) {
                    ShowOrders(false, false);
                } else if (cmd.size() == 2) {
                    ShowOrders(cmd[1] == "-A", cmd[1] == "-M");
                }
            } else if (cmd[0] == "/ORDER" && cmd.size() == 2) {
                ShowOrder(cmd[1]);
            } else if (cmd[0] == "/NEW") {
                NewOrder();
            } else if (cmd[0] == "/SQL") {
                SqlMode();
            } else if (cmd[0] == "/HELP") {
                ShowHelp(active_user.is_engineer, active_user.is_admin);
            } else if (cmd[0] == "/CHU") {
                Authorize();
            } else if (cmd[0] == "/STATS") {
                Stats();
            } else if (cmd[0] == "/EXIT" || cmd[0] == "/QUIT") {
                break;
            } else {
                throw invalid_argument("Unknown command!");
            }
        } catch (const exception &e) {
            cerr << "===================================\n" << endl;
            cerr << "Error: " << e.what() << endl;
            cerr << "\n===================================" << endl;
        }
    }
}

int main() {
    ClearTerminal();
    ConnectDB();

    Authorize();
    cout << "\n=== Welcome to Service Center CRM! ===" << endl;
    cout << "/help - show list of commands\n" << endl;
    process_command();

    base.close();
    if (!base.is_open()) {
        cout << "Сonnection is closed." << endl;
        cout << "=== Goodbye! ===" << endl;
    }
    exit(0);
}
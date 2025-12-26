#include <iostream>
#include <pqxx/pqxx>

#include "db_utils.hpp"
#include "globals.hpp"
#include "input_utils.hpp"
#include "cli_utils.hpp"

using namespace std;

void ShowHelp(bool e, bool a) {
    string help_message_engineer =
    "=== Engineer Commands ===\n"
    "/new                        - Add new order\n"
    "/order [id]                 - Show and change order\n"
    "/orders <-a(ctive)> <-m(y)> - Show list of orders\n"
    "/stats                      - Show stats\n\n";

    string help_message_admin =
    "=== Admin Commands ===\n"
    "/sql - SQL mode\n\n";

    string help_message_user =
    "=== System Commands ===\n"
    "/help | /man    - Show this message\n"
    "/cls  | /clear  - Clear Terminal\n"
    "/chu  | /auth   - Change user\n"
    "/who  | /whoami - Show active user\n"
    "/exit | /quit   - Exit the program\n\n";

    EnterAltScreen();
    cout << "========== HELP ==========" << endl << endl;
    cout << (e ? help_message_engineer : "");
    cout << (a ? help_message_admin : "");
    cout << help_message_user << endl;
    cout << "*Note: All commands support writing in both registers." << endl;
    Confirm();
    ExitAltScreen();
}

void ProcessCommand() {
    vector<string> cmd;
    while (true) {
        try {
            string line = InputLine(">>>");
            cmd.clear();
            if (!line.empty()) {
                cmd = SplitForTokens(line);
            } else {
                throw invalid_argument("Command is empty!");
            }

            //Engineer cmds
            if (cmd[0] == "/NEW" && cmd.size() == 1) {
                NewOrder();
            }
            else if (cmd[0] == "/ORDER" && cmd.size() == 2) {
                ShowOrder(cmd[1]);
            }
            else if (cmd[0] == "/ORDERS" && cmd.size() == 1){
                ShowOrders(false, false);
            }
            else if (cmd[0] == "/ORDERS" && cmd.size() == 2 && (cmd[1] == "-A" || cmd[1] == "-M")) {
                ShowOrders(cmd[1] == "-A", cmd[1] == "-M");
            }
            else if (cmd[0] == "/STATS" && cmd.size() == 1) {
                Stats();
            }

            //Admin cmds
            else if (cmd[0] == "/SQL" && cmd.size() == 1) {
                SqlMode();
            }

            //User cmds
            else if (cmd[0] == "/HELP" || cmd[0] == "/MAN") {
                ShowHelp(active_user.is_engineer, active_user.is_admin);
            }
            else if (cmd[0] == "/CLS" || cmd[0] == "/CLEAR") {
                ClearTerminal();
            }
            else if (cmd[0] == "/CHU" || cmd[0] == "/AUTH") {
                Authorize();
            }
            else if (cmd[0] == "/WHO" || cmd[0] == "/WHOAMI") {
                PrintActiveUser();
            }
            else if (cmd[0] == "/EXIT" || cmd[0] == "/QUIT") {
                break;
            }

            //Throw
            else {
                throw invalid_argument("Unknown command!");
            }
        } catch (const exception &e) {
            cerr << "|" << endl;
            cerr << "Error: " << e.what() << endl;
            cerr << "|" << endl;
        }
    }
}

int main() {
    ClearTerminal();
    ConnectDB();
    Authorize();

    cout << "\n=== Welcome to Service Center CRM! ===" << endl;
    cout << "/help - show list of commands\n" << endl;
    ProcessCommand();

    DisconnectDB();
    exit(0);
}
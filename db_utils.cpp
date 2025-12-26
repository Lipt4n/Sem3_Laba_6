#include <iostream>
#include <iomanip>
#include <pqxx/pqxx>

#include "globals.hpp"
#include "db_utils.hpp"
#include "cli_utils.hpp"
#include "input_utils.hpp"

using namespace std;

void ConnectDB() { //gotovo
    if (base.is_open()) {
        cout << "Connected to database: " << base.dbname() << endl;
    } else {
        cout << "Connection failed." << endl;
        exit(1);
    }
}

void DisconnectDB() { //gotovo
    base.close();
    if (!base.is_open()) {
        cout << "Сonnection is closed." << endl;
        cout << "=== Goodbye! ===" << endl;
    }
}

pqxx::result SqlQuery(const std::string& query) { //gotovo
    pqxx::result res;
    try {
        pqxx::work txn(base);
        res = txn.exec(query);
        txn.commit();
    } catch (const exception& e) {
        cerr << "|" << endl;
        cerr << "SQL Error: " << e.what() << endl;
        cerr << "|" << endl;
    }
    return res;
}


void ShowOrders(bool a, bool m) { //gotovo
    string head = "ORDERS";
    string query =
    "SELECT o.id, dt.name AS type, device, c.name AS client, total_price AS price, os.name AS status, u.name AS engineer "
    "FROM orders o "
    "JOIN device_types dt ON o.type = dt.id "
    "JOIN clients c ON o.client_id = c.id "
    "JOIN order_statuses os ON o.status = os.id "
    "JOIN users u ON o.engineer_id = u.id";
    if (a) {
        query += " WHERE status!=7";
        head = "ACTIVE " + head;
    } else if (m) {
        query += " WHERE status!=7 AND engineer_id=" + active_user.id;
        head = "MY ACTIVE " + head;
    }
    pqxx::result res = SqlQuery(query + " ORDER BY id;");

    EnterAltScreen();
    string name = "========== " + head + " ==========";
    PrintTable(res, true, name);
    Confirm();
    ExitAltScreen();
}

void ShowOrder(string id) { //gotovo
    string Query = "SELECT o.id, dt.name AS type, o.device, o.fault, "
                   "o.view, c.name AS client_name, c.phone AS client_phone, "
                   "en.name AS engineer, o.total_price, o.details_price, "
                   "o.works, os.name AS status, o.comment "
                   "FROM orders o "
                   "JOIN device_types dt ON o.type = dt.id "
                   "JOIN clients c ON o.client_id = c.id "
                   "JOIN order_statuses os ON o.status = os.id "
                   "LEFT JOIN users en ON o.engineer_id = en.id "
                   "WHERE o.id=" + id;
    pqxx::result res = SqlQuery(Query);

    EnterAltScreen();
    PrintCard(res);
    cout << string(40, '-') << endl;
    cout << "What do you want?" << endl;
    cout << "1. Change status" << endl;
    cout << "2. Add comment" << endl;
    cout << "0. Exit" << endl;

    int cmd = InputInt("Сhoice >", 0, 2);
    
    if (cmd == 1) {
        pqxx::result statuses = SqlQuery("SELECT * FROM order_statuses");
        PrintTable(statuses, false, "Available statuses:");

        int selected_status = InputInt("Choice >", 0, statuses.size());
        SqlQuery("UPDATE orders SET status=" + to_string(selected_status) + " WHERE id=" + id); 
        cout << "Success!" << endl;
        Confirm();
    } else if (cmd == 2) {
        pqxx::result available_comment = SqlQuery("SELECT comment FROM orders WHERE id=" + id);
        string new_comment = InputLine("Comment >");
        string final_comment = available_comment[0][0].as<string>() + "\n" + active_user.name + " [" + GetTime() + "] - " + new_comment;
        
        SqlQuery("UPDATE orders SET comment='" + final_comment + "' WHERE id=" + id);
        cout << "Success!" << endl; 
        Confirm();
    }
    ExitAltScreen();
}

void NewOrder() { //gotovo
    EnterAltScreen();
    cout << "========== CREATING ORDER ==========" << endl;
    vector<string> order;

    pqxx::result device_types = SqlQuery("SELECT * FROM device_types");
    PrintTable(device_types, false, "Types:");
    order.push_back(to_string(InputInt("Input Device Type >", 0, device_types.size())));

    order.push_back(InputLine("Input Device name >"));
    order.push_back(InputLine("Input Fault >"));
    order.push_back(InputLine("Input View >"));
    
    
    string phone = InputWord("Input Client Phone (without +7) >");
    pqxx::result client = SqlQuery("SELECT id, name FROM clients WHERE phone='" + phone + "'");
    if (client.empty()) {
        string name = InputLine("Input Client Name >");
        pqxx::result newclient = SqlQuery("INSERT INTO clients (name, phone) VALUES ('" + name + "', '" + phone + "') RETURNING id");
        order.push_back(newclient[0][0].as<string>());
    } else {
        cout << "The client was found in the database!" << endl;
        order.push_back(client[0][0].as<string>());
    }

    order.push_back(to_string(InputInt("Input Total Price >", 0, 999999)));
    order.push_back(to_string(InputInt("Input Details Price >", 0, 999999)));
    
    string query = "INSERT INTO orders (type, device, fault, view, client_id, total_price, details_price, comment, works, status, engineer_id) "
    "VALUES (" + order[0] + ", '" + order[1] + "', '" + order[2] + "', '" + order[3] + "', " + order[4] + ", " + order[5] + ", " + order[6] + ", '', '', 1, " + active_user.id + ')' +
    "RETURNING id";
    pqxx::result res_final = SqlQuery(query);
    cout << "Order was created with id = " << res_final[0][0].as<string>() << "!" << endl;
    Confirm();
    ExitAltScreen();
}

void Stats() { //gotovo
    EnterAltScreen();
    cout << "Select analytics:" << endl;
    cout << "1. Кол-во всех и закрытых заказов" << endl;
    cout << "2. Типы устройств и их кол-во среди всех" << endl;
    cout << "3. Выручка с закрытых заказов" << endl;
    cout << "4. Средняя стоимость заказов по инженерам" << endl;
    cout << "5. Топ клиентов по заказам и общая сумма" << endl;

    int cmd = InputInt("Сhoice >", 1, 5);

    string query;
    switch (cmd) {
        case 1: {
            query =
            "SELECT " 
            "(SELECT COUNT(*) FROM orders) AS total_orders, "
            "(SELECT COUNT(*) FROM orders WHERE status = 7) AS closed_orders;";
            break;
        }
        case 2: {
            query = 
            "SELECT dt.name AS device_type, COUNT(o.id) AS count "
            "FROM orders o "
            "JOIN device_types dt ON o.type = dt.id "
            "GROUP BY dt.name "
            "ORDER BY count DESC; ";
            break;
        }
        case 3: {
            query = 
            "SELECT "
            "SUM(total_price) AS total_revenue, "
            "SUM(total_price - details_price) AS net_profit "
            "FROM orders "
            "WHERE status = 7; ";
            break;
        }
        case 4: {
            query = 
            "SELECT u.name AS engineer, AVG(o.total_price) AS avg_price "
            "FROM orders o "
            "JOIN users u ON o.engineer_id = u.id "
            "GROUP BY u.name "
            "ORDER BY avg_price DESC; ";
            break;
        }
        case 5 : {
            query = 
            "SELECT c.name AS client, c.phone, COUNT(o.id) AS orders_count, SUM(o.total_price) AS total_spent "
            "FROM orders o "
            "JOIN clients c ON o.client_id = c.id "
            "GROUP BY c.name, c.phone "
            "ORDER BY orders_count DESC "
            "LIMIT 5; ";
            break;
        }
    }
    
    pqxx::result res = SqlQuery(query);
    string name = "========== RESULT ==========";
    PrintTable(res, false, name);
    Confirm();
    ExitAltScreen();
}


void Authorize() { //gotovo
    pqxx::result users = SqlQuery("SELECT id, name FROM users WHERE is_active=true ORDER BY id");

    EnterAltScreen();
    PrintTable(users, false, "Select user to authorize:");
    
    string user = InputWord("User >");
    pqxx::result row = SqlQuery("SELECT name, pass_hash, is_engineer, is_admin FROM users WHERE id='" + user + "'");
    if (!row.empty()) {
        cout << "Selected user - " << row[0][0].as<string>() << endl;

        cout << "Input password:" << endl;
        string pass = InputWord("Pass >");
        
        if (!(to_string(hash<string>{}(pass)) == to_string(row[0][1]))) {
            cout << "Invalid password! Nice try, good luck, goodbye" << endl;
            exit(1);
        }
        
        cout << endl << "Successful authorization!" << endl;
        active_user.name = to_string(row[0][0]);
        active_user.is_engineer = (to_string(row[0][2]) == "t");
        active_user.is_admin = (to_string(row[0][3]) == "t");
        active_user.id = (user);
    }
    ExitAltScreen();
}

void SqlMode() { //gotovo
    if (!active_user.is_admin) {
        cout << "Sorry, You don't have rights to use it." << endl;
        return;
    }

    EnterAltScreen();
    cout << "=== ENABLED SQL MODE ===" << endl;
    cout << "Warning: Using sql mode may be dangerous!" << endl;
    cout << "/exit | /quit - Disable SQL mode" << endl;
    
    while (true) {
        string query = InputLine("SQL>>>");

        if (ToUpper(query) == "/EXIT" || ToUpper(query) == "/QUIT" ) {
            break;
        }
        
        if (query.empty()) continue;

        try {
            pqxx::result res = SqlQuery(query);
            
            if (res.empty()) {
                cout << "Query OK, 0 rows returned" << endl;
            } else {
                PrintTable(res, true, "Result:");
            }

        } catch (const exception& e) {
            cerr << "|" << endl;
            cerr << "SQL Error: " << e.what() << endl;
            cerr << "|" << endl;
        }
    }
    cout << "SQL mode disabled." << endl;
    ExitAltScreen();
}
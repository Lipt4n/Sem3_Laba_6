#include <iostream>
#include <iomanip>
#include <pqxx/pqxx>
#include "utils.hpp"
#include "db_utils.hpp"
#include "globals.hpp"

using namespace std;

void ConnectDB() {
    if (base.is_open()) {
        cout << "Connected to database: " << base.dbname() << endl;
    } else {
        cout << "Connection failed." << endl;
        exit(1);
    }
}

pqxx::result SqlQuery(const std::string& query) {
    pqxx::result res;
    try {
        pqxx::work txn(base);
        res = txn.exec(query);
        txn.commit();
    } catch (const exception& e) {
        cerr << "===================================\n" << endl;
        cerr << "SQL Error: " << e.what() << endl;
        cerr << "\n===================================" << endl;
    }
    return res;
}

void ShowOrders(bool a, bool m) {
    ClearTerminal();
    string Query = "SELECT o.id, dt.name AS type, device, fault, c.name AS client, total_price AS price, os.name AS status FROM orders o JOIN device_types dt ON o.type = dt.id JOIN clients c ON o.client_id = c.id JOIN order_statuses os ON o.status = os.id";
    if (a) {
        Query += " WHERE status!=7";
    } else if (m) {
        Query += " WHERE status!=7 AND engineer_id=" + active_user.id;
    }
    pqxx::result res = SqlQuery(Query + " ORDER BY id;");

    vector<string> col_names;
    for (pqxx::row_size_type i = 0; i < res.columns(); ++i) {
        col_names.push_back(res.column_name(i));
    }

    if (res.empty()) {
        cout << "The result is empty =(" << endl;
    } else {
        for (const auto& row : res) {
            for (size_t col = 0; col < row.size(); ++col) {
                cout << col_names[col] << ": " << row[col].as<string>() << endl;
            }
            cout << string(40, '-') << endl;
        }

        cout << "Rows: " << res.size() << endl;
    }
}

//добавить аналитику 5 запр
void Stats() {
    cout << "Select analytics:" << endl;
    cout << "1. Кол-во всех и закрытых заказов" << endl;
    cout << "2. Типы устройств и их кол-во среди всех" << endl;
    cout << "3. Выручка с закрытых заказов" << endl;
    cout << "4. Средняя стоимость заказов по инженерам" << endl;
    cout << "5. Топ клиентов по заказам и общая сумма" << endl;
    int cmd;
    cout << "   > ";
    cin >> cmd;
    cin.clear();
    if (cmd < 0 || cmd > 5) {
        cout << "Wrond command!" << endl;
        return;
    }

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
    
    if (res.empty()) {
        cout << "The result is empty =(" << endl;
    } else {
        for (size_t c = 0; c < res.columns(); ++c) {
            cout << res.column_name(c) << "\t";
        }
        cout << endl;

        for (const auto& r : res) {
            for (size_t c = 0; c < r.size(); ++c) {
                cout << r[c].as<string>() << "\t";
            }
            cout << endl;
        }
    }
    ClearCin();
}

void ShowOrder(string id) {
    ClearTerminal();
    string Query = "SELECT o.id, dt.name AS type, o.device, o.fault, "
                   "o.view, c.name AS client_name, c.phone AS client_phone, "
                   "en.name AS engineer, o.total_price, o.details_price, "
                   "o.comment, o.works, os.name AS status "
                   "FROM orders o "
                   "JOIN device_types dt ON o.type = dt.id "
                   "JOIN clients c ON o.client_id = c.id "
                   "JOIN order_statuses os ON o.status = os.id "
                   "LEFT JOIN users en ON o.engineer_id = en.id "
                   "WHERE o.id=" + id;
    pqxx::result res = SqlQuery(Query);

    vector<string> col_names;
    for (pqxx::row_size_type i = 0; i < res.columns(); ++i) {
        col_names.push_back(res.column_name(i));
    }

    if (res.empty()) {
        cout << "The result is empty =(" << endl;
    } else {
        const auto& row = res[0];

        for (size_t col = 0; col < row.size(); ++col) {
            cout << col_names[col] << ": " << row[col].as<std::string>() << endl;
        }
        cout << string(40, '-') << endl;
        cout << "What do you want?" << endl;
        cout << "1. Change status" << endl;
        cout << "2. Add comment" << endl;
        cout << "0. Exit" << endl;
        cout << "   >>> ";
        int cmd;
        cin >> cmd;
        ClearCin();
        if (cmd == 1) {
            pqxx::result res1 = SqlQuery("SELECT * FROM order_statuses");
            for (pqxx::row_size_type i = 0; i < res1.columns(); i++) {
                cout << res1.column_name(i) << "\t";
            }
            cout << endl << string(40, '-') << endl;
        
            for (const auto& row : res1) {
                for (const auto& field : row) {
                    cout << field.as<string>() << "\t";
                }
                cout << endl;
            }
            int stat;
            cout << "   Select> ";
            cin >> stat;
            if (stat < 0 || stat > res1.size()) {
                cerr << "Wrong status ID!";
                ClearCin();
            } else {
                SqlQuery("UPDATE orders SET status=" + to_string(stat) + " WHERE id=" + id); 
                cout << "Success!" << endl;
                ClearCin();
            }
        } else if (cmd == 2) {
            pqxx::result res = SqlQuery("SELECT status FROM orders WHERE id=" + id);
            string comm;
            cout << "Write comment: ";
            cin.clear();
            getline(cin, comm);
            if (!comm.empty()){
                string newcomm = row["comment"].as<std::string>() + "\n" + active_user.name + " [" + get_time() + "] - " + comm;
                try {
                    SqlQuery("UPDATE orders SET comment='" + newcomm + "' WHERE id=" + id);
                    cout << "Success!" << endl; 
                } catch (const exception& e) {
                    cerr << e.what() << endl;
                }
            }
        } else {
            return;
        }
    }
}

void NewOrder() {
    ClearTerminal;
    cout << "Creating order:" << endl;
    vector<string> order;
    string value;

    pqxx::result res1 = SqlQuery("SELECT * FROM device_types");
    for (pqxx::row_size_type i = 0; i < res1.columns(); i++) {
        cout << res1.column_name(i) << "\t";
    }
    cout << endl << string(40, '-') << endl;

    for (const auto& row : res1) {
        for (const auto& field : row) {
            cout << field.as<string>() << "\t";
        }
        cout << endl;
    }
    int stat;
    cout << "Input Device type > ";
    cin >> stat;
    if (stat < 0 || stat > res1.size()) {
        cout << "Invalid type. Seted 0." << endl;
        order.push_back("0");
    } else {
        order.push_back(to_string(stat));
    }
    cin.clear();
    cin.ignore(256, '\n');
    cout << "Input Device name > ";
    getline(cin,value);
    order.push_back(value);
    cout << "Input Fault > ";
    getline(cin,value);
    order.push_back(value);
    cout << "Input View > ";
    getline(cin,value);
    order.push_back(value);
    cout << "Input Client Phone (without +7) > ";
    getline(cin,value);
    pqxx::result res = SqlQuery("SELECT id, name FROM clients WHERE phone='" + value + "'");
    if (res.empty()) {
        cout << "Input Client Name > ";
        string name;
        getline (cin, name);
        pqxx::result res2 = SqlQuery("INSERT INTO clients (name, phone) VALUES ('" + name + "', '" + value + "') RETURNING id");
        order.push_back(res2[0][0].as<string>());
    } else {
        cout << "The client was found in the database!" << endl;
        order.push_back(res[0][0].as<string>());
    }
    cout << "Input Total Price > ";
    getline(cin,value);
    order.push_back(value);
    cout << "Input Details Price > ";
    getline(cin,value);
    order.push_back(value);
    
    string Query = "INSERT INTO orders (type, device, fault, view, client_id, total_price, details_price, comment, works, status, engineer_id) "
    "VALUES (" + order[0] + ", '" + order[1] + "', '" + order[2] + "', '" + order[3] + "', " + order[4] + ", " + order[5] + ", " + order[6] + ", '', '', 1, " + active_user.id + ')' +
    "RETURNING id";
    try {
        pqxx::result res_final = SqlQuery(Query);
        cout << "Order was created with id = " << res_final[0][0].as<string>() << "!" << endl;
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
}

void SqlMode() {
    if (!active_user.is_admin) {
        cout << "Sorry, You don't have rights to use it." << endl;
        return;
    }
    cout << "=== ENABLED SQL MODE ===" << endl;
    cout << "Warning: Using sql mode may be dangerous!" << endl;
    cout << "/exit | /quit - Disable SQL mode" << endl;
    
    while (true) {
        cout << "   SQL>>> ";
        string sql_req;
        getline(cin, sql_req);

        if (ToUpper(sql_req) == "/EXIT" || ToUpper(sql_req) == "/QUIT" ) {
            break;
        }
        
        if (sql_req.empty()) continue;

        try {
            pqxx::result res = SqlQuery(sql_req);
            
            if (res.empty()) {
                cout << "Query OK, 0 rows returned" << endl;
            } else {
                for (pqxx::row_size_type i = 0; i < res.columns(); ++i) {
                    cout << res.column_name(i) << "\t";
                }
                cout << endl << string(40, '-') << endl;
            
                for (const auto& row : res) {
                    for (const auto& field : row) {
                        cout << field.as<string>() << "\t";
                    }
                    cout << endl;
                }
                cout << "Rows: " << res.size() << endl;
            }
        } catch (const exception& e) {
            cerr << "===================================\n" << endl;
            cerr << "SQL Error: " << e.what() << endl;
            cerr << "\n===================================" << endl;
        }
    }
    cout << "SQL mode disabled." << endl;
}

void Authorize() {
    pqxx::result res = SqlQuery("SELECT id, name FROM users WHERE is_active=true ORDER BY id");

    cout << "Select user to authorize:" << endl;
    for (int i = 0; i < res.size(); i++) {
        cout << res[i][0] << " - " << res[i][1] << endl;
    }
    string user, pass;

    cout << "   User > ";
    cin >> user;
    pqxx::result row = SqlQuery("SELECT name, pass_hash, is_engineer, is_admin FROM users WHERE id='" + user + "'");
    cout << "Selected user - " << row[0][0] << endl;
    cout << "Input password:" << endl;
    cout << "   Pass > ";
    cin >> pass;
    if (!(to_string(std::hash<string>{}(pass)) == to_string(row[0][1]))) {
        cout << "Invalid password! Nice try, good luck, goodbye" << endl;
        exit(1);
    }
    ClearCin();
    cout << endl << "Successful authorization!" << endl;
    active_user.name = to_string(row[0][0]);
    active_user.is_engineer = (to_string(row[0][2]) == "t");
    active_user.is_admin = (to_string(row[0][3]) == "t");
    active_user.id = (user);
}
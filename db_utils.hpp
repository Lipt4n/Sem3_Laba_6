#pragma once
#include <pqxx/pqxx>

void ConnectDB();

void DisconnectDB();

pqxx::result SqlQuery(const std::string& query);


void ShowOrders(bool a, bool m);

void ShowOrder(std::string id);

void NewOrder();

void Stats();


void Authorize();

void SqlMode();
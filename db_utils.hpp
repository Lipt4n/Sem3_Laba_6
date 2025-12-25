#pragma once
#include <pqxx/pqxx>

void ConnectDB();

pqxx::result SqlQuery(const std::string& query);

void ShowOrders(bool a, bool m);

void Stats();

void ShowOrder(std::string id);

void NewOrder();

void SqlMode();

void Authorize();
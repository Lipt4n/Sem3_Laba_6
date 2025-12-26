#pragma once
#include <string>
#include <pqxx/pqxx>

struct usertype {
    std::string name;
    bool is_engineer;
    bool is_admin;
    std::string id;
};

extern usertype active_user;  
extern pqxx::connection base; 
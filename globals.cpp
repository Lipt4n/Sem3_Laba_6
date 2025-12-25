#include "globals.hpp"

usertype active_user;

pqxx::connection base(
    "host=localhost "
    "port=5432 "
    "dbname=service "
    "user=postgres "
    "password=root "
);
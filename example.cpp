#include "sqlitepp.h"
#include <iostream>

int main() {
    std::cout << "Opening database test.db..." << std::endl;
    // :memory: opens an in-memory database
    // use file paths if you want to open a sqlite database file
    sqlitepp::Database db(":memory:");

    std::cout << "Creating table..." << std::endl;
    int v = db.exec("CREATE TABLE users (name TEXT, password TEXT);");

    std::cout << "Inserting data directly..." << std::endl;
    db.exec("INSERT INTO users (name, password) VALUES ('paul', 'test');");

    std::cout << "Inserting data by prepared statement..." << std::endl;
    sqlitepp::Statement st(db);
    st.prepare("INSERT INTO users (name, password) VALUES (?, ?);");
    st.bindString(1, "steve");
    st.bindString(2, "this_is_a_password");
    st.exec();

    std::cout << "Selecting all users..." << std::endl;
    st.prepare("SELECT * FROM users;");
    while(st.fetchRow()) {
        std::cout << "Username: " << st.getString("name")
            << ", password: " << st.getString("password") << std::endl;
    }

    try {
        sqlitepp::Database db2;
        db2.exec("COMMIT;");

        std::cout << "Exceptions doen't work." << std::endl;
    } catch(sqlitepp::SQLiteException& e) {
        std::cout << "Exceptions work." << std::endl;
    }

    try {
        sqlitepp::Statement st(db);
        st.bindInt(1, 4);

        std::cout << "Exceptions doen't work." << std::endl;
    } catch(sqlitepp::SQLiteException& e) {
        std::cout << "Exceptions work." << std::endl;
    }
}


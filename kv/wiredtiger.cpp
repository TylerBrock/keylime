#include <iostream>
#include <string>

#include "wiredtiger.hpp"

int main() {
    const char* home = "./data";

    wiredtiger::Connection conn(home);
    std::cout << "connected to " << conn.getHome() <<std::endl;

    auto session = conn.getSession();

    session.drop("tyler");
    session.create("tyler");
    auto cursor = session.cursor("tyler");

    session.begin_transaction();
    cursor.set("tyler", "brock");
    cursor.set("jimbo", "jones");
    session.commit_transaction();

    session.begin_transaction();
    cursor.set("zombo", "combo");
    session.rollback_transaction();

    cursor.scan();
    std::cout << cursor.get("jimbo") << std::endl;
    std::cout << cursor.get("zombo") << std::endl; // why isn't this rolled back
}
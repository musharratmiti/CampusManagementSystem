#ifndef AUTH_H
#define AUTH_H

#include "common.h"

class User {
public:
    string id;
    string name;
    string email;
    string department;
    int semester;
    string password;
    string role;
};

bool findUser(string id, User &user);

int homeScreen();
int registerScreen();
int loginScreen(User &user, string role);
int dashboardScreen(User &user);
int profileScreen(User &user);

#endif

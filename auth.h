// ============================================================
// auth.h   -  MEMBER 1
// Student Registration, Login, Logout, Profile,
// Profile Edit, Password Management, User Role
// ============================================================

#ifndef AUTH_H
#define AUTH_H

#include "common.h"

class User {
public:
    string studentId;
    string fullName;
    string email;
    string department;
    int semester;
    string password;
    string role;          // "Student" or "Admin"
};

// file functions
vector<User> loadUsers();
void saveUsers(vector<User> users);
void updateUser(User user);
bool findUser(string studentId, User &foundUser);
void createDefaultAdmin();

// features
void registerStudent();
bool loginUser(User &loggedUser);
void showProfile(User user);
void editProfile(User &user);
void changePassword(User &user);

// menus
void profileMenu(User &user);
void manageUsersMenu(User &admin);

#endif

// ============================================================
// auth.cpp   -  MEMBER 1
// Student Registration, Login, Logout, Profile,
// Profile Edit, Password Management, User Role
// ============================================================

#include "auth.h"

const string USERS_FILE = "users.txt";

// Change this to your own university email ending
const string EMAIL_DOMAIN = "@vu.edu";

// ------------------------------------------------------------
// File functions
// users.txt line format:
// studentId|fullName|email|department|semester|password|role
// ------------------------------------------------------------

vector<User> loadUsers() {
    vector<User> users;
    ifstream file(USERS_FILE);
    string line;

    while (getline(file, line)) {
        if (line == "") {
            continue;
        }
        vector<string> parts = splitLine(line);
        if (parts.size() < 7) {
            continue;
        }

        User user;
        user.studentId = parts[0];
        user.fullName = parts[1];
        user.email = parts[2];
        user.department = parts[3];
        user.semester = stoi(parts[4]);
        user.password = parts[5];
        user.role = parts[6];
        users.push_back(user);
    }

    file.close();
    return users;
}

void saveUsers(vector<User> users) {
    ofstream file(USERS_FILE);

    for (int i = 0; i < users.size(); i++) {
        file << users[i].studentId << "|"
             << users[i].fullName << "|"
             << users[i].email << "|"
             << users[i].department << "|"
             << users[i].semester << "|"
             << users[i].password << "|"
             << users[i].role << "\n";
    }

    file.close();
}

// Replaces the old record of this user with the new one
void updateUser(User user) {
    vector<User> users = loadUsers();
    for (int i = 0; i < users.size(); i++) {
        if (users[i].studentId == user.studentId) {
            users[i] = user;
        }
    }
    saveUsers(users);
}

bool findUser(string studentId, User &foundUser) {
    vector<User> users = loadUsers();
    for (int i = 0; i < users.size(); i++) {
        if (users[i].studentId == studentId) {
            foundUser = users[i];
            return true;
        }
    }
    return false;
}

// If no admin exists, make one: ID = admin, Password = admin123
void createDefaultAdmin() {
    vector<User> users = loadUsers();
    for (int i = 0; i < users.size(); i++) {
        if (users[i].role == "Admin") {
            return;
        }
    }

    User admin;
    admin.studentId = "admin";
    admin.fullName = "System Admin";
    admin.email = "admin" + EMAIL_DOMAIN;
    admin.department = "Administration";
    admin.semester = 0;
    admin.password = "admin123";
    admin.role = "Admin";

    users.push_back(admin);
    saveUsers(users);
}

// ------------------------------------------------------------
// Small input helpers for this module
// ------------------------------------------------------------

string askEmail(string ownerId) {
    while (true) {
        string email = toLowerCase(inputText("University Email : "));

        if (!endsWith(email, EMAIL_DOMAIN) || email.length() == EMAIL_DOMAIN.length()) {
            cout << "  Email must end with " << EMAIL_DOMAIN << "\n";
            continue;
        }

        // check that no other user has this email
        bool alreadyUsed = false;
        vector<User> users = loadUsers();
        for (int i = 0; i < users.size(); i++) {
            if (users[i].email == email && users[i].studentId != ownerId) {
                alreadyUsed = true;
            }
        }
        if (alreadyUsed) {
            cout << "  This email is already registered.\n";
            continue;
        }

        return email;
    }
}

int askSemester() {
    while (true) {
        int semester = inputNumber("Semester (1-12)  : ");
        if (semester >= 1 && semester <= 12) {
            return semester;
        }
        cout << "  Semester must be between 1 and 12.\n";
    }
}

string askNewPassword() {
    while (true) {
        string password = inputText("New Password (min 6 characters): ");
        if (password.length() < 6) {
            cout << "  Password is too short.\n";
            continue;
        }

        string confirm = inputText("Confirm Password               : ");
        if (password != confirm) {
            cout << "  Passwords do not match. Try again.\n";
            continue;
        }

        return password;
    }
}

// ------------------------------------------------------------
// 1. Registration
// ------------------------------------------------------------

void registerStudent() {
    printTitle("STUDENT REGISTRATION");

    User newUser;
    newUser.fullName = inputText("Full Name        : ");

    while (true) {
        newUser.studentId = inputText("Student ID       : ");
        User temp;

        if (!isDigitsOnly(newUser.studentId)) {
            cout << "  Student ID must contain digits only.\n";
        } else if (findUser(newUser.studentId, temp)) {
            cout << "  This Student ID is already registered.\n";
        } else {
            break;
        }
    }

    newUser.email = askEmail(newUser.studentId);
    newUser.department = inputText("Department       : ");
    newUser.semester = askSemester();
    newUser.password = askNewPassword();
    newUser.role = "Student";

    vector<User> users = loadUsers();
    users.push_back(newUser);
    saveUsers(users);

    cout << "\n  Registration successful! You can now login.\n";
    pressEnter();
}

// ------------------------------------------------------------
// 2. Login  (3 attempts allowed)
// ------------------------------------------------------------

bool loginUser(User &loggedUser) {
    printTitle("LOGIN");

    int attempts = 0;
    while (attempts < 3) {
        string id = inputText("Student ID : ");
        string password = inputText("Password   : ");

        User user;
        if (findUser(id, user) && user.password == password) {
            loggedUser = user;
            cout << "\n  Login successful! Welcome, " << user.fullName << ".\n";
            pressEnter();
            return true;
        }

        attempts++;
        cout << "  Wrong ID or password. Attempts left: " << 3 - attempts << "\n\n";
    }

    cout << "  Too many failed attempts. Going back to main menu.\n";
    pressEnter();
    return false;
}

// ------------------------------------------------------------
// 4. Student Profile
// ------------------------------------------------------------

void showProfile(User user) {
    printTitle("MY PROFILE");

    string hiddenPassword = "";
    for (int i = 0; i < user.password.length(); i++) {
        hiddenPassword = hiddenPassword + "*";
    }

    cout << "  Full Name        : " << user.fullName << "\n";
    cout << "  Student ID       : " << user.studentId << "\n";
    cout << "  University Email : " << user.email << "\n";
    cout << "  Department       : " << user.department << "\n";
    cout << "  Semester         : " << user.semester << "\n";
    cout << "  Password         : " << hiddenPassword << "\n";
    cout << "  Role             : " << user.role << "\n";
    pressEnter();
}

// ------------------------------------------------------------
// 5. Profile Edit
// ------------------------------------------------------------

void editProfile(User &user) {
    while (true) {
        printTitle("EDIT PROFILE");
        cout << "  1. Full Name    (" << user.fullName << ")\n";
        cout << "  2. Email        (" << user.email << ")\n";
        cout << "  3. Department   (" << user.department << ")\n";
        cout << "  4. Semester     (" << user.semester << ")\n";
        cout << "  0. Back\n";
        printLine();

        int choice = inputNumber("Choose what to edit: ");

        if (choice == 0) {
            return;
        } else if (choice == 1) {
            user.fullName = inputText("New Full Name    : ");
        } else if (choice == 2) {
            user.email = askEmail(user.studentId);
        } else if (choice == 3) {
            user.department = inputText("New Department   : ");
        } else if (choice == 4) {
            user.semester = askSemester();
        } else {
            cout << "  Invalid choice.\n";
            continue;
        }

        updateUser(user);
        cout << "  Profile updated successfully.\n";
        pressEnter();
    }
}

// ------------------------------------------------------------
// 6. Password Management
// ------------------------------------------------------------

void changePassword(User &user) {
    printTitle("CHANGE PASSWORD");

    string oldPassword = inputText("Current Password: ");
    if (oldPassword != user.password) {
        cout << "  Wrong current password.\n";
        pressEnter();
        return;
    }

    user.password = askNewPassword();
    updateUser(user);
    cout << "  Password changed successfully.\n";
    pressEnter();
}

void profileMenu(User &user) {
    while (true) {
        printTitle("PROFILE MENU");
        cout << "  1. View My Profile\n";
        cout << "  2. Edit Profile\n";
        cout << "  3. Change Password\n";
        cout << "  0. Back\n";
        printLine();

        int choice = inputNumber("Enter choice: ");

        if (choice == 1) {
            showProfile(user);
        } else if (choice == 2) {
            editProfile(user);
        } else if (choice == 3) {
            changePassword(user);
        } else if (choice == 0) {
            return;
        } else {
            cout << "  Invalid choice.\n";
        }
    }
}

// ------------------------------------------------------------
// 7. User Role  (Admin only)
// ------------------------------------------------------------

void showAllUsers() {
    printTitle("ALL USERS");
    vector<User> users = loadUsers();

    cout << "  " << left << setw(12) << "ID"
         << setw(22) << "Name"
         << setw(16) << "Department"
         << setw(6) << "Sem"
         << setw(10) << "Role" << "\n";
    printLine();

    for (int i = 0; i < users.size(); i++) {
        cout << "  " << left << setw(12) << users[i].studentId
             << setw(22) << shortText(users[i].fullName, 21)
             << setw(16) << shortText(users[i].department, 15)
             << setw(6) << users[i].semester
             << setw(10) << users[i].role << "\n";
    }

    cout << "\n  Total users: " << users.size() << "\n";
    pressEnter();
}

void resetUserPassword() {
    printTitle("RESET A USER'S PASSWORD");
    string id = inputText("Enter Student ID: ");

    User user;
    if (!findUser(id, user)) {
        cout << "  User not found.\n";
        pressEnter();
        return;
    }

    cout << "  Resetting password for " << user.fullName << "\n";
    user.password = askNewPassword();
    updateUser(user);
    cout << "  Password reset successfully.\n";
    pressEnter();
}

void changeUserRole(User &admin) {
    printTitle("CHANGE USER ROLE");
    string id = inputText("Enter Student ID: ");

    if (id == admin.studentId) {
        cout << "  You cannot change your own role.\n";
        pressEnter();
        return;
    }

    User user;
    if (!findUser(id, user)) {
        cout << "  User not found.\n";
        pressEnter();
        return;
    }

    string newRole = "Admin";
    if (user.role == "Admin") {
        newRole = "Student";
    }

    cout << "  " << user.fullName << " is currently: " << user.role << "\n";
    string answer = toLowerCase(inputText("Change role to " + newRole + "? (y/n): "));

    if (answer == "y") {
        user.role = newRole;
        updateUser(user);
        cout << "  Role changed to " << newRole << ".\n";
    } else {
        cout << "  No change made.\n";
    }
    pressEnter();
}

void manageUsersMenu(User &admin) {
    while (true) {
        printTitle("MANAGE USERS (ADMIN)");
        cout << "  1. View All Users\n";
        cout << "  2. Reset a User's Password\n";
        cout << "  3. Change User Role (Student / Admin)\n";
        cout << "  0. Back\n";
        printLine();

        int choice = inputNumber("Enter choice: ");

        if (choice == 1) {
            showAllUsers();
        } else if (choice == 2) {
            resetUserPassword();
        } else if (choice == 3) {
            changeUserRole(admin);
        } else if (choice == 0) {
            return;
        } else {
            cout << "  Invalid choice.\n";
        }
    }
}

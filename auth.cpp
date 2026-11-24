#include "auth.h"

vector<User> loadUsers() {
    vector<User> users;
    ifstream file("users.txt");
    string line;

    while (getline(file, line)) {
        vector<string> p = splitLine(line);
        if (p.size() < 7) {
            continue;
        }
        User u;
        u.id = p[0];
        u.name = p[1];
        u.email = p[2];
        u.department = p[3];
        u.semester = atoi(p[4].c_str());
        u.password = p[5];
        u.role = p[6];
        users.push_back(u);
    }
    file.close();
    return users;
}

void saveUsers(vector<User> users) {
    ofstream file("users.txt");
    for (int i = 0; i < users.size(); i++) {
        file << users[i].id << "|" << users[i].name << "|" << users[i].email << "|"
             << users[i].department << "|" << users[i].semester << "|"
             << users[i].password << "|" << users[i].role << "\n";
    }
    file.close();
}

void updateUser(User user) {
    vector<User> users = loadUsers();
    for (int i = 0; i < users.size(); i++) {
        if (users[i].id == user.id) {
            users[i] = user;
        }
    }
    saveUsers(users);
}

bool findUser(string id, User &user) {
    vector<User> users = loadUsers();
    for (int i = 0; i < users.size(); i++) {
        if (users[i].id == id) {
            user = users[i];
            return true;
        }
    }
    return false;
}

bool hasBar(string text) {
    return text.find('|') != string::npos;
}

int homeScreen() {

    DrawRectangle(0, 0, 430, HEIGHT, COLOR_PRIMARY);
    labelBold("Campus", 50, 170, 56, WHITE);
    labelBold("Management", 50, 230, 56, WHITE);
    labelBold("System", 50, 290, 56, WHITE);
    Color lightText = {191, 219, 254, 255};
    label("Booking  -  Lost & Found  -  Notices", 52, 370, 18, lightText);
    label("Today: " + getTodayDate(), 52, 560, 18, lightText);

    drawCard(510, 140, 420, 380);
    labelBold("Welcome!", 550, 175, 32, COLOR_TEXT);
    label("Please choose an option", 550, 218, 18, COLOR_MUTED);

    if (button("Student Login", 550, 270, 340, 48)) {
        return SCREEN_STUDENT_LOGIN;
    }
    if (colorButton("Faculty Login", 550, 330, 340, 48, COLOR_TEAL)) {
        return SCREEN_FACULTY_LOGIN;
    }
    if (colorButton("Create Account", 550, 390, 340, 48, COLOR_PURPLE)) {
        return SCREEN_REGISTER;
    }
    if (colorButton("Exit", 550, 450, 340, 40, COLOR_MUTED)) {
        return SCREEN_EXIT;
    }
    return SCREEN_HOME;
}

int regRole = 0;
char regName[64] = "";
char regId[32] = "";
char regEmail[64] = "";
char regDept[64] = "";
char regSemester[8] = "";
char regPassword[32] = "";

void clearRegisterForm() {
    regName[0] = '\0';
    regId[0] = '\0';
    regEmail[0] = '\0';
    regDept[0] = '\0';
    regSemester[0] = '\0';
    regPassword[0] = '\0';
}

int registerScreen() {
    drawHeader("Create Account", "Students and faculty");
    if (backButton()) {
        return SCREEN_HOME;
    }

    drawCard(200, 100, 600, 450);

    label("Register as", 240, 125, 16, COLOR_MUTED);
    toggleGroup("Student;Faculty", 240, 148, 160, 38, regRole);

    string idTitle = "Student ID";
    if (regRole == 1) {
        idTitle = "Faculty ID";
    }

    textField("Full Name", regName, 64, 240, 205, 250, 1);
    textField(idTitle, regId, 32, 510, 205, 250, 2);
    textField("University Email", regEmail, 64, 240, 280, 250, 3);
    textField("Department", regDept, 64, 510, 280, 250, 4);
    if (regRole == 0) {
        textField("Semester (1-12)", regSemester, 8, 240, 355, 250, 5);
    }
    passwordField("Password (min 4)", regPassword, 32, 510, 355, 250, 6);

    if (button("Create Account", 240, 465, 520, 48)) {
        string name = regName;
        string id = regId;
        string email = regEmail;
        string dept = regDept;
        string semester = regSemester;
        string password = regPassword;
        User temp;

        if (name == "" || id == "" || email == "" || dept == "" || password == "") {
            showMessage("Please fill all the fields.", false);
        } else if (hasBar(name + id + email + dept + password)) {
            showMessage("Do not use the | character.", false);
        } else if (findUser(id, temp)) {
            showMessage("This ID is already registered.", false);
        } else if (email.find('@') == string::npos) {
            showMessage("Email must contain @", false);
        } else if (regRole == 0 && (!isNumber(semester) || atoi(regSemester) < 1 || atoi(regSemester) > 12)) {
            showMessage("Semester must be a number from 1 to 12.", false);
        } else if (password.length() < 4) {
            showMessage("Password must be at least 4 characters.", false);
        } else {
            User u;
            u.id = id;
            u.name = name;
            u.email = email;
            u.department = dept;
            u.password = password;
            if (regRole == 0) {
                u.role = "Student";
                u.semester = atoi(regSemester);
            } else {
                u.role = "Faculty";
                u.semester = 0;
            }

            vector<User> users = loadUsers();
            users.push_back(u);
            saveUsers(users);

            clearRegisterForm();
            showMessage("Account created! You can login now.", true);
            return SCREEN_HOME;
        }
    }
    return SCREEN_REGISTER;
}

char loginId[32] = "";
char loginPassword[32] = "";

int loginScreen(User &user, string role) {
    drawHeader(role + " Login", "");
    if (backButton()) {
        loginId[0] = '\0';
        loginPassword[0] = '\0';
        return SCREEN_HOME;
    }

    drawCard(300, 130, 400, 380);
    labelBold("Welcome back", 340, 160, 30, COLOR_TEXT);
    label("Login with your " + role + " ID", 340, 200, 18, COLOR_MUTED);

    textField(role + " ID", loginId, 32, 340, 245, 320, 1);
    passwordField("Password", loginPassword, 32, 340, 320, 320, 2);

    Color color = COLOR_PRIMARY;
    if (role == "Faculty") {
        color = COLOR_TEAL;
    }

    if (colorButton("Login", 340, 420, 320, 48, color)) {
        User u;
        if (findUser(loginId, u) && u.password == loginPassword && u.role == role) {
            user = u;
            loginId[0] = '\0';
            loginPassword[0] = '\0';
            showMessage("Login successful! Welcome, " + u.name, true);
            return SCREEN_DASHBOARD;
        }
        showMessage("Wrong ID or password.", false);
    }

    if (role == "Student") {
        return SCREEN_STUDENT_LOGIN;
    }
    return SCREEN_FACULTY_LOGIN;
}

int dashboardScreen(User &user) {
    drawHeader("Hi, " + user.name, user.role + "  |  ID: " + user.id);
    if (colorButton("Logout", WIDTH - 140, 17, 115, 36, COLOR_RED)) {
        showMessage("You have been logged out.", true);
        return SCREEN_HOME;
    }

    string noticeText = "Read notices, join events";
    if (user.role == "Faculty") {
        noticeText = "Post notices, create events";
    }

    if (tile("My Profile", "View and edit your details", 60, 100, 420, 220, COLOR_PRIMARY)) {
        return SCREEN_PROFILE;
    }
    if (tile("Room Booking", "Book classrooms and labs", 520, 100, 420, 220, COLOR_TEAL)) {
        return SCREEN_BOOKING;
    }
    if (tile("Lost & Found", "Report and find lost items", 60, 345, 420, 220, COLOR_ORANGE)) {
        return SCREEN_LOSTFOUND;
    }
    if (tile("Notices & Events", noticeText, 520, 345, 420, 220, COLOR_PURPLE)) {
        return SCREEN_NOTICES;
    }
    return SCREEN_DASHBOARD;
}

bool profileLoaded = false;
char editName[64] = "";
char editEmail[64] = "";
char editDept[64] = "";
char editSemester[8] = "";
char oldPassword[32] = "";
char newPassword[32] = "";

int profileScreen(User &user) {

    if (!profileLoaded) {
        strcpy(editName, user.name.c_str());
        strcpy(editEmail, user.email.c_str());
        strcpy(editDept, user.department.c_str());
        strcpy(editSemester, to_string(user.semester).c_str());
        oldPassword[0] = '\0';
        newPassword[0] = '\0';
        profileLoaded = true;
    }

    drawHeader("My Profile", "");
    if (backButton()) {
        profileLoaded = false;
        return SCREEN_DASHBOARD;
    }

    drawCard(30, 100, 580, 400);
    DrawCircle(95, 165, 38, COLOR_PRIMARY);
    string letter = user.name.substr(0, 1);
    labelBold(letter, 95 - textWidth(letter, 36) / 2, 147, 36, WHITE);
    labelBold(user.name, 150, 135, 26, COLOR_TEXT);
    label("ID: " + user.id + "   |   " + user.role, 150, 172, 18, COLOR_MUTED);

    textField("Full Name", editName, 64, 60, 230, 250, 1);
    textField("Email", editEmail, 64, 330, 230, 250, 2);
    textField("Department", editDept, 64, 60, 305, 250, 3);
    if (user.role == "Student") {
        textField("Semester", editSemester, 8, 330, 305, 250, 4);
    }

    if (button("Save Changes", 60, 420, 520, 46)) {
        string name = editName;
        string email = editEmail;
        string dept = editDept;

        if (name == "" || email == "" || dept == "") {
            showMessage("Fields cannot be empty.", false);
        } else if (hasBar(name + email + dept)) {
            showMessage("Do not use the | character.", false);
        } else if (email.find('@') == string::npos) {
            showMessage("Email must contain @", false);
        } else if (user.role == "Student" && (atoi(editSemester) < 1 || atoi(editSemester) > 12)) {
            showMessage("Semester must be from 1 to 12.", false);
        } else {
            user.name = name;
            user.email = email;
            user.department = dept;
            if (user.role == "Student") {
                user.semester = atoi(editSemester);
            }
            updateUser(user);
            showMessage("Profile updated.", true);
        }
    }

    drawCard(630, 100, 340, 320);
    labelBold("Change Password", 660, 125, 22, COLOR_TEXT);
    passwordField("Old Password", oldPassword, 32, 660, 170, 280, 5);
    passwordField("New Password", newPassword, 32, 660, 245, 280, 6);

    if (colorButton("Change Password", 660, 340, 280, 46, COLOR_ORANGE)) {
        string newPass = newPassword;

        if (user.password != oldPassword) {
            showMessage("Old password is wrong.", false);
        } else if (newPass.length() < 4) {
            showMessage("New password must be at least 4 characters.", false);
        } else if (hasBar(newPass)) {
            showMessage("Do not use the | character.", false);
        } else {
            user.password = newPass;
            updateUser(user);
            oldPassword[0] = '\0';
            newPassword[0] = '\0';
            showMessage("Password changed.", true);
        }
    }

    return SCREEN_PROFILE;
}

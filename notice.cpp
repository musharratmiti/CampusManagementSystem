#include "notice.h"

const int NOTICE_TYPE_COUNT = 5;
string noticeTypes[NOTICE_TYPE_COUNT] = {"Exam Routine", "Class Suspension",
                                         "Assignment Deadline", "Result Publication",
                                         "Holiday Notice"};
Color noticeColors[NOTICE_TYPE_COUNT] = {COLOR_PRIMARY, COLOR_RED, COLOR_ORANGE,
                                         COLOR_GREEN, COLOR_PURPLE};

const int EVENT_TYPE_COUNT = 2;
string eventTypes[EVENT_TYPE_COUNT] = {"Cultural Program", "Seminar"};

vector<Notice> loadNotices() {
    vector<Notice> list;
    ifstream file("notices.txt");
    string line;

    while (getline(file, line)) {
        vector<string> p = splitLine(line);
        if (p.size() < 6) {
            continue;
        }
        Notice n;
        n.id = atoi(p[0].c_str());
        n.type = p[1];
        n.title = p[2];
        n.details = p[3];
        n.date = p[4];
        n.postedBy = p[5];
        list.push_back(n);
    }
    file.close();
    return list;
}

vector<Event> loadEvents() {
    vector<Event> list;
    ifstream file("events.txt");
    string line;

    while (getline(file, line)) {
        vector<string> p = splitLine(line);
        if (p.size() < 8) {
            continue;
        }
        Event e;
        e.id = atoi(p[0].c_str());
        e.type = p[1];
        e.name = p[2];
        e.date = p[3];
        e.time = p[4];
        e.location = p[5];
        e.description = p[6];
        e.deadline = p[7];
        list.push_back(e);
    }
    file.close();
    return list;
}

vector<string> getRegistrations(int eventId) {
    vector<string> ids;
    ifstream file("registrations.txt");
    string line;

    while (getline(file, line)) {
        vector<string> p = splitLine(line);
        if (p.size() >= 2 && atoi(p[0].c_str()) == eventId) {
            ids.push_back(p[1]);
        }
    }
    file.close();
    return ids;
}

bool isRegistered(int eventId, string studentId) {
    vector<string> ids = getRegistrations(eventId);
    for (int i = 0; i < ids.size(); i++) {
        if (ids[i] == studentId) {
            return true;
        }
    }
    return false;
}

Color getNoticeColor(string type) {
    for (int i = 0; i < NOTICE_TYPE_COUNT; i++) {
        if (noticeTypes[i] == type) {
            return noticeColors[i];
        }
    }
    return COLOR_MUTED;
}

int noticeTab = 0;
int lastNoticeTab = 0;
int noticePage = 0;

int newNoticeType = 0;
char noticeTitle[64] = "";
char noticeDetails[256] = "";

int newEventType = 0;
char eventName[64] = "";
char eventDate[16] = "";
char eventTime[16] = "";
char eventLocation[64] = "";
char eventDescription[128] = "";
char eventDeadline[16] = "";

void drawNoticeList() {
    vector<Notice> list = loadNotices();

    if (list.size() == 0) {
        label("No notices yet.", 60, 165, 18, COLOR_MUTED);
    }

    int perPage = 4;
    pager(noticePage, list.size(), perPage, 60, 540);

    for (int k = 0; k < perPage; k++) {
        int i = list.size() - 1 - (noticePage * perPage + k);
        if (i < 0) {
            break;
        }
        Notice n = list[i];
        float y = 160 + k * 94;

        float badgeWidth = textWidth(n.type, 14) + 20;
        DrawRectangleRounded(Rectangle{60, y, badgeWidth, 24}, 0.5f, 6, getNoticeColor(n.type));
        labelBold(n.type, 70, y + 4, 14, WHITE);
        labelBold(fitText(n.title, 20, 700), 75 + badgeWidth, y + 1, 20, COLOR_TEXT);

        label(fitText(n.details, 17, 870), 60, y + 32, 17, COLOR_TEXT);
        label("Posted by " + n.postedBy + " on " + n.date, 60, y + 56, 14, COLOR_MUTED);
        DrawLine(60, y + 82, 940, y + 82, COLOR_LINE);
    }
}

void drawEventList(User &user) {
    vector<Event> list = loadEvents();
    string today = getTodayDate();

    if (list.size() == 0) {
        label("No events yet.", 60, 165, 18, COLOR_MUTED);
    }

    int perPage = 3;
    pager(noticePage, list.size(), perPage, 60, 540);

    for (int k = 0; k < perPage; k++) {
        int i = list.size() - 1 - (noticePage * perPage + k);
        if (i < 0) {
            break;
        }
        Event e = list[i];
        float y = 160 + k * 125;
        bool open = dateToNumber(today) <= dateToNumber(e.deadline);

        Color typeColor = COLOR_PURPLE;
        if (e.type == "Seminar") {
            typeColor = COLOR_TEAL;
        }
        float badgeWidth = textWidth(e.type, 14) + 20;
        DrawRectangleRounded(Rectangle{60, y, badgeWidth, 24}, 0.5f, 6, typeColor);
        labelBold(e.type, 70, y + 4, 14, WHITE);
        labelBold(fitText(e.name, 22, 520), 75 + badgeWidth, y, 22, COLOR_TEXT);

        label(e.date + ", " + e.time + "   |   " + e.location, 60, y + 32, 17, COLOR_MUTED);
        label(fitText(e.description, 16, 640), 60, y + 56, 16, COLOR_TEXT);

        if (open) {
            label("Registration open until " + e.deadline, 60, y + 80, 15, COLOR_ORANGE);
        } else {
            label("Registration closed (" + e.deadline + ")", 60, y + 80, 15, COLOR_RED);
        }

        vector<string> ids = getRegistrations(e.id);

        if (user.role == "Student") {
            if (isRegistered(e.id, user.id)) {
                labelBold("Registered", 815, y + 30, 18, COLOR_GREEN);
            } else if (!open) {
                labelBold("Closed", 830, y + 30, 18, COLOR_MUTED);
            } else if (colorButton("Register", 790, y + 22, 140, 40, COLOR_GREEN)) {
                ofstream file("registrations.txt", ios::app);
                file << e.id << "|" << user.id << "\n";
                file.close();
                showMessage("Registered for " + e.name + "!", true);
            }
        } else {

            labelBold(to_string(ids.size()) + " registered", 790, y + 2, 18, COLOR_PRIMARY);
            string names = "";
            for (int j = 0; j < ids.size(); j++) {
                User s;
                if (findUser(ids[j], s)) {
                    if (names != "") {
                        names = names + ", ";
                    }
                    names = names + s.name;
                }
            }
            if (names != "") {
                label(fitText("Students: " + names, 14, 870), 60, y + 100, 14, COLOR_MUTED);
            }
        }

        DrawLine(60, y + 118, 940, y + 118, COLOR_LINE);
    }
}

void drawPostNotice(User &user) {
    labelBold("Post a New Notice", 60, 160, 22, COLOR_TEXT);

    comboBox("Notice Type", "Exam Routine;Class Suspension;Assignment Deadline;Result Publication;Holiday Notice",
             60, 200, 300, newNoticeType);
    textField("Title", noticeTitle, 64, 380, 200, 560, 1);
    textField("Details", noticeDetails, 256, 60, 280, 880, 2);

    if (colorButton("Post Notice", 60, 370, 300, 46, COLOR_PURPLE)) {
        string title = noticeTitle;
        string details = noticeDetails;

        if (title == "" || details == "") {
            showMessage("Please write a title and details.", false);
        } else if ((title + details).find('|') != string::npos) {
            showMessage("Do not use the | character.", false);
        } else {
            vector<Notice> list = loadNotices();
            ofstream file("notices.txt", ios::app);
            file << list.size() + 1 << "|" << noticeTypes[newNoticeType] << "|" << title << "|"
                 << details << "|" << getTodayDate() << "|" << user.name << "\n";
            file.close();

            noticeTitle[0] = '\0';
            noticeDetails[0] = '\0';
            showMessage("Notice posted.", true);
        }
    }
}

void drawCreateEvent() {
    labelBold("Create a New Event", 60, 160, 22, COLOR_TEXT);

    comboBox("Event Type", "Cultural Program;Seminar", 60, 200, 280, newEventType);
    textField("Event Name", eventName, 64, 360, 200, 580, 1);
    textField("Event Date (DD-MM-YYYY)", eventDate, 16, 60, 275, 280, 2);
    textField("Time (e.g. 10:00 AM)", eventTime, 16, 360, 275, 280, 3);
    textField("Location", eventLocation, 64, 660, 275, 280, 4);
    textField("Description", eventDescription, 128, 60, 350, 880, 5);
    textField("Registration Deadline (DD-MM-YYYY)", eventDeadline, 16, 60, 425, 280, 6);

    if (colorButton("Create Event", 360, 447, 280, 40, COLOR_PURPLE)) {
        string name = eventName;
        string date = eventDate;
        string time = eventTime;
        string location = eventLocation;
        string description = eventDescription;
        string deadline = eventDeadline;

        if (name == "" || time == "" || location == "" || description == "") {
            showMessage("Please fill all the fields.", false);
        } else if ((name + time + location + description).find('|') != string::npos) {
            showMessage("Do not use the | character.", false);
        } else if (!isValidDate(date) || !isValidDate(deadline)) {
            showMessage("Dates must be like 05-12-2026", false);
        } else if (dateToNumber(date) < dateToNumber(getTodayDate())) {
            showMessage("Event date cannot be in the past.", false);
        } else if (dateToNumber(deadline) > dateToNumber(date)) {
            showMessage("Deadline must be on or before the event date.", false);
        } else {
            vector<Event> list = loadEvents();
            ofstream file("events.txt", ios::app);
            file << list.size() + 1 << "|" << eventTypes[newEventType] << "|" << name << "|"
                 << date << "|" << time << "|" << location << "|" << description << "|"
                 << deadline << "\n";
            file.close();

            eventName[0] = '\0';
            eventDate[0] = '\0';
            eventTime[0] = '\0';
            eventLocation[0] = '\0';
            eventDescription[0] = '\0';
            eventDeadline[0] = '\0';
            showMessage("Event created.", true);
        }
    }
}

int noticeScreen(User &user) {
    drawHeader("Notices & Events", "");
    if (backButton()) {
        noticeTab = 0;
        noticePage = 0;
        return SCREEN_DASHBOARD;
    }

    if (user.role == "Faculty") {
        toggleGroup("Notices;Events;Post Notice;Create Event", 30, 88, 170, 38, noticeTab);
    } else {
        toggleGroup("Notices;Events", 30, 88, 170, 38, noticeTab);
    }

    if (noticeTab != lastNoticeTab) {
        noticePage = 0;
        resetFocus();
        lastNoticeTab = noticeTab;
    }

    drawCard(30, 140, 940, 445);

    if (noticeTab == 0) {
        drawNoticeList();
    } else if (noticeTab == 1) {
        drawEventList(user);
    } else if (noticeTab == 2) {
        drawPostNotice(user);
    } else if (noticeTab == 3) {
        drawCreateEvent();
    }

    return SCREEN_NOTICES;
}

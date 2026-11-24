#ifndef NOTICE_H
#define NOTICE_H

#include "common.h"
#include "auth.h"

class Notice {
public:
    int id;
    string type;
    string title;
    string details;
    string date;
    string postedBy;
};

class Event {
public:
    int id;
    string type;
    string name;
    string date;
    string time;
    string location;
    string description;
    string deadline;
};

int noticeScreen(User &user);

#endif

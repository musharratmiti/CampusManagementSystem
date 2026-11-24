#ifndef LOSTFOUND_H
#define LOSTFOUND_H

#include "common.h"
#include "auth.h"

class Item {
public:
    int id;
    string type;
    string name;
    string description;
    string location;
    string date;
    string reporterId;
    string status;
};

int lostFoundScreen(User &user);

#endif

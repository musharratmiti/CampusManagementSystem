#ifndef BOOKING_H
#define BOOKING_H

#include "common.h"
#include "auth.h"

class Booking {
public:
    int id;
    string userId;
    string room;
    string date;
    int slot;
    string purpose;
    string status;
};

int bookingScreen(User &user);

#endif

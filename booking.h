// ============================================================
// booking.h   -  MEMBER 2
// Classroom / Lab Availability & Booking
// ============================================================

#ifndef BOOKING_H
#define BOOKING_H

#include "common.h"
#include "auth.h"

class Room {
public:
    string roomId;
    string roomName;
    string roomType;      // "Classroom" or "Lab"
    int capacity;
};

class Booking {
public:
    int bookingId;
    string studentId;
    string roomId;
    string date;
    int slotNumber;       // 1 to 5
    string purpose;
    string status;        // "Booked" or "Cancelled"
};

void createDefaultRooms();

void bookingMenuStudent(User user);
void bookingMenuAdmin(User user);

#endif

// ============================================================
// booking.cpp   -  MEMBER 2
// Classroom / Lab Availability & Booking
// ============================================================

#include "booking.h"

const string ROOMS_FILE = "rooms.txt";
const string BOOKINGS_FILE = "bookings.txt";

const int TOTAL_SLOTS = 5;
const string TIME_SLOTS[TOTAL_SLOTS] = {
    "08:00-09:30",
    "09:30-11:00",
    "11:00-12:30",
    "14:00-15:30",
    "15:30-17:00"
};

// ------------------------------------------------------------
// File functions
// rooms.txt    : roomId|roomName|roomType|capacity
// bookings.txt : bookingId|studentId|roomId|date|slotNumber|purpose|status
// ------------------------------------------------------------

vector<Room> loadRooms() {
    vector<Room> rooms;
    ifstream file(ROOMS_FILE);
    string line;

    while (getline(file, line)) {
        if (line == "") {
            continue;
        }
        vector<string> parts = splitLine(line);
        if (parts.size() < 4) {
            continue;
        }

        Room room;
        room.roomId = parts[0];
        room.roomName = parts[1];
        room.roomType = parts[2];
        room.capacity = stoi(parts[3]);
        rooms.push_back(room);
    }

    file.close();
    return rooms;
}

void saveRooms(vector<Room> rooms) {
    ofstream file(ROOMS_FILE);
    for (int i = 0; i < rooms.size(); i++) {
        file << rooms[i].roomId << "|"
             << rooms[i].roomName << "|"
             << rooms[i].roomType << "|"
             << rooms[i].capacity << "\n";
    }
    file.close();
}

vector<Booking> loadBookings() {
    vector<Booking> bookings;
    ifstream file(BOOKINGS_FILE);
    string line;

    while (getline(file, line)) {
        if (line == "") {
            continue;
        }
        vector<string> parts = splitLine(line);
        if (parts.size() < 7) {
            continue;
        }

        Booking booking;
        booking.bookingId = stoi(parts[0]);
        booking.studentId = parts[1];
        booking.roomId = parts[2];
        booking.date = parts[3];
        booking.slotNumber = stoi(parts[4]);
        booking.purpose = parts[5];
        booking.status = parts[6];
        bookings.push_back(booking);
    }

    file.close();
    return bookings;
}

void saveBookings(vector<Booking> bookings) {
    ofstream file(BOOKINGS_FILE);
    for (int i = 0; i < bookings.size(); i++) {
        file << bookings[i].bookingId << "|"
             << bookings[i].studentId << "|"
             << bookings[i].roomId << "|"
             << bookings[i].date << "|"
             << bookings[i].slotNumber << "|"
             << bookings[i].purpose << "|"
             << bookings[i].status << "\n";
    }
    file.close();
}

// Adds some rooms the first time the program runs
void createDefaultRooms() {
    vector<Room> rooms = loadRooms();
    if (rooms.size() > 0) {
        return;
    }

    Room r1; r1.roomId = "R101"; r1.roomName = "Classroom 101";   r1.roomType = "Classroom"; r1.capacity = 40;
    Room r2; r2.roomId = "R102"; r2.roomName = "Classroom 102";   r2.roomType = "Classroom"; r2.capacity = 40;
    Room r3; r3.roomId = "L201"; r3.roomName = "Computer Lab 1";  r3.roomType = "Lab";       r3.capacity = 30;
    Room r4; r4.roomId = "L202"; r4.roomName = "Computer Lab 2";  r4.roomType = "Lab";       r4.capacity = 30;
    Room r5; r5.roomId = "L203"; r5.roomName = "Physics Lab";     r5.roomType = "Lab";       r5.capacity = 25;

    rooms.push_back(r1);
    rooms.push_back(r2);
    rooms.push_back(r3);
    rooms.push_back(r4);
    rooms.push_back(r5);
    saveRooms(rooms);
}

// ------------------------------------------------------------
// Helper functions
// ------------------------------------------------------------

bool findRoom(string roomId, Room &foundRoom) {
    vector<Room> rooms = loadRooms();
    for (int i = 0; i < rooms.size(); i++) {
        if (rooms[i].roomId == roomId) {
            foundRoom = rooms[i];
            return true;
        }
    }
    return false;
}

bool isSlotBooked(string roomId, string date, int slotNumber) {
    vector<Booking> bookings = loadBookings();
    for (int i = 0; i < bookings.size(); i++) {
        if (bookings[i].roomId == roomId &&
            bookings[i].date == date &&
            bookings[i].slotNumber == slotNumber &&
            bookings[i].status == "Booked") {
            return true;
        }
    }
    return false;
}

void printRoomTable() {
    vector<Room> rooms = loadRooms();

    cout << "  " << left << setw(10) << "Room ID"
         << setw(22) << "Name"
         << setw(12) << "Type"
         << setw(10) << "Capacity" << "\n";
    printLine();

    for (int i = 0; i < rooms.size(); i++) {
        cout << "  " << left << setw(10) << rooms[i].roomId
             << setw(22) << shortText(rooms[i].roomName, 21)
             << setw(12) << rooms[i].roomType
             << setw(10) << rooms[i].capacity << "\n";
    }
}

void printBookingTable(vector<Booking> list, bool showStudent) {
    if (list.size() == 0) {
        cout << "  No bookings found.\n";
        return;
    }

    cout << "  " << left << setw(6) << "ID";
    if (showStudent) {
        cout << setw(12) << "Student";
    }
    cout << setw(8) << "Room"
         << setw(13) << "Date"
         << setw(14) << "Time"
         << setw(18) << "Purpose"
         << setw(10) << "Status" << "\n";
    printLine();

    for (int i = 0; i < list.size(); i++) {
        cout << "  " << left << setw(6) << list[i].bookingId;
        if (showStudent) {
            cout << setw(12) << list[i].studentId;
        }
        cout << setw(8) << list[i].roomId
             << setw(13) << list[i].date
             << setw(14) << TIME_SLOTS[list[i].slotNumber - 1]
             << setw(18) << shortText(list[i].purpose, 17)
             << setw(10) << list[i].status << "\n";
    }
}

// ------------------------------------------------------------
// 1. Available Labs / Classrooms
// ------------------------------------------------------------

void showAllRooms() {
    printTitle("ALL CLASSROOMS & LABS");
    printRoomTable();
    pressEnter();
}

// ------------------------------------------------------------
// 2 & 3. Date + Available Time Slots
// ------------------------------------------------------------

void showAvailability() {
    printTitle("CHECK AVAILABILITY");
    string date = inputDate("Enter date (DD-MM-YYYY): ");

    vector<Room> rooms = loadRooms();

    cout << "\n  Availability on " << date << "\n\n";
    cout << "  " << left << setw(8) << "Room";
    for (int s = 0; s < TOTAL_SLOTS; s++) {
        cout << setw(13) << TIME_SLOTS[s];
    }
    cout << "\n";
    printLine();

    for (int i = 0; i < rooms.size(); i++) {
        cout << "  " << left << setw(8) << rooms[i].roomId;
        for (int s = 1; s <= TOTAL_SLOTS; s++) {
            string status = "Free";
            if (isSlotBooked(rooms[i].roomId, date, s)) {
                status = "Booked";
            }
            cout << setw(13) << status;
        }
        cout << "\n";
    }
    pressEnter();
}

// ------------------------------------------------------------
// 4. Booking Request
// ------------------------------------------------------------

void bookRoom(User user) {
    printTitle("BOOK A CLASSROOM / LAB");
    printRoomTable();
    cout << "\n";

    string roomId = toUpperCase(inputText("Enter Room ID: "));
    Room room;
    if (!findRoom(roomId, room)) {
        cout << "  Room not found.\n";
        pressEnter();
        return;
    }

    string date = inputDate("Enter date (DD-MM-YYYY): ");
    if (dateToDays(date) < dateToDays(getTodayDate())) {
        cout << "  You cannot book a past date.\n";
        pressEnter();
        return;
    }

    cout << "\n  Time slots for " << room.roomName << " on " << date << ":\n";
    for (int s = 1; s <= TOTAL_SLOTS; s++) {
        string status = "Free";
        if (isSlotBooked(room.roomId, date, s)) {
            status = "Booked";
        }
        cout << "  " << s << ". " << TIME_SLOTS[s - 1] << "   - " << status << "\n";
    }

    int slot = inputNumber("Choose slot number: ");
    if (slot < 1 || slot > TOTAL_SLOTS) {
        cout << "  Invalid slot.\n";
        pressEnter();
        return;
    }
    if (isSlotBooked(room.roomId, date, slot)) {
        cout << "  Sorry, this slot is already booked.\n";
        pressEnter();
        return;
    }

    string purpose = inputText("Purpose (e.g. Group Study): ");

    vector<Booking> bookings = loadBookings();
    Booking booking;
    booking.bookingId = bookings.size() + 1;
    booking.studentId = user.studentId;
    booking.roomId = room.roomId;
    booking.date = date;
    booking.slotNumber = slot;
    booking.purpose = purpose;
    booking.status = "Booked";

    bookings.push_back(booking);
    saveBookings(bookings);

    cout << "\n  Booking confirmed! Your Booking ID is " << booking.bookingId << ".\n";
    pressEnter();
}

// ------------------------------------------------------------
// 5. Booking History
// ------------------------------------------------------------

void showMyBookings(User user) {
    printTitle("MY BOOKING HISTORY");

    vector<Booking> bookings = loadBookings();
    vector<Booking> myList;
    for (int i = 0; i < bookings.size(); i++) {
        if (bookings[i].studentId == user.studentId) {
            myList.push_back(bookings[i]);
        }
    }

    printBookingTable(myList, false);
    pressEnter();
}

void showAllBookings() {
    printTitle("ALL BOOKINGS");
    printBookingTable(loadBookings(), true);
    pressEnter();
}

// ------------------------------------------------------------
// 6. Booking Cancel
// ------------------------------------------------------------

void cancelBooking(User user, bool isAdmin) {
    printTitle("CANCEL BOOKING");

    vector<Booking> bookings = loadBookings();

    // show the bookings this person can cancel
    vector<Booking> showList;
    for (int i = 0; i < bookings.size(); i++) {
        if (bookings[i].status == "Booked") {
            if (isAdmin || bookings[i].studentId == user.studentId) {
                showList.push_back(bookings[i]);
            }
        }
    }
    printBookingTable(showList, isAdmin);
    if (showList.size() == 0) {
        pressEnter();
        return;
    }

    int id = inputNumber("Enter Booking ID to cancel (0 = back): ");
    if (id == 0) {
        return;
    }

    bool found = false;
    for (int i = 0; i < bookings.size(); i++) {
        if (bookings[i].bookingId == id) {
            found = true;

            if (!isAdmin && bookings[i].studentId != user.studentId) {
                cout << "  This is not your booking.\n";
            } else if (bookings[i].status == "Cancelled") {
                cout << "  This booking is already cancelled.\n";
            } else if (dateToDays(bookings[i].date) < dateToDays(getTodayDate())) {
                cout << "  Past bookings cannot be cancelled.\n";
            } else {
                bookings[i].status = "Cancelled";
                saveBookings(bookings);
                cout << "  Booking cancelled successfully.\n";
            }
        }
    }

    if (!found) {
        cout << "  Booking ID not found.\n";
    }
    pressEnter();
}

// ------------------------------------------------------------
// Admin: add a new room
// ------------------------------------------------------------

void addRoom() {
    printTitle("ADD NEW CLASSROOM / LAB");

    Room room;
    room.roomId = toUpperCase(inputText("Room ID (e.g. L204): "));

    Room temp;
    if (findRoom(room.roomId, temp)) {
        cout << "  This Room ID already exists.\n";
        pressEnter();
        return;
    }

    room.roomName = inputText("Room Name          : ");

    cout << "  1. Classroom\n";
    cout << "  2. Lab\n";
    int type = inputNumber("Choose type        : ");
    if (type == 2) {
        room.roomType = "Lab";
    } else {
        room.roomType = "Classroom";
    }

    room.capacity = inputNumber("Capacity           : ");

    vector<Room> rooms = loadRooms();
    rooms.push_back(room);
    saveRooms(rooms);

    cout << "  Room added successfully.\n";
    pressEnter();
}

// ------------------------------------------------------------
// Menus
// ------------------------------------------------------------

void bookingMenuStudent(User user) {
    while (true) {
        printTitle("CLASSROOM / LAB BOOKING");
        cout << "  1. View All Classrooms & Labs\n";
        cout << "  2. Check Availability (by date)\n";
        cout << "  3. Book a Classroom / Lab\n";
        cout << "  4. My Booking History\n";
        cout << "  5. Cancel a Booking\n";
        cout << "  0. Back\n";
        printLine();

        int choice = inputNumber("Enter choice: ");

        if (choice == 1) {
            showAllRooms();
        } else if (choice == 2) {
            showAvailability();
        } else if (choice == 3) {
            bookRoom(user);
        } else if (choice == 4) {
            showMyBookings(user);
        } else if (choice == 5) {
            cancelBooking(user, false);
        } else if (choice == 0) {
            return;
        } else {
            cout << "  Invalid choice.\n";
        }
    }
}

void bookingMenuAdmin(User user) {
    while (true) {
        printTitle("ROOMS & BOOKINGS (ADMIN)");
        cout << "  1. View All Classrooms & Labs\n";
        cout << "  2. Add New Classroom / Lab\n";
        cout << "  3. Check Availability (by date)\n";
        cout << "  4. View All Bookings\n";
        cout << "  5. Cancel Any Booking\n";
        cout << "  0. Back\n";
        printLine();

        int choice = inputNumber("Enter choice: ");

        if (choice == 1) {
            showAllRooms();
        } else if (choice == 2) {
            addRoom();
        } else if (choice == 3) {
            showAvailability();
        } else if (choice == 4) {
            showAllBookings();
        } else if (choice == 5) {
            cancelBooking(user, true);
        } else if (choice == 0) {
            return;
        } else {
            cout << "  Invalid choice.\n";
        }
    }
}

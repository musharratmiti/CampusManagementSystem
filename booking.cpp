#include "booking.h"

const int ROOM_COUNT = 5;
string rooms[ROOM_COUNT] = {"R101", "R102", "L201", "L202", "L203"};
string roomTypes[ROOM_COUNT] = {"Classroom", "Classroom", "Computer Lab", "Computer Lab", "Physics Lab"};

const int SLOT_COUNT = 5;
string slotStart[SLOT_COUNT] = {"08:00", "09:30", "11:00", "14:00", "15:30"};
string slotEnd[SLOT_COUNT]   = {"09:30", "11:00", "12:30", "15:30", "17:00"};

vector<Booking> loadBookings() {
    vector<Booking> list;
    ifstream file("bookings.txt");
    string line;

    while (getline(file, line)) {
        vector<string> p = splitLine(line);
        if (p.size() < 7) {
            continue;
        }
        Booking b;
        b.id = atoi(p[0].c_str());
        b.userId = p[1];
        b.room = p[2];
        b.date = p[3];
        b.slot = atoi(p[4].c_str());
        b.purpose = p[5];
        b.status = p[6];
        list.push_back(b);
    }
    file.close();
    return list;
}

void saveBookings(vector<Booking> list) {
    ofstream file("bookings.txt");
    for (int i = 0; i < list.size(); i++) {
        file << list[i].id << "|" << list[i].userId << "|" << list[i].room << "|"
             << list[i].date << "|" << list[i].slot << "|" << list[i].purpose << "|"
             << list[i].status << "\n";
    }
    file.close();
}

bool isBooked(vector<Booking> list, string room, string date, int slot) {
    for (int i = 0; i < list.size(); i++) {
        if (list[i].room == room && list[i].date == date &&
            list[i].slot == slot && list[i].status == "Booked") {
            return true;
        }
    }
    return false;
}

bool bookingLoaded = false;
char bookingDate[16] = "";
string lastDate = "";
int selectedRoom = -1;
int selectedSlot = -1;
char purpose[64] = "";
int myBookingPage = 0;

int bookingScreen(User &user) {
    if (!bookingLoaded) {
        strcpy(bookingDate, getTodayDate().c_str());
        selectedRoom = -1;
        selectedSlot = -1;
        purpose[0] = '\0';
        myBookingPage = 0;
        bookingLoaded = true;
    }

    drawHeader("Room Booking", "Classrooms and labs");
    if (backButton()) {
        bookingLoaded = false;
        return SCREEN_DASHBOARD;
    }

    vector<Booking> all = loadBookings();
    string date = bookingDate;
    string today = getTodayDate();

    if (date != lastDate) {
        selectedRoom = -1;
        selectedSlot = -1;
        lastDate = date;
    }

    drawCard(30, 90, 610, 495);
    labelBold("Check Availability", 50, 105, 22, COLOR_TEXT);

    textField("Date (DD-MM-YYYY)", bookingDate, 16, 50, 140, 190, 1);
    if (colorButton("Today", 250, 162, 80, 38, COLOR_MUTED)) {
        strcpy(bookingDate, today.c_str());
    }

    DrawRectangleRounded(Rectangle{360, 172, 16, 16}, 0.3f, 4, COLOR_GREEN);
    label("Free", 382, 171, 16, COLOR_MUTED);
    DrawRectangleRounded(Rectangle{430, 172, 16, 16}, 0.3f, 4, COLOR_RED);
    label("Booked", 452, 171, 16, COLOR_MUTED);
    DrawRectangleRounded(Rectangle{520, 172, 16, 16}, 0.3f, 4, COLOR_PRIMARY);
    label("Selected", 542, 171, 16, COLOR_MUTED);

    if (!isValidDate(date)) {
        label("Please type a correct date, like 05-12-2026", 50, 300, 18, COLOR_RED);
    } else {

        for (int s = 0; s < SLOT_COUNT; s++) {
            float x = 170 + s * 92;
            labelBold(slotStart[s], x + 20, 220, 16, COLOR_TEXT);
            label("to " + slotEnd[s], x + 18, 238, 14, COLOR_MUTED);
        }

        for (int r = 0; r < ROOM_COUNT; r++) {
            float y = 265 + r * 48;
            labelBold(rooms[r], 50, y + 2, 18, COLOR_TEXT);
            label(roomTypes[r], 50, y + 22, 13, COLOR_MUTED);

            for (int s = 1; s <= SLOT_COUNT; s++) {
                float x = 170 + (s - 1) * 92;

                if (isBooked(all, rooms[r], date, s)) {
                    DrawRectangleRounded(Rectangle{x, y, 86, 40}, 0.3f, 16, COLOR_RED);
                    labelBold("Booked", x + 43 - textWidth("Booked", 16) / 2, y + 11, 16, WHITE);
                } else if (r == selectedRoom && s == selectedSlot) {
                    colorButton("Selected", x, y, 86, 40, COLOR_PRIMARY);
                } else if (colorButton("Free", x, y, 86, 40, COLOR_GREEN)) {
                    if (dateToNumber(date) < dateToNumber(today)) {
                        showMessage("You cannot book a past date.", false);
                    } else {
                        selectedRoom = r;
                        selectedSlot = s;
                    }
                }
            }
        }
    }

    string purposeTitle = "Purpose  (click a green box first)";
    if (selectedRoom != -1) {
        purposeTitle = "Purpose for " + rooms[selectedRoom] + ", " +
                       slotStart[selectedSlot - 1] + " - " + slotEnd[selectedSlot - 1];
    }
    textField(purposeTitle, purpose, 64, 50, 505, 360, 2);

    if (button("Book Now", 425, 527, 195, 38)) {
        string text = purpose;

        if (selectedRoom == -1) {
            showMessage("Click a green box to choose a room and time.", false);
        } else if (text == "") {
            showMessage("Please write the purpose.", false);
        } else if (text.find('|') != string::npos) {
            showMessage("Do not use the | character.", false);
        } else if (isBooked(all, rooms[selectedRoom], date, selectedSlot)) {
            showMessage("This slot is already booked.", false);
        } else {
            Booking b;
            b.id = all.size() + 1;
            b.userId = user.id;
            b.room = rooms[selectedRoom];
            b.date = date;
            b.slot = selectedSlot;
            b.purpose = text;
            b.status = "Booked";

            all.push_back(b);
            saveBookings(all);

            selectedRoom = -1;
            selectedSlot = -1;
            purpose[0] = '\0';
            showMessage("Booking done! Booking ID: " + to_string(b.id), true);
        }
    }

    drawCard(660, 90, 310, 495);
    labelBold("My Bookings", 680, 105, 22, COLOR_TEXT);

    vector<Booking> mine;
    for (int i = all.size() - 1; i >= 0; i--) {
        if (all[i].userId == user.id) {
            mine.push_back(all[i]);
        }
    }

    if (mine.size() == 0) {
        label("No bookings yet.", 680, 150, 18, COLOR_MUTED);
    }

    int perPage = 4;
    pager(myBookingPage, mine.size(), perPage, 670, 540);

    for (int k = 0; k < perPage; k++) {
        int i = myBookingPage * perPage + k;
        if (i >= mine.size()) {
            break;
        }
        Booking b = mine[i];
        float y = 145 + k * 98;

        labelBold(b.room, 680, y, 20, COLOR_TEXT);
        if (b.status == "Booked") {
            label("Booked", 880, y + 2, 16, COLOR_GREEN);
        } else {
            label("Cancelled", 870, y + 2, 16, COLOR_RED);
        }
        label(b.date + "   " + slotStart[b.slot - 1] + "-" + slotEnd[b.slot - 1], 680, y + 27, 16, COLOR_MUTED);
        label(fitText(b.purpose, 16, 170), 680, y + 50, 16, COLOR_TEXT);

        bool canCancel = b.status == "Booked" && dateToNumber(b.date) >= dateToNumber(today);
        if (canCancel && colorButton("Cancel", 870, y + 42, 85, 30, COLOR_RED)) {
            for (int j = 0; j < all.size(); j++) {
                if (all[j].id == b.id) {
                    all[j].status = "Cancelled";
                }
            }
            saveBookings(all);
            showMessage("Booking cancelled.", true);
        }

        DrawLine(680, y + 85, 950, y + 85, COLOR_LINE);
    }

    return SCREEN_BOOKING;
}

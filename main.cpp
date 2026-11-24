#include "common.h"
#include "auth.h"
#include "booking.h"
#include "lostfound.h"
#include "notice.h"

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "Campus Management System");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    setupTheme();

    int screen = SCREEN_HOME;
    User user;

    while (!WindowShouldClose() && screen != SCREEN_EXIT) {
        BeginDrawing();
        ClearBackground(COLOR_BG);

        int next = screen;

        if (screen == SCREEN_HOME) {
            next = homeScreen();
        } else if (screen == SCREEN_REGISTER) {
            next = registerScreen();
        } else if (screen == SCREEN_STUDENT_LOGIN) {
            next = loginScreen(user, "Student");
        } else if (screen == SCREEN_FACULTY_LOGIN) {
            next = loginScreen(user, "Faculty");
        } else if (screen == SCREEN_DASHBOARD) {
            next = dashboardScreen(user);
        } else if (screen == SCREEN_PROFILE) {
            next = profileScreen(user);
        } else if (screen == SCREEN_BOOKING) {
            next = bookingScreen(user);
        } else if (screen == SCREEN_LOSTFOUND) {
            next = lostFoundScreen(user);
        } else if (screen == SCREEN_NOTICES) {
            next = noticeScreen(user);
        }

        drawMessage();
        EndDrawing();

        if (next != screen) {
            resetFocus();
            screen = next;
        }
    }

    closeTheme();
    CloseWindow();
    return 0;
}

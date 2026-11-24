#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"
#include "raygui.h"

#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <ctime>

using namespace std;

const int WIDTH = 1000;
const int HEIGHT = 650;

const int SCREEN_EXIT = -1;
const int SCREEN_HOME = 0;
const int SCREEN_REGISTER = 1;
const int SCREEN_STUDENT_LOGIN = 2;
const int SCREEN_FACULTY_LOGIN = 3;
const int SCREEN_DASHBOARD = 4;
const int SCREEN_PROFILE = 5;
const int SCREEN_BOOKING = 6;
const int SCREEN_LOSTFOUND = 7;
const int SCREEN_NOTICES = 8;

const Color COLOR_PRIMARY = {37, 99, 235, 255};
const Color COLOR_DARK    = {30, 64, 175, 255};
const Color COLOR_BG      = {241, 245, 249, 255};
const Color COLOR_CARD    = {255, 255, 255, 255};
const Color COLOR_TEXT    = {30, 41, 59, 255};
const Color COLOR_MUTED   = {100, 116, 139, 255};
const Color COLOR_LINE    = {203, 213, 225, 255};
const Color COLOR_GREEN   = {22, 163, 74, 255};
const Color COLOR_RED     = {220, 38, 38, 255};
const Color COLOR_ORANGE  = {234, 88, 12, 255};
const Color COLOR_PURPLE  = {124, 58, 237, 255};
const Color COLOR_TEAL    = {13, 148, 136, 255};

void setupTheme();
void closeTheme();

void drawHeader(string title, string subtitle);
void drawCard(float x, float y, float w, float h);
void label(string text, float x, float y, int size, Color color);
void labelBold(string text, float x, float y, int size, Color color);
float textWidth(string text, int size);
string fitText(string text, int size, float maxWidth);

bool button(string text, float x, float y, float w, float h);
bool colorButton(string text, float x, float y, float w, float h, Color color);
bool backButton();
bool tile(string title, string subtitle, float x, float y, float w, float h, Color color);
void textField(string title, char *text, int size, float x, float y, float w, int id);
void passwordField(string title, char *text, int size, float x, float y, float w, int id);
void toggleGroup(string options, float x, float y, float w, float h, int &active);
void comboBox(string title, string options, float x, float y, float w, int &active);
void pager(int &page, int total, int perPage, float x, float y);
void resetFocus();

void showMessage(string text, bool success);
void drawMessage();

vector<string> splitLine(string line);
string toLowerCase(string text);
bool isNumber(string text);
bool isValidDate(string date);
int dateToNumber(string date);
string getTodayDate();

#endif

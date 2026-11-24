#define RAYGUI_IMPLEMENTATION
#include "common.h"

Font normalFont;
Font boldFont;
int activeField = -1;
string messageText = "";
bool messageSuccess = true;
double messageTime = -100;

void setupTheme() {
    if (FileExists("fonts/Roboto-Regular.ttf") && FileExists("fonts/Roboto-Bold.ttf")) {
        normalFont = LoadFontEx("fonts/Roboto-Regular.ttf", 48, 0, 0);
        boldFont = LoadFontEx("fonts/Roboto-Bold.ttf", 48, 0, 0);
        SetTextureFilter(normalFont.texture, TEXTURE_FILTER_BILINEAR);
        SetTextureFilter(boldFont.texture, TEXTURE_FILTER_BILINEAR);
    } else {

        normalFont = GetFontDefault();
        boldFont = GetFontDefault();
    }

    GuiSetFont(normalFont);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
    GuiSetStyle(DEFAULT, TEXT_SPACING, 1);

    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(COLOR_LINE));
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(COLOR_CARD));
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(COLOR_TEXT));

    Color lightBlue = {239, 246, 255, 255};
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, ColorToInt(COLOR_PRIMARY));
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(lightBlue));
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(COLOR_TEXT));

    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, ColorToInt(COLOR_PRIMARY));
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, ColorToInt(COLOR_PRIMARY));
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(WHITE));

    GuiSetStyle(TEXTBOX, BASE_COLOR_PRESSED, ColorToInt(lightBlue));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_PRESSED, ColorToInt(COLOR_TEXT));
    GuiSetStyle(TEXTBOX, TEXT_PADDING, 10);
}

void closeTheme() {
    if (normalFont.texture.id != GetFontDefault().texture.id) {
        UnloadFont(normalFont);
        UnloadFont(boldFont);
    }
}

void label(string text, float x, float y, int size, Color color) {
    DrawTextEx(normalFont, text.c_str(), Vector2{x, y}, size, 1, color);
}

void labelBold(string text, float x, float y, int size, Color color) {
    DrawTextEx(boldFont, text.c_str(), Vector2{x, y}, size, 1, color);
}

float textWidth(string text, int size) {
    return MeasureTextEx(normalFont, text.c_str(), size, 1).x;
}

string fitText(string text, int size, float maxWidth) {
    if (textWidth(text, size) <= maxWidth) {
        return text;
    }
    while (text.length() > 0 && textWidth(text + "...", size) > maxWidth) {
        text = text.substr(0, text.length() - 1);
    }
    return text + "...";
}

void drawHeader(string title, string subtitle) {
    DrawRectangle(0, 0, WIDTH, 70, COLOR_PRIMARY);
    labelBold(title, 30, 18, 32, WHITE);
    Color lightText = {191, 219, 254, 255};
    label(subtitle, 40 + textWidth(title, 32) + 20, 28, 18, lightText);
}

void drawCard(float x, float y, float w, float h) {
    Rectangle box = {x, y, w, h};
    DrawRectangleRounded(box, 0.04f, 16, COLOR_CARD);
    DrawRectangleRoundedLines(box, 0.04f, 16, 1, COLOR_LINE);
}

bool colorButton(string text, float x, float y, float w, float h, Color color) {
    Rectangle box = {x, y, w, h};
    bool mouseOver = CheckCollisionPointRec(GetMousePosition(), box);

    Color fill = color;
    if (mouseOver) {
        fill = ColorBrightness(color, -0.15f);
    }
    DrawRectangleRounded(box, 0.3f, 16, fill);

    int size = 18;
    float tx = x + (w - textWidth(text, size)) / 2;
    float ty = y + (h - size) / 2;
    labelBold(text, tx, ty, size, WHITE);

    return mouseOver && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

bool button(string text, float x, float y, float w, float h) {
    return colorButton(text, x, y, w, h, COLOR_PRIMARY);
}

bool backButton() {
    return colorButton("< Back", WIDTH - 140, 17, 115, 36, COLOR_DARK);
}

bool tile(string title, string subtitle, float x, float y, float w, float h, Color color) {
    Rectangle box = {x, y, w, h};
    bool mouseOver = CheckCollisionPointRec(GetMousePosition(), box);

    Color fill = color;
    if (mouseOver) {
        fill = ColorBrightness(color, -0.12f);
    }
    DrawRectangleRounded(box, 0.12f, 16, fill);

    DrawCircle(x + 50, y + 50, 26, Fade(WHITE, 0.25f));
    string letter = title.substr(0, 1);
    labelBold(letter, x + 50 - textWidth(letter, 32) / 2, y + 34, 32, WHITE);

    labelBold(title, x + 25, y + h - 75, 26, WHITE);
    label(subtitle, x + 25, y + h - 40, 18, Fade(WHITE, 0.85f));

    return mouseOver && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

void textField(string title, char *text, int size, float x, float y, float w, int id) {
    label(title, x, y, 16, COLOR_MUTED);

    bool typing = (activeField == id);
    if (GuiTextBox(Rectangle{x, y + 22, w, 38}, text, size, typing)) {
        if (typing) {
            activeField = -1;
        } else {
            activeField = id;
        }
    }
}

void passwordField(string title, char *text, int size, float x, float y, float w, int id) {
    if (activeField == id) {
        textField(title, text, size, x, y, w, id);
        return;
    }

    char stars[64] = "";
    int length = strlen(text);
    if (length > 60) {
        length = 60;
    }
    for (int i = 0; i < length; i++) {
        stars[i] = '*';
    }
    stars[length] = '\0';

    label(title, x, y, 16, COLOR_MUTED);
    if (GuiTextBox(Rectangle{x, y + 22, w, 38}, stars, 64, false)) {
        activeField = id;
    }
}

void toggleGroup(string options, float x, float y, float w, float h, int &active) {
    GuiToggleGroup(Rectangle{x, y, w, h}, options.c_str(), &active);
}

void comboBox(string title, string options, float x, float y, float w, int &active) {
    label(title, x, y, 16, COLOR_MUTED);
    GuiComboBox(Rectangle{x, y + 22, w, 38}, options.c_str(), &active);
}

void pager(int &page, int total, int perPage, float x, float y) {
    int pages = (total + perPage - 1) / perPage;
    if (pages < 1) {
        pages = 1;
    }
    if (page >= pages) {
        page = pages - 1;
    }

    if (page > 0 && colorButton("< Prev", x, y, 90, 32, COLOR_MUTED)) {
        page--;
    }
    label("Page " + to_string(page + 1) + " of " + to_string(pages), x + 100, y + 7, 16, COLOR_MUTED);
    if (page < pages - 1 && colorButton("Next >", x + 200, y, 90, 32, COLOR_MUTED)) {
        page++;
    }
}

void resetFocus() {
    activeField = -1;
}

void showMessage(string text, bool success) {
    messageText = text;
    messageSuccess = success;
    messageTime = GetTime();
}

void drawMessage() {
    if (GetTime() - messageTime > 3.0) {
        return;
    }

    Color color = COLOR_RED;
    if (messageSuccess) {
        color = COLOR_GREEN;
    }

    float w = textWidth(messageText, 18) + 50;
    Rectangle box = {(WIDTH - w) / 2, HEIGHT - 62, w, 44};
    DrawRectangleRounded(box, 0.5f, 16, color);
    labelBold(messageText, box.x + 25, box.y + 12, 18, WHITE);
}

vector<string> splitLine(string line) {
    vector<string> parts;
    string part = "";

    for (int i = 0; i < line.length(); i++) {
        if (line[i] == '|') {
            parts.push_back(part);
            part = "";
        } else if (line[i] != '\r') {
            part = part + line[i];
        }
    }
    parts.push_back(part);
    return parts;
}

string toLowerCase(string text) {
    for (int i = 0; i < text.length(); i++) {
        if (text[i] >= 'A' && text[i] <= 'Z') {
            text[i] = text[i] + 32;
        }
    }
    return text;
}

bool isNumber(string text) {
    if (text.length() == 0 || text.length() > 8) {
        return false;
    }
    for (int i = 0; i < text.length(); i++) {
        if (text[i] < '0' || text[i] > '9') {
            return false;
        }
    }
    return true;
}

bool isValidDate(string date) {
    if (date.length() != 10 || date[2] != '-' || date[5] != '-') {
        return false;
    }
    if (!isNumber(date.substr(0, 2)) || !isNumber(date.substr(3, 2)) || !isNumber(date.substr(6, 4))) {
        return false;
    }

    int day = atoi(date.substr(0, 2).c_str());
    int month = atoi(date.substr(3, 2).c_str());
    int year = atoi(date.substr(6, 4).c_str());

    return day >= 1 && day <= 31 && month >= 1 && month <= 12 && year >= 2000;
}

int dateToNumber(string date) {
    string day = date.substr(0, 2);
    string month = date.substr(3, 2);
    string year = date.substr(6, 4);
    return atoi((year + month + day).c_str());
}

string getTodayDate() {
    time_t now = time(0);
    tm *t = localtime(&now);

    char buffer[11];
    strftime(buffer, 11, "%d-%m-%Y", t);
    return string(buffer);
}

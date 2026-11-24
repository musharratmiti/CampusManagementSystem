#include "lostfound.h"

vector<Item> loadItems() {
    vector<Item> list;
    ifstream file("items.txt");
    string line;

    while (getline(file, line)) {
        vector<string> p = splitLine(line);
        if (p.size() < 8) {
            continue;
        }
        Item it;
        it.id = atoi(p[0].c_str());
        it.type = p[1];
        it.name = p[2];
        it.description = p[3];
        it.location = p[4];
        it.date = p[5];
        it.reporterId = p[6];
        it.status = p[7];
        list.push_back(it);
    }
    file.close();
    return list;
}

void saveItems(vector<Item> list) {
    ofstream file("items.txt");
    for (int i = 0; i < list.size(); i++) {
        file << list[i].id << "|" << list[i].type << "|" << list[i].name << "|"
             << list[i].description << "|" << list[i].location << "|" << list[i].date << "|"
             << list[i].reporterId << "|" << list[i].status << "\n";
    }
    file.close();
}

bool isMatch(string name1, string name2) {
    name1 = toLowerCase(name1) + " ";
    name2 = toLowerCase(name2);

    string word = "";
    for (int i = 0; i < name1.length(); i++) {
        if (name1[i] == ' ') {
            if (word.length() >= 3 && name2.find(word) != string::npos) {
                return true;
            }
            word = "";
        } else {
            word = word + name1[i];
        }
    }
    return false;
}

vector<Item> findMatches(vector<Item> all, Item item) {
    vector<Item> result;
    for (int i = 0; i < all.size(); i++) {
        if (all[i].type != item.type && all[i].status == "Open" && isMatch(item.name, all[i].name)) {
            result.push_back(all[i]);
        }
    }
    return result;
}

bool lostFoundLoaded = false;
int reportType = 0;
char itemName[64] = "";
char itemDescription[128] = "";
char itemLocation[64] = "";
char itemDate[16] = "";

int listTab = 0;
int lastTab = 0;
int listPage = 0;
int matchItemId = -1;

void drawItem(Item it, float y) {
    Color typeColor = COLOR_ORANGE;
    if (it.type == "FOUND") {
        typeColor = COLOR_TEAL;
    }
    float badgeWidth = textWidth(it.type, 14) + 20;
    DrawRectangleRounded(Rectangle{430, y, badgeWidth, 22}, 0.5f, 6, typeColor);
    labelBold(it.type, 440, y + 3, 14, WHITE);
    labelBold(fitText(it.name, 20, 260), 445 + badgeWidth, y, 20, COLOR_TEXT);

    label(fitText(it.location + "   |   " + it.date, 16, 380), 430, y + 28, 16, COLOR_MUTED);
    label(fitText(it.description, 16, 380), 430, y + 50, 16, COLOR_TEXT);

    User reporter;
    if (findUser(it.reporterId, reporter)) {
        label(fitText("Contact: " + reporter.name + " (" + reporter.email + ")", 14, 380),
              430, y + 72, 14, COLOR_PRIMARY);
    }
    DrawLine(430, y + 95, 950, y + 95, COLOR_LINE);
}

int lostFoundScreen(User &user) {
    if (!lostFoundLoaded) {
        strcpy(itemDate, getTodayDate().c_str());
        listTab = 0;
        listPage = 0;
        matchItemId = -1;
        lostFoundLoaded = true;
    }

    drawHeader("Lost & Found", "Report items and find matches");
    if (backButton()) {
        lostFoundLoaded = false;
        return SCREEN_DASHBOARD;
    }

    vector<Item> all = loadItems();

    drawCard(30, 90, 360, 495);
    labelBold("Report an Item", 50, 105, 22, COLOR_TEXT);
    toggleGroup("I Lost It;I Found It", 50, 142, 159, 38, reportType);

    textField("Item Name", itemName, 64, 50, 195, 320, 1);
    textField("Description", itemDescription, 128, 50, 265, 320, 2);
    textField("Location", itemLocation, 64, 50, 335, 320, 3);
    textField("Date (DD-MM-YYYY)", itemDate, 16, 50, 405, 320, 4);

    if (colorButton("Report Item", 50, 500, 320, 46, COLOR_ORANGE)) {
        string name = itemName;
        string description = itemDescription;
        string location = itemLocation;
        string date = itemDate;

        if (name == "" || description == "" || location == "") {
            showMessage("Please fill all the fields.", false);
        } else if ((name + description + location).find('|') != string::npos) {
            showMessage("Do not use the | character.", false);
        } else if (!isValidDate(date)) {
            showMessage("Date must be like 05-12-2026", false);
        } else {
            Item it;
            it.id = all.size() + 1;
            it.type = "LOST";
            if (reportType == 1) {
                it.type = "FOUND";
            }
            it.name = name;
            it.description = description;
            it.location = location;
            it.date = date;
            it.reporterId = user.id;
            it.status = "Open";

            all.push_back(it);
            saveItems(all);

            itemName[0] = '\0';
            itemDescription[0] = '\0';
            itemLocation[0] = '\0';

            matchItemId = it.id;
            listTab = 3;
            showMessage("Item reported! Checking for matches...", true);
        }
    }

    drawCard(410, 90, 560, 495);
    toggleGroup("Lost Items;Found Items;My Items;Matches", 425, 105, 130, 36, listTab);

    if (listTab != lastTab) {
        listPage = 0;
        lastTab = listTab;
    }

    vector<Item> show;
    for (int i = all.size() - 1; i >= 0; i--) {
        Item it = all[i];
        if (listTab == 0 && it.type == "LOST" && it.status == "Open") {
            show.push_back(it);
        } else if (listTab == 1 && it.type == "FOUND" && it.status == "Open") {
            show.push_back(it);
        } else if (listTab == 2 && it.reporterId == user.id) {
            show.push_back(it);
        }
    }

    float startY = 160;
    if (listTab == 3) {
        Item target;
        bool found = false;
        for (int i = 0; i < all.size(); i++) {
            if (all[i].id == matchItemId) {
                target = all[i];
                found = true;
            }
        }

        if (!found) {
            label("Go to 'My Items' and click 'Matches' on one of your items.", 430, 170, 16, COLOR_MUTED);
            return SCREEN_LOSTFOUND;
        }

        labelBold(fitText("Matches for: " + target.name + " (" + target.type + ")", 18, 520), 430, 155, 18, COLOR_PURPLE);
        show = findMatches(all, target);
        startY = 190;

        if (show.size() == 0) {
            label("No match found yet. Check again later.", 430, 200, 18, COLOR_MUTED);
        }
    } else if (show.size() == 0) {
        label("No items to show.", 430, 170, 18, COLOR_MUTED);
    }

    int perPage = 3;
    pager(listPage, show.size(), perPage, 430, 540);

    for (int k = 0; k < perPage; k++) {
        int i = listPage * perPage + k;
        if (i >= show.size()) {
            break;
        }
        Item it = show[i];
        float y = startY + k * 115;
        drawItem(it, y);

        if (listTab == 2) {
            if (it.status == "Returned") {
                labelBold("Returned", 860, y, 16, COLOR_GREEN);
            } else {
                if (colorButton("Matches", 845, y - 2, 105, 30, COLOR_PURPLE)) {
                    matchItemId = it.id;
                    listTab = 3;
                }
                if (colorButton("Returned", 845, y + 34, 105, 30, COLOR_GREEN)) {
                    for (int j = 0; j < all.size(); j++) {
                        if (all[j].id == it.id) {
                            all[j].status = "Returned";
                        }
                    }
                    saveItems(all);
                    showMessage("Item marked as returned.", true);
                }
            }
        }
    }

    return SCREEN_LOSTFOUND;
}

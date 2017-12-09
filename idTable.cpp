//
// Created by Andy on 2017/11/27.
//

#include "main.h"
#include <iomanip>

const int startIndex = 0;

std::vector<tabElement> idTable;
std::vector<tabElement> staticTable;
int addressIndex;

void insertTable(kind cls, type typ, const char name[], int length, int level, int addr) {
    tabElement element;
    if (level == 0){ // Globals
        if (cls == funcs) { // Function
            addressIndex = startIndex;
        }
        if (cls == consts) { // Addr for constant stands for its value
            element.addr = addr;
        } else {
            element.addr = 0;
        }
        strcpy(element.name, name);
        element.cls = cls;
        element.typ = typ;
        element.length = length;
        element.level = level;
        idTable.push_back(element);
        staticTable.push_back(element);
    } else { // Locals
        if (cls == consts) { // Addr for constant stands for its value
            element.addr = addr;
        } else if (cls == vars || cls == params) {
            element.addr = addressIndex;
            if (length != 0) { // Array
                addressIndex = addressIndex + length;
            } else {
                addressIndex++;
            }
        }
        strcpy(element.name, name);
        element.cls = cls;
        element.typ = typ;
        element.length = length;
        element.level = level;
        idTable.push_back(element);
        staticTable.push_back(element);
    }
}

void popTable() {
    idTable.pop_back();
}

// Return index of identifier in idTable, -1 means ot found
int lookUp(char name[]) {
    if (idTable.size() == 0) {
        return -1;
    }
    for (int i = idTable.size() - 1; i >= 0; i--) {
        if (strcmp(idTable[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Return index of identifier in staticTable, -1 means not found
int lookUpStatic(char name[]) {
    if (staticTable.size() == 0) {
        return -1;
    }
    for (int i = staticTable.size() - 1; i >= 0; i--) {
        if (strcmp(staticTable[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

bool isDefinable(char name[]) {
    if (lookUp(name) == -1) { // Identifier with this name never exists
        return true;
    } else if (idTable[lookUp(name)].level < level) { // Identifier with this name's level is shallower
        return true;
    }
    return false;
}

int findCurrentFunc() {
    if (idTable.size() == 0) {
        return -1;
    }
    for (int i = idTable.size() - 1; i >= 0; --i) {
        if (idTable[i].cls == funcs) {
            return i;
        }
    }
    return -1; // No function occurred before
}

void printTable(){
    cout << endl;
    cout << "Name      "
         << "Kind      "
         << "Type      "
         << "Address   "
         << "Length    "
         << "Level     "
         << endl;
    for (int i = 0; i < idTable.size(); ++i) {
        printf("%-*s", 10, idTable[i].name);
        printf("%-*d", 10, idTable[i].cls);
        printf("%-*d", 10, idTable[i].typ);
        printf("%-*d", 10, idTable[i].addr);
        printf("%-*d", 10, idTable[i].length);
        printf("%-*d", 10, idTable[i].level);
        printf("\n");
    }
}

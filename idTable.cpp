//
// Created by Andy on 2017/11/27.
//

#include "main.h"
#include <iomanip>

const int startIndex = 0;

std::vector<tabElement> idTable;
std::vector<tabElement> staticTable;
int addressIndex = startIndex;

void insertTable(kind cls, type typ, const char name[], int length, int level, int addr) {
    tabElement element;
    if (level == 0){ // Globals
        if (cls == consts) { // Addr for constant stands for its value
            element.addr = addr;
        } else if (cls == vars){
            element.addr = addressIndex;
            if (length != 0) { // Array
                addressIndex = addressIndex + length;
            } else {
                addressIndex++;
            }
        } else { // Function or main
            element.addr = 0;
            addressIndex = startIndex;
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

void popElement() {
    idTable.pop_back();
}

void popLocals() {
    for (int i = idTable.size() - 1; i >= 0 ; --i) { // Pop local variables from id table
        if (idTable[i].level != 0) {
            popElement();
        } else { // All local variables popped from id table
            break;
        }
    }
}

// Return index of identifier in idTable, -1 means ot found
int lookUp(const char *name) {
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

// Return index of certain function in staticTable, -1 means not found
int lookUpStatic(const char *functionName) {
    if (staticTable.size() == 0) {
        return -1;
    }
    for (int i = 0; i < staticTable.size(); i++) {
        if (strcmp(staticTable[i].name, functionName) == 0 && staticTable[i].cls == funcs) {
            return i;
        }
    }
    return -1;
}

// Return index of certain constant/variable/parameter inside certain function in staticTable, -1 means not found
int lookUpStatic(const char *functionName, const char *identifier) {
    int funcIndex = lookUpStatic(functionName);
    if (staticTable.size() == 0 || funcIndex == -1) {
        return -1;
    }
    for (int i = funcIndex + 1; i < staticTable.size() &&
            (staticTable[i].cls != funcs); ++i) { // Look up in local
        if (strcmp(staticTable[i].name, identifier) == 0) {
            return i;
        }
    }
    for (int j = 0; j < staticTable.size() &&
                    (staticTable[j].cls != funcs); ++j) { // Look up in global
        if (strcmp(staticTable[j].name, identifier) == 0) {
            return j;
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
    cout << "--------------------Identifier Table-------------------" << endl;
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
    cout << "----------------------Static Table---------------------" << endl;
    cout << "Name      "
         << "Kind      "
         << "Type      "
         << "Address   "
         << "Length    "
         << "Level     "
         << endl;
    for (int i = 0; i < staticTable.size(); ++i) {
        printf("%-*s", 10, staticTable[i].name);
        printf("%-*d", 10, staticTable[i].cls);
        printf("%-*d", 10, staticTable[i].typ);
        printf("%-*d", 10, staticTable[i].addr);
        printf("%-*d", 10, staticTable[i].length);
        printf("%-*d", 10, staticTable[i].level);
        printf("\n");
    }
}

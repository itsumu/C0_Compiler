//
// Created by Andy on 2017/11/27.
//

#include "main.h"
std::vector<tabElement> idTable;

void insertTable(char name[], kind cls, type typ, int level, int addr, int length);
void popTable();
int lookUp(char name[]); // Return index of identifier in table
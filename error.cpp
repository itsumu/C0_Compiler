//
// Created by Andy on 2017/11/22.
//

#include "main.h"

char* errorMessage[] = {
    "Number starts with zero.", // 0
    "Number too large or too long.", // 1
    "! without = backward.", // 2
    "Empty character.", // 3
    "Illegal character between ''.", // 4
    "Character more than one or ' lost.", // 5
    "Empty string.", // 6
    "Illegal character inside string.", // 7
    "String longer than 50.", // 8
    "Unknown character." // 9
};
void error(int errorNum) {
    cout << "[Line "<< linePtr << "] Error: " << errorMessage[errorNum] << endl;
}
//
// Created by Andy on 2017/11/22.
//

#include "main.h"

string errorMessages[] = {
    "Number starts with zero.", // 0
    "Number too large or too long.", // 1
    "! without = backward.", // 2
    "Empty character.", // 3
    "Illegal character between ''.", // 4
    "Character more than one or ' lost.", // 5
    "Empty string.", // 6
    "Illegal character inside string.", // 7
    "String longer than 50.", // 8
    "Unknown word.", // 9
    "Sign before zero.",
    "Number error.",
    "Not a variable or function.",
    "Identifier repeatedly define.",
    "Char const error.",
    "Const definition type error",
    "Array index error.",
    "Definition head error.",
    "Identifier not define.",
    "Invalid factor.",
    "Identifier kind not match.",
    "Step length couldn't be 0.",
    "Case element not a constant.",
    "Parameter count exceeds.",
    "Illegal type.",
    "Printf symbol lost.",
    "Scanf symbol lost.",
    "Return symbol lost.",
    "Default symbol lost.",
    "Colon lost.",
    "Case symbol lost.",
    "Switch symbol lost.",
    "Plus or minus lost.",
    "For symbol lost.",
    "If symbol lost.",
    "Becomes symbol lost.",
    "Void symbol lost.",
    "Left brace lost.",
    "Right brace lost.",
    "Left parenthesis lost.",
    "Right parenthesis lost.",
    "Right bracket lost.",
    "Identifier lost.",
    "Semicolon lost.",
    "Assignment symbol lost.",
    "Type definition lost.",
    "Step length lost",
    "Statement error",
    "Illegal syntax."
};

string warningMessage[] = {
    "Type conflicts." // 0
};
void error(int errorNum) {
    cout << "[Line "<< linePtr << "] Error: " << errorMessages[errorNum] << endl;
}

void warn(int warnNum) {
    cout << "[Line "<< linePtr << "] Warning: " << warningMessage[warnNum] << endl;
}
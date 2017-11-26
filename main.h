//
// Created by Andy on 2017/11/19.
//

#ifndef C0_COMPILER_MAIN_H
#define C0_COMPILER_MAIN_H

#include "iostream"
#include "fstream"
#include "map"
#include "vector"
#include "string"

using std::cin;
using std::cout;
using std::endl;
using std::ios;
using std::fstream;
using std::map;
using std::vector;
using std::string;

const int maxIdentLength = 10;
const int maxLineLength = 121;
const int keywordCount = 14;
const int maxDigit = 15;
const int maxInt = 32767; // 2 ^ 15-1

typedef char alpha[maxIdentLength + 1]; // Alphabet letter should be restricted to maxIdentLength
typedef enum symbol {
    identi,
    constsy,
    intsy, charsy,
    voidsy,
    intcon, charcon, stringcon,
    plus, minus, times, idiv,
    eql, neq, gtr, geq, lss, leq,
    becomes,
    lparent, rparent, lbrack, rbrack, lbrace, rbrace,
    comma, semicolon, colon,
    ifsy, elsesy,
    switchsy, casesy, defaultsy,
    forsy,
    scanfsy, printfsy,
    mainsy,
    returnsy
}; // All C0 reserved words
typedef enum type {
    ints, chars, voids
}; // All C0 types
typedef enum kind {
    consts,
    vars,
    params,
    funcs
}; // All C0 kinds

extern char ch; // Current read character
extern alpha token;
extern symbol sy;
extern int inum;
extern char str[];

extern char line[]; // One source code line
extern bool skipFlag;
extern alpha keywords[];
extern symbol keySymbols[];
extern map<char, symbol> specialSymbols;
extern fstream inputFile, outputFile;

extern int charPtr;
extern int linePtr;

extern int lineLength; // Current read line length
extern int stringLength;

typedef struct{
    char name[10]; // Identifier
    int link; // Index of last element in tab
    kind cls; // Const or variable or function
    type typ; // Int or char
    int level; // Block level where elements lies
    int addr;
    int length;
} tab; // Symbol table


void error(int errorNum); // Output error position & error number
void setup(); // Set up reserved words
void nextch(); // Store one source code line into line[] and store one char into ch
void insymbol(); // Read one symbol, sy stores symbol type, inum stores contents

#endif //C0_COMPILER_MAIN_H

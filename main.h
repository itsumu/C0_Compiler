//
// Created by Andy on 2017/11/19.
//

#ifndef C0_COMPILER_MAIN_H
#define C0_COMPILER_MAIN_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <cstring>
#include <cstdlib>

using std::cin;
using std::cout;
using std::endl;
using std::ios;
using std::fstream;
using std::map;

// Restricts
const int maxIdentLength = 10;
const int maxLineLength = 121;
const int keywordCount = 14;
const int maxDigit = 15;
const int maxInt = 32767; // 2 ^ 15-1

// Types
typedef char alpha[maxIdentLength + 1]; // Alphabet letter should be restricted to maxIdentLength
typedef enum {
    identi, // 0
    constsy, // 1
    intsy, charsy, // 2, 3
    voidsy, // 4
    intcon, charcon, stringcon, // 5, 6, 7
    plus, minus, times, idiv, // 8, 9, 10, 11
    eql, neq, gtr, geq, lss, leq, // 12, 13, 14, 15, 16, 17
    becomes, // 18
    lparent, rparent, lbrack, rbrack, lbrace, rbrace, // 19, 20, 21, 22, 23, 24
    comma, semicolon, colon, // 25, 26, 27
    ifsy, elsesy, // 28, 29
    switchsy, casesy, defaultsy, // 30, 31, 32
    forsy, // 33
    scanfsy, printfsy, // 34, 35
    mainsy, // 36
    returnsy // 37
} symbol; // All C0 reserved words
typedef enum {
    ints, chars, voids
} type; // All C0 types
typedef enum {
    consts,
    vars,
    params,
    funcs
} kind; // All C0 kindshttps://sourceforge.net/projects/codeblocks/files/Binaries/13.12/Windows/
typedef struct {
    //char name[maxIdentLength]; // Identifier
    kind cls; // Const or variable or function
    type typ; // Int or char
    int level; // Block level where elements lies
    int addr;
    int length;
} tabElement; // Symbol table element

// Static elements
extern alpha keywords[];
extern symbol keySymbols[];
extern map<char, symbol> specialSymbols;

// Global Variables changing all the way
extern char line[]; // One source code line
extern char ch; // Currently reading character
extern alpha token;
extern symbol sy;
extern int inum;
extern char str[];
extern std::vector<tabElement> idTable;
extern fstream inputFile, outputFile;

// Lengths for global variables
extern int lineLength; // Currently reading line length
extern int stringLength;

// Pointers(Indices) for global variables
extern int charPtr;
extern int linePtr;
extern std::vector<int> funcPtrs;
extern int idTablePtr;

// Flags
extern bool skipFlag;

// Functions
    // General actions
void error(int errorNum); // Output error position & error number
void setup(); // Set up reserved words
void emit(); // Emit middle code

    // Lexical actions
void nextch(); // Store one source code line into line[] and store one char into ch
void insymbol(); // Read one symbol, sy stores symbol type, inum stores contents

    // Table actions
void insertTable(char name[], kind cls, type typ, int level, int addr, int length);
void popTable();
int lookUp(char name[]); // Return index of identifier in table

    // Syntax actions
void program();
void constState();
void constDef();
void varState();
void varDef();
void funcWithRetDef();
void funcWithoutRetDef();
void defHead();
void parameter();
void parameterTable();
void complexState();
void mainDef();
void factor();
void term();
void expression();
void emptyState();
void assignState();
void ifState();
void judgement(); // Judgement inside if statement
void loopState();
void conditionState();
void conditionTable();
void conditionBranch();
void conditionDefault();
void funcWithRetCall();
void funcWithoutRetCall();
void valueParamTable();
void returnState();
void scanfState();
void printfState();
void stateList();
void statement();
void integer();

#endif //C0_COMPILER_MAIN_H

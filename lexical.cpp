//
// Created by Andy on 2017/11/19.
//
#include "main.h"

#define isLetter(a) (a >= 'a' && a <= 'z' || a >= 'A' && a <= 'Z' || a =='_')
#define isMyAscii(a) (a == 32 || a == 33 || a >= 35 && a <= 126)
#define specials '+', '-', '*', '/', '(', ')', '[', ']', '{', '}', ',', ';', ':'

const int maxStrLength = 50;

char ch = ' ';
alpha token;
symbol sy;
int inum;
char str[maxStrLength + 1]; // +1 for \0

char line[maxLineLength + 1]; // +1 for \0
bool skipFlag = false, errorFlag = false;
alpha keywords[keywordCount];
symbol keySymbols[keywordCount];
map<char, symbol> specialSymbols;
fstream inputFile, outputFile;

int lineLength = 0;
int stringLength = 0;

int charPtr = 0;
int linePtr = 1;



// Set up reserved words
void setup() {

    // Keyword strings
    char* keywordList[] = {
       "const     ",
       "int       ",
       "char      ",
       "void      ",
       "if        ",
       "else      ",
       "switch    ",
       "case      ",
       "default   ",
       "for       ",
       "scanf     ",
       "printf    ",
       "main      ",
       "return    "
    };
    for (int i = 0; i < keywordCount; i++) {
        strcpy(keywords[i], keywordList[i]);
    }

    // Keyword symbols
    symbol keySymbolList[] = {
        constsy, intsy, charsy, voidsy, ifsy, elsesy, switchsy, casesy, defaultsy, forsy,
        scanfsy, printfsy, mainsy, returnsy
    };
    for (int i = 0; i < keywordCount; i++) {
        keySymbols[i] = keySymbolList[i];
    }

    // Special symbols
    char specialSymbolsChar[] = {
        specials
    };
    symbol specialSymbolList[] = {
        plus, minus, times, idiv, lparent, rparent, lbrack, rbrack, lbrace, rbrace,
        comma, semicolon, colon
    };
    for (int i = 0; i < sizeof(specialSymbolList) / sizeof(specialSymbolList[0]); i++) {
        specialSymbols[specialSymbolsChar[i]] = specialSymbolList[i];
    }
}

void fatal(){}

// Store one source code line into line[] and store one char into ch
void nextch() {
    if (charPtr == lineLength) {
        if (inputFile.eof()) { 
            cout << endl;
            cout << "program incomplete" << endl;
            return;
        }
//        if (errpos != 0) {
//            if (skipFlag) endskip;
//            outputFile << endl;
//            errpos = 0;
//        }
//        write( outputFile, lc: 5, ' '); {* 没有错误执行的操作，在list文件中输出当前PCODE的行号以及一个空格，不换行 *}
        lineLength = 0;
        charPtr = 0;
        linePtr++; // New line entered
        char tempChar;
        while (inputFile.peek() != '\n') { // 仍需增加一行字符的长度越界判断
            tempChar = inputFile.get();
//            cout << tempChar << endl;
            line[lineLength] = tempChar;
            lineLength++;
        }
        line[lineLength] = inputFile.get(); // Get '\n'
        lineLength++;
        line[lineLength] = '\0';
//        cout << endl;
    }
    ch = line[charPtr];
    charPtr++;
}

// Clear str's contents
void clearStr() {
    for (int i = 0; i < maxStrLength; ++i) {
        str[i] = '\0';
    }
}

// Set up reserved words
void insymbol() {
    int i, j, k = 0;

    while (isspace(ch)) nextch();
    if (isLetter(ch)) { // Identifier or keyword
        // Initiate token
        for (i = 0; i < maxIdentLength; ++i) {
            token[i] = ' ';
        }
        token[i] = '\0';

        // Read whole alpha into token, token can't be longer than 10 characters
        do {
            if (k < maxIdentLength) {
                token[k] = ch;
                k++;
            }
            nextch();
        } while (isLetter(ch) || isdigit(ch));

        for (k = 0; k < keywordCount; k++) {
            if (strcmp(token, keywords[k]) == 0) {
                break;
            }
        }
        if (k < keywordCount) sy = keySymbols[k];
        else sy = identi;
    } else if (isdigit(ch)) { // Integer
        if (ch == '0') { // Int start with 0 (must be 0)
            inum = 0;
            sy = intcon;
            nextch();
            if (isdigit(ch)) { // Error & read till the last number
                error(0);
                nextch();
                while (isdigit(ch)){
                    nextch();
                }
            }
        } else { // Int start with no 0
            inum = 0;
            sy = intcon;
            for (; isdigit(ch); k++) {
                inum = inum * 10 + ch - '0';
                nextch();
            }
        }
    if( k > maxDigit  ||  inum > maxInt ) {
        error(1);
        inum = 0;
        k = 0;
    }
    } else if (ch == '<') { // Leq or lss
        nextch();
        if (ch == '=') {
            k++;
            sy = leq;
            nextch();
        } else {
            sy = lss;
        }
    } else if (ch == '>') { // Beq or bgt
        nextch();
        if (ch == '=') {
            k++;
            sy = geq;
            nextch();
        } else {
            sy = gtr;
        }
    } else if (ch == '=') { // Equal or becomes
        nextch();
        if (ch == '=') {
            k++;
            sy = eql;
            nextch();
        } else {
            sy = becomes;
        }
    } else if (ch == '!') { // Not equal
        nextch();
        if (ch == '=') {
            sy = neq;
            nextch();
        } else {
            error(2);
            sy = neq;
        }
    } else if (ch == '\'') { // Character
        nextch();
        if (ch == '\'') {
            error(3); // Empty char
            sy = charcon;
            inum = 0;
        } else { // One or more characters in quotation marks
            if (ch != '+' && ch != '-' && ch != '*' && ch != '/'
                    && !isdigit(ch) && !isLetter(ch)) {
                error(4); // Illigal character
                sy = charcon;
                inum = 0;
            } else { // Probable character
                nextch();
                if (ch != '\''){ // Characters more than one
                    errorFlag = true;
                    error(5);
                    sy = charcon;
                    inum = 0;
                    while (ch != '\'' && ch != '\n') nextch(); // End till '\n' or '
                    return;
                } else { // Correct character
                    sy = charcon;
                    inum = ch;
                }
            }
        }
        nextch();
    } else if (ch == '"') { // String
        nextch();
        if (ch == '"') { // Empty string
            error(6);
            sy = stringcon;
            inum = 0;
        } else { // String inside
            clearStr();
            k = 0;
            do {
                if (!isMyAscii(ch)) { // Illegal my ascii
                    errorFlag = true;
                    error(7);
                    sy = stringcon;
                    inum = 0;
                    while (ch != '"' && isspace(ch)) nextch(); // End till space or "
                    return;
                }
                if (k == maxStrLength) { // String too long
                    error(8);
                    sy = stringcon;
                    inum = 0;
                    while (ch != '"' && isspace(ch)) nextch(); // End till space or "
                    return;
                }
                str[k] = ch;
                k++;
                nextch();
            } while (ch != '"');
            sy = stringcon;
            inum = 0;
        }
        nextch();
    } else if (ch == '+' || ch == '-' || ch ==  '*' || ch ==  '/'  ||
        ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' ||
        ch == '}' || ch == ',' || ch == ';' || ch == ':') { // Symbols with single letter
        sy = specialSymbols[ch];
        nextch();
    } else {
        error(9); // Illegal character head
        nextch();
        insymbol();
    }
}
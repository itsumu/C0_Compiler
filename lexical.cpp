//
// Created by Andy on 2017/11/19.
//
#include "main.h"

#define isLetter(a) (a >= 'a' && a <= 'z' || a >= 'A' && a <= 'Z' || a =='_')
#define isMyAscii(a) (a == 32 || a == 33 || a >= 35 && a <= 126)

const int maxStrLength = 1000;

char ch = ' ';
alpha token;
symbol sy;
int inum;
char str[maxStrLength + 1]; // +1 for \0

char line[maxLineLength + 1]; // +1 for \0
alpha keywords[keywordCount];
symbol keySymbols[keywordCount];
map<char, symbol> specialSymbols;
fstream inputFile, infixFile;

int lineLength = 0;

int charPtr = 0;
int linePtr = 0;

void fatal(){}

// Store one source code line into line[] and store one char into ch
void nextch() {
    if (charPtr == lineLength) {
        if (inputFile.eof()) {
            cout << endl;
            cout << "Compiling finished." << endl;
            exit(0);
        }
//        if (errpos != 0) {
//            if (skipFlag) endskip;
//            infixFile << endl;
//            errpos = 0;
//        }
//        write( infixFile, lc: 5, ' '); {* 没有错误执行的操作，在list文件中输出当前PCODE的行号以及一个空格，不换行 *}
        lineLength = 0;
        charPtr = 0;
        linePtr++; // New line entered
        char tempChar;
        while (inputFile.peek() != '\n' && inputFile.peek() != EOF) { // 仍需增加一行字符的长度越界判断
            tempChar = inputFile.get();
//            cout << tempChar << endl;
            line[lineLength] = tempChar;
            lineLength++;
        }
        line[lineLength] = inputFile.get(); // Get '\n' or EOF
        lineLength++;
        line[lineLength] = '\0';
//        cout << endl;
    }
    ch = line[charPtr];
    charPtr++;
}

// Clear str's contents to \0
void clearStr() {
    for (int i = 0; i <= maxStrLength; ++i) {
        str[i] = '\0';
    }
}

// Clear token's contents to \0
void clearToken() {
    for (int i = 0; i <= maxIdentLength; ++i) {
        token[i] = '\0';
    }
}

// Set up reserved words
void insymbol() {
    int k = 0;

    while (isspace(ch)) nextch();
    if (isLetter(ch)) { // Identifier or keyword
        // Initiate token
        clearToken();

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
                error(LeadingZero);
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
        error(NumRangeExceed);
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
            error(ExclamationMark);
            sy = neq;
        }
    } else if (ch == '\'') { // Character
        nextch();
        if (ch == '\'') {
            error(EmptyChar); // Empty char
            sy = charcon;
            inum = 0;
        } else { // One or more characters in quotation marks
            if (ch != '+' && ch != '-' && ch != '*' && ch != '/'
                    && !isdigit(ch) && !isLetter(ch)) {
                error(IllegalChar); // Illigal character
                sy = charcon;
                inum = 0;
            } else { // Probable character
                inum = ch; // Store ch for probable output
                nextch();
                if (ch != '\''){ // Characters more than one
                    errorFlag = true;
                    error(QuotMarkLost);
                    sy = charcon;
                    inum = 0;
                    while (ch != '\'' && ch != '\n') nextch(); // End till '\n' or '
                    return;
                } else { // Correct character
                    sy = charcon; // inum is already set before
                }
            }
        }
        nextch();
    } else if (ch == '"') { // String
        clearStr();
        nextch();
        if (ch == '"') { // Empty string
            sy = stringcon;
            inum = 0;
        } else { // String inside
            k = 0;
            do {
                if (!isMyAscii(ch)) { // Illegal my ascii
                    error(IllegalStr);
                    sy = stringcon;
                    inum = 0;
                    while (ch != '"' && isspace(ch)) nextch(); // End till space or "
                    return;
                }
                if (k == maxStrLength) { // String too long
                    error(StrRangeExceed);
                    sy = stringcon;
                    inum = 0;
                    while (ch != '"' && isspace(ch)) nextch(); // End till space or "
                    return;
                }
                str[k] = ch;
                k++;
                if (ch == '\\') { // Keep string output same as origin
                    str[k] = '\\';
                    k++;
                }
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
    } else if (ch == EOF) {
        printTable();
        outputInfixes();
        printf("Compiling finished.");
        return;
    } else {
        error(UnknownWord); // Illegal character head
        nextch();
        insymbol();
    }
}

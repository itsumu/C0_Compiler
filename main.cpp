#include "main.h"

#define specials '+', '-', '*', '/', '(', ')', '[', ']', '{', '}', ',', ';', ':'

char* symbolNames[] = {
    "Ident",
    "Const",
    "Int", "Char",
    "Void",
    "IntCon", "CharCon", "StringCon",
    "Plus", "Minus", "Times", "Div",
    "Eql", "Neq", "Gtr", "Geq", "Lss", "Leq",
    "Becomes",
    "Lparent", "Rparent", "Lbrack", "Rbrack", "Lbrace", "Rbrace",
    "Comma", "Semicolon", "Colon",
    "If", "Else",
    "Switch", "Case", "Default",
    "For",
    "Scanf", "Printf",
    "Main",
    "Return"
};

void setup() {
    // File initialization
    infixFile.open("infixes.txt", ios::out);
    // Table initialization
    idTable.reserve(10000);
    staticTable.reserve(10000);
    infixTable.reserve(10000);

    // Keyword strings
    char* keywordList[] = {
            "const",
            "int",
            "char",
            "void",
            "if",
            "else",
            "switch",
            "case",
            "default",
            "for",
            "scanf",
            "printf",
            "main",
            "return"
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

int main(int argc, char* argv[]) {
    setup();
    char filename[100];
    cout << "Please input file name (path included): ";
    cin >> filename;
    inputFile.open(filename, ios::in);
    program();
    if (errorFlag) {
        return 0;
    }
    optimizeInfixes();
    mipsProgram("mipsCode.asm");
    optMipsProgram("betterMips.asm");
    return 0;
}
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
    string filename;
    cout << "Please input file name (path included): ";
    cin >> filename;
    inputFile.open(filename, ios::in);
    program();
    if (errorFlag) {
        return 0;
    }
    mipsProgram("mipsCode.asm");
    return 0;
}

/*int outputCount = 1;
    while (true) {
        insymbol();
        cout << outputCount << " ";
        outputCount++;
        switch (sy) {
            case identi: {
                cout << "Identifier " << token << endl;
                break;
            }
            case constsy:
            case intsy:
            case charsy:
            case voidsy:
            case ifsy:
            case elsesy:
            case switchsy:
            case casesy:
            case defaultsy:
            case forsy:
            case scanfsy:
            case printfsy:
            case mainsy:
            case returnsy: {
                cout << symbolNames[sy] << " " << token << endl;
                break;
            }
            case intcon: {
                cout << symbolNames[sy] << " " << inum << endl;
                break;
            }
            case charcon: {
                cout << symbolNames[sy]  << " \'" << char(inum) << "\'" << endl;
                break;
            }
            case stringcon: {
                cout << symbolNames[sy] << " \"" << str << "\"" << endl;
                break;
            }
            case plus:
                cout << symbolNames[sy] << " +" << endl;
                break;
            case minus:
                cout << symbolNames[sy] << " -" << endl;
                break;
            case times:
                cout << symbolNames[sy] << " *" << endl;
                break;
            case idiv:
                cout << symbolNames[sy] << " /" << endl;
                break;
            case eql:
                cout << symbolNames[sy] << " ==" << endl;
                break;
            case leq: {
                cout << symbolNames[sy] << " <=" << endl;
                break;
            }
            case lss: {
                cout << symbolNames[sy] << " <" << endl;
                break;
            }
            case geq: {
                cout << symbolNames[sy] << " >=" << endl;
                break;
            }
            case gtr: {
                cout << symbolNames[sy] << " >" << endl;
                break;
            }
            case neq: {
                cout << symbolNames[sy] << " !=" << endl;
                break;
            }
            case becomes:
                cout << symbolNames[sy] << " =" << endl;
                break;
            case lparent:
                cout << symbolNames[sy] << " (" << endl;
                break;
            case rparent:
                cout << symbolNames[sy] << " )" << endl;
                break;
            case lbrack:
                cout << symbolNames[sy] << "  [" << endl;
                break;
            case rbrack:
                cout << symbolNames[sy] << "  ]" << endl;
                break;
            case lbrace:
                cout << symbolNames[sy] << "  {" << endl;
                break;
            case rbrace:
                cout << symbolNames[sy] << "  }" << endl;
                break;
            case comma:
                cout << symbolNames[sy] << "  ," << endl;
                break;
            case semicolon:
                cout << symbolNames[sy] << "  ;" << endl;
                break;
            case colon:
                cout << symbolNames[sy] << "  :" << endl;
                break;
            default: cout << "Error occurred." << endl;
        }
    }*/
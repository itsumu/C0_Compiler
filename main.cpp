#include "main.h"
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

int main(int argc, char* argv[]) {
    setup();
    inputFile.open(argv[1], ios::in);
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
    program();
    return 0;
}

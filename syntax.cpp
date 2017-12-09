//
// Created by Andy on 2017/11/27.
//

#include "main.h"

#define isCompare(a) (a == eql || a == neq || a == gtr || a == geq || a == lss || a == leq)

int level = 0;
int paramCount = 0;
int labelIndex = 0;
const string labelStr = "LABEL";

void integer(string &infixString) {
    symbol tempSy;

    if (sy == plus || sy == minus) {
        tempSy = sy;
        insymbol();
        if (sy == intcon) {
            if (inum != 0) {
                if (tempSy == minus) { // Get the negative number
                    inum = inum * (-1);
                }
                infixString = toString(inum);
                insymbol();
            } else {
                //error(); // Sign before zero
            }
        } else {
            //error(); // Number error
        }
    } else if (sy == intcon) {
        infixString = toString(inum);
        insymbol();
    } else {
        //error() // Number error
    }
}

// ＜程序＞ ::= ［＜常量说明＞]［＜变量说明＞]{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
void program() {
    insymbol();
    if (sy == constsy) { // Constant statements
        constState();
    }

    if (sy == intsy || sy == charsy) { // Variable statements or definition of function with return
        int tempCharPtr = charPtr; // Store current charPtr for later recovering
        char tempCh = ch;
        symbol tempSy = sy;
        insymbol();
        if (sy == identi) {
            insymbol();
        } else {
            //error() // Not a variable or function
        }
        if (sy == lparent) { // funcWithRetDef
            charPtr = tempCharPtr; // Recover charPtr
            ch = tempCh;
            sy = tempSy;
            // Go to function definition
        } else if (sy == lbrack ||
                   sy == comma || sy == semicolon){ // Variables statements
            charPtr = tempCharPtr; // Recover charPtr
            ch = tempCh;
            sy = tempSy;
            globalVarState();
        } else {
            //error(); // Not a variable or function
        }
    }

    // Function statements
    while (sy == intsy || sy == charsy || sy == voidsy) {
        if (sy == intsy || sy == charsy) { // Functions with returns
            funcWithRetDef();
        } else { // Functions without returns or main
            int tempCharPtr = charPtr;
            char tempCh = ch;
            symbol tempSy = sy;
            insymbol();
            if (sy == identi && sy != mainsy) { // General functions
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                funcWithoutRetDef();
            } else if (sy == mainsy) { // Main
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                break;
            } else {
                // error(); // Identifier lost
            }
        }
    }

    // Main
    if (sy == voidsy) {
        mainDef();
    } else {
        //error(); // Unallowed syntax in program
    }
}

// ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
void constState() {
    while (sy == constsy) {
        insymbol();
        constDef();
        if (sy == semicolon) {
            insymbol();
        } else {
            //error(); // Semicolon lost
        }
    }
    cout << "This is a const declaration statement." << endl;
}

/* ＜常量定义＞ ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}
| char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞} */
void constDef() {
    char idName[100];
    string infixString;

    if (sy == intsy) { // Int const
        do {
            insymbol();
            if (sy == identi) {
                strcpy(idName, token);
                insymbol();
            } else {
                //error(); // Const identifier error
            }
            if (sy == becomes) {
                insymbol();
                integer(infixString); // Manage integer
                if (isDefinable(idName)) {
                    insertTable(consts, ints, idName, 0, level, strToInt(infixString));
                    insertInfix("CONST", infixString, "int", idName);
                } else {
                    //error(); // Identifier repeatedly define
                }
            } else {
                //error(); // Assignment symbol error
            }
        } while (sy == comma);
    } else if (sy == charsy) { // Char const
        do{
            insymbol();
            if (sy == identi) {
                strcpy(idName, token);
                insymbol();
            } else {
                //error(); // Const identifier error
            }
            if (sy == becomes) {
                insymbol();
            } else {
                //error(); // Assignment symbol error
            }
            if (sy == charcon) {
                if (isDefinable(idName)) {
                    insertTable(consts, chars, idName, 0, level, inum);
                    insertInfix("CONST", toString(inum), "char", idName);
                } else {
                    //error(); // Identifier repeatedly define
                }
                insymbol();
            } else {
                //error(); // Char const error
            }
        }while (sy == comma);
    } else {
        // error(); // Const definition type error
    }
}

// ＜全局变量说明＞ ::= ＜变量定义＞;{＜变量定义＞;} （考虑变量定义后可能出现的有返回值函数）
void globalVarState() {
    int tempCharPtr;
    char tempCh;
    symbol tempSy;
    do {
        tempCharPtr = charPtr;
        tempCh = ch;
        tempSy = sy;
        insymbol();
        if (sy == identi) {
            insymbol();
        } else {
            //error(); // Identifier lost
        }
        if (sy == lparent) { // Function occurred, get back
            charPtr = tempCharPtr;
            ch = tempCh;
            sy = tempSy;
            cout << "This is a global variable declaration statement." << endl;
            return;
        } else { // Normal variable, continue
            charPtr = tempCharPtr;
            ch = tempCh;
            sy = tempSy;
        }
        varDef();
        if (sy == semicolon) {
            insymbol();
        } else {
            //error(); // Semicolon lost
        }
    } while (sy == intsy || sy == charsy);
    cout << "This is a global variable declaration statement." << endl;
}

// ＜变量说明＞ ::= ＜变量定义＞;{＜变量定义＞;}
void varState() {
    do {
        varDef();
        if (sy == semicolon) {
            insymbol();
        } else {
            //error(); // Semicolon lost
        }
    } while (sy == intsy || sy == charsy);
    cout << "This is a variable declaration statement." << endl;
}

// ＜变量定义＞ ::= ＜类型标识符＞(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’){,＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’}
void varDef() {
    char idName[100];
    type varType;

    if (sy == intsy || sy == charsy) { // Right variable(s)
        if (sy == intsy) {
            varType = ints;
        } else {
            varType = chars;
        }
        insymbol();
    } else {
        //error(); // Type error
    }
    if (sy == identi) {
        strcpy(idName, token);
        insymbol();
    } else {
        //error(); // Identifier lost
    }
    if (sy == lbrack) { // Following are array management
        insymbol();
        if (sy == intcon && inum != 0) {
            insymbol();
            if (isDefinable(idName)) {
                insertTable(vars, varType, idName, inum, level, 0);
                insertInfix("VAR", toString(inum), varType == ints ? "int" : "char", idName);
            } else {
                //error(); // Identifier repeatedly define
            }
        } else {
            //error(); // Array index error
        }
        if (sy == rbrack) {
            insymbol();
        } else {
            //error(); // Array right bracket lost
        }
    } else { // General variable
        if (isDefinable(idName)) {
            insertTable(vars, varType, idName, 0, level, 0);
            insertInfix("VAR", " ", varType == ints ? "int" : "char", idName);
        } else {
            //error(); // Identifier repeatedly define
        }
    }
    while (sy == comma) {
        insymbol();
        if (sy == identi) {
            strcpy(idName, token);
            insymbol();
        } else {
            //error(); // Identifier lost
        }
        if (sy == lbrack) { // Following are array management
            insymbol();
            if (sy == intcon) {
                insymbol();
                if (isDefinable(idName)) {
                    insertTable(vars, varType, idName, inum, level, 0);
                    insertInfix("VAR", toString(inum), varType == ints ? "int" : "char", idName);
                } else {
                    //error(); // Identifier repeatedly define
                }
            } else {
                //error(); // Array index error
            }
            if (sy == rbrack) {
                insymbol();
            } else {
                //error(); // Array right bracket lost
            }
        } else { // General variable
            if (isDefinable(idName)) {
                insertTable(vars, varType, idName, 0, level, 0); // length = 0, addr automatic set
                insertInfix("VAR", " ", varType == ints ? "int" : "char", idName);
            } else {
                //error(); // Identifier repeatedly define
            }
        }
    }
}

// ＜有返回值函数定义＞ ::= ＜声明头部＞‘(’＜参数＞|＜空＞‘)’ ‘{’＜复合语句＞‘}’
void funcWithRetDef() {
    char idName[100];
    string idNameString;
    paramCount = 0;
    type returnType;

    defHead(&returnType, idName); // Manage definition head
    if (isDefinable(idName)) { // Insert function
        insertTable(funcs, returnType, idName, paramCount, level, 0); // paramCount = 0
        idNameString = idName;
        insertInfix("FUNC", " ", returnType == ints ? "int" : "char", idNameString);
    } else {
        //error(); // Identifier repeatedly define
    }

    level++; // Dig deeper
    if (sy == lparent) {
        insymbol();
    } else {
        //error(); // Left parenthesis lost
    }
    if (sy == rparent) { // Empty parameter
        insymbol();
    } else {
        parameter(); // Manage parameter
        idTable[lookUp(idName)].length = paramCount; // Set parameters
        if (sy == rparent) {
            insymbol();
        } else {
            //error(); // Right parenthesis lost
        }
    }
    if (sy == lbrace) {
        insymbol();
    } else {
        //error(); // Left brace lost
    }
    complexState(); // Manage complex statement
    if (sy == rbrace) {
        cout << "This is a definition of function with return." << endl;
        insymbol();
    } else {
        //error(); // Right brace lost
    }

    // Ending work
    for (int i = idTable.size() - 1; i >= 0 ; --i) { // Pop local variables from id table
        if (idTable[i].level != 0) {
            popTable();
        } else { // All local variables popped from id table
            break;
        }
    }
    level--; // Back to ground
}

// ＜无返回值函数定义＞ ::= void＜标识符＞‘(’＜参数＞|＜空＞‘)’‘{’＜复合语句＞‘}’
void funcWithoutRetDef() {
    char idName[100];
    paramCount = 0;

    level++; // Dig deeper
    if (sy == voidsy) { // Manage definition head
        insymbol();
    } else {
        //error(); // Void lost
    }
    if (sy == identi) {
        if (isDefinable(token)) { // Insert function
            string idNameString = token;
            insertTable(funcs, voids, token, paramCount, level - 1, 0); // paramCount = 0
            insertInfix("FUNC", " ", "void", idNameString);
        } else {
            //error(); // Identifier repeatedly define
        }
        strcpy(idName, token);
        insymbol();
    } else {
        //error(); // Identifier lost
    }
    if (sy == lparent) {
        insymbol();
    } else {
        //error(); // Left parenthesis lost
    }
    if (sy == rparent) { // Empty parameter
        insymbol();
    } else {
        parameter(); // Manage parameter
        idTable[lookUp(idName)].length = paramCount; // Set parameters
        if (sy == rparent) {
            insymbol();
        } else {
            //error(); // Right parenthesis lost
        }
    }
    if (sy == lbrace) {
        insymbol();
    } else {
        //error(); // Left brace lost
    }
    complexState(); // Manage complex statement
    if (sy == rbrace) {
        cout << "This is a definition of function without return." << endl;
        insymbol();
    } else {
        //error(); // Right brace lost
    }

    // Ending work
    for (int i = idTable.size() - 1; i >= 0 ; --i) { // Pop local variables from id table
        if (idTable[i].level != 0) {
            popTable();
        } else { // All local variables popped from id table
            break;
        }
    }
    level--; // Back to ground
}

// ＜声明头部＞ ::=  int＜标识符＞ |char＜标识符＞ （返回值为标识符名）
void defHead(type* returnType, char* identifier) {
    char idName[100];

    if (sy == intsy) {
        *returnType = ints;
        insymbol();
    } else if (sy == charsy) {
        *returnType = chars;
        insymbol();
    } else {
        //error; // Definition head error
    }
    if (sy == identi) {
        strcpy(identifier, token);
        insymbol();
    } else {
        //error; // Identifier lost
    }
}

// ＜参数＞ ::= ＜参数表＞
void parameter() {
    parameterTable();
}

// ＜参数表＞ ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}
void parameterTable() {
    type paramType;

    if (sy == intsy || sy == charsy) {
        if (sy == intsy) {
            paramType = ints;
        } else {
            paramType = chars;
        }
        insymbol();
    } else {
        //error(); // Type lost
    }
    if (sy == identi) {
        paramCount++;
        if (isDefinable(token)) {
            insertTable(params, paramType, token, 0, level, 0);
            insertInfix("PARAM", " ", paramType == ints ? "int" : "char", token);
        } else {
            //error(); // Identifier repeatedly define
        }
        insymbol();
    } else {
        //error(); // Identifier lost
    }
    while (sy == comma) {
        insymbol();
        if (sy == intsy || sy == charsy) {
            if (sy == intsy) {
                paramType = ints;
            } else {
                paramType = chars;
            }
            insymbol();
        } else {
            //error(); // Type definition lost
        }
        if (sy == identi) {
            paramCount++;
            if (isDefinable(token)) {
                insertTable(params, paramType, token, 0, level, 0);
            } else {
                //error(); // Identifier repeatedly define
            }
            insymbol();
        } else {
            //error(); // Identifier lost
        }
    }
}

// ＜复合语句＞ ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
void complexState() {
    while (sy == constsy) {
        constState();
    }
    while (sy == intsy || sy == charsy) {
        varState();
    }
    stateList();
}

// ＜主函数＞ ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
void mainDef() {
    level++;
    if (sy == voidsy) { // Manage definition head
        insymbol();
    } else {
        //error(); // Void lost
    }
    if (sy == mainsy) {
        if (isDefinable(token)) {
            insertTable(funcs, voids, token, 0, level - 1, 0); // paramCount = 0
        } else {
            //error(); // Identifier repeatedly define
        }
        insymbol();
    } else {
        //error(); // Identifier lost
    }
    if (sy == lparent) {
        insymbol();
    } else {
        //error(); // Left parenthesis lost
    }
    if (sy == rparent) {
        insymbol();
    } else {
        //error(); // Right parenthesis lost
    }
    if (sy == lbrace) {
        insymbol();
    } else {
        //error(); // Left brace lost
    }
    insertInfix("FUNC", " ", "void", "main");
    complexState(); // Manage complex statement
    if (sy == rbrace) {
        cout << "This is the main function." << endl;
        insymbol();
    } else {
        //error(); // Right brace lost
    }
    level--;
}

// ＜因子＞ ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’
type factor(string &infixString) {
    type firstType;

    switch (sy) {
        case identi: {
            int idIndex;
            int tempCharPtr = charPtr;
            char tempCh = ch;
            symbol tempSy = sy;

            idIndex = lookUp(token);
            if (idIndex == -1) {
                //error(); // Identifier is not defined
            }
            firstType = idTable[idIndex].typ;
            infixString = token; // Get variable name
            insymbol();
            if (sy == lbrack) { // Array
                string arrayIndex;

                if (idTable[idIndex].cls != vars || idTable[idIndex].length == 0) {
                    //error(); // Identifier kind does not match
                }
                insymbol();
                expression(arrayIndex);
                if (sy == rbrack) { // Array finished
                    string tempVar = createTempVar();
                    insertTable(vars, firstType, tempVar.c_str(), idTable[idIndex].length, level, 0);
                    insertInfix("GETARR", infixString, arrayIndex, tempVar);
                    infixString = tempVar; // Return temp variable
                    insymbol();
                } else {
                    //error(); // Right bracket lost
                }
            } else if (sy == lparent) { // Call function with return
                if (idTable[idIndex].cls != funcs) {
                    //error(); // Identifier type does not match
                }
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                funcWithRetCall(infixString); // infixString is the temp var assigned with return value
            } else { // General identifier
                if (idTable[idIndex].cls != vars || idTable[idIndex].length != 0) {
                    //error(); // Identifier type does not match
                }
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                // Variable name already set to infixString
                insymbol();
            }
            break;
        }
        case plus:
        case minus:
        case intcon: {
            integer(infixString); // Get the number
            firstType = ints;
            break;
        }
        case charcon: {
            infixString = toString(inum);
            insymbol();
            firstType = chars;
            break;
        }
        case lparent: { // "(" expression ")"
            insymbol();
            firstType = expression(infixString);
            if (sy == rparent) {
                insymbol();
            } else {
                //error(); // Right parenthesis lost
            }
            break;
        }
        default:
            ;//error(); // Invalid factor
    }
    return firstType;
}

// ＜项＞ ::= ＜因子＞{＜乘法运算符＞＜因子＞}
type term(string &infixString) {
    type firstType = factor(infixString);
    type tempType;
    string tempOperand;
    string tempVariable;
    string ioperator;

    while (sy == times || sy == idiv) {
        ioperator = sy == times ? "MUL" : "DIV";
        insymbol();
        tempType = factor(tempOperand);
        if (firstType != tempType) { // Check type
            firstType = ints;
        }
        tempVariable = createTempVar();
        insertTable(vars, tempType, tempVariable.c_str(), 0, level, 0);
        insertInfix(ioperator, infixString, tempOperand, tempVariable);
        infixString = tempVariable;
    }
    return firstType;
}

// ＜表达式＞ ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
type expression(string &infixString) { // infixString here must be a variable or value
    type firstType;
    type tempType;
    symbol tempSy;
    string tempOperand;
    string tempVariable;

    if (sy == plus || sy == minus) {
        sy = tempSy;
        insymbol();
    }
    firstType = term(infixString);
    if (tempSy == minus) { // Head is a minus, should get a negative variable
        tempVariable = createTempVar();
        insertTable(vars, ints, tempVariable.c_str(), 0, level, 0);
        insertInfix("NEG", " ", infixString, tempVariable);
        infixString = tempVariable;
    }
    while (sy == plus || sy == minus) {
        tempSy = sy;
        insymbol();
        tempType = term(tempOperand);
        if (firstType != tempType) { // Type check and change
            warn(0);
            firstType = ints; // Change type to int
        }
        tempVariable = createTempVar();
        insertTable(vars, ints, tempVariable.c_str(), 0, level, 0);
        insertInfix(tempSy == plus ? "ADD" : "SUB", infixString, tempOperand, tempVariable);
        infixString = tempVariable;
    }
    return firstType;
}

// ＜赋值语句＞ ::= ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
void assignState() {
    string leftToEqual;
    string rightToEqual;
    int idIndex;
    type firstType;
    type secondType;

    if (sy == identi) {
        if ((idIndex = lookUp(token)) == -1) {
            //error(); // Identifier is not defined
        }
        firstType = idTable[idIndex].typ;
        leftToEqual = token; // Get left variable's name
        insymbol();
    } else {
        //error(); // Identifier lost
    }
    if (sy == becomes) { // General variable
        if (idTable[idIndex].cls != vars || idTable[idIndex].length != 0) {
            //error(); // Identifier type does not match
        }
        insymbol();
        secondType = expression(rightToEqual);
        if (firstType != secondType) {
            warn(0); // Type conflicts
        }
        insertInfix("ASSIGN", " ", rightToEqual, leftToEqual);
        cout << "This is an assignment statement." << endl;
    } else if (sy == lbrack) { // Array
        string arrayIndex;

        if (idTable[idIndex].cls != vars || idTable[idIndex].length == 0) {
            //error(); // Identifier type does not match
        }
        insymbol();
        expression(arrayIndex);
        if (sy == rbrack) {
            insymbol();
        } else {
            //error(); // Right bracket lost
        }
        if (sy == becomes) {
            insymbol();
        } else {
            //error(); // Becomes symbol or left bracket lost
        }
        secondType = expression(rightToEqual);
        if (firstType != secondType) {
            warn(0); // Type conflicts
        }
        insertInfix("SETARR", rightToEqual, arrayIndex, leftToEqual);
        cout << "This is an assignment statement." << endl;
    } else {
        //error(); // Becomes symbol or left bracket lost
    }

}

// ＜条件＞ ::= ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
void judgement(string &infixString) {
    string compareLeft;
    string compareRight;
    symbol compareSy;

    expression(compareLeft);
    if (isCompare(sy)) { // Optional branch
        compareSy = sy;
        insymbol();
        expression(compareRight);
        infixString = createTempVar();
        insertTable(vars, ints, infixString.c_str(), 0, level, 0); // 类型为数字，1表示判断成立，0表示判断不成立
        switch (compareSy) {
            case eql:
                insertInfix("EQL", compareLeft, compareRight, infixString);
                break;
            case neq:
                insertInfix("NEQ", compareLeft, compareRight, infixString);
                break;
            case lss:
                insertInfix("LSS", compareLeft, compareRight, infixString);
                break;
            case leq:
                insertInfix("LEQ", compareLeft, compareRight, infixString);
                break;
            case gtr:
                insertInfix("GTR", compareLeft, compareRight, infixString);
                break;
            case geq:
                insertInfix("GEQ", compareLeft, compareRight, infixString);
                break;
        }
    } else { // No compare symbol occurred
        infixString = compareLeft;
    }
}

// ＜条件语句＞ ::= if ‘(’＜条件＞‘)’＜语句＞［else＜语句＞］
void ifState() {
    string judgeResult;
    string firstLabel;
    string secondLabel;

    if (sy == ifsy) {
        insymbol();
    } else {
        //error(); // If lost
    }
    if (sy == lparent) {
        insymbol();
    } else {
        //error(); // Left parenthesis lost
    }
    judgement(judgeResult);
    firstLabel = "LABEL" + toString(labelIndex);
    labelIndex++;
    insertInfix("BEQ", judgeResult, "0", firstLabel); // If judgement = 0 then jump firstLabel
    if (sy == rparent) {
        insymbol();
    } else {
        //error(); // Right parenthesis lost
    }
    statement();
    secondLabel = "LABEL" + toString(labelIndex);
    labelIndex++;
    insertInfix("JMP", " ", " ", secondLabel); // Jump the else (If not, jump the next label)
    insertInfix("LABEL", " ", " ", firstLabel); // Where if is false go
    labelIndex++;
    if (sy == elsesy) { // Else is optional
        insymbol();
        statement();
        labelIndex++;
    }
    insertInfix("LABEL", " ", " ", secondLabel); // Where if is true go
    cout << "This is an if statement." << endl;
}

// ＜循环语句＞ ::= for‘(’＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞‘)’＜语句＞
void loopState() {
    string idNameString;
    string idNameString2;
    string expressionString;
    string judgeResult;
    string judgeLabel;
    string increaseLabel;
    string bodyLabel;
    string endLabel;

    int idIndex;

    if (sy == forsy) {
        insymbol();
    } else {
        //error(); // For symbol lost
    }
    if (sy == lparent) {
        insymbol();
    } else {
        //error(); // Left parenthesis lost
    }
    if (sy == identi) {
        idIndex = lookUp(token);
        if (idIndex == -1) {
            //error(); // Identifier is not defined
        } else if (idTable[idIndex].cls != vars || idTable[idIndex].length != 0) {
            //error(); // Identifier type does not match
        }
        idNameString = token;
        insymbol();
    } else {
        //error(); // Identifier lost
    }
    if (sy == becomes) {
        insymbol();
    } else {
        //error(); // Becomes symbol lost
    }
    expression(expressionString);
    insertInfix("ASSIGN", " ", expressionString, idNameString);
    bodyLabel = labelStr + toString(labelIndex);
    labelIndex++;
    insertInfix("JMP", " ", " ", bodyLabel);
    if (sy == semicolon) {
        insymbol();
    } else {
        //error(); // Semicolon lost
    }
    judgeLabel = labelStr + toString(labelIndex);
    labelIndex++;
    insertInfix("LABEL", " ", " ", judgeLabel);
    judgement(judgeResult);
    insertInfix("BNE", judgeResult, "0", bodyLabel); // Jump to body
    endLabel = labelStr + toString(labelIndex);
    labelIndex++;
    insertInfix("BEQ", judgeResult, "0", endLabel); // Jump to end
    if (sy == semicolon) {
        insymbol();
    } else {
        //error(); // Semicolon lost
    }
    increaseLabel = labelStr + toString(labelIndex);
    labelIndex;
    insertInfix("LABEL", " ", " ", increaseLabel);
    if (sy == identi) {
        if (idIndex == -1) {
            //error(); // Identifier is not defined
        } else if (idTable[idIndex].cls != vars || idTable[idIndex].length != 0) {
            //error(); // Identifier type does not match
        }
        idNameString = token;
        insymbol();
    } else {
        //error(); // Identifier lost
    }
    if (sy == becomes) {
        insymbol();
    } else {
        //error(); // Becomes symbol lost
    }
    if (sy == identi) {
        if (idIndex == -1) {
            //error(); // Identifier is not defined
        } else if (idTable[idIndex].cls != vars || idTable[idIndex].length != 0) {
            //error(); // Identifier type does not match
        }
        idNameString2 = token;
        insymbol();
    } else {
        //error(); // Identifier lost
    }
    string ioperator;
    if (sy == plus || sy == minus) {
        ioperator = sy == plus ? "ADD" : "MINUS";
        insymbol();
    } else {
        //error(); // Plus or minus before step length lost
    }
    if (sy == intcon) {
        if (inum != 0) {
            insertInfix(ioperator, idNameString2, toString(inum), idNameString); // Increment
            insymbol();
        } else {
            //error(); // Step length couldn't be 0
        }
    } else {
        //error(); // Step length lost
    }
    insertInfix("JMP", " ", " ", judgeLabel);
    if (sy == rparent) {
        insymbol();
    } else {
        //error(); // Right parenthesis lost
    }
    insertInfix("LABEL", " ", " ", bodyLabel);
    statement();
    insertInfix("JMP", " ", " ", increaseLabel);
    insertInfix("LABEL", " ", " ", endLabel);
    cout << "This is a loop statement." << endl;
}

// ＜情况语句＞ ::= switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’
void conditionState() {
    string switchElement;
    string endLabel;

    if (sy == switchsy) {
        insymbol();
    } else {
        //error(); // Switch symbol lost
    }
    if (sy == lparent) {
        insymbol();
    } else {
        //error(); // Left parenthesis lost
    }
    type firstType = expression(switchElement);
    if (sy == rparent) {
        insymbol();
    } else {
        //error(); // Right parenthesis lost
    }
    if (sy == lbrace) {
        insymbol();
    } else {
        //error(); // Left brace lost
    }
    endLabel = labelStr + toString(labelIndex);
    labelIndex++;
    conditionTable(firstType, switchElement, endLabel);
    conditionDefault();
    if (sy == rbrace) {
        cout << "This is a condition statement." << endl;
        insymbol();
    } else {
        //error(); // Right brace lost
    }
    insertInfix("LABEL", " ", " ", endLabel);
}

// ＜情况表＞ ::= ＜情况子语句＞{＜情况子语句＞}
void conditionTable(type firstType, string switchElement, string endSwitchLabel) {
    do {
        conditionBranch(firstType, switchElement, endSwitchLabel);
    } while (sy == casesy);
}

// ＜情况子语句＞ ::= case＜常量＞:＜语句＞
void conditionBranch(type firstType, string switchElement, string endSwitchLabel) {
    string caseElement;
    string nextCaseLabel;

    if (sy == casesy) {
        insymbol();
    } else {
        //error(); // Case symbol lost
    }
    if (sy == charcon) { // Char const
        if (firstType != chars) {
            warn(0); // Type conflicts
        }
        caseElement = toString(inum);
        insymbol();
    } else if (sy == intcon) { // Unsigned integer
        if (firstType != ints) {
            warn(0); // Type conflicts
        }
        caseElement = toString(inum);
        insymbol();
    } else if (sy == plus || sy == minus) { // Signed integer
        symbol tempSy = sy;
        if (firstType != ints) {
            warn(0); // Type conflicts
        }
        insymbol();
        if (sy == intcon && inum != 0) {
            int temp = tempSy == plus ? inum : (inum * (-1));
            caseElement = toString(temp);
            insymbol();
        } else {
            // error(); // Case element not a constant
        }
    } else {
        // error(); // Case element not a constant
    }
    nextCaseLabel = labelStr + toString(labelIndex);
    labelIndex++;
    insertInfix("BNE", switchElement, caseElement, nextCaseLabel); // Jump to next case
    if (sy == colon) {
        insymbol();
    } else {
        // error(); // Colon lost
    }
    statement();
    insertInfix("JMP", " ", " ", endSwitchLabel);
    insertInfix("LABEL", " ", " ", nextCaseLabel);
}

// ＜缺省＞ ::= default:＜语句＞
void conditionDefault() {
    if (sy == defaultsy) {
        insymbol();
    } else {
        //error(); // Default symbol lost
    }
    if (sy == colon) {
        insymbol();
    } else {
        //error(); // Colon lost
    }
    statement();
}

// ＜有返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞|<空>‘)’
void funcWithRetCall(string &infixString) {
    string funcName;
    int idIndex;

    if (sy == identi) {
        idIndex = lookUp(token);
        if (idIndex == -1) {
            //error(); // Identifier is not defined
        } else if (idTable[idIndex].cls != funcs || idTable[idIndex].typ == voids) {
            //error(); // Identifier type does not match
        }
        funcName = token;
        insymbol();
    } else {
        //error(); // Identifier lost
    }
    if (sy == lparent) {
        insymbol();
    } else {
        //error(); // Left parenthesis lost
    }
    if (sy == rparent) { // Function without parameters
        insymbol();
    } else { // Function with parameters
        valueParamTable(idIndex);
        if (sy == rparent) {
            insymbol();
        } else {
            //error(); // Right parenthesis lost
        }
    }
    insertInfix("CALL", " ", " ", funcName);
    infixString = createTempVar();
    insertTable(vars, idTable[idIndex].typ, infixString.c_str(), 0, level, 0);
    insertInfix("ASSIGN", " ", "RET", infixString);
}

// ＜无返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞|<空>‘)’
void funcWithoutRetCall() {
    string funcName;
    int idIndex;

    if (sy == identi) {
        idIndex = lookUp(token);
        if (idIndex == -1) {
            //error(); // Identifier is not defined
        } else if (idTable[idIndex].cls != funcs) {
            //error(); // Identifier type does not match
        }
        funcName= token;
        insymbol();
    } else {
        //error(); // Identifier lost
    }
    if (sy == lparent) {
        insymbol();
    } else {
        //error(); // Left parenthesis lost
    }
    if (sy == rparent) {
        insymbol();
    } else {
        valueParamTable(idIndex);
        if (sy == rparent) {
            insymbol();
        } else {
            //error(); // Right parenthesis lost
        }
    }
    insertInfix("CALL", " ", " ", funcName);
}

// ＜值参数表＞ ::= ＜表达式＞{,＜表达式＞}
void valueParamTable(int idIndex) {
    int valueParamIndex = 1;
    tabElement function = idTable[idIndex];
    int funcIndex = lookUpStatic(function.name); // Index in static table
    type secondType;
    string infixString;

    secondType = expression(infixString);
    if (valueParamIndex <= function.length) {
        if (staticTable[funcIndex + valueParamIndex].typ != secondType) {
            warn(0); // Type conflicts
        }
        insertInfix("PUSH", " ", " ", infixString);
    } else {
        //error(); // Parameter count exceeds
    }
    valueParamIndex++;
    while (sy == comma) {
        insymbol();
        secondType = expression(infixString);
        if (valueParamIndex <= function.length) {
            if (staticTable[funcIndex + valueParamIndex].typ != secondType) {
                warn(0); // Type conflicts
            }
            insertInfix("PUSH", " ", " ", infixString);
        } else {
            //error(); // Parameter count exceeds
        }
        valueParamIndex++;
    }
}

// ＜返回语句＞ ::= return[‘(’＜表达式＞‘)’]
void returnState() {
    type firstType = idTable[findCurrentFunc()].typ;
    type secondType;
    string returnString;

    if (sy == returnsy) {
        insymbol();
    } else {
        //error(); // Return symbol lost
    }
    if (sy == lparent) { // Optional branch
        insymbol();
        secondType = expression(returnString);
        if (sy == rparent) {
            if (firstType != secondType) {
                warn(0); // Type conflicts
            }
            insertInfix("RETURN", " ", " ", returnString);
            cout << "This is a return statement." << endl;
            insymbol();
        } else {
            //error(); // Right parenthesis lost
        }
    } else {
        cout << "This is a return statement." << endl;
    }

}

// ＜读语句＞ ::= scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
void scanfState() {
    int idIndex;
    string scanString;

    if (sy == scanfsy) {
        insymbol();
    } else {
        //error(); // Scanf symbol lost
    }
    if (sy == lparent) {
        insymbol();
    } else {
        //error(); // Left parenthesis lost
    }
    if (sy == identi) {
        if (idIndex == -1) {
            //error(); // Identifier is not defined
        } else if (idTable[idIndex].cls != vars || idTable[idIndex].length != 0) {
            //error(); // Identifier type does not match
        }
        scanString = token;
        insertInfix("SCANF", " ", " ", scanString);
        insymbol();
    } else {
        //error(); // Identifier lost
    }

    while (sy == comma){
        insymbol();
        if (sy == identi) {
            if (idIndex == -1) {
                //error(); // Identifier is not defined
            } else if (idTable[idIndex].cls != vars || idTable[idIndex].length != 0) {
                //error(); // Identifier type does not match
            }
            scanString = token;
            insertInfix("SCANF", " ", " ", scanString);
            insymbol();
        } else {
            //error(); // Identifier lost
        }
    }

    if (sy == rparent) {
        cout << "This is a scanf statement." << endl;
        insymbol();
    } else {
        //error(); // Right parenthesis lost
    }
}

// ＜写语句＞ ::= printf ‘(’ ＜字符串＞,＜表达式＞ ‘)’| printf ‘(’＜字符串＞‘)’| printf ‘(’＜表达式＞‘)’
void printfState() {
    type expressionType;
    string generalString = " "; // If there's no expression, space with take over it
    string expressionString = " ";

    if (sy == printfsy) {
        insymbol();
    } else {
        //error(); // Printf symbol lost
    }
    if (sy == lparent) {
        insymbol();
    } else {
        //error(); // Left parenthesis lost
    }
    if (sy == stringcon) { // First two branches
        generalString = str;
        insymbol();
        if (sy == comma) { // Optional expression
            insymbol();
            expressionType = expression(expressionString);
        }
    } else { // Last branch
        expressionType = expression(expressionString);
    }
    if (sy == rparent) {
        insertInfix("PRINTF", " ", generalString, expressionString);
        cout << "This is a printf statement." << endl;
        insymbol();
    } else {
        //error(); // Right prenthesis lost
    }
}

/* <语句> ::= ＜条件语句＞｜＜循环语句＞| ‘{’＜语句列＞‘}’｜＜有返回值函数调用语句＞;
| ＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;|＜情况语句＞｜
 ＜返回语句＞;｜＜空＞;*/
void statement() {
    string emptyString;

    switch (sy) {
        case ifsy:
            ifState();
            break;
        case forsy:
            loopState();
            break;
        case lbrace: {
            insymbol();
            stateList();
            if (sy == rbrace){
                insymbol();
            } else {
                //error(); // Right brace lost
            }
            break;
        }
        case identi: {
            int tempCharPtr = charPtr;
            char tempCh = ch;
            symbol tempSy = sy;
            char tempToken[100];
            strcpy(tempToken, token);
            insymbol();
            if (sy == becomes) {
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                assignState();
            } else { // Call function
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                if (idTable[lookUp(tempToken)].typ == voids) {
                    funcWithoutRetCall();
                } else {
                    funcWithRetCall(emptyString);
                }
            }
            if (sy == semicolon) {
                insymbol();
            } else {
                //error(); // Semicolon lost
            }
            break;
        }
        case scanfsy: {
            scanfState();
            if (sy == semicolon) {
                insymbol();
            } else {
                //error(); // Semicolon lost
            }
            break;
        }
        case printfsy: {
            printfState();
            if (sy == semicolon) {
                insymbol();
            } else {
                //error(); // Semicolon lost
            }
            break;
        }
        case switchsy:
            conditionState();
            break;
        case returnsy: {
            returnState();
            if (sy == semicolon) {
                insymbol();
            } else {
                //error(); // Semicolon lost
            }
            break;
        }
        case semicolon: // Empty statement, do nothing
            insymbol();
            break;
        default:
            ;//error(); // Statement error
    }
}

// ＜语句列＞ ::=｛＜语句＞｝
void stateList() {
    while (sy == ifsy || sy == forsy || sy == lbrace || sy == identi ||
            sy == scanfsy || sy == printfsy || sy == switchsy ||
            sy == returnsy || sy == semicolon) {
        statement();
    }
    cout << "This is a statement list." << endl;
}

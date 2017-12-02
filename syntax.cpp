//
// Created by Andy on 2017/11/27.
//

#include "main.h"

#define isCompare(a) (a == eql || a == neq || a == gtr || a == geq || a == lss || a == leq)

void integer() {
    if (sy == plus || sy == minus) {
        insymbol();
        if (sy == intcon) {
            if (inum != 0) {
                insymbol();
            } else {
                //error(); // Sign before zero
            }
        } else {
            //error(); // Number error
        }
    } else if (sy == intcon) {
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
            varState();
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
            cout << "This is a const statement." << endl;
            insymbol();
        } else {
            //error(); // Semicolon lost
        }
    }
}

/* ＜常量定义＞ ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}
| char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞} */
void constDef() {
    if (sy == intsy) { // Int const
        do {
            insymbol();
            if (sy == identi) {
                insymbol();
            } else {
                //error(); // Const identifier error
            }
            if (sy == becomes) {
                insymbol();
                integer(); // Manage integer
            } else {
                //error(); // Assignment symbol error
            }
        } while (sy == comma);
    } else if (sy == charsy) { // Char const
        do{
            insymbol();
            if (sy == identi) {
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
                insymbol();
            } else {
                //error(); // Char const error
            }
        }while (sy == comma);
    } else {
        // error(); // Const definition type error
    }
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
    if (sy == intsy || sy == charsy) { // Right variable(s)
        insymbol();
    } else {
        //error(); // Type error
    }
    if (sy == identi) {
        insymbol();
    } else {
        //error(); // Identifier lost
    }

    // Following are array management
    if (sy == lbrack) {
        insymbol();
        if (sy == intcon) {
            insymbol();
        } else {
            //error(); // Array index error
        }
        if (sy == rbrack) {
            insymbol();
        } else {
            //error(); // Array right bracket lost
        }
    }
    while (sy == comma) {
        insymbol();
        if (sy == identi) {
            insymbol();
        } else {
            //error(); // Identifier lost
        }

        // Following are array management
        if (sy == lbrack) {
            insymbol();
            if (sy == intcon) {
                insymbol();
            } else {
                //error(); // Array index error
            }
            if (sy == rbrack) {
                insymbol();
            } else {
                //error(); // Array right bracket lost
            }
        }
    }
}

// ＜有返回值函数定义＞ ::= ＜声明头部＞‘(’＜参数＞|＜空＞‘)’ ‘{’＜复合语句＞‘}’
void funcWithRetDef() {
    defHead(); // Manage definition head
    if (sy == lparent) {
        insymbol();
    } else {
        //error(); // Left parenthesis lost
    }
    if (sy == rparent) { // Empty parameter
        insymbol();
    } else {
        parameter(); // Manage parameter
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
}

// ＜无返回值函数定义＞ ::= void＜标识符＞‘(’＜参数＞|＜空＞‘)’‘{’＜复合语句＞‘}’
void funcWithoutRetDef() {
    if (sy == voidsy) { // Manage definition head
        insymbol();
    } else {
        //error(); // Void lost
    }
    if (sy == identi) {
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
}

// ＜声明头部＞ ::=  int＜标识符＞ |char＜标识符＞
void defHead() {
    if (sy == intsy) {
        insymbol();
    } else if (sy == charsy) {
        insymbol();
    } else {
        //error; // Definition head error
    }
    if (sy == identi) {
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
    if (sy == intsy || sy == charsy) {
        insymbol();
        if (sy == identi) {
            insymbol();
        } else {
            //error(); // Identifier lost
        }
        while (sy == comma) {
            insymbol();
            if (sy == intsy || sy == charsy) {
                insymbol();
            } else {
                //error(); // Type definition lost
            }
            if (sy == identi) {
                insymbol();
            } else {
                //error(); // Identifier lost
            }
        }
    } else {
        //error(); // Parameter error
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
    if (sy == voidsy) { // Manage definition head
        insymbol();
    } else {
        //error(); // Void lost
    }
    if (sy == mainsy) {
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
    complexState(); // Manage complex statement
    if (sy == rbrace) {
        cout << "This is the main function." << endl;
        insymbol();
    } else {
        //error(); // Right brace lost
    }
}

// ＜因子＞ ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’
void factor() {
    switch (sy) {
        case identi: {
            int tempCharPtr = charPtr;
            char tempCh = ch;
            symbol tempSy = sy;
            insymbol();
            if (sy == lbrack) { // Array
                insymbol();
                expression();
                if (sy == rbrack) {
                    insymbol();
                } else {
                    //error(); // Right bracket lost
                }
            } else if (sy == lparent) { // Call function with return
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                funcWithRetCall();
            } else { // General identifier
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                insymbol();
            }
            break;
        }
        case plus:
        case minus:
        case intcon: {
            integer();
            break;
        }
        case charcon: {
            insymbol();
            break;
        }
        case lparent: {
            insymbol();
            expression();
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
}

// ＜项＞ ::= ＜因子＞{＜乘法运算符＞＜因子＞}
void term() {
    factor();
    while (sy == times || sy == idiv) {
        insymbol();
        factor();
    }
}

// ＜表达式＞ ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
void expression() {
    if (sy == plus || sy == minus) {
        insymbol();
    }
    term();
    while (sy == plus || sy == minus) {
        insymbol();
        term();
    }
}

// ＜赋值语句＞ ::= ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
void assignState() {
    if (sy == identi) {
        insymbol();
    } else {
        //error(); // Identifier lost
    }
    if (sy == becomes) { // General variable
        insymbol();
        expression();
        cout << "This is an assignment statement." << endl;
    } else if (sy == lbrack) { // Array
        insymbol();
        expression();
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
        expression();
        cout << "This is an assignment statement." << endl;
    } else {
        //error(); // Becomes symbol or left bracket lost
    }

}

// ＜条件语句＞ ::= if ‘(’＜条件＞‘)’＜语句＞［else＜语句＞］
void ifState() {
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
    judgement();
    if (sy == rparent) {
        insymbol();
    } else {
        //error(); // Right parenthesis lost
    }
    statement();
    if (sy == elsesy) { // Else is optional
        insymbol();
        statement();
    }
    cout << "This is an if statement." << endl;
}

// ＜条件＞ ::= ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
void judgement() {
    expression();
    if (isCompare(sy)) { // Optional branch
        insymbol();
        expression();
    }
}

// ＜循环语句＞ ::= for‘(’＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞‘)’＜语句＞
void loopState() {
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
        insymbol();
    } else {
        //error(); // Identifier lost
    }
    if (sy == becomes) {
        insymbol();
    } else {
        //error(); // Becomes symbol lost
    }
    expression();
    if (sy == semicolon) {
        insymbol();
    } else {
        //error(); // Semicolon lost
    }
    judgement();
    if (sy == semicolon) {
        insymbol();
    } else {
        //error(); // Semicolon lost
    }
    if (sy == identi) {
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
        insymbol();
    } else {
        //error(); // Identifier lost
    }
    if (sy == plus || sy == minus) {
        insymbol();
    } else {
        //error(); // Plus or minus before step length lost
    }
    if (sy == intcon) {
        if (inum != 0) {
            insymbol();
        } else {
            //error(); // Step length couldn't be 0
        }
    } else {
        //error(); // Step length lost
    }
    if (sy == rparent) {
        insymbol();
    } else {
        //error(); // Right parenthesis lost
    }
    statement();
    cout << "This is a loop statement." << endl;
}

// ＜情况语句＞ ::= switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’
void conditionState() {
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
    expression();
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
    conditionTable();
    conditionDefault();
    if (sy == rbrace) {
        cout << "This is a condition statement." << endl;
        insymbol();
    } else {
        //error(); // Right brace lost
    }
}

// ＜情况表＞ ::= ＜情况子语句＞{＜情况子语句＞}
void conditionTable() {
    do {
        conditionBranch();
    } while (sy == casesy);
}

// ＜情况子语句＞ ::= case＜常量＞:＜语句＞
void conditionBranch() {
    if (sy == casesy) {
        insymbol();
    } else {
        //error(); // Case symbol lost
    }
    if (sy == charcon) { // Char const
        insymbol();
    } else if (sy == intcon) { // Unsigned integer
        insymbol();
    } else if (sy == plus || sy == minus) { // Signed integer
        insymbol();
        if (sy == intcon && inum != 0) {
            insymbol();
        } else {
            // error(); // Case element not a constant
        }
    } else {
        // error(); // Case element not a constant
    }
    if (sy == colon) {
        insymbol();
    } else {
        // error(); // Colon lost
    }
    statement();
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
void funcWithRetCall() {
    if (sy == identi) {
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
        valueParamTable();
        if (sy == rparent) {
            insymbol();
        } else {
            //error(); // Right parenthesis lost
        }
    }
}

// ＜无返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞|<空>‘)’
void funcWithoutRetCall() {
    if (sy == identi) {
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
        valueParamTable();
        if (sy == rparent) {
            insymbol();
        } else {
            //error(); // Right parenthesis lost
        }
    }
}

// ＜值参数表＞ ::= ＜表达式＞{,＜表达式＞}
void valueParamTable() {
    expression();
    while (sy == comma) {
        insymbol();
        expression();
    }
}

// ＜返回语句＞ ::= return[‘(’＜表达式＞‘)’]
void returnState() {
    if (sy == returnsy) {
        insymbol();
    } else {
        //error(); // Return symbol lost
    }
    if (sy == lparent) { // Optional branch
        insymbol();
        expression();
        if (sy == rparent) {
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
        insymbol();
    } else {
        //error(); // Identifier lost
    }

    while (sy == comma){
        if (sy == identi) {
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
        insymbol();
        if (sy == comma) { // Optional expression
            insymbol();
            expression();
        }
    } else { // Last branch
        expression();
    }
    if (sy == rparent) {
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
            insymbol();
            if (sy == becomes) {
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                assignState();
            } else {
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                funcWithRetCall();
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

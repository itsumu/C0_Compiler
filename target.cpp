//
// Created by Andy on 2017/12/9.
//

#include "main.h"

#define isLetter(a) (a >= 'a' && a <= 'z' || a >= 'A' && a <= 'Z' || a =='_')

const string labelMain = "main";
const int frameHeadOffset = 8;
const int maxLabelCount = 10000;

fstream mipsFile;
int infixIndex = 0;
int stringIndex = 0;
int funcIndex = 0;
string currentFunc;
std::vector<string> stringLabels;
string mipsCodes = "";
int globalAddr = 0;


inline bool isGlobal(int index) {
    return (staticTable[index].level == 0);
}

inline int getGlobalOffset(string varName) {
    return (staticTable[lookUp(varName.c_str())].addr * 4);
}

inline int getLocalOffset(string varName) {
    return ((-1) * (frameHeadOffset +
           staticTable[lookUpStatic(currentFunc.c_str(), varName.c_str())].addr * 4));
}

inline void insertCode(string code) {
    mipsCodes = mipsCodes + code + "\n";
}

inline bool isIdentifier(string name) {
    if (isLetter(name[0]) || name[0] == '#') {
        return true;
    }
    return false;
}

// Transform first operand to $t1
void firstOperandToRegister(string operand1) {
    int staticIndex = lookUpStatic(currentFunc.c_str(), operand1.c_str());

    if (!isIdentifier(operand1)) {
        // Number to variable
        insertCode("li $t1, " + toString(operand1));
    } else if (isIdentifier(operand1) && staticTable[staticIndex].cls == consts) {
        // Constant to variable
        insertCode("li $t1, " + toString(staticTable[staticIndex].addr));
    } else {
        int op1Offset;
        if (!isGlobal(staticIndex)) { // Local variable
            op1Offset = getLocalOffset(operand1);
            insertCode("lw $t1, " + toString(op1Offset) + "($fp)");
        } else { // Global variable
            op1Offset = getGlobalOffset(operand1);
            insertCode("lw $t1, " + toString(op1Offset) + "($gp)");
        }
    }
}

// Transform second operand to $t2
void secondOperandToRegister(string operand2) {
    int staticIndex = lookUpStatic(currentFunc.c_str(), operand2.c_str());

    if (!isIdentifier(operand2)) {
        // Value to variable
        insertCode("li $t2, " + toString(operand2));
    } else if (isIdentifier(operand2) && staticTable[staticIndex].cls == consts) {
        // Constant to variable
        insertCode("li $t2, " + toString(staticTable[staticIndex].addr));
    } else {
        int op2Offset;
        if (staticTable[staticIndex].level != 0) {
            op2Offset = getLocalOffset(operand2);
            insertCode("lw $t2, " + toString(op2Offset) + "($fp)");
        } else {
            op2Offset = getGlobalOffset(operand2);
            insertCode("lw $t2, " + toString(op2Offset) + "($gp)");
        }
    }
}

// Transform first two operands to registers
void operandsToRegister(infixNotation notation) {
    firstOperandToRegister(notation.operand1);
    secondOperandToRegister(notation.operand2);
}

// Transform third operand to $t0
void thirdOperandToRegister(string operand3) {
    int staticIndex = lookUpStatic(currentFunc.c_str(), operand3.c_str());

    if (!isIdentifier(operand3)) { // Value to register
        insertCode("li $t0, " + toString(operand3));
    } else if (isIdentifier(operand3) && staticTable[staticIndex].cls == consts) {
        // Constant to register
        insertCode("li $t0, " + toString(staticTable[staticIndex].addr));
    } else { // Variable to register
        int op3Offset;
        if (staticTable[staticIndex].level != 0) {
            op3Offset = getLocalOffset(operand3);
            insertCode("lw $t0, " + toString(op3Offset) + "($fp)");
        } else {
            op3Offset = getGlobalOffset(operand3);
            insertCode("lw $t0, " + toString(op3Offset) + "($gp)");
        }
    }
}

// Store result of $t0 (third operand) in memory
void thirdOperandToMemory(string operand3, string registerName) {
    int staticIndex3 = lookUpStatic(currentFunc.c_str(), operand3.c_str());
    int op3Offset;

    // Get offset from $gp or $fp
    if (!isGlobal(staticIndex3)) {
        op3Offset = getLocalOffset(operand3);
    } else {
        op3Offset = getGlobalOffset(operand3);
    }
    string rootRegister = isGlobal(staticIndex3) ? "($gp)" : "($fp)";
    insertCode("sw " + registerName + ", " + toString(op3Offset) + rootRegister); // Store
}

// Push one zero to global stack where $gp + offset points
void pushGlobalStack() {
    ++globalAddr;
}

// Push zeros to local stack with size of count
void pushGlobalStack(int count) {
    for (int i = 0; i < count; ++i) {
        ++globalAddr;
    }
}

// Push one zero to local stack where $fp + offset points
void pushLocalStack() {
    insertCode("sub $sp, $sp, 4");
}

// Push zeros to local stack with size of count
void pushLocalStack(int count) {
    insertCode("sub $sp, $sp, " + toString(4 * count));
}

// Manage variable and store it into stack from $gp or $fp
void mipsVarDef(bool isGlobal) {
    if (isGlobal) { // Push global variable into stack started from $gp
        for (int i = 0; i < staticTable.size() && staticTable[i].cls != funcs; ++i) {
            if (staticTable[i].cls == vars) {
                if (staticTable[i].length == 0) { // General variable
                    pushGlobalStack();
                } else { // Array
                    pushGlobalStack(staticTable[i].length);
                }
            }
        }
    } else { // Push local variable into stack started from $fp
        int startIndex = lookUpStatic(currentFunc.c_str());
        for (int i = startIndex + 1; i < staticTable.size() && staticTable[i].cls != funcs ; ++i) {
            if (staticTable[i].cls == vars) {
                if (staticTable[i].length == 0) { // General variable
                    pushLocalStack();
                } else { // Array
                    pushLocalStack(staticTable[i].length);
                }
            }
        }
    }
}

// Store $fp and $ra into stack
void funcHead() {
    int paramCount = staticTable[lookUpStatic(currentFunc.c_str())].length;
    int fpOffset = frameHeadOffset + paramCount * 4;
    int raOffset = fpOffset - 4;

    insertCode("sw $fp," + toString(fpOffset) + "($sp)"); // Push parent's $fp into stack
    insertCode("add $fp, $sp," + toString(fpOffset)); // Set current $fp
    insertCode("sw $ra," + toString(raOffset) + "($sp)"); // Push current $ra into stack

    // Skip all parameter definitions. Params was pushed by caller
    for (; infixTable[infixIndex].ioperator == "PARAM"; ++infixIndex);
}

// Recover $fp and $ra
void funcTail() {
    insertCode("lw $ra, -4($fp)");
    insertCode("move $sp, $fp");
    insertCode("lw $fp, ($sp)");
    insertCode("jr $ra\n");
}

// Mange codes inside func/main definition
void funcContent() {
    for (; infixIndex < infixTable.size() && infixTable[infixIndex].ioperator == "CONST"; ++infixIndex); // Jump all constant definitions
    for (; infixIndex < infixTable.size() && infixTable[infixIndex].ioperator == "VAR" ; ++infixIndex); // Skip all variable definitions
    mipsVarDef(false); // Push all variables
    for (; infixIndex < infixTable.size() && infixTable[infixIndex].ioperator != "FUNC" ; ++infixIndex) {
        // Read codes until next function definition or return
        infixNotation notation = infixTable[infixIndex];
        int staticIndex1 = lookUpStatic(currentFunc.c_str(), notation.operand1.c_str());
        int staticIndex2 = lookUpStatic(currentFunc.c_str(), notation.operand2.c_str());
        int staticIndex3 = lookUpStatic(currentFunc.c_str(), notation.operand3.c_str());

        if (notation.ioperator == "ADD") {
            operandsToRegister(notation);
            insertCode("add $t0, $t1, $t2");
            thirdOperandToMemory(notation.operand3, "$t0");
        } else if (notation.ioperator == "SUB") {
            operandsToRegister(notation);
            insertCode("sub $t0, $t1, $t2");
            thirdOperandToMemory(notation.operand3, "$t0");
        } else if (notation.ioperator == "MUL") {
            operandsToRegister(notation);
            insertCode("mul $t0, $t1, $t2");
            thirdOperandToMemory(notation.operand3, "$t0");
        } else if (notation.ioperator == "DIV") {
            operandsToRegister(notation);
            insertCode("div $t0, $t1, $t2");
            thirdOperandToMemory(notation.operand3, "$t0");
        } else if (notation.ioperator == "ASSIGN") {
            if (notation.operand2 == "#RET") { // Return value assignment
                insertCode("move $t0, $v1");
                thirdOperandToMemory(notation.operand3, "$t0");
            } else { // Regular assignment
                if (!isIdentifier(notation.operand2)) { // Value
                    insertCode("li $t0, " + toString(notation.operand2));
                } else if (isIdentifier(notation.operand2) && staticTable[staticIndex2].cls != consts) {
                    // Temp or general variable or parameters
                    int op2Offset;
                    if (staticTable[staticIndex2].level != 0) { // Local
                        op2Offset = getLocalOffset(notation.operand2);
                        insertCode("lw $t0, " + toString(op2Offset) + "($fp)");
                    } else { // Global
                        op2Offset = getGlobalOffset(notation.operand2);
                        insertCode("lw $t0, " + toString(op2Offset) + "($gp)");
                    }
                } else if (isIdentifier(notation.operand2) && staticTable[staticIndex2].cls == consts) {
                    // Constant
                    insertCode("li $t0, " + toString(staticTable[staticIndex2].addr));
                }
                thirdOperandToMemory(notation.operand3, "$t0");
            }
        } else if (notation.ioperator == "NEG") {
            if (!isIdentifier(notation.operand2)) { // Value
                insertCode("li $t0, " + toString(notation.operand2));
                insertCode("sub $t0, $0, $t0");
            } else if (isIdentifier(notation.operand2) && staticTable[staticIndex2].cls != consts) {
                // Temp or general variable or parameters
                int op2Offset;
                if (staticTable[staticIndex2].level != 0) { // Local
                    op2Offset = getLocalOffset(notation.operand2);
                    insertCode("lw $t0, " + toString(op2Offset) + "($fp)");
                } else { // Global
                    op2Offset = getGlobalOffset(notation.operand2);
                    insertCode("lw $t0, " + toString(op2Offset) + "($gp)");
                }
                insertCode("sub $t0, $0, $t0");
            } else if (isIdentifier(notation.operand2) && staticTable[staticIndex2].cls == consts) {
                // Constant
                insertCode("li $t0, " + toString(staticTable[staticIndex2].addr));
            }
            thirdOperandToMemory(notation.operand3, "$t0");
        } else if (notation.ioperator == "EQL") {
            operandsToRegister(notation);
            insertCode("seq $t0, $t1, $t2");
            thirdOperandToMemory(notation.operand3, "$t0");
        } else if (notation.ioperator == "NEQ") {
            operandsToRegister(notation);
            insertCode("sne $t0, $t1, $t2");
            thirdOperandToMemory(notation.operand3, "$t0");
        } else if (notation.ioperator == "LSS") {
            operandsToRegister(notation);
            insertCode("slt $t0, $t1, $t2");
            thirdOperandToMemory(notation.operand3, "$t0");
        } else if (notation.ioperator == "LEQ") {
            operandsToRegister(notation);
            insertCode("sle $t0, $t1, $t2");
            thirdOperandToMemory(notation.operand3, "$t0");
        } else if (notation.ioperator == "GTR") {
            operandsToRegister(notation);
            insertCode("sgt $t0, $t1, $t2");
            thirdOperandToMemory(notation.operand3, "$t0");
        } else if (notation.ioperator == "GEQ") {
            operandsToRegister(notation);
            insertCode("sge $t0, $t1, $t2");
            thirdOperandToMemory(notation.operand3, "$t0");
        } else if (notation.ioperator == "RETURN") {
            if (notation.operand3 != " ") { // Return with value
                if (staticIndex3 == -1) { // Value
                    insertCode("li $t0, " + notation.operand3);
                } else if (staticTable[staticIndex3].cls == consts) { // Constant
                    insertCode("li $t0, " + toString(staticTable[staticIndex3].addr));
                } else { // Variable or parameter
                    if (!isGlobal(staticIndex3)) {
                        int op3Offset = getLocalOffset(notation.operand3);
                        insertCode("lw $t0, " + toString(op3Offset) + "($fp)");
                    } else {
                        int op3Offset = getGlobalOffset(notation.operand3);
                        insertCode("lw $t0, " + toString(op3Offset) + "($gp)");
                    }
                }
            }
            insertCode("move $v1, $t0");
            insertCode("j " + currentFunc + "Tail"); // Jump to end of function
        } else if (notation.ioperator == "PUSH") { // Mange all PUSHes and CALL
            insertCode("sub $sp, $sp, " + toString(frameHeadOffset)); // $sp jump frame head offset
            for (; infixTable[infixIndex].ioperator == "PUSH"; ++infixIndex) {
                // Store all parameter strings until CALL met
                thirdOperandToRegister(infixTable[infixIndex].operand3); // Store operand3 value into $t0
                insertCode("sw $t0, ($sp)");
                insertCode("sub $sp, $sp, 4");
            }
            insertCode("jal " + infixTable[infixIndex].operand3); // CALL
        } else if (notation.ioperator == "CALL") { // Only enter when no parameter is needed
            insertCode("sub $sp, $sp, " + toString(frameHeadOffset)); // $sp jump frame head offset
            insertCode("jal " + infixTable[infixIndex].operand3); // CALL
        } else if (notation.ioperator == "SETARR") {
            int op3Offset;

            operandsToRegister(notation); // Get index & store it in $t2. Get value $ store it in $t1
            if (!isGlobal(staticIndex3)) { // Array is local
                insertCode("mul $t2, $t2, -4"); // Calculate extra offset of array
                op3Offset = getLocalOffset(notation.operand3); // Array head
                insertCode("add $t3, $fp, " + toString(op3Offset)); // Get base offset
            } else { // Array is global
                insertCode("mul $t2, $t2, 4"); // Calculate extra offset of array
                op3Offset = getGlobalOffset(notation.operand3);
                insertCode("add $t3, $gp, " + toString(op3Offset));
            }
            insertCode("add $t2, $t2, $t3"); // Merge offset
            insertCode("sw $t1, ($t2)"); // Set value to array
        } else if (notation.ioperator == "GETARR") {
            int op1Offset;

            secondOperandToRegister(notation.operand2); // Get index & store it in $t2. Get value $ store it in $t1
            if (!isGlobal(staticIndex1)) { // Array is local
                insertCode("mul $t2, $t2, -4"); // Calculate extra offset of array
                op1Offset = getLocalOffset(notation.operand1); // Array head
                insertCode("add $t3, $fp, " + toString(op1Offset)); // Get base offset
            } else { // Array is global
                insertCode("mul $t2, $t2, 4"); // Calculate extra offset of array
                op1Offset = getGlobalOffset(notation.operand1);
                insertCode("add $t3, $gp, " + toString(op1Offset));
            }
            insertCode("add $t2, $t2, $t3"); // Merge offset
            insertCode("lw $t1, ($t2)"); // Get value from array
            thirdOperandToMemory(notation.operand3, "$t1");
        } else if (notation.ioperator == "JMP") {
            insertCode("j " + notation.operand3);
        } else if (notation.ioperator == "BEQ") {
            operandsToRegister(notation);
            insertCode("beq $t1, $t2, " + notation.operand3);
        } else if (notation.ioperator == "BNE") {
            operandsToRegister(notation);
            insertCode("bne $t1, $t2, " + notation.operand3);
        } else if (notation.ioperator == "SCANF") {
            int operand3Offset = getLocalOffset(notation.operand3);
            if (staticTable[staticIndex3].typ == chars) { // Read character
                insertCode("li $v0, 12");
                insertCode("syscall");
                thirdOperandToMemory(notation.operand3, "$v0");
            } else { // Read integer
                insertCode("li $v0, 5");
                insertCode("syscall");
                thirdOperandToMemory(notation.operand3, "$v0");
            }
        } else if (notation.ioperator == "PRINTF") {
            if (notation.operand2 != " ") { // Output string
                insertCode("la $a0, " + stringLabels[stringIndex]);
                insertCode("li $v0, 4");
                insertCode("syscall");
                ++stringIndex;
            }
            if (notation.operand3 != " ") { // Output expression
                int operand3Offset;
                if (staticTable[staticIndex3].cls == consts) { // Constants
                    insertCode("li $a0, " + toString(staticTable[staticIndex3].addr)); // Get expression value
                } else { // Variable or parameters
                    if (!isGlobal(staticIndex3)) { // Local
                        operand3Offset = getLocalOffset(notation.operand3);
                        insertCode("lw $a0, " + toString(operand3Offset) + "($fp)"); // Get expression value
                    } else { // Global
                        operand3Offset = getGlobalOffset(notation.operand3);
                        insertCode("lw $a0, " + toString(operand3Offset) + "($gp)"); // Get expression value
                    }
                }
                if (staticTable[staticIndex3].typ == chars) { // Output char
                    insertCode("li $v0, 11");
                } else { // Output int
                    insertCode("li $v0, 1");
                }
                insertCode("syscall");
            }
        } else if (notation.ioperator == "LABEL") {
            insertCode(notation.operand3 + ":");
        }
    }
}

void mipsFuncDef() {
    currentFunc = infixTable[infixIndex].operand3;
    insertCode(currentFunc + ":"); // Func label
    ++infixIndex;
    funcHead();
    funcContent();
    insertCode(currentFunc + "Tail:"); // Func tail label. Used by return
    funcTail();
}

void mainHead() {
    insertCode("sw $fp, ($sp)");
    insertCode("move $fp, $sp");
    insertCode("sub $sp, $sp, 8");
}

void mipsMainDef() {
    currentFunc = "main";
    insertCode("main:"); // Main label
    ++infixIndex;
    mainHead();
    funcContent();
    insertCode("mainTail:");
}

void mipsAssign();
void mipsAdd();
void mipsSub();
void mipsMul();
void mipsDiv();
void mipsSeq();
void mipsSne();
void mipsSlt();
void mipsSle();
void mipsSgt();
void mipsSge();
void mipsBeq();
void mipsBne();
void mipsJ(string label) {
    insertCode("j " + label);
}
void mipsJal();
void mipsJr();
void mipsPrint();
void mipsScan();

inline void outputMipsCodes() {
    mipsFile << mipsCodes;
}

void mipsProgram(string mipsFileName) {
    mipsFile.open(mipsFileName.c_str(), ios::out);

    stringLabels.reserve(maxLabelCount);

    // Store strings into .data area
    insertCode(".data");
    for (int i = 0, count = 0; i < infixTable.size(); ++i) {
        if (infixTable[i].ioperator == "PRINTF" && infixTable[i].operand2 != " ") {
            insertCode("strLabel" + toString(count) + ":");
            insertCode(".asciiz");
            insertCode(infixTable[i].operand2);
            stringLabels.push_back("strLabel" + toString(count));
            count++;
        }
    }
    insertCode(""); // Insert a line break

    // General codes
    insertCode(".text");
    // Jump all global variable & constant definitions
    for (; infixIndex < infixTable.size() && infixTable[infixIndex].ioperator == "CONST"; ++infixIndex);
    for (; infixIndex < infixTable.size() && infixTable[infixIndex].ioperator == "VAR" ; ++infixIndex) {
    }
    mipsVarDef(true); // Push all global variables from static table into global stack
    mipsJ(labelMain); // Generate code to jump main
    insertCode("");
    for (; infixIndex < infixTable.size() && infixTable[infixIndex].ioperator == "FUNC"
           && infixTable[infixIndex].operand3 != "main";) {
        mipsFuncDef();
    }
    mipsMainDef();
    outputMipsCodes();
}
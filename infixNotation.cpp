//
// Created by Andy on 2017/12/5.
//

#include "main.h"

std::vector<infixNotation> infixTable; // Memory reserved in setup()
string outputBuff;
int tempVarIndex = 0;

void insertInfix(string ioperator, string operand1, string operand2, string operand3){
    infixNotation notation;
    notation.ioperator = ioperator;
    notation.operand1 = operand1;
    notation.operand2 = operand2;
    notation.operand3 = operand3;
    infixTable.push_back(notation);
    
    if (ioperator == "CONST") {
        outputBuff = outputBuff + "const " + operand2 + " " + operand3 + " = "
                     + operand1 + "\n";
    } else if (ioperator == "VAR") {
        if (operand1 != " ") { // Array
            outputBuff = outputBuff + operand2 + " " + operand3 + "[" + operand1 + "]" +"\n";
        } else { // General variable
            outputBuff = outputBuff + operand2 + " " + operand3 + "\n";
        }
    } else if (ioperator == "FUNC") {
        outputBuff = outputBuff + operand2 + " " + operand3 + "()" + "\n";
    } else if (ioperator == "PARAM") {
        outputBuff = outputBuff + "param " + operand2 + " " + operand3 + "\n";
    } else if (ioperator == "ADD") {
        outputBuff = outputBuff + operand3 + " = " + operand1 + " + " + operand2 + "\n";
    } else if (ioperator == "SUB") {
        outputBuff = outputBuff + operand3 + " = " + operand1 + " - " + operand2 + "\n";
    } else if (ioperator == "MUL") {
        outputBuff = outputBuff + operand3 + " = " + operand1 + " * " + operand2 + "\n";
    } else if (ioperator == "DIV") {
        outputBuff = outputBuff + operand3 + " = " + operand1 + " / " + operand2 + "\n";
    } else if (ioperator == "ASSIGN") {
        outputBuff = outputBuff + operand3 + " = " + operand2 + "\n";
    } else if (ioperator == "NEG") {
        outputBuff = outputBuff + operand3 + " = -" + operand2 + "\n";
    } else if (ioperator == "EQL") {
        outputBuff = outputBuff + operand3 + " = " + operand1 + " == " + operand2 + "\n";
    } else if (ioperator == "NEQ") {
        outputBuff = outputBuff + operand3 + " = " + operand1 + " != " + operand2 + "\n";
    } else if (ioperator == "LSS") {
        outputBuff = outputBuff + operand3 + " = " + operand1 + " < " + operand2 + "\n";
    } else if (ioperator == "LEQ") {
        outputBuff = outputBuff + operand3 + " = " + operand1 + " <= " + operand2 + "\n";
    } else if (ioperator == "GTR") {
        outputBuff = outputBuff + operand3 + " = " + operand1 + " > " + operand2 + "\n";
    } else if (ioperator == "GEQ") {
        outputBuff = outputBuff + operand3 + " = " + operand1 + " >= " + operand2 + "\n";
    } else if (ioperator == "PUSH") {
        outputBuff = outputBuff + "PUSH " + operand3 + "\n";
    } else if (ioperator == "CALL") {
        outputBuff = outputBuff + "CALL " + operand3 + "\n";
    } else if (ioperator == "RETURN") {
        outputBuff = outputBuff + "RETURN " + operand3 + "\n";
    } else if (ioperator == "SETARR") {
        outputBuff = outputBuff + operand3 + "[" + operand2 + "] = " + operand1 + "\n";
    } else if (ioperator == "GETARR") {
        outputBuff = outputBuff + operand3 + " = " + operand2 + "[" + operand1 + "]" + "\n";
    } else if (ioperator == "JMP") {
        outputBuff = outputBuff + "JMP " + operand3 + "\n";
    } else if (ioperator == "BEQ") {
        outputBuff = outputBuff + "GOTO " + operand3 + ", IF " + operand1 + " == " +
        operand2 + "\n";
    } else if (ioperator == "BNE") {
        outputBuff = outputBuff + "GOTO " + operand3 + ", IF " + operand1 + " != " +
                     operand2 + "\n";
    } else if (ioperator == "SCANF") {
        outputBuff = outputBuff + "SCANF " + operand3 + "\n";
    } else if (ioperator == "PRINTF") {
        if (operand2 != " " && operand3 != " ") {
            outputBuff = outputBuff + "PRINTF " + operand2 + ", " + operand3 + "\n";
        } else if (operand2 != " " && operand3 == " ") {
            outputBuff = outputBuff + "PRINTF " + operand2 + "\n";
        } else if (operand2 == " " && operand3 != " ") {
            outputBuff = outputBuff + "PRINTF " + operand3 + "\n";
        } else {
            cout << "Can't print nothing!" << endl;
        } // Do nothing
    } else if (ioperator == "LABEL") {
        outputBuff = outputBuff + operand3 + "\n";
    }
}

void outputInfixes() {
    infixFile << outputBuff;
}

string createTempVar() {
    string tempVar;
    tempVar = "#t" + std::to_string(tempVarIndex);
    tempVarIndex++;
    return tempVar;
}
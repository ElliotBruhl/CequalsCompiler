#include "codeGen.h"

bool moveValue(FILE* file, ValueNode* value, ValueStatus src, ValueStatus dest, bool saveRegs, VarTable* varTable, FuncTable* funcTable) { //r10 scratch and func param regs may be used for C std lib
    if (src == dest) return true; //no instruction needed
    if (file == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to moveValue.\033[0m\n");
        return false;
    }
    if (dest == UNPROCESSED) {
        printf("\033[1;31mInvalid destination status %d in moveValue.\033[0m\n", dest);
        return false;
    }
    if (value == NULL && src == UNPROCESSED) {
        printf("\033[1;31mNull value parameter to moveValue.\033[0m\n");
        return false;
    }
    fprintf(file, "\t;moving value from type %d to type %d\n", src, dest);
    switch (src) {
        case UNPROCESSED:
            if (value->valueType == VALUE_NUM) {
                if (dest == RAX) {
                    fprintf(file, "\tmov rax, %lld\n", *(long long*)value->value);
                }
                else if (dest == RCX) {
                    fprintf(file, "\tmov rcx, %lld\n", *(long long*)value->value);
                }
                else if (dest == STACK) {
                    fprintf(file, "\tmov r10, %lld\n", *(long long*)value->value); //mov stack, immediate only supports 32-bit, so include extra step
                    fprintf(file, "\tpush r10\n");
                }
                else {
                    printf("\033[1;31mInvalid destination status %d in moveValue.\033[0m\n", dest);
                    return false;
                }
            }
            else if (value->valueType == VALUE_VAR) {
                VarEntry* var = (VarEntry*)value->value;
                if (dest == RAX) {
                    fprintf(file, "\tmov rax, [rbp-%d]\n", var->stackOffset);
                }
                else if (dest == RCX) {
                    fprintf(file, "\tmov rcx, [rbp-%d]\n", var->stackOffset);
                }
                else if (dest == STACK) {
                    fprintf(file, "\tpush [rbp-%d]\n", var->stackOffset);
                }
                else {
                    printf("\033[1;31mInvalid destination status %d in moveValue.\033[0m\n", dest);
                    return false;
                }
            }
            else if (value->valueType == VALUE_FUNC_RET) {
                FuncCallNode* funcCall = (FuncCallNode*)value->value;

                if (saveRegs) { //save appropriate registers
                    if (dest != RAX)
                        fprintf(file, "\tpush rax\n");
                    if (dest != RCX)
                        fprintf(file, "\tpush rcx\n");
                }

                //check for special handling of C standard library functions

                for (int i = 0; i < funcCall->argCount; i++) { //prepare arguments (all args go to stack)
                    if (!moveValue(file, funcCall->args[i], UNPROCESSED, STACK, false, varTable, funcTable)) return false;
                }
                fprintf(file, "\tcall %s\n", funcCall->funcName); //call function
                if (funcCall->argCount > 0)
                    fprintf(file, "\tadd rsp, %d\n", funcCall->argCount * 8); //remove arguments from stack
                
                //move return value
                if (dest == RCX) { 
                    fprintf(file, "\tmov rcx, rax\n");
                }
                if (saveRegs) { //restore appropriate registers
                    if (dest != RCX)
                        fprintf(file, "\tpop rcx\n");
                    if (dest != RAX)
                        fprintf(file, "\tpop rax\n");
                }
                if (dest == STACK)
                    fprintf(file, "\tpush rax\n");
            }
            else {
                printf("\033[1;31mInvalid value type %d in moveValue.\033[0m\n", value->valueType);
                return false;
            }
            break;
        case RAX:
            if (dest == RCX)
                fprintf(file, "\tmov rcx, rax\n");
            else if (dest == STACK)
                fprintf(file, "\tpush rax\n");
            else {
                printf("\033[1;31mInvalid destination status %d in moveValue.\033[0m\n", dest);
                return false;
            }
            break;
        case RCX:
            if (dest == RAX)
                fprintf(file, "\tmov rax, rcx\n");
            else if (dest == STACK)
                fprintf(file, "\tpush rcx\n");
            else {
                printf("\033[1;31mInvalid destination status %d in moveValue.\033[0m\n", dest);
                return false;
            }
            break;
        case STACK:
            if (dest == RAX)
                fprintf(file, "\tpop rax\n");
            else if (dest == RCX)
                fprintf(file, "\tpop rcx\n");
            else {
                printf("\033[1;31mInvalid destination status %d in moveValue.\033[0m\n", dest);
                return false;
            }
            break;
        default:
            printf("\033[1;31mInvalid source status %d in moveValue.\033[0m\n", src);
            return false;
    }
    return true;
}
bool evaluateMathExpr(FILE* file, MathOpNode* mathOp, bool resultInRAX, ValueStatus leftStatus, ValueStatus rightStatus, VarTable* varTable, FuncTable* funcTable) {
    if (mathOp == NULL || file == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to evaluateMathExpr.\033[0m\n");
        return false;
    }
    if (mathOp->right == NULL) {
        printf("\033[1;31mNo right operand found.\033[0m\n");
        return false;
    }
    //fetch left and right operands to registers rax and rcx respectively
    char* resultReg = resultInRAX ? "rax" : "rcx";
    if (mathOp->left == NULL) { //unary operator -> only load right operand
        if (mathOp->opType >= OP_MUL) { //binary operator (with no left operand)
            printf("\033[1;31mBinary operator with no left operand.\033[0m\n");
            return false;
        }
    }
    else {
        if (!moveValue(file, mathOp->left, leftStatus, RAX, true, varTable, funcTable)) return false; //put left operand in rax
    }
    if (!moveValue(file, mathOp->right, rightStatus, RCX, true, varTable, funcTable)) return false; //put right operand in rcx
    switch (mathOp->opType) { //consult header file for how to handle each operator in detail
        case OP_BIT_NOT: //~
            fprintf(file, "\t;bitwise not (~)\n");
            fprintf(file, "\tnot rcx\n");
            if (resultInRAX)
                fprintf(file, "\tmov rax, rcx\n");
            break;
        case OP_NOT: //!
            fprintf(file, "\t;logical not (!)\n");
            fprintf(file, "\ttest rcx, rcx\n");
            fprintf(file, "\tsetz cl\n");
            fprintf(file, "\tmovzx %s, cl\n", resultReg);
            break;
        case OP_REF: //&
            if (mathOp->right->valueType != VALUE_VAR) {
                printf("\033[1;31mInvalid operand for & operator.\033[0m\n");
                return false;
            }
            VarEntry* varEntry = varLookup(varTable, (char*)mathOp->right->value);
            if (varEntry == NULL) {
                printf("\033[1;31mVariable %s not found in varTable during evaluation.\033[0m\n", (char*)mathOp->right->value);
                return false;
            }
            fprintf(file, "\t;reference operator (&)\n");
            fprintf(file, "\tlea %s, [rbp-%d]\n", resultReg, varEntry->stackOffset);
            break;
        case OP_DEREF: //*
            fprintf(file, "\t;dereference operator (*)\n");
            fprintf(file, "\tmov %s, [rcx]\n", resultReg);
            break;
        case OP_MUL: //*
            fprintf(file, "\t;multiply (*)\n");
            if (resultInRAX) //multiplication is commutative -> skip possible mov
                fprintf(file, "\timul rax, rcx\n");
            else
                fprintf(file, "\timul rcx, rax\n");
            break;
        case OP_DIV: // /
            fprintf(file, "\t;divide (/)\n");
            fprintf(file, "\tcqo\n");
            fprintf(file, "\tidiv rcx\n");
            if (!resultInRAX)
                fprintf(file, "\tmov rcx, rax\n");
            break;
        case OP_MOD: //%
            fprintf(file, "\t;modulo (%%)\n");
            fprintf(file, "\tcqo\n");
            fprintf(file, "\tidiv rcx\n");
            fprintf(file, "\tmov %s, rdx\n", resultReg);
            break;
        case OP_ADD: //+
            fprintf(file, "\t;addition (+)\n");
            if (resultInRAX) //addition is commutative -> skip possible mov
                fprintf(file, "\tadd rax, rcx\n");
            else
                fprintf(file, "\tadd rcx, rax\n");
            break;
        case OP_SUB: //-
            fprintf(file, "\t;subtraction (-)\n");
            fprintf(file, "\tsub rax, rcx\n");
            if (!resultInRAX)
                fprintf(file, "\tmov rcx, rax\n");
            break;
        case OP_BIT_L: //<<
            fprintf(file, "\t;bitwise left shift (<<)\n");
            fprintf(file, "\tshl rax, cl\n");
            if (!resultInRAX)
                fprintf(file, "\tmov rcx, rax\n");
            break;
        case OP_BIT_R: //>>
            fprintf(file, "\t;bitwise right shift (>>)\n");
            fprintf(file, "\tshr rax, cl\n");
            if (!resultInRAX)
                fprintf(file, "\tmov rcx, rax\n");
            break;
        case OP_LT: //<
            fprintf(file, "\t;less than (<)\n");
            fprintf(file, "\tcmp rax, rcx\n");
            fprintf(file, "\tsetl cl\n");
            fprintf(file, "\tmovzx %s, cl\n", resultReg);
            break;
        case OP_GT: //>
            fprintf(file, "\t;greater than (>)\n");
            fprintf(file, "\tcmp rax, rcx\n");
            fprintf(file, "\tsetg cl\n");
            fprintf(file, "\tmovzx %s, cl\n", resultReg);
            break;
        case OP_LTE: //<=
            fprintf(file, "\t;less than or equal (<=)\n");
            fprintf(file, "\tcmp rax, rcx\n");
            fprintf(file, "\tsetle cl\n");
            fprintf(file, "\tmovzx %s, cl\n", resultReg);
            break;
        case OP_GTE: //>=
            fprintf(file, "\t;greater than or equal (>=)\n");
            fprintf(file, "\tcmp rax, rcx\n");
            fprintf(file, "\tsetge cl\n");
            fprintf(file, "\tmovzx %s, cl\n", resultReg);
            break;
        case OP_EQU: //==
            fprintf(file, "\t;equal (==)\n");
            fprintf(file, "\tcmp rax, rcx\n");
            fprintf(file, "\tsete cl\n");
            fprintf(file, "\tmovzx %s, cl\n", resultReg);
            break;
        case OP_NEQ: //!=
            fprintf(file, "\t;not equal (!=)\n");
            fprintf(file, "\tcmp rax, rcx\n");
            fprintf(file, "\tsetne cl\n");
            fprintf(file, "\tmovzx %s, cl\n", resultReg);
            break;
        case OP_BIT_AND: //&
            fprintf(file, "\t;bitwise and (&)\n");
            if (resultInRAX) //and is commutative -> skip possible mov
                fprintf(file, "\tand rax, rcx\n");
            else
                fprintf(file, "\tand rcx, rax\n");
            break;
        case OP_BIT_XOR: //^
            fprintf(file, "\t;bitwise xor (^)\n");
            if (resultInRAX) //xor is commutative -> skip possible mov
                fprintf(file, "\txor rax, rcx\n");
            else
                fprintf(file, "\txor rcx, rax\n");
            break;
        case OP_BIT_OR: //|
            fprintf(file, "\t;bitwise or (|)\n");
            if (resultInRAX) //or is commutative -> skip possible mov
                fprintf(file, "\tor rax, rcx\n");
            else
                fprintf(file, "\tor rcx, rax\n");
            break;
        case OP_AND: //&&
            fprintf(file, "\t;logical and (&&)\n");
            fprintf(file, "\ttest rax, rax\n");
            fprintf(file, "\tje ANDfalse(%p)\n", mathOp);
            fprintf(file, "\ttest rcx, rcx\n");
            fprintf(file, "\tje ANDfalse(%p)\n", mathOp);
            fprintf(file, "\tmov %s, 1\n", resultReg);
            fprintf(file, "\tjmp ANDend(%p)\n", mathOp);
            fprintf(file, "ANDfalse(%p):\n", mathOp);
            fprintf(file, "\txor %s, %s\n", resultReg, resultReg);
            fprintf(file, "ANDend(%p)\n:", mathOp);
            break;
        case OP_OR: //||
            fprintf(file, "\t;logical or (||)\n");
            fprintf(file, "\ttest rax, rax\n");
            fprintf(file, "\tjne ORtrue(%p)\n", mathOp);
            fprintf(file, "\ttest rcx, rcx\n");
            fprintf(file, "\tjne ORtrue(%p)\n", mathOp);
            fprintf(file, "\txor %s, %s\n", resultReg, resultReg);
            fprintf(file, "\tjmp ORend(%p)\n", mathOp);
            fprintf(file, "ORtrue(%p):\n", mathOp);
            fprintf(file, "\tmov %s, 1\n", resultReg);
            fprintf(file, "ORend(%p)\n:", mathOp);
            break;
        default:
            printf("\033[1;31mInvalid operator %d in evaluateMathExpr.\033[0m\n", mathOp->opType);
            return false;
    }
    return true;
}
bool traverseMathOpTree(FILE* file, MathOpNode* mathAST, bool isLeft, VarTable* varTable, FuncTable* funcTable) { //isLeft -> result to rax while !isLeft -> result to rcx
    if (file == NULL || mathAST == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to traverseMathOpTree.\033[0m\n");
        return false;
    }
    if (mathAST->left->valueType != VALUE_MATH_OP) {
        if (mathAST->right->valueType != VALUE_MATH_OP) { //left and right are simple -> evaluate
            return evaluateMathExpr(file, mathAST, isLeft, UNPROCESSED, UNPROCESSED, varTable, funcTable); //evaluate
        }
        else { //left is simple -> push left to stack and recurse right
            if (!moveValue(file, mathAST->left, UNPROCESSED, STACK, false, varTable, funcTable)) return false; //push left to stack
            if (!traverseMathOpTree(file, (MathOpNode*)mathAST->right->value, false, varTable, funcTable)) return false; //recurse right - result to RCX
            return evaluateMathExpr(file, mathAST, isLeft, STACK, RCX, varTable, funcTable); //evaluate - result to RCX
        }
    }
    else { //left is math op -> recurse left
        if (!traverseMathOpTree(file, (MathOpNode*)mathAST->left->value, true, varTable, funcTable)) return false; //recurse left - result to RAX
        if (mathAST->right->valueType != VALUE_MATH_OP) //right is simple and left is in RAX -> evaluate
            return evaluateMathExpr(file, mathAST, isLeft, RAX, UNPROCESSED, varTable, funcTable); //evaluate - result to RAX (if in left branch) or RCX (if in right branch)
        else { //right is math op and left is in RAX -> push left to stack and recurse right
            fprintf(file, "\tpush rax"); //push left to stack (from RAX)
            if (!traverseMathOpTree(file, (MathOpNode*)mathAST->right->value, false, varTable, funcTable)) return false; //recurse right - result to RCX
            return evaluateMathExpr(file, mathAST, isLeft, STACK, RCX, varTable, funcTable); //evaluate - result to RAX (if in left branch) or RCX (if in right branch)
        }
    }
}

bool writeCode(FILE* file, ASTNode* head, FuncDeclNode* func, VarTable* varTable, FuncTable* funcTable) {
    if (file == NULL || head == NULL || func == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to writeCode.\033[0m\n");
        return false;
    }
    //prepare stack frame
    fprintf(file, "\tpush rbp\n");
    fprintf(file, "\tmov rbp, rsp\n");

    ASTNode* current = head;
    while (current != NULL) {
        if (current->subNode == NULL) {
            printf("\033[1;31mNull subNode in ASTNode.\033[0m\n");
            return false;
        }
        switch (current->nodeType) {
            case NODE_VAR_DECL:
                fprintf(file, "\t;declaring %s\n", ((VarDeclNode*)(current->subNode))->varInfo->name);
                fprintf(file, "\tsub rsp, 8\n");
                break;
            case NODE_VAR_ASSIGN:
                VarEntry* var = ((VarAssignNode*)(current->subNode))->varInfo;
                ValueNode* newVal = ((VarAssignNode*)(current->subNode))->newValue;
                fprintf(file, "\t;assigning %s\n", var->name);
                switch (newVal->valueType) {
                    case VALUE_NUM:
                        fprintf(file, "\tmov r10, %lld\n", *(long long*)newVal->value);
                        fprintf(file, "\tmov [rbp-%d], r10\n", var->stackOffset);
                        break;
                    case VALUE_VAR:
                        fprintf(file, "\tmov r10, [rbp-%d]\n", ((VarEntry*)newVal->value)->stackOffset);
                        fprintf(file, "\tmov [rbp-%d], r10\n", var->stackOffset);
                        break;
                    case VALUE_FUNC_RET:
                        if (!moveValue(file, newVal, UNPROCESSED, RAX, false, varTable, funcTable)) return false;
                        fprintf(file, "\tmov [rbp-%d], rax\n", var->stackOffset);
                        break;
                    case VALUE_MATH_OP:
                        if (!traverseMathOpTree(file, (MathOpNode*)newVal->value, true, varTable, funcTable)) return false;
                        fprintf(file, "\tmov [rbp-%d], rax\n", var->stackOffset);
                        break;
                    default:
                        printf("\033[1;31mInvalid value type %d in variable assignment.\033[0m\n", newVal->valueType);
                        return false;
                }
                break;
            case NODE_FUNC_DECL:
                fprintf(file, "%s:\n", ((FuncDeclNode*)(current->subNode))->funcInfo->name);
                if (!writeCode(file, ((FuncDeclNode*)(current->subNode))->body, (FuncDeclNode*)(current->subNode), varTable, funcTable)) return false;
                break;
            case NODE_WHILE:
                //...
                break;
            case NODE_IF:
                //...
                break;
            case NODE_RETURN:
                ValueNode* retVal = ((ReturnNode*)(current->subNode))->returnValue;
                switch (retVal->valueType) {
                    case VALUE_NUM:
                    case VALUE_FUNC_RET:
                        if (!moveValue(file, retVal, UNPROCESSED, RAX, false, varTable, funcTable)) return false;
                        break;
                    case VALUE_VAR:
                        fprintf(file, "\tmov rax, [rbp-%d]\n", ((VarEntry*)retVal->value)->stackOffset);
                        break;
                    case VALUE_MATH_OP:
                        if (!traverseMathOpTree(file, (MathOpNode*)retVal->value, true, varTable, funcTable)) return false;
                        break;
                    default:
                        printf("\033[1;31mInvalid value type %d in return statement.\033[0m\n", retVal->valueType);
                        return false;
                }
                fprintf(file, "\tmov rsp, rbp\n");
                fprintf(file, "\tpop rbp\n");
                fprintf(file, "\tret\n");
                break;
            default:
                printf("\033[1;31mInvalid node type %d in writeCode.\033[0m\n", current->nodeType);
                return false;
        }
        current = current->next;
    }
    return true;
}

bool codeGen(FILE* file, ASTNode* head, VarTable* varTable, FuncTable* funcTable) {
    if (file == NULL || head == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to writeCode.\033[0m\n");
        return false;
    }

    //write header
    //enter functions with writeCode

    return true;
}
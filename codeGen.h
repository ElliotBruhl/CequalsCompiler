#include "tokenizer.h"
#include "varTable.h"
#include "funcTable.h"
#include "parser.h"

typedef enum {      //describes where a piece of data is
    UNPROCESSED,    //value has not been loaded yet (fetch it from ValueNode)
    RAX,            //value was a left branch result
    RCX,            //value was a right branch result
    STACK           //value is on top of the stack
} ValueStatus;

bool traverseMathOpTree(FILE* file, MathOpNode* mathAST, bool isLeft, VarTable* varTable, FuncTable* funcTable);
/*
C standard library supported functions:

print(format str, ...) - use printf
read(dest ptr) - use scanf
malloc(size) - use malloc
free(ptr) - use free

How to hande each operator:

OP_BIT_NOT~	NOT {dest} ;

OP_NOT	    !	test {value}, {value}       ;use same value
                setz {value-8}              ;value must be 8-bit
                movzx {value-64}, {value-8} ;make the 8-bit result 64-bit

OP_REF	    &	lea {dest}, [rbp-{offset}]  ;offset is from varTable

OP_DEREF    *	mov {dest}, [{source}]

OP_MUL	    *	imul {dest}, {source}

OP_DIV	    /	xor rdx, rdx                ;zero the high 64 bits
                idiv {source}               ;divide rax by source - result in rax

OP_MOD	    %	xor rdx, rdx                ;zero the high 64 bits
                idiv {source}               ;divide rax by source - remainder in rdx

OP_ADD	    +	add {dest}, {source}

OP_SUB	    -	sub {dest}, {source}

OP_BIT_L    <<	shl {dest}, {count-8}

OP_BIT_R    >>	shr {dest}, {count-8}

OP_LT	    <	cmp {a}, {b}                ;for a < b
                setl {value-8}              ;value must be 8-bit
                movzx {value-64}, {value-8} ;make the 8-bit result 64-bit
            
OP_GT	    >	cmp {a}, {b}                ;for a > b
                setg {value-8}              ;value must be 8-bit
                movzx {value-64}, {value-8} ;make the 8-bit result 64-bit

OP_LTE	    <=	cmp {a}, {b}                ;for a <= b
                setle {value-8}             ;value must be 8-bit
                movzx {value-64}, {value-8} ;make the 8-bit result 64-bit

OP_GTE	    >=	cmp {a}, {b}                ;for a >= b
                setge {value-8}             ;value must be 8-bit
                movzx {value-64}, {value-8} ;make the 8-bit result 64-bit

OP_EQU	    ==	cmp {a}, {b}                ;for a == b
                sete {value-8}              ;value must be 8-bit
                movzx {value-64}, {value-8} ;make the 8-bit result 64-bit

OP_NEQ	    !=	cmp {a}, {b}                ;for a != b
                setne {value-8}             ;value must be 8-bit
                movzx {value-64}, {value-8} ;make the 8-bit result 64-bit

OP_BIT_AND  &	and {dest}, {source}

OP_BIT_XOR  ^	xor {dest}, {source}

OP_BIT_OR   |	or {dest}, {source}

OP_AND	    &&	test {a}, {a}                  ;for a && b
                je {false}                  
                test {b}, {b}
                je {false}
                mov {value}, 1
                jmp {end}
            {falseBranch}:
                xor {value}, {value}
            {end}:

OP_OR	    ||	test {a}, {a}                 ;for a || b
                jne {true}
                test {b}, {b}
                jne {true}
                xor {value}, {value}
                jmp {end}
            {trueBranch}:
                mov {value}, 1
            {end}:
*/
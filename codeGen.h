/*
How to hande each operator:

OP_BIT_NOT~	NOT {dest} ;

OP_NOT	    !	test {value}, {value}       ;use same value
                setz {value-8}              ;value must be 8-bit
                movzx {value-64}, {value-8} ;make the 8-bit result 64-bit

OP_REF	    &	calculate from stack offset in varEntry at compile time

OP_DEREF    *	mov {dest}, [{source}]

OP_MUL	    *	imul {dest}, {source}

OP_DIV	    /	xor rdx, rdx                ;zero the high 64 bits
                idiv {source}               ;divide rax by source - result in rax

OP_MOD	    %	xor rdx, rdx                ;zero the high 64 bits
                idiv {source}               ;divide rax by source - remainder in rdx

OP_ADD	    +	add {dest}, {source}

OP_SUB	    -	sub {dest}, {source}

OP_BIT_L    <<	shl {dest}, {count}

OP_BIT_R    >>	shr {dest}, {count}

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

OP_AND	    &&	test {a}, {a}               ;for a && b
                setnz {al}                  ;test a
                test {b}, {b}
                setnz {bl}                  ;test b
                and {al}, {bl}              ;a & b
                movzx {value-64}, {value-8} ;make the 8 bit result 64 bit
                
OP_OR	    ||	test {a}, {a}               ;for a && b
                setnz {al}                  ;test a
                test {b}, {b}
                setnz {bl}                  ;test b
                or {al}, {bl}               ;a | b
                movzx {value-64}, {value-8} ;make the 8 bit result 64 bit
*/
GENERAL INFO:
  -compiler is written in C
  -output code will be in x86 assembly
    -I use nasm to assemble (nasm -f win64 -g test.asm -o test.obj), gcc to link (gcc test.obj -o test.exe -g), and gdb to debug (gdb test.exe)
    -Not very cross-platform friendly, but I'm not going through too much effort to fix it
  -language generally follows C syntax and specific rules defined below
  -all variables are signed 64 bit
  -all pointer arithmetic is in 64 bit increments
  -unary -, +, &, *, ~, and ! should be followed immediately with their operand

EXIT CODES (main.c):
  0 - sucess
  -1 - file read failed
  -2 - tokenizer failed (should have specified error)
  -3 - symbol table creation failed
  -4 - parser failed (should have specified error)

TOKENIZER (in tokenizer.(c/h)):
  -Stores data in double linked list of structs where each node holds line number, token type, token value, previous, and next:
  -'#' denotes a comment and the rest of the line will be ignored
  -Token Types:
    0: identifiers - all alphanumeric strings, except keywords, that begin with a letter and continue until non-alphanumeric char encountered.
    1: numbers - positive or negative numbers. Positive or negative signs must immediately precede a number (no whitespace between) if present. (no sign defaults to positive).
    2: operators - all symbols included in language:
      +: add
      -: subtract
      *: multiply or pointer dereference when immediately followed by var
      /: divide
      %: modulus
      =: change var value or logical equals when followed by another '='
      <: less than or bitshift left when followed by another '<' or less than or equal to when followed by '='
      >: greater than or bitshift right when followed by another '>' or greater than or equal to when followed by '='
      !: logical not or not equals when followed by '='
      &: bitwise and (when alone) or logical and (when followed with another '&') or address operator (when followed by var)
      |: bitwise or (when alone) or logical or (when followed with another '|')
      ~: bitwise not
      ^: bitwise xor
    3: keywords - all language defined words:
      Note that keywords will be ignored if followed by alphanumeric char: break1 is not a keyword but break+ is a keyword (and then operator).
      var: allocates memory for 64 bit variable value. No initial values. Adheres to scope. Ex: var varName; for 64 bits of memory
      array: allocates multiple vars contigously. Ex: array arrayName 5; for 5*64 bits of memory and arrayName as pointer to element 0
      func: delcares a function in current scope. Must return a value. Ex: func funcName(param1, param2) {return 0;}
      return: leaves current function and returns value listed afterward.
      while: declares a loop. Ex: while(x < 5) {while loop body}
      continue: skips to next iteration in while loop.
      break: jumps out of current loop.
      if: declares a conditional block. Ex: if(x == 5) {if body}
      else: optional second part of if statement that will execute if the if statement fails. Must construct elif manually. Ex: if(x != 0) {...} else {...}
    4: separators
    parenthesis: for loop, if, func, or math
    curly braces: define subscope for loops, if, or funcs
    commas: seperate function arguments
    semicolon: ends a statement

PARSER (parser.(c/h)):
  -stores ASTs in double linked list structure where each node holds sub-node type indicator, pointer to sub-node, previous, and next
  -each AST node type (see parser.h for more):
    math operation,
    var declaration,
    array declaration,
    var assignment,
    function node,
    while node,
    if node

SYMBOL TABLE (symbolTable.(c/h)):
  -keeps track of all declared functions and variable names for each scope
  -scopes can be created and freed easily
  -scopes automatically resize (double their size when full)
  -default size is 10 scopes, each with 10 spots for var/func names
  -function overloading and variable/function shadowing work, but should be avoided

GENERAL INFO:
  -compiler is written in C
  -output code is in LLVM assembly
  -generally follows C syntax

EXIT CODES:
  0 - sucess
  -1 - file read failed
  -2 - tokenizer failed (should have specified error)
  -3 - symbol table creation failed
  -4 - parser failed (should have specified error)

TOKENIZER:
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
      var: allocates memory for 64 bit variable value. No initial value supported. Adheres to scope. Ex: var varName; would allocate 64 bits of memory for varName.
      array: allocates n number of variables contigously in memory. Adheres to scope. Must create and use pointer manip to access elements. Ex: array arrayName 5; would allocate 64*5 bits of memory for arrayName (arrayName refers the value of first element).
      func: delcares a function in current scope. Must return a value. Ex: func funcName(param1, param2) {return 0;} would declare funcName as a valid function that takes 2 parameters.
      return: leaves current function and returns value listed afterward.
      while: declares a loop. Ex: while(x < 5) {while loop body} would create a while loop that will execute until x < 5 is not satisfied.
      continue: skips to next iteration in while loop.
      break: jumps out of current loop.
      if: declares a conditional block. Ex: if(x == 5) {"if body"} will create a conditional block that only executes if condition is satisfied.
      else: optional second part of if statement that will execute if the if statement fails. No else if construction supported. Ex: if(x != 0) {...} else {...}
    4: separators
    parenthesis: for loop, if, func, or math
    brackets: define subscope for loops, if, or funcs
    commas: seperate function arguments
    semicolon: ends a statement

PARSER:
-stores ASTs in double linked list structure where each node holds sub-node type indicator, sub-node of specific type, previous, and next
-each AST node type (see parser.h for more):
  math operation
  var declaration,
  var re-assignment,
  function node,
  while node,
  if node

IDENTIFIER HASH TABLE:
-A hash table is used to store all of the identifers and their info when parsing to avoid string operations.
-It uses the DJB2 algorithm and has a default size of 509.
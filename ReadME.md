GENERAL INFO:
  -compiler is written in C
  -input code in custom language
  -output code will be in x86 assembly
    -I use nasm to assemble (nasm -f win64 -g test.asm -o test.obj), gcc to link (gcc test.obj -o test.exe -g), and gdb to debug
  -custom language generally follows C syntax

EXIT CODES (main.c):
   0 - sucess
  -1 - file read failed
  -2 - tokenizer failed
  -3 - variable symbol table creation failed
  -4 - function symbol table creation failed

TOKENIZER (in tokenizer.(c/h)):
  -Stores data in double linked list of structs where each node holds line number, token type, token value, previous, and next:
  -'#' denotes a comment and the rest of the line will be ignored
  -Token Types:
    0: identifiers - all alphanumeric strings, except keywords, that begin with a letter and continue until non-alphanumeric char encountered.
    1: numbers - positive or negative numbers. Positive or negative signs must immediately precede a number if present. over/underflow are undefined behavior.
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
      Note that keywords will be ignored if followed by alphanumeric character
      var: allocates memory for 64 bit variable value. Memory is uninitialized. Adheres to scope.
      func: delcares a function in current scope. All functions must return a value (no void functions).
      return: leaves current function and returns value listed afterward.
      while: declares a loop.
      continue: skips to next iteration in while loop.
      break: jumps out of current loop.
      if: declares a conditional block.
      else: optional second part of if statement that will execute if the if statement fails. Must construct else if manually.
    4: separators
    parenthesis: used for while, if, func, or math operations
    curly braces: defines a subscope for loops, if, else, or funcs
    commas: seperate function arguments
    semicolon: ends a statement

PARSER (parser.(c/h)):
  -stores ASTs in double linked list structure where each node holds sub-node type indicator, sub-node, previous, and next
  -each AST node type:
    var declaration: holds var name (which can be looked up in symbol table for stack offset)
    var assignment: holds var name and new value
    function declaration: holds func name, argument count, argument names, and func body
    while loop: holds condition and loop body
    conditional: holds condition, if body, and nullable else body
  -parsing of math operations:
    -tokenizer tokens are converted into more specific  value tokens: operators as an OperatorType* (an enum), number literals as long long*, variables as char*, function calls into FuncCallNode* (contains name, arg count, and args)
    -those tokens are then put into postfix notation, then into binary tree of value tokens
    -that binary tree can be referenced by any AST node that takes a value (simple values are the same type but without any branches)

VARTABLE (varTable.c/h):
  -keeps track of all variable names for a given scope and their offset from the basePtr of the scope
  -shadowing is allowed (lookup of name goes from current scope outward)
  -default capacity of table is 5 scopes of 5 entries each
  -when capacity is reached, number of scopes or entries doubles (keeping old entries/scopes)
  
FUNCTABLE (funcTable.c/h):
  -keeps track of all functions' names and parameters
  -only a global scope for functions is defined (attempting to shadow will overwrite if name and parameter count is the same)
  -default capacity of 5 entries, which doubles when full

CODEGEN:
  -todo
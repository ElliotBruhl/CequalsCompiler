# Compiler Information
- **Language**: C
- **Input**: Custom language
- **Output**: x86 Assembly
- **Assembler**: I use NASM (not included)
- **Linker**: I use GCC (not included)
- **Debugger**: I use GDB (not included)
- **Syntax**: Generally follows C syntax
## Exit Codes (main.c)
- `0`: Success
- `-1`: File read failed
- `-2`: Tokenizer failed
- `-3`: Variable symbol table creation failed
- `-4`: Function symbol table creation failed
## Tokenizer (tokenizer.c/h)
The tokenizer stores data in a doubly linked list of structs where each node holds the following information:
- **Line number**
- **Token type**
- **Token value**
- **Previous node**
- **Next node**
### Token Types
1. **Identifiers**: All alphanumeric strings, except keywords, that begin with a letter and continue until a non-alphanumeric character is encountered.
2. **Numbers**: Positive or negative numbers. Positive or negative signs must immediately precede a number if present. Overflow/underflow are undefined behavior.
3. **Operators**: Symbols used in the language:
   - `+`: Add
   - `-`: Subtract
   - `*`: Multiply or pointer dereference when followed by a variable
   - `/`: Divide
   - `%`: Modulus
   - `=`: Assign value or logical equals when followed by another '='
   - `<`: Less than or left bitshift when followed by another `<` or less than or equal to when followed by `=`
   - `>`: Greater than or right bitshift when followed by another `>` or greater than or equal to when followed by `=`
   - `!`: Logical not or not equals when followed by `=`
   - `&`: Bitwise and (alone), logical and (followed by another `&`), or address operator (followed by a variable)
   - `|`: Bitwise or (alone) or logical or (followed by another `|`)
   - `~`: Bitwise not
   - `^`: Bitwise xor
4. **Keywords**: Reserved words in the language (ignored if followed by an alphanumeric character):
   - `var`: Allocates memory for a 64-bit variable (uninitialized, adheres to scope)
   - `func`: Declares a function (must return a value)
   - `return`: Leaves the current function and returns the specified value
   - `while`: Declares a loop
   - `continue`: Skips to the next iteration of a `while` loop
   - `break`: Jumps out of the current loop
   - `if`: Declares a conditional block
   - `else`: Optional second part of an `if` statement
5. **Separators**:
   - Parentheses: Used for `while`, `if`, `func`, or math operations
   - Curly braces: Defines a subscope for loops, `if`, `else`, or functions
   - Comma: Separates function arguments
   - Semicolon: Ends a statement
## Parser (parser.c/h)
The parser stores Abstract Syntax Trees (ASTs) in a doubly linked list structure where each node holds:
- **Sub-node type indicator**
- **Sub-node**
- **Previous node**
- **Next node**
### AST Node Types
- **Var Declaration**: Holds variable name (looked up in the symbol table for stack offset)
- **Var Assignment**: Holds variable name and new value
- **Function Declaration**: Holds function name, argument count, argument names, and function body
- **While Loop**: Holds condition and loop body
- **Conditional**: Holds condition, if body, and optional else body
### Math Operations
The tokenizer's tokens are converted into more specific value tokens:
- **Operators**: Stored as `OperatorType*` (an enum)
- **Number literals**: Stored as `long long*`
- **Variables**: Stored as `char*`
- **Function Calls**: Stored as `FuncCallNode*` (contains name, argument count, and arguments)
These tokens are converted into postfix notation and then into a binary tree of value tokens. This binary tree can be referenced by any AST node that requires a value (simple values are stored as the same type without branches).
## VarTable (varTable.c/h)
- Tracks all variable names for a given scope and their offset from the base pointer of the scope
- **Shadowing** is allowed (lookup of name goes from current scope outward)
- Default capacity of the table is 5 scopes of 5 entries each
- When capacity is reached, the number of scopes or entries doubles, retaining old entries/scopes
## FuncTable (funcTable.c/h)
- Tracks all function names and their parameters
- Only a global scope for functions is defined (attempting to shadow will overwrite if name and parameter count are the same)
- Default capacity of the table is 5 entries, which doubles when full
## Code Generation
- **TODO**: Code generation is yet to be implemented
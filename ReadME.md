# Compiler Information

- **Language**: C (version C17 - should be compatible with C99+)
- **Input**: Custom (currently unnamed) language
- **Output**: x86-64 Assembly (NASM syntax) for Win64

## How It Works

The compilation process is broken into three main steps:

1. **Tokenization**  
   The input file is read, and a list of tokens is created. Tokens include words, numbers, or symbols that have specific meanings in the language.

2. **Parsing**  
   The tokens are converted into an Abstract Syntax Tree (AST), which is a highly detailed, language-independent representation of the program. This step also detects and reports errors.

3. **Code Generation**  
   The AST is transformed into x86 assembly code and written to the output file.

## How to Use the Compiler (Win64)

1. Download the executable or compile it from the source files
2. Create a folder with the executable and an input file (`input.txt`)
3. Write some code in the input file
4. Run the executable; it will generate an output file named `output.asm`
5. Assemble the file using NASM: nasm -f win64 -g output.asm -o output.obj
6. Link the file (must link with the C standard library) using GCC: gcc output.obj -o output.exe -g
7. Run the output executable.

## Exit Codes (`main.c`)

- `0`  – Success
- `-1` – File read failed
- `-2` – Tokenizer failed
- `-3` – Variable symbol table creation failed
- `-4` – Function symbol table creation failed
- `-5` – Parser failed
- `-6` – Assembly generation failed

## The Tokenizer (`tokenizer.c/h`)

The tokenizer stores data in a **doubly linked list** of structs, where each node holds the following information:

- **Line number**
- **Token type**
- **Token value**
- **Previous node**
- **Next node**

### Token Types

1. **Identifiers** – Alphanumeric strings (except keywords).
2. **Numbers** – Whole numbers in the range `-(2^63)` to `(2^63) - 1`.
3. **Operators** – Almost all operators available in C.
4. **Keywords** – Special reserved words in the language.
5. **Separators** – Symbols like `;`, `()`, `{}`, and `,`.

## The Parser (`parser.c/h`)

The parser stores a **doubly linked list** of structs, where each node holds:

- **Sub-node type indicator**
- **Sub-node**
- **Previous node**
- **Next node**

### AST Node Types

Each sub-node is a struct containing the necessary details to generate code later:

- **Variable Declaration** – Holds the variable name.
- **Variable Assignment** – Holds the variable name and new value.
- **Function Declaration** – Holds the function name, argument count, argument names, and function body.
- **While Loop** – Holds the loop condition and body.
- **Conditional** – Holds the condition, `if` body, and optional `else` body.

### Math Operations

Math operations are particularly tricky due to order of operations and mixed operand types. To handle this:

1. Tokens are converted into **ValueNodes**, structs that store operand types (number, variable, or function call).
2. ValueNodes are converted into a **postfix expression**, which eliminates the need for order-of-operations parsing.
3. The postfix expression is then transformed into a **binary tree** of operators and operands.

## Variable Table (`varTable.c/h`)

- Tracks all variable names within a given scope.
- Computes the offset of each variable from the base pointer of the scope.
- Supports **variable shadowing** (name lookups start in the current scope and proceed outward).

## Function Table (`funcTable.c/h`)

- Tracks all function names and their parameters.
- Uses a **single global scope** for functions (no shadowing; duplicate function names overwrite each other).

## Code Generation

**TODO**
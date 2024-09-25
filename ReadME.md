TOKENIZER:
Stores data in double linked list of structs:
  typedef struct Token {
      int tokenType;
      char* value;
      struct Token* prevToken;
      struct Token* nextToken;
  } Token;
  the value field is always null terminated
  
Exit codes:
0 - sucess
1 - malloc failed
2 - file failed to open
3 - line over length 255 chars (prevents buffer overflow)
4 - unknown char encountered

Token Types:
  0: identifiers
  all things that begin with a letter and continue until non-alphanumeric char encountered except keywords.
  
  1: numbers
  Overflow should be handled with wraparound but is undefined behavior so don't do it.
  note that negatives can be stored if '-' is used immediately before a digit.
  + and - can come immediately before number and will be treated as part of number (ex 1+1 will be 2 tokens (2 numbers) but 1 + 1 will be 3 tokens (2 numbers and operator))
  
  2: operators
  all symbols included in language:
    {'+', '-', '*', '/', '%', '=', '<', '>', '!', '&', '|', '~', '^'},
    and those symbols with = after for assignment,
    and && or ||.
  
  3: keywords
  all language defined words:
    {"var ", "func ", "while ", "if ", "else ", "return ", "continue", "break"}
  note that spaces must follow all keywords if they are to be considered except for return and continue (the no arg keywords)
  
  4: separators
  parenthesis, brackets, semicolons. Don't mean anything on own but necessary for syntax.

SYNTAX:
Linked list structure again for making Abstract Syntax Trees (ASTs):
typedef struct ASTNodeLL {
    int type; //node type
    union {
        MathOpNode* mathOpPtr;
        VarNode* varPtr;
        FuncNode* funcPtr;
        WhileNode* whilePtr;
        IfNode* ifPtr;
        ElseNode* elsePtr;
        RetNode* retPtr;
        ContNode* contPtr;
        BreakNode* breakPtr;
    } node;
    struct ASTNodeLL* next;
} ASTNodeLL;

each AST node type:
  math operations -> begin and end with parenthesis can contain: operators, numbers, identifiers, mathOp
  variable declaration (var) -> var(keyword), name(identifier) equals(operator) identifier/number/mathOp semicolon(seperator)
  func declaration -> ...
  while declaration -> mathOp bracket(seperator) ... bracket(seperator)
  if declaration -> mathOp bracket(seperator) ... bracket(seperator)
  else -> if(keyword) OR (mathOp bracket(seperator) ... bracket(seperator))
  return -> number/identifier/mathOp semicolon(seperator)
  continue -> semicolon(seperator)
  break -> semicolon(seperator)

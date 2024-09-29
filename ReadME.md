EXIT CODES:
  0 - sucess
  1 - tokenizer failed
  2 - file read failed

TOKENIZER:
  -Stores data in double linked list where each node holds line number, token type, token value, previous, and next:
  -Token Types:
    0: identifiers - all alphanumeric strings, except keywords, that begin with a letter and continue until non-alphanumeric char encountered.
    1: numbers - positive or negative numbers. Positive or negative signs must immediately precede a number (no whitespace between) if present. (no sign defaults to positive).
    2: operators - all symbols included in language:
      {'+', '-', '*', '/', '%', '=', '<', '>', '!', '&', '|', '~', '^'}, those with '=' after, and && or ||.
    3: keywords - all language defined words:
      {"var", "func", "while", "if", "else", "return", "continue", "break"}
      note that keywords will be ignored if followed by alphanumeric char: break1 is not a keyword but break+ is a keyword (and then operator).
    4: separators
    parenthesis, brackets, semicolons, and commas

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
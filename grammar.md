# **EBNF CGF**

StmtList    ->  { Stmt }
  
Start       ->  StmtList  
  
Stmt        ->  Id "=" AExpr ";" | output "(" AExpr ")" ";" | "if" "(" RExpr ")" "{" StmtList "}" ElsePart | "while" "(" RExpr ")" "{" StmtList "}"  
  
ElsePart    ->  "else" "{" StmtList "}" | ε  
  
RExpr       ->  AExpr ( "<" | ">" | "==" ) AExpr  
  
AExpr       ->  Atom { ( "+" | "-" ) Atom }  
  
Atom        ->  Id | Num  
  
Id          ->  "a...z"|"A...Z",{ "0...9"|"a...z"|"A...Z" }  
  
Num         ->  "0...9",{ "0...9" }  
  

### Notes
* An `else` binds to the nearest `if` before it that doesnt have an `else`


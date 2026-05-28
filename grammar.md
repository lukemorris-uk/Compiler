# **Luke CGF**

Start       ->  StmtBlock  
  
StmtBlock   ->  Stmt | StmtBlock Stmt  
  
Stmt        ->  Id *=* RExpr *;* | *{* StmtBlock *}* | *if* *(* RExpr *)* Stmt | *if* *(* RExpr *)* Stmt *else* Stmt | *while* *(* RExpr *)* Stmt  
  
RExpr       ->  RExpr *<* AExpr | RExpr *>* AExpr | RExpr *==* AExpr | AExpr  
  
AExpr       ->  AExpr *+* PExpr | AExpr *-* PExpr | PExpr  
  
PExpr       ->  Id | Num  
  
Id          ->  (*a...z*|*A...Z*)⁺(*0...9*|*a...z*|*A...Z*)\*  
  
Num         ->  (*0...9*)⁺  
  

## Notes
An `else` binds to the nearest preceding `if` that has no `else` of its own

# **Luke CGF**

Start       ->  StmtBlock  
  
StmtBlock   ->  Stmt | StmtBlock Stmt  
  
Stmt        ->  Id *=* RExpr *;* | *{* StmtBlock *}* | *if* *(* RExpr *)* Stmt *else* Stmt | *while* *(* RExpr *)* Stmt  
  
RExpr       ->  RExpr *<* AExpr | RExpr *>* AExpr  
  
AExpr       ->  AExpr *+* PExpr | AExpr *-* PExpr | PExpr  
  
PExpr       ->  Id | Num  
  
Id          ->  (*a...z*|*A...Z*)⁺(*0...9*|*a...z*|*A...Z*)\*  
  
Num         ->  (*0...9*)⁺  
  

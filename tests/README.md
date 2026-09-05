# Test suite

Three groups:

- `NN_*.src` — should compile and run, with a known expected output.
- `nNN_*.src` — should be **rejected**, with an error and a non-zero exit. Nothing should be written to the output file, and codegen should never run.
- `qNN_*.src` — behaviour questions. These aren't clearly pass/fail yet; they're places where you need to decide what the language means.

Suggested run loop once codegen is wired up:

```bash
./compile tests/01_arith.src
```

---

## Passing tests

### 01_arith.src
Constants, chained `+`, left-associativity of `-`, negative results, and a literal at the top of the signed 32-bit range.
```bash
42
55
5
-7
2147483647
```
If `10 - 3 - 2` prints `9`, your operand order in `subq` is backwards.

### 02_assign.src
Store and reload from `.bss`, negative values in a variable, and reading a variable that appears twice in one expression.
```bash
5
-3
2
12
```

### 03_if_basic.src
Taken and not-taken `if` with no `else`. Checks that the false path lands past the body and that the following statement still executes.
```bash
1
3
```

### 04_if_else.src
Both directions of `if`/`else` in one program, so a single run exercises both label paths.
```bash
10
40
```

### 05_comparisons.src
Every relational operator in both directions, plus a comparison whose left side is an arithmetic expression. This is the one that catches a flipped `cmpq` operand order — a flip makes `<` and `>` swap while `==` still looks fine.
```bash
1
0
0
1
1
0
1
```

### 06_while_count.src
Countdown loop. Confirms the condition is re-evaluated each iteration and that the loop variable survives the `printf` call.
```bash
5
4
3
2
1
0
```

### 07_while_zero_trip.src
Condition false on entry — the body must not run once.
```bash
10
```

### 08_while_sum.src
Accumulator. Also checks the loop variable's exit value.
```bash
55
11
```

### 09_nested_while.src
Nested loops. If your label allocation collides, the inner loop's exit will jump somewhere in the outer loop and this will hang or print garbage.
```bash
0
1
2
1
2
3
2
3
4
```

### 10_if_in_while.src
`if`/`else` nested inside a loop, with a further `if` inside the `else`. Labels from three constructs interleave here.
```bash
0
-1
-2
3
100
5
```

### 11_empty_blocks.src
Empty `if` body, empty `else` body, empty `while` body with a false condition. Checks that a zero-length statement list generates a well-formed (if pointless) block rather than a malformed label sequence.
```bash
1
```

### 12_expr_conditions.src
Arithmetic on both sides of a relational operator, inside a loop condition. Each iteration must fully re-evaluate the expression and leave the stack balanced.
```bash
0
1
2
100
```

### 13_empty_program.src
Empty source file. Should produce a valid `main` that returns 0 — a `.bss` section with no variables, a prologue, and an epilogue. No output, exit status 0.

### 14_nested_if.src
Three levels of `if` nesting where only the innermost `else` fires.
```bash
2
```

---

## Rejection tests

### n01_use_before_assign.src
`x` read with no prior assignment. Error on line 1.

### n02_undefined_in_condition.src
Undefined variable inside an `if` condition — confirms the analyser walks conditions, not just statement bodies. Error on line 2.

### n03_undefined_rhs.src
Undefined variable on the right-hand side of an assignment. Error on line 1.

### n04_missing_semicolon.src
Missing `;`. Parse error — expected `;`, got `output`.

### n05_rexpr_in_assign.src
`a = 1 < 2;`. Your grammar only allows `RExpr` in conditions, so this should fail at the `<`. Worth checking the message is comprehensible; "expected `;`, got `<`" is technically accurate but a bit opaque.

### n06_no_braces.src
Braceless `if` body. Parse error — expected `{`, got `output`.

### n07_self_reference.src
`a = a + 1;` with no prior `a`. This is the case your symbol-table ordering exists to catch — if the target were added before the RHS was analysed, this would sail through. Error on line 1.

---

## Behaviour questions

### q01_branch_scope.src
`z` is assigned only inside a branch that never runs, then read afterwards. Right now the analyser accepts this (assignments inside `if` bodies land in the same flat symbol table), and `.bss` zero-init means it prints `0`. Two defensible answers: keep it (Python-ish, everything is one scope, uninitialised reads give 0) or reject it (requires tracking definite assignment per branch, which means intersecting the sets of variables defined on each path). Worth deciding explicitly rather than by accident.

### q02_big_literal.src
`4294967296` doesn't fit in the sign-extended 32-bit immediate that `pushq $imm` takes. Watch what the assembler says. The lexer currently stores the value via `strtoul` into an `int`, and codegen emits the original text, so the two disagree about what this program even means. A range check in the lexer is the usual fix.

### q03_dangling_else.src
Your grammar note says `else` binds to the nearest `if` without one, and the parser gets this right structurally. Expected output is `10`. This is here as a regression guard — if you ever restructure `parse_if_stmt`, run it again.

---

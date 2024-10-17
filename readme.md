# Rottent

a minimal interpreted programming language.

a program in rottent consists of ascii characters which separately perform small actions. unlike mouse, the language uses a hack to support variables with long names.

date of conception - 19.08.2024

## usage

run `make` in the project's directory, or just compile `rottent.c` yourself.

run programs like this

```sh
rottent your-program.rtn
```

## interpreter

the only data type is signed integer numbers. their bit-width is decided by the implementation.

numbers to be used are stored in the data stack.

variables and macros are stored in the virtual memory, referred to as the storage. it consists of numbers which can be indexed.


## symbols

- `'` ( -- ) skip a comment line
- `#` ( -- 0) start a number
- 0...9 (x -- x*10+digit) complete a number
- a...z (name -- name) complete a name
- `_` (x name -- ) create a variable
- `=` (name -- address) reference a variable
- `,` (x -- ) append
- `.` (address -- value) fetch a number
- `:` (value address -- ) deposit a number
- `@` (name -- ) define a macro
- `;` ( -- ) end a macro and forget its variables
- `$` (name -- ) execute a macro
- `%` (x -- x x) copy a number
- `+` (x y -- x+y)
- `-` (x y -- x-y)
- `*` (x y -- x*y)
- `/` (x y -- x/y)
- `[` (flag -- ) skip to `|` or `]` if the flag is false
- `|` ( -- ) the false part of a branch
- `]` ( -- ) end a branch
- `>` (x -- flag) push 1 if x is positive, otherwise 0
- `<` (x -- flag) push 1 if x is negative, otherwise 0
- `(` ( -- ) begin a loop
- `^` (flag -- ) continue the loop if given true
- `)` ( -- ) repeat the loop
- `"` ( -- ) write a string that ends with "
- `{` ( -- x) read a character
- `}` (x -- ) write a character
- `?` ( -- x) read a number
- `!` (x -- ) write a number


## literals

a number starts with `#`. `#` itself evaluates to zero. spaces don't separate numbers.

```
#12 3!   ' prints "123"
```

use this hack for negaive numbers.

```
##123-!   ' prints "-123"
```


## variables

start describing a variable with `#`. follow with letters or digits to create a key corresponding to your variable - a name.

```
#Ba!   ' prints the key for "BA"
```

the symbol `=` receives a name and finds the address of the corresponding variable. if the variable is not found, it gets created and it will be assigned zero.

```
#444 #UwU=:   ' UwU=444
#123 #UwU=. +!   ' prints the sum of 123 and UwU
```

the symbol `,` appends a number to the most recent definition. it's used to make arrays. the symbol `_` creates a variable even if a variable with the same name was defined earlier. the most recently defined one will be refernced.

```
#my array_   #1, #2, #3,
#my array=   #1+%.!" " #1+%.!" " #1+.!   ' prints "1 2 3"
```


## macros

a macro has to be defined before use.

the definition of a macro begins with `@` and ends with `;`. it takes a name for the macro from the stack. a macro has one exit point.

use `_` to create local variables. the data allocated when a macro ran will be lost, including data created with `,`.

```
#HI@   "hell"#!" world" ;
#minus@   #x_ ##x=.- ;   ' receive x and negate it
```

macros are invoked with $.

```
#HI$   ' prints "hell0 world"
#123 #minus$   ' a clearer alternative to "##123-"
```

variables and macros can't share names.


## storage

a definition looks like this
- name: number
- link: address of the previous definition, or 0
- data: either cells for a variable, or the address of the macro's definition in the program

for ease of implementation, definitions don't start at cell 0.


## control flow

decisions are done like this. take a number and run the first branch if it's not zero, otherwise, the second branch, if present.

```
#TRUE=. [ "true" | "false" ]
#A=.#B=.- <[ "A<B" ]
```

a loop can be infinite

```
#1 ( %! ", " #1+ )   ' prints "1, 2, 3,"...
```

or conditional. `^` ends the loop if it receives zero.

```
#10 ( %! #1- %^ ", " )#0: "."   ' prints "10, 9, 8, 7, 6, 5, 4, 3, 2, 1."
```

make sure that after a loop the stack doesn't have the loop's counter. you can safely remove it by sending it to cell 0.


## execution

the interpreter might abort execution in the following cases
- an invalid character gets interpreted
- the data stack is in an invalid state
- control structure mismatch, including at the end of the program
- the dictionary gets full
- division by zero
- an undefined macro is invoked


## notes

rottent is inspired by Mouse, a language by Peter Grogono, and so is compared to it in this section.

mouse doesn't shuffle stacks. at first i added stack operators for convenience, but in result the code became messy.

compared to rottent, mouse has a complicated interpreter, requiring looking up the next character as well as needing a separate stack for macros' arguments.

rottent has way more primitives than mouse. still, the working interpreter written in c fits in less than 300 lines of code.

the way local variables are implemented means that you can't allocate data in a macro and expect it to persist. i was caught by this. the pervious versions let you make macros for allocating data and that was awesome.

mouse chose to handle variables so primitively that it hurts their use and clarity. i tried to make variables with long names work. the storage is a linked list because it allows to have arrays that are bigger than 26 numbers. it's a jump in complexity. in mouse it takes one character to use a variable, in rottent - 3.

the branch statement in mouse doesn't have a false part. as it turns out, its implementation is unintuitive. i couldn't find use in a structuted language without a false branch.

reading programs written for mouse, for a long time i thought that `!` in text literals was a writing style and not a marker for newlines. in rottent you can write newlines either directly in the text or as `#10}`.

mouse didn't feel the need for single character input and output. i added this feature hoping to make the language usable. without direct access to files, this decision seems naive. in this implementation, input is realised with `getc()`, though `getch()` is desirable.

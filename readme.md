# Rottent

a small interpreted programming language based on Mouse.

a program in rottent consists of ascii characters which separately perform small actions. unlike mouse, the language uses a hack to support variables with long names.

date of conception - 19.08.2024

to do:
- good char input


## description

lowercase letters are interpreted as uppercase letters. note that this doesn't apply to words like `"`, since they handle their letters instead of the interpreter.

the only data type is numbers. whether they're integer or real, 16 bit or 32 bit, is decided by the implementation.

the data stack stores temporary numbers.

the interpreter is assumed to have a control stack for handling control structures and return addresses. its implementation doesn't matter.

variables and macros are stored in the virtual memory, referred to as the storage. it consists of numbers which can be indexed.

the program text and the storage may be stored in one memory region, if that makes implementation easier.


## commands

- : (x -- x x) duplicate
- . (x -- ) remove
- % (x y -- y x) swap
- > (name -- address) reference
- , (x -- ) append a number
- _ (address -- value) fetch a number
- = (value address -- ) deposit a number
- @ (name -- ) define a macro
- ; ( -- ) end a macro
- $ (name -- ) execute a macro
- { ( -- x) read a character
- } (x -- ) write a character
- ? ( -- x) read a number
- ! (x -- ) write a number
- + (x y -- x+y)
- - (x y -- x-y)
- * (x y -- x*y)
- / (x y -- x/y)
- [ (flag -- ) if
- | ( -- ) else
- ] ( -- ) end if
- < (x -- flag) check for negative
- # ( -- ) push zero
- 0...9 (x -- x*10+digit) multiply by 10 and add the digit
- a...z (x -- x*26+letter) multiply by 26 and add the index of the letter
- ( ( -- ) begin a loop
- ^ (flag -- ) continue the loop if given true
- ) ( -- ) repeat the loop
- ' ( -- ) skip a comment line
- " ( -- ) write a string that ends with "


## literals

a number starts with `#`. `#` itself evaluates to zero.

```
#123! ' prints "123 "
```


## variables

variable names are numbers in base 26. the number might overflow and the interpreter won't tell you about it. names are not case sensitive.

```
#Ba! ' prints 26
```

the s `>` finds the address of the variable referenced with the given name. if the variable is not found, it gets created.

```
#444 #OwO>= ' OWO=444
#123 #OwO>_ +! ' prints the sum of 123 and OWO
```

the command `,` appends a number to the most recent definition. it's used to make arrays.

```
#Array>. #1, #2, #3,
#Array> #1+:_!" " #1+:_!" " #1+_! ' prints "1 2 3"
```


## macros

a macro has to be defined before use.

the definition of a macro begins with `@` and ends with `;`. it takes a name for the macro from the stack. a macro is expected to have only one exit point.

```
#HI@ "hell"#!" world" ;
```

macros are invoked with $.

```
#HI$ ' prints "hell0 world"
```

variables and macros can't share names.


## dictionary

a definition looks like this
- name: number
- link: address of the previous definition, or 0
- data: either cells for a variable, or the address of the macro's definition in the program

for ease of implementation, definitions don't start at cell 0.

you may hide a definition like this

```
#0 #VAR> #1-= ' the name field of VAR is now 0
```

next time the name VAR is referenced, the previous definition won't be found, and a new one will be created.


## control flow

an "if" structure looks like this

```
#TRUE>_ [ "true" | "false" ]
#A>_#B>_- <[ "A<B" ]
```

a loop can be infinite

```
#1 ( :! ", " #1+ ) ' prints "1, 2, 3,"...
```

or conditional. `^` ends the loop if it receives false.

```
#10 ( :! #1- :^ ", " ) . "." ' prints "10, 9, 8,"...
```

a number is considered true if it's not zero.


## execution

the interpreter might abort execution in the following cases
- an invalid character gets interpreted
- the data stack is in an invalid state
- structure mismatch
- dictionary gets full
- division by zero


## rationale

mouse doesn't shuflle stacks. the stack is simply an aid in transferring data. the programmer isn't meant to think about the stack. it's a neat idea. i wonder how it would work with registers. i put the stack commands anyway because they're handy.

mouse chose to handle variables so primitively that it hurts their use and clarity. i tried to make variables with long names work. the storage is a linked list because it allows to have arrays that are bigger than 26 numbers. it's a jump in complexity.

mouse handles macros like functions in algol-likes. i'm not sure how this feature is implemented, but it seems too complex to stay.

the if-statement in mouse doesn't have an else part. as it turns out, its implementation is complicated.

reading programs written for mouse, for a long time i thought that `!` in text literals was a writing style and not a marker of newlines. in rottent you can write newlines either directly in  the text or as `#10}`.

mouse didn't feel need for single character input and output. i added this feature in hope to make the language usable. without direct access to files, this decision seems naive.

rottent has way more primitives than mouse. it could use less. rottent seems more complicated than mouse, while being just as weird to program.

the working interpreter is in less than 300 lines of c (to do: count with cloc).

i'm unaware of why mouse is the way it is, mainly because the book on it is unobainable. i designed rottent to see what mouse would be like if you were able to get things done with it. in result i brang it closer to forth. indeed, if you need to get things done, just use forth.


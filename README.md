# ⚜️ Assembler written in C

Double-pass assembler, written in ANSI C90 for an imaginary 14-bit computer.

---

## 🎯 The Mission

The goal of the project is to write an assembler for an assembly language. <br>
The assembler needs to convert the assembly language, which is defined below, into machine code, which is also defined below.

<br>

## Assembler With Two Passes

First of all, the assembler spreads the macros in the code. This stage is called the **_preProcessor_**. <br>
Then, spreading the macros, the assembler converts the assembly language into machine code. <br>
It does so by going through the code twice: <br>

-   **First Pass** - The assembler goes through the code and creates a symbol table, which contains all the labels in the code, and their addresses. It also converts all instructions into binary [words](#📫-a-word), of course only those that do not contain a [label](#🏷️-labels), since it does not yet know their addresses.<br>

-   **Second Pass** - The assembler goes through the code again, fills the missing lables addresses, and converts the binary code into [machine code](#💢-the-machine-code). <br>

---

# 🖥️ The Imaginary Computer

The computer consists of a CPU, Registers, and RAM. <br>
The CPU has 8 registers: `r0`, `r1`, `r2`, `r3`, `r4`, `r5`, `r6`, `r7`. The size of each register is 14 bits. <br>
The RAN has has 256 (0 - 255) memory cells, and the size of each memory cell is also 14 bits. <br>

> A cell in memory is also called a **_word_**. Each machine instruction is encoded into a number of memory words.

<br>

## 📫 A Word

A word is a 14-bit number, which is divided into 6 parts, described in the following table: <br>

| **Bit(s)**  | 13 12  | 11 10  | 9 8 7 6 |            5 4            |            3 2            | 1 0 |
| :---------: | :----: | :----: | :-----: | :-----------------------: | :-----------------------: | :-: |
| **Meaning** | param1 | param2 | opcode  | source operand addressing | target operand addressing | ERA |

> I'm not going to explain the meaning of each part.

<br>
<br>

## 📜 The Assembly Language

The assembly language consists of 16 different instructions: <br>

| Instruction | Opcode |
| :---------: | :----: |
|    `mov`    |   00   |
|    `cmp`    |   01   |
|    `add`    |   02   |
|    `sub`    |   03   |
|    `not`    |   04   |
|    `clr`    |   05   |
|    `lea`    |   06   |
|    `inc`    |   07   |
|    `dec`    |   08   |
|    `jmp`    |   09   |
|    `bne`    |   10   |
|    `red`    |   11   |
|    `prn`    |   12   |
|    `jsr`    |   13   |
|    `rts`    |   14   |
|   `stop`    |   15   |

## 💢 The Machine Code

The machine code consists of only two characters: `.` & `/`, where `.` represents `0` and `/` represents `1`. <br>

<br>

## 🎭 Types of Lines

There are 4 types of lines in the assembly language: <br>

-   **Empty Line** - A line that contains nothing but whitespace characters (`\t`, `space(s)` or `\n`). The assembler ignores these lines.
-   **Comment Line** - A line that starts with a semicolon (`;`). The assembler ignores these lines.
-   **Instruction Line** - A line that contains an instruction. The assembler converts these lines into machine code.
-   **Directive Line** - A line that starts with a dot (`.`). The assembler converts these lines into machine code.

### 🏷️ Labels

Each line _can_ be followed by a `label`. Label is like a variable name, which can be used to reference a memory cell.

Example of a label: <br>

```assembly
1| XYZ: mov r0, r1
2| bne LOOP(XYZ, r3)
```

<br>

## 🔬 Marcos

The assembly language supports macros. <br>
A macro is a sequence of instructions, which can be called by a single instruction. <br>
A macro is defined by the following syntax: <br>

```assembly
1| mcr MACRO_NAME
2|     MACRO_CODE
3| endmcr
```

And then can be called by just writing the macro's name: <br>

```assembly
MACRO_NAME
```

Therefor, the end result will be the same as if the macro's code was written instead of the macro's name.<br>

<br>

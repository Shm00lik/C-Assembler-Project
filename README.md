# Assembler written in C

Double-pass assembler, written in ANSI C90 for an imaginary 14-bit computer.

---

## 🎯 The Mission
The goal of the project is to write an assembler for the assembly language defined below, especially for the needs of the project. <br>
The assembler needs to convert the assembly language into machine code, which is also defined below.

<br>

## 🖥️ The Imaginary Computer
The computer consists of a CPU, Registers, and RAM. <br>
The CPU has 8 registers: `r0`, `r1`, `r2`, `r3`, `r4`, `r5`, `r6`, `r7`. The size of each register is 14 bits. <br>
The RAN has has 256 (0 - 255) memory cells, and the size of each memory cell is also 14 bits. <br>

> A cell in memory is also called a ***`word`***. Each machine instruction is encoded into a number of memory words.

<br>
<br>

### 📫 A Word
A word is a 14-bit number, which is divided into 6 parts, described in the following table: <br>

| **Bit- Num** | 13 12 | 11 10 | 9 8 7 6 | 5 4 | 3 2 | 1 0 |
|:-----:|:-----:|:-----:|:-------:|:---:|:---:|:---:|
| **Meaning** |  param1 |  param2 |  opcode   |  source operand addressing  |  target operand addressing  |  ERA  |
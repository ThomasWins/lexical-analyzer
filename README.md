# lexical-analyzer
This README details the implementation and usage of the lexical analyzer written in C for a subset of the PL/0 language.

## Overview

The lexical analyzer is designed to tokenize input source code written in the PL/0 programming language. It processes the input to produce tokens, which represent the basic building blocks of the language (keywords, symbols, identifiers, constants, etc.). Additionally, it includes error handling for syntax violations and maintains a symbol table to track declared constants, variables, and procedures.

### Key Features
1) **Tokenization:** Recognizes keywords, operators, symbols, identifiers, and constants.
2) **Symbol Table Management:** Stores information about declared identifiers (constants, variables, procedures).
3) **Error Handling:** Reports detailed syntax errors.
4) **Code Emission:** Generates intermediate code instructions for further processing.

### Files and Components

**Source Code:** The implementation is provided in lexical_analyzer.c.

**Input File:** The analyzer reads the source code from a text file (specified at runtime).

**Output:**
Tokenized representation of the input and symbol table content.

## How to Use

1) **Compile the program:**
   gcc lexical_analyzer.c -o lexical_analyzer
2) **Run the Program:**
   ./lexical_analyzer <input_file>

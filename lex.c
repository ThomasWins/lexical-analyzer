#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SYMBOL_TABLE_SIZE 500

//____________________________________________________________

typedef struct
{
    int kind;      // const = 1, var = 2, proc = 3
    char name[10]; // name up to 11 chars
    int val;       // number (ASCII value)
    int level;     // L level
    int curlevel;
    int addr; // M address
    int mark; // to indicate unavailable or deleted
} symbol;

// For constants, you must store kind, name and value.
// For variables, you must store kind, name, L and M.
//____________________________________________________________

typedef struct
{
    char tkn[10];
    char value[12];
    int type;
} token;
//____________________________________________________________

typedef struct
{
    int OP;
    int R;
    int L;
    int M;
} code;
//____________________________________________________________

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
code cd[500];
token tok;

// Global Variables
FILE *ifp;
int currentCodeIndex = 0, rp = 0, sp = 1, gp = 4, curlvl = 1, counter = 0;
//////////////////////////////////////////////////////////
typedef enum
{
    oddsym = 1,
    identsym = 2,
    numbersym = 3,
    plussym = 4,
    minussym = 5,
    multsym = 6,
    slashsym = 7,
    fisym = 8,
    eqsym = 9,
    neqsym = 10,
    lessym = 11,
    leqsym = 12,
    gtrsym = 13,
    geqsym = 14,
    lparentsym = 15,
    rparentsym = 16,
    commasym = 17,
    semicolonsym = 18,
    periodsym = 19,
    becomessym = 20,
    beginsym = 21,
    endsym = 22,
    ifsym = 23,
    thensym = 24,
    whilesym = 25,
    dosym = 26,
    constsym = 28,
    varsym = 29,
    writesym = 31,
    readsym = 32,
} wsym;
//////////////////////////////////////////////////////////
void ERROR(int errorCode);
void getToken();
void ADDTOSYMBOLTABLE(int kind, char name[], int value, int addr, int mark);
void emit(int OP, int R, int L, int M);
int SYMBOLTABLECHECK();
void FACTOR();
void TERM();
void EXPRESSION();
void CONDITION();
void STATEMENT();
void PrintSymbolTable();
//////////////////////////////////////////////////////////
void ERROR(int errorCode)
{
    printf("Error: ");
    switch (errorCode)
    {
    case 1:
        printf("PROGRAM must end with a period\n");
        break;
    case 2:
        printf("const, var, and read keywords must be followed by identifier\n");
        break;
    case 3:
        printf("symbol name has already been declared\n");
        break;
    case 4:
        printf("constants must be assigned with =\n");
        break;
    case 5:
        printf("constants must be assigned an integer value\n");
        break;
    case 6:
        printf("constant and variable declarations must be followed by a semicolon\n");
        break;
    case 7:
        printf("undeclared identifier %s\n", tok.value);
        //___________Insert undeclared identifier after printf_________________
        break;
    case 8:
        printf("only variable values may be altered\n");
        break;
    case 9:
        printf("assignment statements must use :=\n");
        break;
    case 10:
        printf("begin must be followed by end\n");
        break;
    case 11:
        printf("if must be followed by then\n");
        break;
    case 12:
        printf("while must be followed by do\n");
        break;
    case 13:
        printf("condition must contain comparison operator\n");
        break;
    case 14:
        printf("right parenthesis must follow left parenthesis\n");
        break;
    case 15:
        printf("arithmetic equations must contain operands, parentheses, numbers, or symbols\n");
        break;
    default:
        printf("Unknown Error\n");
    }
    PrintSymbolTable();
    exit(1);
}
//////////////////////////////////////////////////////////
int wordChecker(const char *string)
{
    const char *word[] = {
        "const", "var", " ", " ", "begin", "end",
        "if", "then", " ", "while", "do", "read", "write", "fi"};

    const char *symbol[] = {
        "", "", "", "", "", "", "", "", "", "", "+", "-", "*", "/", "", "=", "<>", "<", "<=", ">", ">=", "(", ")", ",", ";", ".", ":="};

    int isNumber = 1;
    for (int i = 0; i < strlen(string); i++)
    {
        if (!isdigit(string[i]))
        {
            isNumber = 0;
            break;
        }
    }

    if (isNumber)
    {
        return 28;
    }

    for (int i = 0; i < sizeof(word) / sizeof(word[0]); i++)
    {
        if (strcmp(string, word[i]) == 0)
        {
            return i + 1;
        }
    }

    for (int i = 0; i < sizeof(symbol) / sizeof(symbol[0]); i++)
    {
        if (strcmp(string, symbol[i]) == 0)
        {
            return i + 1; // Return the code for symbols
        }
    }

    return 0; // Not found
}

//////////////////////////////////////////////////////////
void getToken()
{
    tok.type = 0; // Reset the token type to 0
    int result = fscanf(ifp, "%s", tok.tkn);

    if (result == EOF)
    {
        tok.tkn[0] = '\0';
        tok.value[0] = '\0';
    }
    else
    {

        int wordValue = wordChecker(tok.tkn);

        if (wordValue != 0)
        {
            // Save the word as the value
            strcpy(tok.value, tok.tkn);

            // Convert the token to the corresponding number
            switch (wordValue)
            {
            case 1:
                tok.type = constsym;
                break;
            case 2:
                tok.type = varsym;
                break;
            case 5:
                tok.type = beginsym;
                break;
            case 6:
                tok.type = endsym;
                break;
            case 7:
                tok.type = ifsym;
                break;
            case 8:
                tok.type = thensym;
                break;
            case 10:
                tok.type = whilesym;
                break;
            case 11:
                tok.type = dosym;
                break;
            case 12:
                tok.type = readsym;
                break;
            case 13:
                tok.type = writesym;
                break;
            case 14:
                tok.type = fisym;
                break;
            case 15:
                tok.type = oddsym;
                break;
            case 16:
                tok.type = eqsym;
                break;
            case 17:
                tok.type = neqsym;
                break;
            case 18:
                tok.type = lessym;
                break;
            case 19:
                tok.type = leqsym;
                break;
            case 20:
                tok.type = gtrsym;
                break;
            case 21:
                tok.type = geqsym;
                break;
            case 22:
                tok.type = lparentsym;
                break;
            case 23:
                tok.type = rparentsym;
                break;
            case 24:
                tok.type = commasym;
                break;
            case 25:
                tok.type = semicolonsym;
                break;
            case 26:
                tok.type = periodsym;
                break;
            case 27:
                tok.type = becomessym;
                break;
            case 28:
                tok.type = numbersym;
                break;
            default:
                fprintf(stderr, "Error: Unknown word value %d.\n", wordValue);
                exit(1);
            }
        }
        else
        {
            tok.type = identsym; // Assuming it's an identifier if not a reserved word
            tok.value[0] = '\0';
        }
    }
}

//////////////////////////////////////////////////////////
void ADDTOSYMBOLTABLE(int kind, char name[], int value, int addr, int mark)
{
    symbol_table[counter].kind = kind;
    strcpy(symbol_table[counter].name, name);
    symbol_table[counter].val = value;
    symbol_table[counter].addr = addr;
    symbol_table[counter].mark = 1;
    counter++;
}
//////////////////////////////////////////////////////////
void emit(int OP, int R, int L, int M)
{
    cd[currentCodeIndex].OP = OP;
    cd[currentCodeIndex].R = R;
    cd[currentCodeIndex].L = L;
    cd[currentCodeIndex].M = M;
    currentCodeIndex++;
}
//////////////////////////////////////////////////////////
// linear search through symbol table looking at name
int SYMBOLTABLECHECK()
{
    for (int i = 0; i < counter; i++)
    {
        if (strcmp(symbol_table[i].name, tok.tkn) == 0)
        {
            printf("%d\n", tok.tkn);
            return i; // Return index
        }
    }
    return -1; // Name was not found
}
//////////////////////////////////////////////////////////
void FACTOR()
{
    int symIdx;
    if (tok.type == 2)
    { // Token = identsym
        symIdx = SYMBOLTABLECHECK();
        if (symIdx == -1)
            ERROR(7); // undeclared identifier

        if (symbol_table[symIdx].kind == 1)
        {                                            // Const
            emit(1, 0, 0, symbol_table[symIdx].val); // LIT
        }
        else
        {                                             // Var
            emit(3, 0, 0, symbol_table[symIdx].addr); // LOD
        }
        getToken();
    }
    else if (tok.type == 3)
    {                                   // Token = numbersym
        emit(1, 0, 0, atoi(tok.value)); // LIT
        getToken();
    }
    else if (tok.type == 15)
    { // Token = lparentsym
        getToken();
        EXPRESSION();
        if (tok.type != 16) // Token != rparentsym
            ERROR(14);      // right parenthesis must follow left parenthesis
        getToken();
    }
    else
    {
        ERROR(15); // arithmetic equations must contain operands, parentheses, numbers, or symbols
    }
}
//////////////////////////////////////////////////////////
void TERM()
{
    FACTOR();
    // token = multsym or slashsym or modsym
    while ((tok.type == 6) || (tok.type == 7) || (tok.type == 8))
    {
        if (tok.type == 6)
        { // multsym
            getToken();
            FACTOR();
            emit(5, 0, 0, 0); // MUL
        }
        if (tok.type == 7)
        { // slashsym
            getToken();
            FACTOR();
            emit(6, 0, 0, 0); // DIV
        }
        if (tok.type == 8)
        { // modsym
            getToken();
            FACTOR();
            emit(7, 0, 0, 0); // MOD
        }
    }
}
//////////////////////////////////////////////////////////
void EXPRESSION()
{
    getToken();
    printf("\n%s\n%d\n", tok.tkn, tok.type);
    if (tok.type == 5)
    { // minussym
        getToken();
        TERM();

        while ((tok.type == 4) || (tok.type == 5))
        { // token = plussym or minussym
            if (tok.type == 4)
            {
                getToken();
                TERM();
                emit(3, 0, 0, 0); // ADD
            }
            else
            {
                getToken();
                TERM();
                emit(4, 0, 0, 0); // SUB
            }
        }
    }
    else
    {
        if (tok.type == 4)
        { // plussym
            getToken();
        }
        TERM();
        while ((tok.type == 4) || (tok.type == 5))
        { // token = plussym or minussym
            if (tok.type == 4)
            {
                getToken();
                TERM();
                emit(3, 0, 0, 0); // ADD
            }
            else
            {
                getToken();
                TERM();
                emit(4, 0, 0, 0); // SUB
            }
        }
    }
}
//////////////////////////////////////////////////////////
void CONDITION()
{

    if (tok.type == 1)
    { // token == oddsym

        getToken();
        EXPRESSION();

        emit(15, 0, 0, 0); // ODD
    }
    //________________________________________________________
    else
    {
        EXPRESSION();
        //________________________________________________________
        if (tok.type == 9)
        { // Token == eqlsym
            getToken();
            EXPRESSION();
            emit(9, 0, 0, 0); // EQL
        }
        //________________________________________________________
        else if (tok.type == 10)
        { // Token == neqsym
            getToken();
            EXPRESSION();
            emit(10, 0, 0, 0); // NEQ
        }
        //________________________________________________________
        else if (tok.type == 11)
        { // Token == lessym
            getToken();
            EXPRESSION();
            emit(11, 0, 0, 0); // LES
        }
        //________________________________________________________
        else if (tok.type == 12)
        { // Token == leqsym
            getToken();
            EXPRESSION();
            emit(12, 0, 0, 0); // LEQ
        }
        //________________________________________________________
        else if (tok.type == 13)
        { // Token == gtrsym
            getToken();
            EXPRESSION();
            emit(13, 0, 0, 0); // GTR
        }
        //________________________________________________________
        else if (tok.type == 14)
        { // Token == geqsym
            getToken();
            EXPRESSION();
            emit(14, 0, 0, 0); // GEQ
        }
        else
            ERROR(13); // condition must contain comparison operator
    }
}
//////////////////////////////////////////////////////////
void STATEMENT()
{
    int symIdx = 0, jpcIdx = 0;
    //________________________________________________________
    if (tok.type == 2)
    { // identsym

        symIdx = SYMBOLTABLECHECK();

        if (symIdx == -1)
            ERROR(7); // undeclared identifier

        if (symbol_table[symIdx].kind != 2)
            ERROR(8); // only variable values may be altered

        getToken();
        printf("\n%s\n%d\n", tok.tkn, tok.type);
        if (tok.type != 20)
            ERROR(9); // assignment statements must use :=\n
        getToken();
        printf("\n%s\n%d\n", tok.tkn, tok.type);
        EXPRESSION();
        emit(4, 0, 0, symbol_table[symIdx].addr); // STO
        return;
    }
    //________________________________________________________
    if (tok.type == 21)
    { // token = beginsym
        do
        {
            getToken();
            printf("\n%s\n%d\n", tok.tkn, tok.type);
            STATEMENT();
        } while (tok.type == 18); // token = semicolonsym
        if (tok.type != 22)
            ERROR(10); // begin must be followed by end
        getToken();
        return;
    }
    //________________________________________________________
    if (tok.type == 23) // CHECK THIS
    {                   // token = ifsym
        getToken();
        printf("\n%s\n%d\n", tok.tkn, tok.type);
        CONDITION();
        jpcIdx = currentCodeIndex;
        emit(6, 0, 0, 0); // JPC

        if (tok.type != 24) // token != thensym
            ERROR(11);      // if must be followed by then

        getToken();
        printf("\n%s\n%d\n", tok.tkn, tok.type);
        STATEMENT();
        cd[jpcIdx].M = currentCodeIndex;
        return;
    }
    //________________________________________________________
    if (tok.type == 25)
    { // token == whilesym
        getToken();
        int loopIdx = currentCodeIndex;

        CONDITION();

        if (tok.type != 26) // token != dosym
            ERROR(12);      // while must be followed by do

        getToken();
        int jpcIdx = currentCodeIndex;
        emit(6, 0, 0, 0); // JPC

        STATEMENT();
        emit(7, 0, 0, loopIdx); // JMP
        cd[jpcIdx].M = currentCodeIndex;
        return;
    }
    //________________________________________________________
    if (tok.type == 32)
    { // token = readsym
        getToken();
        if (tok.type != 2)
            ERROR(15); // arithmetic equations must contain operands, parentheses, numbers, or symbols

        symIdx = SYMBOLTABLECHECK();
        if (symIdx == -1)
            ERROR(7); // undeclared identifier

        if (symbol_table[symIdx].kind != 2) // Not a var
            ERROR(8);                       // only variable values may be altered

        getToken();
        emit(9, 0, 2, symbol_table[symIdx].addr); // READ
        emit(4, 0, 0, symbol_table[symIdx].addr); // STO
        return;
    }
    //________________________________________________________
    if (tok.type == 31)
    { // token = writesym
        getToken();
        EXPRESSION();
        emit(13, 0, 0, 0); // WRITE
        return;
    }
}
//////////////////////////////////////////////////////////
void CONST_DECLARATION(char *name, int *val)
{

    if (tok.type != 2)
        ERROR(2); // Token != identsym

    strcpy(name, tok.tkn);
    if (SYMBOLTABLECHECK() != -1)
        ERROR(3); // Name has already been declared

    getToken();
    printf("\n%s\n%d\n", tok.tkn, tok.type);
    if (tok.type != 9)
        ERROR(4); // Constants must be assigned with =

    getToken();
    printf("\n%s\n%d\n", tok.tkn, tok.type);
    if (tok.type != 3)
        ERROR(5); // constants must be assigned an integer value

    val = atoi(tok.value);
    ADDTOSYMBOLTABLE(1, name, val, 0, 0);

    getToken();
    printf("\n%s\n%d\n", tok.tkn, tok.type);
}

//////////////////////////////////////////////////////////
int VAR_DECLARATION(char *name, int numVars)
{
    do
    {

        numVars++;
        getToken();
        printf("\n%s\n%d\n", tok.tkn, tok.type);
        if (tok.type != 2)
            ERROR(2); // Token != identsym

        strcpy(name, tok.tkn);
        if (SYMBOLTABLECHECK() != -1)
            ERROR(3); // Name has already been declared

        ADDTOSYMBOLTABLE(2, name, 0, 2 + sp++, 0);

        getToken();
        printf("\n%s\n%d\n", tok.tkn, tok.type);
    } while (tok.type == 17); // While token is ,

    if (tok.type != 18)
        ERROR(6); // constant and variable declarations must be followed by a semicolon
    getToken();
    printf("\n%s\n%d\n", tok.tkn, tok.type);
    return numVars;
}
//////////////////////////////////////////////////////////
void BLOCK()
{
    char tmpName[12] = {0};
    int sp = 4, v, numVars = 0;
    do
    {
        // Constant Declaration
        if (tok.type == 28)
        { // token is constsym
            getToken();
            printf("\n%s\n%d\n", tok.tkn, tok.type);
            do
            {
                CONST_DECLARATION(tmpName, gp);
                while (tok.type == 17)
                { // Token = ,
                    getToken();
                    printf("\n%s\n%d\n", tok.tkn, tok.type);
                    CONST_DECLARATION(tmpName, gp);
                }

                if (tok.type != 18)
                    ERROR(6); // constant and variable declarations must be followed by a semicolon

                getToken();
                printf("\n%s\n%d\n", tok.tkn, tok.type);
            } while (tok.type == 2); // Double check this
        }
        // Variable Declaration

        if (tok.type == 29) // token is varsym
            numVars = VAR_DECLARATION(tmpName, numVars);

    } while ((tok.type == 28) || (tok.type == 29));

    emit(0, 0, 0, numVars + 3);
    STATEMENT();
}
//////////////////////////////////////////////////////////
void PROGRAM()
{
    getToken();
    printf("\n%s\n%d\n", tok.tkn, tok.type);
    BLOCK();
    if (tok.type != 19)
        ERROR(1);      // Program must end with a period
    emit(11, 0, 0, 3); // Halt
}
//////////////////////////////////////////////////////////
void PrintSymbolTable()
{
    printf("Symbol Table:\n");
    printf("Kind | Name | Value | Level | Address | Mark\n");
    printf("----------------------------------------------\n");
    for (int i = 0; i < counter; i++)
    {
        printf("   %d |    %s |     %d |     %d |       %d |    %d\n", symbol_table[i].kind, symbol_table[i].name, symbol_table[i].val, symbol_table[i].level, symbol_table[i].addr, symbol_table[i].mark);
    }
}

int main(int argc, char *argv[])
{
    /*
    if (argc != 2)
    {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Open the file for reading
    ifp = fopen(argv[1], "r");
    */
    ifp = fopen("lexemeList.txt", "r");

    if (ifp == NULL)
    {
        fprintf(stderr, "Error: File pointer is not initialized.\n");
        exit(1);
    }

    PROGRAM();
    PrintSymbolTable();

    // Close the file after reading
    fclose(ifp);

    return 0;
}
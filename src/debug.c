/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       debug.c
 * Autor(i):    Fridolin Pokorny
 *
 * Operacie nad suborom run.log, ktory zaznamenava informacie o behu programu
 * pri debugovani.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "debug.h"
#include "instruction.h"
#include "variable.h"
#include "scanner.h"
#include "ial.h"
#include "pparser.h"
#include "ifj11.h"
#include "interpret.h"
#include "xwrappers.h"
#include "pstack.h"


#ifdef DEBUG

#define HTABLE_SIZE 2

/* Subor run.log */
static FILE * debug_file = NULL;

/**
 * Otvori subor run.log - pripravi na logovanie.
 * @param - void
 * @return - void
 */
void DEBUG_START(void)
{
    if (! (debug_file = fopen("run.log", "w")))
    {
        perror("run.log");
        setvbuf(debug_file, NULL, _IONBF, 0);
    }
}

/**
 * Zapise spravu do logu run.log.
 * @param - printf-like arguments
 * @return - void
 */
void DEBUG_MSG(const char * format, ...)
{
    va_list arg;
    va_start(arg, format);

    fflush(debug_file);

    if (debug_file)
        vfprintf(debug_file, format, arg);

    va_end(arg);
}

/**
 * Uzavrie subor run.log - ukonci logovanie.
 * @param - void
 * @return - void
 */
void DEBUG_END(void)
{
    if (fclose(debug_file))
        perror("run.log");
}

/**
 * Vypise token do suboru run.log
 * @param - argument ukazatel na token
 * @return - void
 */
void DEBUG_PRINT_TOKEN(const Ttoken * token)
{
    switch (token->id)
    {
        case TKN_ID:
            fprintf(debug_file, "TOKEN:\tID | %s\n", token->val.str);
            break;

        case TKN_NUMBER:
            fprintf(debug_file, "TOKEN:\tNUM | %lf\n", token->val.num);
            break;

        case TKN_STRING:
            fprintf(debug_file, "TOKEN:\tSTR | %s\n", token->val.str);
            break;

        case TKN_OP_ASSIGNMENT:
            fprintf(debug_file, "TOKEN:\tOP | =\n");
            break;

        case TKN_BRACKET_L:
            fprintf(debug_file, "TOKEN:\tOP | (\n");
            break;

        case TKN_BRACKET_R:
            fprintf(debug_file, "TOKEN:\tOP | )\n");
            break;

        case TKN_OP_POWER:
            fprintf(debug_file, "TOKEN:\tOP | ^\n");
            break;

        case TKN_OP_MUL:
            fprintf(debug_file, "TOKEN:\tOP | *\n");
            break;

        case TKN_OP_DIV:
            fprintf(debug_file, "TOKEN:\tOP | /\n");
            break;

        case TKN_OP_ADD:
            fprintf(debug_file, "TOKEN:\tOP | +\n");
            break;

        case TKN_OP_SUB:
            fprintf(debug_file, "TOKEN:\tOP | -\n");
            break;

        case TKN_OP_CONC:
            fprintf(debug_file, "TOKEN:\tOP | ..\n");
            break;

        case TKN_OP_LESS:
            fprintf(debug_file, "TOKEN:\tOP | <\n");
            break;

        case TKN_OP_GREATER:
            fprintf(debug_file, "TOKEN:\tOP | >\n");
            break;

        case TKN_OP_LESS_EQUAL:
            fprintf(debug_file, "TOKEN:\tOP | <=\n");
            break;

        case TKN_OP_GREATER_EQUAL:
            fprintf(debug_file, "TOKEN:\tOP | >=\n");
            break;

        case TKN_OP_NOT_EQUAL:
            fprintf(debug_file, "TOKEN:\tOP | ~=\n");
            break;

        case TKN_OP_EQUAL:
            fprintf(debug_file, "TOKEN:\tOP | ==\n");
            break;

        case TKN_COMMA:
            fprintf(debug_file, "TOKEN:\tOP | ,\n");
            break;

        case TKN_SEMICOL:
            fprintf(debug_file, "TOKEN:\tOP | ;\n");
            break;

        case TKN_OP_MOD:
            fprintf(debug_file, "TOKEN:\tOP | %%\n");
            break;

        case TKN_OP_STRLEN:
            fprintf(debug_file, "TOKEN:\tOP | #\n");
            break;


        case TKN_KW_DO:
            fprintf(debug_file, "TOKEN:\tKEY WORD | do\n");
            break;

        case TKN_KW_IF:
            fprintf(debug_file, "TOKEN:\tKEY WORD | if\n");
            break;

        case TKN_KW_THEN:
            fprintf(debug_file, "TOKEN:\tKEY WORD | then\n");
            break;

        case TKN_KW_ELSE:
            fprintf(debug_file, "TOKEN:\tKEY WORD | else\n");
            break;

        case TKN_KW_LOCAL:
            fprintf(debug_file, "TOKEN:\tKEY WORD | local\n");
            break;

        case TKN_KW_TRUE:
            fprintf(debug_file, "TOKEN:\tKEY WORD | true\n");
            break;

        case TKN_KW_END:
            fprintf(debug_file, "TOKEN:\tKEY WORD | end\n");
            break;

        case TKN_KW_NIL:
            fprintf(debug_file, "TOKEN:\tKEY WORD | nil\n");
            break;

        case TKN_KW_WHILE:
            fprintf(debug_file, "TOKEN:\tKEY WORD | while\n");
            break;

        case TKN_KW_FALSE:
            fprintf(debug_file, "TOKEN:\tKEY WORD | false\n");
            break;

        case TKN_KW_READ:
            fprintf(debug_file, "TOKEN:\tKEY WORD | read\n");
            break;

        case TKN_KW_WRITE:
            fprintf(debug_file, "TOKEN:\tKEY WORD | write\n");
            break;

        case TKN_KW_FUNCTION:
            fprintf(debug_file, "TOKEN:\tKEY WORD | function\n");
            break;

        case TKN_KW_RETURN:
            fprintf(debug_file, "TOKEN:\tKEY WORD | return\n");
            break;


        case TKN_RW_AND:
            fprintf(debug_file, "TOKEN:\tRESERVED WORD | and\n");
            break;

        case TKN_RW_BREAK:
            fprintf(debug_file, "TOKEN:\tRESERVED WORD | break\n");
            break;

        case TKN_RW_ELSEIF:
            fprintf(debug_file, "TOKEN:\tRESERVED WORD | elseif\n");
            break;

        case TKN_RW_FOR:
            fprintf(debug_file, "TOKEN:\tRESERVED WORD | for\n");
            break;

        case TKN_RW_IN:
            fprintf(debug_file, "TOKEN:\tRESERVED WORD | in\n");
            break;

        case TKN_RW_NOT:
            fprintf(debug_file, "TOKEN:\tRESERVED WORD | not\n");
            break;

        case TKN_RW_OR:
            fprintf(debug_file, "TOKEN:\tRESERVED WORD | or\n");
            break;

        case TKN_RW_REPEAT:
            fprintf(debug_file, "TOKEN:\tRESERVED WORD | repeat\n");
            break;

        case TKN_RW_UNTIL:
            fprintf(debug_file, "TOKEN:\tRESERVED WORD | until\n");
            break;

        case TKN_BI_TYPE:
            fprintf(debug_file, "TOKEN:\tBUILT-IN FUNCTION | type\n");
            break;

        case TKN_BI_SUBSTR:
            fprintf(debug_file, "TOKEN:\tBUILT-IN FUNCTION | substr\n");
            break;

        case TKN_BI_FIND:
            fprintf(debug_file, "TOKEN:\tBUILT-IN FUNCTION | find\n");
            break;

        case TKN_BI_SORT:
            fprintf(debug_file, "TOKEN:\tBUILT-IN FUNCTION | sort\n");
            break;

        case TKN_EOF:
            fprintf(debug_file, "TOKEN:\tEND OF FILE\n");
            break;

        default:
            assert(0);
            break;
    }
}

/**
 * Vypise instrukciu a obsah ich operandov v zavislosti na type instrukcii.
 * @param - instrukcia k vypisu do run.log
 * @return - void
 */
void DEBUG_PRINT_INSTRUCTION(const Tinstruction * inst)
{

    fprintf(debug_file, "%p\n", (void*)inst);

    switch (inst->opcode)
    {
        case INST_ADD:
            fprintf(debug_file, "\tadd [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_MUL:
            fprintf(debug_file, "\tmul [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_POW:
            fprintf(debug_file, "\tpow [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_DIV:
            fprintf(debug_file, "\tdiv [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_MOD:
            fprintf(debug_file, "\tmod [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_SUB:
            fprintf(debug_file, "\tsub [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_AND:
            fprintf(debug_file, "\tand [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_OR:
             fprintf(debug_file, "\tor [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

           break;
        case INST_NOT:
            fprintf(debug_file, "\tnot [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            break;

        case INST_CONC:
            fprintf(debug_file, "\tconc [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);
            break;
        case INST_CMPL:
            fprintf(debug_file, "\tcmpl [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_CMPLE:
            fprintf(debug_file, "\tcmple [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_CMPG:
            fprintf(debug_file, "\tcmpg [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_CMPGE:
            fprintf(debug_file, "\tcmpge [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_CMPE:
            fprintf(debug_file, "\tcmpe [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_CMPNE:
            fprintf(debug_file, "\tcmpne [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_JMP:
            fprintf(debug_file, "\tjmp [%p]\n",
                    (void *) inst->dst.var);

            break;
        case INST_CLR:
            fprintf(debug_file, "\tclr [%p]\n", (void *) inst->dst.var);

            break;
        case INST_INC:
            fprintf(debug_file, "\tinc [%p]\n", (void *) inst->dst.var);
            fputs("\tINST_OP dst:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->dst.var);

            break;
        case INST_JNIF:
            fprintf(debug_file, "\tjnif [%p], [%p]\n",
                    (void *) inst->dst.inst,
                    (void *) inst->src1.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);

            break;
        case INST_JIF:
            fprintf(debug_file, "\tjif [%p], [%p]\n",
                    (void *) inst->dst.inst,
                    (void *) inst->src1.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);

            break;
        case INST_MOV:
            fprintf(debug_file, "\tmov [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);

            break;
        case INST_CPY:
            fprintf(debug_file, "\tcpy [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);

            break;
        case INST_ENTER:
            fputs("\tenter\n", debug_file);

            break;
        case INST_NOP:
            fputs("\tnop\n", debug_file);

            break;
        case INST_LEAVE:
            fputs("\tleave\n", debug_file);

            break;
        case INST_READ:
            fprintf(debug_file, "\tread [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);

            break;
        case INST_WRITE:
            fprintf(debug_file, "\twrite [%p]\n",
                    (void *) inst->src1.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);

            break;
        case INST_TYPE:
            fprintf(debug_file, "\ttype [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);

            break;
        case INST_SUBSTR:
            fprintf(debug_file, "\tsubstr [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->dst.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src3:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_STRLEN:
            fprintf(debug_file, "\tstrlen [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);

            break;
        case INST_FIND:
            fprintf(debug_file, "\tfind [%p], [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var,
                    (void *) inst->src2.var);
            fputs("\tINST_OP dst:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->dst.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);
            fputs("\tINST_OP src2:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src2.var);

            break;
        case INST_SORT:
            fprintf(debug_file, "\tsort [%p], [%p]\n",
                    (void *) inst->dst.var,
                    (void *) inst->src1.var);
            fputs("\tINST_OP src1:\t", debug_file);
            DEBUG_PRINT_VARIABLE(inst->src1.var);

            break;
        case INST_CALL:
            fprintf(debug_file, "\tcall [%p]\n",
                    (void *) inst->dst.var);
            break;
        default:
            assert(0);
            break;
    }
    fflush(debug_file);
}

/**
 * Vypis premennej v zavislosti od jej typu.
 * @param - premenna k vypisu do run.log
 * @return - void
 */
void DEBUG_PRINT_VARIABLE(const Tvariable * var)
{
    if (! var)
        return;

    switch (var->type)
    {
        case VAR_NUM:
            fprintf(debug_file, "VAR:\t%g (NUM)\n", var->val.num);

            break;
        case VAR_NIL:
            fputs("VAR:\tnil (NIL)\n", debug_file);

            break;
        case VAR_STR:
            if (! var->val.str)
                fprintf(debug_file, "VAR:\tNULL (STR)\n");
            else
                fprintf(debug_file, "VAR:\t\"%s\" (STR)\n", var->val.str);

            break;
        case VAR_BOOL:
            if (var->val.boolean)
                fputs("VAR:\ttrue (BOOL)\n", debug_file);
            else
                fputs("VAR:\tfalse (BOOL)\n", debug_file);

            break;
        default:
            assert(0);
            break;
    }
}

/**
 * funkcia na testovanie pparseru s interpretom
 * @param - void
 * @return - void
 */
void DEBUG_PPARSER(void)
{
    static char * fname_orig = "funble";
    static char * str1_orig = "cigan jedinacik\n"; // najkratsi vtip aky poznam
    static char * str2_orig = "bol dvojca";

    static char * varname1_orig = "num1";
    static char * varname2_orig = "num2";
    static char * varname3_orig = "s1";
    static char * varname4_orig = "s2";
    static char * varname5_orig = "nnil";
    static char * varname6_orig = "ttrue";
    static char * varname7_orig = "ffalse";

    char * fname = xmalloc(strlen(fname_orig) * sizeof(char) + 1);
    strcpy(fname, fname_orig);

    char * varname1 = xmalloc(strlen(varname1_orig) * sizeof(char) + 1);
    strcpy(varname1, varname1_orig);

    char * varname2 = xmalloc(strlen(varname2_orig) * sizeof(char) + 1);
    strcpy(varname2, varname2_orig);

    char * varname3 = xmalloc(strlen(varname3_orig) * sizeof(char) + 1);
    strcpy(varname3, varname3_orig);

    char * varname4 = xmalloc(strlen(varname4_orig) * sizeof(char) + 1);
    strcpy(varname4, varname4_orig);

    char * varname5 = xmalloc(strlen(varname5_orig) * sizeof(char) + 1);
    strcpy(varname5, varname5_orig);

    char * varname6 = xmalloc(strlen(varname6_orig) * sizeof(char) + 1);
    strcpy(varname6, varname6_orig);

    char * varname7 = xmalloc(strlen(varname7_orig) * sizeof(char) + 1);
    strcpy(varname7, varname7_orig);

    Thtable * htable = ial_htable_init(HTABLE_SIZE);
    /*
     * Nazov funkcie je nutne vytvorit na halde, pretoze funkcia na uvolnenie
     * hash tabulky uvolni aj nazov funkcie.
     */
    Thtable_item * func_ptr = ial_htable_insert(htable, fname);

    /*
     * nazov vlozenej funkcie - funble
     *
     * vlozenie premennych:
     * VAR_NUM:
     * num1 = 20.123
     * num2 = 15.123
     *
     * VAR_STR:
     * s1 = "cigan jedinacik\n"
     * s2 = "bol dvojca"
     *
     * VAR_BOOL:
     * ttrue = true
     * ffalse = false
     *
     * VAR_NIL:
     * nnil = nil
     */
    union variable_val var_union;

    /* number premenne */
    var_union.num = 20.123;
    Tvariable * variable1 = variable_create(varname1, VAR_NUM, var_union);
    ial_htable_insert_var(variable1, func_ptr);

    var_union.num = 15.123;
    Tvariable * variable2 = variable_create(varname2, VAR_NUM, var_union);
    ial_htable_insert_var(variable2, func_ptr);

    /* string premenne */
    var_union.str = xmalloc(strlen(str1_orig) * sizeof(char) + 1);
    strcpy(var_union.str, str1_orig);
    Tvariable * variable3 = variable_create(varname3, VAR_STR, var_union);
    ial_htable_insert_var(variable3, func_ptr);

    var_union.str = xmalloc(strlen(str2_orig) * sizeof(char) + 1);
    strcpy(var_union.str, str2_orig);
    Tvariable * variable4 = variable_create(varname4, VAR_STR, var_union);
    ial_htable_insert_var(variable4, func_ptr);

    /* nil premenna */
    Tvariable * variable5 = variable_create(varname5, VAR_NIL, var_union);
    ial_htable_insert_var(variable5, func_ptr);

    /* boolean premenne */
    var_union.boolean = true;
    Tvariable * variable6 = variable_create(varname6, VAR_BOOL, var_union);
    ial_htable_insert_var(variable6, func_ptr);

    var_union.boolean = false;
    Tvariable * variable7 = variable_create(varname7, VAR_BOOL, var_union);
    ial_htable_insert_var(variable7, func_ptr);



    Ttoken token;
    scanner_next_token(&token);

    Tvariable * ret_pparser_variable = pparser(htable, func_ptr, &token, false);
    DEBUG_MSG("aa\n");
    DEBUG_MSG("aa\n");
    DEBUG_MSG("aa\n");
    if (ret_pparser_variable == NULL)
    {
        if (get_error() == RET_ERR_SYN)
            fprintf(debug_file, "Syntakticha chyba!\n");
        else if (get_error() == RET_ERR_SEM)
            fprintf(debug_file, "Semanticka chyba!\n");
        else if (get_error() == RET_ERR_MY)
            fprintf(debug_file, "Interna chyba!\n");
        else if (get_error() == RET_ERR_LEX)
            fprintf(debug_file, "Lexikalna chyba!\n");
        else
            fprintf(debug_file, "Ina chyba !!\n");
    }
    else
    {
        if (ret_pparser_variable->type == VAR_NUM)
        {
            fprintf(debug_file, "Vysledok je cislo\n");

            union inst_operand inst_union_a = { .var = ret_pparser_variable};
            union inst_operand inst_union_b;
            union inst_operand inst_union_c;
            Tinstruction * instruction = inst_create(INST_WRITE, inst_union_b,
                                                 inst_union_a, inst_union_c);
            inst_append(&func_ptr->inst, instruction);
            interpret_run(func_ptr->inst.first);
            fprintf(debug_file, "\n");

        }
        else if (ret_pparser_variable->type == VAR_STR)
        {
            fprintf(debug_file, "Vysledok je STRING\n");

            union inst_operand inst_union_a = { .var = ret_pparser_variable};
            union inst_operand inst_union_b;
            union inst_operand inst_union_c;
            Tinstruction * instruction = inst_create(INST_WRITE, inst_union_b,
                                                 inst_union_a, inst_union_c);
            inst_append(&func_ptr->inst, instruction);
            interpret_run(func_ptr->inst.first);
            fprintf(debug_file, "\n");
        }
        else if (ret_pparser_variable->type == VAR_BOOL)
        {
            fprintf(debug_file, "Vysledok je BOOLEAN\n");
        }
        else if (ret_pparser_variable->type == VAR_NIL)
        {
            fprintf(debug_file, "Vysledok je NIL\n");
        }
        else
        {
            fprintf(debug_file, "Vysledok je ineho typu XXX !!\n");
        }

    for (Tinstruction * instruction = func_ptr->inst.first; instruction != NULL;
         instruction = instruction->next)
                DEBUG_PRINT_INSTRUCTION(instruction);

    }

    //ial_htable_delete(htable);
}

/**
 * vypise cely obsah pstack
 * @param - void
 * @return - void
 */
void DEBUG_PSTACK_PRINT_ALL(void)
{
    DEBUG_MSG("\n\nVypis obsahu pstack\n");

    for (int index = pstack_top_index(); index >= 0; index--)
    {
        enum pstack_item_type type = pstack_type_from_index(index);

        switch (type)
        {
            case START:
            {
                DEBUG_MSG("START\n");
                break;
            }

            case REDUCE:
            {
                DEBUG_MSG("REDUCE\n");
                break;
            }

            case EXPR:
            {
                DEBUG_MSG("EXPR\n");
                break;
            }

            case ADD:
            {
                DEBUG_MSG("ADD\n");
                break;
            }

            case SUB:
            {
                DEBUG_MSG("SUB\n");
                break;
            }

            case MUL:
            {
                DEBUG_MSG("MUL\n");
                break;
            }

            case DIV:
            {
                DEBUG_MSG("DIV\n");
                break;
            }

            case CONCAT:
            {
                DEBUG_MSG("CONCAT\n");
                break;
            }

            case POWER:
            {
                DEBUG_MSG("POWER\n");
                break;
            }

            case EQUAL:
            {
                DEBUG_MSG("EQUAL\n");
                break;
            }

            case NOT_EQUAL:
            {
                DEBUG_MSG("NOT_EQUAL\n");
                break;
            }

            case GREATER:
            {
                DEBUG_MSG("GREATER\n");
                break;
            }

            case LESS:
            {
                DEBUG_MSG("LESS\n");
                break;
            }

            case GREATER_EQUAL:
            {
                DEBUG_MSG("GREATER_EQUAL\n");
                break;
            }

            case LESS_EQUAL:
            {
                DEBUG_MSG("LESS_EQUAL\n");
                break;
            }

            case BRACKET_L:
            {
                DEBUG_MSG("BRACKET_L\n");
                break;
            }

            case BRACKET_R:
            {
                DEBUG_MSG("BRACKET_R\n");
                break;
            }

            default:
            {
                DEBUG_MSG("\nINY TYP\n");
                break;
            }
        }
    }
    DEBUG_MSG("Koniec vypisu pstack\n\n");
}

/**
 * vypise cely obsah htable, instrukcie aj premenne
 * @param - hash tabulka
 * @return - void
 */
void DEBUG_PRINT_HTABLE(const struct Thtable * htable)
{
    assert(htable);

    DEBUG_MSG("\nVYPIS HTABLE\n");

    for(unsigned int i = 0; i < htable->htable_size; i++)
    {
        Thtable_item * func = htable->uk[i];

        while(func)
        {

            DEBUG_MSG("\nFunkcia: %s\n", func->key);
            DEBUG_MSG("Pocet parametrov: %d\n", func->param_number);
            DEBUG_MSG("Navratova hodnota: ");
            DEBUG_PRINT_VARIABLE(&func->ret_value);

            DEBUG_MSG("\tInstrukcie:\n");
            for (Tinstruction * inst = func->inst.first; inst; inst = inst->next)
                DEBUG_PRINT_INSTRUCTION(inst);

            DEBUG_MSG("\tPremenne:\n");
            for (Tvariable * var = func->var.first; var; var = var->next)
                DEBUG_PRINT_VARIABLE(var);
            func = func->next;
        }
    }
}

#endif /* DEBUG */

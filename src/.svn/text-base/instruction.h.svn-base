/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       instruction.h
 * Autor(i):    Fridolin Pokorny
 *
 * Implementacia zoznamu trojadresnych instrukcii.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#ifndef IFJ11_INSTRUCTION_H_
#define IFJ11_INSTRUCTION_H_

/*
 * Typ vykonavanej trojadresnej instrukcie.
 */
enum inst_type {
    /*
     * Binarne
     */
    INST_ADD,   /* dst = src1 + src2    */
    INST_MUL,   /* dst = src1 * src2    */
    INST_POW,   /* dst = src1 ^ src2    */
    INST_DIV,   /* dst = src1 / src2    */
    INST_MOD,   /* dst = src1 % src2    */
    INST_SUB,   /* dst = src1 - src2    */
    INST_CONC,  /* dst = src1 .. src2   */
    INST_CMPL,  /* dst = src1 < src2    */
    INST_CMPLE, /* dst = src1 <= src2   */
    INST_CMPG,  /* dst = src1 > src2    */
    INST_CMPGE, /* dst = src1 >= src2   */
    INST_CMPE,  /* dst = src1 == src2   */
    INST_CMPNE, /* dst = src1 ~= src2   */
    /*
     * Logicke operatory
     */
    INST_AND,   /* dst = src1 and src2  */
    INST_OR,    /* dst = src1 or src2   */
    INST_NOT,   /* dst = not src1       */
    /*
     * Unarne
     */
    INST_CLR,   /* dst = nil vynuluje premennu - nastavy ju na typ nil */
    INST_INC,   /* dst = dst + 1        */
    INST_JMP,   /* jump to dst, navratova hodnota je ulozena v src1 */
    INST_JNIF,  /* jump to dst if src1 ~= true */
    INST_JIF,   /* jump to dst if src1 == true */
    INST_MOV,   /* dst = src1 (kopirovanie odkazom)  */
    INST_CPY,   /* dst = src1 (kopirovanie hodnotou) */
    /*
     * Instrukcia volana pri volani funkcie - uchovanie premennych a vytvorenie
     * aktivacneho zaznamu na stacku. dst je Thtable_item (funkcia z ktorej sa
     * vola podpogram).
     */
    INST_ENTER,
    /*
     * Prazdna instrukcia - nevykona sa nic.
     */
    INST_NOP,
    /*
     * Instrukcia ulozi navratovu adresu z podprogramu (prva vykonavana
     * instrukcia pri navrate z podpogramu) a vygeneruje skok do podpogramu.
     * Instrukcia naviac vynuluje navratovu adresu volanej funkcie.
     * dst - funkcia kam instrukcia vygeneruje skok (prva instrukcia podprogramu.
     */
    INST_CALL,
    /*
     * Instrukcia volana pri navratu z funkcie - obnovenie obsahu premennych a
     * odstranenie aktivacneho zaznamu zo stacku. Instrukcia bez parametrov.
     */
    INST_LEAVE,
    /*
     * I/O operacie a vstavane funkcie.
     */
    INST_READ,  /* dst = read(src1)     */
    INST_WRITE, /* write(src1)          */
    INST_TYPE,  /* dst = type(src1)     */
    INST_SUBSTR,/* dst = substr(dst, src1, src2)   */
    INST_STRLEN,/* dst = #src1          */
    INST_FIND,  /* dst = find(src1)     */
    INST_SORT,  /* dst = sort(src1)     */
};

/* forward */
union inst_operand;
struct Thtable_item;
struct Tvariable;

union inst_operand {
    struct Thtable_item * tab;
    struct Tinstruction * inst;
    struct Tvariable * var;
};

/*
 * Instrukcia a operandy instrukcie.
 */
typedef struct Tinstruction {
    enum inst_type opcode;
    union inst_operand src1;
    union inst_operand src2;
    union inst_operand dst;

    struct Tinstruction * next;
    struct Tinstruction * prev;
} Tinstruction;

/*
 * Hlavicka linearneho zoznamu instrukcii.
 */
typedef struct Tistruction_list {
    Tinstruction * first;
    Tinstruction * last;
    Tinstruction * active;
} Tinstruction_list;

void inst_list_init(Tinstruction_list * list);
Tinstruction * inst_list_first(Tinstruction_list * list);
Tinstruction * inst_list_succ(Tinstruction_list * list);
Tinstruction * inst_create(enum inst_type opcode,
                           union inst_operand dst,
                           union inst_operand src1,
                           union inst_operand src2);
Tinstruction * inst_alloc(void);
void inst_fill(Tinstruction * inst,
               enum inst_type opcode,
               union inst_operand dst,
               union inst_operand src1,
               union inst_operand src2);
void inst_append(Tinstruction_list * to, Tinstruction * inst);
void inst_last(Tinstruction_list * list);
void inst_list_postinsert(Tinstruction_list * list, Tinstruction * inst);
void inst_list_destroy(Tinstruction_list * list);
void inst_delete(Tinstruction_list * list, Tinstruction * inst);

#endif /* IFJ11_INSTRUCTION_H_ */

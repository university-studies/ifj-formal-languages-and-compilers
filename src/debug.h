/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       debug.h
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

#ifndef IFJ11_DEBUG_H_
#define IFJ11_DEBUG_H_

#ifdef DEBUG

/* forward */
struct Ttoken;
struct Tinstruction;
struct Tvariable;
struct Thtable;

void DEBUG_START(void);
void DEBUG_MSG(const char * format, ...);
void DEBUG_END(void);
void DEBUG_PRINT_TOKEN(const struct Ttoken * token);
void DEBUG_PRINT_INSTRUCTION(const struct Tinstruction * inst);
void DEBUG_PRINT_VARIABLE(const struct Tvariable * var);
void DEBUG_PPARSER(void);
void DEBUG_PSTACK_PRINT_ALL(void);
void DEBUG_PRINT_HTABLE(const struct Thtable * htable);

#else

#define DEBUG_START()               ((void) 0)
#define DEBUG_MSG(...)              ((void) 0)
#define DEBUG_END()                 ((void) 0)
#define DEBUG_PRINT_TOKEN(T)        ((void) 0)
#define DEBUG_PRINT_INSTRUCTION(I)  ((void) 0)
#define DEBUG_PRINT_VARIABLE(V)     ((void) 0)
#define DEBUG_PPARSER()             ((void) 0)
#define DEBUG_PSTACK_PRINT_ALL      ((void) 0)
#define DEBUG_PRINT_HTABLE(H)       ((void) 0)

#endif /* DEBUG */

#endif /* IFJ11_DEBUG_H_ */

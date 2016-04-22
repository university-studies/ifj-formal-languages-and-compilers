/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       interpret.h
 * Autor(i):    Fridolin Pokorny
 *
 * Implementacia interpretru.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#ifndef IFJ11_INTERPRET_H_
#define IFJ11_INTERPRET_H_

#include <stdbool.h>

struct Tinstruction; /* forward */

bool interpret_run(struct Tinstruction * inst);

#endif /* IFJ11_INTEPRET_H_ */

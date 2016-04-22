/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       pparser.h
 * Autor(i):    Pavol Loffay
 *
 * Implementacia precedencnej analyzi na spracovavanie vyrazov.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#ifndef IFJ11_PPARSER_H_
#define IFJ11_PPARSER_H_

#include "ial.h"
#include "scanner.h"
#include "variable.h"
#include "stdbool.h"
#include "pstack.h"

Tvariable * pparser(Thtable * htable, Thtable_item * ptr_func,
                    Ttoken * ret_token, bool write);

#endif /* IFJ11_PPARSER_H_ */

/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       ifj11.h
 * Autor(i):    Fridolin Pokorny
 *
 * Hlavny hlavickovy subor pre projekt ifj11.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#ifndef IFJ11_IFJ11_H_
#define IFJ11_IFJ11_H_

#include <stdbool.h>

/**
 * Navratova hodnota v zavislosti na chybe.
 */
enum ifj11_rt {
  RET_OK,
  RET_ERR_LEX,      /* lexikalna chyba   */
  RET_ERR_SYN,      /* syntakticka chyba */
  RET_ERR_SEM,      /* semanticka chyba  */
  RET_ERR_INT,      /* chyba interpretace*/
  RET_ERR_MY        /* interna chyba     */
};

typedef enum ifj11_rt ifj11_rt;

bool set_error(enum ifj11_rt err);
ifj11_rt get_error(void);

#endif /* IFJ11_IFJ11_H_ */

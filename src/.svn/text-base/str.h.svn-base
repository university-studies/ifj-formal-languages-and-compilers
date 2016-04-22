/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       str.h
 *
 * Funkcie na pracu s retazcami.
 *
 * - Tento subor pochadza zo vzoroveho projektu,
 *   ulozeneho na strankach predmetu IFJ.
 *
 * - Autori tohto projektu niesu autormi tohto suboru.
 *
 * - V subore boli spravene drobne zmeny aby mal
 *   subor podobnu strukturu ako otatna cast projektu.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#include <stdbool.h>

#ifndef IFJ11_STR_H_
#define IFJ11_STR_H_

/**
 * Struktura pre string
 */
typedef struct string
{
  char* str;        // misto pro dany retezec ukonceny znakem '\0'
  int length;       // skutecna delka retezce
  int alloc_size;    // velikost alokovane pameti
} Tstring;

bool str_init(Tstring *s);
void str_free(Tstring *s);

void str_clear(Tstring *s);
bool str_add_char(Tstring *s1, char c);
bool str_copy_string(Tstring *s1, Tstring *s2);
int str_cmp_string(Tstring *s1, Tstring *s2);
int str_cmp_const_str(Tstring *s1, char *s2);

char *str_get_str(Tstring *s);
int str_get_length(Tstring *s);

#endif /* IFJ11_STR_H_ */

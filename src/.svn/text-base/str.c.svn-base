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

#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include "xwrappers.h"
#include "str.h"

#define STR_LEN_INC 8
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla

/**
 * inicializuje strukturu retazca
 * @param - argument struktura Tstring
 * @return - navratova hodnota, false pri chybe
 */
bool str_init(Tstring *s)
{
    s->str = xmalloc(STR_LEN_INC);
    s->str[0] = '\0';
    s->length = 0;
    s->alloc_size = STR_LEN_INC;
    return true;
}

/**
 * uvolni z pameti strukturu retazca
 * @param - argument struktura Tstring
 * @return - void
 */
void str_free(Tstring *s)
{
    xfree(s->str);
}

/**
 * vymaze obsah retazca
 * @param - argument struktura Tstring
 * @return - void
 */
void str_clear(Tstring *s)
{
    s->str[0] = '\0';
    s->length = 0;
}

/**
 * prida na koniec retazca jeden znak
 * @param - argument struktura Tstring
 * @param - argument znak ktory sa prida
 * @return - navratova hodnota, false pri chybe
 */
bool str_add_char(Tstring *s1, char c)
{
    if (s1->length + 1 >= s1->alloc_size)
    {
        /* pamet nestaci, je potreba provest realokaci */
        if ((s1->str = (char*) realloc(s1->str, s1->length + STR_LEN_INC)) ==
            NULL)
            return false;
        s1->alloc_size = s1->length + STR_LEN_INC;
    }
    s1->str[s1->length] = c;
    s1->length++;
    s1->str[s1->length] = '\0';
    return true;
}

/**
 * prekopiruje retazec s2 do s1
 * @param - argument struktura Tstring retazca s1
 * @param - argument struktura Tstring retazca s2
 * @return - navratova hodnota, false pri chybe
 */
bool str_copy_string(Tstring *s1, Tstring *s2)
{
    int new_length = s2->length;
    if (new_length >= s1->alloc_size)
    {
        /* pamet nestaci, je potreba provest realokaci */
        if ((s1->str = (char*) realloc(s1->str, new_length + 1)) == NULL)
            return false;
        s1->alloc_size = new_length + 1;
    }
    strcpy(s1->str, s2->str);
    s1->length = new_length;
    return true;
}

/**
 * porovna retazce a vrati vysledok
 * @param - argument struktura Tstring retazca s1
 * @param - argument struktura Tstring retazca s2
 * @return - navratova hodnota, rovnako ako strcmp
 */
int str_cmp_string(Tstring *s1, Tstring *s2)
{
    return strcmp(s1->str, s2->str);
}

/**
 * porovna retazec s konstantnym retazcom
 * @param - argument struktura Tstring retazca
 * @param - argument konstantny retazec
 * @return - navratova hodnota, rovnako ako strcmp
 */
int str_cmp_const_str(Tstring *s1, char* s2)
{
    return strcmp(s1->str, s2);
}

/**
 * vrati textovu cast retazca
 * @param - argument struktura Tstring retazca
 * @return - navratova hodnota, char *
 */
char *str_get_str(Tstring *s)
{
    return s->str;
}

/**
 * vrati dlzku retazca
 * @param - argument struktura Tstring retazca s1
 * @return - navratova hodnota, dlzka
 */
int str_get_length(Tstring *s)
{
    return s->length;
}

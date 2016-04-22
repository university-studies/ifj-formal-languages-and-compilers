/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       ial.h
 * Autor(i):    Fridolin Pokorny    - sort()
 *              Pavol Loffay        - substr()
 *              Pavol Loffay        - find_boyer_moore()
 *
 * - implementacia vyhladania podretazca v retazci;
 * - implementacia radeni;
 * - implementacia tabulky symbolov;
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#ifndef IFJ11_IAL_H_
#define IFJ11_IAL_H_

#include <stdbool.h>
#define ASCII 255

#include "stack.h"
#include "instruction.h"

void ial_sort(char * str);
char * ial_substr(char * str, double from, double to);
long int ial_find_boyer_moore(char * source, char * needle);

/*
 * Polozka tabulky symbolov.
 */
typedef struct Thtable_item
{
    // nazov funkcie
    char * key;
    //pocet parametrov
    int param_number;
    // ukazatel na zoznam instrukcii
    Tinstruction_list inst;
    // ukazatel na zoznam premennych pouzivanych vo funkcii
    struct Tvariable_list var;
    // navratova hodnota pri ukonceni interpretacii funkcie
    struct Tvariable ret_value;

    // zretazeny zoznam
    struct Thtable_item * next;
} Thtable_item;

/*
 * Struktura hash tabulky symbolov s velkostou a polom ukazatelov na polozky.
 */
typedef struct Thtable
{
    unsigned int htable_size;
    Thtable_item * uk[];
} Thtable;

unsigned int ial_hash_function(const char * key, unsigned htable_size);
void ial_htable_insert_var(Tvariable * var, Thtable_item * func);
Thtable * ial_htable_init(unsigned int htable_size);
void ial_htable_delete(Thtable * htptr);
Thtable_item * ial_htable_insert(Thtable * htptr, char * name);
void ial_htable_inc_param_count(Thtable_item * func);
Tinstruction * ial_htable_inst_first(Thtable_item * func);
Tinstruction * ial_htable_inst_succ(Thtable_item * func);
Tvariable * ial_htable_var_first(Thtable_item * func);
Tvariable * ial_htable_var_succ(Thtable_item * func);
Thtable_item * ial_htable_look_func(const Thtable * htable, const char * name);
Tvariable * ial_htable_look_var(const Thtable_item * htable, const char * name);

#endif /* IFJ11_IAL_H_ */

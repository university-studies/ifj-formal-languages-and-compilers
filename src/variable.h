/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       variable.h
 * Autor(i):    Fridolin Pokorny
 *
 * Implementacia zoznamu premennych a zakladnymi operaciami nad nimi.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#ifndef IFJ11_VARIABLE_H_
#define IFJ11_VARIABLE_H_

#include <stdbool.h>

enum var_type {
    VAR_NUM,
    VAR_STR,
    VAR_BOOL,
    VAR_NIL,
};

typedef struct Tvariable {
    enum var_type type;
    char * name;

    union variable_val {
        double num;
        char * str;
        bool boolean;
    } val;

    struct Tvariable * next;
    struct Tvariable * prev;
} Tvariable;

typedef struct Tvariable_list {
    Tvariable * first;
    Tvariable * last;
    Tvariable * active;
} Tvariable_list;


Tvariable *
variable_create(char * name, enum var_type type, union variable_val);
void variable_delete(Tvariable_list * list, Tvariable * var);
char * variable_create_random_name(void);
char * variable_alloc_concat_space(char * s1, char * s2);
void variable_list_init(Tvariable_list * list);
void variable_copy(Tvariable * dst, const Tvariable * src);
Tvariable * variable_list_look(const Tvariable_list * list, const char * name);
void variable_list_append(Tvariable_list * list, Tvariable * var);
Tvariable * variable_list_first(Tvariable_list * list);
Tvariable * variable_list_succ(Tvariable_list * list);
void variable_list_copy(Tvariable_list * dst, const Tvariable_list * src);
void variable_list_assign_del(Tvariable_list * dst, const Tvariable_list * src);
void variable_list_destroy(Tvariable_list * list);

#endif /* IFJ11_VARIABLE_H_ */

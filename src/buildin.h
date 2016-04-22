/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       buildin.h
 * Autor(i):    Fridolin Pokorny
 *
 * Implementacia vstavanych funkcii interpretra.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#ifndef IFJ11_BUILDIN_H_
#define IFJ11_BUILDIN_H_

#include <stdbool.h>

struct Tvariable; /* forward */

bool buildin_type(struct Tvariable * dst, const struct Tvariable * item);
bool buildin_conc(struct Tvariable * dst,
                  const struct Tvariable * var_str1,
                  const struct Tvariable * var_str2);
bool buildin_read(struct Tvariable * dst, const struct Tvariable * item);
bool buildin_write(const struct Tvariable * item);


bool buildin_substr(struct Tvariable * src1,
                    const struct Tvariable * src2,
                    const struct Tvariable * src3);
bool buildin_find(struct Tvariable * dst,
                  const struct Tvariable * str,
                  const struct Tvariable * pattern);
bool buildin_sort(struct Tvariable * dst, const struct Tvariable * str);

#endif /* IF11_BUILDIN_H_ */

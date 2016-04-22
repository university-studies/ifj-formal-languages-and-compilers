/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       fbuffer.h
 * Autor(i):    Fridolin Pokorny
 *
 * Implementacia cyklickeho bufferu nad suborom.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#ifndef IFJ11_FBUFFER_H_
#define IFJ11_FBUFFER_H_

#include <stdbool.h>

enum fbuffer_command
{
    FBUF_GET,
    FBUF_UNGET,
};

bool fbuffer_init(const char * fname);
bool fbuffer_del(void);
int fbuffer(enum fbuffer_command cmd);

#endif /* IFJ11_FBUFFER_H_ */

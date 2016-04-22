/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       fbuffer.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "fbuffer.h"
#include "debug.h"
#include "ifj11.h"
#include "xwrappers.h"

#define FBUFFER_SIZE     8
#define FBUFFER_MOD      0x7

/*
 * Vrati true, ak je buffer plny. Buffer je plny, ak potencionalne nasledovny
 * nacitany znak by prepisal najstarsi pamatovany.
 */
#define FBUFFER_FULL(b)     (! (((b->up + 1) & FBUFFER_MOD) ^ b->down))

/*
 * Vrati true, ak sa momentalne nachadzame niekde uprostred bufferu.
 */
#define FBUFFER_MARK(b)     (b->mark ^ b->up)

/*
 * Struktura popisujuca cyklicky buffer nad suborom f.
 */
typedef struct fbuffer_pool
{
    FILE * f;                   /* FILE s ktorym sa pracuje             */
    const char * fname;         /* Nazov suboru s ktorym buffer pracuje */
    int buf[FBUFFER_SIZE];      /* Samotny cyklicky buffer              */
    int up;                     /* Horne ukazovatko do bufferu          */
    int down;                   /* Dolne ukazovatko do bufferu          */
    int mark;                   /* Aktualna pozicia v befferu           */
} Tfbuffer;

/*
 * Alokovany buffer
 */
static Tfbuffer * b = NULL;

/**
 * Inicializuje cyklicky buffer nad suborom.
 * @param - nazov suboru z argv
 * @return - false v pripade chyby nad suborom
 * Funkcia meni premennu ifj11_error.
 */
bool fbuffer_init(const char * fname)
{
    assert(fname);
    /*
     * Funkcia moze byt volana prave raz!
     */
    assert(! b);

    b = xmalloc(sizeof(Tfbuffer));

    b->fname = fname;
    b->up = 0;
    b->down = 0;
    b->mark = 0;
    b->f = fopen(fname, "r");
    if (! b->f)
    {
        perror(fname);
        return set_error(RET_ERR_MY);
    }

    DEBUG_MSG("Zdrojovy subor '%s' bol otvoreny.\n", fname);

    return true;
}

/*
 * Ukonci pracu so suborom.
 * @param - void
 * @return - v pripade neuspechu zavretia suboru false
 * Funkcia meni premennu ifj11_error.
 */
bool fbuffer_del(void)
{
    if (fclose(b->f) == EOF)
    {
        perror(b->fname);
        return set_error(RET_ERR_MY);
    }

    DEBUG_MSG("Zdrojovy subor '%s' bol zatvoreny.\n", b->fname);

    free(b);
    b = NULL;

    return true;
}

/*
 * Umoznuje prechadzat v subore spat s obmedzenym poctom krokov (FBUFFER_SIZE).
 * @param - prikaz: FBUF_GET - nacitanie dalsieho znaku
 *                  FBUF_UNGET - vratenie nacitaneho znaku
 * @return - nacitany alebo vrateny znak
 */
int fbuffer(enum fbuffer_command cmd)
{
    int rv = 0;
    switch (cmd)
    {
        case FBUF_GET:
            if (FBUFFER_FULL(b))
            {
                /*
                 * Pokial je buffer plny, je nutne zahodit najstarsi pamatovany
                 * znak.
                 */
                b->down = (b->down + 1) & FBUFFER_MOD;
                rv = fgetc(b->f);
                b->buf[b->up] = rv;
                b->up = (b->up + 1) & FBUFFER_MOD;
                b->mark = (b->mark + 1) & FBUFFER_MOD;
            }
            else if (FBUFFER_MARK(b))
            {
                /*
                 * Bolo prevedene FBUF_UNGET. Caka sa na do stavu, kedy bude
                 * mark ukazovat na rovnaku poziciu ako b->up (vsetky vratene
                 * znaky budu opat precitane.
                 */
                rv = b->buf[b->up];
                b->up = (b->up + 1) & FBUFFER_MOD;
            }
            else
            {
                /*
                 * Buffer je v stave, ked sa doplna. Nebolo prevedene FBUF_UNGET
                 * ani nie je plny.
                 */
                rv = fgetc(b->f);
                b->buf[b->up] = rv;
                b->up = (b->up + 1) & FBUFFER_MOD;
                b->mark = (b->mark + 1) & FBUFFER_MOD;
            }

            break;

        case FBUF_UNGET:
            /*
             * V pripade unget sa pohybuje len ukazovatkom up. mark poznamenava
             * predoslu poziciu (pred prvym FBUF_UNGET).
             */
            b->up = (b->up + FBUFFER_SIZE - 1) & FBUFFER_MOD;
            rv = b->buf[b->up];
            break;
        default:
            assert(0);
    }

    return rv;
}


/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       main.c
 * Autor(i):    Fridolin Pokorny
 *
 * Subor s main().
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ifj11.h"
#include "debug.h"
#include "parser.h"
#include "scanner.h"
#include "str.h"
#include "fbuffer.h"
#include "ial.h"
#include "pstack.h"
#include "pparser.h"

/**
 * Aktualny chybovy stav interpretru - priznak chyby.
 */
static ifj11_rt ifj11_error = RET_OK;

/**
 * Zobrazi textovu napovedu.
 * @param - nazov programu
 * @return - void
 */
static void print_help(const char * pname)
{
    static const char * msg =
        "ifj11 - Lua like interpreter\n"
        "Usage: %s FILE\n"
        "Authors:\n"
        "\tMiroslav Lisik   - xlisik00\n"
        "\tPavol Loffay     - xloffa00\n"
        "\tDusan Madarka    - xmadar01\n"
        "\tFridolin Pokorny - xpokor32\n";

    printf(msg, pname);
}

/**
 * Funkcia kontroluje, ci uz nie je nastaveny priznak chyby, aby neprepisala
 * iny typ chyby (RET_ERR_LEX a RET_ERR_SEM).
 * Ak nie je, nastavi ho, inak nechava povodny.
 * Vracia stale false, lebo uz doslo k chybe.
 * @param - chyba, ktora sa ma nastavit
 * @return - stale false
 */
bool set_error(enum ifj11_rt err)
{
    ifj11_error = ifj11_error == RET_OK ? err : ifj11_error;
    return false;
}

/**
 * Funkcia vrati hodnotu premennej ifj11_error.
 * @param - void
 * @return - hodnota premennej ifj11_error.
 */
ifj11_rt get_error(void)
{
    return ifj11_error;
}

/**
 * main.c
 * @param - argument count
 * @param - argument vector
 * @return - ifj11_rt: RETURN_OK pri ziadnej chybe
 *
 */
int main(int argc, char *argv[])
{
    DEBUG_START();
    DEBUG_MSG("Debug spusteny.\n");

    if (argc == 2)
    {
        if (! fbuffer_init(argv[1]))
        {
            DEBUG_END();
            return RET_ERR_MY;
        }

#ifdef PPARSER_DEBUG
        DEBUG_PPARSER();
#endif /* PPARSER_DEBUG */

        syntax_directed_translation();

        fbuffer_del();
    }
    else
    {
        print_help(argv[0]);
        DEBUG_END();
        return RET_ERR_MY;
    }

    DEBUG_END();

    return ifj11_error;
}


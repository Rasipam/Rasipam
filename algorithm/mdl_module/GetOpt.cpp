#include "Common.h"
#include "GetOpt.h"

/* global variables that are specified as exported by get_opt() */
char *poptarg = nullptr;    /* pointer to the start of the option argument  */
int poptind = 1;       /* number of the next argv[] to be evaluated    */
int popterr = 1;       /* non-zero if a question mark should be returned
                          * when a non-valid option character is detected */

/* handle possible future character set concerns by putting this in a macro */
#define _next_char(string)  (char)(*(string+1))

int get_opt(int argc, char **argv, char *optstring) {
    static char *index_position = nullptr; /* place inside current argv string */
    char *arg_string = nullptr;        /* where to start from next */
    char *opt_string;               /* the string in our program */


    if (index_position != nullptr) {
        /* we last left off inside an argv string */
        if (*(++index_position)) {
            /* there is more to come in the most recent argv */
            arg_string = index_position;
        }
    }

    if (arg_string == nullptr) {
        /* we didn't leave off in the middle of an argv string */
        if (poptind >= argc) {
            /* more command-line arguments than the argument count */
            index_position = nullptr;  /* not in the middle of anything */
            return EOF;             /* used up all command-line arguments */
        }

        /*---------------------------------------------------------------------
         * If the next argv[] is not an option, there can be no more options.
         *-------------------------------------------------------------------*/
        arg_string = argv[poptind++]; /* set this to the next argument ptr */

        if (('/' != *arg_string) && /* doesn't start with a slash or a dash? */
            ('-' != *arg_string)) {
            --poptind;               /* point to current arg once we're done */
            poptarg = nullptr;          /* no argument follows the option */
            index_position = nullptr;  /* not in the middle of anything */
            return EOF;             /* used up all the command-line flags */
        }

        /* check for special end-of-flags markers */
        if ((strcmp(arg_string, "-") == 0) ||
            (strcmp(arg_string, "--") == 0)) {
            poptarg = nullptr;          /* no argument follows the option */
            index_position = nullptr;  /* not in the middle of anything */
            return EOF;             /* encountered the special flag */
        }

        arg_string++;               /* look past the / or - */
    }

    if (':' == *arg_string) {       /* is it a colon? */
        /*---------------------------------------------------------------------
         * Rare case: if opterr is non-zero, return a question mark;
         * otherwise, just return the colon we're on.
         *-------------------------------------------------------------------*/
        return (popterr ? (int) '?' : (int) ':');
    } else if ((opt_string = strchr(optstring, *arg_string)) == nullptr) {
        /*---------------------------------------------------------------------
         * The letter on the command-line wasn't any good.
         *-------------------------------------------------------------------*/
        poptarg = nullptr;              /* no argument follows the option */
        index_position = nullptr;      /* not in the middle of anything */
        return (popterr ? (int) '?' : (int) *arg_string);
    } else {
        /*---------------------------------------------------------------------
         * The letter on the command-line matches one we expect to see
         *-------------------------------------------------------------------*/
        if (':' == _next_char(opt_string)) { /* is the next letter a colon? */
            /* It is a colon.  Look for an argument string. */
            if ('\0' != _next_char(arg_string)) {  /* argument in this argv? */
                poptarg = &arg_string[1];   /* Yes, it is */
            } else {
                /*-------------------------------------------------------------
                 * The argument string must be in the next argv.
                 * But, what if there is none (bad input from the user)?
                 * In that case, return the letter, and poptarg as nullptr.
                 *-----------------------------------------------------------*/
                if (poptind < argc)
                    poptarg = argv[poptind++];
                else {
                    poptarg = nullptr;
                    return (popterr ? (int) '?' : (int) *arg_string);
                }
            }
            index_position = nullptr;  /* not in the middle of anything */
        } else {
            /* it's not a colon, so just return the letter */
            poptarg = nullptr;          /* no argument follows the option */
            index_position = arg_string;    /* point to the letter we're on */
        }
        return (int) *arg_string;    /* return the letter that matched */
    }
}



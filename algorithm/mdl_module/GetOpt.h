#ifndef GETOPT_H
#define GETOPT_H

extern char *poptarg;  /* carries the optional argument when a command line
							* arg is specified with a ':' after it in the optstring
							* and is usually handled by the caller in a switch()
							* block. */
extern int poptind;    /* The caller should not need to adjust this normally */
extern int popterr;  /* The get_opt() function returns a question mark (?)
							* when it encounters an option character not included in 
									* optstring.  This error message can be disabled by 
							* setting popterr to zero.  Otherwise, it returns the 
							* option character that was detected. */

int get_opt(int argc, char **argv, char *optstring);


#endif


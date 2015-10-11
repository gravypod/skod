/* utils.h - just utils.h */

#ifndef UTILS_H
#define UTILS_H

#include "skod.h"

/* Give some colors to the output */
#define NRM  "\x1B[0m"
#define RED  "\x1B[31m"
#define GREEN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLUE  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"
#define END "\033[0m"

/* Handle data */
typedef struct {
	float bytes;
	int bytes_postfix;
} data_t;

/* prototypes */
int util_file_exists(char*);
void calc_bytes(data_t *, float);
void print(int mode,char*, ...);
void die(char*, ...);
void signal_handler(int);
void chkbuffer(char*);

#endif /*UTILS_H*/

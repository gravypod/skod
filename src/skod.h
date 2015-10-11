/* skod.h - headers and global stuff */

#ifndef SKOD_H
#define SKOD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/stat.h>

#define MAX_STR	256
#define FD_MODE	"r+"

#include "utils.h"
#include "tcp.h"
#include "ftp.h"

char ip[MAX_STR];
enum { False=0, True=1 };

/* fot socket */
int fd;
int dfd;

/* Flags */
int flag;
int ipv4;
int ipv6;

/* types */
typedef struct {
	char *user;
	char *password;
	char *server;
	char *port;
	char *path;
	char *os;
	char *prod;
	char *dest;
} skod_t;

/* prototypes */
unsigned long skod_checksum(char*);
void skod_detect_ip(void);

#endif /*skod_H*/

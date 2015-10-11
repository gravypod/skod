/* ftp.h - RFC 959 */

#ifndef FTP_H
#define FTP_H

#include "skod.h"

#define FTP_PORT "21"
#define FTP_USER	"anonymous"
#define FTP_PASSWORD	"anonymous"
#define FTP_SERVER	"localhost"

typedef struct {
	FILE *FD;
	int code;
	int dataport;
	unsigned int alarm_sec;
	int logged;
	char *user;
	char *password;
	char *server;
	char *port;
} ftp_t;

/* prototypes */
char * ftp_getline(ftp_t*);
char * ftp_pwd(ftp_t *ftp);
int ftp_banner(ftp_t*);
int ftp_login(ftp_t*);
int ftp_getdataport(ftp_t*);
int ftp_size(ftp_t*, char*);
void ftp_list(ftp_t*, char*,int);
void ftp_remove(ftp_t*, char*);
void ftp_mkcon(ftp_t *);
void ftp_close(ftp_t*);
void ftp_cat(ftp_t*, char*);
void ftp_download_single(ftp_t*, char*);
void ftp_upload_single(ftp_t*, char*);
void ftp_delete(ftp_t*, char*);
void ftp_cwd(ftp_t*, char*);
void ftp_mdtm(ftp_t*, char*);

#endif /*FTP_H*/

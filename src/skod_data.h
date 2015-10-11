/* skod_data.h - This file conatin the skod DATA for multiple tasks*/

#define NIX	"*nix"
#define NT	"Windows"

/* FTP prod */
#define SKOD_PROD_UNKNOWN "unknown"

/* OS */
#define SKOD_OS_NIX	"Linux" /* *NIX 8 */
#define SKOD_OS_NT	"Windows NT"
#define SKOD_OS_UNKNOWN "unknown"

/* for The fingerprints */
typedef struct {
	char *prod;
	char *print;
} hcf_t;

const char *ftp_commands[] = {
	"NOTEXISTS", /* This is just to see how to server will respone to unvaild command. */
	"PASS", /* Just to see whats the output. */
	"ALLO", 
	"USER", /* This changes from server to server. */
};

const hcf_t hcf_fingerprints[] = {
	{"inetutils (FTPD)", "3295:1948:1933:1956"},
	{"vsFTPD",  "1769:1861:2088:2859"}, /* Logged*/
	{"vsFTPD",  "1769:1769:2088:1769"}, 
	{"vsFTPD 3.0.2+",  "2933:2169:2933:2909"}, 
	{"ProFTPD", "2421:2123:2887:3382"},
	{"ProFTPD", "2421:2543:3280:2543"},
	{"PureFTPD", "2038:2607:673:2665"},
	{"PureFTPD", "2038:2607:858:2665"},
	{"PureFTPD", "1723:2370:2006:2557"}, /* Logged */
	{"Microsoft FTPD", "3402:2169:2436:3483"},
};

const char *ftp_prod[] = {
	"ProFTPD",
	"vsFTPD",
	"PureFTPD",
};

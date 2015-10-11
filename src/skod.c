/*   skod.c -   File transfer protocol client for humans.            
*		    
* Copyright (c) 2015 by Hypsurus <hypsurus@mail.ru>     
*
* skod is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* skod is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/


#include "skod.h"
#include "skod_data.h"

void skod_init(skod_t *skod) {
	skod->user = FTP_USER;
	skod->password = FTP_PASSWORD;
	skod->port = FTP_PORT;
	skod->server = NULL;
	skod->os = SKOD_OS_UNKNOWN;
	skod->prod = SKOD_PROD_UNKNOWN;
	skod->dest = NULL;
}

void skod_usage(char *arg) {
	printf("%sUsage: %s -s (server) [OPTIONS] ...\n"
		"\nOPTIONS:\n"
		"\t--server      -s : FTP server host.\n"
		"\t--user        -u : FTP username (anonymous).\n"
		"\t--password    -p : FTP password. (anonymous)\n"
		"\t--port        -P : FTP server port (21).\n"
		"\t--ls          -l : List directory contents.\n"
		"\t--rm          -r : Remove files/direcotrys.\n"
		"\t--cat         -c : Print files.\n"
		"\t--pwd         -c : Print current working directory.\n"
		"\t--download    -d : Download files/folders.\n"
		"\t--upload      -U : Upload files/folders.\n"
		"\t--dest        -e : Destination folder (for --upload).\n"
		"\t--size        -z : Get file size.\n"
		"\t--delete      -D : Delete files/folders.\n"
		"\t--mdtm        -m : Return the modification time of a file.\n"
		"\t--scan        -S : Get information about the server.\n"
		"\t--help        -h : show help.\n"
		"\t--version     -v : show version.\n"
		"%s\n%sskod - File Transfer Protocol client for humans.\n"
		"Full documentation at: <https://github.com/Hypsurus/skod>%s\n",WHT,arg, END,YEL,END);
		exit(0);
}

/* Parse OS/STAT*/
void skod_parse_stat(skod_t *skod, ftp_t *ftp) {
	char *line = NULL;
	char buffer[MAX_STR];
	const char **p = NULL;

	if ( ftp->logged ) {
		fprintf(ftp->FD, "STAT\r\n");
		while (( fgets(buffer, sizeof(buffer), ftp->FD)) != NULL ) {
			for ( p = ftp_prod; *p;p++ ) {
				if ( strstr(buffer, *p))
					print(3, "%s", buffer);
			}
		}
				print(0, " ");
	}
	ftp_close(ftp);
	ftp_mkcon(ftp);

	/* Parse OS */
	fprintf(ftp->FD, "SYST\r\n");
	line = ftp_getline(ftp);
	if (( strstr(line, "UNIX")))
		skod->os = SKOD_OS_NIX;
	if (( strstr(line, "NT")))
		skod->os = SKOD_OS_NT;
}

/** FINGERPRINT **/
unsigned long skod_checksum(char *str) {
	unsigned long sum = 0UL;

	if (!str)
		return 0;
	while ( *str != 0 ) {
		sum += (unsigned char)*str++;
	}
	return sum;
}

char * skod_hc_fingerprint(ftp_t *ftp) {
	char *line = NULL;
	const char **ptr = NULL;
	unsigned long sum = 0;
	char ssum[MAX_STR];
	static char patr[MAX_STR];
	int size = sizeof(ftp_commands) / sizeof(ftp_commands[0]);
	int c = 1;

	bzero(patr, MAX_STR);
	bzero(ssum, MAX_STR);
		
	ftp_close(ftp);
	/* It's more safe to make connection to each request
	 * This way we will get the fingerprint 100% */
	for ( ptr = ftp_commands; *ptr; ptr++ ) {
		ftp_mkcon(ftp);
		fprintf(ftp->FD, "%s\r\n", *ptr);
		line = ftp_getline(ftp);
		sum = skod_checksum(line);
		sprintf(ssum, "%lu", sum);
		strcat(patr, ssum);
		if ( c == size )
				break;
		strcat(patr,":");
		c++;
		ftp_close(ftp);
		}
	return (char *)patr;
}

/* Parse Hardcore fingerprints 
 * Format: 000:000:000:000:000:000:000 */
void skod_hc_fingerprint_parse(skod_t *skod, ftp_t *ftp, int p) {
	const hcf_t *h = hcf_fingerprints;
	const char *hcf = NULL;
	int hcf_size = sizeof(hcf_fingerprints) / sizeof(hcf_fingerprints[0]);

	hcf = skod_hc_fingerprint(ftp);
	print(0,"HC fingerpirnt: %s%s%s ", RED,hcf,END);
	print(0, " ");
	while ( hcf_size != 0 ) {
		if (( strcmp(hcf, h->print)) == 0 ) {
			skod->prod = h->prod;
			break;
		}
		h++;
		hcf_size--;
	}
}

/* skod scan */
void skod_scan(skod_t *skod, ftp_t *ftp) {
	int v = 0;
	ftp->alarm_sec = 3;

	print(0, "%sStarting skod %s - scanning %s%s%s%s ...", WHT,PACKAGE_VERSION,END, 
						GREEN, ftp->server, END);
	print(0, " ");
	skod_parse_stat(skod, ftp);
	print(0, "%sGenerating fingerprint %s ...", WHT,END);
	skod_hc_fingerprint_parse(skod, ftp, v);
	print(0, "%sHmm%s... %s%s%s server running %s%s%s.",WHT,END,
			GREEN,skod->os,END, 
			YEL,skod->prod,END);
}


/* Destect IP version */ 
void skod_detect_ip(void) {
    struct in_addr addr;
    struct in6_addr addr6;

	if ( inet_pton(AF_INET, ip, &addr))
		ipv4 = 1;
	else if ( inet_pton(AF_INET6, ip, &addr6.s6_addr))
		ipv6 = 1;
	else
		/* Will not happen in vaild host */
		die("Failed to detect %s IP version.", ip);
}

void skod_parse_cla(int argc, char **argv, skod_t *skod) {
	int opt = 0;
	int index = 0;
	static struct option longopt[] = {
    	{"server",   required_argument, 0, 's'},
    	{"scan",     no_argument,       0, 'S'},
    	{"port",     required_argument, 0, 'P'},
    	{"user",     required_argument, 0, 'u'},
    	{"ls",       required_argument, 0, 'l'},
    	{"rm",       required_argument, 0, 'r'},
    	{"cat",      required_argument, 0, 'c'},
    	{"download", required_argument, 0, 'd'},
    	{"upload",   required_argument, 0, 'U'},
    	{"dest",     required_argument, 0, 'e'},
    	{"size",     required_argument, 0, 'z'},
    	{"password", required_argument, 0, 'p'},
		{"delete",   required_argument, 0, 'D'},
		{"mdtm",     required_argument, 0, 'm'},
    	{"pwd",      no_argument,       0, 'w'},
    	{"help",     no_argument,       0, 'h'},
    	{"version",  no_argument,       0, 'v'},
    	};

	while (( opt = getopt_long(argc, argv, "s:SP:u:p:l:r:d:U:c:z:D:we:m:hv", longopt, &index)) != -1 ) {
		chkbuffer(optarg);
		switch(opt) {
			case 's':
				skod->server = optarg;
				break;
			case 'S':
				flag = 99;
				break;
			case 'P':
				skod->port = optarg;
				break;
			case 'u':
				skod->user = optarg;
				break;
			case 'p':
				skod->password = optarg;
				break;
			case 'l':
				skod->path = optarg;
				flag = 1; 
				break;
			case 'r':
				skod->path = optarg;
				flag = 2;
				break;
			case 'd':
				skod->path = optarg;
				flag = 3;
				break;
			case 'U':
				skod->path = optarg;
				flag = 4;
				break;
			case 'c':
				skod->path = optarg;
				flag = 5;
				break;
			case 'z':
				skod->path = optarg;
				flag = 6;
				break;
			case 'w':
				flag = 7;
				break;
			case 'D':
				flag = 8;
				skod->path = optarg;
				break;
			case 'e':
				skod->dest = optarg;
				break;
			case 'm':
				flag = 10;
				skod->path = optarg;
				break;
			case 'h':
				skod_usage(argv[0]);
			default:
				skod_usage(argv[0]);
			}
		}	
	if ( argc < 2 )
		skod_usage(argv[0]);
	if ( skod->server == NULL ) {
		print(1, "Please provide server (-s).");
		skod_usage(argv[0]);
	}
	if ( flag == 0 )
		die("%sPlease specify option (try --help).%s", WHT,END);
}

int main(int argc, char **argv) {
	skod_t skod;
	ftp_t ftp;

	skod_init(&skod);
	skod_parse_cla(argc, argv, &skod);		

	signal(SIGINT, signal_handler);

	/* Init FTP*/
	ftp.user = skod.user;
	ftp.password = skod.password;
	ftp.server = skod.server;
	ftp.port = skod.port;
	ftp.alarm_sec = 1; /* High risk */

	ftp_mkcon(&ftp);

	/* --dest, -e*/
	if ( skod.dest != NULL )
		ftp_cwd(&ftp, skod.dest);
	else if ( skod.dest == NULL && flag == 4 )
		die("You need to pass --dest (destination folder) with --upload.");

	switch(flag) {
		case 1:
			ftp_list(&ftp, skod.path, 1);
			break;
		case 2:
			ftp_remove(&ftp, skod.path);
			break;
		case 3:
			ftp_download_single(&ftp, skod.path);
			break;
		case 4:
			ftp_upload_single(&ftp, skod.path);
			break;
		case 5:
			ftp_cat(&ftp, skod.path);
			break;
		case 6:
			printf("%d\n", ftp_size(&ftp, skod.path));
			break;
		case 7:
			printf("%s\n", ftp_pwd(&ftp));
			break;
		case 8:
			ftp_delete(&ftp, skod.path);
			break;
		case 10:
			ftp_mdtm(&ftp, skod.path);
			break;
		case 99:
			skod_scan(&skod, &ftp);
			break;
	}

	ftp_close(&ftp);
	return 0;
}


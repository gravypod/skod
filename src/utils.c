/* utils.c - utils */

#include "utils.h"

/** usage: print(mode, msg) **/
void print(int mode,char *format, ...) {
		char tmp[MAX_STR];

		va_list li;
		va_start(li, format);
		vsprintf(tmp, format, li);
		va_end(li);
		
		switch(mode) {
				case 0:
						printf("%s::%s %s\n", WHT,END,tmp);
						break;
				case 1:
						printf("%s::%s %sError:%s %s\n", WHT,END,RED,END,tmp);
						break;
				/* debug */
				case 2:
						printf("%s:: DEBUG ::%s%s %s%s", CYN,END,YEL,tmp,END);
						break;
				/* print without new line */
				case 3:
						printf("%s::%s %s", WHT,END,tmp);
						break;
		}
}

void die(char *format, ...) {
		va_list li;
		char tmp[MAX_STR];

		va_start(li,format);
		vsprintf(tmp, format, li);
		printf("%s::%s %sError%s %s%s%s\n", RED,END,WHT,END,YEL,tmp,END);
		va_end(li);
		exit(1);
}
void chkbuffer(char *buffer) {
		int i = 0;
		if ( buffer ) {
				for ( i = 0; buffer[i] != 0; i++) {
						if ( i > MAX_STR-6 )
								die("You cannot pass this string.");
				}
		}

}

int util_file_exists(char *filename) {
		struct stat buffer;
		return (stat(filename, &buffer) == 0);
}

void signal_handler(int sig) {
		if ( sig == SIGALRM ) {
				close(fd);
				fd = -1;
		}
		if ( sig == SIGINT ) {
				print(3, "%ssInterrrupt ... %s\n", YEL,END);
				close(fd);
				exit(1);
		}
}

void calc_bytes(data_t *data, float bytes) {                   
		if ( bytes >= 1024.0 ) {
				data->bytes = bytes / 1024;
				data->bytes_postfix = 0x08;
		}
		if ( bytes >= 1048576.0 ) {
				data->bytes = bytes / 1048576.0;
				data->bytes_postfix = 'M';
		}
		if ( bytes >= 1073741824.0 ) {
				data->bytes = bytes / 1073741824.0;
				data->bytes_postfix = 'G';
		}
}

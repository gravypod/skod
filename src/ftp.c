/* ftp.c - File transfer protocol functions - will try to follow RFC 959 
 *
 * Written by Hypsurus <hypsurus@mail.ru> *
 *
 * See 'LICENSE' for copying 
 *
 */

#include "ftp.h"

/* Mdtm - return the modification time of a file*/
void ftp_mdtm(ftp_t *ftp, char *path) {
	char *line = NULL;
	char act[MAX_STR];

	fprintf(ftp->FD, "MDTM %s\r\n", path);
	line = ftp_getline(ftp);
	ftp->code = atoi(line);
	if ( ftp->code == 550 )
		die("Failed to run MTDM on \'%s\'.", path);
	sprintf(act, "%s", strrchr(line, 0x20)+1);
	print(0, "%s%s%s %c%c:%c%c:%c%c %c%c/%c%c/%c%c%c%c",
			GREEN,path,END,
			act[8], act[9], act[10], act[11], act[12], act[13],
			act[6], act[7],
			act[4], act[5],
			act[0], act[1],act[2], act[3]);
}

/* Change working directory for --dest. 
 * Will work only with --upload */
void ftp_cwd(ftp_t *ftp, char *path) {
	char *line = NULL;

	fprintf(ftp->FD, "CWD %s\r\n", path);
	line = ftp_getline(ftp);
	ftp->code = atoi(line);
	if ( ftp->code == 550 )
		die("Failed to change directory to %s\'%s\'%s.", RED,END);
}

/* delete file/folder  from the server */
void ftp_delete(ftp_t *ftp, char *path) {
	char *line = NULL;

	fprintf(ftp->FD, "DELE %s\r\n", path);
	line = ftp_getline(ftp);
	ftp->code = atoi(line);
	if ( ftp->code == 250 )
		print(0, "File %s\'%s\'%s deleted!", GREEN,path,END);
	else if ( ftp->code ==  550 )
		die("%s\'%s\'%s No such file or directory.", RED,path,END);
}

/* upload single file to the FTP server */
void ftp_upload_single(ftp_t *ftp, char *path) {
	char *line = NULL;
	char *filename = NULL;
	FILE *data;
	FILE *fp;
	char buffer[MAX_STR];
	long int dsize = 0;
	long int rsize = 0;
	long int fsize = 0;
	data_t d1;
	data_t d2;

	/* strrchr return the last string */
	filename = strrchr(path, '/') + 1;

	if (( fp = fopen(path, "r")) == NULL )
		die("Cannot read %s.", path);

	fseek(fp, 0L, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	ftp->dataport = ftp_getdataport(ftp);
	fprintf(ftp->FD, "TYPE I\r\n");
	line = ftp_getline(ftp);
	if ( atoi(line) == 500 )
		die("Failed to set TYPE.");

	fprintf(ftp->FD, "STOR %s\r\n", path);

	/* SIGALRM */
	line = ftp_getline(ftp);
	data = tcp_connect2(ip, ftp->dataport, "w");

	calc_bytes(&d1, fsize);
	while(( rsize = fread(buffer, 1, sizeof(buffer), fp) ) > 0 ) {
		if ( buffer[rsize +1] == '\r' )
			buffer[rsize +1] = '\0';
		dsize += fwrite(buffer, 1, rsize, data);
		calc_bytes(&d2, dsize);
		fprintf(stdout, "%s:: Uploading %s\'%s\'%s %s(%.2f%c) %.2f%c%s\r",
				WHT,GREEN,
				filename,END,
				YEL,
				d1.bytes,
				d1.bytes_postfix,
				d2.bytes,
				d2.bytes_postfix,
				END);

		fflush(stdout);
	}
	putchar(0x0a);

	if ( fsize == dsize )
		print(0, "File %s\'%s\'%s saved on the server.", GREEN,filename,END);
	else
		print(1, "File %s\'%s\'%s is corrupted, try uploading it again?", RED,filename,END);
		
	fclose(fp);
	fclose(data);
	close(dfd);
}

/* download single file from the FTP server */
void ftp_download_single(ftp_t *ftp, char *path) {
	char *line = NULL;
	FILE *data;
	FILE *fp;
	char buffer[MAX_STR];
	char *filename = NULL;
	long int dsize = 0;
	int rsize = 0;
	long int fsize = 0;
	long int lsize = 0;
	data_t d1;
	data_t d2;

	/* In case the URL have more then one / */
	if ( path[strlen(path)-1] == 0x2f )
		path[strlen(path)-1] = 0x00;

	/* strrchr return the last string */
	filename = strrchr(path, '/') + 1;

	/* Check if file exists before downloading */
	if (util_file_exists(filename) == 1 ) {
		print(1, "%s\'%s\' already exists.%s", WHT,filename,END);
		exit(0);
	}

	fsize = (int)ftp_size(ftp, path);
	ftp->dataport = ftp_getdataport(ftp);
	fprintf(ftp->FD, "TYPE I\r\n");
	line = ftp_getline(ftp);
	if ( atoi(line) == 500 )
		die("Failed to set TYPE.");
	fprintf(ftp->FD, "RETR %s\r\n", path);

	/* SIGALRM */
	line = ftp_getline(ftp);
	data = tcp_connect2(ip, ftp->dataport, "r");
		
	if (( fp = fopen(filename, "w")) == NULL )
		die("Cannot create %s.", filename);

	calc_bytes(&d1, fsize);
	while(( rsize = fread(buffer, 1, sizeof(buffer), data) ) > 0 ) {
		if ( buffer[rsize +1] == '\r' )
			buffer[rsize +1] = '\0';
		dsize += fwrite(buffer, 1, rsize, fp);
		calc_bytes(&d2, dsize);
		print(3, "%s Downloading %s\'%s\'%s (%.2f%c) %.2f %c\b\b\b\b\b\r", WHT,GREEN,
				filename,END,
				d1.bytes, d1.bytes_postfix,
				d2.bytes, d2.bytes_postfix);
		fflush(stdout);
	}
	putchar(0x0a);
	fseek(fp, 0L, SEEK_END);
	lsize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
		
	/* All good */
	if ( lsize == fsize )
		print(0, "File %s\'%s\'%s saved.", GREEN,filename,END);
	else {
		/* inetutils <=1.9.4, 
		 * This bug discoverd by me in 05-10-2015,
		 *  SIZE command return file_size+23 bytes*/
		if ( lsize+23 == fsize ) {
			print(0, "File \'%s\' saved.", GREEN,filename,END);
			print(0, "%sYou have unpatched version of inetutils 1.x.x please upgrade.%s", YEL,END);
		}
		/* Bad */
		else
			print(1, "File %s\'%s\'%s is corrupted, try downloading it again?", RED,filename,END);
	}
	fclose(fp);
	fclose(data);
	close(dfd);
}

char * ftp_pwd(ftp_t *ftp) {
	char *ppwd = NULL;
	char *line = NULL;
	static char pwd[MAX_STR];

	fprintf(ftp->FD, "PWD\r\n");
	line = ftp_getline(ftp);
	ppwd = strstr(line, "\"");
	sscanf(ppwd, "\"%s\" %[^\n]s", pwd, line);
	strtok(pwd, "\"");

	return pwd;
}

/* Get file size */
int ftp_size(ftp_t *ftp, char *path) {
	char *line = NULL;
	int file_size = 0;
		
	fprintf(ftp->FD, "SIZE %s\r\n", path);
	line = ftp_getline(ftp);
	if ( atoi(line) != 213 )
		die("Failed to get \'%s\' size.", path);

	sscanf(line, "213 %d", &file_size);
	return (file_size);
}

/* print contents of single file from the FTP server */
void ftp_cat(ftp_t *ftp, char *path) {
	char *line = NULL;
	FILE *data;
	char buffer[MAX_STR];

	ftp->dataport = ftp_getdataport(ftp);
	fprintf(ftp->FD, "TYPE I\r\n");
	line = ftp_getline(ftp);
	if ( atoi(line) == 500 )
		die("Failed to set TYPE.");
	fprintf(ftp->FD, "RETR %s\r\n", path);
	/* SIGALRM */
	ftp_getline(ftp);

	data = tcp_connect2(ip, ftp->dataport, "r");
	while(( fgets(buffer, sizeof(buffer), data)) != NULL ) {
		printf("%s", buffer);
	}
	fclose(data);
	close(dfd);
}

/* Delete file/directory */
void ftp_remove(ftp_t *ftp, char *path) {
	char *line = NULL;
		
	fprintf(ftp->FD, "DELE %s\r\n", path);
	line = ftp_getline(ftp);
	ftp->code = atoi(line);
	if ( ftp->code == 250 )
		print(0, "=> \'%s\' deleted.", path);
	else
		print(1, "=> \'%s\' not removed.", path);
}

/* Get list of files 
 * opt:
 * 1 - NLST
 * 2 - LIST */
void ftp_list(ftp_t *ftp, char *path, int opt) {
	FILE *data;
	char buffer[MAX_STR];

	ftp->dataport = ftp_getdataport(ftp);
		
	switch(opt) {
		case 1:
			fprintf(ftp->FD, "LIST %s\r\n", path);
		case 2:
			fprintf(ftp->FD, "NLST %s\r\n", path);
	}
	/* SIGALRM */
	ftp_getline(ftp);

	data = tcp_connect2(ip, ftp->dataport, "r");

	while ( fgets(buffer, sizeof(buffer), data) != NULL ) {
		/* dir*/
		if ( buffer[0] == 0x64 )
			print(3, "%s%s%s", BLUE,buffer, END);
		/* symlink*/
		if ( buffer[0] == 0x6c )
			print(3, "%s%s%s", CYN,buffer,END);
		/* File */
		else if ( buffer[0] == 0x2d )
			print(3, "%s%s%s", WHT,buffer, END);
	}
	fclose(data);
	close(dfd);
	/* No need to close fd the SIGALRM do it */
}

/* get data port using PASV*/
int ftp_getdataport(ftp_t *ftp) {
	char *line = NULL;
	int p1 = 0;
	int p2 = 0;
	int p3_secure = 0;
	char *act = NULL;

	fprintf(ftp->FD, "PASV\r\n");
	line = ftp_getline(ftp);

	act = strrchr(line, ',') -5;
	/* p3_secure is for secureing the data port,
	 * in case it will be 3 digits */
	sscanf(act, "%d,%d,%d)", &p3_secure, &p1, &p2);
	
	#ifdef DEBUG
	print(2, "Dataport: %d\n", (p1*256+p2));
	#endif
	return (p1*256+p2);
}

/* Will try to login */
int ftp_login(ftp_t *ftp) {
	char *line = NULL;

	fprintf(ftp->FD, "USER %s\r\n", ftp->user);
	line = ftp_getline(ftp);
	ftp->code = atoi(line);

	/* case it will login anonymous user without PASS */
	if ( ftp->code == 220 || ftp->code == 230 ) {
		ftp->logged = 1;
		#ifdef DEBUG
			print(2, "Login success!\n");
		#endif
		return ftp->code;
	}
	fprintf(ftp->FD, "PASS %s\r\n", ftp->password);
	line = ftp_getline(ftp);
	ftp->code = atoi(line);

	if ( ftp->code == 220 || ftp->code == 230 ) {
		ftp->logged = 1;
		#ifdef DEBUG
		print(2, "Login success!\n");
		#endif
	}
	else 
		ftp->logged = 0;
	return ftp->code;
}

/* Will grab the banner */
int ftp_banner(ftp_t *ftp) {
	char *line = NULL;
	line = ftp_getline(ftp);
	ftp->code = atoi(line);
	return (ftp->code = 0);
}

/* Will return line from the socket */
char * ftp_getline(ftp_t *ftp) {
	static char line[MAX_STR];
	char *s = NULL;

	signal(SIGALRM, signal_handler);
	alarm(ftp->alarm_sec);

	while (( fgets(line, sizeof(line), ftp->FD)) != NULL ) {
    	/* Avoid the comments when connecting to server */
		if (strtol(line, &s, 10) != 0 && s != NULL) {
			if (isspace(*s)) {
				#ifdef DEBUG
					print(2,"%s", line);
				#endif
            	return line;
            }
        }
	}
		
	return "None\n";
}

/* handle TCP connection */
void ftp_mkcon(ftp_t *ftp) {
	/* Make the connection */
	ftp->FD = tcp_connect(ftp->server, ftp->port);
	if ( !ftp->FD) 
		die("Connection failed.");
	ftp_banner(ftp);
	ftp_login(ftp);

	/* Check if logged IN and not scanning */
	if ( ftp->logged != 1 && flag != 99 ) {
		die("%sNo logged in%s, try to reconnect.", RED,END);
	}
}

/* close connection */
void ftp_close(ftp_t *ftp) {
	fprintf(ftp->FD, "QUIT\r\n");
	#ifdef DEBUG
		print(2, "%s", ftp_getline(ftp));
	#endif
	fclose(ftp->FD);
	ftp->logged = 0;
}



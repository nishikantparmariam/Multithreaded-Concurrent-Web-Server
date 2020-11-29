#include "common_headers.h"


int request_parse_uri_modified(char *uri, char *filename, char *cgiargs) {
    char *ptr;    
    if (!strstr(uri, "cgi")) { 	            
        strcpy(cgiargs, "");
        sprintf(filename, ".%s", uri);
        if (uri[strlen(uri)-1] == '/') {
            strcat(filename, "index.html");
        }
        return 1;
    } else { 
        // dynamic
        ptr = index(uri, '?');
        if (ptr) {
            strcpy(cgiargs, ptr+1);
            *ptr = '\0';
        } else {
            strcpy(cgiargs, "");
        }                
        return 0;
    }
}


file_prop* request_file_properties(int fd) {
	char buf[8192], method[8192], uri[8192], version[8192];
    char filename[8192], cgiargs[8192];	
	struct stat s;
	recv(fd, buf, sizeof(buf), MSG_PEEK);
   	sscanf(buf, "%s %s %s\n", method, uri, version);
	request_parse_uri_modified(uri, filename, cgiargs);
	stat(filename, &s);

    file_prop* FileProp = (file_prop*)malloc(sizeof(file_prop));
    FileProp->file_name = filename;
    FileProp->file_size = (long)s.st_size;

	return FileProp;
}

/*
    int is_uri_safe(char uri[]):
    Function to check if URI is trying to use directory of the server 
    Returns 1 if uri is safe
    Returns 0 if uri is not safe     
*/
int is_uri_safe(char uri[]) {
    int safe = 1;
    for(int i = 0; i < strlen(uri) - 1; i++) {
        if(uri[i] == '.' && uri[i+1] == '.') {
            safe = 0; break;
        }
    }
    return safe;
}

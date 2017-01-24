#include "minet_socket.h"
#include <stdlib.h>
#include <ctype.h>

#define BUFSIZE 1024

int write_n_bytes(int fd, char * buf, int count);

void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char * argv[]) {
    char * server_name = NULL;
    int server_port = 0;
    char * server_path = NULL;
			
    int sock = 0;
    int rc = -1;
    int datalen = 0;
    bool ok = true;
    struct sockaddr_in sa;
    FILE * wheretoprint = stdout;
    struct hostent * site = NULL;
    char * req = NULL;

    char buf[BUFSIZE + 1];
    char * bptr = NULL;
    char * bptr2 = NULL;
    char * endheaders = NULL;
   
    struct timeval timeout;
    fd_set set;
	
	fprintf(stdout, "123\n");

    /*parse args */
    if (argc != 5) {
	fprintf(stderr, "usage: http_client k|u server port path\n");
	exit(-1);
    }

    server_name = argv[2];
    server_port = atoi(argv[3]);
    server_path = argv[4];

	

    /* initialize minet */
    if (toupper(*(argv[1])) == 'K') { 
	minet_init(MINET_KERNEL);
    } else if (toupper(*(argv[1])) == 'U') { 
	minet_init(MINET_USER);
    } else {
	fprintf(stderr, "First argument must be k or u\n");
	exit(-1);
    }

    /* create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		error("ERROR opening socket");
	
    // Do DNS lookup
    /* Hint: use gethostbyname() */
	site = gethostbyname(server_name);
	if(site == NULL){
		fprintf(stderr,"ERROR, no such host as %s\n", site);
		exit(0);
	}
		
    /* set address */
	memset(&sa,0,sizeof sa);
	sa.sin_port = htons(server_port);
	sa.sin_addr.s_addr = htonl(*(unsigned long *)site->h_addr_list[0]);
	sa.sin_family = AF_INET;
	
    /* connect socket */
    if(connect(sock, (struct sockaddr*)&sa, sizeof(sa)) < 0){
		error("ERROR connecting");
	}	
	
    /* send request */	
	char * getRequest = strcat("GET ", server_path, " HTTP/1.0\r\n\r\n");
	
	if( write(sock, getRequest, strlen(getRequest)) < strlen(getRequest))
		error("ERROR did nor send all bytes");
								
    /* wait till socket can be read */
    /* Hint: use select(), and ignore timeout for now. */
    
	
	int n;	
	
	FD_ZERO(&set);		//zero out set
	FD_SET(sock,&set);	//add sockfd to set

	if(select(sock+1, &set, NULL, NULL, NULL) == -1)
	{
		perror("select");
		exit(1);
	}
	
	if(FD_ISSET(sock, &set))
	{
		n = read(sock, buf, BUFSIZE);
		if(n<0)
		{
			error("ERROR writing to socket");
			
		}
		//fprintf(stdout, "# of bytes read = %d\n", n);			
	}
	
	fprintf(wheretoprint, "%s\n",buf);
	
	
	
    /* first read loop -- read headers */
    
	
    /* examine return code */   
    //Skip "HTTP/1.0"
    //remove the '\0'
    // Normal reply has return code 200

    /* print first part of response */

    /* second read loop -- print out the rest of the response */
    
    /*close socket and deinitialize */
	close(sock);


    if (ok) {
	return 0;
    } else {
	return -1;
    }
}

int write_n_bytes(int fd, char * buf, int count) {
    int rc = 0;
    int totalwritten = 0;

    while ((rc = minet_write(fd, buf + totalwritten, count - totalwritten)) > 0) {
	totalwritten += rc;
    }
    
    if (rc < 0) {
	return -1;
    } else {
	return totalwritten;
    }
}


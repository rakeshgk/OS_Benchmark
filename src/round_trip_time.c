#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define DATA_BYTES 1048576
#define RADDR "128.54.253.242"
#define RPORT 7
#define NUM_LOOP 100

int main(void)
{
	uint32_t cycles_low, cycles_high;
	uint32_t cycles_low1, cycles_high1;
	uint64_t start, end;
	int i,len;
	FILE* fp;
    	fp = fopen("../data/round_trip_time.csv", "a");
    	int sock_fd, status, bytes_read;
    	struct addrinfo remote;
    	struct sockaddr_in remote_addr;
    	struct in_addr raddr;
    	char buf[DATA_BYTES],msg[DATA_BYTES];
	for (i = 0; i < DATA_BYTES; i++)
	        msg[i] = '0';
	len=strlen(msg);
	//socket struct setup
    	memset(&remote, 0, sizeof remote);
    	memset(&remote_addr, 0, sizeof remote_addr);
	remote.ai_family = AF_INET;
    	remote.ai_socktype = SOCK_STREAM;
	raddr.s_addr = inet_addr(RADDR);
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(RPORT);
	remote_addr.sin_addr = raddr;
	
        for (i=0; i<NUM_LOOP; i++) {	
		asm volatile (
		        "CPUID\n\t"
		        "RDTSC\n\t"
		        "mov %%edx, %0\n\t"
		        "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
		        "%rax", "%rbx", "%rcx", "%rdx");
		//initialise socket
		if ((sock_fd = socket(remote.ai_family, remote.ai_socktype, remote.ai_protocol)) == -1) {
		    printf("unable to create client socket");
		    return 1;
		}
		//connect to server
		if (connect(sock_fd, (struct sockaddr *) &remote_addr, sizeof remote_addr) == -1) {
		    close(sock_fd);
		    printf("unable to connect to server");
		    return 1;
		}

		// send packet and recieve it back
		send(sock_fd, msg, len, 0);
        	bytes_read = recv(sock_fd, buf, DATA_BYTES, 0);
		asm volatile(
		        "RDTSCP\n\t"
		        "mov %%edx, %0\n\t"
		        "mov %%eax, %1\n\t"
		        "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
		        "%rbx", "%rcx", "%rdx");
		if(bytes_read==DATA_BYTES){
		    	start = (((uint64_t)cycles_high << 32) | cycles_low);
		    	end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);
		    	fprintf(fp, "%lu,%lu\n", start, end);
		}
		close(sock_fd);
	}
	close(sock_fd);
   	fclose(fp);
   	return 0;
}




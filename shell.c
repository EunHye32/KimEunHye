#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <pwd.h> // userhostname

#include <dirent.h> // ls

#define MAX_LEN_LINE    100
#define LEN_HOSTNAME	30 // userhostname

#define PATH_LEN 1024 // cwd

int main(void)
{
	char command[MAX_LEN_LINE];
	char *args[] = {command, NULL};
	int ret, status;
	pid_t pid, cpid;

	/// 추가 기능: Shell 시작 시, hostname과 username 출력  
	char hostname[LEN_HOSTNAME + 1];
	memset(hostname, 0x00, sizeof(hostname));
	
	printf("\033[1;33musername: %s\n\033[0m", getpwuid(getuid())->pw_name); // Yellow color

	gethostname(hostname, LEN_HOSTNAME);
	printf("\033[1;33mhostname: %s\n\033[0m", hostname); // Orange color
	///

	while (true) {
		char *s;
		int len;
		printf("MyShell $ ");
		s = fgets(command, MAX_LEN_LINE, stdin);

		if (s == NULL) {
			fprintf(stderr, "\033[31mfgets failed\033[0m\n"); // red color
			exit(1);
		}

		// len = strlen(command);
		// printf("%d\n", len);
		if (command[len - 1] == '\n') {
			command[len - 1] = '\0';
		}

		// printf("[%s]\n", command);

		/// 추가 기능: getpwuid 명령을 사용해 사용자 상세 정보 출력
		if (!strcmp(command, "getpwuid")) {
			// Orange color
			printf("\033[33m------------------------------------\033[0m\n");
			printf("\033[33muser name     : %s\033[0m\n", getpwuid(getuid())->pw_name);
			printf("\033[33muser id       : %d\033[0m\n", getpwuid(getuid())->pw_uid);
			printf("\033[33mhome directory: %s\033[0m\n", getpwuid(getuid())->pw_dir);
			printf("\033[33mshell program : %s\033[0m\n", getpwuid(getuid())->pw_shell);
			printf("\033[33mhost name     : %s\033[0m\n", hostname);
			printf("\033[33m------------------------------------\033[0m\n");
		}
		///

		/// 추가 기능: cwd 명령 사용해 현재 작업 중인 디렉토리 출력
		if (!strcmp(command, "cwd")) {
			char *pathname;
			pathname = malloc(PATH_LEN);
			if (getcwd(pathname, PATH_LEN) == NULL) {
				fprintf(stderr, "\033[31mgetcwd error\033[0m\n"); // red color
				exit(1);
			}

			printf("\033[32mcurrent directory = %s\033[0m\n", pathname); // green color
			free(pathname);
		}
		///

		/// 추가 기능: ls 명령 사용해 디렉토리에 있는 내용 출력 
		if (!strcmp(command, "ls")) {
			char * cwd = (char *)malloc(sizeof(char) * 1024);
			DIR * dir = NULL;
			struct dirent * entry = NULL;
			getcwd(cwd, 1024);

			if ((dir = opendir(cwd)) == NULL) {
				//red color
				printf("\033[31mdirectory 정보를 읽을 수 없습니다.(ls error)\n\033[0m");

				exit(1);
			}

			printf("%ld\n", sizeof(entry));/////////
			while ((entry = readdir(dir)) != NULL) {
				printf("\033[1;34m%s\033[0m\t", entry->d_name); // blue color
			}

			printf("\n");
			//free(cwd);
			closedir(dir);
		}
		///

		/// 추가 기능: exit 명령 사용해 현재 쉘 종료
		if (!strcmp(command, "exit")) {
			printf("\033[1;31mClose MyShell. Goodbye\033[0m\n"); // red color
			exit(1);
		}
		///

		pid = fork();
		if (pid < 0) {
			fprintf(stderr, "\033[32mfork failed\033[0m\n"); // red color
			exit(1);
		} 
		if (pid != 0) {  /* parent */
			cpid = waitpid(pid, &status, 0);
			if (cpid != pid) {
				fprintf(stderr, "waitpid failed\n");        
			}
			printf("Child process terminated\n");
			if (WIFEXITED(status)) {
				printf("Exit status is %d\n", WEXITSTATUS(status)); 
			}
		}
		else {  /* child */
			ret = execve(args[0], args, NULL);
			if (ret < 0) {
				fprintf(stderr, "execve failed\n");   
				return 1;
			}
		} 
	}
	return 0;
}

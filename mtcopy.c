#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

int partition_size;
int fd1, fd2, offset, i = 0;

void *thread_function(void *arg)
{
	printf("Thread %d is started\n", (int *)(arg));
	char buffer[100];
	int n = 0;
	offset = i++ * partition_size;
	for(i = offset; i <= partition_size; i++)
	{
		pread(fd1, buffer, 100, offset);
		pwrite(fd1, buffer, 100, offset);
	}
}

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		printf("Usage : ./a.out src.txt dest.txt\n");
		return 0;
	}
	else
	{
		char buffer[1024];
		int n_cores;
		FILE *fp = popen("nproc", "r");
	     	if(fp == NULL)
		{
			perror("popen failed : ");
			return 0;
		}

		while(fgets(buffer, sizeof(buffer), fp) != NULL)
		{
			//printf("%s", buffer);
			n_cores = atoi(buffer);
			printf("%d\n", n_cores);
		}
		pclose(fp);

		fd1 = open(argv[1], O_RDONLY);
		if(fd1 == -1)
		{
			perror("Error in opening file : ");
			return 0;
		}

		
		struct stat st;
    		if (stat(argv[1], &st) == -1) 
		{
        		perror("stat");
        		exit(EXIT_FAILURE);
    		}
    		off_t file_size = st.st_size;
		printf("%ld\n", file_size);


		partition_size = file_size / (n_cores * 2);

		fd2 = open(argv[2], O_WRONLY | O_CREAT, 0777);
		int i;
		pthread_t thread[2 * n_cores];

		char *arg[n_cores * 2];

		for(i = 0; i < (n_cores * 2); i++)
		{
			arg[i] = &i;
			pthread_create(&thread[i], NULL, thread_function, (void *)arg[i]);
		}

		for(i = 0; i < (n_cores * 2); i++)
		{
			pthread_join(thread[i], NULL);
		}

	}
}

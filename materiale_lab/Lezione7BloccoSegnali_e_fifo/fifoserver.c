include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAX_BUF_SIZE 1000
int main(int argc, char *argv[])
{int fd, ret_val, count, numread;
 char buf[MAX_BUF_SIZE];
 /* Create the named - pipe */
 ret_val = mkfifo("miafifo", 0666);
 if ((ret_val == -1) && (errno != EEXIST))
  {	perror("Error creating the named pipe");exit (1); }
    /* Open the pipe for reading */
    fd = open("miafifo", O_RDONLY);
    /* Read from the pipe */
	numread = read(fd, buf, MAX_BUF_SIZE);
	buf[numread] = '0';
	printf("Server : Read From the pipe : %s\n", buf);
	/* Convert to the string to upper case */
	count = 0;
	while (count < numread)
	{buf[count] = toupper(buf[count]);
	 count++;
	}
 printf("Server : Converted String : %s\n", buf);
}

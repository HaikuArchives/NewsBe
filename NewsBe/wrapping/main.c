/*
 * $Id$
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/stat.h>


char *NewQuotedText(char **text, long start, long end, char *quoteString);
char *RewrapText(char **text, long start_index, long end_index);


size_t	FileSize(char *fnam)
{
	struct stat	statbuf;

	if(stat(fnam, &statbuf) < 0) {
		fprintf(stderr, "could not stat() %s\n", fnam);
		return(-1);
	}
	return(statbuf.st_size);
}

char *SlurpOpenFile(FILE *fd, size_t size)
{
	char *buf;

	if((buf = (char *)malloc(size)) == NULL ) {
		fprintf(stderr, "could not allocate %li bytes\n", (long)size);
		return(NULL);
	}
	if(fread(buf, 1, size, fd) != size) {
		fprintf(stderr, "could not fread() %li bytes\n", (long)size);
		free(buf);
		return(NULL);
	}
	return(buf);
}

char *SlurpFile(char *fnam, size_t *size)
{
	char *buf;
	FILE *fd;

	*size = FileSize(fnam);
	if((fd = fopen(fnam, "r")) == NULL) {
		fprintf(stderr, "could not fopen() %s\n", fnam);
		*size = 0;
		return(NULL);
	}
	buf = SlurpOpenFile(fd, *size);
	if(buf == NULL) *size = 0;
	fclose(fd);
	return(buf);
}


int main(int argc, char ** argv)
{
	int i = 0;
	size_t	size;
	char	*inbuf, *outbuf;

	while(++i < argc) {
		inbuf = SlurpFile(argv[i], &size); outbuf = NULL;
// fprintf(stdout, "Read file: %s, size: %d\n", argv[i], size);
		#if PLAIN
			outbuf = NewQuotedText(&inbuf, 0, strlen(inbuf), "> ");
		#elif QUOTE
			outbuf = RewrapText(&inbuf, 0, strlen(inbuf));
		#endif
		if(outbuf != NULL) {
//			puts(inbuf);
			puts(outbuf);
			free(outbuf);
		} else {
			fprintf(stderr, "Operation failed for %s\n", argv[i]);
		}
		free(inbuf);
		fflush(stderr); fflush(stdout);
	}
	return 0;
}

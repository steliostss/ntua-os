#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


void doWrite(int fd_out, const char *buff, ssize_t len);
void write_file(int fd_out, const char *infile);


int main(int argc, char **argv)
{


    int fd_out, i;
    int oflags = O_CREAT | O_WRONLY | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR;
    int flag_same = 0;


    if(argc < 3){
        printf("Usage: ./fconc.out infile1 infile2 [outfile (default:fconc.out.out)]\n");
        return(2);
    }
    else if (argc > 3) {

        for (i=1; i < argc-1; i++){
            if (!(strcmp(argv[i],argv[argc-1]))) {
                flag_same = 1;
                break;
            }
        }

        if(flag_same == 1) {
            printf("Warning: The requested output file matches one of the input files. File will be overwritten.\n");
        }

        fd_out = open("output_file", oflags, mode);

        for (i = 1; i < argc - 1; i++) {
            write_file(fd_out, argv[i]);
        }

        fd_out = open(argv[argc - 1], oflags, mode);
        write_file(fd_out, "output_file");
    	remove("output_file");
    }
    else {
        fd_out = open("fconc.out", oflags, mode);
        if (fd_out == -1) {
            perror("Open");
            exit(1);
        }
        for (i = 1; i < argc; i++) {
            write_file(fd_out, argv[i]);
        }
    }
    return 0;
}



void write_file(int fd_out, const char *infile){

    int fd_in;
    char buff [1024];
    size_t len;
    ssize_t rcnt;

    fd_in = open(infile, O_RDONLY);
    if (fd_in == -1) {
        perror("Open");
        exit(1);
    }

    for(;;) {

        rcnt = read(fd_in, buff, sizeof(buff)-1);

        if (rcnt == -1) {
            perror("read");
            exit(1);
        }

        if (rcnt == 0) { //EOF
            return;
        }

        buff[rcnt] = '\0';
        len = strlen(buff);
        doWrite(fd_out, buff, len);
    }
}



void doWrite(int fd_out, const char *buff, ssize_t len){

    size_t idx_out;
    ssize_t wcnt;

    idx_out = 0;

    do {
        wcnt = write(fd_out, buff + idx_out, len - idx_out);
        if (wcnt == -1) {
            perror("write");
            exit(1);
        }

        idx_out += wcnt;

    } while(idx_out < len);
}

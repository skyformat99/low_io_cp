#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "low_io_cp.h"
#include "util.h"

// Useage: low_io_cp /data/file1 /date/file2
// only support copy one file. Do not support dir.
// only support file with full path name.
int main(int argc, char **argv)
{
    printf("bits:\t%d\n", GetProgramBits());

    if (argc != 3)
    {
        printf("Wrong parameter\nPlease give source file name and target file name. \n Example: low_io_cp /data/file1 /date/file2\n");
        return -1;
    }

    char* pfile1_name = argv[1];
    char* pfile2_name = argv[2];

    int fd_src = open(pfile1_name, O_RDONLY);
    if (fd_src == -1)
    {
        error_exit("Can't open source file");
    }

    // source  file access mode
    struct stat stat_buf;
    if (fstat(fd_src, &stat_buf) == -1)
      error_exit("Get source file's access mode fail");

    int fd_target = open(pfile2_name, O_WRONLY | O_CREAT | O_TRUNC, stat_buf.st_mode);
    if (fd_target == -1)
    {
        error_exit("Can't open target file");
    }

    const time_t begin_time = time(NULL);
    do_cp(fd_src, fd_target, &stat_buf);
    const time_t end_time = time(NULL);
    printf("100%% total seconds of copying file: %ld\n", end_time - begin_time);

    // try making owner same as source file.
    if (fchown(fd_target, stat_buf.st_uid, stat_buf.st_gid) == -1)
      printf("Info: Changing owner to source file's owner failed. Because: %s\n", strerror(errno));

    exit(0);
}

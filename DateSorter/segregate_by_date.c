/////////////////////////////////////////////////////////////////////////////
//
//  File Name     : segregate_by_date.c
//  Project       : Segregate By Date
//  Description   : Linux command-line utility that organizes the regular
//                  files of a directory into sub-folders named YYYY-MM-DD,
//                  based on each file's creation (birth) time obtained
//                  through the statx() system call.
//
//  Usage         : ./segregate_by_date <Directory_absolute_Path>
//
//  Platform      : Linux (kernel 4.11+ for statx, glibc 2.28+)
//  Compile       : gcc -Wall -Wextra -o segregate_by_date segregate_by_date.c
//
//  Author        : Mangesh Ashok Bedre
//  Date          : 30/09/2026
//
/////////////////////////////////////////////////////////////////////////////

#define _GNU_SOURCE

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/stat.h>   /* struct statx, STATX_* constants */
#include <time.h>
#include <errno.h>

/////////////////////////////////
//
//  Macros
//
////////////////////////////////

#define MAX_PATH_SIZE 256

/////////////////////////////////
//
//  Function Prototypes
//
////////////////////////////////

void GetCreationTime(char * ,char *);

//////////////////////////////////////////////////////
//
//  Function Name : Segregate_By_Date
//  Input         : directory_name (String) - path of the directory to
//                  organize
//  Output        : None (exits with EXIT_FAILURE if the directory
//                  cannot be opened)
//  Description   : Opens the given directory and iterates over its
//                  entries. Hidden files (names starting with '.') and
//                  anything that is not a regular file are skipped.
//                  Every remaining file is handed over to
//                  GetCreationTime(), which moves it into a folder
//                  named after its creation date.
//
//  Author        : Mangesh Ashok Bedre
//  Date          : 30/09/2026
//
//////////////////////////////////////////////////////

void Segregate_By_Date(char *directory_name)
{
    DIR * fd = NULL;
    struct dirent *entry = NULL;
    char file_name[MAX_PATH_SIZE];

    fd = opendir(directory_name);

    if(fd == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    printf("Open directory\n");

    while(entry = readdir(fd))
    {
        if (entry->d_type != DT_REG)   // only normal files
            continue;
        if (entry->d_name[0] == '.')   // skip hidden files
            continue;

        printf("%s\n",entry->d_name);

        // printf(directory_name);
        strcpy(file_name,entry->d_name);

        printf("File name : %s\n", file_name);

        GetCreationTime(directory_name,file_name);
    }

    closedir(fd);

    printf("Success\n");
}

//////////////////////////////////////////////////////
//
//  Function Name : GetCreationTime
//  Input         : directory_name (String) - directory containing the file
//                  file_name      (String) - name of the file to move
//  Output        : None (exits with EXIT_FAILURE on statx/rename failure)
//  Description   : Builds the full path of the file and reads its
//                  creation (birth) time using statx(). The time is
//                  converted to UTC and formatted as YYYY-MM-DD. A
//                  folder with that name is created inside the target
//                  directory (if it does not already exist) and the file
//                  is moved into it using rename(). If the filesystem
//                  does not provide a birth time, an error message is
//                  printed and the file is left untouched.
//
//  Author        : Mangesh Ashok Bedre
//  Date          : 30/09/2026
//
//////////////////////////////////////////////////////

void GetCreationTime(char * directory_name,char * file_name)
{
    struct statx stx;
    struct tm tm;
    char CompleteName[MAX_PATH_SIZE];
    char buf[64];
    char dest[MAX_PATH_SIZE];
    time_t t = 0;
    char AbsolutePathOFile[512];

    memset(&CompleteName,0,sizeof(MAX_PATH_SIZE));

    snprintf(CompleteName,sizeof(CompleteName),"%s/%s",directory_name,file_name);

    int ret = statx(AT_FDCWD, CompleteName, AT_SYMLINK_NOFOLLOW,
                    STATX_BASIC_STATS | STATX_BTIME, &stx);
    if (ret == -1)
    {
        perror("statx");
        exit(EXIT_FAILURE);
    }

    if (stx.stx_mask & STATX_BTIME)
    {
        t = stx.stx_btime.tv_sec;
        gmtime_r(&t, &tm);

        memset(&buf,0,sizeof(buf));

        strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);

        printf("Folder name : %s\n", buf);

        snprintf(dest,sizeof(dest),"%s/%s",directory_name,buf);

        printf("Directory Path to mkdir : %s\n",dest);

        // clean memory
        memset(&AbsolutePathOFile,0,sizeof(AbsolutePathOFile));

        snprintf(AbsolutePathOFile,sizeof(AbsolutePathOFile) , "%s/%s",dest,file_name);

        // creating dir with date
        if (mkdir(dest, 0755) == -1)
        {
            if(errno == EEXIST)
            {
                // Error is dir already present then we need to just rename the file into that dir

                // rename call
                printf("Directory Already present we rename only\n");

                if(rename(CompleteName,AbsolutePathOFile) == -1)
                {
                    perror("Error rename : ");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    printf("Moved file : %s successfully\n",file_name);
                }
            }
            else
            {
                perror("Error : making dir");
                return;
            }
        }
        else
        {
            // rename call

            if(rename(CompleteName,AbsolutePathOFile) == -1)
            {
                perror("Error rename : ");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("Moved file : %s successfully\n",file_name);
            }
         }
    }
    else
    {
        printf("Error : (not available on this filesystem)\n");
    }
}

//////////////////////////////////////////////////////
//
//  Function Name : main
//  Input         : agrc (int)    - argument count
//                  argv (char**) - argument vector; argv[1] must be the
//                                  absolute path of the directory
//  Output        : 0 on success, -1 on invalid usage
//  Description   : Entry point of the program. Validates the command
//                  line arguments and starts the segregation process
//                  by calling Segregate_By_Date().
//
//  Author        : Mangesh Ashok Bedre
//  Date          : 30/09/2026
//
//////////////////////////////////////////////////////

int main(int agrc, char ** argv)
{
    if(agrc != 2)
    {
        printf("usage : <excutable_name> <Directory_absolute_Path>\n");
        return -1;
    }

    Segregate_By_Date(argv[1]);

    return 0;
}
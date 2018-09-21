/*
* myls.c
*
* usage: mimics the behhivior of the ls command in terminal
*
* Laura Friedrich and Henry Mound CS315 HW1
*/

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
  /*Open current directory*/
  DIR *currentDirectory;
  currentDirectory = opendir(".");
  struct dirent *dp;
  int opt;
  int argL = 0;
  int argA = 0;
  struct stat *buffer;
  buffer = malloc(sizeof(struct stat));

  /*get command line args*/
  while ((opt = getopt(argc, argv, "la")) != -1) {
    switch (opt) {
      case 'l':
      argL = 1;
      break;
      case 'a':
      argA = 1;
      break;
      default: /* '?' */
      printf("hello");
      break;
    }
  }
  /*Run main 'meat' of the program ie produce text to be printed */

  /*Go through all files in current directory*/
  while ((dp=readdir(currentDirectory)) != NULL){
    char *fileName = dp->d_name;
    /*if -l is an arg, print long*/
    if (argL == 1){
      stat(dp->d_name, buffer);
      /*if -a is an arg, show hidden files*/
      if(fileName[0] != '.' || argA == 1){
        int size = buffer->st_size;
        int userID = buffer->st_uid;
        int groupID = buffer->st_gid;
        char mtime[80];
        time_t t = buffer->st_mtime; /*st_mtime is type time_t */
        struct tm lt;
        localtime_r(&t, &lt); /* convert to struct tm */
        strftime(mtime, sizeof mtime, "%a, %d %b %Y %T", &lt);
        printf("%d \t %d \t %d \t %s \t %s \n", userID, groupID, size, mtime, fileName);
        free(buffer);
      }
    }
      /*if no args are given, just list files in current directory*/
    else{
      if(fileName[0] != '.' || argA == 1){
        printf("%s\t", dp->d_name);
      }
    }
  }
  /*clean up*/
  printf("\n");
  closedir(currentDirectory);
}

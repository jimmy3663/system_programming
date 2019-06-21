#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
   int aflag = 0, bflag = 0;
   char *cvalue = NULL;
   int index = 0, c = 0;
   int i = 0;
   opterr = 0;
   while((c = getopt(argc, argv, "abc:")) != -1)
   {
      switch(c)
      {
         case 'a' :
                        aflag++;
                        break;
         case 'b' :
                        bflag++;
                        break;
         case 'c' :
                         cvalue = optarg;
                        break;
         case '?' :
                        printf("Error\n");
                        break;
      }
   }
   printf("aflag = %d, bflag = %d, cvalue = %s\n", aflag, bflag, cvalue);
   if(optind != argc)
      printf("Non-option argument %s\n", argv[optind]);
   printf("%d\n",optind);
   printf("%d\n",argc);
   printf("%s\n",optarg);
   return 0;
}
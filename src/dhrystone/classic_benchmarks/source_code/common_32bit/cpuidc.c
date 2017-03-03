/*
;  nasm -f elf cpuida.asm   for cpuida.o
;  gcc cpuidc.c -c          for cpuidc.o
;  gcc test.c cpuidc.o cpuida.o -lrt -lc -lm -o test
;  ./test
*/

  #include <stdio.h>
  #include "cpuidh.h"
  #include <stdlib.h>     
  #include <string.h> 
  #include <time.h>
  #include <math.h>
  #include <sys/time.h>
  #include <sys/resource.h>

  char    configdata[10][200];
  char    timeday[30];
  char    idString1[100] = " ";
  char    idString2[100] = " ";
  double  theseSecs = 0.0;
  double  startSecs = 0.0;
  double  secs;
  int     megaHz;

  int     pagesize;
  double  ramGB;

  int CPUconf;
  int CPUavail;
  long int pages;
 

  unsigned int eaxCode1   = 0;
  unsigned int ebxCode1   = 0;
  unsigned int ecxCode1   = 0;
  unsigned int edxCode1   = 0;
  unsigned int ext81edx   = 0;
  unsigned int intel1amd2 = 0;
  unsigned int startCount = 0;
  unsigned int endCount   = 0;
  unsigned int cycleCount = 0;
  unsigned int millisecs  = 0;
  unsigned int looptime   = 10;
  
  int     hasMMX = 0;
  int     hasSSE = 0;
  int     hasSSE2 = 0;
  int     hasSSE3 = 0;
  int     has3DNow = 0;

  #include <sys/sysinfo.h> 
  #include <sys/utsname.h> 


  void local_time()
  {
     time_t t;

     t = time(NULL);
     sprintf(timeday, "%s", asctime(localtime(&t)));
     return;
  }

  struct timespec tp1;

  void getSecs()
  {
     clock_gettime(CLOCK_REALTIME, &tp1);
 
     theseSecs =  tp1.tv_sec + tp1.tv_nsec / 1e9;           
     return;
  }

  void start_time()
  {
      getSecs();
      startSecs = theseSecs;
      return;
  }

  void end_time()
  {
      getSecs();
      secs = theseSecs - startSecs;
      millisecs = (int)(1000.0 * secs);
      return;
  }    

  int getDetails()
  {
     int max;
     int min = 1000000;
     int i;
     unsigned int MM_EXTENSION   = 0x00800000;
     unsigned int SSE_EXTENSION  = 0x02000000;
     unsigned int SSE2_EXTENSION = 0x04000000;
     unsigned int SSE3_EXTENSION = 0x00000001;
     unsigned int _3DNOW_FEATURE = 0x80000000;

     printf("  ####################################################\n  getDetails and MHz\n\n");

     struct sysinfo sysdata;
     struct utsname udetails; 
 
      _cpuida();
     sprintf(configdata[1], "  Assembler CPUID and RDTSC      ");  
     sprintf(configdata[2], "  CPU %s, Features Code %8.8X, Model Code %8.8X",
                           idString1, edxCode1, eaxCode1);
     sprintf(configdata[3], "  %s", idString2);

     max = 0;
     for (i=0; i<10; i++)
     {
        startCount = 0;
        endCount   = 0;
        start_time();
        _calculateMHz();
        end_time();      
        megaHz = (int)((double)cycleCount / 1000000.0 / secs + 0.5);
        if (megaHz > max) max = megaHz;
        if (megaHz < min) min = megaHz;
     }
     sprintf(configdata[4], "  Measured - Minimum %d MHz, Maximum %d MHz", min, max);
     megaHz = max;

     sprintf(configdata[5], "  Linux Functions"); 
     CPUconf  =  get_nprocs_conf();
     CPUavail =  get_nprocs();
     sprintf(configdata[6], "  get_nprocs() - CPUs %d, Configured CPUs %d", CPUconf, CPUavail);

     pages = get_phys_pages();
     pagesize = getpagesize();
     ramGB = ((double)pages * (double)pagesize / 1024 / 1024 / 1024);
     sprintf(configdata[7], "  get_phys_pages() and size - RAM Size %5.2f GB, Page Size %d Bytes", ramGB, pagesize);

     if (uname(&udetails) > -1)
     {
        sprintf(configdata[8], "  uname() - %s, %s, %s", udetails.sysname,
                                  udetails.nodename, udetails.release);  
        sprintf(configdata[9], "  %s, %s", udetails.version, udetails.machine);
     }
     else
     {
         sprintf(configdata[8], "  uname() - No details"); 
         sprintf(configdata[9], "  ");
     }

     if (edxCode1 & MM_EXTENSION)
     {
           hasMMX = 1;
     }
     if (edxCode1 & SSE_EXTENSION)
     {
           hasSSE = 1;
     }
     if (edxCode1 & SSE2_EXTENSION)
     {
           hasSSE2 = 1;
     }
     if (ecxCode1 & SSE3_EXTENSION)
     {
           hasSSE3 = 1;
     }
     if (intel1amd2 == 2)
     {
           if (ext81edx & _3DNOW_FEATURE)        
           {
               has3DNow = 1;
           }
     }

     return 0; 
  }


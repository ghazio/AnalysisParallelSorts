#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXSTRINGSIZE 10

#define readfile =


int copier(char* finalstring, char* line,char* token,int index){
  char* read = strtok(line, token);
  strcpy(finalstring,&read[index]);
  return 0;
}

float time_converter(char* string){
  char* delim1 ="m";
  char* delim2 ="s";
  char* min = strtok(string, delim1);
  char* seconds = strtok(NULL,delim2);
  //printf("%s %s",min,seconds);

  float runtime = atof(min)+atof(seconds);

  //printf("seconds: %f\n", runtime);
  return runtime;
}


int main (void){
  FILE *myfile = fopen("resultsbigger__OddevenMPI.txt", "r" );
  FILE *output;
  output=fopen("MyFile.csv", "w+");
  fprintf(output,"Num_procs, Size, User, Real, System\n");
  char line[256];
  char* size = "SIZE:";
  char* sys  = "sys";
  char* real = "real";
  char* user = "user";


  char size_final[6]="";
  char core_final[9] ="";
  char real_final[10]="";
  char user_final[10]="";
  char sys_final[10]="";
  int i =0;
  while(fgets(line, sizeof(line), myfile)){
      if(strncmp(line,size,5)==0){

      //    printf("Starting\n");
          char* delim1 = ",";
          //read till the first comma
          copier(size_final, line,delim1,6);
        //  printf("%s\n",size_final);
          char* delim2 = "_";

          //find the process number and store it, max is 256
          copier(core_final,NULL,delim2,12);
        //  printf("token: %s\n",core_final);
          char* delim3 = " ";
          int i=0;
          while(i<3){
              i++;
              fgets(line, sizeof(line), myfile);
              //printf("line: %s \n",line);
            }



          copier(user_final,line,delim3,5);
          //printf("%s\n",user_final);

          fgets(line, sizeof(line), myfile);

          copier(real_final,line,delim3,5);
        //  printf("%s\n",real_final);

          fgets(line, sizeof(line), myfile);
          copier(sys_final,line,delim3,4);
        //  printf("%s\n",sys_final);
          float user_time=time_converter(user_final);
          float sys_time=time_converter(sys_final);
          float real_time=time_converter(real_final);
          //Num_procs, Size, User, Real, System
          fprintf(output,"%d, %d, %f, %f, %f\n",atoi(core_final),atoi(size_final),user_time, real_time, sys_time);
        }


      }
      fclose(output);


    return 0;

}

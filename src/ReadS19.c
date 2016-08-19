#include "ReadS19.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <CException.h>
#include "MCU_Operation.h"
#include "CPUConfig.h"
#include "Memory.h"
#include "ErrorObject.h"
#include <malloc.h>
#include "Description.h"

/**
   Library    char *fgets(char *str, int n, FILE *stream)
                return the string of line


*/


void readS19(char *fileName){
  char line[512];
  FILE *fp = fopen(fileName, "r");
  
  if(!fp) Throw(ERR_FAILED_TO_OPEN);
  
  while(fgets(line, sizeof(line), fp)){
    S19Interpret(line);
  }
 
  fclose(fp);
}

void S19Interpret(char *line){
  char mark[2];
   
  if( mark == "S0"){
    
  }
  else if( mark == "S1"){
    
  }
  else if( mark == "S9"){
    
  }
}


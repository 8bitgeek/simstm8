#include "Memory.h"
#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include "CPUConfig.h"
#include "MCU_Operation.h"
#include "Description.h"
#include "ErrorObject.h"

CPU_t *cpu;
uint32_t *pcToLoad;

MemoryBlock *ramBlock, *gpioBlock, *eepromBlock, *flashBlock, *cpuBlock;

MemoryMap memoryTable[0x280] = {};

MemoryBlock *createMemoryBlock( uint32_t startAddr, uint32_t size){
  MemoryBlock *mb = malloc(sizeof(MemoryBlock));
  mb->startAddr =& startAddr;
  mb->size = size;
  mb->data = malloc(size);
  return mb;
}

void assignNoMemoryToWholeMemoryTable(){
  int i =0, range = STM8_END_ADDR/0x100 ;
  
  for( i=STM8_START_ADDR ; i<= range ; i++){
    memoryTable[i] = noMemory;
  }
}

void setMemoryTable(uint32_t (*memoryFunc)(Mode mode, uint32_t address, uint8_t size, uint32_t data), uint32_t startAddr, uint32_t memSize){
  int i, range = (startAddr + memSize)/0x100;
  
  startAddr /= 0x100;

  for( i=startAddr ; i<= range ; i++)
    memoryTable[i] = memoryFunc;
}

void memoryInit(void){
                 
  assignNoMemoryToWholeMemoryTable();
  ramInit   (RAM_START_ADDR   , RAM_SIZE   ); //0x0 ---> 0x03FF
  gpioInit  (GPIO_START_ADDR  , GPIO_SIZE  ); //0x5000----> 0x57FF
  eepromInit(EEPROM_START_ADDR, EEPROM_SIZE); //0x4000----> 0x427F
  cpuInit   (CPU_START_ADDR, CPU_SIZE);  //0x7F00 - 0x7FFF
  flashInit (OPTION_BYTE_START_ADDR, GPIO_START_ADDR - 1- OPTION_BYTE_START_ADDR); //0x4800 --> 0x4FFF
}


void memoryFree(void){
  free(ramBlock);
  free(gpioBlock);
  free(eepromBlock);
  free(flashBlock);
}

void instantiateCPU(void){
  cpu = malloc( sizeof(CPU_t) );
}

void ramInit(uint32_t address, uint32_t size){
   ramBlock = createMemoryBlock(address, size);
   setMemoryTable( ramMemory , address , size );
}

void gpioInit(uint32_t address, uint32_t size){
   gpioBlock = createMemoryBlock(address, size);
   setMemoryTable( gpioMemory , address , size );
}

void eepromInit(uint32_t address, uint32_t size){
   eepromBlock = createMemoryBlock(address, size);
   setMemoryTable( eepromMemory , address , size );
}

void flashInit(uint32_t address, uint32_t size){
   flashBlock = createMemoryBlock(address, size);
   setMemoryTable( flashMemory , address , size );
}

void cpuInit(uint32_t address, uint32_t size){
  cpuBlock = createMemoryBlock(address, size);
  setMemoryTable( cpuMemory , address , size );
  instantiateCPU();
  cpu = (CPU_t*) cpuBlock->data;
}

uint32_t noMemory(Mode mode, uint32_t address, uint8_t size, uint32_t data){
  
  if(mode == MEM_READ)
    ThrowError(ERR_UNINITIALIZED_ADDRESS, "Attempt to read %d byte start from address 0x%x\n"   , size, address);
  ThrowError(ERR_UNINITIALIZED_ADDRESS, "Attempt to write data 0x%x start from address 0x%x\n", data, address);
}

uint32_t ramMemory(Mode mode, uint32_t address, uint8_t size, uint32_t data)
{
  if(mode == MEM_READ){  
    return ( size == 1 ? RAM_ARR(address) : 
             size == 2 ? GET_WORD( RAM_ARR(address), RAM_ARR(address+1) )  
                       : GET_EXT(  RAM_ARR(address), RAM_ARR(address+1), RAM_ARR(address+2) )
           );
  }
 
  if(mode == MEM_WRITE){
    size == 1 ? RAM_ARR(address) = data :
    size == 2 ? setBigEndianWord(&RAM_ARR(address), data)
              : setBigEndianExt(&RAM_ARR(address), data) ;
    
  }
}

uint32_t gpioMemory(Mode mode, uint32_t address, uint8_t size, uint32_t data){

  if(mode == MEM_READ){
    return ( size == 1 ? GPIO_ARR(address) : 
             size == 2 ? GET_WORD( GPIO_ARR(address), GPIO_ARR(address+1) )  
                       : GET_EXT(  GPIO_ARR(address), GPIO_ARR(address+1), GPIO_ARR(address+2) )
           );
  }
  
  if(mode == MEM_WRITE){
    size == 1 ? GPIO_ARR(address) = data :
    size == 2 ? setBigEndianWord(&GPIO_ARR(address), data)
              : setBigEndianExt(&GPIO_ARR(address), data) ; 
  }
}

uint32_t eepromMemory  (Mode mode, uint32_t address, uint8_t size, uint32_t data){
  if(mode == MEM_READ){
    return ( size == 1 ? EEPROM_ARR(address) : 
             size == 2 ? GET_WORD( EEPROM_ARR(address), EEPROM_ARR(address+1) )  
                       : GET_EXT(  EEPROM_ARR(address), EEPROM_ARR(address+1), EEPROM_ARR(address+2) )
           );
  }
  
  if(mode == MEM_WRITE){
    size == 1 ? EEPROM_ARR(address) = data :
    size == 2 ? setBigEndianWord(&EEPROM_ARR(address), data)
              : setBigEndianExt(&EEPROM_ARR(address), data) ; 
  }   
}

uint32_t flashMemory   (Mode mode, uint32_t address, uint8_t size, uint32_t data){
  if(mode == MEM_READ){
    return ( size == 1 ? FLASH_ARR(address) : 
             size == 2 ? GET_WORD( FLASH_ARR(address), FLASH_ARR(address+1) )  
                       : GET_EXT(  FLASH_ARR(address), FLASH_ARR(address+1), FLASH_ARR(address+2) )
           );
  }
  
  if(mode == MEM_WRITE){
    size == 1 ? FLASH_ARR(address) = data :
    size == 2 ? setBigEndianWord(&FLASH_ARR(address), data)
              : setBigEndianExt(&FLASH_ARR(address), data) ; 
  }   
}


uint32_t cpuMemory(Mode mode, uint32_t address, uint8_t size, uint32_t data){
  if(mode == MEM_READ){
    return ( size == 1 ? CPU_ARR(address) : 
             size == 2 ? GET_WORD( CPU_ARR(address), CPU_ARR(address+1) )  
                       : GET_EXT(  CPU_ARR(address), CPU_ARR(address+1), CPU_ARR(address+2) )
           );
  }
  
  if(mode == MEM_WRITE){
    size == 1 ? CPU_ARR(address) = data :
    size == 2 ? setBigEndianWord(&CPU_ARR(address), data)
              : setBigEndianExt(&CPU_ARR(address), data) ; 
  } 
}

void clearConditionCodeRegister(Flag *ccR){
  ccR->full = 0;
  (ccR->bits).v  = 0;
  (ccR->bits).l1 = 0;
  (ccR->bits).h  = 0;
  (ccR->bits).l0 = 0;
  (ccR->bits).n  = 0;
  (ccR->bits).z  = 0;
  (ccR->bits).c  = 0;
}


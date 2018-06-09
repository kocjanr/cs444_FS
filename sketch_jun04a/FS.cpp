#include "FS.h"
byte bitMasks[] = {0x01, 0x02, 0x03, 0x04, 0x10, 0x20, 0x30, 0x40};
FS::FS() {
  fileCount = 0;
  for (int i = 0; i < 64; i++) {
    bitVector[i] = 0;
  }
}

//void FS::reformat()
//
// Takes in nothing sets all indexes of
// directory to -1 and sets all bits in
// bitvector to 1
void FS::reformat() {
  for (int i = 0; i < DIRECTORY_SIZE; i++) {
    directory[i] = -1;
  }

  memset(bitVector, 0xff, 64);

  setBit(0);
  setBit(1);

  eprom.write_page(0, (byte*)&directory);
  eprom.write_page(1, (byte*)&bitVector);
}

//void FS::initalize()
// takes in nothing loads in directory
// and bit vector from eprom
void FS::initalize() {
  eprom.read_page(0, (byte*)&directory);
  eprom.read_page(1, (byte*)&bitVector);
}


//void FS::list()
//
// i hate comments
void FS::list() {
  Serial << "Files in directory: " << endl;
  //eprom.read_page(0, (byte*)&directory);
  for (int i = 0; i < DIRECTORY_SIZE; i++) {

    int x = directory[i];
    if (x == -1) {
      continue;
    }

    eprom.read_page(x, (byte*)&fcb);
    Serial << "File: " << fcb.fileName << " " << sizeof(fcb) << " bytes" << endl;
  }
}

//void FS::create(char fileName[])
//
// takes in a file name and creates
// a fcb that gets stored to eprom
void FS::create(char fileName[]) {
  setBit(0);
  setBit(1);
  fileCount++;

  if (fileCount > DIRECTORY_SIZE) {
    Serial << "No room in directory" << endl;
    return;
  }

  //    //serach for duplicates
  //    for(int j=0;j<DIRECTORY_SIZE;j++){
  //      int block = directory[j];
  //      eprom.read_page(j,(byte*)&fcb);
  //      int val = strcmp(fcb.fileName, fileName);
  //      if(val == 0){
  //        Serial << "Duplicate File " << endl;
  //        return;
  //      }
  //    }

  //look for free spot in directory
  int freeDirectorySpace = 0;
  for (int k = 0; k < DIRECTORY_SIZE; k++) {
    if (directory[k] == -1) {
      freeDirectorySpace = k;
      break;
    }
  }

  int freeBlock = 0;
  int value = 0;
  int byteWithFreeBit = 0;
  for (int i = 0; i < 64; i++) {
    value = findBit(bitVector[i]);
    freeBlock = (i * 8) + value;
    byteWithFreeBit = i;
    break;
  }

  strcpy(fcb.fileName, fileName);
  fcb.offset = 0;
  setBit(freeBlock);
  directory[freeDirectorySpace] = freeBlock;
  eprom.write_page(freeBlock, (byte*)&fcb);

  eprom.write_page(0, (byte*)&directory);
  eprom.write_page(1, (byte*)&bitVector);

  Serial << "File created: " << fileName << endl;
}

void FS::openFile(char fileName[], FCB *f) {
  for (int i = 0; i < DIRECTORY_SIZE; i++) {
    int directoryBlock = directory[i];
    eprom.read_page(directoryBlock, (byte*)&fcb);
    int result = strcmp(fcb.fileName, fileName);
    if (result == 0) {
      eprom.read_page(directoryBlock, (byte*)f);
    }
  }
}

//void FS::writeFile(FCB *f, char data[], int dataSize)
//
// takes in a pointer of a fcb, the data to be written
// and the size of incoming data. does something stupid
// and writes that stupid stuff to eprom
void FS::writeFile(FCB *f, char data[], int dataSize) {
  int currentBlock = f->offset / 64;
  int amount = f->offset + dataSize;


  //Serial <<"File: " << f->fileName << " CB: " << currentBlock << " amnt: " << amount << endl;
  //Serial <<"data: " << data << " size: " << dataSize << " offset: " << f->offset << endl;

  if (dataSize > 1024) {
    //Serial << "This file is too big ya dingus " << endl;
    return;
  }

  if (currentBlock > 16) { // no room left in dataBlock
    //Serial << "This file is too big ya turkey " << endl;
    return;

  }

  if (currentBlock < 16) { // room in dataBlock array
    //Serial << "room in dataBlock array" << endl;

    if (f->dataBlock[currentBlock] == -1) { // no block loaded into current block
      //Serial << "no block loaded into current block" << endl;

      if (amount < ((1 + currentBlock) * 64)) { // room left to write in current block
        char dataBuffer[64];
        //Serial << "room left to write in current block" << endl;

        //find free bit
        int freeBlock = 0;
        int value = 0;
        int byteWithFreeBit = 0;
        for (int i = 0; i < 64; i++) {
          value = findBit(bitVector[i]);
          freeBlock = (i * 8) + value;
          byteWithFreeBit = i;
          break;
        }
        //set bit with free bit
        setBit(freeBlock);

        //update directory
        f->dataBlock[currentBlock] = freeBlock;

        //eprom read page with free bit into dataBuffer
        eprom.read_page(freeBlock, dataBuffer);

        //append data into bufferr
        strcpy(dataBuffer, data);

        //write buffer to eprom
        eprom.write_page(freeBlock, dataBuffer);

        //offset += dataSize
        f->offset += dataSize;
        eprom.write_page(0, (byte*)&directory);
        eprom.write_page(1, (byte*)&bitVector);
        return;
      }
      if (amount > ((1 + currentBlock) * 64)) { // not enough room in current block, get another block
        //Serial << "not enough room in current block, get another block" << endl;
        char dataBuffer[64];
        int numberOfBlocksToFind = dataSize / 64;
        //find how much free blocks are needed

        eprom.write_page(0, (byte*)&directory);
        eprom.write_page(1, (byte*)&bitVector);
        return;
      }

    }

    if (f->dataBlock[currentBlock] != -1) {
      //Serial << "block already in dataBlock[currentBlock]" << endl;

      if (amount < (1 + currentBlock) * 64) { //room left to write in block
        char dataBuffer[64];
        int offset = f->offset;
      
        //read in directory[freebit] from eprom
        eprom.read_page(f->dataBlock[currentBlock], dataBuffer);
        

        //append data to buffer
        strcpy(dataBuffer + offset, data);
       
        //write to directory[freebit] eprom
        eprom.write_page(f->dataBlock[currentBlock], dataBuffer);

        // offset =+ dataSize
        f->offset += dataSize;

        eprom.write_page(0, (byte*)&directory);
        eprom.write_page(1, (byte*)&bitVector);
  
        return;
      }

      if (amount > (1 + currentBlock) * 64) { //no room left to write in block, get anothe block
        char dataBuffer[64];
        
        int writeToNextBlock = amount - ((1 + currentBlock) * 64);
        int writeToCurrentBlock = dataSize - writeToNextBlock;
        
        char nextBlockBuffer[64] = {};
        char currentBLockBuffer[64] = {};

        strcpy(nextBlockBuffer, data + writeToNextBlock);
        strncpy(currentBLockBuffer,data,writeToCurrentBlock);
               
        eprom.read_page(f->dataBlock[currentBlock], dataBuffer);

        int x =f->dataBlock[currentBlock];

        strcpy(dataBuffer + f->offset, currentBLockBuffer);
       
        f->dataBlock[currentBlock] = x;
 
        eprom.write_page(f->dataBlock[currentBlock], dataBuffer);
        
        f->offset += writeToCurrentBlock;

        //find free bit
        int freeBlock = 0;
        int value = 0;
        int byteWithFreeBit = 0;
        for (int i = 0; i < 64; i++) {
          value = findBit(bitVector[i]);
          freeBlock = (i * 8) + value;
          byteWithFreeBit = i;
          break;
        }
        
        // find -1 in directory
        int freeDataBlockSpace = 0;
        for (int k = 0; k < 16; k++) {
          int val = f->dataBlock[k];
          if(val == 0){
            freeDataBlockSpace=k;
            break;
          }
        }

        //set bit with free bit
        setBit(freeBlock);
        f->dataBlock[freeDataBlockSpace] = freeBlock;
       
        eprom.read_page(f->dataBlock[freeDataBlockSpace], dataBuffer);
         
        strcpy(dataBuffer, nextBlockBuffer);
        eprom.write_page(f->dataBlock[freeDataBlockSpace], dataBuffer);
        f->offset += writeToNextBlock;
        eprom.write_page(0, (byte*)&directory);
        eprom.write_page(1, (byte*)&bitVector);
        
        return;
      }
    }
  }
}

//void FS::readFile(FCB*f)
//
// prints out the junk in the dumb file
void FS::readFile(FCB*f){
  Serial << "File: " << f->fileName << " contents ";
  char dataBuffer[64];
  char output[1024];
  for(int i=0;i<16;i++){
    if(f->dataBlock[i] != 0){
      int val = f->dataBlock[i];
      eprom.read_page(f->dataBlock[i], dataBuffer);
      Serial << dataBuffer << endl;
    }
  }

}
// void FS::fileSeek(FCB *f)
//
// displays something
void FS::fileSeek(FCB *f) {
  char dataBuffer[64];
  eprom.read_page(f->dataBlock[0], dataBuffer);

  Serial << "File: " << f->fileName << " data: " << dataBuffer << endl;
}

//void FS::deleteFile(char fileName[])
//
// Takes in a file name and sets the bit
// associated with file to 1 and sets
// the directory index to -1
void FS::deleteFile(char fileName[]) {
  int directoryBlock = 0;
  for (int i = 0; i < DIRECTORY_SIZE; i++) {
    directoryBlock = directory[i];
    eprom.read_page(directoryBlock, (byte*)&fcb);

    int result = strcmp(fileName, fcb.fileName);
    if (result == 0) {
      clearBit(directoryBlock);
      directory[i] = -1;

      Serial << fileName << " has been deleted " << endl;
      eprom.write_page(0, (byte*)&directory);
    }
  }
}

void FS::closeFile(FCB *f) {
  for (int i = 0; i < DIRECTORY_SIZE; i++) {
    int x = directory[i];
    if (x != -1) {
      eprom.read_page(x, (byte*)&fcb);
      int val = strcmp(f->fileName, fcb.fileName);
      if (val == 0) {
        Serial << f->fileName << " closed" << endl;
        eprom.write_page(x, (byte*)f);
        break;
      }

    }
  }

}

//int FS::findBit(byte bytes)
//
// takes in a byte and finds
// an avaible bit and returns
// the byte value
int FS::findBit(byte bytes) {
  for (int i = 0; i < 8; i++) {
    byte result = bytes & bitMasks[i];
    if (result > 0) {
      return i;
    }
  }
}

//void FS::clearBit(int value)
//
// takes in int value and sets
// the corresponding bit to 1
void FS::clearBit(int value) {
  int byteValue = value / 8;
  int bitValue = value % 8;

  //    byte y = ~here & bitMasks[4];
  //    byte res = here | y;
  //    Serial.flush();
  //    Serial.println(res,BIN);

  byte x = bitVector[byteValue];
  byte y = ~x & bitMasks[bitValue];
  byte z = x | y;

  bitVector[byteValue] = z;
}

//void FS::setBit(int value)
//
// takes in int valu and sets
// the corresponding bit to 0
void FS::setBit(int value) {
  int byteValue = value / 8;
  int bitValue = value % 8;

  byte byteToSet = bitVector[byteValue] & ~(bitMasks[bitValue]);
  bitVector[byteValue] = byteToSet;
}

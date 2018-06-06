#include "FS.h"
byte bitMasks[] = {0x01,0x02,0x03,0x04,0x10,0x20,0x30,0x40};
FS::FS(){
    fileCount = 0;
    for(int i=0;i<64;i++){
        bitVector[i] = 0;
    }
}

void FS::reformat(){
    for(int i=0;i<DIRECTORY_SIZE;i++){
        directory[i]=-1;
    }

    memset(bitVector,0xff,64);

    setBit(0);
    setBit(1);

    //eprom.write_page(0,(byte*)&directory);
    //eprom.write_page(1,(byte*)&bitVector);
}

void FS::initalize(){
  eprom.read_page(0,(byte*)&directory);
  eprom.read_page(1,(byte*)&bitVector);
}

void FS::list(){
  eprom.read_page(0,(byte*)&directory);
    for(int i=0;i<DIRECTORY_SIZE;i++){
      
      int x = directory[i];
      if(x == -1){
        continue;
      }
      
      eprom.read_page(x,(byte*)&fcb);
      Serial << "File: " <<fcb.fileName << " "<< sizeof(fcb) << " bytes"<<endl;
    }
}

void FS::create(char fileName[]){
  setBit(0);
  setBit(1);
    fileCount++;

    if(fileCount > DIRECTORY_SIZE){
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
    for(int k=0;k<DIRECTORY_SIZE;k++){
        if(directory[k] == -1){
            freeDirectorySpace = k;
            break;
        }
    }

    int freeBlock=0;
    int value=0;
    int byteWithFreeBit = 0;
    for(int i=0;i<64;i++){
        value = findBit(bitVector[i]);
        freeBlock = (i*8)+value;
        byteWithFreeBit =i;
        break;
    }

    strcpy(fcb.fileName,fileName);
    setBit(freeBlock);
    directory[freeDirectorySpace] = freeBlock;
    eprom.write_page(freeBlock,(byte*)&fcb);

    eprom.write_page(0,(byte*)&directory);
    eprom.write_page(1,(byte*)&bitVector);

    Serial <<"File created: " << fileName << endl;
}

void FS::openFile(char fileName[], FCB *f){
  
 for(int i=0;i<DIRECTORY_SIZE;i++){
    int directoryBlock = directory[i];
    eprom.read_page(directoryBlock,(byte*)&fcb);
    int result = strcmp(fcb.fileName,fileName);
    if(result == 0){
      eprom.read_page(directoryBlock, (byte*)f);
    }
  }
}

void FS::writeFile(FCB *f,char data, int dataSize){
    int freeBlock=0;
    int value=0;
    int byteWithFreeBit = 0;
    for(int i=0;i<64;i++){
        value = findBit(bitVector[i]);
        freeBlock = (i*8)+value;
        byteWithFreeBit =i;
        break;
    } 
  
}

void FS::deleteFile(char fileName[]){

}

void FS::closeFile(char fileName[]){
  
}

int FS::findBit(byte bytes){
  for(int i=0;i<8;i++){
    byte result = bytes & bitMasks[i];
    if(result > 0){
      return i;
    }
  }
}

void FS::clearBit(int value){
    int byteValue = value/8;
    int bitValue = value%8;
   
//    byte y = ~here & bitMasks[4];
//    byte res = here | y;
//    Serial.flush();
//    Serial.println(res,BIN);

    byte x = bitVector[byteValue];
    byte y = ~x & bitMasks[bitValue];
    byte z = x | y;

    bitVector[byteValue] = z;
}

void FS::setBit(int value){
   // Serial << "setting: " << value << endl;
    int byteValue = value/8;
    int bitValue = value%8;

    byte byteToSet = bitVector[byteValue] & ~(bitMasks[bitValue]);
    bitVector[byteValue] = byteToSet;
}

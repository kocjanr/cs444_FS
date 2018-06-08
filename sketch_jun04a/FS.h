#include <Microchip_24LC256.h>

#define BLOCK_SIZE 64
#define DIRECTORY_SIZE 32
#define BIT_VECTOR_SIZE 64
#define MAX_FILE_SIZE 1024
#define BIT_VECTOR_BITS 512
#define FREE_BIT 1
#define USED_BIT 0
#include <string.h>
#include <Streaming.h>

using namespace std;

struct FCB{
    char fileName[30];
    int offset = 0;
    int dataBlock[16] = {-1};
};

class FS{
    public:
        FS();
        void reformat();
        void initalize();
        void list();
        void create(char fileName[]);
        void deleteFile(char fileName[]);
        void closeFile(FCB *f);
        void openFile(char fileName[], FCB *f);
        void writeFile(FCB *f,char data[], int dataSize);
        void readFile(FCB *f);
        void fileSeek(FCB *f);
        
        Microchip_24LC256 eprom;
    private:
        void clearBit(int value);
        void setBit(int value);
        int findBit(byte bytes);

        byte bitVector[64];
        signed int directory[32];
        int fileCount;
        int freeBlock;
        FCB fcb;         
};

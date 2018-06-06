#include <Streaming.h>
#include "FS.h"

void setup() {
  Serial.begin(115200);
  FS fileSystem;
  FCB x;

  
  fileSystem.initalize();
  fileSystem.reformat();
  
  fileSystem.create("file.txt");
  fileSystem.create("hellow.txt");
  fileSystem.create("junk.txt");
  fileSystem.create("file.txt");
  fileSystem.list();
  
  fileSystem.openFile("file.txt",&x);
  Serial << "x file name:: ";
  Serial << x.fileName << endl;
}

void loop() {

}

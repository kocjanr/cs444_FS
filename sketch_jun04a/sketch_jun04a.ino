#include <Streaming.h>
#include "FS.h"

void setup() {
  Serial.begin(115200);
  FS fileSystem;
  FCB x;

  //reformat and initalize
  fileSystem.reformat();
  fileSystem.initalize();
  
  //create some files
  fileSystem.create("top_secret_stuff.txt");
  fileSystem.create("bottom_secret_stuff.txt");
  fileSystem.create("do_not_open.txt");
  fileSystem.create("sweet_car_pics.txt");
  Serial.println();

  //list files
  fileSystem.list();
  Serial.println();

  //open a file
  fileSystem.openFile("top_secret_stuff.txt",&x);
  Serial << x.fileName << " loaded in from eprom" << endl;
  Serial.println();

  //wite to file "top_secret_stuff.txt"
  fileSystem.writeFile(&x,"mic check number 1",18);
  Serial.println();
  fileSystem.fileSeek(&x);
  Serial.println();
  
  fileSystem.writeFile(&x,"isnt this a lot of fun?",23);
  Serial.println();
  fileSystem.fileSeek(&x);
  Serial.println();

  
  //close a file
  //fileSystem.closeFile(&x);
  //Serial.println();
  //fileSystem.list();

  //delete a file
  //fileSystem.deleteFile("top_secret_stuff.txt");
  //fileSystem.list();

   //fileSystem.reformat();
  
}

void loop() {

}

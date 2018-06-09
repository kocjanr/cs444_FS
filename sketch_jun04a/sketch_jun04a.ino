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
  
  fileSystem.writeFile(&x,"isnt this a lot of fun?",23);

  //seek to front of file
  fileSystem.fileSeek(&x);
  Serial.println();

  fileSystem.writeFile(&x,"we are spinal tap from the uk, you must be the usa",50);
  Serial.println();
  fileSystem.readFile(&x);

  //close a file
  fileSystem.closeFile(&x);
  Serial.println();
  fileSystem.list();
  Serial.println();

  //delete a file
  fileSystem.deleteFile("top_secret_stuff.txt");
  fileSystem.list();

   fileSystem.reformat();
  
}

void loop() {

}

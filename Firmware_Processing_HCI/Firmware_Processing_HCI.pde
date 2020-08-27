import processing.serial.*;
import processing.sound.*;

Serial myPort;  // Create object from Serial class
int val;        // Data received from the serial port
int layout = 1;
boolean stopped = true;
int time_previous = 0;
int time_recorded = 0;
int buttons_number = 0;
SoundFile buzzer;

void setup() 
{
  size(1100, 500);
  buzzer = new SoundFile(this, "beep.wav");
  // I know that the first port in the serial list on my mac
  // is always my  FTDI adaptor, so I open Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
  printArray(Serial.list());
  String portName = Serial.list()[1];
  myPort = new Serial(this, portName, 115200);
}

void draw() {
  background(255);
  if(mousePressed){
      if(layout < 2){
        fill(128 - (mouseOverUp()*100));
        rect(200, 100, 100, 100,10);         // Draw a square
        fill(128 - (mouseOverRight()*100));
        rect(300, 200, 100, 100,10);         // Draw a square
        fill(128 - (mouseOverLeft()*100));
        rect(100, 200, 100, 100,10);         // Draw a square
        fill(128 - (mouseOverDown()*100));
        rect(200, 300, 100, 100,10);         // Draw a square
      }
      if(layout > 0){
        fill(128 - (mouseOverUp2()*100));
        rect(710, 200, 100, 100,10);         // Draw a square
        fill(128 - (mouseOverLeft2()*100));
        rect(600, 200, 100, 100,10);         // Draw a square
        fill(128 - (mouseOverRight2()*100));
        rect(930, 200, 100, 100,10);         // Draw a square
        fill(128 - (mouseOverDown2()*100));
        rect(820, 200, 100, 100,10);         // Draw a square
      }
   }else{
      fill(128);
      if(layout < 2){
        rect(200, 100, 100, 100,10);         // Draw a square
        rect(100, 200, 100, 100,10);         // Draw a square
        rect(300, 200, 100, 100,10);         // Draw a square
        rect(200, 300, 100, 100,10);         // Draw a square
      }
      if(layout > 0){
        rect(600, 200, 100, 100,10);         // Draw a square
        rect(710, 200, 100, 100,10);         // Draw a square
        rect(820, 200, 100, 100,10);         // Draw a square
        rect(930, 200, 100, 100,10);         // Draw a square
      }
   }
   if(stopped){
     fill(255,0,0);
     rect(450, 200, 100, 100,100);
     textSize(32);
     textAlign(CENTER);
     text(time_recorded,500,350);
     text(buttons_number,500,380);
   }
}

int mouseOverUp() { // Test if mouse is over square
  if((mouseX >= 200) && (mouseX <= 300) && (mouseY >= 100) && (mouseY <= 200)) return 1;
  else return 0;
}

int mouseOverRight() { // Test if mouse is over square
  if((mouseX >= 300) && (mouseX <= 400) && (mouseY >= 200) && (mouseY <= 300)) return 1;
  else return 0;
}
int mouseOverLeft() { // Test if mouse is over square
  if((mouseX >= 100) && (mouseX <= 200) && (mouseY >= 200) && (mouseY <= 300)) return 1;
  else return 0;
}
int mouseOverDown() { // Test if mouse is over square
  if((mouseX >= 200) && (mouseX <= 300) && (mouseY >= 300) && (mouseY <= 400)) return 1;
  else return 0;
}

int mouseOverLeft2() { // Test if mouse is over square
  if ((mouseX >= 600) && (mouseX <= 700) && (mouseY >= 200) && (mouseY <= 300)) return 1;
  else return 0;
}
int mouseOverUp2() { // Test if mouse is over square
  if ((mouseX >= 710) && (mouseX <= 810) && (mouseY >= 200) && (mouseY <= 300)) return 1;
  else return 0;
}
int mouseOverDown2() { // Test if mouse is over square
  if ((mouseX >= 820) && (mouseX <= 920) && (mouseY >= 200) && (mouseY <= 300)) return 1;
  else return 0;
}
int mouseOverRight2() { // Test if mouse is over square
  if ((mouseX >= 930) && (mouseX <= 1030) && (mouseY >= 200) && (mouseY <= 300)) return 1;
  else return 0;
}


void serialEvent(Serial myPort) {
  // read a byte from the serial port:
  char inByte = (char)myPort.read();
  print(inByte);
}

void stop() {
  myPort.clear();
  // Close the port
  myPort.stop();
}

void mouseClicked() {
  if(mouseOverUp()==1 || mouseOverUp2()==1){
    myPort.write("8");
    buzzer.play();
    buttons_number++;
  }
  if(mouseOverRight()==1 || mouseOverRight2()==1){
    myPort.write("6");
    buzzer.play();
    buttons_number++;
  }
    if(mouseOverLeft()==1 || mouseOverLeft2()==1){
    myPort.write("4");
    buzzer.play();
    buttons_number++;
  }
  if(mouseOverDown()==1 || mouseOverDown2()==1){
    myPort.write("2");
    buzzer.play();
    buttons_number++;
  }
}

void keyPressed() {
  if (key == 'c' || key == 'C') {
    layout = (layout+1)%3;
  }
  if (key == 's' || key == 'S') {
    stopped = !stopped;
    if(stopped){
      time_recorded = millis() - time_previous;
    }else{
      time_previous = millis();
      buttons_number = 0;
    }
  }
}

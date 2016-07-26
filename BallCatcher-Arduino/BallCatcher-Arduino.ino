/////////////////////////////////////////////////////////////////////////////
// BALL CATCHER
// This program controls a 2 DOF Cartesian Coordinate robot with
// values it receives from a computer over the serial connection.
//////////////////////////////////////////////////////////////////////////////

//Pins
int directX = 8;
int stepX = 9;
int stepY = 11;
int directY =10;

//Current coordinates
int xCoord = 0;
int yCoord = 0;

//X and Y coordinates from computer
int setX;
int setY;

char intBufferX[12];
String intDataX = "";

char intBufferY[12];
String intDataY = "";

int ch = 0;
bool isX = true;

long startCount = 0;
  
void setup() {
  Serial.begin(115200);
  pinMode(directX, OUTPUT);
  pinMode(stepX, OUTPUT);
  pinMode(directY, OUTPUT);
  pinMode(stepY, OUTPUT);

  while (true){
    if (Serial.available()){
      ch = Serial.read();
      if (ch == (int)'-') break; // check for priming character
    }
    delay(1);
  }
  Serial.print("#"); // send response
}

void loop() {
  if(Serial.available()){
    isX = true;
    while (Serial.available()){
        ch = Serial.read();
        if (ch == -1) {
           break;
        }
        else if (ch == (int)'x') {
            if (isX)
              isX = false;
            else
              break;
        }
        else if (isX) {
            intDataX += (char) ch;
        } else {
            intDataY += (char) ch;
        }
        delay(1);
    }
    intDataX.toCharArray(intBufferX, intDataX.length() + 1);
    intDataX = "";
    setX = 2150 - atoi(intBufferX);

    if (abs(setX-xCoord) < 40) setX = xCoord; // movement buffer

    intDataY.toCharArray(intBufferY, intDataY.length() + 1);
    intDataY = "";
    setY = 1975 - atoi(intBufferY);

    if (abs(setY-yCoord) < 40) setY = yCoord;

    /*Serial.print("X COORDINATE SET TO:");
    Serial.print(setX);
    Serial.print("\n");
    Serial.print("Y COORDINATE SET TO:");
    Serial.print(setY);
    Serial.print("\n");*/

    startCount = 0;
  }
  
  if (abs(setX-xCoord) > 0){
    if (setX-xCoord > 0){
      digitalWrite(directX, HIGH);
      xCoord++;
    }
    else {
      digitalWrite(directX, LOW);
      xCoord--;
    }
    
    digitalWrite(stepX, HIGH);
  }
  
  if (abs(setY-yCoord) > 0){
    if (setY-yCoord > 0){
      digitalWrite(directY, HIGH);
      yCoord++;
    }
    else {
      digitalWrite(directY, LOW);
      yCoord--;
    }
    
    digitalWrite(stepY, HIGH);
  }

  delayMicroseconds( 250*(1+7*pow(2,-startCount/20)) ); // speed ramp function
  //delayMicroseconds(1000);

  digitalWrite(stepX, LOW);
  digitalWrite(stepY, LOW);

  startCount++;
}


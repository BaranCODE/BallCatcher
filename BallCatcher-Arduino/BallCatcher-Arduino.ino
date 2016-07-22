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
    if (Serial.available() && Serial.readString().equals("ok")) break;
    delay(10);
  }
  Serial.print("READY\n");
}

void loop() {  
  // put your main code here, to run repeatedly:
  if(Serial.available()!=0){
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
    setX = atoi(intBufferX);

    intDataY.toCharArray(intBufferY, intDataY.length() + 1);
    intDataY = "";
    setY = atoi(intBufferY);

    Serial.print("X COORDINATE SET TO:");
    Serial.print(setX);
    Serial.print("\n");
    Serial.print("Y COORDINATE SET TO:");
    Serial.print(setY);
    Serial.print("\n");

    startCount = 0;
  }
  
  if (abs(setX-xCoord) >0){
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

  delayMicroseconds( 250*(1+7*pow(2,-startCount/50)) ); // speed ramp function

  digitalWrite(stepX, LOW);
  digitalWrite(stepY, LOW);

  startCount++;
}


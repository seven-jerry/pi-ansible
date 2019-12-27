#include <SoftwareSerial.h>
SoftwareSerial inputSerial(10, 11);


int SEND_DELAY = 10;
int WRITE_DELAY = 1;

//empty output
int EO = 0;
int NOOP = 7;


int light_x = 6;
int light_y = 7;


int lights [6][7] = {
  {44,16,30,28,26,43,25},
  {4,37,33,3,24,21,EO},
  {45,9,38,39,8,22,23},
  {6,29,2,27,7,20,EO},
  {42,5,34,40,32,12,18},
  {17,36,41,31,15,19} 
  };

int state [6][7] = {
  {0,0,0,0,0,0,0},
  {0,0,0,0,0,0,NOOP},
  {0,0,0,0,0,0,0},
  {0,0,0,0,0,0,NOOP},   
  {0,0,0,0,0,0,0},
  {0,0,0,0,0,0,NOOP}
};

int inverseMask [6][7] = {
  {0,1,0,1,0,1,0},
  {1,0,1,1,0,1,0},
  {0,1,0,1,0,1,0},
  {1,0,1,1,0,1,0},   
  {0,1,0,1,0,1,0},
  {1,0,1,1,0,1,0}
};


int inputsCount = 2;
int inputs[] = {46,47};
int input_state[] = {0,0};

int outSidesCount = 0;
int outSides[] = {};
int outSideState[] = {};

String commandQuery = "";


void setup() {
  Serial.begin(115200);
   while (!Serial) {
  }
  inputSerial.begin(9600);
  initPins();

}

void initPins(){
 for (int i = 0; i < light_x; i = i + 1) {
     for(int j = 0;j < light_y;j++){
         pinMode(lights[i][j],OUTPUT);
         toLow(i,j);          
         delay(10);
     }
  }

  for(int i =0;i < inputsCount;i++){
    pinMode(inputs[i],INPUT);
    delay(10);
  }
  
  for(int i =0;i < outSidesCount;i++){
    pinMode(outSides[i],OUTPUT);
    delay(10);
  }
}


void loop() {
  checkInputs();
  
  if(recieveStatus()){
      String appliedState = "{";
      appliedState.concat(proccessStatusString());
      commandQuery = "";
      if(appliedState.length() > 1){
        appliedState.concat("}");
        Serial.print(appliedState);
        delay(SEND_DELAY);
      }
  }
  checkInputs();
}



/**
 * 
 * 
 *   User Commands
 * 
 */

// gets a command for the input source
// expected format is {command}
boolean recieveStatus(){
  char character;
  
  String content = "";
  while(inputSerial.available()) {
      character = inputSerial.read(); 
      content.concat(character);
  }  
  int startBracket = content.indexOf('{');
  if(startBracket > -1 ){
     commandQuery = content;
     return false;
  }
  commandQuery += content;
  return commandQuery.indexOf('}') != -1;
}


String proccessStatusString(){
   int startBracket = commandQuery.indexOf('{');
  int endBracket = commandQuery.indexOf('}');
  commandQuery = commandQuery.substring(startBracket+1,endBracket);


  int len = commandQuery.length();
  int counter = 0;

  while(counter <= len){
    if(commandQuery.charAt(0) == 's'){
      return readCommandString(commandQuery);
    }
    int x = (int)commandQuery.charAt(counter + 0) - '0';
    int y = (int)commandQuery.charAt(counter + 1) - '0';
    int s = (int)commandQuery.charAt(counter + 2) - '0';
    if(s == 0){
       toLow(x,y);
    }
    if(s == 1){
       toHigh(x,y);
    }
    counter += 3;
  }
  return commandQuery;
}

String readCommandString(String command){
  if(command == "sah"){
       turnAllOn(); 
       return command;
  }
  else if(command == "sal"){
    turnAllOff();
    return command;
  }
  else if(command == "shh"){
    turnHalfOn();
    return command;
  }  
  else if(command == "smh"){
    turnAllOff();
    turnMotionOn();
    return command;
  }
  else if(command == "soh"){
    turnOutsideHigh();
    return command;
  }
  else if(command == "sol"){
    turnOutsideLow();
    return command;
  }

  else if(command == "shb"){
    fullOutputStatus();
  }
  else if(command == "sin"){
    fullInputStatus();
  }

  return "";
}


/**
 * 
 * 
 *     Inputs
 * 
 */

void checkInputs(){
  String sender = "{i";
 boolean has_sender = false;
  for(int i = 0; i < inputsCount; i++){
    int state = digitalRead(inputs[i]);
    if(state == 0){
      state = 1;
    }
    if(state == 1){
      state = 0;
    }
    if(state != input_state[i]){
      input_state[i] = state;
      sender.concat(i);
      sender.concat(state);
      has_sender = true;
    }   
  }
  if(has_sender){
    sender.concat("}");
    Serial.print(sender);
    delay(SEND_DELAY);
  }

}



void fullOutputStatus(){
  String sender = "{o";
  for (int i = 0; i < light_x; i = i + 1) {
     for(int j = 0;j < light_y;j++){
          sender.concat(i);
          sender.concat(j);
         sender.concat(state[i][j]);
     }
  }
  sender.concat('u');  
  for(int i = 0; i < outSidesCount;i++){
     sender.concat(i);
     sender.concat(outSideState[i]);
  }

   sender.concat('i');
   for(int i = 0; i < inputsCount; i++){
    sender.concat(i);
    sender.concat(digitalRead(inputs[i]));
  }
  
  sender.concat("}");
  Serial.print(sender);
    delay(SEND_DELAY);
}

int fullInputStatus(){
  String sender = "{i";

  for(int i = 0; i < inputsCount; i++){
    sender.concat(i);
    sender.concat(digitalRead(inputs[i]));
  }

  sender.concat("}");
  Serial.print(sender);
    delay(SEND_DELAY);

}


/**
 * 
 *    State Changing Functions
 * 
 * */

void turnAllOn(){
   for (int i = 0; i < light_x; i = i + 1) {
     for(int j = 0;j < light_y;j++){
         toHigh(i,j);
     }
  } 
}

void turnMotionOn(){
   turnAllOff();
   turnHalfOn();
  }

void turnAllOff(){
   for (int i = 0; i < light_x; i = i + 1) {
     for(int j = 0;j < light_y;j++){
         toLow(i,j);
     }
  } 
}

void turnHalfOn(){ 
  turnAllOff();
  // turning all low will set inversed on
  for (int i = 0; i < light_x; i = i + 1) {
     for(int j = 0;j < light_y;j++){
        if(state[i][j] != NOOP){
          state[i][j] = 0;
          digitalWrite(lights[i][j],state[i][j]);
        } 
     }
  } 
  
}


void turnOutsideLow(){
   for (int i = 0; i < outSidesCount; i = i + 1) {
     digitalWrite(outSides[i],HIGH);
     outSideState[i] = 0;
   }
}
void turnOutsideHigh(){
   for (int i = 0; i < outSidesCount; i = i + 1) {
     digitalWrite(outSides[i],LOW);
     outSideState[i] = 1;
   }
}


void toLow(int i,int j){
    int light = lights[i][j];
    int currState = state[i][j];
    int inverse = inverseMask[i][j];
    if(currState == NOOP){
      return;
    }
    state[i][j] = 0;
    if(inverse == 1){
       digitalWrite(light,HIGH);
       delay(10);
       return;
    }
    digitalWrite(light,LOW); 
    delay(10); 
}

void toHigh(int i,int j){
    int light = lights[i][j];
    int currState = state[i][j];
    int inverse = inverseMask[i][j];
    if(currState == NOOP){
      return;
    }
    state[i][j] = 1;
    if(inverse == 1){
       digitalWrite(light,LOW);
       delay(WRITE_DELAY);
       return;
    }
    digitalWrite(light,HIGH); 
    delay(WRITE_DELAY);
 
}

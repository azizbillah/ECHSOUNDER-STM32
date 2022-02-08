#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);
const byte buttonSTN = 2;
const byte buttonXDR = 3;

int xdr;
int dbt;
int stn;


String dt[16];
String dat[4];
boolean parsing=false;
boolean parsing1=false;
boolean draft_flag = false;
String data,dataEcho,cmd;
const byte buff_len = 90;
char CRCbuffer[buff_len];
float draft = 0.0;
int flag_data = 0;
int status_Interface_Thirdparty = 0;
int status_thirdparty = 0;
void setup() {
  Serial.begin(38400);
  mySerial.begin(38400);
  pinMode(buttonXDR,INPUT);
  pinMode(buttonSTN,INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonXDR), stateXDR, RISING);
  attachInterrupt(digitalPinToInterrupt(buttonSTN), stateSTN, RISING);
}

void stateXDR()   {flag_data = 1;}
void stateSTN()   {flag_data = 2;}

void loop() {
  while (mySerial.available()) {
  char inChar2 = (char)mySerial.read();
//    if        ( inChar2 == 'A' ) { flag_data = 1;draft_flag = false;}
//    else if   ( inChar2 == 'C' ) { flag_data = 2;draft_flag = false;}
//    else if   ( inChar2 == 'D' ) { draft = data.toFloat();
//                                   data ="";}
//    else if   ( inChar2 == 'J')  { status_Interface_Thirdparty = data.toInt();
//                                  data = "";}
//    else {data.concat(inChar2); draft_flag = false;}
//    //parsing1 = true;

    switch(inChar2)
    {
      case 'A' :  flag_data = 1;
                  break;
      case 'C' :  flag_data = 2;
                  break;
      case 'D' :  draft = data.toFloat();
                  data = "";
                  break;
      case 'J' :  status_Interface_Thirdparty = data.toInt();
                  data = "";
                  break;
      default : data.concat(inChar2);
                  //data ="";
    }
  }
  
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    dataEcho += inChar;
    if (inChar == '\n') {parsing = true;}
  }
  if(parsing){
    parsingData();
    dataEcho="";
    data= "";
    parsing=false;
    
  }
}
void parsingData(){
  int j=0;
  int k=0;
  dt[j]="";
  int minLength;
  if (dataEcho.length()>50) {minLength = 20;}
  else                      {minLength = 0;}//dataEcho.length()-minLength
  for(k=1; k < dataEcho.length(); k++){
    if ((dataEcho[k] == '$') || (dataEcho[k] == ',')|| (dataEcho[k] == '*')){
      j++;
      dt[j]="";
    } 
    else { dt[j] = dt[j] + dataEcho[k];}
  }
 
  if (dt[0]=="SDXDR"){
    float depth_H = dt[2].toFloat()+draft;
    float depth_L = dt[6].toFloat()+draft;
    if (flag_data==1){
      
      String msg={"$SDXDR,D," + String(depth_H) + ",M,XDHI,D,"  + String(depth_L) + ",M,XDLO,C,"+ dt[10] + ",C,WTHI*"};
      outputMsg(msg);
      memset(CRCbuffer,0,sizeof(CRCbuffer)); //clear data
    }
    else if (flag_data==2) {
      
      float DA = dt[2].toFloat()+draft;
      float DB = dt[6].toFloat()+draft;
      Serial.print("DA0000"); Serial.print(DA);Serial.println(" m");
      Serial.print("DB0000"); Serial.print(DB);Serial.println(" m");

      if (status_Interface_Thirdparty == 1 or status_Interface_Thirdparty == 2 or status_Interface_Thirdparty == 3) {Serial.println("*");}

    }
    else if (flag_data==0){;}
  }
  else if (dt[0]=="SDDBT"){
    float depth_f = dt[1].toFloat()+ draft;
    float depth_M = dt[3].toFloat()+ draft;
    float depth_F = dt[5].toFloat()+ draft;
    cmd = "$SDDBT," + String(depth_f) + ",f," + String(depth_M) + ",M," + String(depth_F) + ",F*";
    outputMsg(cmd);
    memset(CRCbuffer,0,sizeof(CRCbuffer)); //clear data
  }
}

void outputMsg(String msg) {
  msg.toCharArray(CRCbuffer,sizeof(CRCbuffer));
  byte crc = convertToCRC(CRCbuffer);
  Serial.print(msg);                 //  Serial.print(msg);
  if (crc < 16) { Serial.print("0");}//Serial.print("0");}
  Serial.println(crc,HEX);           //Serial.println(crc,HEX);
}

byte convertToCRC(char *buff) {
  char c;
  byte i;
  byte start_with = 0;
  byte end_with = 0;
  byte crc = 0;

  for (i = 0; i < buff_len; i++) {
    c = buff[i];
    if(c == '$') {start_with = i;}
    if(c == '*') {end_with = i;}      
  }
  if (end_with > start_with){
    for (i = start_with+1; i < end_with; i++){ crc = crc ^ buff[i] ;}
  }
  else {
    Serial.println("CRC ERROR"); //Serial.println("CRC ERROR");
    return 0;
  }
  return crc;
}

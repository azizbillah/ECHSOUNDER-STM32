//#include <IWatchdog.h>


//Modify this code as you see fit.
//This code will output data to the Arduino serial monitor.
//This code will allow you to control up to 8 Atlas Scientific devices through 1 serial port line.
//This code was written in the Arduino 1.8.9 IDE
//An Arduino MEGA was used to test this code.
//This code was last tested 9/2019


//To open a channel (marked on the board as P1 to P8) send the number of the channel followed by a colon and the command (if any) that you want to send. End the string with a carriage return.
//1:r<CR>
//2:i<CR>
//3:c<CR>
//4:r<CR>

//To open a channel and not send a command just send the channel number followed by a colon.
//1:<CR>
//3:<CR>

const byte buff_len = 90;
char CRCbuffer[buff_len];

char char_draft[100];
char char_sow[100];
char char_dfilter[100];
char char_sfilter[100];
char char_dblank[100];
int range =0;
String cmd;
//Led status
int led_dbt = PB7;
int led_stn = PB8;
int led_xdr = PB9;
int state_Anotasi_pin = PB5;


//Status third party

int status_third_party1 = 0;
int status_third_party2 = 0;
int status_third_party3 = 0;

//Echosounder setting
float  draft = 0;
int sow = 0;
int dfilter=4;;
int sfilter=4;;
int dblank =20;


//anotasi
int flag_anotasi =0;
int flag_input = 0;
int LED = PC13;
volatile int flagBtn = 0;
int delay_data = 100 ;
String data;




String inputstring1 =  "";                             //a string to hold incoming data from the PC
String inputstring2 = "";
String inputstring3 = "";                             //a string to hold the data from the Atlas Scientific product
boolean input_string_complete1 = false;                //have we received all the data from the PC
boolean input_string_complete2 = false;               //have we received all the data from the Atlas Scientific product
void setup() {
  pinMode(state_Anotasi_pin, OUTPUT);
  pinMode(led_dbt, OUTPUT);
  pinMode(led_stn, OUTPUT);
  pinMode(led_xdr, OUTPUT);
  digitalWrite(led_dbt, HIGH);
  digitalWrite(led_stn, LOW);
  digitalWrite(led_xdr, HIGH);
  digitalWrite(state_Anotasi_pin, HIGH);
  Serial1.begin(38400);                                 //Set the hardware serial port to 9600
  Serial2.begin(921600);                                //set baud rate for software serial port_3 to 9600
  Serial3.begin(921600);
  flagBtn = 0;
  delay(500);
  //Setting awal
  Serial1.println("$PAMTC,EN,L*0C"); delay(200);
  Serial1.println("$PAMTC,OPTION,SET,PING,OFF*55"); delay(200);
  Serial1.println("$PAMTC,OPTION,SET,OUTPUTMC,PING*0B"); delay(200);
  Serial1.println("$PAMTC,OPTION,SET,SLAVE,ON*46"); delay(200);
  Serial1.println("$PAMTC,OPTION,SET,SYNCMODE,MANUAL*14"); delay(200);
  Serial1.println("$PAMTC,OPTION,SET,PULSESPP,2,6,21,24,S*54"); delay(200);
  Serial1.println("$PAMTC,OPTION,SET,PULSESPP,2,10,50,60,M*7B"); delay(200);
  Serial1.println("$PAMTC,OPTION,SET,RANGE,0,S*1A"); delay(200);
  Serial1.println("$PAMTC,OPTION,SET,RANGE,0,M*04"); delay(200);
  Serial1.println("$PAMTC,EEC,SUBSET,FULL*31");  delay(200);
  Serial1.println("$PAMTC,OPTION,SET,SYNC,TS*0A"); delay(200);
  Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");delay(200);
  Serial1.println("$PAMTC,EN,ALL,0*1D\r\n");delay(200);
  Serial1.println("$PAMTC,EN,XDRX,1,10*66"); delay(200);
  Serial1.println("$PAMTC,EN,DPT,1,10*30");  delay(200);
// Serial1.println("$PAMTC,EN,MTW,1,10*3E");delay(200);
  Serial1.println("$PAMTC,EN,S*13\r\n");  
//  IWatchdog.begin(4000000);

}

void serialEvent1() {
    if(Serial2.available()){
    inputstring1 = Serial2.readStringUntil('\n');           //read the string until we see a <CR>
    input_string_complete1 = true;                       //set the flag used to tell if we have received a completed string from the PC
    }
   
}

void serialEvent3(){
    if(Serial3.available()){
    inputstring2 = Serial3.readStringUntil('\n');
    input_string_complete2 = true;   
    }
  
}

void loop() {

//  serialEvent1();
//  serialEvent3();
  
  if(Serial2.available()){
    inputstring1 = Serial2.readStringUntil(13);           //read the string until we see a <CR>
    input_string_complete1 = true;                       //set the flag used to tell if we have received a completed string from the PC
    }
  if(Serial3.available()){
    inputstring2 = Serial3.readStringUntil(13);
    input_string_complete2 = true;   
  }
  
  if(input_string_complete1 && input_string_complete2)
  {
    Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
    delay(400);
    inputstring1 = "";
    inputstring2 = "";
    input_string_complete1 = false;                   
    input_string_complete2 = false;
    
   
  } 
  
  command();
  

}

void edit_input(int c){

  if( c == 1 ){
      if        ( flag_anotasi == 0){ digitalWrite(state_Anotasi_pin,LOW);}
      else if   ( flag_anotasi == 1){ digitalWrite(state_Anotasi_pin,HIGH);}
  }

  else if ( c == 2){
      if        ( range == 1){Serial1.println("$PAMTC,OPTION,SET,RANGE,0,S*1A"); 
                              Serial1.println("$PAMTC,OPTION,SET,RANGE,0,M*04"); 
                              Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                              Serial1.println("$PAMTC,EN,S*13");}
      else if   ( range == 2){Serial1.println("$PAMTC,OPTION,SET,RANGE,1,S*1B"); 
                              Serial1.println("$PAMTC,OPTION,SET,RANGE,1,M*05"); 
                              Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                              Serial1.println("$PAMTC,EN,S*13");}
      else if   ( range == 3){Serial1.println("$PAMTC,OPTION,SET,RANGE,2,S*18"); 
                              Serial1.println("$PAMTC,OPTION,SET,RANGE,2,M*06"); 
                              Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                              Serial1.println("$PAMTC,EN,S*13");}
      else if   ( range == 4){Serial1.println("$PAMTC,OPTION,SET,RANGE,3,S*19"); 
                              Serial1.println("$PAMTC,OPTION,SET,RANGE,3,M*07"); 
                              Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                              Serial1.println("$PAMTC,EN,S*13");
                               }
      else if   ( range == 5){Serial1.println("$PAMTC,OPTION,SET,RANGE,4,S*1E"); 
                              Serial1.println("$PAMTC,OPTION,SET,RANGE,4,M*00"); 
                              Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                              Serial1.println("$PAMTC,EN,S*13");
                              }
  }

}

void command(){

  char buffer[100];
  while(Serial1.available())
  {

    char inChar = (char)Serial1.read();
   

    if                ( inChar == 'A') {  digitalWrite(led_dbt, HIGH);
                                          digitalWrite(led_stn, HIGH);
                                          digitalWrite(led_xdr, LOW);
                                          Serial1.println("$PAMTC,EN,ALL,0*1D\r\n");
                                          Serial1.println("$PAMTC,EN,XDRX,1,10*66");delay(200);
                                          Serial1.println("$PAMTC,EN,DPT,1,10*30"); delay(200);
                                          Serial1.println("$PAMTC,EN,S*13");delay(200);
                                          Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          }
    else if           ( inChar == 'B') {  digitalWrite(led_dbt, LOW);
                                          digitalWrite(led_stn, HIGH);
                                          digitalWrite(led_xdr, HIGH);
                                          Serial1.println("$PAMTC,EN,ALL,0*1D\r\n");
                                          Serial1.println("$PAMTC,EN,DBT,1,10*22"); delay(200);
                                          Serial1.println("$PAMTC,EN,DPT,1,10*30"); delay(200);
                                          Serial1.println("$PAMTC,EN,MTW,1,10*3E"); delay(200);
                                          Serial1.println("$PAMTC,EN,S*13");delay(200);
                                          Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          }
    else if           ( inChar == 'C') {  digitalWrite(led_dbt, HIGH);
                                          digitalWrite(led_stn, LOW);
                                          digitalWrite(led_xdr, HIGH);
                                          Serial1.println("$PAMTC,EN,ALL,0*1D\r\n");
                                          Serial1.println("$PAMTC,EN,XDRX,1,10*66"); delay(200);
                                          Serial1.println("$PAMTC,EN,DPT,1,10*30");  delay(200);
                                          Serial1.println("$PAMTC,EN,S*13"); delay(200);
                                          Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          }
    else if           ( inChar == 'D') {  draft=data.toFloat();
                                          data="";
                                          int c_draft = draft*1000;
                                          dtostrf(draft,3,2,char_draft);
                                          sprintf(buffer,"$PAMTC,OPTION,SET,DOFFSET,%d*",c_draft);
                                          cmd = buffer;
                                          outputMsg(cmd); delay(200);
                                          Serial1.println("$PAMTC,EN,S*13");
                                          Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          memset(CRCbuffer,0,sizeof(CRCbuffer)); //clear data
                                          }
    else if           ( inChar == 'E') {  sow=data.toInt();
                                          data="";
                                          String str = String(sow);
                                          str.toCharArray(char_sow,5);
                                          int c_sow = sow*10;
                                          sprintf(buffer, "$PAMTC,OPTION,SOSTW,%d*",c_sow);
                                          cmd= buffer;
                                          outputMsg(cmd); delay(200);
                                          Serial1.println("$PAMTC,EN,S*13"); delay(200);
                                          Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          memset(CRCbuffer,0,sizeof(CRCbuffer)); //clear data
                                          }  
    else if           ( inChar == 'F') {  dfilter=data.toInt(); 
                                          data="";
                                          String str = String(dfilter);
                                          str.toCharArray(char_dfilter,2);
                                          sprintf(buffer,"$PAMTC,OPTION,SET,DFILTER,%d*",dfilter);
                                          cmd = buffer;
                                          outputMsg(cmd); delay(200);
                                          Serial1.println("$PAMTC,EN,S*13");delay(200);
                                          Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          memset(CRCbuffer,0,sizeof(CRCbuffer)); //clear data
                                          }
    else if           ( inChar == 'G') {  sfilter=data.toInt();
                                          data = "";
                                          String str = String(sfilter);
                                          str.toCharArray(char_sfilter,2);
                                          sprintf(buffer,"$PAMTC,OPTION,SET,SFILTER,%d*",sfilter);
                                          cmd = buffer;
                                          outputMsg(cmd); delay(200);
                                          Serial1.println("$PAMTC,EN,S*13"); delay(200);
                                          Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          memset(CRCbuffer,0,sizeof(CRCbuffer)); //clear data 
                                          }
    else if           ( inChar == 'H') {  dblank=data.toInt();
                                          data="";
                                          char str_tenth[6];
                                          String str1 = String(dblank);
                                          str1.toCharArray(char_dblank,4);
                                          float float_tenth = (float)dblank/10;
                                          dtostrf(float_tenth, 3, 1, str_tenth);
                                          sprintf(buffer,"$PAMTC,OPTION,SET,DBLANK,%s*",str_tenth);
                                          cmd = buffer;
                                          outputMsg(cmd);delay(200);
                                          Serial1.println("$PAMTC,EN,S*13"); delay(200);
                                          Serial1.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          memset(CRCbuffer,0,sizeof(CRCbuffer)); //clear data
    }    
    else if           ( inChar == 'K') {  flag_anotasi=data.toInt();
                                          edit_input(1);
                                          data="";    
                                          }    
    else if           ( inChar == 'M') {  range=data.toInt();
                                          edit_input(2);
                                          data="";}      
    else if           ( inChar == 'S') {
                                          Serial1.println("$PAMTC,OPTION,SET,SYNC,TS*0A"); delay(200);
                                          data="";
                                          }                                                                                                                   
    else{data.concat(inChar);}
    
    
  }
}

void outputMsg(String msg) {
  msg.toCharArray(CRCbuffer,sizeof(CRCbuffer));
  byte crc = convertToCRC(CRCbuffer);
  Serial1.print(msg);                //Serial.print(msg);
  if (crc < 16) {Serial1.print("0"); }//Serial.print("0");}
  Serial1.println(crc,HEX);          //Serial.println(crc,HEX);
}

byte convertToCRC(char *buff) {
  char c;
  byte i;
  byte start_with = 0,end_with = 0;
  byte crc = 0;

  for (i = 0; i < buff_len; i++) {
    c = buff[i];
    if(c == '$') {start_with = i;}
    if(c == '*') {end_with = i;}      
  }
  if (end_with > start_with){
    for (i = start_with+1; i < end_with; i++){ crc = crc ^ buff[i] ;}
  }
  else {//Serial1.println("CRC ERROR");
    return 0;
  }
  return crc;
}

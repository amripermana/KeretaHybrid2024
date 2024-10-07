#include "functions.h"

//Library
Mudbus Mb;
IPAddress ip(192, 168, 137, 5);
EthernetServer server(502);
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAA};

//General Variables
bool dcdc_stat = false;
bool vvvf_stat = false;
bool bms_stat = false;
int tombol1 = 24;
int tombol2 = 22;
int tombol3 = 23;
int tombol4 = 25;
int DCK_catenary = 2;
int DCK_battery = 29;
int DCK_load = 39;
int master_control = 7;
unsigned long timer_10ms = millis();
unsigned long timer_500ms = millis();
unsigned long timer_1000ms = millis();
unsigned long overcurrent_time = millis();
bool toggle = false;
bool proteksi = false;
bool overcurrent_state = false;


//BMS Variables
float soc = 0;
float soh = 0;
float bms_tegangan = 0;
float bms_arus = 0;
float bms_suhu = 0;

//VVVF Variables
float vvvf_speed = 0;
float vvvf_torsi = 0;

//DCDC Variables
float dcdc_arus_in = 0;
float dcdc_arus_out = 0;
float dcdc_volt_in = 0;
float dcdc_volt_out = 0;

//Sensor Variables
float dcpt_catenary = 0;
float dcct_catenary = 0;
float dcpt_battery = 0;
float dcct_battery = 0;

//LCD Variables
int lcd_page = 0;

//tombol 22, 23, 24 25
void init_display(){
  lcd.begin(20, 4);
  lcd.setCursor(0,0);
  lcd.print("Hybrid Control Unit");
  lcd.setCursor(3,1);
  lcd.print("Kereta Hybrid");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Devices Booting...");
  delay(1000);
  lcd.clear();
}

void init_MC(){
  pinMode(42, INPUT);
  pinMode(44, INPUT);
  pinMode(46, INPUT);
  pinMode(48, INPUT);
  pinMode(30, INPUT);
  pinMode(32, INPUT);
  pinMode(34, INPUT);
  pinMode(36, INPUT);
  pinMode(38, INPUT);
  pinMode(40, INPUT);
}


void devices_boot(){
  lcd.setCursor(0,0);
  lcd.print("Devices Status :");

  boot_bms();
  boot_dcdc();
  boot_vvvf();
  
}

void boot_bms(){
  unsigned long start_time = millis();
  String Load = "BMS : .";
  while((millis()-start_time) < 3000){
    frame.id = 0x0CF12200;
    frame.length = 8;
    frame.data.low = 0x00000001;
    frame.data.high = 0x00;
    frame.extended = 1;
    CAN.sendFrame(frame);
    
    if(Can0.available() > 0){
      Can0.read(frame);
      if(frame.id == 0x18F11384){
        if(frame.data.low == 0x00){
          //BMS NORMAL
          bms_stat =  true; 
          lcd.setCursor(0,1);
          lcd.print("BMS :    ");
          lcd.setCursor(0,1);
          lcd.print("BMS : OK");
          break;
        }
        else{
          bms_stat = false;
        }
      }
    }
  }
  if(bms_stat == false){
    lcd.setCursor(0,1);
    lcd.print("BMS :    ");
    lcd.setCursor(0,1);
    lcd.print("BMS : ERROR");
  }

  frame.id = 0x0C8F0000;
  frame.length = 8;
  frame.data.low = 0x00000000;
  frame.data.high = 0x00;
  frame.extended = 1;
  CAN.sendFrame(frame);
}

void boot_dcdc(){
  unsigned long start_time = millis();
  while(millis()-start_time < 3000){
    frame.id = 0x0CF11800;
    frame.length = 8;
    frame.data.low = 0x00000001;
    frame.data.high = 0x00;
    frame.extended = 1;
    CAN.sendFrame(frame);

    if(Can0.available() > 0){
      Can0.read(frame);
      if(frame.id == 0x18F11382){
        if(frame.data.low == 0x00){
          //DCDC NORMAL
          dcdc_stat =  true; 
          lcd.setCursor(0,2);
          lcd.print("DCDC :    ");
          lcd.setCursor(0,2);
          lcd.print("DCDC : OK");
          break;
        }
        else{
          dcdc_stat = false;
        }
      }
    }
  }
  if(dcdc_stat == false){
    lcd.setCursor(0,2);
    lcd.print("DCDC :    ");
    lcd.setCursor(0,2);
    lcd.print("DCDC : ERROR");
  }
}


void boot_vvvf(){
  unsigned long start_time = millis();
  while(millis()-start_time < 3000){
    frame.id = 0x0C8F0000;
    frame.length = 8;
    frame.data.low = 0x00000001;
    frame.data.high = 0x00;
    frame.extended = 1;
    CAN.sendFrame(frame);

    if(Can0.available() > 0){
      Can0.read(frame);
      if(frame.id == 0x18F11394){
        if(frame.data.low == 0x00){
          //DCDC NORMAL
          vvvf_stat =  true; 
          lcd.setCursor(0,3);
          lcd.print("VVVF :    ");
          lcd.setCursor(0,3);
          lcd.print("VVVF : OK");
          break;
        }
        else{
          vvvf_stat = false;
        }
      }
    }
  }
  if(vvvf_stat == false){
    lcd.setCursor(0,3);
    lcd.print("VVVF :    ");
    lcd.setCursor(0,3);
    lcd.print("VVVF : ERROR");
  }
}

void device_check(){

  modbusTCP_update();

  lcd.setCursor(0,0);
  lcd.print("Devices Status :    ");
  
  if(bms_stat == true){
    lcd.setCursor(0,1);
    lcd.print("BMS : OK            ");
  }
  else{
    lcd.setCursor(0,1);
    lcd.print("BMS : ERROR         ");
  }

  if(dcdc_stat == true){
    lcd.setCursor(0,2);
    lcd.print("DCDC : OK           ");
  }
  else{
    lcd.setCursor(0,2);
    lcd.print("DCDC : ERROR        ");
  }

  if(vvvf_stat == true){
    lcd.setCursor(0,3);
    lcd.print("VVVF : OK           ");
  }
  else{
    lcd.setCursor(0,3);
    lcd.print("VVVF : ERROR        ");
  }
 

  lcd.setCursor(14,3);
  lcd.print("*Reset");
  if(digitalRead(tombol1) == LOW){
    NVIC_SystemReset();
  }
}

int read_master_control(){
  int digitalPins[10] = {40, 42, 44, 46, 48, 30, 32, 34, 36, 38}; //42, 40, 38, 36, 34, 52, 50, 48, 46, 44
  uint16_t result = 0;
  int MCState = 0;
  for (int i = 0; i < 10; i++) {
    int pinState = !digitalRead(digitalPins[i]);
    result |= (pinState << (9-i));
  
  }
  switch (result) {
  case 0b0000011111:
    MCState = 0;
    break;
  case 0b0000011011:
    MCState = 1;
    break;
  case 0b0000010011:
    MCState = 2;
    break;
  case 0b0000010111:
    MCState = 3;
    break;
  case 0b0000010101:
    MCState = 4;
    break;
  case 0b0000011101:
    MCState = 5;
    break;
  case 0b0000011001:
    MCState = 6;
    break;
  case 0b1000000000:
    MCState = 7;
    break;
  case 0b0110000001:
    MCState = 8;
    break;
  case 0b0111000001:
    MCState = 9;
    break;
  case 0b0101000001:
    MCState = 10;
    break;
  case 0b0101100001:
    MCState = 11;
    break;
  case 0b0111100001:
    MCState = 12;
    break;
  case 0b0110100001:
    MCState = 13;
    break;
  case 0b0100100001:
    MCState = 14;
    break;
  }
  return MCState;
}

void read_can(){
  if(Can0.available() > 0){
    Can0.read(frame);
    //read bms
    if(frame.id == 0x18F11484){
      int teg = 0;
      teg = (frame.data.byte[1] << 8);
      teg = teg | frame.data.byte[0];
      bms_tegangan = teg*0.01;
  
      int hex_arus = 0;
      hex_arus = (frame.data.byte[3] << 8);
      hex_arus = hex_arus | frame.data.byte[2];
      bms_arus = (hex_arus*0.1)-3212.7;
  
      int hex_soc = 0;
      hex_soc = (frame.data.byte[4]);
      if(hex_soc > 200) hex_soc = 200;
      soc = hex_soc*0.5;

      int hex_soh = 0;
      hex_soh = (frame.data.byte[6]);
      if(hex_soh > 200) hex_soh = 200;
      soh = hex_soh*0.5;
  
      int hex_temp = 0;
      hex_temp = (frame.data.byte[5]);
      if(hex_temp > 200) hex_temp = 200;
      bms_suhu = (hex_temp-40);      
    }

    //read vvvf
    if(frame.id == 0x18F02194){
        int hex_speed = 0;
        hex_speed = (frame.data.byte[3]);
        if(hex_speed > 200) hex_speed = 200;
        vvvf_speed = hex_speed;

        int hex_torque = 0;
        hex_torque = (frame.data.byte[2]);
        if(hex_torque > 200) hex_torque = 200;
        vvvf_torsi = hex_torque*0.5; 
    }

    //read dcdc
    if(frame.id == 0x18F11482){
      int hex_vout = 0;
      hex_vout = (frame.data.byte[1] << 8);
      hex_vout = hex_vout | frame.data.byte[0];
      dcdc_volt_out = hex_vout*0.05;

      int hex_aout = 0;
      hex_aout = (frame.data.byte[3] << 8);
      hex_aout = hex_aout | frame.data.byte[2];
      dcdc_arus_out = (hex_aout*0.1)-3212.7; 
      
      int hex_vin = 0;
      hex_vin = (frame.data.byte[5] << 8);
      hex_vin = hex_vin | frame.data.byte[4];
      dcdc_volt_in = hex_vin*0.05;

      int hex_ain = 0;
      hex_ain = (frame.data.byte[7] << 8);
      hex_ain = hex_ain | frame.data.byte[6];
      dcdc_arus_in = (hex_ain*0.1)-3212.7;   
    }

    //cek bms
    if(frame.id == 0x18F11384){
        if(frame.data.low == 0x00){
          bms_stat =  true; 
        }
        else{
          bms_stat = false;
        }
    }

    //cek dcdc
    if(frame.id == 0x18F11382){
        if(frame.data.low == 0x00){
          dcdc_stat =  true; 
        }
        else{
          dcdc_stat = false;
        }
    }

    //cek vvvf
    if(frame.id == 0x18F11394){
        if(frame.data.low == 0x00){
          vvvf_stat =  true;
        }
        else{
          vvvf_stat = false;
        }
    }
  }

}

void display_bms(){
  lcd.setCursor(0,0);
  lcd.print("======BMS Data======");

  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  lcd.print("SoC : ");
  lcd.print(round(soc));
  lcd.print("%");

  lcd.setCursor(15,1);
  lcd.print("     ");
  lcd.setCursor(15,1);
  lcd.print(round(bms_suhu));
  lcd.print(" C");

  lcd.setCursor(0,2);
  lcd.print("                    ");
  lcd.setCursor(0,2);
  lcd.print("Volt : ");
  lcd.print(bms_tegangan, 1);
  lcd.print("V");

  lcd.setCursor(0,3);
  lcd.print("                    ");
  lcd.setCursor(0,3);
  lcd.print("Arus : ");
  lcd.print(bms_arus, 1);
  lcd.print("A");
}

void display_dcdc(){
  lcd.setCursor(0,0);
  lcd.print("=====DCDC Data======");

  lcd.setCursor(0,1);
  lcd.print("Input      Output");

  lcd.setCursor(0,2);
  lcd.print("                    ");
  lcd.setCursor(0,2);
  lcd.print("V:");
  lcd.print(dcdc_volt_in, 1);
  lcd.print("V");

  lcd.setCursor(0,3);
  lcd.print("                    ");
  lcd.setCursor(0,3);
  lcd.print("A:");
  lcd.print(dcdc_arus_in, 1);
  lcd.print("A");

  lcd.setCursor(11,2);
  lcd.setCursor(11,2);
  lcd.print("V:");
  lcd.print(dcdc_volt_out, 1);
  lcd.print("V");

  lcd.setCursor(11,3);
  lcd.setCursor(11,3);
  lcd.print("A:");
  lcd.print(dcdc_arus_out, 1);
  lcd.print("A");
  
}

void display_vvvf(){
  lcd.setCursor(0,0);
  lcd.print("=====VVVF Data======");

  lcd.setCursor(0,1);
  lcd.print("                    ");
  
  lcd.setCursor(0,1);
  lcd.print("Torsi : ");
  lcd.print(vvvf_torsi, 1);
  lcd. print(" %");

  lcd.setCursor(0,2);
  lcd.print("Speed : ");
  lcd.print(vvvf_speed, 1);
  lcd. print(" km/jam");
}

void display_sensor(){
  lcd.setCursor(0,0);
  lcd.print("====Sensor Data=====");

  lcd.setCursor(0,1);
  lcd.print("Catenary     Battery");

  lcd.setCursor(0,2);
  lcd.print("                    ");
  lcd.setCursor(0,2);
  lcd.print("PT:");
  lcd.print(dcpt_catenary,1);
  lcd.print("V");

  lcd.setCursor(0,3);
  lcd.print("                    ");
  lcd.setCursor(0,3);
  lcd.print("CT:");
  lcd.print(dcct_catenary,1);
  lcd.print("A");

  lcd.setCursor(11,2);
  lcd.setCursor(11,2);
  lcd.print("PT:");
  lcd.print(dcpt_battery,1);
  lcd.print("V");

  lcd.setCursor(11,3);
  lcd.setCursor(11,3);
  lcd.print("CT:");
  lcd.print(dcct_battery,1);
  lcd.print("A");
}

void read_analog(){
  dcpt_catenary = analogRead(A0);
  dcct_catenary = analogRead(A6);
  dcpt_battery = analogRead(A7);
  dcct_battery = analogRead(A10);
}

void display_handler(){
  if(digitalRead(tombol3) == LOW){
      lcd_page++;
      if(lcd_page > 3){
        lcd_page = 3;
      }
      lcd.clear();
    }

  if(digitalRead(tombol2) == LOW){
    lcd_page--;
    if(lcd_page < 0){
      lcd_page = 0;
    }
    lcd.clear();
  }  

  switch (lcd_page) {
    case 0:
      display_bms();
      break;
    case 1:
      display_dcdc();
      break;
    case 2:
      display_vvvf();
      break;
    case 3:
      display_sensor();
      break;
  }
}

void modbusTCP_init(){
  Mb.Run();
  Mb.C[0] = 1;
  Mb.C[1] = 1;
  Mb.C[2] = 1;
  Mb.C[3] = 1;
  Mb.C[4] = 1;
  Mb.C[5] = 1;
  Mb.C[6] = 1;
  Mb.C[7] = 1;
  Mb.C[8] = 1;
  Mb.C[8] = 1;

  Mb.R[0] = 0;
  Mb.R[1] = 0;
  Mb.R[2] = 0;
  Mb.R[3] = 0;
  Mb.R[4] = 0;
  Mb.R[5] = 0;
  Mb.R[6] = 0;
  Mb.R[7] = 0;
  Mb.R[8] = 0;
  Mb.R[9] = 0;
  Mb.R[11] = 0;
}

void modbusTCP_update(){
  Mb.Run();
  Mb.C[0] = 1;
  Mb.C[1] = 1;
  Mb.C[2] = 1;
  Mb.C[3] = 1;
  Mb.C[4] = 1;
  Mb.C[5] = 1;
  Mb.C[6] = 1;
  Mb.C[7] = 1;
  Mb.C[8] = 1;
  Mb.C[9] = vvvf_stat;

  Mb.R[0] = vvvf_speed;
  Mb.R[1] = vvvf_torsi;
  Mb.R[2] = bms_tegangan;
  Mb.R[3] = soc;
  Mb.R[4] = bms_arus;
  Mb.R[5] = bms_suhu;
  Mb.R[6] = dcdc_arus_in;
  Mb.R[7] = dcdc_volt_in;
  Mb.R[8] = dcdc_arus_out;
  Mb.R[9] = dcdc_volt_out;
  Mb.R[10] = master_control;
  Mb.R[11] = 0;
  Mb.R[12] = 0;
  Mb.R[13] = 0;
  Mb.R[14] = 0;
}

void DCK_short(){
  digitalWrite(DCK_battery, LOW);
  digitalWrite(DCK_catenary, LOW);
  digitalWrite(DCK_load, LOW);
}

void DCK_open(){
  digitalWrite(DCK_battery, HIGH);
  digitalWrite(DCK_catenary, HIGH);
  digitalWrite(DCK_load, HIGH);
}

void proteksi_dck(){
  //proteksi short circuit
  if(dcct_catenary > 500 || dcct_battery > 500){
    DCK_open();
    lcd.clear();
    while(true){
      lcd.setCursor(0,0);
      lcd.print("Short Circuit");
      lcd.setCursor(0,1);
      lcd.print("Terdeteksi!");    

      lcd.setCursor(14,3);
      lcd.print("*Reset");
      if(digitalRead(tombol1) == LOW){
        NVIC_SystemReset();
      }
    }
  }

  if((dcct_battery > 100 && dcct_battery < 500 || (dcct_catenary>100 && dcct_catenary < 500))){
    overcurrent_state = true;
    if(toggle == false){
      overcurrent_time = millis();
      toggle = true;
    }
    
  }
  else{
    overcurrent_state = false;
    toggle = false;
  }

  if(overcurrent_state == true && (millis()-overcurrent_time > 1000)){
    DCK_open();
    lcd.clear();
    while(true){
      lcd.setCursor(0,0);
      lcd.print("Overcurrent");
      lcd.setCursor(0,1);
      lcd.print("Terdeteksi!");

      lcd.setCursor(14,3);
      lcd.print("*Reset");
      if(digitalRead(tombol1) == LOW){
        NVIC_SystemReset();
      }
    }
  }

}




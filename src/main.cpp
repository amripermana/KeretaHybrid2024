#include "functions.h"

LiquidCrystal lcd(9, 8,7, 6, 5, 4);
CAN_FRAME frame;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  Can0.begin(250000);
  Can0.watchFor();

  Ethernet.init(10);
  Ethernet.begin(mac, ip);
  server.begin();

  pinMode(tombol1, INPUT);
  pinMode(tombol2, INPUT);
  pinMode(tombol3, INPUT);
  pinMode(tombol4, INPUT);
  pinMode(A0, INPUT);
  pinMode(A7, INPUT);
  pinMode(A6, INPUT);
  pinMode(A10, INPUT);
  pinMode(DCK_battery, OUTPUT);
  pinMode(DCK_catenary, OUTPUT);
  pinMode(DCK_load, OUTPUT);

  DCK_open();

  init_MC();
  init_display();
  devices_boot();
  modbusTCP_init();

  while((dcdc_stat&vvvf_stat&bms_stat) != true){
    device_check();
  }

  lcd.clear();

}

void loop() {

  if(millis()-timer_10ms > 10){
    //baca master control
    master_control = read_master_control();

    //baca seluruh data CAN
    read_can();

    //baca DCPT dan DCCT
    read_analog();
    proteksi_dck();
    DCK_short();
    
    //modbus TCP update
    modbusTCP_update();
    
    //check jika terdapat device yang error
    //reset ketika device error
    while((dcdc_stat&vvvf_stat&bms_stat) != true){
      device_check();
    }
    
  
    timer_10ms = millis();
  }

  if(millis()-timer_500ms > 500){
    
    display_handler();

    timer_500ms = millis();
  }


  
}


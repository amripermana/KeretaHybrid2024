#include <Arduino.h>
#include <due_can.h>
#include <LiquidCrystal.h>
#include <UIPEthernet.h>
#include "Mudbus.h"

extern Mudbus Mb;
extern IPAddress ip;
extern EthernetServer server;
extern LiquidCrystal lcd;
extern CAN_FRAME frame;
extern byte mac[];
extern bool dcdc_stat;
extern bool vvvf_stat;
extern bool bms_stat;
extern int tombol1;
extern int tombol2;
extern int tombol3;
extern int tombol4;
extern int master_control;
extern float soc;
extern float soh;
extern float bms_tegangan;
extern float bms_arus;
extern float bms_suhu;
extern unsigned long timer_10ms;
extern unsigned long timer_500ms;
extern unsigned long timer_1000ms;
extern float vvvf_speed;
extern float vvvf_torsi;
extern float dcdc_arus_in;
extern float dcdc_arus_out;
extern float dcdc_volt_in;
extern float dcdc_volt_out;
extern int lcd_page;
extern float dcpt_catenary;
extern float dcct_catenary;
extern float dcpt_battery;
extern float dcct_battery;
extern int DCK_battery;
extern int DCK_catenary;
extern int DCK_load;
extern bool proteksi;
extern bool overcurrent_state;
extern unsigned long overcurrent_time;


void init_display();
void devices_boot();
void boot_bms();
void boot_dcdc();
void boot_vvvf();
void device_check();
void init_MC();
void read_can();
void display_bms();
void display_handler();
int read_master_control();
void modbusTCP_init();
void modbusTCP_update();
void read_analog();
void display_sensor();
void proteksi_dck();
void DCK_short();
void DCK_open();

//////////dummy////////////////

// Serial.print("tegangan : ");
// Serial.print(bms_tegangan);
// Serial.print(", arus : ");
// Serial.print(bms_arus);
// Serial.print(", soc : ");
// Serial.print(soc);
// Serial.print(", suhu : ");
// Serial.print(bms_suhu);

// Serial.print(" || dcVin : ");
// Serial.print(dcdc_volt_in);
// Serial.print(", dcVout : ");
// Serial.print(dcdc_volt_out);
// Serial.print(", dcAin : ");
// Serial.print(dcdc_arus_in);
// Serial.print(", dcAout : ");
// Serial.print(dcdc_arus_out);

// Serial.print(" || torsi : ");
// Serial.print(vvvf_torsi);
// Serial.print(", speed : ");
// Serial.println(vvvf_speed);

// A0 = DCPT Catenary
// A7 = DCPT Baterai
// A6 = DCCT Catenary
// A10 = DCCT Baterai
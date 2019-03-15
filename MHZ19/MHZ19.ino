#include <M5Stack.h>
#include "Free_Fonts.h" // Include the header file attached to this sketch


const int rx_pin = 16;  //Serial rx pin no
const int tx_pin = 17;  //Serial tx pin no

int buffer_index = 0;
int avarage_result = 0;
const int buffer_size = 8;
uint8_t mhz19_request[9]  = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; 
int mhz19_buffer[buffer_size] = {0,0,0,0,0,0,0,0};

/*------------------------------------------------------
  MH-Z19 CO2 sensor setup
  ------------------------------------------------------*/
void setup() {
  M5.begin();
  Serial.begin(115200);
  Serial.println(F("Starting..."));
  // Serial2.begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert)
  Serial2.begin(9600, SERIAL_8N1, rx_pin, tx_pin);
}

/*------------------------------------------------------
  MH-Z19 CO2 sensor loop
  ------------------------------------------------------*/
void loop() {
   uint8_t mhz19_response[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   writeCommand(mhz19_request, mhz19_response);
   //Serial2.flush();
   //int co2ppm = 800;
   //Serial.print("co2: "); Serial.println(co2ppm);
  
   Serial.print("mhz19_response: ");
   for (int i = 0; i < 9; i++){
      Serial.printf("0x%02X",mhz19_response[i]);
      Serial.print(" ");
   }

   int result = ((int) mhz19_response[2])*256 + (int) mhz19_response[3];
   Serial.println("");
   Serial.println("CO2 = "); Serial.println(result);
   Serial.println("");
   
   mhz19_buffer[buffer_index]=result;
   buffer_index = (buffer_index +1)%buffer_size;
   
   int buffer_sum = 0;
   for(int i=0; i< buffer_size; i++){
    buffer_sum = buffer_sum + mhz19_buffer[i];
   }
   
   avarage_result = buffer_sum/buffer_size;
   
   if (avarage_result < 800 ){
     M5.Lcd.setTextColor(TFT_GREEN);
   } else if (avarage_result > 800 && avarage_result < 1200){
     M5.Lcd.setTextColor(TFT_YELLOW);
   } else if (avarage_result > 1200){
     M5.Lcd.setTextColor(TFT_RED);
   } else{
     M5.Lcd.setTextColor(TFT_WHITE);
   }

   M5.Lcd.setCursor(20, 100);
   M5.Lcd.setFreeFont(FSB24);
   M5.Lcd.clear();
   M5.Lcd.printf("CO2 = %d", avarage_result);
   
   delay(5000);
}

void writeCommand(uint8_t cmd[], uint8_t *response){
   Serial2.write(cmd,9);
   int time_counter=0;
   if (response != NULL){
     while (Serial2.available() <= 0){
       if (++time_counter > 10){
         Serial.println("error: can't get MH-Z19 response.");
         return;
       }
       yield();
       delay(200);
     }
     Serial2.readBytes(response,9);
   }
}

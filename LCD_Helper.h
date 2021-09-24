#ifndef LCD_Helper_h
#define LCD_Helper_h
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
  
#endif
#include <LiquidCrystal.h>
 //********************** DECLARE LCD ******************************/
//2 - LiquidCrystal lcd(12, 14, 2, 0, 4, 5);
//3 - lcd(14, 12, 13, 5, 4, 16); FINAL BOARD LiquidCrystal lcd(4, 5, 13, 12, 14, 16);
LiquidCrystal lcd(4, 5, 13, 12, 14, 16);

class LCD_Helper {
  public:
  byte arrow_u[8] = {B00100, B01110,B11111,B00100,B00100,B00100,B00100, B00100 }; // Arrow UP
  byte arrow_d[8] = {B00100, B00100, B00100, B00100, B00100, B11111, B01110, B00100};  // Arrow Down
  int LCD_row_number = 4;
  int LCD_column_number = 20;

  void Begin_LCD()
  {
    lcd.begin(LCD_column_number, LCD_row_number);
    lcd.createChar (5, arrow_u); // load arrow up to memory 5
    lcd.createChar (6, arrow_d); // load arrow down to memory 6
  }
  
  void PrintLCD(String str)
  {
    if (Last_Data == str)
    {
      return;
    }
    int last_result_index =0;
    int result_number = 0;
    lcd.clear();
    for (int i = 0; i < str.length(); i++) 
    {
          if (str.substring(i, i + 4) == "<br>") 
          {
            String mess = str.substring(last_result_index, i);
            last_result_index = i + 4;
            i = last_result_index - 1;
            if (result_number > LCD_row_number)
            {
              result_number = 0;
            }
            mess = mess.substring(0, LCD_column_number);
            lcd.setCursor(0, result_number);
            lcd.print(mess);
            Serial.println (String(result_number) + " : " + mess);
            result_number++;
          }
    }
    String mess = str.substring(last_result_index, str.length());
    mess = mess.substring(0, LCD_column_number);
    lcd.setCursor(0, result_number);
    lcd.print(mess);
    Serial.println (String(result_number) + " : " + mess);
    Last_Data = str;
  }
  private :
  String Last_Data ="";
};

#endif

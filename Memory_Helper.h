#ifndef Memory_Helper_h
#define Memory_Helper_h
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif
class Memory_Helper {
  public:
 // Memory_Helperz();
   String Read_Wifi_UserName ()
    {
      String esid= "";
      for (int i = 0; i < 32; ++i)
      {
        if (EEPROM.read(i) > 0)
        {
          esid += char(EEPROM.read(i));
        }
      }
      return esid;
    }
    String Read_Wifi_Password ()
    {
      String epass= "";
      for (int i = 32; i < 64; ++i)
      {
          if (EEPROM.read(i) > 0)
          {
             epass += char(EEPROM.read(i));
          }
      }
      return epass;
    }
    void Write_Wifi_Configuration (String data)
    {
        int size;
        int _current_char  = 0;
        EEPROM.begin(512);
        Clear_Data_In_Mmemory (0, 64);
        if (debug_serial == true)
        {
          Serial.println("Wifi data " + data);
        }
        for (int i = 3; i < data.length(); i++) 
        {
          if (data.substring(i, i + 4) == "<br>") // Search <br> tag to Split Username & Password
          {
            String mess = data.substring(3, i);
            _current_char = i;
            for (int j = 0; j < mess.length(); ++j)
            {
              EEPROM.write(j, mess[j]);
            }
          }
        }
        //###################### SAVE PASSWORD TO MEMORY #####################################
        String mess = data.substring( _current_char + 4, data.length()); // +4 From <br>
        for (int i = 0; i <  mess.length(); ++i)
        {
          EEPROM.write(32 + i,  mess[i]);
          if (debug_serial == true)
          {
            Serial.println("Write wifi password : " + (String)i +"  data " + mess[i]);
          }
        }
        EEPROM.commit();
    }
    String Read_Host_Configuration ()
    {
        String data= "";
        for (int i = CONFIG_HOST_SAVE_POSITION; i < CONFIG_END_HOST_SAVE_POSITION; ++i)
        {
          if (debug_serial == true)
          {
            Serial.println("Read host: " + (String)i +"  data " + char(EEPROM.read(i)));
          }
          if (EEPROM.read(i) > 0)
          {
             data += char(EEPROM.read(i));
          };
        }
        return data;
    }
    void Write_Host_Configuration (String data)
    {
      
      EEPROM.begin(512);
      Clear_Data_In_Mmemory (CONFIG_HOST_SAVE_POSITION, CONFIG_END_HOST_SAVE_POSITION);
      String mess = data;
      int j = 0;
      for (int i = CONFIG_HOST_SAVE_POSITION; i <  CONFIG_HOST_SAVE_POSITION + mess.length(); ++i)
      { 
        if (debug_serial == true)
        {
          Serial.println("Write host i : " + (String)i +" data " + mess[j]);
        }
        EEPROM.write(i,mess[j]);
        j++;
      }
      Serial.println("Saved Host : " + data);
      EEPROM.commit();
    }
    String Read_Lastest_Data ( )
    {
      String data= "";
      for (int i = CONFIG_DATA_SAVE_POSITION; i < CONFIG_END_DATA_SAVE_POSITION; ++i)
      {
        if (debug_serial == true)
        {
          Serial.println("Read_Lastest i : " + (String)i +" data " + char(EEPROM.read(i)));
        }
        
        if (EEPROM.read(i) > 0)
        {
          data += char(EEPROM.read(i));
        }
      }
      return data;
    }
    void Write_Lastest_Data (String data)
    {
      
      EEPROM.begin(512);
      Clear_Data_In_Mmemory (CONFIG_DATA_SAVE_POSITION, CONFIG_END_DATA_SAVE_POSITION);
      String mess = data;
      int j = 0;
      for (int i = CONFIG_DATA_SAVE_POSITION; i <  CONFIG_DATA_SAVE_POSITION + mess.length(); ++i)
      { 
        if (debug_serial == true)
        {
          Serial.println("Write i : " + (String)i +" data " + mess[j]);
        }
        
        EEPROM.write(i,  mess[j]);
        j++;
      }
      EEPROM.commit();
    }
    void Clear_Data_In_Mmemory (int data_start_position , int data_position)
    {
      if (debug_serial == true)
      {
        Serial.println("Clear start : " + (String)data_start_position +"  end " + data_position);
      }
      
      for (int i = data_start_position; i < data_position; ++i) {
          EEPROM.write(i, 0); //clearing
        }
    }
    private:
    int CONFIG_HOST_SAVE_POSITION = 64;
    int CONFIG_END_HOST_SAVE_POSITION = 128;
    int CONFIG_DATA_SAVE_POSITION = 128;
    int CONFIG_END_DATA_SAVE_POSITION = 256;
    bool debug_serial =  false;
    

};
#endif

/*
    Simple HTTP get webclient test
*/

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "Memory_Helper.h"
#include "LCD_Helper.h"
//********************** DECLARE Wifi ******************************/

char ssid[32] ;
char password[32];
String host = "boonygroup.com";
char* host_char = "boonygroup.com";
const int httpPort = 80;
String Macaddress;
boolean IsConnected = false;
boolean Found_Network = false;

const int port = 9876; // and this port
WiFiServer server(port);
WiFiClient client;
boolean Develop_Mode = false;
boolean Default_Loaded  = false;


/*********************** DECLARE SERVER **************************/
int Success_Message = 0;
String Server_Messsage;
String Data_Message;
String Last_Data_Message;
String Default_Data;

//##########################DECLARE DEVELOP MODE ########################///

bool Debug_Mode_Live = false;
bool force_develop_mode = false;
String develop_version = "1.1";
int CONNECTION_TIMES_FAILED = 0;
int setting_config = 1;
const char* develop_ssid = "BoonyDeveloper";
const char* develop_password = "vanphong@2017"; 

//set command config -- not change
char cmd[100]; // stores the command chars received from RoboRemo
int cmdIndex;
unsigned long lastCmdTime = 60000;
unsigned long aliveSentTime = 0;

//load library
LCD_Helper lcd_helper = LCD_Helper();
Memory_Helper memory_helper = Memory_Helper();


void setup() {
  Serial.begin(115200);
  //############################ STEP 1 SCAN NETWORK #############################//
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  //WiFi.disconnect();
  delay(100);
  Serial.println("Scan network Nearby");
  delay(5000);
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
  {
    Serial.println("no networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    if (force_develop_mode == false)
    {
      for (int i = 0; i < n; ++i)
      {
        if (WiFi.SSID(i) ==  develop_ssid)
        {
          Develop_Mode = true;
          Found_Network = true;
          break;
        }
        delay(10);
      }
    }
    else
    {
      Develop_Mode = true;
      Found_Network = true;
    }
    
  }
  Serial.println("");
  byte mac[6];
  WiFi.macAddress(mac);
  Macaddress = macToStr(mac);
  lcd_helper.Begin_LCD();
  delay(100);
  // END STEP 1 SCAN NETWORK //
}


void ConnectWifi()
{
  String str_ssid = String(ssid);
  String str_develop_ssid = String(develop_ssid);
  if (Develop_Mode == true )
  {
    if (force_develop_mode == true)
    {
      char*  _force_develop_ssid = "Boony";
      char*  _develop_password= "#boonygroup";
      str_develop_ssid = String(_force_develop_ssid);
      
      lcd_helper.PrintLCD("FORCE DEVELOPER MODE<br>Connecting to Wifi<br>" + str_develop_ssid+ +"<br>Version: "+develop_version);
      WiFi.begin(_force_develop_ssid, _develop_password);
      delay (8000);
    }
    else
    {
      
      if (CONNECTION_TIMES_FAILED < 3)
      {
          lcd_helper.PrintLCD("   DEVELOPER MODE   <br>Connecting to Wifi<br>" + str_develop_ssid);
          WiFi.begin(develop_ssid, develop_password);
          delay (8000);
      }
      else
      {
          Develop_Mode = false;
          CONNECTION_TIMES_FAILED = 0;
          lcd_helper.PrintLCD("Swiching To<br>Normal Mode<br>Connecting to Wifi<br>" + str_ssid); 
      }
    }
    
  }
  else // Develop_Mode = false;
  {
     // ########################### READ WIFI CONFIGURATION FROM MEMORY ##################################
     if (Default_Loaded == false)
     {
        EEPROM.begin(512);
        String esid=  memory_helper.Read_Wifi_UserName();
        String epass = memory_helper.Read_Wifi_Password();
        Default_Data = memory_helper.Read_Lastest_Data();
        String temp_host= memory_helper.Read_Host_Configuration();

        Last_Data_Message = Default_Data;
        EEPROM.commit();
        Serial.println("Memory wifi setup " );
        if (temp_host.length() > 2)
        {
          host = temp_host;
          // Length (with one extra character for the null terminator)
          int str_len = host.length() + 1; 
          // Prepare the character array (the buffer) 
          host_char =new char[str_len];
          // Copy it over 
          host.toCharArray(host_char, str_len);
        }
        if (esid.length() > 2)
        {
          Serial.println("Had Wifi memory " + esid);
          Serial.println("Default data" + Default_Data);
          if (Debug_Mode_Live == true)
          {
            Serial.println("Wifi SSID " + esid);
            Serial.println("Wifi PASS " + epass);
          }
          
        }
        else
        {
          esid = "sony";
          epass = "sony@2017";   
        }
        esid.toCharArray(ssid, esid.length()+1);
        epass.toCharArray(password, epass.length()+1);
        Default_Loaded = true;
        str_ssid = String(ssid);
     }// END READ WIFI CONFIGURATION FROM MEMORY 
     
    if (Found_Network == false)
    {
      int n = WiFi.scanNetworks();
      if (n == 0)
      {
        print_in_debug("no networks found");
      }
      else
      {
        for (int i = 0; i < n; ++i)
        {
          if (WiFi.SSID(i) ==  ssid)
          {
            lcd_helper.PrintLCD("Connecting to Wifi<br>" + str_ssid);
            WiFi.begin(ssid, password);
            Found_Network = true;
            delay (5000);
            break;
          }
          delay(10);
        }
      }
    }//End Searching network
    
  }//End read data from ROM
  if (Found_Network == false || (String)WiFi.localIP() == "0") // Connect Failed
  {
    CONNECTION_TIMES_FAILED ++;
    if (Develop_Mode == true)
    {
        lcd_helper.PrintLCD("Developer Failed<br>" + Macaddress + "<br>" + "Trying... " + String (CONNECTION_TIMES_FAILED) + " Time(s)" ); 
    }
    else
    {
      Load_Default_Data();
      delay (2000);
      if (Found_Network == false)
      {
        print_in_debug("NOT FOUND NETWORK:<br>" + str_ssid); 
      }
      else
      {
        if (CONNECTION_TIMES_FAILED >= 3)
        {
          print_in_debug("Can't connect Wifi<br>" + str_ssid); 
        }
        else
        {
           String connect_times_point = ".";
           for (int i=0; i < 20; i++){
              connect_times_point = connect_times_point + ".";
           }
          if (Debug_Mode_Live == true)
          {
            print_in_debug("Connect fail to Wifi<br>" + str_ssid + "<br>Trying again " + CONNECTION_TIMES_FAILED + " times");
          }
           
        }
      }
    }
    
  }
  else
  {
    IsConnected = true;
    String ipaddress = WiFi.localIP().toString();
    if (Develop_Mode == true)
    {
      server.begin(); // start TCP server
      lcd_helper.PrintLCD("   DEVELOPER MODE  <br>IP: " + ipaddress + "<br>" + Macaddress+"<br>Version: "+develop_version);
    }
    else
    {
        lcd_helper.PrintLCD("Test connect to: <br>" + String(host) + "<br>" + Macaddress);
        if (!client.connect(host_char, httpPort)) {
          //######################## LOAD DEFAULT DATA##################################
          print_in_debug("Test connection Failed<br>Load default data");
          Load_Default_Data();
          return;
        }
        else
        {
          lcd_helper.PrintLCD("LOADING . . . .<br>Please wait..." + (String)char(5) +  (String)char(6)); //Display arrow up and down
        }
      
    }
  }
}
void loop() {

  if (IsConnected == false)
  {
      ConnectWifi();
  }
  else // End if (IsConnected == false)
  {
      if (Develop_Mode == true)
      {
        String ipaddress = WiFi.localIP().toString();
        lcd_helper.PrintLCD("   DEVELOPER MODE  <br>IP: " + ipaddress + "<br>" + Macaddress+"<br>Version: "+develop_version);
        
        if (!client.connected()) 
        {
          // listen for incoming clients
          client = server.available();
          return;
        }
        
        if (client.available()) 
        {
          //Serial.println("Client connected");
          char c = (char)client.read(); // read char from client (RoboRemo app)
          
          if (c == '\n') { // if it is command ending
            Serial.println("Raw data " + (String)cmd + " Length " + (String)cmdIndex);
            cmd[cmdIndex] = 0;
            exeCmd();  // execute the command
            cmdIndex = 0; // reset the cmdIndex
          } else 
          {
            cmd[cmdIndex] = c; // add to the cmd buffer
            if (cmdIndex < 99) cmdIndex++;
          }
          
        }
        if (millis() - aliveSentTime > 500) { // every 500ms
            client.write("Connected. 1\n");
            aliveSentTime = millis();
          }
        
    }
    else //IsConnected && Develop_Mode == false
    {
      print_in_debug("Downloading Data...<br>Connnected : " + (String)IsConnected + "<br>Found Network : " + (String)Found_Network); // Debug only
      Load_Data();
      delay(10000);
    }
  }
}
void Load_Data()
{ 
  //########################### LAY DU LIEU #######################################################
  if (!client.connect(host_char, httpPort)) {
    print_in_debug("Can't Connect To Server<br>Please Check Server...");
    Load_Default_Data();
    IsConnected = false;
    Found_Network = false;
    return;
  }
  print_in_debug("Get Data on server ");
  // LAY THÔNG TIN TRÊN SERVER
  String device_status = "MACAddress=" + Macaddress + "&local_ip=" + WiFi.localIP().toString()+"&ssid=" + ssid;
  GetPostData("/VideoAdvertising/Simplepages/GetVideo", device_status);
  String msgString(Server_Messsage);
  
  if (Success_Message == 1)
  {
    if (msgString == "Register")//Nếu chưa đăng ký
    {
      //Register
      GetPostData("/VideoAdvertising/Simplepages/VerifyAddEdit_device", "MACAddress=" + Macaddress);
    }
    else
    {
        // GET AND SAVE DATA 
        print_in_debug("Saved Data Debug " + Data_Message);
        lcd_helper.PrintLCD(Data_Message);
    } // END LOAD DATA TO LCD
  }// END LOAD DATA TO SCREEN
  
  else // Không thể kết nối Server
  {
      print_in_debug("SERVER MAINTAINING.<br>Load DataMessage From Memeroy");
      Load_Default_Data();
  }
}// END LAY DU LIEU
void GetPostData(String url, String PostData)
{
  // Func sumbit POST get respond tu Server
  // This will send the request to the server
  client.println("POST " + url + " HTTP/1.1");
  client.println("Host: " + host);
  client.println("Cache-Control: no-cache");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(PostData.length());
  client.println();
  client.println(PostData);

  delay(500);
  //Serial.println (host+ url);
  String section = "header";
  // Read all the lines of the reply from server and print them to Serial
  int json_size;
  while (client.available()) {
    String line = client.readStringUntil('\r');
    int header_json;
    if (section == "header") {
      for (header_json = 0; header_json < 6; header_json++) {
        if (line.substring(header_json, header_json + 1) == "{")
        {
          section = "json";
          line = line.substring(header_json, line.length());
          break;
        }
      }
      //Serial.println ("section " + section +  " Line " + line);
    }
    if (section == "json") { // print the good stuff
      section = "ignore";
      json_size = line.length() + 1;// Tuy vao json
      Serial.println(line); // Hex
      int size = json_size;
      char json[size];
      line.toCharArray(json, size);
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json_parsed = jsonBuffer.parseObject(const_cast<char*>(json));
      
      if (!json_parsed.success())
      {
        Serial.println("parseObject() failed");
        Serial.println("Data : " + line);
        break;
      }
      Server_Messsage = json_parsed["Message"].as<String>();;
      Success_Message = json_parsed["Success"];
      print_in_debug("Data Status :" + Server_Messsage);
      if (sizeof(json_parsed["Content"]) > 0)
      {
        Data_Message = json_parsed["Content"].as<String>();
        if (Data_Message != Last_Data_Message)
        {
          Last_Data_Message = Data_Message;
          Default_Data = Data_Message;
          memory_helper.Write_Lastest_Data(Last_Data_Message);
          print_in_debug("Write Default Data");
        }
      }
    }
  }

}// END Func sumbit POST get respond tu Server
void Load_Default_Data()
{
  if(Data_Message.length() == 0)
  {
    print_in_debug("Don't have default data");
  }
  lcd_helper.PrintLCD(Default_Data);
  
}
boolean cmdStartsWith(const char *st) { // checks if cmd starts with st

  for (int i = 0; ; i++) {

    if (st[i] == 0) return true;
    if (cmd[i] == 0) return false;
    if (cmd[i] != st[i]) return false;;
  }
  return false;
}
void exeCmd() { // executes the command from cmd
  lastCmdTime = millis();
  if ( cmdStartsWith("swf") ) {
    String data;
    data = String(cmd);
    memory_helper.Write_Wifi_Configuration(data);
    for( int i = 0; i < sizeof(data);  ++i )
    {
      cmd[i] = (char)0;
    }
    lcd_helper.PrintLCD("Write...<br>Wifi Configuration<br>Successful!");
    print_in_debug("Saved: " + data);
    delay(2000);
  }
  if ( cmdStartsWith("sdt") ) {
    int ch = cmd[2] - '0';
    int i;
    String data;
    for (i = 4; i <= cmdIndex; i ++)
    {
      //data = String(cmd[i], cmdIndex);
    }
    data = String(cmd);
    data = data.substring(3, data.length());
    memory_helper.Write_Lastest_Data(data);
    for( int i = 0; i < sizeof(data);  ++i )
    {
      cmd[i] = (char)0;
    }
    lcd_helper.PrintLCD("Write...<br>Default Data<br>Successful!");
    print_in_debug("Saved: " + data);
    delay(2000);
  }
  if ( cmdStartsWith("ssv") ) {
    int ch = cmd[2] - '0';
    int i;
    String data;
    for (i = 4; i <= cmdIndex; i ++)
    {
      //data = String(cmd[i], cmdIndex);
    }
    data = String(cmd);
    data = data.substring(3, data.length());
    memory_helper.Write_Host_Configuration(data);
    for( int i = 0; i < sizeof(data);  ++i )
    {
      cmd[i] = (char)0;
    }
    lcd_helper.PrintLCD("Write...<br>Server Configuration<br>Successful!");
    print_in_debug("Saved: " + data);
    delay(2000);
  }
}
String macToStr(const uint8_t* mac)
{
  String result;
  String atom;
  for (int i = 0; i < 6; ++i) {
    atom = String(mac[i], 16);
    if ( atom.length() < 2)
    {
      result += String ("0") + atom;
    }
    else
    {
      result += atom;
    }
    if (i < 5)
      result += ':';
  }
  return result;
}
void print_in_debug(String msg)
{
  if (Debug_Mode_Live == true)
  {
    lcd_helper.PrintLCD(msg);
    delay(2000);
    Serial.println(msg);
  }
}

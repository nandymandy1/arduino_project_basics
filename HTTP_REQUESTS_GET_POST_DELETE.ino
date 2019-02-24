// Library to connect with the Wifi
#include <ESP8266WiFi.h>
// Http client module to make GET POST DELETE and PUT Requests to the server
#include <ESP8266HTTPClient.h>
// Library inport to Deal with JSON and its functions
#include <ArduinoJson.h>
// Library to Read and Write the data to the machine's storage
#include <EEPROM.h>

//  Initializing the Storage of the Device
//  EEPROM.begin(512);

const char *ssid = "nandy";
const char *password = "nandymandy123";

void setup()
{

  Serial.begin(9600);
  // Connecting with the WIFI
  connectWifi();
  // Authenticating the USER
  auhtenticate();
}

void loop()
{
  delay(30000); //Send a request every 30 seconds
}

/*
 * Function to Find and Connect the WIFI
 */

void connectWifi()
{
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting..");
  }
}

/*
 * Function to Authenticate the USER
 */

void auhtenticate()
{
  if (WiFi.status() == WL_CONNECTED)
  { //Check WiFi connection status

    HTTPClient http; 
    //Declare an object of class HTTPClient
    /* GET REQUEST TEst to connect with the server over wifi running over same router */
    /*  WORKING STATUS TRUE */
    //
    //    http.begin("http://192.168.43.159:5000/users/device");  //Specify request destination
    //    int httpCode = http.GET();                                                                  //Send the request

    //    if (httpCode > 0) { //Check the returning code
    //      String payload = http.getString();   //Get the request response payload
    //      Serial.println(payload);             //Print the response payload
    //    }
    //
    //    http.end();   //Close connection

    /* POST REQUEST Authenticate the USER  */
    /*  WORKING STATUS CORRECT  */
    // Create a new JSON Obejct
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &userDetails = jsonBuffer.createObject();
    userDetails["username"] = "nandymandy123";
    userDetails["password"] = "password";

    char JSONmessageBuffer[300];
    // Converting the user Object into JSON Object which is valid for the server
    userDetails.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

    // Opening the HTTP Connection with the URI
    http.begin("http://192.168.43.159:5000/users/login");

    // Appending the Header with the requested URI which in this case is Application/JSON Content-Type
    http.addHeader("Content-Type", "application/json");

    // Sending the prepared json object to the server
    int httpCode = http.POST(JSONmessageBuffer);

    /* Parsing the Http Response from the server into JSON Object
       *  Step one specify the Response Json Object's size which in out case is one parent
       *  with 3 key and last one is the child object in the parent object with 4 keys
    */
    const int capacity = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4);
    /*
       * Defined the Dynamic buffer Size by the initialized capacity of the response
    */
    DynamicJsonBuffer res(capacity);
    /*
       * Streaming the responsed and parsing it into the Json Format
    */
    JsonObject &response = res.parseObject(http.getString());
    // Checking for the response of the json encodation of the response
    if (response.success())
    {
      auto success_status = response["success"].as<bool>();
      // Checking the status of the response if it true then the user is logged in otherwise it will be called back with error message from the server
      if (success_status)
      {
        auto token = response["token"].as<char *>();
        auto user = response["user"].as<String>();

        // Printing the Authentication status
        Serial.println("Status: User is Logged in.");
        /* Printing the token on screen for now but later it will 
        be stored in the memory of the device
        */
        //      Serial.print("Token: ");
        //      Serial.println(token);
        //      Parsing the user object to into JSON Objects
        const int user_obj_capacity = JSON_OBJECT_SIZE(4);
        DynamicJsonBuffer usr(user_obj_capacity);
        JsonObject &authUser = usr.parseObject(user);
        Serial.print("Authenticated User: ");
        authUser.prettyPrintTo(Serial);
      }
      else
      {
        // On Failed Login
        // Parsing the error from the server
        auto msg = response["msg"].as<String>();
        Serial.println("Login failed...");
        Serial.println(msg);
      }
    }
    else
    {
      Serial.println("Unknown Response");
    }
    http.end();
  }
}

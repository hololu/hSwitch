/** The MIT License (MIT)

Copyright (c) 2019 magnum129@github

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "TimeDB.h"
#include <ArduinoJson.h>


TimeDB::TimeDB(String apiKey)
{
  myApiKey = apiKey;
}

void TimeDB::updateConfig(String apiKey, String lat, String lon)
{
  myApiKey = apiKey;
  myLat = lat;
  myLon = lon;
}

time_t TimeDB::getTime()
{
  WiFiClient client;
  String apiGetData = "GET /v2.1/get-time-zone?key=" + myApiKey + "&format=json&by=position&lat=" + myLat + "&lng=" + myLon + " HTTP/1.1";
  Serial.println("Getting Time Data for " + myLat + "," + myLon);
  Serial.println(apiGetData);
  String result = "";
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println(apiGetData);
    client.println("Host: " + String(servername));
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  }
  else {
    Serial.println("connection for time data failed"); //error message if no client connect
    Serial.println();
    return 20;
  }

  while (client.connected() && !client.available()) delay(1); //waits for data

  Serial.println("Waiting for data");

  boolean record = false;
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    if (String(c) == "{") {
      record = true;
    }
    if (record) {
      result = result + c;
    }
    if (String(c) == "}") {
      record = false;
    }
  }
  client.stop(); //stop client
  Serial.println(result);

/*
  DynamicJsonDocument doc(result.length() + 2);
  DeserializationError error = deserializeJson(doc, result);
  
  switch (error.code()) {
    case DeserializationError::Ok:
        Serial.print(F("Deserialization succeeded"));
        break;
    case DeserializationError::InvalidInput:
        Serial.print(F("Invalid input!"));
        break;
    case DeserializationError::NoMemory:
        Serial.print(F("Not enough memory"));
        break;
    default:
        Serial.print(F("Deserialization failed"));
        break;
}
*/
  DynamicJsonDocument doc(2048); // doc(result.length() + 1);
  deserializeJson(doc, result);

  //JsonObject& root = json_buf.parseObject(jsonArray);
  
  localMillisAtUpdate = millis();
  Serial.print("Gelen Time : ");
  Serial.println(doc["timestamp"].as<long>());
  Serial.print("Gelen Statü : ");
  Serial.println(doc["status"].as<String>());
  Serial.print("Gelen Formatli : ");
  Serial.println((const char*)doc["formatted"]);
  Serial.print("Gelen countryCode : ");
  Serial.println((const char*)doc["countryCode"]);

  if (doc["timestamp"].as<long>() == 0) {
    return 20;
  } else {
    return (unsigned long) doc["timestamp"].as<long>();
  }
}

String TimeDB::getDayName() {
  switch (weekday()) {
    case 1:
      return "Pazar";
      break;
    case 2:
      return "Pazartesi";
      break;
    case 3:
      return "Salı";
      break;
    case 4:
      return "Çarşamba";
      break;
    case 5:
      return "Perşembe";
      break;
    case 6:
      return "Cuma";
      break;
    case 7:
      return "Cumartesi";
      break;
    default:
      return "";
  }
}

String TimeDB::getMonthName() {
  String rtnValue = "";
  switch (month()) {
    case 1:
      rtnValue = "Ock";
      break;
    case 2:
      rtnValue = "Şbt";
      break;
    case 3:
      rtnValue = "Mar";
      break;
    case 4:
      rtnValue = "Nis";
      break;
    case 5:
      rtnValue = "May";
      break;
    case 6:
      rtnValue = "Haz";
      break;
    case 7:
      rtnValue = "Tem";
      break;
    case 8:
      rtnValue = "Agu";
      break;
    case 9:
      rtnValue = "Eyl";
      break;
    case 10:
      rtnValue = "Eki";
      break;
    case 11:
      rtnValue = "Kas";
      break;
    case 12:
      rtnValue = "Ara";
      break;
    default:
      rtnValue = "";
  }
  return rtnValue;
}


String TimeDB::getAmPm() {
  String ampmValue = "ÖÖ";
  if (isPM()) {
    ampmValue = "ÖS";
  }
  return ampmValue;
}

String TimeDB::zeroPad(int number) {
  if (number < 10) {
    return "0" + String(number);
  } else {
    return String(number);
  }
}


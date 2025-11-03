#include <HTTPClient.h>

String http_get(String url){
  //String url = "http://temperature.local/?cmd=get;1";
  HTTPClient http;
  http.begin(url.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode>0) {
    //Serial.print("HTTP Response code: ");
    //Serial.println(httpResponseCode);
    String payload = http.getString();
    return (String(httpResponseCode)+":"+payload);
  }
  else {
    return ("Error code: "+String(httpResponseCode));
  }
}
void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void wserver_setup(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String msg="";//randomString(30,65, 122)+"\r\n"; //solo per test
    if (request->hasParam("cmd")) {
        String cmd = request->getParam("cmd")->value();
        msg=WS_cmd_interpreter(cmd);
    }
    else {
      msg=msg+WS_helpMessage();
    }
    //text/plain
    request->send(200, "text/plain", msg);
  });


  server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", get_weblog());
  });


    server.onNotFound(notFound);
    server.begin();
    otalog("wserver_setup(): executed !");
}
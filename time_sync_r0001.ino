

unsigned int TimeLastSetMillis=0;
boolean time_is_set=false;


void run_time_sync(unsigned int millis_delta_fra_sincronizzazioni){
  if (millis()>=TimeLastSetMillis+millis_delta_fra_sincronizzazioni){
    TimeLastSetMillis=millis();
    if (WiFi.status() == WL_CONNECTED){
      String http_got_string= http_get("http://timeserver.local/?cmd=getepochtime"); // 200:1709422319(+3600);1709422319(+3600)
      //Serial.println("run_time_sync():"+http_got_string);
      if (getValue(http_got_string, ':',0).equals("200")){ // 200 = ok
        String time_got=getValue(http_got_string, ':',1); // "1709405876(+3600);1709405876(+3600)" GPS:NTP
        //Serial.println("run_time_sync():"+time_got);
        String t_gps=getValue(time_got, ';',0); // "1709405876(+3600);"
        //Serial.println("run_time_sync():"+t_gps);
        String t_ntp=getValue(time_got, ';',1); // "1709405876(+3600)"
        //Serial.println("run_time_sync():"+t_ntp);

        if (String(t_gps).length()==17){ //se il gps funziona uso questo
          //Serial.println("run_time_sync()- uso GPS time");
          String epoch_s=getValue(t_gps, '(',0); //1709405876
          int epoch = epoch_s.toInt();
          //Serial.println("run_time_sync()-epoch="+String(epoch));
          String offset_s=getValue(t_gps, '(',1);
          //Serial.println("run_time_sync()"+String(offset_s));
          offset_s.replace("(","");
          //Serial.println("run_time_sync()"+String(offset_s));
          offset_s.replace("+","");
          //Serial.println("run_time_sync()"+String(offset_s));
          offset_s.replace(")","");
          //Serial.println("run_time_sync()"+String(offset_s));
          int offset=offset_s.toInt();
          //Serial.println("run_time_sync()-offset="+String(offset));
          int local_time_epoch=epoch+offset;
          //Serial.println("run_time_sync()-local_time_epoch="+String(local_time_epoch));
          time_object.setTime(local_time_epoch);
          time_is_set=true;
          otalog("run_time_sync(): refreshed time stamp from GPS="+time_object.getTimeDate());
          return;
        }


        if (String(t_ntp).length()==17){ //altrimenti uso ntp
          //Serial.println("run_time_sync()- uso NTP time");
          String epoch_s=getValue(t_ntp, '(',0); //1709405876
          int epoch = epoch_s.toInt();
          //Serial.println("run_time_sync()-epoch="+String(epoch));
          String offset_s=getValue(t_gps, '(',1);
          //Serial.println("run_time_sync()"+String(offset_s));
          offset_s.replace("(","");
          //Serial.println("run_time_sync()"+String(offset_s));
          offset_s.replace("+","");
          //Serial.println("run_time_sync()"+String(offset_s));
          offset_s.replace(")","");
          //Serial.println("run_time_sync()"+String(offset_s));
          int offset=offset_s.toInt();
          //Serial.println("run_time_sync()-offset="+String(offset));
          int local_time_epoch=epoch+offset;
          //Serial.println("run_time_sync()-local_time_epoch="+String(local_time_epoch));
          time_object.setTime(local_time_epoch);
          time_is_set=true;
          otalog("run_time_sync(): refreshed time stamp from NTP="+time_object.getTimeDate());
          return;
        }


      }
    }
  }
}

/* non usata
int getSecondsFromMidnight(){
  return time_object.getEpoch()%(3600*24);
  //return time_object.getHour()*3600+time_object.getMinute()*60+time_object.getSecond();
}
*/

String timestamp(){
  return String(time_object.getYear())+"-"+format_int(time_object.getMonth(),2)+"-"+format_int(time_object.getDay(),2)+" "+format_int(time_object.getHour(true),2)+":"+format_int(time_object.getMinute(),2)+":"+format_int(time_object.getSecond(),2); 
}

/*non usata
String formatted_time(int seconds_from_midnight){
  int sec_start=seconds_from_midnight%60;
  int min_start=((seconds_from_midnight-sec_start)%3600)/60;
  int hour_start=(seconds_from_midnight-min_start*60-sec_start)/3600;
  return format_int(hour_start,2)+":"+format_int(min_start,2)+":"+format_int(sec_start,2);
}
*/

String elapsed_time_online(){
  int dt=time_object.getEpoch()-boot_time;
  int days=dt/86400;
      dt=dt%86400;
  int hours=dt/3600;
      dt=dt%3600;
  return (String(days)+" days, "+String(hours)+" hours, "+String(dt/60)+" minutes, "+String(dt%60)+" secs");
}

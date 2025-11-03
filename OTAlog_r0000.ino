String weblog_s="";
int max_len_weblog=10000; //max caratteri da tenere in memoria



void otalog(String s){
  String new_input_s=timestamp()+"->"+s;
  Serial.println(new_input_s);
  String new_input_html="    <p  style=\"height:2px;\">"+new_input_s+"</p>\r\n";
  int eccedenza=weblog_s.length()+new_input_html.length()-max_len_weblog; //calcolo l'eccedenza ipotetica (se aggiungessi la stringa)
  //Serial.println("eccedenza1="+String(eccedenza));
  if (eccedenza>0){
    weblog_s=weblog_s.substring(0,weblog_s.length()-eccedenza);
    eccedenza=weblog_s.length()+new_input_html.length()-max_len_weblog; //ricalcolo eccedenza
    //Serial.println("eccedenza2="+String(eccedenza));
    //if (!weblog_s.startsWith("    <p")){
    String xx ="<\/p>\r\n";
    if (!weblog_s.endsWith(xx)){
      int pos=weblog_s.lastIndexOf(xx);
      //weblog_s=weblog_s.substring(pos_p); //elimino una riga
      weblog_s.remove(pos+xx.length());
      //weblog_s=weblog_s.substring(0,weblog_s.length());
    }

  }
  weblog_s=new_input_html+weblog_s;
  //Serial.println("----");
  //Serial.print(weblog_s);
  //Serial.println("***");
}

String get_weblog(){
  String html1="";
  html1=html1+"<!DOCTYPE html>";
  html1=html1+"<html>";
  html1=html1+"<head>";
  html1=html1+"    <title>weblog<\/title>";
  html1=html1+"    <meta http-equiv=\"refresh\" content=\"5\">";
  html1=html1+"<\/head>";
  html1=html1+"";
  html1=html1+"<body>";

  //String body=+"    <p>---WEBLOG---</p>\r\n";

  String html2="";
  html2=html2+"<\/body>";
  html2=html2+"<\/html>";
  
  String res=html1+weblog_s+html2;
  

  return res;
  }

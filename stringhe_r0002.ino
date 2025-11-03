

//https://stackoverflow.com/questions/9072320/split-string-into-string-array
// 1st string --> index=0
// il separatore va messo con single quote (es. ';') e non con virgolette doppie, se no il compilatore lo vede come stringa e non come char e da errore
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// funzione esclusa perchè non usata, benchè funzionante
String randomString(int L,int minASCI, int maxASCII){
  String res="";
  for (int i=0;i<L;i++){
    res+=String(char(random(minASCI,maxASCII+1)));
  }
  return res;
}


//crea stringa aggiungendo zeri a sx per arrivare alla lunghezza
String format_int(int n, int len){
  String res=String(n);
  while(res.length()<len) res="0"+res;
  return res;
}

String allunga(String s, int lunghezza, String carattere){
  String res=s;
  if (carattere.length()>0){
    while (res.length()<lunghezza) res=res+carattere;
  }
  return res;
}

String tronca(String s, int lunghezza){
  return s.substring(0,lunghezza);
}


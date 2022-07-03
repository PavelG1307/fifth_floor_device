void onConnectionEstablished() {
  mqtt = true;
  client.subscribe("torsh/brt", [] (const String & payload)  {
    brt(payload);

  });

  client.subscribe("torsh/req", [] (const String & payload)  {
    if (payload == "stt") client.publish("torsh/stt", STT());
    
    if (payload.indexOf("rst") >= 0) {
      client.publish("torsh/err", "Reset!");
      rstf();
    }
    
    if (payload.indexOf("rmvsns") >= 0) {
      idS = payload.substring(7, 11).toInt();
      for (int i = 0; i < CountSens; i++) {
        if (sens[i].id == idS) {
          flsens = true;
          CountSens--;
        }
        if (flsens) {
          sens[i].id = sens[i + 1].id;
          sens[i].value = sens[i + 1].value;
          sens[i].type = sens[i + 1].type;
          sens[i].timereq = sens[i + 1].timereq;
        }
      }
    }

    if (payload.indexOf("rng") >= 0) {
      byte count = payload.substring(3, 4).toInt();
      for (int k = 0; k < 5; k++) {
        alarm[k].act = false;
      }
      if (count != 0) {
        for (int i = 0; i < count; i++) {
          int j = payload.substring(4 + i * 7, 5 + i * 7).toInt();
          alarm[j].time = payload.substring(5 + i * 7, 9 + i * 7).toInt();
          alarm[j].music = payload.substring(9 + i * 7, 10 + i * 7).toInt();
          if (payload.substring(10 + i * 7, 11 + i * 7) == "1") {
            alarm[j].sinerise = true;
          }
          else {
            alarm[j].sinerise = false;
          }
          alarm[j].act = true;

          if (alarm[j].act) {
            Serial.print("Set ring number: ");
            Serial.println(j);
            Serial.print("On: ");
            Serial.println(time_form(alarm[j].time * 60));
            Serial.print("music:");
            Serial.println(alarm[j].music);
            Serial.print("sunlight:");
            Serial.println(alarm[j].sinerise);
            Serial.print("до будильника:");
            Serial.println(alarm[j].time - (int) timevalue / 60);
          }
        }
      }
    }
  });

//  client.subscribe("torsh/sens", [] (const String & payload)  {
//  });

  client.subscribe("torsh/mode", [] (const String & payload)  {
    currentMode = payload.toInt();
    if (brightnessls == 0) {
      brightnessls = 30;
    }
    FastLED.setBrightness(brightnessls);
    counter = 0;
  });
  client.subscribe("torsh/modesettings", [] (const String & payload)  {
    mds(payload);
  });

  client.subscribe("torsh/spq", [] (const String & payload)  {
    volume=(byte) payload.toInt();
    
  });
}

void mds(String p) {
  if (currentMode == 1) {
    //    r = (p.substring(0, p.lastIndexOf(':') - 1)).toInt;
    //    g = (p.substring(p.indexOf(':' + 1), p.lastIndexOf(':') - 1)).toInt;
    //    b = (p.substring(p.lastIndexOf(':') + 1, p.length)).toInt;
    //    Serial.println(r);
    //    Serial.println(g);
    //    Serial.println(b);
  }
}

void brt(String p) {
  brightnessls = p.toInt();
  FastLED.setBrightness(brightnessls * 3);
  FastLED.show();
  Serial.println("Brithness: ");
  Serial.println(brightnessls);
  //  if (brightnessls == 0) {
  //    digitalWrite(2, HIGH);
  //  } else {
  //    digitalWrite(2, LOW);
  //  }
  //  FastLED.setBrightness(brightnessls * 3);
  //  FastLED.show();
}


String STT() {
  String answ = "";
  answ += (String) id + " ";

  answ += key() + " ";
  answ += (String) brightnessls + " ";
  int count = 0;
  for (int i = 0; i < 5; i++) {
    if (alarm[i].act) {
      answ += (String) (i + 1);
      answ += ",";
    } else {
      count++;
    }
  }
  if (count != 5) {
    answ.remove(answ.length() - 1);
  } else {
    answ += "0";
  }
  answ += " ";

  for (int i = 0; i < 5; i++) {
    if (alarm[i].act) {
      answ += (String) alarm[i].time;
      answ += ",";
    }
  }


  if (count != 5) {
    answ.remove(answ.length() - 1);
  }  else {
    answ += "0";
  }
  answ += " ";

  for (int i = 0; i < 5; i++) {
    if (alarm[i].act) {
      if (alarm[i].sinerise) {
        answ += "true";
        answ += ",";
      } else {
        answ += "false";
        answ += ",";
      }
    }
  }

  if (count != 5) {
    answ.remove(answ.length() - 1);
  }  else {
    answ += "false";
  }
  answ += " ";

  for (int i = 0; i < 5; i++) {
    if (alarm[i].act) {
      answ += (String) alarm[i].music;
      answ += ",";
    }
  }
  if (count != 5) {
    answ.remove(answ.length() - 1);
  }  else {
    answ += "0";
  }
  answ += " ";

  if (nghtlight) {
    answ += "true ";
  } else {
    answ += "false ";
  }
  answ += (String) timeonhrs;
  answ += ",";
  answ += (String) timeonmnts;
  answ += ",";
  answ += (String) timeoffhrs;
  answ += ",";
  answ += (String) timeoffmnts;
  answ += " ";

  if (speaker) {
    answ += "true ";
  } else {
    answ += "false ";
  }
  answ += "55 ";

  answ += (String) ((int) (analogRead(0) * 165) / 274) + " ";
  answ += (String) currentMode;
  answ += " ";
  answ += (int) timevalue/60;
  Serial.println(answ);
  return answ;
}

void rstf() {
  EEPROM.write(0, 0);
  EEPROM.commit();
  delay(500);
  EEPROM.end();
  Serial.println("Reset");
  resetFunc();
}

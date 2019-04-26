#include <Controller.h>


void progress(DlState state, int percent)
{
  Log.notice("state = %d - percent = %d\n", state, percent);
}

void error(char *message) {
  Log.error(message);
}

void startDl(void) {
}

void endDl(void) {
}

void Modules::addModule(Module *module) {
  modules[length++] = module;
}

uint8_t Modules::count() {
  return length;
}

Module *Modules::getAt(uint8_t index) {
  return modules[index];
}

void Modules::setup() {
  for (int i = 0; i < length; i++) {
    modules[i]->setup();
  }  
};

void Modules::loop() {
  for (int i = 0; i < length; i++) {
    modules[i]->loop();
  }  
};

void Modules::every10Milliseconds() {
  for (int i = 0; i < length; i++) {
    modules[i]->every10Milliseconds();
  }  
};

void Modules::every50Milliseconds() {
  for (int i = 0; i < length; i++) {
    modules[i]->every50Milliseconds();
  }  
};

void Modules::every100Milliseconds() {
  for (int i = 0; i < length; i++) {
    modules[i]->every100Milliseconds();
  }  
};

void Modules::everySecond() {
  for (int i = 0; i < length; i++) {
    modules[i]->everySecond();
  }  
};

void Modules::getTelemetryData(char *targetBuffer) {
  char telemetryBuffer[1024] = {0};
  strcpy(telemetryBuffer, "{");
  int currentLength = 1;
  for (int i = 0; i < length; i++) {
    strcpy(telemetryBuffer + currentLength, "\"");
    currentLength += 1;

    const char *moduleName = modules[i]->getName();
    strcpy(telemetryBuffer + currentLength, moduleName);
    currentLength += strlen(moduleName);

    strcpy(telemetryBuffer + currentLength, "\":");
    currentLength += 2;

    char moduleBuffer[100] = {0};
    modules[i]->getTelemetryData(moduleBuffer);
    strcpy(telemetryBuffer + currentLength, moduleBuffer);
    currentLength += strlen(moduleBuffer);

    if (i != length-1) {
      strcpy(telemetryBuffer + currentLength, ",");
      currentLength += 1;
    }
  }
  strcpy(telemetryBuffer + currentLength, "}");
  strcpy(targetBuffer, telemetryBuffer);
};

void Controller::commandReceived(const char *command, const char *payload) {
  if (strcmp(OTA_TOPIC, command) == 0) {
    memset(url, 0, 100);
    memset(md5_check, 0, 50);
    char *tmp = strstr(payload, "url:");
    char *tmp1 = strstr(payload, ",");
    if (!tmp || !tmp1) {
      Log.warning("unsupported payload format!\n");
      return;
    }
    memcpy(url, tmp + strlen("url:"), tmp1 - (tmp + strlen("url:")));

    char *tmp2 = strstr(payload, "md5:");
    int length = strlen(payload);
    memcpy(md5_check, tmp2 + strlen("md5:"), length - (tmp2 + strlen("md5:") - &payload[0]));

    Log.notice("started fota url: %s\n", url);
    Log.notice("started fota md5: %s\n", md5_check);
    state = Fota_e;
  }
}


Controller::Controller()
{
  // put your setup code here, to run once:
  Serial.begin(BAUD_RATE);
  Log.begin(LOG_LEVEL_TRACE, &Serial);

  prefs = Prefs::getInstance();
  Log.notice("Controller Prefs %d\n", prefs);
  ledController = LedController::getInstance();

  // register prefs
  prefs->registerConfigParam("ConfigPortalPin", "Config-Portal Pin", "32", 3, this);

  // create all modules 
  networkControl = NetworkControl::getInstance();
  modules.addModule(networkControl);
  modules.addModule(new HeatingController());
  modules.addModule(new DisplayControl());

  setup();
}

void Controller::setup() {
  enterConfigPortalPin = prefs->getInt("ConfigPortalPin");
  pinMode(enterConfigPortalPin, INPUT_PULLUP);  

  networkControl->subscribeToCommand(OTA_TOPIC, this);

  for (int i = 0; i < modules.count(); i++) {
    modules.getAt(i)->setup();
  }
}


void Controller::loop()
{
  unsigned long currentMillis = millis();

  switch (state)
  {
  case Runnning_e:

      // is configuration portal requested?
    /*if ( digitalRead(enterConfigPortalPin) == LOW) {
      networkControl->enterConfigPortal();

      //if you get here you have connected to the WiFi
      Log.notice("connected...yeey :)\n");
    }*/

    modules.loop();

    if (currentMillis > timer) {
        timer = timer + LOOP_INTERVAL_IN_MS;
        modules.every10Milliseconds();

        // every 50 milliseconds
        if (loopCounter % 5 == 0) {
          modules.every50Milliseconds();
        } 

        // every 100 milliseconds
        if (loopCounter % 10 == 0) {
          modules.every100Milliseconds();
        }

        // every second
        if (loopCounter % 100 == 0) {
          modules.everySecond();
        }

        if (loopCounter % 1000 == 0) {
          char telemetryData[1024];
          modules.getTelemetryData(telemetryData);
          networkControl->sendTelemetry(telemetryData);
        }

        loopCounter++;
    }    

    break;

  case Fota_e:
    ledController->blinkSlow();

    DlInfo info;
    char urlStr[100];
    strcpy(urlStr, url);
    info.url = urlStr;
    char md5Str[50];
    strcpy(md5Str, md5_check);
    info.md5 = md5Str;
    info.startDownloadCallback = startDl;
    info.endDownloadCallback = endDl;
    info.progressCallback = progress;
    info.errorCallback = error;
    info.caCert = NULL;
    Log.notice("starting ota update!\n");
    httpFOTA.start(info);

    networkControl->send(OTA_TOPIC, "ok");
    break;
  default:
    break;
  }
}

void Controller::configUpdate(const char *id, const char *value) {
    enterConfigPortalPin = atoi(value);
}

void Controller::getTelemetryData(char *targetBuffer) {
  sprintf(targetBuffer, "\"\"");
}
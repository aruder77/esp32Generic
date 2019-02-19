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


void Controller::commandReceived(const char *command, const char *payload) {
  if (strcmp(OTA_TOPIC, command) == 0)
  {
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

const char *Controller::getTelemetryData() {
	return "";
}


Controller::Controller()
{
  // put your setup code here, to run once:
  Serial.begin(BAUD_RATE);
  Log.begin(LOG_LEVEL_TRACE, &Serial);

  pinMode(INTERRUPT_PIN, INPUT_PULLUP);  

  networkControl = NetworkControl::getInstance();
  prefs = Prefs::getInstance();
  ledController = LedController::getInstance();
}

void Controller::loop()
{
  switch (state)
  {
  case Runnning_e:

      // is configuration portal requested?
    if ( digitalRead(INTERRUPT_PIN) == LOW ) {
      ledController->blink();

      networkControl->enterConfigPortal();

      //if you get here you have connected to the WiFi
      Log.notice("connected...yeey :)\n");
      ledController->on();     
    }

    networkControl->loop();
    break;
  case Fota_e:
    DlInfo info;
    info.url = url;
    info.md5 = md5_check;
    info.startDownloadCallback = startDl;
    info.endDownloadCallback = endDl;
    info.progressCallback = progress;
    info.errorCallback = error;
    info.caCert = (char *)root_ca;
    httpFOTA.start(info);

    networkControl->send(OTA_TOPIC, "ok");
    break;
  default:
    break;
  }
}

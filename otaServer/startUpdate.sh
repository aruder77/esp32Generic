cp ../.pioenvs/esp32doit-devkit-v1/firmware.bin data/
mosquitto_pub -h 192.168.178.27 -t cmnd/esp32generic/ota -m "url:http://192.168.178.20/firmware.bin,md5:xy"
cp ../.pioenvs/esp-wrover-kit/firmware.bin data/
mosquitto_pub -h 192.168.178.27 -t cmnd/heatmaster/ota -m "url:http://192.168.178.20:8080/firmware.bin,md5:xy"
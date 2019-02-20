#!/bin/bash
docker run -d -p 8080:80 -v /Users/axel/Documents/Projects/Arduino/esp32Generic/otaServer/data:/usr/share/nginx/html:ro nginx:stable 

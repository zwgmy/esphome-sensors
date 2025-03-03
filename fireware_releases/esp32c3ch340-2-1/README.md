```
ota:
#  platform: esphome
#  password: "a407b7e58f0ce0833271a46bd8296ade"
  - platform: http_request

http_request:
  timeout: 60s
  verify_ssl: false

update:
  - platform: http_request
    name: Firmware Update
    source: https://raw.githubusercontent.com/zwgmy/esphome-sensors/refs/heads/main/fireware_releases/esp32c3ch340-2-1/esp32c3ch340-2-1.json            
    update_interval: 12h
```

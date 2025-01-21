
Mysentech(敏源传感)M1820Z温度传感器接入esphome
转载https://bbs.hassbian.com/thread-26116-1-1.html
```c
external_components:
  - source:
      type: git
      url: https://github.com/zwgmy/esphome-sensors
#      ref: dev components/mysentech_m601_m1601_m1820  
    components: [ mysentech_m601_m1601_m1820]
    
one_wire:
  - platform: gpio
    pin: GPIO2
    id: bus1
    
sensor:
  - platform: mysentech_m601_m1601_m1820
    
    address: 0x0000f45e7710e528 #0x0000b4650b435728
    name: "M1820Z Temperature"
    id: my_sensor
    one_wire_id: bus1
    accuracy_decimals: 2
    update_interval: 1s
    repeatability: 2
```

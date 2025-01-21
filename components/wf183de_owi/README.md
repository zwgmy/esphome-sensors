example for reference：
```c
external_components:
  - source:
      type: git
      url: https://github.com/zwgmy/esphome-sensors
    components: [wf183de_owi]

one_wire:
  - platform: gpio
    pin: GPIO2
    id: bus1

    
sensor:
  - platform: wf183de_owi
    address: 0x1234567812345628  # 替换为您实际的传感器地址
    temperature:
      name: "WF183DE Temperature"
    pressure:
      name: "WF183DE Pressure"
    update_interval: 60s
```   

##wf180de i2c 
external_components:
  - source:
      type: git
      url: https://github.com/zwgmy/esphome-sensors
    refresh: 10s
    components: [wf183de_i2c]


sensor:

  - platform: wf183de_i2c
    temperature:
      name: "WF183DE Temperature"
    pressure:
      name: "WF183DE Pressure"
    update_interval: 5s    

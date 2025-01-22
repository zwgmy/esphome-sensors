```c
# External components
external_components:
  - source:
      type: git
      url: https://github.com/zwgmy/esphome-sensors
    components: [wf183de_uart]

# Configure UART bus
uart:
  - id: uart_bus
    tx_pin: GPIO1
    rx_pin: GPIO3
    baud_rate: 9600
    stop_bits: 1
    data_bits: 8
    parity: NONE

# Configure WF183DE UART sensor
sensor:
  - platform: wf183de_uart
    uart_id: uart_bus
    temperature:
      name: "WF183DE Temperature"
    pressure:
      name: "WF183DE Pressure"
    update_interval: 60s
```

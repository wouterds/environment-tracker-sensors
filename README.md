# environment-tracker-arduino

Arduino (NodeMCU ESP-12E module) code for the sensors API

## Hardware

### Setup

![Hardware Setup](https://i.imgur.com/mS9Tymc.gif)

### Sensors

- BME280 (temperature, pressure, humidity)
- HTU21DF (temperature & humidity; but more accurate than the BME280)
- TSL2561 (illuminance; full spectrum, ir spectrum & visible spectrum)

## Output

When browsing to the IP of the module you will see a similar output as below.

```json
{
  "illuminance": {
    "visible": 905,
    "full": 1255,
    "ir": 347
  },
  "temperature": 24.51,
  "humidity": 37.49,
  "pressure": 1027.82
}
```

But there's also other routes to fetch raw values separately & disable / enable the LED.

- `GET /`
- `GET /illuminance/full`
- `GET /illuminance/visible`
- `GET /illuminance/ir`
- `GET /temperature`
- `GET /humidity`
- `GET /pressure`
- `POST /led` // body payload: enabled: 0 | 1

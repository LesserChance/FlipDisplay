# Flip Display

https://user-images.githubusercontent.com/819400/149609962-681a9211-6ebb-46da-95a2-772721c91d99.mp4

This is a 3D-printed flip character display. Each character utilizes a position sensor for loop detection in order to ensure accuract positioning of character display. The display is capable of a few different animation styles, e.g. all arriving at the same time, or cascading left-to-right. It has a simple web-based UI and API, and is fairly easy to construct.

## UI
The ESP32 hosts a web server that serves a simple web page and API that allows you to update the display.

![UI](https://raw.githubusercontent.com/LesserChance/FlipDisplay/main/img/UI.png)

## Electronics
There are only a few different major components: A buck convertor, an ESP32, and an A4988 Motor driver (per character)

### PCB: Main
The main panel is used as the power bus and ESP32 break out. It connects each motor step pin and loop detection button pin to an individual GPIO pin, and connects all driver enable pins to a single GPIO pin. There is also a section for screw terminals for easy access to all ESP32 pins, as well as power.

![Main PCB](https://raw.githubusercontent.com/LesserChance/FlipDisplay/main/img/PCB-Main.png)

### PCB: Motor
One motor panel exists per character. This acts as the interface between the A4988 Motor Driver, the motor, and the ESP32.

![Motor PCB](https://raw.githubusercontent.com/LesserChance/FlipDisplay/main/img/PCB-Motor.png)

## Models
![models](https://raw.githubusercontent.com/LesserChance/FlipDisplay/main/img/Model-Character.png)

The character models are designed to be nested against one another and to minimize the distance between characters as much as possible.

### Assembly 1: The Barrel
The barrel and the stator connect to one another, and the flaps are placed within the holes.

### Assembly 2: The Bracket
A motor is mounted to the bracket. The button holder is connected to the two front bottom holes on the motor and a leaf switch button is glued on. The barrel assembly is connected to the motor with a coupler so that it rotates around the motor housing. A stopper is fastened into the top of the bracket, which is used to stop the letters at a certain position

### Assembly 3: Circuit
The circuit assembly is just used to house the individual PCBs.

---

Originally inspired from designs by [hakernia on Thingiverse](https://www.thingiverse.com/thing:2369832), but I found that the small motor was not driving well, as it'd get hot and fail, or get stuck easily. So I upgraded to heavier duty Nema17 Motorslike the ones used in 3D Printers and decided to use an actual coupler to connect the 3d printed parts to the motor.

## Supplies and Costs
A 6 character Flip Display will cost about $170.00 USD for all the raw materials, then additional costs based on your enclosure.  

Filament: $40.00 USD  
Electronics: $100.00 USD  
PCBs: $30.00 USD  

### Bill of Materials
All models were printed using [MatterHackers Build Series PLA](https://www.matterhackers.com/store/l/175mm-pla-filament-black-1-kg/sk/MY6CYEZM) on a Creality 10s Pro V2.
| Part | Supply | Cost | Count |
|----------|----------|----------|----------|
|Black PLA|[MatterHackers](https://www.matterhackers.com/store/l/175mm-pla-filament-black-1-kg/sk/MY6CYEZM)|$19.00|1|
|White PLA|[MatterHackers](https://www.matterhackers.com/store/l/175mm-pla-filament-white-1-kg/sk/MEEDKTKU)|$19.00|1|

Electronics and hardware were sourced through a few different providers.

| Component | Part | Supply | Cost | Count |
|----------|----------|----------|----------|----------|
|-|12v Power adaptor|[Amazon](https://www.amazon.com/gp/product/B07FFKL7CR/ref=ppx_yo_dt_b_asin_title_o05_s00)|$19.00|1|
|Main|Buck Convertor|[Amazon](https://www.amazon.com/gp/product/B07QKHR6PY/ref=ppx_yo_dt_b_asin_title_o05_s00)|$1.50|1|
|Main|ESP32|[Amazon](https://www.amazon.com/gp/product/B08246MCL5/ref=ppx_yo_dt_b_asin_title_o04_s00)|$7.00|1|
|Main|Panel Mount Micro USB Extension|[Amazon](https://www.amazon.com/gp/product/B082HZKVP1/ref=ppx_yo_dt_b_asin_title_o05_s01)|$11.00|1 (optional)|
|Main|15 Pin Header|[Tayda](https://www.taydaelectronics.com/15-pin-2-54-mm-single-row-female-pin-header.html)|$0.12|2|
|Main|Screw Terminal|[Tayda](https://www.taydaelectronics.com/dg300-screw-terminal-block-3-positions-5mm.html)|$0.14|A bunch|
|Motor|Nema17 Stepper Motor|[Amazon](https://www.amazon.com/gp/product/B07THK8RS1/ref=ppx_yo_dt_b_asin_title_o01_s00)|$7.40|1 per character|
|Motor|A4988 Motor Driver|[Amazon](https://www.amazon.com/gp/product/B092PZNFNS/ref=ppx_yo_dt_b_asin_title_o05_s00)|$1.40|1 per character|
|Motor|Coupler|[Amazon](https://www.amazon.com/gp/product/B0852P8WSH/ref=ppx_yo_dt_b_asin_title_o09_s00)|$0.90|1 per character|
|Motor|10mm x 26mm x8mm Bearing|[Digikey](https://www.digikey.com/en/products/detail/mechatronics-bearing-group/6000-2RS/9608370)|$0.85|1 per character|
|Motor|Leaf Switch|[Mouser](https://www.mouser.com/ProductDetail/612-MS0850506F020V2A)|$1.33|1 per character|
|Both|DC Power Jack|[Tayda](https://www.taydaelectronics.com/dc-power-jack-2-1mm-enclosed-frame-with-switch.html)|$0.13|1|
|Both|8 Pin Header|[Tayda](https://www.taydaelectronics.com/8-pin-2-54-mm-single-row-female-pin-header.html)|$0.08|A bunch|
|Both|2 Pin JST Connector|[Tayda](https://www.taydaelectronics.com/2-pins-jst-xh-2-54-male-connector-straight-180-degree.html)|$0.01|A bunch|
|Both|3 Pin JST Connector|[Tayda](https://www.taydaelectronics.com/3-pins-jst-xh-2-54-male-connector-straight-180-degree.html)|$0.02|A bunch|
|Both|5 Pin JST Connector|[Tayda](https://www.taydaelectronics.com/5-pins-jst-xh-2-54-male-connector-straight-180-degree.html)|$0.03|A bunch|

All PCBs were printed using [JLCPCB](https://jlcpcb.com/).  
| Part | Supply | Cost | Count |
|----------|----------|----------|----------|
|Main PCB|[JLCPCB](https://jlcpcb.com/) (requires SMT assembly)|$9.00|1|
|Motor PCB|[JLCPCB](https://jlcpcb.com/)|$1.70|1 per character|

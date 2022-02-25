# NodeMCU-32s Garage Controller

<img src="/images/controller.jpg" width=600px>

This project is still a WIP, so the code is yet to be optimized. The goal is to turn my two regular garage doors into "smart" garage doors. This controller will be the base platform using a NodeMCU-32s (or similarly cloned SoC).
  
## Sensors/Devices Attached
 
There are multiple things connected to this control board, with more to be added in the future. 
  
- Two magnetic reed switches
  - Similar item on Amazon: [Gikfun MC-38 Wired Door Sensor Magnetic Switch](https://www.amazon.com/dp/B0154PTDFI?psc=1&ref=ppx_yo2_dt_b_product_details)

- Two 5mm green LEDs (you can use pre-soldered LEDs with resistors, I believe mine were 460 ohm.)
  - Similar item on Amazon: [EDGELEC 56pcs (7 Colors x 8pcs)](https://www.amazon.com/EDGELEC-LED-Emitting-Assorted-Warm-White/dp/B07W4H66LR/)

- One 2-channel 5VDC relay module (high trigger)
  - Similar item on Amazon: [2 pcs 5V 2 Channel DC 5V Relay Module with Optocoupler High/Low Level Trigger](https://www.amazon.com/dp/B079FGPC9Y?psc=1&ref=ppx_yo2_dt_b_product_details)

## Additional Materials

These are other things that I will be using in this project.

- Speaker wire
  - Run from the relay to the garage door button sockets.
  - Similar item on Amazon: [TYUMEN 100FT 16 Gauge 2pin 2 Color Red Black Cable](https://www.amazon.com/dp/B07SG23DT1?psc=1&ref=ppx_yo2_dt_b_product_details)

- Printed circuit board
  - Just a generic PCB large enough to accommodate solder points for LEDs, relay leads, etc.
  - Similar item set on Amazon: [ELEGOO 32 Pcs Double Sided PCB Board Prototype Kit](https://www.amazon.com/dp/B072Z7Y19F?psc=1&ref=ppx_yo2_dt_b_product_details)

- 3D printer filament
  - The case for this project has been 3D printed, and I have provided the models along with the codebase. Please see the [Enclosure](#enclosure) section below.

## Schematic

![schematic.png](/images/schematic.png "Basic Schematic")

Here is the schematic (created using Fritzing). Please note, the schematic does not have any resistors for the LEDs. This is because the LEDs I am using are pre-wired, and already have the correct resistors added for this voltage/amperage. Just don't forget to add some to your LEDs if you are not using pre-wired LEDs.

## Enclosure

![case_design_v2.png](/images/case_design_v2.png "Fusion 360 model - v2")

In this project, you will find the [STL files](/enclosure/STL/) for the top and bottom of the case, as well as, the [Fusion 360 model](/enclosure/Fusion%20360/) file. These can be used to 3D print the enclosure, or can be modified to fit your own project. It was designed around the 7cm x 9cm PCB linked in the [Additional Materials](#additional-materials) section above.

### 3D Print Settings

---

I have a Creality Ender 3 v2 with an upgraded Micro Swiss all-metal hotend, and these are the settings I used to print the enclosure (and most other things) in the [Cura slicing software](https://ultimaker.com/software/ultimaker-cura).

- Quality
  - Layer Height: 0.16 mm
  - Initial Layer Height: 0.2 mm
  - Line Width: 0.4 mm
  - Initial Layer Line Width: 100%
- Walls
  - Wall Thickness: 1.2 mm
    - Wall Line Count 3
  - Optimize Wall Printing Order: YES
  - Fill Gaps Between Walls: Everywhere
  - Horizontal Expansion: 0 mm
- Top/Bottom
  - Top/Bottom Thickness: 0.84 mm
  - Monotronic Top/Bottom Order: NO
  - Enable Ironing: NO
- Infill
  - Infill Density: 15%
    - Infill Line Distance 8.0 mm
  - Infill Pattern: Cubic
- Material (PLA)
  - Printing Temperature: 200° C
  - Build Plate Temperature: 60° C
- Speed
  - Print Speed: 50 mm/s
    - Infill Speed: 50 mm/s
    - Wall Speed: 25 mm/s
  - Travel Speed: 150 mm/s
  - Initial Layer Speed: 20 mm/s
- Travel
  - Enable Retraction: YES
  - Retract Layer Change: NO
  - Retraction Distance: 5 mm
  - Retraction Speed: 45 mm/s
  - Avoid Printed Parts When Traveling: YES
  - Avoid Supports When Traveling: YES
  - Travel Avoid Distance: 0.625 mm
  - Z Hop When Retracted: NO
- Cooling
  - Enable Print Cooling: YES
  - Fan Speed 100%
  - Regular/Maximum Fan Speed Threshold: 10s
  - Life Head: NO
- Support
  - Generate Support: NO
- Build Plate Adhesion
  - Build Plate Adhesion Type: None

## Home Assistant Configuration

To use this controller with [Home Assistant](https://home-assistant.io) you will need to configure an MQTT Broker. Here is a guide on configuring the integrated [MQTT Broker](https://www.home-assistant.io/docs/mqtt/broker/).

Once you have configured an MQTT Broker for use in Home Assistant, you will need to add content below to your `configuration.yaml`, then restart Home Assistant.

```yaml
cover:
  - platform: mqtt
    command_topic: "gumby_garage/door_1_toggle"
    name: "Garage Door 1"
    payload_close: "close"
    payload_open: "open"
    device_class: garage
    position_topic: "gumby_garage/door_1_pos"
    unique_id: "garage_door_1"
  - platform: mqtt
    command_topic: "gumby_garage/door_2_toggle"
    name: "Garage Door 2"
    payload_close: "close"
    payload_open: "open"
    device_class: garage
    position_topic: "gumby_garage/door_2_pos"
    unique_id: "garage_door_2"
```

Home Assistant should now be subscribed to the above topics, and will update the `cover.garage_door_1` and `cover.garage_door_2` objects with their statuses.

These can now be used with the automations provided in the [automations.yaml](/home-assistant/automations.yaml) file.
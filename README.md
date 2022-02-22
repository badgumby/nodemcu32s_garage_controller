# NodeMCU-32s Garage Controller

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
  - The case for this project has been 3D printed, and I have provided the models along with the codebase. Please see the [Enclosure](##enclosure) section below.

## Enclosure

![case_design_v2.png](/images/case_design_v2.png "Fusion 360 model - v2")

In this project, you will find the [STL files](/Enclosure/STL/) for the top and bottom of the case, as well as, the [Fusion 360 model](/Enclosure/Fusion%20360/) file. These can be used to 3D print the enclosure, or can be modified to fit your own project. It was designed around the 7cm x 9cm PCB linked in the [Additional Materials](#additional-materials) section above.

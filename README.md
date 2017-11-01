# HeartOfTeFiti
A 3d Printed Amulet with Neopixels controlled by an Arduino, Synchronized with other via NRF24 Radio.

When the button is pushed the arduino chooses a random light pattern and displays it.  While displaying it broadcasts each pixel value using the nrf24 radio to any nearby units as well.  Nearby units will receive the pixel instructions and replicate the pattern.  When the pattern ends there is a 5 second interval before another pattern can be begun (by any which participated in the previous pattern).

Components
* Shell/Cover (3d printed, models included)
* Arduino Pro Mini 3.3v
* NRF24001+ Transciever
* 5x Addressable LED
* 2x 10uf Capacitors (Optional but recommended)
* 2x 470 ohm resistors (Optional but recommended)
* wire, Recommend 30awg solid
* momentary button switch
* 3.7v 370mah li-poly Battery
* 2x M3 x 10mm Hex Head Bolt

For Assembly
 * Solder
 * Hot Glue (Optional)
 * Necklace
 
![Demo](/img/VID_20171031_155220.mp4?raw=true "Demo")

![Alt text](/img/MVIMG_20171031_203501.jpg?raw=true "Front")

![Alt text](/img/MVIMG_20171031_203534.jpg?raw=true "Back")

![Alt text](/img/MVIMG_20171031_203648.jpg?raw=true "Apart")

![Alt text](/img/MVIMG_20171031_203709.jpg?raw=true "Internals")

# GROWTH
Edge computing code to solve World Hunger. GROWTH represents an idea me and 3 others came up with to improve urban farming. It is an IOT scalable vertical garden composed of sensors which in turn tell a few actuators what to do.
(1) Soil moisture sensors inform the drip water/fertilizer irrigation system.
(2) DHT11 temp and humidity sensors inform the dehumidification system and
(3) A lighting system composed of UV LEDs, blue and red LEDs to control the plant growth versus fruit yield when programmatically adjusted per needs of consumer.

During development and debugging of this code I discovered several ways to attempt to fit more sensor inputs on a single microcontroller. The Arduino MEGA is a great place to start, however even with NANO we could still implement an I2C serial communication solution that works by connecting and getting several microcontrollers to talk to each other as did the first computers in a bus or star network topology.

This project has taught me the value of the vegetables we grow and eat, however to become fully scalable some challenges must be addressed. 
1. Electricity to power the LEDs must be subsidized or perhaps converted from renewable or nuclear supply.
2. When choosing LEDs always choose based on PPFD (Photosynthetic Photon Flux Density) not some other measure such as Power Wattage or Lux (Luminosity)

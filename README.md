# 16 Channel PT1000 Reader

This project describes an Arduino shield which allows the reading of up to 16 PT1000 2 wire RTD temperature sensors, and communicate that information to a computer. It also has two digital outputs, designed to control relays which can turn heaters on and off. This design allows the Arduino to autonomously control the heaters, ensuring consistent temperature control even if the main computer loses connection.

<p align="center">
  <img src="images/eagle.png" width="700" />
</p>

## Getting Started

### Materials

To make the Arduino shield, you need the following items:

- 1 Arduino Due or Arduino Mega (with corresponding cable, and power supply if you'd like continued operation in the event of a computer failure)
- 1 Printed Circuit Board (see .brd file)
- 1 LM324 Op Amp chip [link](https://www.digikey.com/product-detail/en/texas-instruments/LM324N/296-1391-5-ND/277627)
- 3 CD4052B Multiplexing chips [link](https://www.digikey.com/product-detail/en/texas-instruments/CD4051BE/296-2057-5-ND/67305)
- 18 0.1" double right angle connectors [link](https://www.digikey.com/products/en?mpart=PREC040SFAN-RC&v=35)
- 40 0.1" straight connectors [link](https://www.digikey.com/products/en?mpart=PREC040SFAN-RC&v=35)
- 16 1kΩ resistors
- 6 amplification resistors (optional, see "choosing resistors" section)

### Choosing Resistors

For the 16 1kΩ resistors, you can measure and record their specific resistances after installation for increased accuracy, so there is no special process to choose them.

The Arduino will constantly measure an unamplified temperature, which has a resolution which varies from about 0.1°C near -200°C, to about 1°C near 40°C (this is calculated using 10 bit resolution, an Arduino Due can be configured to use 12 bits instead, giving 100 times finer resolution). 

<p align="center">
  <img src="images/amplification_demo.png" width="700" />
</p>

<p alight="center">Serial monitor output comparing amplified and unamplified measurements. These curves were obtained by pinching a PT1000 with my fingers and then releasing. The overall offset can likely be fixed with calibration. (Image will be updated to a cleaner one soon.)</p>

To improve this resolution, the Arduino will also measure an amplified temperature, which lets you effectively "zoom in" on an specific temperature range (see above comparison). There is a several step process to determine the ideal resistors for a given temperature range of interest.

1. Decide on T<sub>min</sub> and T<sub>max</sub>, the minimum and maximum temperatures you plan to measure with your amplified circuit.
2. Calculate the R<sub>min</sub> and R<sub>max</sub>, the corresponding minimum and maximum PT1000 resistances (use something like [this table](https://web.mst.edu/~cottrell/ME240/Resources/Temperature/RTD%20table.pdf))
3. Calculate V<sub>min</sub> and V<sub>max</sub>, the corresponding voltages read by the Arduino. Use the formula V=R/(R+1000Ω)*(3.3 Volts).
4. We have our unamplified voltage range, and we will use our amplifier to convert this to our desired voltage range of 0.1V-1.7V (the op-amp only gets within 1.5V of the maximum, so the measurable range is 0-1.8V, and we give each side 0.1V of buffer). So, the desired amplification factor is A=(V<sub>max</sub>- V<sub>min</sub>)/(1.6 Volts). We also have an ideal offset voltage V<sub>off</sub>  equal to V<sub>min</sub>.
5. Now that we have ideal A and V<sub>off</sub> values chosen, we must modify them to make their values easy to construct with available resistors. We must choose resistors with resistance values R<sub>1</sub>, R<sub>2</sub>, R<sub>off1</sub>, and R<sub>off2</sub> such that A=R<sub>2</sub>/R<sub>1</sub> and  V<sub>off</sub>=V<sub>min</sub>. It is perfectly fine to decrease A and adjust V<sub>off</sub> in order to accomplish this without having to use nonstandard resistors, it will just slightly change the range of voltages, which is why we gave each side 0.1V of buffer in step 4. 
6. To check that your resistance values will work as expected, recalculate your amplified voltage range. The amplified voltage will be V<sub>amp </sub>= R<sub>2</sub>/R<sub>1</sub> * (V - (3.3 Volts) * R<sub>off2</sub> / (R<sub>off1</sub> + R<sub>off2</sub> )) where V is the unamplified voltage. It's a good idea to use that formula to double check that the amplified voltage will stay in the range 0 to 1.7 Volts for the expected unamplified voltages from step 3.

Now that we have our desired resistor values, R<sub>1</sub>, R<sub>2</sub>, R<sub>off1</sub>, and R<sub>off2</sub>, we need to choose resistors with those resistance values. We need a pair of resistors, R<sub>amp1</sub> and R<sub>amp2</sub>, to have resistances as close as possible to R<sub>1</sub>, and another pair R<sub>amp3</sub> and R<sub>amp4</sub>, to have resistances as close as possible to R<sub>amp1</sub> and R<sub>2</sub>. It is worth taking some time to match the resistances of each pair, as the linearity of the amplifier depends on their values being equal. Once you have also chosen R<sub>off1</sub> and R<sub>off2</sub>, take the time to measure and record the resistances of each resistor, as their precise values are used by the arduino program to reconstruct the temperature from the amplified voltage, and the resistances cannot be simply measured after soldering.

### Assembly

Soldering can be done in any order, but I recommend soldering the pin connectors last as they can give you less room to solder the 16 1kΩ resistors.

### Program Modification

In the Arduino file, you should modify the parameters to correspond to your measured resistances and active channels. You can also switch between LabVIEW output and serial monitor output.

I plan to improve the readability of the arduino code once the LabVIEW integration is finalized.

### Calibration

Will be updated soon. I should be able to use a known temperature to calibrate the temperature measurements, as well as matching the amplified measurement to the unamplified measurement just using the ambient room temperature.

## LabVIEW Integration

Will be updated soon. I plan to support output of temperature as well as input of desired temperature windows.

## Acknowledgments

* Domenico Franco helped me iterate the PCB design.


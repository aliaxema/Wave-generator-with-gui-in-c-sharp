# Wave-generator-with-gui-in-c-sharp
Waveform Generation Approach: To understand circuit operation we need to understand how microcontroller
gives different data to DAC to generate require waveform. Each waveform will be implemented as a separate function, where this function will generate digital values and be interpreted by DAC and generate the required waveform. For simplification
the following table shows each function approach:
# 1 Square Wave: 
To generate Square Wave the microcontroller gives alternatively 00h (low) and FFh (high) outputs as an input to DAC after some delay. The DAC will generate corresponding alternate low and high Analog outputs through Op-Amp circuit
as +12 V and -12 V that will generate Square Wave pattern.
# 2 Staircase Wave:
To generate Staircase Wave the microcontroller first gives 00h (low) output and then after some delay it increases output in steps like 33h, 66h, 99h, CCh and FFh. The DAC will generate Analog output as per these inputs from microcontroller that looks like Staircase Wave.
# 3 Sine Wave:
To generate Sine Wave it is required to make a table of data that contains values calculated using equation (Value = 5 + 5sin(?)) for different angle values like 30o, 60o, 90o,....... of ? Note: the value is Analog output value. The applied digital input must be corresponding to generate this Analog output The values from this table are given to DAC. So DAC will generate corresponding Analog output that generates Sine Wave in output.
# 4 Triangular Wave:
To generate Triangular Wave the microcontroller first gives data from 00h to FFh and then from FFh to 00h. This will generate linearly increasing and decreasing output through
Op-Amp that will generate Triangular Wave.
we will send waveform # followed by amplitude and frequency, therefore the command will be:
@ [0...3] [000...255] [000..255];
# Circuit in proteus:
![image](https://user-images.githubusercontent.com/102418786/224506199-feefeefe-97d4-4752-8ef0-db2785efff5e.png)

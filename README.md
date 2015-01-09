#UBurner#

-----------------------------

A Qt4 based GUI for UBurner(a Universal Programmer). Intended for flexible and customizable
user experience.


#UBurner Library#

-----------------------------

The underlying library used in UBurner. The library can be used separately for development purpose. 
As it is licensed under LGPLv3. Source code is hereby not permitted to use, reuse or distribute without any permission
of the original author.


#History#

------------------------------

I was looking for an open-source universal programmer for microcontrollers and memory chips. But it gave me 
less luck on it than I hoped. Thanks to **Mr. Alberto Maccioni**. His great work - **Open Programmer**, gave me another
shot of hope. But when I came through the GUI, I felt it could be better. It could be made extensible more 
configurable and robust. Say I have an MCU not listed in it's support list. But I find there are other MCUs from 
the same family or series. I wish I could program that too. Or think that I could manage to embed my own 
functionalities inside the GUI or the command-line software. Here comes another shot of hope. :) 

Altogether I found myself lost in the thought of a really OPEN PROGRAMMER. Well not to mention it, but 
it's quite useless unless I change the firmware. Well lets see the other way around, some people with better knowledge 
and better mind, develop the firmware to include more families of MCUs and some other like me, software 
guys, work it out how to make it more comfortable, more manageable. And here it goes ...

Now all it leaves is a good start. I had started to make the GUI once before but soon i found it not the way 
I wanted. So I started all over. Solid new way. A core library that contains all the low level codes like -
interfacing and communicating with the USB burner, a plugin management library, mcu or chips configuration management
library. And all these are designed to be done dynamically.

So say now I have an MCU not listed, but same family available, I just write an xml file with the following 
tags - 

```
<mcu_list>
<mcu>
<id>1edfa2</id>
<name>ATmega8</name>
<family>ATMEGA</family>
<flash_memory_size>8192</flash_memory_size>
<data_memory_size>512</data_memory_size>
<ram_size>1024</ram_size>
<operating_voltage_range>2.8~5.5 V</operating_voltage_range>
<pgm_voltage_range>5 V</pgm_voltage_range>
<pgm_pin_map></pgm_pin_map>
<speed_grade>0-16 MHz</speed_grade>
<package_type>1</package_type>
<pin_count>28</pin_count>
</mcu>
</mcu_list>
```

It's just an example. Wrong info but just example. Now put this xml file in a place where the software will look for 
configurations. It will find out itself.

#Thanks#

-------------------------------------------------

I would like to give thanks to **Alan Ott **- Original Author and Maintainer of the hidapi, **Ramiro Polla **- Author of dlfcn-win32, **Matthieu Labas **- the Author of sxml - A Small XML library in solid C.

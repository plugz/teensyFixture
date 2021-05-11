/*
 ___  _   _  _ __ ___    ___  | |_  ___   _   _ 
/ __|| | | || '_ ` _ \  / _ \ | __|/ _ \ | | | |
\__ \| |_| || | | | | || (_) || |_| (_) || |_| |
|___/ \__,_||_| |_| |_| \___/  \__|\___/  \__, |
                                          |___/ 
										  
	gpio_MCP23S17 - A complete library for Microchip MCP23S17 for many MCU's.
	
model:			company:		pins:		protocol:		Special Features:
---------------------------------------------------------------------------------------------------------------------
mcp23s17		Microchip		 16			SPI					INT/HAEN
---------------------------------------------------------------------------------------------------------------------
Version history:
0.9 Fixed an example, added getInterruptNumber function.
---------------------------------------------------------------------------------------------------------------------
		Copyright (c) 2013-2014, s.u.m.o.t.o.y [sumotoy(at)gmail.com]
---------------------------------------------------------------------------------------------------------------------

    gpio_MCP23S17 Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    gpio_MCP23S17 Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/

/* ------------------------------ MCP23S17 WIRING ------------------------------------
This chip has a very useful feature called HAEN that allow you to share the same CS pin trough
8 different addresses. Of course chip has to be Microchip and should be assigned to different addresses!

Basic Address:  00100 A2 A1 A0 (from 0x20 to 0x27)
A2,A1,A0 tied to ground = 0x20
				__ __
		IOB-0 [|  U  |] IOA-7
		IOB-1 [|     |] IOA-6
		IOB-2 [|     |] IOA-5
		IOB-3 [|     |] IOA-4
		IOB-4 [|     |] IOA-3
		IOB-5 [|     |] IOA-2
		IOB-6 [|     |] IOA-1
		IOB-7 [|     |] IOA-0
		++++  [|     |] INT-A
		GND   [|     |] INT-B
		CS    [|     |] RST (connect to +)
		SCK   [|     |] A2
		MOSI  [|     |] A1
		MISO  [|_____|] A0
*/

#ifndef __MYMCP23S17_HPP__
#define __MYMCP23S17_HPP__

#include <cstdio>
#include <cstdint>

//------------------------- REGISTERS

const static uint8_t			MCP23S17_IODIR = 	0x00;
const static uint8_t			MCP23S17_IPOL = 	0x02;
const static uint8_t			MCP23S17_GPINTEN =  0x04;
const static uint8_t			MCP23S17_DEFVAL = 	0x06;
const static uint8_t			MCP23S17_INTCON = 	0x08;
const static uint8_t			MCP23S17_IOCON = 	0x0A;
const static uint8_t			MCP23S17_GPPU = 	0x0C;
const static uint8_t			MCP23S17_INTF = 	0x0E;
const static uint8_t			MCP23S17_INTCAP = 	0x10;
const static uint8_t			MCP23S17_GPIO = 	0x12;
const static uint8_t			MCP23S17_OLAT = 	0x14;

#include <SPI.h> //this chip needs SPI

const static uint32_t _MCPMaxSpeed = 10000000UL;


/*   The IOCON register!
		 7		6     5	     4     3   2     1    0
IOCON = BANK MIRROR SEQOP DISSLW HAEN ODR INTPOL -NC-
-----------------------------------------------------------------------
0b01101100
BANK: (Controls how the registers are addressed)
1 The registers associated with each port are separated into different banks
0 The registers are in the same bank (addresses are sequential)
MIRROR: (INT Pins Mirror bit)
1 The INT pins are internally connected
0 The INT pins are not connected. INTA is associated with PortA and INTB is associated with PortB
SEQOP: (Sequential Operation mode bit)
1 Sequential operation disabled, address pointer does not increment
0 Sequential operation enabled, address pointer increments.
DISSLW: (Slew Rate control bit for SDA output, only I2C)
HAEN: (Hardware Address Enable bit, SPI only)
1 Enables the MCP23S17 address pins
0 Disables the MCP23S17 address pins
ODR: (This bit configures the INT pin as an open-drain output)
1 Open-drain output (overrides the INTPOL bit).
0 Active driver output (INTPOL bit sets the polarity).
INTPOL: (This bit sets the polarity of the INT output pin)
1 Active high
0 Active low
*/

class MyMCP23S17 {

public:
	
	MyMCP23S17();//For include inside other libraries
    // spiIdx is idx of spi wire
    // csPin is select pin
    // addr is 0-7
	void 			setup(uint8_t csPin, uint8_t haenAdrs);//used with other libraries only

	void 			begin();
    
	void 			gpioPinMode(uint16_t mode);					//OUTPUT=all out,INPUT=all in,0xxxx=you choose
	void 			gpioPinMode(uint8_t pin, bool mode);		//set a unique pin as IN(1) or OUT (0)
	void 			gpioPort(uint16_t value);					//HIGH=all Hi, LOW=all Low,0xxxx=you choose witch low or hi
	void			gpioPort(uint8_t lowByte, uint8_t highByte);		//same as abowe but uses 2 separate uint8_ts
	uint16_t 		readGpioPort();								//read the state of the pins (all)
	uint16_t 		readGpioPortFast();							
	
	void 			gpioDigitalWrite(uint8_t pin, bool value);  //write data to one pin
	void			gpioDigitalWriteFast(uint8_t pin, bool value);
	int 			gpioDigitalRead(uint8_t pin);				//read data from one pin
	uint8_t		 	gpioRegisterReadByte(uint8_t reg);					//read a uint8_t from chip register
	uint16_t		gpioRegisterReadWord(uint8_t reg);					//read a word from chip register
	int 			gpioDigitalReadFast(uint8_t pin);
	void 			gpioRegisterWriteByte(uint8_t reg,uint8_t data,bool both=false);//write a uint8_t in a chip register, optional for both ports
					//if both=true it will write the same register in bank A & B
	void 			gpioRegisterWriteWord(uint8_t reg,word data);		//write a word in a chip register
	void			portPullup(uint16_t data);						// HIGH=all pullup, LOW=all pulldown,0xxxx=you choose witch
	void			gpioPortUpdate();
	// direct access commands
	uint16_t 		gpioReadAddress(uint8_t addr);

	

protected:
	
	
	inline __attribute__((always_inline))
	void _GPIOstartSend(bool mode) {
	SPI.beginTransaction(SPISettings(_MCPMaxSpeed, MSBFIRST, SPI_MODE0));
		digitalWrite(_cs, LOW);
		mode == 1 ? SPI.transfer(_readCmd) : SPI.transfer(_writeCmd);
	}
	
	
	inline __attribute__((always_inline))
	void _GPIOendSend(void){
		digitalWrite(_cs, HIGH);

		SPI.endTransaction();
	}
	
	inline __attribute__((always_inline))
	void _GPIOwriteByte(uint8_t addr, uint8_t data){
		_GPIOstartSend(0);
		SPI.transfer(addr);
		SPI.transfer(data);
		_GPIOendSend();
	}
	
	inline __attribute__((always_inline))
	void _GPIOwriteWord(uint8_t addr, uint16_t data){
		_GPIOstartSend(0);
		SPI.transfer(addr);
		SPI.transfer16(data);
		_GPIOendSend();
	}
	
	
private:
    uint8_t 		_cs;
	uint8_t 		_adrs;

	
	uint8_t 		_readCmd;
	uint8_t 		_writeCmd;
	uint16_t		_gpioDirection;
	uint16_t		_gpioState;
};
#endif

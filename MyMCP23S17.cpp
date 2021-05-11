#include "MyMCP23S17.hpp"
#include "Log.hpp"
#include "Translate.hpp"

#include <Arduino.h>

static uint8_t s_haenEnabledCSPins[SPI_WIRES_COUNT][256] = {
    {0,}
};

MyMCP23S17::MyMCP23S17(){

}


void MyMCP23S17::setup(uint8_t spiIdx, uint8_t csPin, uint8_t haenAdrs){
    _spiIdx = spiIdx;
	_cs = csPin;
	if (haenAdrs > 7)
        haenAdrs = 7;
    _adrs = haenAdrs + 0x20;
	_readCmd =  (_adrs << 1) | 1;
	_writeCmd = _adrs << 1;
}



void MyMCP23S17::begin() {
	pinMode(_cs, OUTPUT);
	digitalWrite(_cs, HIGH);
	delay(100);
	
    if (!s_haenEnabledCSPins[_spiIdx][_cs])
    {
        // See https://www.electro-tech-online.com/threads/how-to-init-multiple-mcp23s17.126668/
        // -> Send message to set IOCON.HAEN (0bxxxxXxxx) to 1
        // send it to address 0 and to address 4
        //
        // With HAEN enabled, it's possible to have,
        // on the same spi wire, 8 MCP23S17 per CS pin.
        for (uint8_t tempAddr : {0, 4})
        {
            LOGLN_DEBUG("set HAEN on wire %u on csPin %u on addr %u",
                    unsigned(_spiIdx), unsigned(_cs), unsigned(tempAddr));
            MyMCP23S17 temp;
            temp.setup(_spiIdx, _cs, tempAddr);
            temp._GPIOwriteByte(MCP23S17_IOCON, 0b00101000);
        }

        s_haenEnabledCSPins[_spiIdx][_cs] = true;
    }
	_gpioDirection = 0xFFFF;//all in
	_gpioState = 0xFFFF;//all low 
}



uint16_t MyMCP23S17::gpioReadAddress(uint8_t addr){
	_GPIOstartSend(1);
	spi_wires[_spiIdx]->transfer(addr);
	uint16_t temp = spi_wires[_spiIdx]->transfer16(0x0);
	_GPIOendSend();
	return temp;
}


void MyMCP23S17::gpioPinMode(uint16_t mode){
	if (mode == INPUT){
		_gpioDirection = 0xFFFF;
	} else if (mode == OUTPUT){	
		_gpioDirection = 0x0000;
		_gpioState = 0x0000;
	} else {
		_gpioDirection = mode;
	}
	_GPIOwriteWord(MCP23S17_IODIR,_gpioDirection);
}

void MyMCP23S17::gpioPinMode(uint8_t pin, bool mode){
	if (pin < 16){//0...15
		mode == INPUT ? _gpioDirection |= (1 << pin) :_gpioDirection &= ~(1 << pin);
		_GPIOwriteWord(MCP23S17_IODIR,_gpioDirection);
	}
}


void MyMCP23S17::gpioPort(uint16_t value){
	if (value == HIGH){
		_gpioState = 0xFFFF;
	} else if (value == LOW){	
		_gpioState = 0x0000;
	} else {
		_gpioState = value;
	}
	_GPIOwriteWord(MCP23S17_GPIO,_gpioState);
}

void MyMCP23S17::gpioPort(uint8_t lowByte, uint8_t highByte){
	_gpioState = highByte | (lowByte << 8);
	_GPIOwriteWord(MCP23S17_GPIO,_gpioState);
}


uint16_t MyMCP23S17::readGpioPort(){
	return gpioReadAddress(MCP23S17_GPIO);
}

uint16_t MyMCP23S17::readGpioPortFast(){
	return _gpioState;
}

void MyMCP23S17::portPullup(uint16_t data) {
	if (data == HIGH){
		_gpioState = 0xFFFF;
	} else if (data == LOW){	
		_gpioState = 0x0000;
	} else {
		_gpioState = data;
	}
	_GPIOwriteWord(MCP23S17_GPPU, _gpioState);
}


void MyMCP23S17::gpioDigitalWrite(uint8_t pin, bool value){
	if (pin < 16){//0...15
		value == HIGH ? _gpioState |= (1 << pin) : _gpioState &= ~(1 << pin);
		_GPIOwriteWord(MCP23S17_GPIO,_gpioState);
	}
}

void MyMCP23S17::gpioDigitalWriteFast(uint8_t pin, bool value){
	if (pin < 16){//0...15
		value == HIGH ? _gpioState |= (1 << pin) : _gpioState &= ~(1 << pin);
	}
}

void MyMCP23S17::gpioPortUpdate(){
	_GPIOwriteWord(MCP23S17_GPIO,_gpioState);
}

int MyMCP23S17::gpioDigitalRead(uint8_t pin){
	if (pin < 16) return (int)(gpioReadAddress(MCP23S17_GPIO) & 1 << pin);
	return 0;
}


int MyMCP23S17::gpioDigitalReadFast(uint8_t pin){
	int temp = 0;
	if (pin < 16) temp = bitRead(_gpioState,pin);
	return temp;
}

uint8_t MyMCP23S17::gpioRegisterReadByte(uint8_t reg){
  uint8_t data = 0;
    _GPIOstartSend(1);
    spi_wires[_spiIdx]->transfer(reg);
    data = spi_wires[_spiIdx]->transfer(0);
    _GPIOendSend();
  return data;
}

uint16_t MyMCP23S17::gpioRegisterReadWord(uint8_t reg){
  uint16_t data = 0;
    _GPIOstartSend(1);
    spi_wires[_spiIdx]->transfer(reg);
    data = spi_wires[_spiIdx]->transfer16(0);
    _GPIOendSend();
  return data;
}

void MyMCP23S17::gpioRegisterWriteByte(uint8_t reg,uint8_t data,bool both){
	if (!both){
		_GPIOwriteByte(reg,(uint8_t)data);
	} else {
		_GPIOstartSend(0);
		spi_wires[_spiIdx]->transfer(reg);
		spi_wires[_spiIdx]->transfer(data);
		spi_wires[_spiIdx]->transfer(data);
		_GPIOendSend();
	}
}



void MyMCP23S17::gpioRegisterWriteWord(uint8_t reg,word data){
	_GPIOwriteWord(reg,(word)data);
}





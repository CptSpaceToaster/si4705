/*
 * si4705.c
 *
 * Created: 10/27/2014 7:01:16 PM
 *  Author: CaptainSpaceToaster
 */ 

#include <stdio.h>
#include <stdarg.h>
#include "si4705.h"
#include "../twi/i2cmaster.h"
#include "util/delay.h"

uint8_t shadow_registers[8]; // for reading from the chip

void si4705_init(void) {
	CLEAR_SI4705_RESET;
	_delay_ms(1);
	SET_SI4705_RESET;
}

void si4705_set_channel(uint16_t channel) {
	si4705_send_command(5, SI4705_SET_CHANNEL, 0x00, (uint8_t)(channel>>8), (uint8_t)(channel & 0xFF), 0x00);
}

uint16_t si4705_get_channel(void) {
	si4705_send_command(2, SI4705_GET_CHANNEL, 0x01);
	si4705_pull();
	return (shadow_registers[2]<<8 | shadow_registers[2]);
}

void si4705_set_volume(uint8_t volume) {
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x40, 0x00, 0x00, volume);
}

uint8_t si4705_get_volume(void) {
	si4705_send_command(4, SI4705_GET_PROPERTY, 0x00, 0x40, 0x00);
	si4705_pull();
	return shadow_registers[3];
}

void si4705_powerOn() {
	si4705_send_command(3, SI4705_POWERUP, 0xD0, 0x05);
	_delay_ms(500); //Wait for oscillator to settle
	
	// Disable "periodic noise" from silicon labs debug setting... I think?
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0xFF, 0x00, 0x00, 0x00);
	
	// Force Stereo
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x18, 0x04, 0x00, 0x00);
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x18, 0x05, 0x00, 0x00);
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x18, 0x08, 0x00, 0x64);
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x18, 0x09, 0x00, 0x64);
	
	//Disable Soft Mute
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x13, 0x02, 0x00, 0x00);
	
	
	//FM DE-Emphasis apparently used in Europe, Japan, and Australia
#ifdef IN_EUROPE
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x11, 0x00, 0x00, 0x00);
#endif
	
	//Using External Headphone Antenna
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x11, 0x07, 0x00, 0x00);
	
}

uint8_t si4705_send_command(uint8_t howmany, ...) {
	uint8_t err;
	va_list ap;
	va_start(ap, howmany);
	
	if (howmany > MAX_SI4705_ARGS) {
		return 1;
	}
	
	err = i2c_start(SI4705_ADDR | I2C_WRITE);
	if (err) {
		printf("device not ack\n");
		return 2;
	}
	
	for(int i = 0; i < howmany; i++) {
		char c = va_arg(ap, int);
		printf("%02x ", c);
		err = i2c_write(c);
		if (err) {
			printf("could not write byte... no ack\n");
			return 1;
		}
	}
	printf("\n");
	i2c_stop();
	va_end(ap);
	_delay_ms(10);
	return 0;
}

//Reads 8 bytes from the device... fills shadow_registers with the data!
uint8_t si4705_pull() {
	 uint8_t err;
	 err = i2c_start(SI4705_ADDR | I2C_READ);
	 if (err) {
		 printf("It's not there\n");
		 return 2;
	 }
	 printf("Fetching Registers... \n");
	 for(int i = 0; i < 7; i++) { 
		shadow_registers[i] = i2c_readAck();
		printf("%02x\n", shadow_registers[i]);
	 }
	 shadow_registers[7] = i2c_readNak();
	 printf("%02x\n", shadow_registers[7]);
	 return 0;
 }
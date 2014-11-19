/*
 * si4705.c
 *
 * Created: 10/27/2014 7:01:16 PM
 * Author: CaptainSpaceToaster
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
/* Takes in an integer frequency from 881 (FM:88.1) to 1089 (FM:108.9) */
uint8_t si4705_set_channel(uint16_t channel) {
	if (channel > SI4705_FM_HIGH || channel < SI4705_FM_LOW) {
		//frequency out of range
		return 1;
	}
	si4705_send_command(5, SI4705_SET_CHANNEL, 0x00, (uint8_t)((channel*10)>>8), (uint8_t)((channel*10) & 0xFF), 0x00);
	_delay_ms(10);
	return 0;
}

/* Takes in an integer 0 = DOWN, or 1 = UP (These are enumerated in the header file for readability) */
uint8_t si4705_seek(uint8_t direction) {
	if (direction > UP) {
		//cannot tune sideways... try that in another dimension
		return 1;
	}
	printf(direction?"UP\n":"DOWN\n");
	si4705_send_command(2, SI4705_SEEK, direction?0x0C:0x04);
	_delay_ms(10);
	return 0;
}

/* Takes in an integer volume from 0 to 63 */
uint8_t si4705_set_volume(uint8_t volume) {
	if (volume > SI4705_VOL_HIGH || volume < SI4705_VOL_LOW) {
		//volume out of range
		return 1;
	}
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x40, 0x00, 0x00, volume);
	return 0;
}

/* Returns an integer frequency from 881 (FM:88.1) to 1089 (FM:108.9) */
uint16_t si4705_get_channel(void) {
	si4705_send_command(2, SI4705_GET_CHANNEL, 0x01);
	si4705_pull();
	return (shadow_registers[2]<<8 | shadow_registers[3])/10;
}

void si4705_get_status(status_t *status) {
	si4705_send_command(2, SI4705_GET_CHANNEL, 0x01);
	si4705_pull();
	status->valid = shadow_registers[1];
	status->tuneFrequency = (shadow_registers[2]<<8 | shadow_registers[3])/10;
	status->rssi = shadow_registers[4];
	status->snr = shadow_registers[5];
	status->multipath = shadow_registers[6];
	status->antenaCap = shadow_registers[7];
}

/* Returns an integer volume from 0 to 63 */
uint8_t si4705_get_volume(void) {
	si4705_send_command(4, SI4705_GET_PROPERTY, 0x00, 0x40, 0x00);
	si4705_pull();
	return shadow_registers[3];
}

void si4705_power_off() {
	si4705_send_command(1, SI4705_POWER_DOWN);
}

void si4705_power_on() {
	// Power on, use an external 32.768 crystal oscillator, enable CTS interrupt, and GPO2 output enable
	si4705_send_command(3, SI4705_POWERUP, 0xD0, 0x05);
	_delay_ms(500); //Wait for oscillator to settle
	
	// Disable "periodic noise" from silicon labs debug setting... I think?
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0xFF, 0x00, 0x00, 0x00);
	
	// Force Stereo
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x18, 0x04, 0x00, 0x00);
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x18, 0x05, 0x00, 0x00);
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x18, 0x08, 0x00, 0x64);
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x18, 0x09, 0x00, 0x64);
	
	// Disable Soft Mute
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x13, 0x02, 0x00, 0x00);
	
	
	// FM DE-Emphasis apparently used in Europe, Japan, and Australia.  Can I get someone over yonder to test this for me?
#ifdef IN_EUROPE
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x11, 0x00, 0x00, 0x00);
#endif
	
	// Using External Headphone Antenna (0x01 = Use TXO/LPI) (0x00 = Use FMI)
	si4705_send_command(6, SI4705_SET_PROPERTY, 0x00, 0x11, 0x07, 0x00, 0x01);
}

/* First argument: Number of bytes being sent,
 * The rest: The bytes being sent out.
 * 
 * exe: The Power-up Command:
 * si4705_send_command(3, 0x01, 0xD0, 0x05); */
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
//		printf("%02x ", c);
		err = i2c_write(c);
		if (err) {
//			printf("could not write byte... no ack\n");
			return 1;
		}
	}
//	printf("\n");
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
//		 printf("It's not there\n");
		 return 2;
	 }
//	 printf("Fetching Registers... \n");
	 for(int i = 0; i < 7; i++) { 
		shadow_registers[i] = i2c_readAck();
//		printf("%02x\n", shadow_registers[i]);
	 }
	 shadow_registers[7] = i2c_readNak();
//	 printf("%02x\n", shadow_registers[7]);
	 return 0;
 }
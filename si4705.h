/*
 * si4705.h
 *
 * Created: 10/27/2014 7:01:16 PM
 * Author: CaptainSpaceToaster
 */ 
#ifndef SI4705_H
#define SI4705_H

//Structures
typedef struct status {
	uint8_t valid;
	uint16_t tuneFrequency;
	uint8_t rssi;
	uint8_t snr;
	uint8_t multipath;
	uint8_t antenaCap;
} status_t;

//Defines
#define SI4705_RST_PIN               4
#define SET_SI4705_RESET             PORTB |= _BV(SI4705_RST_PIN)
#define CLEAR_SI4705_RESET           PORTB &= ~_BV(SI4705_RST_PIN)

#define SI4705_ADDR                  (0x11<<1) //device address shifted over one
//The other address is 0x63

#define SI4705_POWERUP               0x01
#define SI4705_POWER_DOWN            0x11
#define SI4705_SET_PROPERTY          0x12
#define SI4705_GET_PROPERTY          0x13
#define SI4705_SET_CHANNEL           0x20
#define SI4705_SEEK                  0x21
#define SI4705_GET_CHANNEL           0x22

#define MAX_SI4705_ARGS              7
#define SI4705_FM_HIGH               1079
#define SI4705_FM_LOW                881
#define SI4705_VOL_HIGH              63
#define SI4705_VOL_LOW               0

enum SI4705_DIRECTION {
	DOWN,
	UP,
};

uint8_t si4705_set_channel(uint16_t channel);
uint8_t si4705_seek(uint8_t direction);
uint8_t si4705_set_volume(uint8_t volume);
uint16_t si4705_get_channel(void);
void si4705_get_status(status_t *status);

uint8_t si4705_get_volume(void);

void si4705_init(void);
void si4705_power_on(void);
void si4705_power_off(void);
uint8_t si4705_send_command(uint8_t howmany, ...);
uint8_t si4705_pull();


#endif //SI4705_H
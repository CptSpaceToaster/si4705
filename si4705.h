/*
 * si4705.h
 *
 * Created: 10/27/2014 7:01:16 PM
 *  Author: CaptainSpaceToaster
 */ 
#ifndef SI4705_H
#define SI4705_H

//Put defines here
#define SI4705_RST_PIN               4
#define SET_SI4705_RESET             PORTB |= _BV(SI4705_RST_PIN)
#define CLEAR_SI4705_RESET           PORTB &= ~_BV(SI4705_RST_PIN)

#define SI4705_ADDR                  (0x11<<1) //device address shifted over one
//The other address is 0x63

#define SI4705_POWERUP               0x01
#define SI4705_SET_PROPERTY          0x12
#define SI4705_GET_PROPERTY          0x13
#define SI4705_SET_CHANNEL           0x20
#define SI4705_SEEK                  0x21
#define SI4705_GET_CHANNEL           0x22

#define MAX_SI4705_ARGS              7

enum SI4705_DIRECTION {
	DOWN,
	UP,
};

//uint8_t si4705_seek(uint8_t direction);
void si4705_set_channel(uint16_t channel);
uint8_t si4705_seek(uint8_t direction);
uint16_t si4705_get_channel(void);
void si4705_set_volume(uint8_t volume);
uint8_t si4705_get_volume(void);

void si4705_init(void);
void si4705_powerOn(void);
uint8_t si4705_send_command(uint8_t howmany, ...);
uint8_t si4705_pull();


#endif //SI4705_H
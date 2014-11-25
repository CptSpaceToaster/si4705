/*
 * si4705.h
 *
 * Created: 10/27/2014 7:01:16 PM
 * Author: CaptainSpaceToaster
 */ 
#ifndef SI4705_H
#define SI4705_H

#include <stdbool.h>

//Structures
typedef struct si4705_tune_status {
	uint8_t valid;
	uint16_t tuneFrequency;
	uint8_t rssi;
	uint8_t snr;
	uint8_t multipath;
	uint8_t antenaCap;
} si4705_tune_status_t;

typedef struct si4705_rsq_status {
	bool blendint;          // 0 = Blend is within the blend threshold
	bool multhint;          // 0 = Detected multipath has not exceeded the Multipath high threshold
	bool multlint;          // 0 = Detected multipath has not fallen below the Multipath low threshold
	bool snrhint;           // 0 = Received SNR has not exceeded above the SNR high threshold
	bool snrlint;           // 0 = Received SNR has not fallen below above the SNR low threshold 
	bool rssihint;          // 0 = RSSI has not exceeded the RSSI high threshold
	bool rssilint;          // 0 = RSSI has not fallen below the RSSI low threshold
	bool smute;             // 1 = the soft mute is engaged
	bool afcrl;             // 1 = if the AFC rails?
	bool valid;             // 1 = Current channel is valid, and would have been found during a Seek Operation
	bool pilot;             // 1 = stereo pilot presence
	uint8_t stblend;        // is only 7 bits max, indicates the % of stereo blend (127 = full stereo, 0 = full mono)
	uint8_t rssi;           // current receive strength 0-127 db?V
	uint8_t snr;            // signal to noise metric
	uint8_t multipath;      // 0 = no multipath, 100 = full multipath
	int8_t frequency_offset; // signed frequency offset
} si4705_rsq_status_t;

//Defines
#define SI4705_RST_PIN               4
#define SET_SI4705_RESET             PORTB |= _BV(SI4705_RST_PIN)
#define CLEAR_SI4705_RESET           PORTB &= ~_BV(SI4705_RST_PIN)

#define SI4705_ADDR                  (0x11<<1) //device address shifted over one
//The other address is 0x63

#define USING_RDS
#define DISPLAY_OFFSET               12

#define SI4705_POWERUP               0x01
#define SI4705_POWER_DOWN            0x11
#define SI4705_SET_PROPERTY          0x12
#define SI4705_GET_PROPERTY          0x13
#define SI4705_SET_CHANNEL           0x20
#define SI4705_SEEK                  0x21
#define SI4705_GET_TUNE_STATUS       0x22
#define SI4705_GET_RSQ_STATUS        0x23
#define SI4705_GET_RDS_STATUS        0x24

#define SI4705_RDS_PROGRAM_SERVICE   0
#define SI4705_RDS_RADIO_TEXT        2

#define MAX_SI4705_ARGS              7
#define SHADOW_REGISTER_SIZE         13
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
void si4705_get_tune_status(si4705_tune_status_t *status);
void si4705_get_rsq_status(si4705_rsq_status_t *status);
void si4705_get_rdbs(char *program_service, char *radio_text);

uint8_t si4705_get_volume(void);

void si4705_init(void);
void si4705_power_on(void);
void si4705_power_off(void);
#define si4705_pull() si4705_pull_n(8) //Here for backwards compatibility
uint8_t si4705_send_command(uint8_t howmany, ...);
uint8_t si4705_pull_n(uint8_t howmany);


#endif //SI4705_H
#ifndef __CAN_H
#define __CAN_H

#include "stdint.h"

#define CMD_CAN_NODATA			"00000000"


/* CAN Baud Rate Configuration 		*/
#define FCAN  	40000000 
#define BITRATE 250000  
//#define NTQ 	20			// 20 Time Quanta in a Bit Time
//#define BRP_VAL		((FCAN/(2*NTQ*BITRATE))-1)
#define BRP_VAL		3		// this equals a 250kbs baud rate


extern uint32_t PSUADR ;
extern uint32_t DCADR ;


typedef struct  {
 	uint8_t data[8];  
	uint32_t id;
	uint8_t flag;
	uint8_t dlc;
}  CANmsgbuff;


/* Function Prototype 	*/
void CanApiEnqueue(uint32_t id, uint8_t *canbuffer,uint8_t dlc);

void CANInit(void);
void CANStartup(void);
void IccpCanApiInit(void);
void CANDoCAN(void);
void CANLoadSyntax(void);
void CANParseCommand(uint8_t *cmd, uint32_t id,uint8_t dlc);
uint32_t CANSetCanID(uint8_t psuadr, uint8_t dcadr);


void CANWrite(uint8_t psuadr,uint8_t dcadr,uint16_t funcid,uint8_t *data, uint8_t datalength);
void CANWriteRaw(uint16_t scmd,char *data,uint32_t id);
uint16_t ConvertCharToHex(char ch);
uint16_t CANGetCommand(uint32_t id);


#define CMD_CAN_200			0x200
#define CMD_CAN_201  		0x201
#define CMD_CAN_202			0x202
#define CMD_CAN_203			0x203
#define CMD_CAN_204			0x204
#define CMD_CAN_205			0x205
#define CMD_CAN_206			0x206
#define CMD_CAN_20A			0x20A
#define CMD_CAN_20B			0x20B
#define CMD_CAN_20C			0x20C

#define CMD_CAN_20D			0x20D
#define CMD_CAN_20E			0x20E
#define CMD_CAN_20F			0x20F
#define CMD_CAN_210 		0x210
#define CMD_CAN_211			0x211
#define CMD_CAN_212			0x212
#define CMD_CAN_213			0x213
#define CMD_CAN_214			0x214
#define CMD_CAN_215			0x215
#define CMD_CAN_216			0x216

#define CMD_CAN_217			0x217
#define CMD_CAN_218			0x218
#define CMD_CAN_219			0x219
#define CMD_CAN_21A			0x21A
#define CMD_CAN_21B			0x21B
#define CMD_CAN_21C			0x21C
#define CMD_CAN_21D			0x21D
#define CMD_CAN_21E			0x21E
#define CMD_CAN_21F			0x21F
#define CMD_CAN_220			0x220
#define CMD_CAN_221			0x221
#define CMD_CAN_222			0x222
#define CMD_CAN_223			0x223
#define CMD_CAN_224			0x224
#define CMD_CAN_225			0x225
#define CMD_CAN_226			0x226
#define CMD_CAN_2FF			0x2FF


void CANFunction_200(uint8_t *cmd, uint32_t id,uint8_t dlc);
void CANFunction_201(uint8_t *cmd, uint32_t id,uint8_t dlc);
void CANFunction_202(uint8_t *cmd, uint32_t id,uint8_t dlc);
void CANFunction_203(uint8_t *cmd, uint32_t id,uint8_t dlc);
void CANFunction_204(uint8_t *cmd, uint32_t id,uint8_t dlc);

#endif




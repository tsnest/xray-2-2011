/*  SCE CONFIDENTIAL
 *  PlayStation(R)3 Programmer Tool Runtime Library 300.001
 *  Copyright (C) 2008 Sony Computer Entertainment Inc.
 *  All Rights Reserved.
 */

#ifndef __PAD_UTILS_H__
#define __PAD_UTILS_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cell/pad.h>
#include <cell/padfilter.h>

#include <sys/cdefs.h>

CDECL_BEGIN


#define CELL_UTIL_PAD_MAX	7

/* Bit assignment of digital buttons. */
/* "CELL_PAD_BTN_OFFSET_DIGITAL1" assignment */
#define CELL_UTIL_BUTTON_SELECT   ( CELL_PAD_CTRL_SELECT << 0 )
#define CELL_UTIL_BUTTON_L3       ( CELL_PAD_CTRL_L3	 << 0 )
#define CELL_UTIL_BUTTON_R3       ( CELL_PAD_CTRL_R3	 << 0 )
#define CELL_UTIL_BUTTON_START    ( CELL_PAD_CTRL_START	 << 0 )
#define CELL_UTIL_BUTTON_UP       ( CELL_PAD_CTRL_UP	 << 0 )
#define CELL_UTIL_BUTTON_RIGHT    ( CELL_PAD_CTRL_RIGHT  << 0 )
#define CELL_UTIL_BUTTON_DOWN     ( CELL_PAD_CTRL_DOWN	 << 0 )
#define CELL_UTIL_BUTTON_LEFT     ( CELL_PAD_CTRL_LEFT	 << 0 )

/* "CELL_PAD_BTN_OFFSET_DIGITAL2" assignment */
#define CELL_UTIL_BUTTON_L2       ( CELL_PAD_CTRL_L2		<< 8 )
#define CELL_UTIL_BUTTON_R2       ( CELL_PAD_CTRL_R2		<< 8 )
#define CELL_UTIL_BUTTON_L1       ( CELL_PAD_CTRL_L1		<< 8 )
#define CELL_UTIL_BUTTON_R1       ( CELL_PAD_CTRL_R1		<< 8 )
#define CELL_UTIL_BUTTON_TRIANGLE ( CELL_PAD_CTRL_TRIANGLE	<< 8 )
#define CELL_UTIL_BUTTON_CIRCLE   ( CELL_PAD_CTRL_CIRCLE	<< 8 )
#define CELL_UTIL_BUTTON_CROSS    ( CELL_PAD_CTRL_CROSS		<< 8 )
#define CELL_UTIL_BUTTON_SQUARE   ( CELL_PAD_CTRL_SQUARE	<< 8 )

#define CELL_UTIL_ANALOG_RIGHT    ( CELL_PAD_BTN_OFFSET_ANALOG_RIGHT_X	<< 16 )
#define CELL_UTIL_ANALOG_LEFT     ( CELL_PAD_BTN_OFFSET_ANALOG_LEFT_X	<< 16 )

typedef struct {
	uint8_t x;
	uint8_t y;
} CellPadUtilAxis;

typedef struct{
	uint8_t up, right, down, left;
	uint8_t L1, L2, R1, R2;
	uint8_t circle, cross, square, triangle;
} CellPadUtilPress;

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t z;
	uint16_t g;
} CellPadUtilSensor;

extern uint32_t gCellPadUtilDigitalButtonData[CELL_UTIL_PAD_MAX];
extern uint32_t gCellPadUtilDigitalButtonDataOld[CELL_UTIL_PAD_MAX];

extern bool cellPadUtilPadInit(void);
extern void cellPadUtilPadEnd(void);

extern bool cellPadUtilIsConnected(uint8_t pad);

extern bool cellPadUtilUpdate();
extern void cellPadUtilClearBuffer();

/* get first active pad */
/* return -1 : [ERROR] Pad Not Found */
extern int16_t cellPadUtilGetFirstConnectedPad();

/* buttons */
inline bool cellPadUtilButtonPressed(uint8_t pad, uint32_t button){
	return (gCellPadUtilDigitalButtonData[pad] & button) == button;
}
inline bool cellPadUtilButtonReleased(uint8_t pad, uint32_t button){
	return !(cellPadUtilButtonPressed(pad, button));
}
inline bool cellPadUtilButtonPressedOnce(uint8_t pad, uint32_t button){
	return cellPadUtilButtonPressed(pad, button) && ((gCellPadUtilDigitalButtonDataOld[pad] & button) != button);
}
inline bool cellPadUtilButtonReleasedOnce(uint8_t pad, uint32_t button){
	return cellPadUtilButtonReleased(pad, button) && ((gCellPadUtilDigitalButtonDataOld[pad] & button) == button);
}

/* digital */
inline uint32_t cellPadUtilGetDigitalData(uint8_t pad){
	return gCellPadUtilDigitalButtonData[pad];
}
inline uint32_t cellPadUtilConvertToDigitalData(const CellPadData* pData){
	return ((0x00FF & pData->button[CELL_PAD_BTN_OFFSET_DIGITAL2]) << 8) | (0x00FF & pData->button[CELL_PAD_BTN_OFFSET_DIGITAL1]);
}
inline uint32_t cellPadUtilDigitalButtonPressed(uint32_t digital, uint32_t button){
	return (digital & button) == button;
}
inline bool cellPadUtilButtonPressedImidiate(const CellPadData* pData, uint32_t button) 
{
	return cellPadUtilDigitalButtonPressed(cellPadUtilConvertToDigitalData(pData), button);
}

extern uint32_t cellPadUtilGetEnterButton();
extern uint32_t cellPadUtilGetBackButton();

/* axis */
extern CellPadUtilAxis cellPadUtilGetAxisValue(uint8_t pad, uint32_t axis);

/* sensor */
extern void cellPadUtilSetSensorMode(bool bModeOn);
extern bool cellPadUtilGetSensorMode();
extern bool cellPadUtilSetSensorFilter(int32_t filter);
extern CellPadUtilSensor cellPadUtilGetSensorValue(uint8_t pad);
extern CellPadUtilSensor cellPadUtilGetFilterdSensorValue(uint8_t pad);

/* press */
extern void cellPadUtilSetPressMode(bool bModeOn);
extern bool cellPadUtilGetPressMode();
extern uint8_t cellPadUtilGetButtonPressValue(uint8_t pad, uint32_t button);
extern CellPadUtilPress cellPadUtilGetPressValue(uint8_t pad);

/* actuator */
extern void cellPadUtilSetActuatorMode(bool bModeOn);
extern bool cellPadUtilGetActuatorMode();
extern bool cellPadUtilSetActDirect(uint8_t pad, uint8_t motor1, uint8_t motor2);

/* check caps */
extern bool cellPadUtilIsPs3Conformity(uint8_t pad);
extern bool cellPadUtilIsSupportedPressMode(uint8_t pad);
extern bool cellPadUtilIsSupportedSensorMode(uint8_t pad);
extern bool cellPadUtilIsSupportedHpAnalogStick(uint8_t pad);
extern bool cellPadUtilIsSupportedActuator(uint8_t pad);

/* pad data */
extern const CellPadData* cellPadUtilGetPadData(uint8_t pad);
extern const CellPadInfo2* cellPadUtilGetPadInfo2();


CDECL_END

#endif /* __PAD_UTILS_H__ */

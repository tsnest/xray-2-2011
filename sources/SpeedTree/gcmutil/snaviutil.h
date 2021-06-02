/*  SCE CONFIDENTIAL
 *  PlayStation(R)3 Programmer Tool Runtime Library 300.001
 *  Copyright (C) 2007 Sony Computer Entertainment Inc.
 *  All Rights Reserved.
 */
 
#ifndef __CELL_SNAVIUTIL_H__
#define __CELL_SNAVIUTIL_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef CELL_SNAVIUTIL_DISABLE_NAVIGATOR

#define cellSnaviUtilIsExitRequested(x) (false)
#define CELL_SNAVI_ADJUST_PATH(x) (x)
#define cellSnaviUtilAdjustPath(x, y) (x)

#else

#include <stdbool.h>
#include <cell/pad.h>

	bool cellSnaviUtilIsExitRequested(const CellPadData *pPad);
	const char* cellSnaviUtilAdjustPath(const char* app_home_path, char* buffer);
	extern char gCellSnaviUtilTempBufferForAdjstPath[256];
	#define CELL_SNAVI_ADJUST_PATH(x) cellSnaviUtilAdjustPath(x, gCellSnaviUtilTempBufferForAdjstPath)

#endif /* CELL_SNAVIUTIL_DISABLE_NAVIGATOR */

#ifdef __cplusplus
} /* extern C */
#endif


#endif /* __CELL_SNAVIUTIL_H__ */

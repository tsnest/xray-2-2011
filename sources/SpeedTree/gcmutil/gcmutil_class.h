/*  SCE CONFIDENTIAL
 *  PlayStation(R)3 Programmer Tool Runtime Library 300.001
 *  Copyright (C) 2008 Sony Computer Entertainment Inc.
 *  All Rights Reserved.
 */

#ifndef __CELL_GCMUTIL_CLASS_H__
#define __CELL_GCMUTIL_CLASS_H__


#if defined(__cplusplus)
namespace CellGcmUtil{

class CTimer
{
private:
	CTimer(const CTimer&);
	CTimer& operator=(const CTimer&);
	
protected:
	usecond_t m_count;
	uint64_t m_start;
	bool m_is_timebase;
	float m_time;
	uint32_t sample_count;
	uint64_t m_prev;

public:
	CTimer();
	virtual ~CTimer();
	
	virtual void StartCount(void);
	virtual void EndCount(void);
	virtual void UpdateTimeBase(void);
	virtual void UpdateUSec(system_time_t now_time);
	virtual void UpdateCountUSec(uint64_t count);
	
	virtual float GetTime(uint32_t sample);
};

} // namespace CellGcmUtil
#endif


#endif /* __CELL_GCMUTIL_TYPE_H__ */


// ==========================================================================
// Copyright 1995,2006,2008 Autodesk, Inc. All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================

//#ifndef _animFileExport
//#define _animFileExport
//
//class animBase 
//{
//public:
//						animBase			();
//	virtual				~animBase			();
//
//	enum AnimBaseType{kAnimBaseUnitless, kAnimBaseTime, kAnimBaseLinear, kAnimBaseAngular};
//
//	const char *		tangentTypeAsWord	(MFnAnimCurve::TangentType);
//	const char *		infinityTypeAsWord	(MFnAnimCurve::InfinityType);
//	const char *		outputTypeAsWord	(MFnAnimCurve::AnimCurveType);
//
//	const char *		boolInputTypeAsWord	(bool);
//
//protected:
//	void				resetUnits			();
//
//	MTime::Unit			timeUnit;
//	MAngle::Unit		angularUnit;
//	MDistance::Unit		linearUnit;
//};
//
//class animWriter : public animBase 
//{
//public:
//						animWriter			();
//	virtual 			~animWriter			();
//	bool				writeHeader			(FILE*, double startTime, double endTime, double startUnitless, double endUnitless);
//	bool 				writeAnimCurve		(FILE*, const MObject *, bool = false);
//	bool 				writeAnimCurve2		(const MObject*);
//};
//
//class animUnitNames 
//{
//public:
//						animUnitNames		();
//	virtual				~animUnitNames		();
//
//	static void 		setToLongName		(const MAngle::Unit&, MString&);
//	static void 		setToShortName		(const MAngle::Unit&, MString&);
//
//	static void 		setToLongName		(const MDistance::Unit&, MString&);
//	static void 		setToShortName		(const MDistance::Unit&, MString&);
//
//	static void 		setToLongName		(const MTime::Unit&, MString&);
//	static void 		setToShortName		(const MTime::Unit&, MString&);
//};
//
//#endif /* _animFileExport */

//-
// ==========================================================================
// Copyright 1995,2006,2008 Autodesk, Inc. All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================
//+

#include "pch.h"
//#include "anim_text_file_export.h"
//
////	String names for units.
////
//const char *kMmString = 		"mm";
//const char *kCmString =			"cm";
//const char *kMString =			"m";
//const char *kKmString =			"km";
//const char *kInString =			"in";
//const char *kFtString =			"ft";
//const char *kYdString =			"yd";
//const char *kMiString =			"mi";
//
//const char *kMmLString =		"millimeter";
//const char *kCmLString =		"centimeter";
//const char *kMLString =			"meter";
//const char *kKmLString =		"kilometer";
//const char *kInLString =		"inch";
//const char *kFtLString =		"foot";
//const char *kYdLString =		"yard";
//const char *kMiLString =		"mile";
//
//const char *kRadString =		"rad";
//const char *kDegString =		"deg";
//const char *kMinString =		"min";
//const char *kSecString =		"sec";
//
//const char *kRadLString =		"radian";
//const char *kDegLString =		"degree";
//const char *kMinLString =		"minute";
//const char *kSecLString =		"second";
//
//const char *kHourTString =		"hour";
//const char *kMinTString =		"min";
//const char *kSecTString =		"sec";
//const char *kMillisecTString =	"millisec";
//
//const char *kGameTString =		"game";
//const char *kFileTString =		"film";
//const char *kPalTString =		"pal";
//const char *kNtscTString =		"ntsc";
//const char *kShowTString =		"show";
//const char *kPalFTString =		"palf";
//const char *kNtscFTString =		"ntscf";
//
//const char *kUnitlessString = "unitless";
//const char *kUnknownTimeString =	"Unknown Time Unit";
//const char *kUnknownAngularString =	"Unknown Angular Unit";
//const char *kUnknownLinearString = 	"Unknown Linear Unit";
//
//animUnitNames::animUnitNames()
//{}
//
//animUnitNames::~animUnitNames()
//{}
//
///* static */
//void animUnitNames::setToLongName(const MAngle::Unit& unit, MString& name)
////
////	Description:
////		Sets the string with the long text name of the angle unit.
////
//{
//	switch(unit) {
//		case MAngle::kDegrees:
//			name.set(kDegLString);
//			break;
//		case MAngle::kRadians:
//			name.set(kRadLString);
//			break;
//		case MAngle::kAngMinutes:
//			name.set(kMinLString);
//			break;
//		case MAngle::kAngSeconds:
//			name.set(kSecLString);
//			break;
//		default:
//			name.set(kUnknownAngularString);
//			break;
//	}
//}
//
///* static */
//void animUnitNames::setToShortName(const MAngle::Unit& unit, MString& name)
////
////	Description:
////		Sets the string with the short text name of the angle unit.
////
//{
//	switch(unit) {
//		case MAngle::kDegrees:
//			name.set(kDegString);
//			break;
//		case MAngle::kRadians:
//			name.set(kRadString);
//			break;
//		case MAngle::kAngMinutes:
//			name.set(kMinString);
//			break;
//		case MAngle::kAngSeconds:
//			name.set(kSecString);
//			break;
//		default:
//			name.set(kUnknownAngularString);
//			break;
//	}
//}
//
///* static */
//void animUnitNames::setToLongName(const MDistance::Unit& unit, MString& name)
////
////	Description:
////		Sets the string with the long text name of the distance unit.
////
//{
//	switch(unit) {
//		case MDistance::kInches:
//			name.set(kInLString);
//			break;
//		case MDistance::kFeet:
//			name.set(kFtLString);
//			break;
//		case MDistance::kYards:
//			name.set(kYdLString);
//			break;
//		case MDistance::kMiles:
//			name.set(kMiLString);
//			break;
//		case MDistance::kMillimeters:
//			name.set(kMmLString);
//			break;
//		case MDistance::kCentimeters:
//			name.set(kCmLString);
//			break;
//		case MDistance::kKilometers:
//			name.set(kKmLString);
//			break;
//		case MDistance::kMeters:
//			name.set(kMLString);
//			break;
//		default:
//			name.set(kUnknownLinearString);
//			break;
//	}
//}
//
///* static */
//void animUnitNames::setToShortName(const MDistance::Unit& unit, MString& name)
////
////	Description:
////		Sets the string with the short text name of the distance unit.
////
//{
//	switch(unit) {
//		case MDistance::kInches:
//			name.set(kInString);
//			break;
//		case MDistance::kFeet:
//			name.set(kFtString);
//			break;
//		case MDistance::kYards:
//			name.set(kYdString);
//			break;
//		case MDistance::kMiles:
//			name.set(kMiString);
//			break;
//		case MDistance::kMillimeters:
//			name.set(kMmString);
//			break;
//		case MDistance::kCentimeters:
//			name.set(kCmString);
//			break;
//		case MDistance::kKilometers:
//			name.set(kKmString);
//			break;
//		case MDistance::kMeters:
//			name.set(kMString);
//			break;
//		default:
//			name.set(kUnknownLinearString);
//			break;
//	}
//}
//
///* static */
//void animUnitNames::setToLongName(const MTime::Unit &unit, MString &name)
////
////	Description:
////		Sets the string with the long text name of the time unit.
////
//{
//	switch(unit) {
//		case MTime::kHours:
//			name.set(kHourTString);
//			break;
//		case MTime::kMinutes:
//			name.set(kMinTString);
//			break;
//		case MTime::kSeconds:
//			name.set(kSecTString);
//			break;
//		case MTime::kMilliseconds:
//			name.set(kMillisecTString);
//			break;
//		case MTime::kGames:
//			name.set(kGameTString);
//			break;
//		case MTime::kFilm:
//			name.set(kFileTString);
//			break;
//		case MTime::kPALFrame:
//			name.set(kPalTString);
//			break;
//		case MTime::kNTSCFrame:
//			name.set(kNtscTString);
//			break;
//		case MTime::kShowScan:
//			name.set(kShowTString);
//			break;
//		case MTime::kPALField:
//			name.set(kPalFTString);
//			break;
//		case MTime::kNTSCField:
//			name.set(kNtscFTString);
//			break;
//		default:
//			name.set(kUnknownTimeString);
//			break;
//	}
//}
//
///* static */
//void animUnitNames::setToShortName(const MTime::Unit &unit, MString &name)
////
////	Description:
////		Sets the string with the short text name of the time unit.
////
//{
//	setToLongName(unit, name);
//}
//
////-------------------------------------------------------------------------
////	Class animBase
////-------------------------------------------------------------------------
//
//// Tangent type words
////
//const char *kWordTangentGlobal = "global";
//const char *kWordTangentFixed = "fixed";
//const char *kWordTangentLinear = "linear";
//const char *kWordTangentFlat = "flat";
//const char *kWordTangentSmooth = "spline";
//const char *kWordTangentStep = "step";	// Maintains current value until next key
//const char *kWordTangentStepNext = "stepnext";	// Immediately steps to next key's value
//const char *kWordTangentSlow = "slow";
//const char *kWordTangentFast = "fast";
//const char *kWordTangentClamped = "clamped";
//const char *kWordTangentPlateau = "plateau";
//
//// Infinity type words
////
//const char *kWordConstant = "constant";
//const char *kWordLinear = "linear";
//const char *kWordCycle = "cycle";
//const char *kWordCycleRelative = "cycleRelative";
//const char *kWordOscillate = "oscillate";
//
////	Param Curve types
////
//const char *kWordTypeUnknown = "unknown";
//const char *kWordTypeLinear = "linear";
//const char *kWordTypeAngular = "angular";
//const char *kWordTypeTime = "time";
//const char *kWordTypeUnitless = "unitless";
//
////	Keywords
////
//const char *kAnim = "anim";
//const char *kAnimData = "animData";
//const char *kMovData = "movData";
//const char *kMayaVersion = "mayaVersion";
//const char *kAnimVersion = "animVersion";
//
//const char *kTimeUnit = "timeUnit";
//const char *kLinearUnit = "linearUnit";
//const char *kAngularUnit = "angularUnit";
//const char *kStartTime = "startTime";
//const char *kEndTime = "endTime";
//const char *kStartUnitless = "startUnitless";
//const char *kEndUnitless = "endUnitless";
//
//// animVersions:
////
//const char *kAnimVersionString = "1.1";
//
//const char *kTwoSpace = "  ";
//
////	animData keywords
////
//const char *kInputString = "input";
//const char *kOutputString = "output";
//const char *kWeightedString = "weighted";
//const char *kPreInfinityString = "preInfinity";
//const char *kPostInfinityString = "postInfinity";
//const char *kInputUnitString = "inputUnit";
//const char *kOutputUnitString = "outputUnit";
//const char *kTanAngleUnitString = "tangentAngleUnit";
//const char *kKeysString = "keys";
//
////	special characters
////
//const char kSemiColonChar	= ';';
//const char kSpaceChar		= ' ';
//const char kBraceLeftChar	= '{';
//const char kBraceRightChar	= '}';
//
//animBase::animBase ()
//{
//	resetUnits();
//}
//	
//animBase::~animBase()
//{
//}
//
//void animBase::resetUnits()
//{
//	timeUnit	= MTime::uiUnit();
//	linearUnit	= MDistance::uiUnit();
//	angularUnit = MAngle::uiUnit();
//}
//
//const char* animBase::tangentTypeAsWord(MFnAnimCurve::TangentType type)
////
////	Description:
////		Returns a string with a test based desription of the passed
////		MFnAnimCurve::TangentType. 
////
//{
//	switch (type) {
//		case MFnAnimCurve::kTangentGlobal:
//			return (kWordTangentGlobal);
//		case MFnAnimCurve::kTangentFixed:
//			return (kWordTangentFixed);
//		case MFnAnimCurve::kTangentLinear:
//			return (kWordTangentLinear);
//		case MFnAnimCurve::kTangentFlat:
//			return (kWordTangentFlat);
//		case MFnAnimCurve::kTangentSmooth:
//			return (kWordTangentSmooth);
//		case MFnAnimCurve::kTangentStep:
//			return (kWordTangentStep);
//		case MFnAnimCurve::kTangentSlow:
//			return (kWordTangentSlow);
//		case MFnAnimCurve::kTangentFast:
//			return (kWordTangentFast);
//		case MFnAnimCurve::kTangentClamped:
//			return (kWordTangentClamped);
//		case MFnAnimCurve::kTangentPlateau:
//			return (kWordTangentPlateau);
//		case MFnAnimCurve::kTangentStepNext:
//			return (kWordTangentStepNext);
//		default:
//			break;
//	}
//	return (kWordTangentGlobal);
//}
//
//const char *
//animBase::infinityTypeAsWord(MFnAnimCurve::InfinityType type)
////	
////	Description:
////		Returns a string containing the name of the passed 
////		MFnAnimCurve::InfinityType type.
////
//{
//	switch (type) {
//		case MFnAnimCurve::kConstant:
//			return (kWordConstant);
//		case MFnAnimCurve::kLinear:
//			return (kWordLinear);
//		case MFnAnimCurve::kCycle:
//			return (kWordCycle);
//		case MFnAnimCurve::kCycleRelative:
//			return (kWordCycleRelative);
//		case MFnAnimCurve::kOscillate:
//			return (kWordOscillate);
//		default:
//			break;
//	}
//	return (kWordConstant);
//}
//
//const char *
//animBase::outputTypeAsWord (MFnAnimCurve::AnimCurveType type)
////
////	Description:
////		Returns a string identifying the output type of the
////		passed MFnAnimCurve::AnimCurveType.
////
//{
//	switch (type) {
//		case MFnAnimCurve::kAnimCurveTL:
//		case MFnAnimCurve::kAnimCurveUL:
//			return (kWordTypeLinear);
//		case MFnAnimCurve::kAnimCurveTA:
//		case MFnAnimCurve::kAnimCurveUA:
//			return (kWordTypeAngular);
//		case MFnAnimCurve::kAnimCurveTT:
//		case MFnAnimCurve::kAnimCurveUT:
//			return (kWordTypeTime);
//		case MFnAnimCurve::kAnimCurveTU:
//		case MFnAnimCurve::kAnimCurveUU:
//			return (kWordTypeUnitless);
//		case MFnAnimCurve::kAnimCurveUnknown:
//			return (kWordTypeUnitless);
//	}
//	return (kWordTypeUnknown);
//}
//
//const char *
//animBase::boolInputTypeAsWord(bool isUnitless) 
////
////	Description:
////		Returns a string based on a bool. 
////
//{
//	if (isUnitless) {
//		return (kWordTypeUnitless);
//	} else {
//		return (kWordTypeTime);
//	}
//}
//
////-------------------------------------------------------------------------
////	Class animWriter
////-------------------------------------------------------------------------
//
//animWriter::animWriter()
//{}
//
//animWriter::~animWriter()
//{}
//
//bool animWriter::writeHeader(FILE* const clip, double startTime, double endTime, double startUnitless, double endUnitless)
////
////	Description:
////		Writes the header for the file. The header contains the clipboard
////		specific data. 
////
//{
//	if (!clip)
//		return false;
//
//	fprintf( clip, "%s%c%s%c\r\n", kAnimVersion, kSpaceChar, kAnimVersionString, kSemiColonChar );
//	fprintf( clip, "%s%c%s%c\r\n", kMayaVersion, kSpaceChar, MGlobal::mayaVersion(), kSemiColonChar );
//
//	if (startTime != endTime) 
//	{
//		MString unit;
//		animUnitNames::setToShortName(timeUnit, unit);
//		fprintf( clip, "%s%c%s%c\r\n", kTimeUnit, kSpaceChar, unit, kSemiColonChar );
//
//		animUnitNames::setToShortName(linearUnit, unit);
//		fprintf( clip, "%s%c%s%c\r\n", kLinearUnit, kSpaceChar, unit, kSemiColonChar );
//
//		animUnitNames::setToShortName(angularUnit, unit);
//		fprintf( clip, "%s%c%s%c\r\n", kAngularUnit, kSpaceChar, unit, kSemiColonChar );
//
//		fprintf( clip, "%s%c%s%c\r\n", kStartTime, kSpaceChar, startTime, kSemiColonChar );
//		fprintf( clip, "%s%c%s%c\r\n", kEndTime, kSpaceChar, endTime, kSemiColonChar );
//	}
//
//	if (startUnitless != endUnitless) 
//	{
//		fprintf( clip, "%s%c%s%c\r\n", kStartUnitless, kSpaceChar, startUnitless, kSemiColonChar );
//		fprintf( clip, "%s%c%s%c\r\n", kEndUnitless, kSpaceChar, endUnitless, kSemiColonChar );
//	}
//
//	return true;
//}
//
//bool animWriter::writeAnimCurve(FILE* const clip, const MObject *animCurveObj, bool verboseUnits)
////
////	Description:
////		Write out the anim curve from the clipboard item into the
////		ofstream. The actual anim curve data is written out.
////
////		This method returns true if the write was successful.
////
//{
//	if (NULL == animCurveObj || animCurveObj->isNull() || !clip)
//		return true;
//
//	MStatus status			= MS::kSuccess;
//	MFnAnimCurve animCurve	(*animCurveObj, &status);
//
//	if (MS::kSuccess != status) 
//	{
//		display_warning("Error: Could not read the anim curve for export.");
//		return false;
//	}
//
//	fprintf( clip, "%s%c%c\r\n", kAnimData, kSpaceChar, kBraceLeftChar );
//
//	fprintf( clip, "%s%s%c%s%c\r\n", kTwoSpace, kInputString, kSpaceChar, boolInputTypeAsWord(animCurve.isUnitlessInput()), kSemiColonChar );
//
//	fprintf( clip, "%s%s%c%s%c\r\n", kTwoSpace, kOutputString, kSpaceChar, outputTypeAsWord(animCurve.animCurveType()), kSemiColonChar );
//
//	fprintf( clip, "%s%s%c%d%c\r\n", kTwoSpace, kWeightedString, kSpaceChar, (animCurve.isWeighted() ? 1 : 0), kSemiColonChar );
//
//	//	These units default to the units in the header of the file.
//	//	
//	if (verboseUnits) 
//	{
//		fprintf( clip, "%s%s%c", kTwoSpace, kInputUnitString, kSpaceChar );
//		if (animCurve.isTimeInput()) 
//		{
//			MString unitName;
//			animUnitNames::setToShortName(timeUnit, unitName);
//			fprintf( clip, "%s", unitName );
//		}else 
//		{
//			//	The anim curve has unitless input.
//			//
//			fprintf( clip, "%s", kUnitlessString );
//		}
//		fprintf( clip, "%c\r\n", kSemiColonChar );
//
//		fprintf( clip, "%s%s%c", kTwoSpace, kOutputUnitString, kSpaceChar );
//	}
//
//	double conversion = 1.0;
//	MString unitName;
//	switch (animCurve.animCurveType()) 
//	{
//		case MFnAnimCurve::kAnimCurveTA:
//		case MFnAnimCurve::kAnimCurveUA:
//
//			animUnitNames::setToShortName(angularUnit, unitName);
//			if (verboseUnits) 
//				fprintf( clip, "%s", unitName );
//			{
//				MAngle angle(1.0);
//				conversion = angle.as(angularUnit);
//			}
//			break;
//
//		case MFnAnimCurve::kAnimCurveTL:
//		case MFnAnimCurve::kAnimCurveUL:
//
//			animUnitNames::setToShortName(linearUnit, unitName);
//			if (verboseUnits) 
//				fprintf( clip, "%s", unitName );
//			{
//				MDistance distance(1.0);
//				conversion = distance.as(linearUnit);
//			}
//			break;
//
//		case MFnAnimCurve::kAnimCurveTT:
//		case MFnAnimCurve::kAnimCurveUT:
//
//			animUnitNames::setToShortName(timeUnit, unitName);
//			if (verboseUnits) 
//				fprintf( clip, "%s", unitName );
//			break;
//		default:
//			if (verboseUnits)
//				fprintf( clip, "%s", kUnitlessString );
//			break;
//	}
//	if (verboseUnits) 
//		fprintf( clip, "%ñ\r\n", kSemiColonChar );
//
//	if (verboseUnits) 
//	{
//		MString angleUnitName;
//		animUnitNames::setToShortName(angularUnit, angleUnitName);
//		fprintf( clip, "%s%s%c%s%c\r\n", kTwoSpace, kTanAngleUnitString, kSpaceChar, angleUnitName, kSemiColonChar );
//	}
//
//	fprintf( clip, "%s%s%c%s%c\r\n", kTwoSpace, kPreInfinityString, kSpaceChar, infinityTypeAsWord(animCurve.preInfinityType()), kSemiColonChar );
//
//	fprintf( clip, "%s%s%c%s%c\r\n", kTwoSpace, kPostInfinityString, kSpaceChar, infinityTypeAsWord(animCurve.postInfinityType()), kSemiColonChar );
//
//	fprintf( clip, "%s%s%c%c\r\n", kTwoSpace, kKeysString, kSpaceChar, kBraceLeftChar );
//
//	// And then write out each keyframe
//	//
//	unsigned numKeys = animCurve.numKeyframes();
//
//	for (unsigned i=0; i<numKeys; i++) 
//	{
//		fprintf( clip, "%s%s", kTwoSpace, kTwoSpace );
//		if (animCurve.isUnitlessInput()) 
//			fprintf( clip, "%g", animCurve.unitlessInput(i) );
//		else
//			fprintf( clip, "%g", animCurve.time(i).value() );
//
//		fprintf( clip, "%ñ%g", kSpaceChar, (conversion*animCurve.value(i)) );
//
//		fprintf( clip, "%ñ%s", kSpaceChar, tangentTypeAsWord(animCurve.inTangentType(i)) );
//		fprintf( clip, "%ñ%s", kSpaceChar, tangentTypeAsWord(animCurve.outTangentType(i)) );
//
//		fprintf( clip, "%ñ%d", kSpaceChar, (animCurve.tangentsLocked(i) ? 1 : 0) );
//		fprintf( clip, "%ñ%d", kSpaceChar, (animCurve.weightsLocked(i) ? 1 : 0) );
//		fprintf( clip, "%ñ%d", kSpaceChar, (animCurve.isBreakdown(i) ? 1 : 0) );
//
//		if (animCurve.inTangentType(i) == MFnAnimCurve::kTangentFixed) 
//		{
//			MAngle angle;
//			double weight;
//			animCurve.getTangent(i, angle, weight, true);
//
//			fprintf( clip, "%ñ%g", kSpaceChar, angle.as(angularUnit) );
//			fprintf( clip, "%ñ%g", kSpaceChar, weight );
//		}
//		if (animCurve.outTangentType(i) == MFnAnimCurve::kTangentFixed) 
//		{
//			MAngle angle;
//			double weight;
//			animCurve.getTangent(i, angle, weight, false);
//
//			fprintf( clip, "%ñ%g", kSpaceChar, angle.as(angularUnit) );
//			fprintf( clip, "%ñ%g", kSpaceChar, weight );
//		}
//
//		fprintf( clip, "%ñ\r\n", kSemiColonChar );
//	}
//
//	fprintf( clip, "%s%c\r\n", kTwoSpace, kBraceRightChar );
//	fprintf( clip, "%c\r\n", kBraceRightChar );
//
//	return true;
//}

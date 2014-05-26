// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- ADSKMyEntity.cpp : Implementation of ADSKMyEntity
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKMyEntity.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 ADSKMyEntity::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	ADSKMyEntity, AcDbPolyline,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, ADSKMYENTITY,
ADSKADSKENTITYAPP
|Product Desc:     A description for your object
|Company:          Your company name
|WEB Address:      Your company WEB site address
)

//-----------------------------------------------------------------------------
ADSKMyEntity::ADSKMyEntity () : AcDbPolyline () {
}

ADSKMyEntity::~ADSKMyEntity () {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus ADSKMyEntity::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbPolyline::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (ADSKMyEntity::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	//.....

	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus ADSKMyEntity::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbPolyline::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > ADSKMyEntity::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < ADSKMyEntity::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....

	return (pFiler->filerStatus ()) ;
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean ADSKMyEntity::subWorldDraw (AcGiWorldDraw *mode)
{
assertReadEnabled () ;
//return (AcDbPolyline::subWorldDraw (mode)) ;
return myEdge(mode);
}


Adesk::UInt32 ADSKMyEntity::subSetAttributes (AcGiDrawableTraits *traits) {
	assertReadEnabled () ;
	return (AcDbPolyline::subSetAttributes (traits)) ;
}

Adesk::Boolean ADSKMyEntity::myEdge(AcGiWorldDraw* pWd)
{
	/*Purpose of this method is to diplay the hatch using new API AcGiGeometry::edge
	on the my entity[ADSKMyEntity] deriving from polyline */
	Adesk::Boolean result = Adesk::kFalse; 
	AcArray<AcGeCurve2d*> geCurves;
	double hatchDev = pWd->deviation(kAcGiMaxDevForCurve, AcGePoint3d::kOrigin);
	AcGiFill acgiSolidFill;
	acgiSolidFill.setDeviation(hatchDev);
	int nSegs = -1;
	AcGeLineSeg2d line;
	AcGeLineSeg2d *pLine;
	nSegs = this->numVerts() ;
	for(int i = 0; i < nSegs; i++)
	{
		if(this->segType(i) == AcDbPolyline::kLine)
		{
			this->getLineSegAt(i, line);
			pLine = new AcGeLineSeg2d(line);
			geCurves.append(pLine);

		}
	}
		
	pWd->subEntityTraits().setFill(&acgiSolidFill);
	/* geCurves represents boundary of hatch*/
	result = pWd->geometry().edge(geCurves);
	return result;
}

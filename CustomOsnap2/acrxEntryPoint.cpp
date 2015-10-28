// (C) Copyright 2002-2012 by Autodesk, Inc. 
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
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "acedCmdNF.h"
#include "dbosnap2.h"
#include <AcColorThemeEnum.h>
#include <aduiImage.h>

#define ASDK_GLYPH_TEXT _T("1/3")
extern bool bSnapToSegments;

//-----------------------------------------------------------------------------
#define szRDS _RXST("ADSK")
class AsdkThirdGlyph : public AcGiGlyph
{
public:
	virtual Acad::ErrorStatus setLocation(const AcGePoint3d& dcsPoint)
	{
		m_center = dcsPoint;
		return Acad::eOk;
	}
	virtual void subViewportDraw(AcGiViewportDraw* vportDrawContext)
	{
		// Calculate the size of the glyph in WCS (use for text height factor)
		int glyphPixels = acdbCustomOsnapManager()->osnapGlyphSize();
		AcGePoint2d glyphSize;
		vportDrawContext->viewport().getNumPixelsInUnitSquare( m_center, glyphSize );
		double glyphHeight = glyphPixels / glyphSize[ Y ];

		// Get the extents of the glyph text, so we can centre it
		AcGiTextStyle style;
		AcGePoint2d ptExt = style.extents( ASDK_GLYPH_TEXT, Adesk::kFalse, -1, Adesk::kFalse );

		struct resbuf rbFrom, rbTo;
		rbFrom.restype = RTSHORT;
		rbFrom.resval.rint = 2; // From DCS
		rbTo.restype = RTSHORT;
		rbTo.resval.rint = 0; // To WCS

		// Translate the X-axis of the DCS to WCS co-ordinates (as a displacement vector)
		AcGeVector3d ptDir;
		acedTrans( asDblArray( AcGeVector3d::kXAxis ),
			&rbFrom, 
			&rbTo, 
			1, 
			asDblArray( ptDir ));

		// Translate the centre of the glyph from DCS to WCS co-ordinates
		AcGePoint3d ptPos, ptCen;
		AcGeVector3d vecExt( ptExt[ X ] / 2, ptExt[ Y ] / 2, 0 );
		ptPos = m_center - vecExt / 2; 
		if ( RTNORM != acedTrans( asDblArray( ptPos ),
			&rbFrom,
			&rbTo,
			0,
			asDblArray( ptCen )))
			ptCen = m_center;

		// Draw the centred text representing the glyph
		vportDrawContext->geometry().text( ptCen,
			vportDrawContext->viewport().viewDir(),
			ptDir, 
			glyphHeight, 
			1.0,
			0.0,
			ASDK_GLYPH_TEXT );

	}

private:
	AcGePoint3d m_center;
};
class AsdkThirdOsnapInfo : public AcDbCustomOsnapInfo
{
public:
	ACRX_DECLARE_MEMBERS(AsdkThirdOsnapInfo);

	virtual Acad::ErrorStatus   getOsnapInfo(
		AcDbEntity*			pickedObject,
		Adesk::GsMarker		gsSelectionMark,
		const AcGePoint3d&		pickPoint,
		const AcGePoint3d&		lastPoint,
		const AcGeMatrix3d&	viewXform,
		AcArray<AcGePoint3d>&	snapPoints,
		AcArray<int>&			geomIdsForPts,
		AcArray<AcGeCurve3d*>& snapCurves,
		AcArray<int>&			geomIdsForLines) = 0;
};
ACRX_NO_CONS_DEFINE_MEMBERS( AsdkThirdOsnapInfo, AcDbCustomOsnapInfo );
// AcDbEntity level protocol extension


class AsdkThirdOsnapEntityInfo : public AsdkThirdOsnapInfo 
{ 
public:
	virtual Acad::ErrorStatus   getOsnapInfo(
		AcDbEntity*			pickedObject,
		Adesk::GsMarker		gsSelectionMark,
		const AcGePoint3d&		pickPoint,
		const AcGePoint3d&		lastPoint,
		const AcGeMatrix3d&	viewXform,
		AcArray<AcGePoint3d>&	snapPoints,
		AcArray<int>&			geomIdsForPts,
		AcArray<AcGeCurve3d*>& snapCurves,
		AcArray<int>&			geomIdsForLines)
	{
		// Base definition with no functionality
		return Acad::eOk;
	}
};

// AcDbCurve level protocol extension

class AsdkThirdOsnapCurveInfo : public AsdkThirdOsnapInfo 
{ 
public:
	virtual Acad::ErrorStatus   getOsnapInfo(
		AcDbEntity*			pickedObject,
		Adesk::GsMarker		gsSelectionMark,
		const AcGePoint3d&		pickPoint,
		const AcGePoint3d&		lastPoint,
		const AcGeMatrix3d&	viewXform,
		AcArray<AcGePoint3d>&	snapPoints,
		AcArray<int>&			geomIdsForPts,
		AcArray<AcGeCurve3d*>& snapCurves,
		AcArray<int>&			geomIdsForLines)
	{
		// Generic curve function for all AcDbCurves (except AcDbPolylines)

		// Protocol Extension insures that the following assertion is always
		// true, but check in non-prod versions just to be safe.
		assert( pickedObject->isKindOf( AcDbCurve::desc() ));

		// but in production, a hard cast is fastest
		AcDbCurve *pCurve = (AcDbCurve*)pickedObject;

		double startParam, endParam;
		AcGePoint3d pt;

		Acad::ErrorStatus es;
		es=pCurve->getStartParam( startParam);
		es=pCurve->getEndParam( endParam );
		es=pCurve->getPointAtParam( startParam + ((endParam - startParam) / 3), pt );
		assert( Acad::eOk == es);
		snapPoints.append( pt );
		es=pCurve->getPointAtParam( startParam + ((endParam - startParam) * 2 / 3), pt );
		assert(Acad::eOk==es);
		snapPoints.append( pt );
		if ( pCurve->isClosed() )
		{
			es=pCurve->getStartPoint( pt );
			assert(Acad::eOk==es);
			snapPoints.append( pt );
		}
		return Acad::eOk;
	}
};

// AcDbPolyline level protocol extension

class AsdkThirdOsnapPolylineInfo : public AsdkThirdOsnapInfo
{
public:
	virtual Acad::ErrorStatus   getOsnapInfo(
		AcDbEntity*			pickedObject,
		Adesk::GsMarker		gsSelectionMark,
		const AcGePoint3d&		pickPoint,
		const AcGePoint3d&		lastPoint,
		const AcGeMatrix3d&	viewXform,
		AcArray<AcGePoint3d>&	snapPoints,
		AcArray<int>&			geomIdsForPts,
		AcArray<AcGeCurve3d*>& snapCurves,
		AcArray<int>&			geomIdsForLines)
	{
		// Specialised implementation for AcDbPolylines:
		//  Parametrisation of AcDbPolylines is different: each whole numbered paramater appears
		//  at a vertex, so we cannot simply divide by three to get the correct parameter.

		// Protocol Extension insures that the following assertion is always
		// true, but check in non-prod versions just to be safe.
		assert( pickedObject->isKindOf( AcDbPolyline::desc() ));

		// but in production, a hard cast is fastest
		AcDbPolyline *pPline = (AcDbPolyline*)pickedObject;

		Acad::ErrorStatus es;

		if ( bSnapToSegments )
		{
			// Snap to a third of each of the segments
			unsigned int numSegs = pPline->numVerts() - 1;
			AcGeLineSeg3d segLn;
			AcGeCircArc3d segArc;
			double startParam, endParam, newParam, dist;
			AcGePoint3d pt;

			for ( unsigned int idx = 0; idx < numSegs; idx++ )
			{
				switch( pPline->segType( idx ))
				{
				case AcDbPolyline::kLine:
					es=pPline->getLineSegAt( idx, segLn );
					startParam = segLn.paramOf( segLn.startPoint() );
					endParam = segLn.paramOf( segLn.endPoint() );
					snapPoints.append(segLn.evalPoint( startParam + ((endParam - startParam) / 3 )));
					snapPoints.append(segLn.evalPoint( startParam + ((endParam - startParam) * 2 / 3 )));
					break;
				case AcDbPolyline::kArc:
					es=pPline->getArcSegAt( idx, segArc );
					startParam = segArc.paramOf( segArc.startPoint() );
					endParam = segArc.paramOf( segArc.endPoint() );
					dist = segArc.length( startParam, endParam );
					newParam = segArc.paramAtLength( startParam, dist / 3 );
					snapPoints.append( segArc.evalPoint( newParam ));
					newParam = segArc.paramAtLength( startParam, dist * 2 / 3 );
					snapPoints.append( segArc.evalPoint( newParam ));
					break;
				default:
					break;
				}
			}
		}
		else {
			double endParam;
			AcGePoint3d pt;
			double dist;

			es=pPline->getEndParam( endParam );
			es=pPline->getDistAtParam( endParam, dist );
			es=pPline->getPointAtDist( dist / 3, pt );
			assert(Acad::eOk==es);
			snapPoints.append( pt );
			es=pPline->getPointAtDist( dist * 2 / 3, pt );
			assert(Acad::eOk==es);
			snapPoints.append( pt );
			if ( pPline->isClosed() )
			{
				es=pPline->getStartPoint( pt );
				snapPoints.append( pt );
			}
		}
		return Acad::eOk;
	}
};

class AsdkThirdOsnapMode : public AcDbCustomOsnapMode2
{	
public:
	AsdkThirdOsnapMode()
	{
		m_pGlyph = new AsdkThirdGlyph;
		m_lightIcon = NULL;
		m_darkIcon = NULL;
	}
	virtual ~AsdkThirdOsnapMode()
	{
		delete m_pGlyph;
		m_pGlyph = NULL;

		DestroyIcon(m_lightIcon);
		m_lightIcon = NULL;
		DestroyIcon(m_darkIcon);
		m_darkIcon = NULL;

	}
	virtual const TCHAR* localModeString() const
	{
		return _T("THIrd");
	}
	virtual const TCHAR* globalModeString() const
	{
		return _T("_THIrd");
	}
	virtual const AcRxClass* entityOsnapClass() const
	{
		return AsdkThirdOsnapInfo::desc();
	}
	virtual AcGiGlyph* glyph() const
	{
		return m_pGlyph;
	}
	virtual const TCHAR* tooltipString() const{
		return _T("Third of length");
	}
	virtual const ACHAR * displayString()
	{
		return localModeString();
	}
	virtual HICON menuIcon()
	{
		if(m_lightIcon ==NULL)
		{
			m_lightIcon = (HICON)::LoadImage(_hdllInstance, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 16, 16, 0);
		}
		return m_lightIcon;
	}
	virtual HICON menuIconForDarkTheme()
	{
		if(m_darkIcon == NULL)
		{
			m_darkIcon = (HICON)::LoadImage(_hdllInstance,MAKEINTRESOURCE(IDI_ICON2),IMAGE_ICON,16,16,0);
		}
		return m_darkIcon;
	}
private:
	AsdkThirdGlyph *m_pGlyph;
	HICON m_lightIcon;
	HICON m_darkIcon;
};

AsdkThirdOsnapMode thirdMode;
bool bSnapToSegments = false;



void
	snapToPolylineSegments()
{
	acutPrintf(_T("\nThird object snap will snap to polyline segments."));
	bSnapToSegments = true;
}

void
	snapToWholePolyline()
{
	acutPrintf(_T("\nThird object snap will snap to whole polyline."));
	bSnapToSegments = false;
}
void
	initApp()
{
	acedRegCmds->addCommand( _T("ASDK_THIRD"), _T("ASDK_SNAP2PLINE"),    _T("SNAP2PLINE"),   ACRX_CMD_TRANSPARENT, snapToWholePolyline );
	acedRegCmds->addCommand( _T("ASDK_THIRD"), _T("ASDK_SNAP2SEG"),      _T("SNAP2SEG"),     ACRX_CMD_TRANSPARENT, snapToPolylineSegments );

	// Add parent protocol extension class description object to hierarchy
	AsdkThirdOsnapInfo::rxInit();
	acrxBuildClassHierarchy();

	// Create the protocol extension objects
	AsdkThirdOsnapEntityInfo *pEntityInfo = new AsdkThirdOsnapEntityInfo;
	AsdkThirdOsnapCurveInfo *pCurveInfo = new AsdkThirdOsnapCurveInfo;
	AsdkThirdOsnapPolylineInfo *pPolylineInfo = new AsdkThirdOsnapPolylineInfo;

	// Register the protocol extension object with parent class object
	AcDbEntity::desc()->addX( AsdkThirdOsnapInfo::desc(), pEntityInfo );
	AcDbCurve::desc()->addX( AsdkThirdOsnapInfo::desc(), pCurveInfo );
	AcDbPolyline::desc()->addX( AsdkThirdOsnapInfo::desc(), pPolylineInfo );

	acdbCustomOsnapManager()->addCustomOsnapMode( &thirdMode );
	acdbCustomOsnapManager()->activateOsnapMode( _T("_third") );

	snapToWholePolyline();
}

void
	unloadApp()
{
	acedRegCmds->removeGroup( _T("ASDK_THIRD") );

	acdbCustomOsnapManager()->removeCustomOsnapMode( &thirdMode );

	AcDbEntity::desc()->delX( AsdkThirdOsnapInfo::desc() );
	AcDbCurve::desc()->delX( AsdkThirdOsnapInfo::desc() );
	AcDbPolyline::desc()->delX( AsdkThirdOsnapInfo::desc() );

	deleteAcRxClass( AsdkThirdOsnapInfo::desc() );
}







//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CCustomOsnap2App : public AcRxArxApp {

public:
	CCustomOsnap2App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;

		// TODO: Add your initialization code here
		initApp();

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here
		unloadApp();
		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}



} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CCustomOsnap2App)




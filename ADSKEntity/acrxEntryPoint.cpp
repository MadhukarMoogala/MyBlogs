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
#include "ADSKMyEntity.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("ADSK")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CADSKEntityApp : public AcRxArxApp {

public:
	CADSKEntityApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
	
	// The ACED_ARXCOMMAND_ENTRY_AUTO macro can be applied to any static member 
	// function of the CADSKEntityApp class.
	// The function should take no arguments and return nothing.
	//
	// NOTE: ACED_ARXCOMMAND_ENTRY_AUTO has overloads where you can provide resourceid and
	// have arguments to define context and command mechanism.
	
	// ACED_ARXCOMMAND_ENTRY_AUTO(classname, group, globCmd, locCmd, cmdFlags, UIContext)
	// ACED_ARXCOMMAND_ENTRYBYID_AUTO(classname, group, globCmd, locCmdId, cmdFlags, UIContext)
	// only differs that it creates a localized name using a string in the resource file
	//   locCmdId - resource ID for localized command

	// Modal Command with localized name
	// ACED_ARXCOMMAND_ENTRY_AUTO(CADSKEntityApp, ADSKMyGroup, MyCommand, MyCommandLocal, ACRX_CMD_MODAL)
	static void ADSKMyGroupMyCommand () {
		// Put your command code here
		MyEntityCommand();
	}

	static void MyEntityCommand()
	{
		AcDbBlockTable *pBlockTable ;
		if ( acdbHostApplicationServices()->workingDatabase()->getBlockTable (pBlockTable, AcDb::kForRead) == Acad::eOk )
		{
			// Get the Model Space record and open it for write. This will be the owner of the new employee entity.
			AcDbBlockTableRecord *pSpaceRecord ;
			if ( pBlockTable->getAt (ACDB_MODEL_SPACE, pSpaceRecord, AcDb::kForWrite) == Acad::eOk )
			{
				AcGeTol tol;
				ADSKMyEntity* pEnt = new ADSKMyEntity();
				pEnt->addVertexAt(0,AcGePoint2d(0.0,0.0));
				pEnt->addVertexAt(1,AcGePoint2d(0.0,10.0));
				pEnt->addVertexAt(2,AcGePoint2d(10.0,10.0));
				pEnt->addVertexAt(3,AcGePoint2d(10.0,0.0));
				pEnt->addVertexAt(4,AcGePoint2d(0.0,0.0));
				pEnt->makeClosedIfStartAndEndVertexCoincide(tol.equalPoint());
				pEnt->setClosed(Adesk::kTrue);
				Adesk::UInt16 greenColor = 4;
				pEnt->setColorIndex(greenColor);
				AcDbObjectId idObj ;
				if ( pSpaceRecord->appendAcDbEntity (idObj, pEnt) == Acad::eOk )
					pEnt->close () ;
				else
					delete pEnt ;
				pSpaceRecord->close () ;
			}
			pBlockTable->close () ;
		}
	}

	
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CADSKEntityApp)
ACED_ARXCOMMAND_ENTRY_AUTO(CADSKEntityApp, ADSKMyGroup, MyCommand, MyCommandLocal, ACRX_CMD_MODAL, NULL)



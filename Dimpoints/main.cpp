// (C) Copyright 1997-1999 by Autodesk, Inc.
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
//    The close-to-minimal ARX program demonstate how to make a custom
//    text-looking entity to be displayed parallel to the current view screen.
//    The user would expand this program by allowing some user inputs such as
//	  getting a point to locate the custom entity to a user desired position.
//
//    The application defines an AutoCAD command my_text.
//

#include "StdAfx.h"
#include <dbosnap2.h>
#include <dbobjptr2.h>
#include <acedads.h>
#include <tchar.h>
#include <rxboiler.h>


#pragma region customClass for Filer

class ArxDbgReferenceFiler: public AcDbDwgFiler {

public:
    ACRX_DECLARE_MEMBERS(ArxDbgReferenceFiler);

                        ArxDbgReferenceFiler() {}
    virtual             ~ArxDbgReferenceFiler() {}

    virtual Acad::ErrorStatus   filerStatus() const { return Acad::eOk; }
    virtual AcDb::FilerType     filerType() const   { return AcDb::kIdFiler; }
    virtual void                setFilerStatus(Acad::ErrorStatus) {}
    virtual void                resetFilerStatus()      {}

    virtual Acad::ErrorStatus   readHardOwnershipId(AcDbHardOwnershipId*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeHardOwnershipId(const AcDbHardOwnershipId&);

    virtual Acad::ErrorStatus   readSoftOwnershipId(AcDbSoftOwnershipId*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeSoftOwnershipId(const AcDbSoftOwnershipId&);

    virtual Acad::ErrorStatus   readHardPointerId(AcDbHardPointerId*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeHardPointerId(const AcDbHardPointerId&);

    virtual Acad::ErrorStatus   readSoftPointerId(AcDbSoftPointerId*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeSoftPointerId(const AcDbSoftPointerId&);

    virtual Acad::ErrorStatus   readString(ACHAR **)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeString(const ACHAR *)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readString(AcString &)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeString(const AcString &)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readBChunk(ads_binary*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeBChunk(const ads_binary&)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readAcDbHandle(AcDbHandle*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeAcDbHandle(const AcDbHandle&)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readInt64(Adesk::Int64*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeInt64(Adesk::Int64)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readInt32(Adesk::Int32*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeInt32(Adesk::Int32)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readInt16(Adesk::Int16*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeInt16(Adesk::Int16)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readInt8(Adesk::Int8*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeInt8(Adesk::Int8)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readUInt64(Adesk::UInt64*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeUInt64(Adesk::UInt64)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readUInt32(Adesk::UInt32*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeUInt32(Adesk::UInt32)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readUInt16(Adesk::UInt16*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeUInt16(Adesk::UInt16)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readUInt8(Adesk::UInt8*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeUInt8(Adesk::UInt8)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readBoolean(Adesk::Boolean*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeBoolean(Adesk::Boolean)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readBool(bool*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeBool(bool)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readDouble(double*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeDouble(double)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readPoint2d(AcGePoint2d*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writePoint2d(const AcGePoint2d&)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readPoint3d(AcGePoint3d*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writePoint3d(const AcGePoint3d&)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readVector2d(AcGeVector2d*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeVector2d(const AcGeVector2d&)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readVector3d(AcGeVector3d*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeVector3d(const AcGeVector3d&)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readScale3d(AcGeScale3d*)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeScale3d(const AcGeScale3d&)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readBytes(void*, Adesk::UIntPtr)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeBytes(const void*, Adesk::UIntPtr)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   readAddress(void**)
                                        { return Acad::eOk; }
    virtual Acad::ErrorStatus   writeAddress(const void*)
                                        { return Acad::eOk; }

    virtual Acad::ErrorStatus   seek(Adesk::Int64 nOffset, int nMethod)
                                        { return Acad::eOk;; }
    virtual Adesk::Int64        tell() const
                                        { return 0L; }

        void            reset();        // set all arrays to empty

    // data members
    AcDbObjectIdArray   m_hardPointerIds;  
    AcDbObjectIdArray   m_softPointerIds;  
    AcDbObjectIdArray   m_hardOwnershipIds;  
    AcDbObjectIdArray   m_softOwnershipIds;  
};

ACRX_CONS_DEFINE_MEMBERS(ArxDbgReferenceFiler, AcDbDwgFiler, 0);

Acad::ErrorStatus
ArxDbgReferenceFiler::writeHardOwnershipId(const AcDbHardOwnershipId& id)
{
	if (id.isNull() == false)
		m_hardOwnershipIds.append(id);
	return Acad::eOk;
}

Acad::ErrorStatus
ArxDbgReferenceFiler::writeSoftOwnershipId(const AcDbSoftOwnershipId& id)
{
	if (id.isNull() == false)
		m_softOwnershipIds.append(id);
	return Acad::eOk;
}

Acad::ErrorStatus
ArxDbgReferenceFiler::writeHardPointerId(const AcDbHardPointerId& id)
{
	if (id.isNull() == false)
		m_hardPointerIds.append(id);
	return Acad::eOk;
}

Acad::ErrorStatus
ArxDbgReferenceFiler::writeSoftPointerId(const AcDbSoftPointerId& id)
{
	if (id.isNull() == false)
		m_softPointerIds.append(id);
	return Acad::eOk;
}

void
ArxDbgReferenceFiler::reset()
{
    m_hardPointerIds.setLogicalLength(0);
    m_softPointerIds.setLogicalLength(0);
    m_hardOwnershipIds.setLogicalLength(0);
    m_softOwnershipIds.setLogicalLength(0);
}

#pragma endregion
#pragma region Utilities
Acad::ErrorStatus postToDatabase (/*[in]*/AcDbDatabase *pDb /*=NULL*/, AcDbEntity *pEnt, AcDbObjectId &idObj) {
	//----- Purpose:
	//----- Adds an entity to the MODEL_SPACE of the database given in argument.
	//-----   * pDb:   pointer to the databse where to add the entity,
	//-----            if NULL, then the curretn database is used.
	//-----   * pEnt:  pointer to an entity instance.
	//-----   * idObj: it will contain the assign ID to the object if successfully added to the database.
	//----- Note:
	//-----   The entity object is closed while we return from that function. Only the idObj can be used after.
	assert ( pEnt != NULL ) ;

	if ( pDb == NULL )
		pDb =acdbHostApplicationServices ()->workingDatabase () ;
	//----- Get a pointer to the current drawing
	//----- and get the drawing's block table. Open it for read.
	Acad::ErrorStatus es ;
	AcDbBlockTable *pBlockTable ;
	if ( (es =pDb->getBlockTable (pBlockTable, AcDb::kForRead)) == Acad::eOk ) {
		//----- Get the Model Space record and open it for write. This will be the owner of the new line.
		AcDbBlockTableRecord *pSpaceRecord ;
		if ( (es =pBlockTable->getAt (ACDB_MODEL_SPACE, pSpaceRecord, AcDb::kForWrite)) == Acad::eOk ) {
			//----- Append pEnt to Model Space, then close it and the Model Space record.
			if ( (es =pSpaceRecord->appendAcDbEntity (idObj, pEnt)) == Acad::eOk )
				pEnt->close () ;
			pSpaceRecord->close () ;
		}
		pBlockTable->close () ;
	}
	//----- It is good programming practice to return an error status
	return (es) ;
}
void makePointEnt(const AcGePoint3d& pt, int colorIndex)
{
    int mode = 0;
 	resbuf rb;
	if (acedGetVar(_T("PDMODE"), &rb)== RTNORM)
	{
        ASSERT(rb.restype == RTSHORT);
        mode = rb.resval.rint;
    }

    if (mode == 0)
	{
    rb.restype = RTSHORT;
    rb.resval.rint = 99;
	acedSetVar(_T("PDMODE"),&rb);
	}

    AcDbPoint* tmpPt = new AcDbPoint(pt);
    tmpPt->setColorIndex(colorIndex);
	AcDbObjectId idObj;
	postToDatabase(acdbHostApplicationServices()->workingDatabase(),tmpPt,idObj);
	tmpPt->close();
}

AcGePoint2d get2dPoint(AcGePoint3d& pt3d)
{
	return AcGePoint2d(pt3d.x,pt3d.y);
}

PCTSTR booleanToStr(bool b, CString& str)
{
    if (b)
        str = _T("True");
    else
        str = _T("False");

    return str;
}

void cleanLineArray(AcArray<AcDbLine*> &lines)
{
	int sizeofArray = lines.length();
		
		for(int i =0; i< sizeofArray;i++)
		{
			AcDbLine* tempLine = lines.at(i);
			if(tempLine != NULL) delete tempLine;
		}
		lines.setLogicalLength(0);
}

/*Creates intersection points and wipes off the array*/
void makeIntersectionPoints(AcArray<AcDbLine*> &lines)
{
	
	AcDb::Intersect type = AcDb::Intersect::kExtendBoth;
	AcGePoint3dArray points;	
		int sizeofArray = lines.length();
		/*As there are only 3 Lines in cylic order, this logic works! For other cases this logic is stupid*/
		for(int i =0; i< sizeofArray;i++)
		{
			AcDbObjectId id,rayId;
			AcDbLine* tempLine = lines.at(i);
			if(tempLine != NULL)
			{

			/*i+1 & i++ makes huge here difference*/
			bool good = eOkVerify(tempLine->intersectWith(lines.at(i== sizeofArray-1 ? 0:i+1),type,points));

			if(good && points.length() > 0 )
			{
				makePointEnt(points.first(),3);
				points.setLogicalLength(0);
			}
			
			}
			
		}
	cleanLineArray(lines);
		
}


#pragma endregion
#pragma region Commands
void test1()
{
	Adesk::Boolean status = false;
	ads_name ename;
	ads_point pt;
	if(RTNORM !=(acedEntSel(_T(""),ename,pt)))return;
	AcDbObjectId objid;
	if(!eOkVerify(acdbGetObjectId(objid,ename)))return;
	AcDbObjectPointer<AcDbAlignedDimension> entity(objid,AcDb::kForRead);
	
	if(entity != NULL)
	{

		/*Some point lying somewhere on Dimension line*/
		AcGePoint3d dimLoc = entity->dimLinePoint();

		/*Extension Line Orgins*/
		AcGePoint3d xPt1 = entity->xLine1Point();
		AcGePoint3d xPt2 = entity->xLine2Point();

		/*Do the geometry work*/
		AcGeLine2d entityLine = AcGeLine2d(AcGePoint2d(xPt1.x,xPt1.y),AcGePoint2d(xPt2.x,xPt2.y));
		AcGeLine2d dimensionLine = AcGeLine2d(AcGePoint2d(dimLoc.x,dimLoc.y),entityLine.direction());
		AcGeLine2d lineStartingFromxPt1;
		entityLine.getPerpLine(get2dPoint(xPt1),lineStartingFromxPt1);
		AcGeLine2d lineStartingFromxPt2;
		entityLine.getPerpLine(get2dPoint(xPt2),lineStartingFromxPt2);
		AcGePoint2d interSection1;
		status = lineStartingFromxPt1.intersectWith(dimensionLine,interSection1);
		if(status)
		{
			/*Utility to create and post Point entity*/
			makePointEnt(AcGePoint3d(interSection1.x,interSection1.y,0.00),3);
		}

		AcGePoint2d interSection2;
		status = lineStartingFromxPt2.intersectWith(dimensionLine,interSection2);
		if(status)
		{
			makePointEnt(AcGePoint3d(interSection2.x,interSection2.y,0.00),3);
		}

	}

 }

void test2()
{
	Adesk::Boolean isConstraintChanged = false;
	ads_name ename;
	ads_point pt;
	if(RTNORM !=(acedEntSel(_T("Select Rotated Dimension"),ename,pt)))return;
	AcDbObjectId objid;
	if(!eOkVerify(acdbGetObjectId(objid,ename)))return;
	AcDbSmartObjectPointer<AcDbRotatedDimension> entity(objid,AcDb::kForWrite);
	AcArray<AcDbLine*> lines;

	if(entity != NULL )
	{
		/*Explode is not possible on Constraint Dynamic*/
		if(!entity->isConstraintDynamic())
		{
			AcDbVoidPtrArray entitySet;


			if(!eOkVerify(entity->explode(entitySet))) return;

			for(int i=0; i < entitySet.length(); i++)
			{
				AcDbObject* ent = (AcDbObject*&)entitySet.at(i);
				if(ent->isKindOf(AcDbLine::desc()))
								{
									AcDbLine* pLine = new AcDbLine();
									pLine = AcDbLine::cast(ent->clone());
									if(pLine != NULL)
										lines.append(pLine);


								}
				else continue;
			}
			if(lines.length() > 0)
			{
				makeIntersectionPoints(lines);
				

			}

			/*Clear */
			for(int i=0; i < entitySet.length(); i++)
			{
				AcDbObjectId ObjId;
				AcDbEntity *pNewEnt=AcDbEntity::cast((AcRxObject*)entitySet[i]);
				if(pNewEnt != NULL)
				{
					delete pNewEnt;
				}
				else continue;
			}




		}
		/* Logic is to dig in to hardpointer references of Dimension,
		find the owner holding dimension entity lines
		*/
		else /*Could be Constrained on Dimension*/
		{
			/*We need to change constraint form, to help get Lines and restore back*/
			entity->setConstraintDynamic(false);


			if(!eOkVerify(entity->downgradeOpen())) return;

			ArxDbgReferenceFiler filer;
			AcDbObjectIdArray   hardPointerIds;
			AcDbObjectId tobePurgeLater;
			entity->dwgOut(&filer);

			hardPointerIds = filer.m_hardPointerIds;
			AcDbObject* tmpObj;
			int len = hardPointerIds.length();
			for (int i=0; i<len; i++)
			{
				// might have passed in erased ones
				if (!eOkVerify(acdbOpenAcDbObject(tmpObj, hardPointerIds[i], AcDb::kForRead, true))) return; 

				if(tmpObj->isKindOf(AcDbBlockTableRecord::desc()))
				{
					AcDbBlockTableRecord* btr = AcDbBlockTableRecord::cast(tmpObj);
					if(btr->isAnonymous())
					{
						tobePurgeLater = hardPointerIds[i];

						AcDbBlockTableRecordIterator* iter = NULL;
						btr->newIterator(iter);
						AcDbEntity* ent;
						/*Filter out only lines*/
						for (; !iter->done(); iter->step()) {
							if (!eOkVerify(iter->getEntity(ent, AcDb::kForRead))) return;
							if(ent->isKindOf(AcDbLine::desc()))
							{
								AcDbLine* pLine = new AcDbLine();
								pLine = AcDbLine::cast(ent->clone());
								if(pLine != NULL)
									lines.append(pLine);


							}
							if(!eOkVerify(ent->close())) return;	


						}
						delete iter;
						break; /*further dredging not required*/
					}
				}
				/*Could be ltr,str,vtr etc*/
				else continue;
			}

			if(!eOkVerify(tmpObj->close())) return;

			if(lines.length() > 0)	
			{
				makeIntersectionPoints(lines);
			}
			/*Reverse the changes*/
			if(eOkVerify(entity->upgradeOpen()))
			{
				entity->setConstraintDynamic(true);
				if(!eOkVerify(entity->downgradeOpen())) return;
			}

			/*When we switch from Dynamic constraint form to annotation, Acad creates an anonymous.
			We don't want leave this trace in customer database, hence purge */
			AcDbObjectIdArray purgableIds;
			purgableIds.append(tobePurgeLater);

			if(!eOkVerify(acdbHostApplicationServices()->workingDatabase()->purge(purgableIds))) return;
		}

	


}
}



#pragma endregion


//**************************************************************
extern "C"
AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void *pkt)
//**************************************************************
{
	switch(msg)
	{
		case AcRx::kInitAppMsg:
			acrxDynamicLinker->unlockApplication(pkt);
			acrxDynamicLinker->registerAppMDIAware(pkt);
			
			
			acrxBuildClassHierarchy();

			acedRegCmds->addCommand(_T("SIMPLETEST"), _T("TA"), _T("TA"), ACRX_CMD_MODAL, test1);
			acedRegCmds->addCommand(_T("SIMPLETEST"), _T("TR"), _T("TR"), ACRX_CMD_MODAL, test2);
			break;
		case AcRx::kUnloadAppMsg:
			 // Remove the AsdkOwnerDemo class from the ACRX runtime
			// class hierarchy.  If this is done while the database is
			// still active, it should cause all objects of class
			// AsdkOwnerDemo to be turned into proxies.
			// 
			
			
			
   			acedRegCmds->removeGroup(_T("SIMPLETEST"));
			break;
		default:
			break;
	}
	return AcRx::kRetOK;
}
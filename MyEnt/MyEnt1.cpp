#include "StdAfx.h"
#include "adesk.h"
#include "acdb.h"
#include "dbmain.h"
#include "dbproxy.h"
#include "dbents.h"
#include "drawable.h"
#include "acgi.h"
#include "dbapserv.h"
#include "MyEnt1.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 MyEnt1::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	MyEnt1, AcDb3dSolid,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, MYENT1,
	ADSMYDBXAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
MyEnt1::MyEnt1() : AcDb3dSolid()
	{
	}

MyEnt1::~MyEnt1()
	{
	}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus MyEnt1::dwgOutFields(AcDbDwgFiler *pFiler) const
	{
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDb3dSolid::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(MyEnt1::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params

	if (pFiler != NULL)
		{
		pFiler->writePoint3d(m_ptSP);
		pFiler->writePoint3d(m_ptEP);
		}
	return (pFiler->filerStatus());
	}

Acad::ErrorStatus MyEnt1::dwgInFields(AcDbDwgFiler *pFiler)
	{
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDb3dSolid::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > MyEnt1::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//----- Read params
	if (pFiler != NULL)
		{
		pFiler->readPoint3d(&m_ptSP);
		pFiler->readPoint3d(&m_ptEP);
		}
	return (pFiler->filerStatus());
	}

Adesk::Boolean MyEnt1::subWorldDraw(AcGiWorldDraw *mode)
	{
	assertReadEnabled();

	AcDbEntity::subWorldDraw(mode);

	AcGePoint3d points[2];

	mode->subEntityTraits().setSelectionMarker(kCustomEdgeGsMarker);
	points[0] = AcGePoint3d(m_ptSP.x, m_ptSP.y, 0.0);
	points[1] = AcGePoint3d(m_ptEP.x, m_ptEP.y, 0.0);
	mode->geometry().polyline(2, points, &AcGeVector3d::kZAxis);

	// Vertex subentities are endpoints of the edge subentity
	//
	mode->subEntityTraits().setSelectionMarker(kCustomVertex1GsMarker);
	mode->geometry().polypoint(1, &points[0], &AcGeVector3d::kZAxis);

	mode->subEntityTraits().setSelectionMarker(kCustomVertex2GsMarker);
	mode->geometry().polypoint(1, &points[1], &AcGeVector3d::kZAxis);

	return Adesk::kTrue;
	}

AcDbEntity* MyEnt1::subSubentPtr(const AcDbFullSubentPath& path) const
{
const AcDbSubentId subEntId = path.subentId();

if (isCustomSubentId(subEntId))
{
AcDbEntity* pSubEntity = nullptr;
const Adesk::GsMarker subentIndex = subEntId.index();

switch (subEntId.type())
	{
	case AcDb::kEdgeSubentType:
	{
	if (subentIndex == kCustomEdgeSubentIndex)
		{
		pSubEntity = new AcDbLine(m_ptSP, m_ptEP); 
		}
	}
	break;
	case AcDb::kVertexSubentType:
	{
	if (subentIndex == kCustomVertex1SubentIndex)
		{
		pSubEntity = new AcDbPoint(m_ptSP); 
		}
	else if (subentIndex == kCustomVertex2SubentIndex)
		{
		pSubEntity = new AcDbPoint(m_ptEP);
		}
	}
	break;
	}
ASSERT(pSubEntity != nullptr);
return pSubEntity;
}

// If not data of the derived class, use the base class implementation
//
return AcDb3dSolid::subSubentPtr(path);
}

Acad::ErrorStatus MyEnt1::subGetSubentPathsAtGsMarker(AcDb::SubentType type,
												Adesk::GsMarker gsMark,
											const AcGePoint3d & pickPoint, 
											const AcGeMatrix3d & viewXform,
											int & numPaths, 
											AcDbFullSubentPath *& subentPaths,
											int numInserts, 
											AcDbObjectId * entAndInsertStack) const
{
assertReadEnabled();

numPaths = 0;
subentPaths = NULL;

if (gsMark == 0)
return Acad::eInvalidInput;

if (isCustomGsMarker(gsMark))
{
switch (type)
	{
	case AcDb::kEdgeSubentType:
	{
	if (gsMark == kCustomEdgeGsMarker)
		{
		numPaths = 1;
		subentPaths = new AcDbFullSubentPath[1];
		subentPaths[0] = AcDbFullSubentPath(objectId(),
						AcDb::kEdgeSubentType,
						kCustomEdgeSubentIndex);
		}
	}
	break;
	case AcDb::kVertexSubentType:
	{
	if (gsMark == kCustomEdgeGsMarker)
		{
		numPaths = 2;
		subentPaths = new AcDbFullSubentPath[2];
		subentPaths[0] = AcDbFullSubentPath(objectId(),
							AcDb::kVertexSubentType,
							kCustomVertex1SubentIndex);
		subentPaths[1] = AcDbFullSubentPath(objectId(),
						AcDb::kVertexSubentType,
						kCustomVertex2SubentIndex);
		}
	}
	break;
	}
ASSERT(numPaths != 0);
return Acad::eOk;
}

// If not data of the derived class, use the base class implementation
//
return AcDb3dSolid::subGetSubentPathsAtGsMarker(type,
											gsMark, 
											pickPoint,
											viewXform,
											numPaths,
											subentPaths,
											numInserts, 
											entAndInsertStack);
}

Acad::ErrorStatus MyEnt1::subGetGsMarkersAtSubentPath(const AcDbFullSubentPath& path, 
												AcDbIntPtrArray&  gsMarkers) const
{
const AcDbSubentId subEntId = path.subentId();

if (isCustomSubentId(subEntId))
{
gsMarkers.removeAll();

const Adesk::GsMarker subentIndex = subEntId.index();

switch (subEntId.type())
	{
	case AcDb::kEdgeSubentType:
	{
	if (subentIndex == kCustomEdgeSubentIndex)
		{
		gsMarkers.append(kCustomEdgeGsMarker);
		}
	}
	break;
	case AcDb::kVertexSubentType:
	{
	if (subentIndex == kCustomVertex1SubentIndex || 
		subentIndex == kCustomVertex2SubentIndex)
		{
		gsMarkers.append(kCustomEdgeGsMarker);
		}
	}
	break;
	}
ASSERT(!gsMarkers.isEmpty());
return Acad::eOk;
}

// If not data of the derived class, use the base class implementation
//
return AcDb3dSolid::subGetGsMarkersAtSubentPath(path, gsMarkers);
}

Adesk::UInt32 MyEnt1::subSetAttributes(AcGiDrawableTraits *traits)
{
assertReadEnabled();
return AcDb3dSolid::subSetAttributes(traits);
}

Acad::ErrorStatus MyEnt1::subGetOsnapPoints(AcDb::OsnapMode osnapMode,
Adesk::GsMarker gsSelectionMark,
const AcGePoint3d& pickPoint,
const AcGePoint3d& lastPoint,
const AcGeMatrix3d& viewXform,
AcGePoint3dArray& snapPoints,
AcDbIntArray& geomIds) const
{
assertReadEnabled();

if (MyEnt1::isCustomGsMarker(gsSelectionMark))
{
switch (osnapMode)
	{
	case AcDb::kOsModeEnd:
	{
	snapPoints.append(m_ptSP);
	snapPoints.append(m_ptEP);
	}
	break;
	case AcDb::kOsModeNear:
	{
	AcGeVector3d viewDir(viewXform(Z, 0), viewXform(Z, 1), viewXform(Z, 2));
	AcGeLine3d line3d(m_ptSP, m_ptEP);
	snapPoints.append(line3d.projClosestPointTo(pickPoint, viewDir));
	}
	break;
	}
return Acad::eOk;
}

// If not data of the derived class, use the base class implementation
//
return AcDb3dSolid::subGetOsnapPoints(osnapMode,
										gsSelectionMark,
										pickPoint, 
										lastPoint, 
										viewXform,
										snapPoints,
										geomIds);
}

Acad::ErrorStatus MyEnt1::subTransformBy(const AcGeMatrix3d& xform)
{
assertWriteEnabled();

m_ptSP.transformBy(xform);
m_ptEP.transformBy(xform);

return AcDb3dSolid::subTransformBy(xform);
}


//Always check iff 100000 <= gsMarker <= 1000002
bool MyEnt1::isCustomGsMarker(Adesk::GsMarker gsMarker)
{
return kCustomGsMarkerMin <= gsMarker &&
	gsMarker <= kCustomGsMarkerMax;
}


bool MyEnt1::isCustomSubentId(const AcDbSubentId& subentId)
{
const Adesk::GsMarker index = subentId.index();
return kCustomSubentIdIndexMin <= index &&
	index <= kCustomSubentIdIndexMax;
}



static Acad::ErrorStatus postToDatabase(/*[in]*/AcDbDatabase *pDb /*=NULL*/, AcDbEntity *pEnt, AcDbObjectId &idObj) {
	//----- Purpose:
	//----- Adds an entity to the MODEL_SPACE of the database given in argument.
	//-----   * pDb:   pointer to the databse where to add the entity,
	//-----            if NULL, then the curretn database is used.
	//-----   * pEnt:  pointer to an entity instance.
	//-----   * idObj: it will contain the assign ID to the object if successfully added to the database.
	//----- Note:
	//-----   The entity object is closed while we return from that function. Only the idObj can be used after.
	assert(pEnt != NULL);

	if (pDb == NULL)
		pDb = acdbHostApplicationServices()->workingDatabase();
	//----- Get a pointer to the current drawing
	//----- and get the drawing's block table. Open it for read.
	Acad::ErrorStatus es;
	AcDbBlockTable *pBlockTable;
	if ((es = pDb->getBlockTable(pBlockTable, AcDb::kForRead)) == Acad::eOk) {
		//----- Get the Model Space record and open it for write. This will be the owner of the new line.
		AcDbBlockTableRecord *pSpaceRecord;
		if ((es = pBlockTable->getAt(ACDB_MODEL_SPACE, pSpaceRecord, AcDb::kForWrite)) == Acad::eOk) {
			//----- Append pEnt to Model Space, then close it and the Model Space record.
			if ((es = pSpaceRecord->appendAcDbEntity(idObj, pEnt)) == Acad::eOk)
				pEnt->close();
			pSpaceRecord->close();
			}
		pBlockTable->close();
		}
	//----- It is good programming practice to return an error status
	return (es);
	}


void MyEnt1::createTestInstance()
	{
	MyEnt1 *pMyEnt1 = new MyEnt1();

	AcGePoint3d SP = AcGePoint3d(2.0, 0.0, 0.0);
	AcGePoint3d EP = AcGePoint3d(8.0, 0.0, 0.0);

	pMyEnt1->setDatabaseDefaults();

	pMyEnt1->m_ptSP = SP;
	pMyEnt1->m_ptEP = EP;

	AcGeVector3d normalLen = EP - SP;

	AcGeVector3d xDirection = (EP - SP).normalize();
	AcGeVector3d yDirection = xDirection.perpVector().normalize();
	AcGeVector3d zDirection = xDirection.crossProduct(yDirection).normalize();
	AcGeMatrix3d matrix;
	matrix.setCoordSystem(SP + (EP - SP) * 0.5, zDirection, yDirection, xDirection);

	double radius = 3.0;
	Acad::ErrorStatus es = pMyEnt1->createFrustum(normalLen.length(), radius, radius, radius);
	pMyEnt1->transformBy(matrix);

	pMyEnt1->m_ptSP = SP;
	pMyEnt1->m_ptEP = EP;

	AcDbObjectId oid;
	postToDatabase(NULL, pMyEnt1, oid);
	}

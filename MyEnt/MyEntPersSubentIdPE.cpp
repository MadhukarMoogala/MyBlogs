//-----------------------------------------------------------------------------
//----- MyEntPersSubentIdPE.cpp : Implementation of MyEntPersSubentIdPE
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "dbmain.h"
#include "gemat3d.h"
#include "gelnsg3d.h"
#include "AcDbAssocGlobal.h"
#include "dbRevolveOptions.h"
#include "dbsweepoptions.h"
#include "dbloftoptions.h"
#include "AcDbCompoundObjectId.h"
#include "AcDbAssocEdgePersSubentId.h"
#include "MyEntPersSubentIdPE.h"
#include "MyEnt1.h"

ACRX_CONS_DEFINE_MEMBERS(MyEntPersSubentIdPE, AcDbAssocPersSubentIdPE, 1);


AcDbAssocPersSubentId* MyEntPersSubentIdPE::createNewPersSubent(
	AcDbEntity*                 pEntity,
	AcDbDatabase*               pDatabase,
	const AcDbCompoundObjectId& compId,
	const AcDbSubentId&         subentId)
	{
	if (MyEnt1::isCustomSubentId(subentId))
		{
		AcDbAssocPersSubentId* pNewCustomPersSubentId = nullptr;

		// In the following code we can just use the given transient subentId
		// as the persistent subent id as it does not change. If it changed, a more
		// elaborate scheme would be needed to create a persistent subent id from the
		// transient one

		const Adesk::GsMarker subentIndex = subentId.index();

		switch (subentId.type())
			{
			case AcDb::kEdgeSubentType:
			{
			if (subentIndex == MyEnt1::kCustomEdgeSubentIndex)
				{
				pNewCustomPersSubentId = new AcDbAssocSimplePersSubentId(subentId);
				}
			}
			break;
			case AcDb::kVertexSubentType:
			{
			if (subentIndex == MyEnt1::kCustomVertex1SubentIndex)
				{
				pNewCustomPersSubentId = new AcDbAssocSimplePersSubentId(subentId);
				}
			else if (subentIndex == MyEnt1::kCustomVertex2SubentIndex)
				{
				pNewCustomPersSubentId = new AcDbAssocSimplePersSubentId(subentId);
				}
			}
			break;
			}
		ASSERT(pNewCustomPersSubentId != nullptr);
		return pNewCustomPersSubentId;
		}

	// If not data of the derived class, use the base class implementation.
	//
	// The AcDbAssocPersSubentIdPE class for AcDb3dSolid is not exported in RealDWG,
	// however we can obtain pointer to the PE object from the AcDb3dSolid entity
	//
	AcDbAssocPersSubentIdPE* const pSolidPersSubentIdPE = AcDbAssocPersSubentIdPE::cast(AcDb3dSolid::desc()->getX(AcDbAssocPersSubentIdPE::desc()));
	if (pSolidPersSubentIdPE == nullptr)
		{
		ASSERT(!"AcDbAssocPersSubentIdPE for AcDb3dSolid not found");
		return nullptr;
		}

	return pSolidPersSubentIdPE->createNewPersSubent(pEntity, pDatabase, compId, subentId);
	}


bool MyEntPersSubentIdPE::isCustomPersSubentId(const AcDbEntity* pEntity, const AcDbAssocPersSubentId* pPersSubentId)
	{
	const AcDbAssocSimplePersSubentId* const pCustomPersSubentId = AcDbAssocSimplePersSubentId::cast(pPersSubentId);
	if (pCustomPersSubentId == nullptr)
		return false;

	return MyEnt1::isCustomSubentId(pCustomPersSubentId->subentId(pEntity));
	}


Acad::ErrorStatus MyEntPersSubentIdPE::getTransientSubentIds(
	const AcDbEntity*            pEntity,
	AcDbDatabase*                pDatabase,
	const AcDbAssocPersSubentId* pPersSubentId,
	AcArray<AcDbSubentId>&       subents) const
	{
	const MyEnt1* const pMyEnt1 = MyEnt1::cast(pEntity);
	if (pMyEnt1 == nullptr)
		{
		ASSERT(false);
		return Acad::eNotThatKindOfClass;
		}

	if (isCustomPersSubentId(pEntity, pPersSubentId))
		{
		const AcDbAssocSimplePersSubentId* const pCustomPersSubentId = AcDbAssocSimplePersSubentId::cast(pPersSubentId);
		subents.append(pCustomPersSubentId->subentId(pEntity));
		return Acad::eOk;
		}

	// If not data of the derived class, use the base class implementation.
	//
	// The AcDbAssocPersSubentIdPE class for AcDb3dSolid is not exported in RealDWG,
	// however we can obtain pointer to the PE object from the AcDb3dSolid entity
	//
	AcDbAssocPersSubentIdPE* const pSolidPersSubentIdPE = AcDbAssocPersSubentIdPE::cast(AcDb3dSolid::desc()->getX(AcDbAssocPersSubentIdPE::desc()));
	if (pSolidPersSubentIdPE == nullptr)
		{
		ASSERT(!"AcDbAssocPersSubentIdPE for AcDb3dSolid not found");
		return Acad::eNotThatKindOfClass;
		}

	return pSolidPersSubentIdPE->getTransientSubentIds(pEntity, pDatabase, pPersSubentId, subents);
	}


Acad::ErrorStatus MyEntPersSubentIdPE::getAllSubentities(
	const AcDbEntity*      pEntity,
	AcDb::SubentType       subentType,
	AcArray<AcDbSubentId>& allSubentIds)
	{
	// First add subentities of the base class entity
	//
	AcDbAssocPersSubentIdPE* const pSolidPersSubentIdPE = AcDbAssocPersSubentIdPE::cast(AcDb3dSolid::desc()->getX(AcDbAssocPersSubentIdPE::desc()));
	if (pSolidPersSubentIdPE == nullptr)
		{
		ASSERT(!"AcDbAssocPersSubentIdPE for AcDb3dSolid not found");
		return Acad::eNotThatKindOfClass;
		}

	pSolidPersSubentIdPE->getAllSubentities(pEntity, subentType, allSubentIds);

	const MyEnt1* const pMyEnt1 = MyEnt1::cast(pEntity);
	if (pMyEnt1 == nullptr)
		{
		ASSERT(false);
		return Acad::eNotThatKindOfClass;
		}

	// Add subentities of the custom entity
	//
	switch (subentType)
		{
		case AcDb::kEdgeSubentType:
			allSubentIds.append(AcDbSubentId(AcDb::kEdgeSubentType, MyEnt1::kCustomEdgeSubentIndex));
			break;
		case AcDb::kVertexSubentType:
			allSubentIds.append(AcDbSubentId(AcDb::kVertexSubentType, MyEnt1::kCustomVertex1SubentIndex));
			allSubentIds.append(AcDbSubentId(AcDb::kVertexSubentType, MyEnt1::kCustomVertex2SubentIndex));
			break;
		}
	return Acad::eOk;
	}


Acad::ErrorStatus MyEntPersSubentIdPE::getEdgeVertexSubentities(
	const AcDbEntity*      pEntity,
	const AcDbSubentId&    edgeSubentId,
	AcDbSubentId&          startVertexSubentId,
	AcDbSubentId&          endVertexSubentId,
	AcArray<AcDbSubentId>& otherVertexSubentIds)
	{
	const MyEnt1* const pMyEnt1 = MyEnt1::cast(pEntity);
	if (pMyEnt1 == nullptr)
		{
		ASSERT(false);
		return Acad::eNotThatKindOfClass;
		}

	if (MyEnt1::isCustomSubentId(edgeSubentId))
		{
		if (edgeSubentId.index() == MyEnt1::kCustomEdgeSubentIndex)
			{
			startVertexSubentId = AcDbSubentId(AcDb::kVertexSubentType, MyEnt1::kCustomVertex1SubentIndex);
			endVertexSubentId = AcDbSubentId(AcDb::kVertexSubentType, MyEnt1::kCustomVertex2SubentIndex);
			return Acad::eOk;
			}
		else
			{
			ASSERT(!"Invalid edge subent index");
			return Acad::eInvalidIndex;
			}
		}

	// If not data of the derived class, use the base class implementation
	//
	AcDbAssocPersSubentIdPE* const pSolidPersSubentIdPE = AcDbAssocPersSubentIdPE::cast(AcDb3dSolid::desc()->getX(AcDbAssocPersSubentIdPE::desc()));
	if (pSolidPersSubentIdPE == nullptr)
		{
		ASSERT(!"AcDbAssocPersSubentIdPE for AcDb3dSolid not found");
		return Acad::eNotThatKindOfClass;
		}

	return pSolidPersSubentIdPE->getEdgeVertexSubentities(pEntity, edgeSubentId, startVertexSubentId, endVertexSubentId, otherVertexSubentIds);
	}


Acad::ErrorStatus MyEntPersSubentIdPE::getVertexSubentityGeometry(
	const AcDbEntity*   pEntity,
	const AcDbSubentId& vertexSubentId,
	AcGePoint3d&        vertexPosition)
	{
	const MyEnt1* const pMyEnt1 = MyEnt1::cast(pEntity);
	if (pMyEnt1 == nullptr)
		{
		ASSERT(false);
		return Acad::eNotThatKindOfClass;
		}

	if (MyEnt1::isCustomSubentId(vertexSubentId))
		{
		const Adesk::GsMarker subentIndex = vertexSubentId.index();
		Acad::ErrorStatus err = Acad::eOk;
		if (subentIndex == MyEnt1::kCustomVertex1SubentIndex)
			{
			vertexPosition = pMyEnt1->m_ptSP;
			}
		else if (subentIndex == MyEnt1::kCustomVertex2SubentIndex)
			{
			vertexPosition = pMyEnt1->m_ptEP;
			}
		else
			{
			ASSERT(!"Invalid vertex subent index");
			err = Acad::eInvalidIndex;
			}
		return err;
		}

	// If not data of the derived class, use the base class implementation
	//
	AcDbAssocPersSubentIdPE* const pSolidPersSubentIdPE = AcDbAssocPersSubentIdPE::cast(AcDb3dSolid::desc()->getX(AcDbAssocPersSubentIdPE::desc()));
	if (pSolidPersSubentIdPE == nullptr)
		{
		ASSERT(!"AcDbAssocPersSubentIdPE for AcDb3dSolid not found");
		return Acad::eNotThatKindOfClass;
		}

	return pSolidPersSubentIdPE->getVertexSubentityGeometry(pEntity, vertexSubentId, vertexPosition);
	}


Acad::ErrorStatus MyEntPersSubentIdPE::getEdgeSubentityGeometry(
	const AcDbEntity*   pEntity,
	const AcDbSubentId& edgeSubentId,
	AcGeCurve3d*&       pEdgeCurve)
	{
	const MyEnt1* const pMyEnt1 = MyEnt1::cast(pEntity);
	if (pMyEnt1 == nullptr)
		{
		ASSERT(false);
		return Acad::eNotThatKindOfClass;
		}

	if (MyEnt1::isCustomSubentId(edgeSubentId))
		{
		const Adesk::GsMarker subentIndex = edgeSubentId.index();

		if (subentIndex == MyEnt1::kCustomEdgeSubentIndex)
			{
			pEdgeCurve = new AcGeLineSeg3d(pMyEnt1->m_ptSP, pMyEnt1->m_ptEP);
			return Acad::eOk;
			}
		else
			{
			ASSERT(!"Invalid edge subent index");
			return Acad::eInvalidIndex;
			}
		}

	// If not data of the derived class, use the base class implementation
	//
	AcDbAssocPersSubentIdPE* const pSolidPersSubentIdPE = AcDbAssocPersSubentIdPE::cast(AcDb3dSolid::desc()->getX(AcDbAssocPersSubentIdPE::desc()));
	if (pSolidPersSubentIdPE == nullptr)
		{
		ASSERT(!"AcDbAssocPersSubentIdPE for AcDb3dSolid not found");
		return Acad::eNotThatKindOfClass;
		}

	return pSolidPersSubentIdPE->getEdgeSubentityGeometry(pEntity, edgeSubentId, pEdgeCurve);
	}

// Attaches AcDbAssocAnnotationPE to the custom entity
//
Acad::ErrorStatus AssocAnnotationsEnabler::enableForEntity(AcRxClass* pEntityClass)
	{
	if (pEntityClass == nullptr || !pEntityClass->isDerivedFrom(AcDbEntity::desc()))
		return Acad::eNotThatKindOfClass;

	AcRxClass* const pAssocAnnotationPEClass = AcRxClass::cast(acrxClassDictionary->at(L"AcDbAssocAnnotationPE"));

	if (pEntityClass->queryX(pAssocAnnotationPEClass) == nullptr)
		{
		AcRxObject* const pAssocAnnotationPE = pAssocAnnotationPEClass->create();
		pEntityClass->addX(pAssocAnnotationPEClass, pAssocAnnotationPE);
		}
	return Acad::eOk;
	}


// Detaches AcDbAssocAnnotationPE from the custom entity
//
Acad::ErrorStatus AssocAnnotationsEnabler::disableForEntity(AcRxClass* pEntityClass)
	{
	if (pEntityClass == nullptr || !pEntityClass->isDerivedFrom(AcDbEntity::desc()))
		return Acad::eNotThatKindOfClass;

	AcRxClass*  const pAssocAnnotationPEClass = AcRxClass::cast(acrxClassDictionary->at(L"AcDbAssocAnnotationPE"));
	AcRxObject* const pAssocAnnotationPE = pEntityClass->queryX(pAssocAnnotationPEClass);

	if (pAssocAnnotationPE != nullptr && pAssocAnnotationPE->isA() == pAssocAnnotationPEClass)
		{
		pEntityClass->delX(pAssocAnnotationPEClass);
		delete pAssocAnnotationPE;
		}
	return Acad::eOk;
	}


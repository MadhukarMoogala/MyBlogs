//-----------------------------------------------------------------------------
//----- MyEntPersSubentIdPE.h : Declaration of the MyEntPersSubentIdPE
//-----------------------------------------------------------------------------
#pragma once
#ifndef DLLIMPEXP
#define DLLIMPEXP __declspec(dllexport)
#endif

#include "dbmain.h"
#include "AcDbAssocSimplePersSubentId.h"
#include "AcDbAssocPersSubentIdPE.h"

class DLLIMPEXP MyEntPersSubentIdPE : public AcDbAssocPersSubentIdPE
	{

	public:
		ACRX_DECLARE_MEMBERS(MyEntPersSubentIdPE);

	public:
		MyEntPersSubentIdPE() {}
		virtual ~MyEntPersSubentIdPE() {}

		// Returns true iff the given AcDbAssocPersSubentId is for the geometry of the derived
		// MyEnt1 entity and not for geometry of the base AcDb3dSolid
		//
		static bool isCustomPersSubentId(const AcDbEntity*, const AcDbAssocPersSubentId*);

		virtual AcDbAssocPersSubentId* createNewPersSubent(AcDbEntity*                 pEntity,
			AcDbDatabase*               pDatabase,
			const AcDbCompoundObjectId& compId,
			const AcDbSubentId&         subentId);

		virtual Acad::ErrorStatus getTransientSubentIds(const AcDbEntity*            pEntity,
			AcDbDatabase*                pDatabase,
			const AcDbAssocPersSubentId* pPersSubentId,
			AcArray<AcDbSubentId>&       subents) const;

		virtual Acad::ErrorStatus getAllSubentities(const AcDbEntity*      pEntity,
			AcDb::SubentType       subentType,
			AcArray<AcDbSubentId>& allSubentIds);

		virtual Acad::ErrorStatus getEdgeVertexSubentities(const AcDbEntity*      pEntity,
			const AcDbSubentId&    edgeSubentId,
			AcDbSubentId&          startVertexSubentId,
			AcDbSubentId&          endVertexSubentId,
			AcArray<AcDbSubentId>& otherVertexSubentIds);

		virtual Acad::ErrorStatus getVertexSubentityGeometry(const AcDbEntity*   pEntity,
			const AcDbSubentId& vertexSubentId,
			AcGePoint3d&        vertexPosition);

		virtual Acad::ErrorStatus getEdgeSubentityGeometry(const AcDbEntity*   pEntity,
			const AcDbSubentId& edgeSubentId,
			AcGeCurve3d*&       pEdgeCurve);

		// Other PE methods should also be implemented, just forwarding to the 
		// AcDbAssocPersSubentIdPE for the AcDb3dSolid. If the PE for AcDb3dSolid
		// was exposed in RealDWG, MyEntPersSubentIdPE could just derive from this PE

	};

class AssocAnnotationsEnabler
	{
	public:
		static Acad::ErrorStatus enableForEntity(AcRxClass* pEntityClass);
		static Acad::ErrorStatus disableForEntity(AcRxClass* pEntityClass);
	};

#ifdef MYDBX_MODULE
//ACDB_REGISTER_OBJECT_ENTRY_AUTO(MyEntPersSubentIdPE)
#endif

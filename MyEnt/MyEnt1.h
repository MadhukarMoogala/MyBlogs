#pragma once

#ifndef DLLIMPEXP
#define DLLIMPEXP __declspec(dllexport)
#endif

#include "adesk.h"
#include "acdb.h"
#include "dbsol3d.h"

class DLLIMPEXP MyEnt1 : public AcDb3dSolid
	{
	public:
		ACRX_DECLARE_MEMBERS(MyEnt1);

		static void createTestInstance();

		MyEnt1();
		virtual ~MyEnt1();

		AcGePoint3d m_ptSP;
		AcGePoint3d m_ptEP;


		virtual AcDbEntity* subSubentPtr(const AcDbFullSubentPath& id) const;
		virtual Acad::ErrorStatus subGetSubentPathsAtGsMarker(AcDb::SubentType type,
			Adesk::GsMarker gsMark, const AcGePoint3d & pickPoint, const AcGeMatrix3d & viewXform, int & numPaths,
			AcDbFullSubentPath *& subentPaths, int numInserts, AcDbObjectId * entAndInsertStack) const;
		virtual Acad::ErrorStatus subGetGsMarkersAtSubentPath(const AcDbFullSubentPath& path, AcDbIntPtrArray&  gsMarkers) const;

		virtual Adesk::UInt32 subSetAttributes(AcGiDrawableTraits *traits);
		virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw* mode);
		virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* pFiler);
		virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* pFiler) const;
		virtual Acad::ErrorStatus subGetOsnapPoints(AcDb::OsnapMode osnapMode,
			Adesk::GsMarker gsSelectionMark,
			const AcGePoint3d& pickPoint,
			const AcGePoint3d& lastPoint,
			const AcGeMatrix3d& viewXform,
			AcGePoint3dArray& snapPoints,
			AcDbIntArray& geomIds) const;

		virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform);

		/*Thanks to Jiri Kripac,Sr.Architect*/
		// Gs markers and AcDbSubentId indices for the custom geometry must be in the
		// well-specified range so that they do not overlap with markers and indices of
		// the base entity. In this case we just assume that AcDb3dSolids do not use 
		// indicest latger than 1000000
		//
		static const Adesk::GsMarker kCustomGsMarkerMin = 1000000;
		static const Adesk::GsMarker kCustomGsMarkerMax = 1000000;
		static const Adesk::GsMarker kCustomSubentIdIndexMin = 1000000;
		static const Adesk::GsMarker kCustomSubentIdIndexMax = 1000002;

		static const Adesk::GsMarker kCustomEdgeGsMarker = kCustomGsMarkerMin;
		static const Adesk::GsMarker kCustomVertex1GsMarker = kCustomGsMarkerMin+1;
		static const Adesk::GsMarker kCustomVertex2GsMarker = kCustomGsMarkerMin+2;

		static const Adesk::GsMarker kCustomEdgeSubentIndex = kCustomSubentIdIndexMin;
		static const Adesk::GsMarker kCustomVertex1SubentIndex = kCustomSubentIdIndexMin + 1;
		static const Adesk::GsMarker kCustomVertex2SubentIndex = kCustomSubentIdIndexMin + 2;

		// Returns true iff the given Adesk::GsMarker or AcDbSubentId is for the geometry 
		// of the derived MyEnt1 entity and not for geometry of the base AcDb3dSolid
		//
		static bool isCustomGsMarker(Adesk::GsMarker);
		static bool isCustomSubentId(const AcDbSubentId&);

	protected:
		static Adesk::UInt32 kCurrentVersionNumber;

	};

#ifdef MYDBX_MODULE
//ACDB_REGISTER_OBJECT_ENTRY_AUTO(MyEnt1)
#endif

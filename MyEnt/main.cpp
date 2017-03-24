#include "StdAfx.h"
#include <dbosnap2.h>
#include <dbobjptr2.h>
#include <dbindex.h>
#include <dbboiler.h>
#include <dbents.h>
#include <core_rxmfcapi.h>
#include <acedCmdNF.h>
#include <dbloftedsurf.h>
#include <dbSubD.h>
#include <rxmfcapi.h>
#include <dbViewRepBlockReference.h>
#include <dbViewBorder.h>
#include <AdApplicationFrame.h>
#include "comdef.h"
#include"MyEnt1.h"
#include"MyEntPersSubentIdPE.h"

#define CALC_EPSILON  10E-5
static int YORN = 0;
#define M_PI       3.14159265358979323846   // pi

// Test1 command
// Insert MyEnt1 custom entity that uses custom AcDbAssocPersSubentIdPE
// for ensuring associativity of dimensions
static void Test1(void)
	{
	MyEnt1::createTestInstance();
	}





static MyEntPersSubentIdPE *pMyEntPersSubentIdPE = NULL;

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
			MyEnt1::rxInit();
			MyEntPersSubentIdPE::rxInit();
			acrxBuildClassHierarchy();//optional for build efficiency
		
			if (pMyEntPersSubentIdPE == NULL)
				{
				pMyEntPersSubentIdPE = new MyEntPersSubentIdPE();
				MyEnt1::desc()->addX(AcDbAssocPersSubentIdPE::desc(), pMyEntPersSubentIdPE);

				/*Explicit expose Annotation protocol, 
				in prinicpal For AcDb3dSolid and entities derived from it this PE should already be exposed so you should not need to do anything.*/
				
				/*---------------------------------------*/

				//AcRxClass* const pAssocAnnotationPEClass = AcRxClass::cast(acrxClassDictionary->at(L"AcDbAssocAnnotationPE"));

				//if (MyEnt1::desc()->queryX(pAssocAnnotationPEClass) == nullptr)
				//	{
				//	// This is wrong, MyEnt1 should already expose AcDbAssocAnnotationPE because 
				//	// AcDb3dSolid exposes it
				//	//
				//	ASSERT(!"MyEnt1 does not expose AcDbAssocAnnotationPE!!!");

				//	AssocAnnotationsEnabler::enableForEntity(MyEnt1::desc());
				//	}

				/*------------------------------*/

				}
			/*Load commands*/
			acedRegCmds->addCommand(_T("TestCmd"), _T("test1"), _T("test1"), ACRX_CMD_MODAL, Test1);
			
			break;
		case AcRx::kUnloadAppMsg:
			/*unload commands*/
			/*For Annotations*/
			//AssocAnnotationsEnabler::disableForEntity(MyEnt1::desc());
			MyEnt1::desc()->delX(MyEntPersSubentIdPE::desc());
			deleteAcRxClass(MyEnt1::desc());
			deleteAcRxClass(MyEntPersSubentIdPE::desc());
			acedRegCmds->removeGroup(_T("TestCmd"));
			acrxBuildClassHierarchy();

			break;
		default:
			break;
	}
	return AcRx::kRetOK;
}
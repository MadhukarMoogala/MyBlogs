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
#include <acedads.h>
#include <acdbimageutils.h>
#include <Unknwn.h>
#include <windows.h>


//I'm not able to figure out a way to avoid to method calls.

//We need size to allocate memory
extern "C" __declspec(dllexport) int getSizeOfCmds()
{
	AcEdCommandIterator* iter = acedRegCmds->iterator();
	if (iter == NULL)
	{
		return -1;
	}
	int size = 0;
	for (; !iter->done(); iter->next())
	{
		const AcEdCommand* pp = iter->command();
		size++;
	}
	return size;

}
/*
Using BSTR allows us to allocate the strings in the native code,
and have them deallocated in the managed code. That's because BSTR is allocated on the shared COM heap,
and the p / invoke marshaller understands them*/
extern "C" __declspec(dllexport) Acad::ErrorStatus cmdExtract(BSTR* comStringCmdNames)
{
	
	AcEdCommandIterator* iter = acedRegCmds->iterator();
	if (iter == NULL)
	{
		return Acad::eCreateFailed;
	}
	int i = 0;
	for (;!iter->done();iter->next())
	{
		const AcEdCommand* pp = iter->command();
		//the beauty is here, sometimes COM is nice 
		//Allocate here and DeAllocate in C#
		comStringCmdNames[i]  = ::SysAllocString(pp->globalName());
	    i++;
	}

	return Acad::eOk;
}


//**************************************************************
extern "C" AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt)
//**************************************************************
	{
	switch (msg) {
		case AcRx::kInitAppMsg:
			acrxDynamicLinker->unlockApplication(pkt);
			acrxDynamicLinker->registerAppMDIAware(pkt);
			break;
		case AcRx::kUnloadAppMsg:
			break;
		default:
			break;
		}
	return AcRx::kRetOK;
	}
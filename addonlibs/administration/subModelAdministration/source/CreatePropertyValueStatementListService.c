
/******************************************************************************
*
*   FILE
*   ----
*   CreatePropertyValueStatementListService.c
*
*   History
*   -------
*   2017-10-13   File created
*
*******************************************************************************
*
*   This file is generated by the 'acplt_builder' command
*
******************************************************************************/


#ifndef OV_COMPILE_LIBRARY_subModelAdministration
#define OV_COMPILE_LIBRARY_subModelAdministration
#endif


#include "subModelAdministration.h"
#include "libov/ov_macros.h"
#include "helper.h"

OV_DLLFNCEXPORT OV_RESULT subModelAdministration_CreatePropertyValueStatementListService_CallMethod(      
  OV_INSTPTR_services_Service pobj,       
  OV_UINT numberofInputArgs,       
  const void **packedInputArgList,       
  OV_UINT numberofOutputArgs,      
  void **packedOutputArgList,
  OV_UINT *typeArray       
) {
    /*    
    *   local variables
    */

	OV_INSTPTR_ov_object pParent = NULL;
	IdentificationType parentId;
	IdentificationType_init(&parentId);
	OV_STRING name = NULL;
	OV_UINT mask = 0;
	IdentificationType carrierId;
	IdentificationType_init(&carrierId);
	OV_UINT expressionLogic = 0;
	OV_UINT expressionSemantic = 0;
	IdentificationType propertyId;
	IdentificationType_init(&propertyId);
	OV_UINT view = 0;
	OV_UINT visibility = 0;
	OV_STRING status = NULL;
	OV_UINT result = NULL;

	packedOutputArgList[0] = ov_database_malloc(sizeof(OV_STRING));
	*(OV_STRING*)packedOutputArgList[0] = NULL;
	typeArray[0] = OV_VT_STRING;

	if (*(OV_UINT*)packedInputArgList[1] != 0){
		ov_string_setvalue(&status, "Only Uri as identifier are implemented");
		goto FINALIZE;
	}

	pParent = ov_path_getobjectpointer(*(OV_STRING*)(packedInputArgList[0]), 2);
	if (pParent == NULL){
		ov_string_setvalue(&status, "Find no Object for parrentId");
		goto FINALIZE;
	}

	result = (OV_UINT)checkForSameAAS(Ov_PtrUpCast(ov_object, pobj), pParent);
	if (result != OV_ERR_OK){
		ov_string_setvalue(&status, "Parent is not in the same AAS as the method");
		goto FINALIZE;
	}

	ov_string_setvalue(&parentId.IdSpec, *(OV_STRING*)(packedInputArgList[0]));
	parentId.IdType = *(OV_UINT*)(packedInputArgList[1]);

	ov_string_setvalue(&name, *(OV_STRING*)(packedInputArgList[2]));

	mask = *(OV_UINT*)(packedInputArgList[3]);

	if (mask & 0x01){
		ov_string_setvalue(&carrierId.IdSpec, *(OV_STRING*)(packedInputArgList[4]));
		carrierId.IdType = *(OV_UINT*)(packedInputArgList[5]);
	}

	if (mask & 0x02){
		expressionLogic = *(OV_UINT*)(packedInputArgList[6]);
	}

	if (mask & 0x04){
		expressionSemantic = *(OV_UINT*)(packedInputArgList[7]);
	}

	if (mask & 0x08){
		ov_string_setvalue(&propertyId.IdSpec, *(OV_STRING*)(packedInputArgList[8]));
		propertyId.IdType = *(OV_UINT*)(packedInputArgList[9]);
	}

	if (mask & 0x10){
		view = *(OV_UINT*)(packedInputArgList[10]);
	}

	if (mask & 0x20){
		visibility = *(OV_UINT*)(packedInputArgList[11]);
	}

    result = propertyValueStatement_modelmanager_createPVSL(parentId, name, mask, carrierId, expressionLogic, expressionSemantic, propertyId, view, visibility);
    ov_string_setvalue(&status, ov_result_getresulttext(result));

    FINALIZE:

    *(OV_STRING*)packedOutputArgList[0] = ov_database_malloc(ov_string_getlength(status)+1);
	strcpy(*(OV_STRING*)packedOutputArgList[0], status);

	IdentificationType_deleteMembers(&parentId);
	ov_string_setvalue(&name, NULL);
	IdentificationType_deleteMembers(&carrierId);
	IdentificationType_deleteMembers(&propertyId);

    return OV_ERR_OK;
}


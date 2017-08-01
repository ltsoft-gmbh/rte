
/******************************************************************************
*
*   FILE
*   ----
*   CarrierId.c
*
*   History
*   -------
*   2017-08-01   File created
*
*******************************************************************************
*
*   This file is generated by the 'acplt_builder' command
*
******************************************************************************/


#ifndef OV_COMPILE_LIBRARY_propertyValueStatement
#define OV_COMPILE_LIBRARY_propertyValueStatement
#endif


#include "propertyValueStatement.h"
#include "libov/ov_macros.h"


OV_DLLFNCEXPORT OV_ACCESS propertyValueStatement_CarrierId_getaccess(
	OV_INSTPTR_ov_object	pobj,
	const OV_ELEMENT		*pelem,
	const OV_TICKET			*pticket
) {
    /*    
    *   local variables
    */
    OV_INSTPTR_propertyValueStatement_CarrierId pinst = Ov_StaticPtrCast(propertyValueStatement_CarrierId, pobj);

    return (OV_ACCESS)0;
}

OV_DLLFNCEXPORT OV_RESULT propertyValueStatement_CarrierId_constructor(
	OV_INSTPTR_ov_object 	pobj
) {
    /*    
    *   local variables
    */
    OV_INSTPTR_propertyValueStatement_CarrierId pinst = Ov_StaticPtrCast(propertyValueStatement_CarrierId, pobj);
    OV_RESULT    result;

    /* do what the base class does first */
    result = ov_object_constructor(pobj);
    if(Ov_Fail(result))
         return result;

    /* do what */


    return OV_ERR_OK;
}


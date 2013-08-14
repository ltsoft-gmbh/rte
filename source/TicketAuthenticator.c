
/******************************************************************************
*
*   FILE
*   ----
*   TicketAuthenticator.c
*
*   History
*   -------
*   2013-01-31   File created
*
*******************************************************************************
*
*   This file is generated by the 'acplt_builder' command
*
******************************************************************************/


#ifndef OV_COMPILE_LIBRARY_ksbase
#define OV_COMPILE_LIBRARY_ksbase
#endif


#include "ksbase.h"
#include "libov/ov_macros.h"


OV_DLLFNCEXPORT OV_RESULT ksbase_TicketAuthenticator_constructor(
	OV_INSTPTR_ov_object 	pobj
) {
    /*    
    *   local variables
    */
    OV_INSTPTR_ov_class	pclass = NULL;

    OV_RESULT    result;

    /* do what the base class does first */
    result = ov_object_constructor(pobj);
    if(Ov_Fail(result))
         return result;

    /* do what */

    /*	check if there is already an authenticator of the same type     */
    pclass = Ov_GetParent(ov_instantiation, pobj);
    if(Ov_GetFirstChild(ov_instantiation, pclass) != pobj)
    	return OV_ERR_ALREADYEXISTS;
    if(Ov_GetNextChild(ov_instantiation, pobj))
    	return OV_ERR_ALREADYEXISTS;
    if(Ov_GetPreviousChild(ov_instantiation, pobj))
    	return OV_ERR_ALREADYEXISTS;



    return OV_ERR_OK;
}

/**
 * This method is called on startup.
 * It allocates memory for the ticket vtable and set the functions pointers to the object's functions
 */
OV_DLLFNCEXPORT void ksbase_TicketAuthenticator_startup(
		OV_INSTPTR_ov_object 	pobj
) {
	OV_INSTPTR_ksbase_TicketAuthenticator this = Ov_StaticPtrCast(ksbase_TicketAuthenticator, pobj);
	OV_VTBLPTR_ksbase_TicketAuthenticator pVTBLTicketAuthenticator = NULL;

	ov_object_startup(pobj);

	/*	link authenticator functions to ticketvtable	*/
	Ov_GetVTablePtr(ksbase_TicketAuthenticator, pVTBLTicketAuthenticator, this);
	if(!pVTBLTicketAuthenticator)
		return;
	this->v_ticket.vtbl = ov_malloc(sizeof(OV_TICKET_VTBL));
	if(!this->v_ticket.vtbl)
		return;

	this->v_ticket.vtbl->createticket = pVTBLTicketAuthenticator->m_createticket;
	this->v_ticket.vtbl->deleteticket = pVTBLTicketAuthenticator->m_deleteticket;
	this->v_ticket.vtbl->encodereply = pVTBLTicketAuthenticator->m_encodereply;
	this->v_ticket.vtbl->getaccess = pVTBLTicketAuthenticator->m_TicketGetaccess;

	this->v_ticket.type = this->v_TicketType;
	return;
}

/**
 * Procedure called on object shutdown.
 * It frees the ticket's vtable pointer
 */
OV_DLLFNCEXPORT void ksbase_TicketAuthenticator_shutdown(
		OV_INSTPTR_ov_object 	pobj
) {
	OV_INSTPTR_ksbase_TicketAuthenticator this = Ov_StaticPtrCast(ksbase_TicketAuthenticator, pobj);

	if(pdb->started){
		//prevent a crash if the database did not shutdown clean and the pointer points to foreign data
		ov_free(this->v_ticket.vtbl);
	}
	this->v_ticket.vtbl = NULL;

	ov_object_shutdown(pobj);
	return;
}


OV_DLLFNCEXPORT OV_TICKET *ksbase_TicketAuthenticator_createticket(
	void *data,
	OV_TICKET_TYPE type
) {
    return NULL;
}

OV_DLLFNCEXPORT void ksbase_TicketAuthenticator_deleteticket(
	OV_TICKET *pticket
) {
    return;
}

OV_DLLFNCEXPORT OV_BOOL ksbase_TicketAuthenticator_encodereply(
	void *data,
	OV_TICKET *pticket
) {
    return FALSE;
}

OV_DLLFNCEXPORT OV_ACCESS ksbase_TicketAuthenticator_TicketGetaccess(
	const OV_TICKET *pticket
) {
    return OV_AC_NONE;
}


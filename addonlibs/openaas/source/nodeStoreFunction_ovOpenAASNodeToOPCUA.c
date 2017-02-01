/******************************************************************************
 *
 *   FILE
 *   ----
 *   nodeStoreFunctions.c
 *
 *   History
 *   -------
 *   2014-10-21   File created
 *
 *******************************************************************************
 *
 *   This file is generated by the 'acplt_builder' command
 *
 ******************************************************************************/

#ifndef OV_COMPILE_LIBRARY_openaas
#define OV_COMPILE_LIBRARY_openaas
#endif

#include "openaas.h"
#include "libov/ov_macros.h"
#include "ksbase.h"
#include "opcua.h"
#include "opcua_helpers.h"
#include "NoneTicketAuthenticator.h"
#include "libov/ov_path.h"
#include "libov/ov_memstack.h"
#include "ks_logfile.h"
#include "nodeset.h"

extern OV_INSTPTR_openaas_nodeStoreFunctions pNodeStoreFunctions;



OV_DLLFNCEXPORT UA_StatusCode openaas_nodeStoreFunctions_ovOpenAASNodeToOPCUA(
		void *handle, const UA_NodeId *nodeId, UA_Node** opcuaNode) {
	UA_Node 				*newNode = NULL;
	UA_StatusCode 			result = UA_STATUSCODE_GOOD;
	OV_PATH 				path;
	OV_INSTPTR_ov_object	pobj = NULL;
	OV_TICKET 				*pTicket = NULL;
	OV_VTBLPTR_ov_object	pVtblObj = NULL;
	OV_ACCESS				access;
	UA_NodeClass 			*nodeClass = NULL;

	ov_memstack_lock();
	result = opcua_nodeStoreFunctions_resolveNodeIdToPath(*nodeId, &path);
	if(result != UA_STATUSCODE_GOOD){
		ov_memstack_unlock();
		return result;
	}
	result = opcua_nodeStoreFunctions_getVtblPointerAndCheckAccess(&(path.elements[path.size-1]), pTicket, &pobj, &pVtblObj, &access);
	if(result != UA_STATUSCODE_GOOD){
		ov_memstack_unlock();
		return result;
	}

	nodeClass = UA_NodeClass_new();
	if(!nodeClass){
		result = ov_resultToUaStatusCode(OV_ERR_HEAPOUTOFMEMORY);
		ov_memstack_unlock();
		return result;
	}

	*nodeClass = UA_NODECLASS_OBJECT;
	newNode = (UA_Node*)ov_database_malloc(sizeof(UA_ObjectNode));


	// Basic Attribute
	// BrowseName
	UA_QualifiedName qName;
	qName.name = UA_String_fromChars(pobj->v_identifier);
	if(Ov_GetClassPtr(pobj) != pclass_opcua_arguments){
		qName.namespaceIndex = pNodeStoreFunctions->v_NameSpaceIndexNodeStoreInterface;
	} else {
		qName.namespaceIndex = 0;
	}
	newNode->browseName = qName;

	// Description
	OV_STRING tempString = pVtblObj->m_getcomment(pobj, &(path.elements[path.size-1]));
	UA_LocalizedText lText;
	lText.locale = UA_String_fromChars("en");
	if(tempString){
		lText.text = UA_String_fromChars(tempString);
	} else {
		lText.text = UA_String_fromChars("");
	}
	newNode->description = lText;

	// DisplayName
	lText.locale = UA_String_fromChars("en");
	lText.text = UA_String_fromChars(pobj->v_identifier);
	newNode->displayName = lText;

	// NodeId
	newNode->nodeId = *nodeId;

	// NodeClass
	newNode->nodeClass 	= *nodeClass;

	// WriteMask
	UA_UInt32 writeMask = 0;
	if(path.elements[path.size-1].elemtype != OV_ET_VARIABLE){
		if(access & OV_AC_WRITE){
			writeMask |= (1<<2);	//	BrowseName
			writeMask |= (1<<6);	//	DisplayName
		}
		if(access & OV_AC_RENAMEABLE){
			writeMask |= (1<<14);	//	NodeId
		}
	}
	newNode->writeMask 	= writeMask;


	((UA_ObjectNode*)newNode)->eventNotifier = 0;

	// References
	size_t size_references = 4; //For Header & Body & LifeCycleArchive & Views


	OV_INSTPTR_ov_object pchild = NULL;
	Ov_ForEachChild(ov_containment, Ov_DynamicPtrCast(ov_domain,pobj), pchild) {
			size_references++;
		}

	size_references = size_references + 2;// For Parent&TypeNode
	newNode->references = UA_calloc(size_references, sizeof(UA_ReferenceNode));
	newNode->referencesSize = size_references;

	// ParentNode
	newNode->references[0].referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
	newNode->references[0].isInverse = UA_TRUE;
	OV_UINT len = 0;
	OV_STRING *plist = NULL;
	OV_STRING tmpString = NULL;
	copyOPCUAStringToOV(nodeId->identifier.string, &tmpString);
	plist = ov_string_split(tmpString, "/", &len);
	ov_string_setvalue(&tmpString, "");
	for (OV_UINT i = 0; i < len-1; i++){
		if (i != 0)
			ov_string_append(&tmpString, "/");
		ov_string_append(&tmpString, plist[i]);
	}
	newNode->references[0].targetId = UA_EXPANDEDNODEID_STRING(pNodeStoreFunctions->v_NameSpaceIndexNodeStoreInterface, tmpString);
	// TypeNode
	newNode->references[1].referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASTYPEDEFINITION);
	newNode->references[1].isInverse = UA_FALSE;
	newNode->references[1].targetId = UA_EXPANDEDNODEID_NUMERIC(pNodeStoreFunctions->v_NameSpaceIndexInformationModel, UA_NS2ID_ASSETADMINISTRATIONSHELLTYPE);

	// HeaderNode
	newNode->references[2].referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
	newNode->references[2].isInverse = UA_FALSE;
	OV_INSTPTR_openaas_aas ptr = Ov_DynamicPtrCast(openaas_aas,pobj);
	tmpString = NULL;
	copyOPCUAStringToOV(nodeId->identifier.string, &tmpString);
	ov_string_append(&tmpString, ".");
	ov_string_append(&tmpString, ptr->p_Header.v_identifier);
	newNode->references[2].targetId = UA_EXPANDEDNODEID_STRING_ALLOC(pNodeStoreFunctions->v_NameSpaceIndexNodeStoreInterface, tmpString);

	// BodyNode
	newNode->references[3].referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
	newNode->references[3].isInverse = UA_FALSE;
	tmpString = NULL;
	copyOPCUAStringToOV(nodeId->identifier.string, &tmpString);
	ov_string_append(&tmpString, ".");
	ov_string_append(&tmpString, ptr->p_Body.v_identifier);
	newNode->references[3].targetId = UA_EXPANDEDNODEID_STRING_ALLOC(pNodeStoreFunctions->v_NameSpaceIndexNodeStoreInterface, tmpString);

	// LifeCycleArchivNode
	newNode->references[4].referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
	newNode->references[4].isInverse = UA_FALSE;
	tmpString = NULL;
	copyOPCUAStringToOV(nodeId->identifier.string, &tmpString);
	ov_string_append(&tmpString, ".");
	ov_string_append(&tmpString, ptr->p_LifeCycleArchive.v_identifier);
	newNode->references[4].targetId = UA_EXPANDEDNODEID_STRING_ALLOC(pNodeStoreFunctions->v_NameSpaceIndexNodeStoreInterface, tmpString);

	// ViewsNode
	newNode->references[5].referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
	newNode->references[5].isInverse = UA_FALSE;
	tmpString = NULL;
	copyOPCUAStringToOV(nodeId->identifier.string, &tmpString);
	ov_string_append(&tmpString, ".");
	ov_string_append(&tmpString, ptr->p_Views.v_identifier);
	newNode->references[5].targetId = UA_EXPANDEDNODEID_STRING_ALLOC(pNodeStoreFunctions->v_NameSpaceIndexNodeStoreInterface, tmpString);

	size_t i = 5;
	Ov_ForEachChild(ov_containment, Ov_DynamicPtrCast(ov_domain,pobj), pchild) {
		i++;
		if (Ov_CanCastTo(openaas_aas, pchild)){
			newNode->references[i].referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
			newNode->references[i].isInverse = UA_FALSE;
			OV_INSTPTR_openaas_aas pref =
									Ov_DynamicPtrCast(openaas_aas,pchild);
			OV_STRING tmpString = NULL;
			copyOPCUAStringToOV(nodeId->identifier.string, &tmpString);
			ov_string_append(&tmpString, "/");
			ov_string_append(&tmpString, pref->v_identifier);
			newNode->references[i].targetId = UA_EXPANDEDNODEID_STRING_ALLOC(pNodeStoreFunctions->v_NameSpaceIndexNodeStoreInterface, tmpString);
		}
	}
	*opcuaNode = newNode;
	ov_memstack_unlock();
	return UA_STATUSCODE_GOOD;
}


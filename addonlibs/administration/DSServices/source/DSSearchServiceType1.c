
/******************************************************************************
*
*   FILE
*   ----
*   DSSearchServiceType1.c
*
*   History
*   -------
*   2018-05-14   File created
*
*******************************************************************************
*
*   This file is generated by the 'acplt_builder' command
*
******************************************************************************/


#ifndef OV_COMPILE_LIBRARY_DSServices
#define OV_COMPILE_LIBRARY_DSServices
#endif


#include "DSServices.h"
#include "libov/ov_macros.h"
#include "service_helper.h"

struct searchtag{
	OV_STRING tag;
	OV_STRING value;
};

struct searchStatement{
	OV_STRING carrierID;
	OV_STRING propertyID;
	OV_STRING expressionSemantic;
	OV_STRING relation;
	OV_STRING value;
	OV_STRING valueType;
	OV_STRING submodel;
};

struct endpoint{
	OV_STRING protocolType;
	OV_STRING endpointString;
};

struct component{
	OV_STRING componentID;
	struct endpoint* endpoints;
	OV_UINT	  endpointsSize;
};

OV_DLLFNCEXPORT OV_RESULT DSServices_DSSearchServiceType1_executeService(OV_INSTPTR_openAASDiscoveryServer_DSService pinst, const json_data JsonInput, OV_STRING *JsonOutput, OV_STRING *errorMessage) {
    /*    
    *   local variables
    */
	// Parsing Body
	OV_STRING_VEC componentIDs;
	OV_STRING_VEC* resultStatements = NULL;
	componentIDs.value = NULL;
	componentIDs.veclen = 0;
	OV_STRING_VEC endpointStruct;
	endpointStruct.value = NULL;
	endpointStruct.veclen = 0;
	struct component *components = NULL;
	struct searchtag *searchtags = NULL;
	OV_UINT searchtagsSize = 0;
	OV_STRING_VEC tags;
	tags.value = NULL;
	tags.veclen = 0;
	Ov_SetDynamicVectorLength(&tags, 3, STRING);
	ov_string_setvalue(&tags.value[0], "componentID");
	ov_string_setvalue(&tags.value[1], "securityKey");
	//ov_string_setvalue(&tags.value[2], "tags");
	ov_string_setvalue(&tags.value[2], "statements");
	struct searchStatement *searchStatements = NULL;
	OV_UINT searchStatementSize = 0;
	OV_UINT_VEC tokenIndex;
	tokenIndex.value = NULL;
	tokenIndex.veclen = 0;
	Ov_SetDynamicVectorLength(&tokenIndex, 3, UINT);

	jsonGetTokenIndexByTags(tags, JsonInput, 1, &tokenIndex);

	OV_STRING componentID = NULL;
	jsonGetValueByToken(JsonInput.js, &JsonInput.token[tokenIndex.value[0]+1], &componentID);
	OV_STRING securityKey = NULL;
	jsonGetValueByToken(JsonInput.js, &JsonInput.token[tokenIndex.value[1]+1], &securityKey);

	// check SecurityKey in Database
	OV_RESULT resultOV = checkSecurityKey(pinst->v_DBWrapperUsed, componentID, securityKey);
	if (resultOV){
		ov_string_setvalue(errorMessage, "SecurityKey is not correct");
		goto FINALIZE;
	}

//	// get tags
//	searchtagsSize = JsonInput.token[tokenIndex.value[2]+1].size;
//	searchtags = malloc(sizeof(struct searchtag)*searchtagsSize);
//	if (!searchtags){
//		ov_string_setvalue(errorMessage, "Fault in malloc");
//		goto FINALIZE;
//	}
//	for (OV_UINT i = 0; i < searchtagsSize; i++){
//		searchtags[i].tag = NULL;
//		// value + 2 start of objects + i*5 next object + 2/4 values of tag and value
//		jsonGetValueByToken(JsonInput.js, &JsonInput.token[tokenIndex.value[2]+2+i*5+2], &searchtags[i].tag);
//		searchtags[i].value = NULL;
//		jsonGetValueByToken(JsonInput.js, &JsonInput.token[tokenIndex.value[2]+2+i*5+4], &searchtags[i].value);
//	}
//
//	// search for tags in Database and get componentID and endpoints
//	struct DB_QUERY * query = NULL;
//	query = malloc(sizeof(struct DB_QUERY) * searchtagsSize);
//
//	for (OV_UINT i = 0; i < searchtagsSize; i++){
//		query[i].column.veclen = 0;
//		query[i].column.value = NULL;
//		query[i].value.veclen = 0;
//		query[i].value.value = NULL;
//
//		Ov_SetDynamicVectorLength(&query[i].column, query[i].column.veclen + 2, STRING);
//		ov_string_setvalue(&query[i].column.value[query[i].column.veclen-2], "Tag");
//		ov_string_setvalue(&query[i].column.value[query[i].column.veclen-1], "Value");
//		Ov_SetDynamicVectorLength(&query[i].value, query[i].value.veclen + 2, STRING);
//		ov_string_setvalue(&query[i].value.value[query[i].value.veclen-2], searchtags[i].tag);
//		ov_string_setvalue(&query[i].value.value[query[i].value.veclen-1], searchtags[i].value);
//	}
//
//	OV_STRING table  = "tags";
//	// Find ComponentID by Tags
//	OV_INSTPTR_openAASDiscoveryServer_DBWrapper pDBWrapper = NULL;
//	OV_VTBLPTR_openAASDiscoveryServer_DBWrapper pDBWrapperVTable = NULL;
//	for (OV_UINT i = 0; i < pinst->v_DBWrapperUsed.veclen; i++){
//		pDBWrapper = Ov_DynamicPtrCast(openAASDiscoveryServer_DBWrapper, ov_path_getobjectpointer(pinst->v_DBWrapperUsed.value[i], 2));
//		if (!pDBWrapper)
//			break;
//
//		Ov_GetVTablePtr(openAASDiscoveryServer_DBWrapper,pDBWrapperVTable, pDBWrapper);
//		pDBWrapperVTable->m_getComponentID(table, query, searchtagsSize, &componentIDs);
//		if (componentIDs.veclen > 0){
//			break;
//		}
//	}
//
//	for (OV_UINT i = 0; i < searchtagsSize; i++){
//		Ov_SetDynamicVectorLength(&query[i].column, 0, STRING);
//		Ov_SetDynamicVectorLength(&query[i].value, 0, STRING);
//	}
//	free(query);

	// get statements from JSON
	searchStatementSize = JsonInput.token[tokenIndex.value[2]+1].size;
	searchStatements = malloc(sizeof(struct searchStatement)*searchStatementSize);
	for (OV_UINT i = 0; i < searchStatementSize; i++){
		// value + 2 start of objects + i*15 next object + 2 values of carrierID
		searchStatements[i].carrierID = NULL;
		jsonGetValueByToken(JsonInput.js, &JsonInput.token[tokenIndex.value[2]+2+i*15+2], &searchStatements[i].carrierID);
		// value + 2 start of objects + i*15 next object + 4 values of propertyID
		searchStatements[i].propertyID = NULL;
		jsonGetValueByToken(JsonInput.js, &JsonInput.token[tokenIndex.value[2]+2+i*15+4], &searchStatements[i].propertyID);
		// value + 2 start of objects + i*15 next object + 6 values of expressionSemantic
		searchStatements[i].expressionSemantic = NULL;
		jsonGetValueByToken(JsonInput.js, &JsonInput.token[tokenIndex.value[2]+2+i*15+6], &searchStatements[i].expressionSemantic);
		// value + 2 start of objects + i*15 next object + 8 values of relation
		searchStatements[i].relation = NULL;
		jsonGetValueByToken(JsonInput.js, &JsonInput.token[tokenIndex.value[2]+2+i*15+8], &searchStatements[i].relation);
		// value + 2 start of objects + i*15 next object + 10 values of value
		searchStatements[i].value = NULL;
		jsonGetValueByToken(JsonInput.js, &JsonInput.token[tokenIndex.value[2]+2+i*15+10], &searchStatements[i].value);
		// value + 2 start of objects + i*15 next object + 12 values of valuetype
		searchStatements[i].valueType = NULL;
		// ['','Integer','Real','Text']
		jsonGetValueByToken(JsonInput.js, &JsonInput.token[tokenIndex.value[2]+2+i*15+12], &searchStatements[i].valueType);
		// value + 2 start of objects + i*15 next object + 14 values of submodel
		searchStatements[i].submodel = NULL;
		jsonGetValueByToken(JsonInput.js, &JsonInput.token[tokenIndex.value[2]+2+i*15+14], &searchStatements[i].submodel);
	}


	struct DB_QUERY * query = NULL;
	query = malloc(sizeof(struct DB_QUERY) * searchStatementSize);
	OV_STRING_VEC* table  = NULL;
	table = malloc(sizeof(OV_STRING_VEC) * searchStatementSize);
//	OV_UINT queryNumber = 0;
//	for (OV_UINT i = 0; i < searchStatementSize; i++){
//		if (searchStatements[i].valueType == NULL || ov_string_compare(searchStatements[i].valueType, "") == OV_STRCMP_EQUAL){
//			query[queryNumber].column.veclen = 0;
//			query[queryNumber].column.value = NULL;
//			query[queryNumber].value.veclen = 0;
//			query[queryNumber].value.value = NULL;
//			Ov_SetDynamicVectorLength(&query[i].column, query[queryNumber].column.veclen + 7, STRING);
//			ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-6], "CarrierID");
//			ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-5], "PropertyID");
//			ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-4], "ExpressionSemantic");
//			ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-3], "Relation");
//			ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-2], "Value");
//			ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-1], "SubModel");
//			Ov_SetDynamicVectorLength(&query[queryNumber].value, query[queryNumber].value.veclen + 7, STRING);
//			ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-6], searchStatements[i].carrierID);
//			ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-5], searchStatements[i].propertyID);
//			ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-4], searchStatements[i].expressionSemantic);
//			ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-3], searchStatements[i].relation);
//			ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-2], searchStatements[i].value);
//			ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-1], searchStatements[i].submodel);
//			queryNumber++;
//			// Only EQUAL allowed
//			continue;
//		}
//		if (ov_string_compare(searchStatements[i].valueType, "Integer") == OV_STRCMP_EQUAL || ov_string_compare(searchStatements[i].valueType, "Real") == OV_STRCMP_EQUAL){
//			if (ov_string_compare(searchStatements[i].relation, "<"){
//			for (OV_UINT i = 0; i < 6; i++){
//				query[queryNumber].column.veclen = 0;
//				query[queryNumber].column.value = NULL;
//				query[queryNumber].value.veclen = 0;
//				query[queryNumber].value.value = NULL;
//				Ov_SetDynamicVectorLength(&query[i].column, query[queryNumber].column.veclen + 7, STRING);
//				ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-6], "CarrierID");
//				ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-5], "PropertyID");
//				ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-4], "ExpressionSemantic");
//				ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-3], "Relation");
//				ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-2], "Value");
//				ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-1], "SubModel");
//				Ov_SetDynamicVectorLength(&query[queryNumber].value, query[queryNumber].value.veclen + 7, STRING);
//				ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-6], searchStatements[i].carrierID);
//				ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-5], searchStatements[i].propertyID);
//				ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-4], searchStatements[i].expressionSemantic);
//				ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-1], searchStatements[i].submodel);
//				switch (i){
//					case 0: // <
//						ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-3], "<");
//						ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-2], searchStatements[i].value);
//						break;
//					case 1: // <=
//						ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-3], "<");
//						ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-2], searchStatements[i].value);
//						break;
//					case 2: // ==
//						ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-3], "<");
//						ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-2], searchStatements[i].value);
//						break;
//					case 3: // !=
//						ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-3], "<");
//						ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-2], searchStatements[i].value);
//						break;
//					case 4: // >
//						ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-3], "<");
//						ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-2], searchStatements[i].value);
//						break;
//					case 5: // >=
//						ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-3], "<");
//						ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-2], searchStatements[i].value);
//						break;
//				}
//				queryNumber++;
//			}
//
//				Ov_SetDynamicVectorLength(&query[i].column, query[queryNumber].column.veclen + 7, STRING);
//				ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-6], "CarrierID");
//				ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-5], "PropertyID");
//				ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-4], "ExpressionSemantic");
//				ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-3], "Relation");
//				ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-2], "Value");
//				ov_string_setvalue(&query[queryNumber].column.value[query[queryNumber].column.veclen-1], "SubModel");
//				Ov_SetDynamicVectorLength(&query[queryNumber].value, query[queryNumber].value.veclen + 7, STRING);
//				ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-6], searchStatements[i].carrierID);
//				ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-5], searchStatements[i].propertyID);
//				ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-4], searchStatements[i].expressionSemantic);
//				ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-3], searchStatements[i].relation);
//				ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-2], searchStatements[i].value);
//				ov_string_setvalue(&query[queryNumber].value.value[query[queryNumber].value.veclen-1], searchStatements[i].submodel);
//			}
//			queryCount = queryCount + 6;
//			// GREATER THAN, GREATER, EQUAL, NOTEQUAL, LESS, LESS THAN allowed
//			continue;
//		}
//	}


	for (OV_UINT i = 0; i < searchStatementSize; i++){
		table[i].value = NULL;
		table[i].veclen = 0;
		Ov_SetDynamicVectorLength(&table[i], 1, STRING);
		ov_string_setvalue(&table[i].value[0], "statements_TextBoolean");
		query[i].column.veclen = 0;
		query[i].column.value = NULL;
		query[i].value.veclen = 0;
		query[i].value.value = NULL;

		Ov_SetDynamicVectorLength(&query[i].column, query[i].column.veclen + 6, STRING);
		ov_string_setvalue(&query[i].column.value[query[i].column.veclen-6], "CarrierID");
		ov_string_setvalue(&query[i].column.value[query[i].column.veclen-5], "PropertyID");
		ov_string_setvalue(&query[i].column.value[query[i].column.veclen-4], "ExpressionSemantic");
		ov_string_setvalue(&query[i].column.value[query[i].column.veclen-3], "Relation");
		ov_string_setvalue(&query[i].column.value[query[i].column.veclen-2], "Value");
		ov_string_setvalue(&query[i].column.value[query[i].column.veclen-1], "SubModel");
		Ov_SetDynamicVectorLength(&query[i].value, query[i].value.veclen + 6, STRING);
		ov_string_setvalue(&query[i].value.value[query[i].value.veclen-6], searchStatements[i].carrierID);
		ov_string_setvalue(&query[i].value.value[query[i].value.veclen-5], searchStatements[i].propertyID);
		ov_string_setvalue(&query[i].value.value[query[i].value.veclen-4], searchStatements[i].expressionSemantic);
		ov_string_setvalue(&query[i].value.value[query[i].value.veclen-3], searchStatements[i].relation);
		ov_string_setvalue(&query[i].value.value[query[i].value.veclen-2], searchStatements[i].value);
		ov_string_setvalue(&query[i].value.value[query[i].value.veclen-1], searchStatements[i].submodel);
	}

	// Find ComponentID by Tags
	OV_INSTPTR_openAASDiscoveryServer_DBWrapper pDBWrapper = NULL;
	OV_VTBLPTR_openAASDiscoveryServer_DBWrapper pDBWrapperVTable = NULL;
	for (OV_UINT i = 0; i < pinst->v_DBWrapperUsed.veclen; i++){
		pDBWrapper = Ov_DynamicPtrCast(openAASDiscoveryServer_DBWrapper, ov_path_getobjectpointer(pinst->v_DBWrapperUsed.value[i], 2));
		if (!pDBWrapper)
			break;

		Ov_GetVTablePtr(openAASDiscoveryServer_DBWrapper,pDBWrapperVTable, pDBWrapper);
		pDBWrapperVTable->m_getComponentID(table, query, searchStatementSize, &componentIDs);

		if (componentIDs.veclen > 0){
			resultStatements = malloc(sizeof(OV_STRING_VEC)*componentIDs.veclen);
			for (OV_UINT j = 0; j < componentIDs.veclen; j++){
				resultStatements[j].value = NULL;
				resultStatements[j].veclen = 0;
				pDBWrapperVTable->m_getFittingStatements(table, componentIDs.value[j], query, searchStatementSize, &resultStatements[j]);
			}
			break;
		}
	}

	for (OV_UINT i = 0; i < searchStatementSize; i++){
		Ov_SetDynamicVectorLength(&query[i].column, 0, STRING);
		Ov_SetDynamicVectorLength(&query[i].value, 0, STRING);
		Ov_SetDynamicVectorLength(&table[i], 0, STRING);
	}
	free(query);
	free(table);

	if (componentIDs.veclen == 0){
		ov_string_setvalue(errorMessage, "no component found by your statements");
		ov_string_setvalue(JsonOutput, "\"body\":{}");
		goto FINALIZE;
	}

	components = malloc(sizeof(struct component)*componentIDs.veclen);
	Ov_SetDynamicVectorLength(&endpointStruct, 0, STRING);
	OV_STRING tmpFields[2] = {"ProtocolType","EndpointString"};
	OV_STRING whereFields = NULL;
	OV_STRING tmpValues = NULL;
	for (OV_UINT i = 0; i < componentIDs.veclen; i++){
		components[i].componentID = NULL;
		ov_string_setvalue(&components[i].componentID, componentIDs.value[i]);
		components[i].endpoints = NULL;
		components[i].endpointsSize = 0;
		// Find ComponentID by Tags
		whereFields = NULL;
		ov_string_setvalue(&whereFields,"ComponentID");
		tmpValues = NULL;
		ov_string_print(&tmpValues, "'%s'", components[i].componentID);
		OV_INSTPTR_openAASDiscoveryServer_DBWrapper pDBWrapper = NULL;
		OV_VTBLPTR_openAASDiscoveryServer_DBWrapper pDBWrapperVTable = NULL;
		for (OV_UINT j = 0; j < pinst->v_DBWrapperUsed.veclen; j++){
			pDBWrapper = Ov_DynamicPtrCast(openAASDiscoveryServer_DBWrapper, ov_path_getobjectpointer(pinst->v_DBWrapperUsed.value[j], 2));
			if (!pDBWrapper)
				break;

			Ov_GetVTablePtr(openAASDiscoveryServer_DBWrapper,pDBWrapperVTable, pDBWrapper);
			pDBWrapperVTable->m_selectData("Endpoints", tmpFields, 2, &whereFields, 1, &tmpValues, 1, &endpointStruct);
			if (endpointStruct.veclen > 0){
				components[i].endpointsSize = endpointStruct.veclen / 2;
				components[i].endpoints = malloc(sizeof(struct endpoint)*endpointStruct.veclen / 2);
				for (OV_UINT k = 0; k < endpointStruct.veclen/2; k++){
					components[i].endpoints[k].protocolType = NULL;
					ov_string_setvalue(&components[i].endpoints[k].protocolType, endpointStruct.value[k*2]);
					components[i].endpoints[k].endpointString = NULL;
					ov_string_setvalue(&components[i].endpoints[k].endpointString, endpointStruct.value[k*2+1]);
				}
				Ov_SetDynamicVectorLength(&endpointStruct, 0, STRING);
				break;
			}
		}
		ov_string_setvalue(&tmpValues, NULL);
		ov_string_setvalue(&whereFields, NULL);
	}

	// Generate Response
	ov_string_setvalue(JsonOutput, "\"body\":{\"components\":[");
	for (OV_UINT i = 0; i < componentIDs.veclen; i++){
		OV_STRING tmpString = NULL;
		ov_string_print(&tmpString, "{\"componentID\":\"%s\", \"endpoints\":[", components[i].componentID);
		ov_string_append(JsonOutput, tmpString);
		ov_string_setvalue(&tmpString, NULL);
		for (OV_UINT j = 0; j < components[i].endpointsSize; j++){
			tmpString = NULL;
			ov_string_print(&tmpString, "{\"protocolType\":\"%s\", \"endpointString\":\"%s\"}", components[i].endpoints[j].protocolType, components[i].endpoints[j].endpointString);
			ov_string_append(JsonOutput, tmpString);
			ov_string_setvalue(&tmpString, NULL);
			if (j < components[i].endpointsSize - 1)
				ov_string_append(JsonOutput, ",");
		}
		ov_string_append(JsonOutput, "]");
		ov_string_append(JsonOutput, ",\"statements\":[");
		ov_string_setvalue(&tmpString, NULL);
		for (OV_UINT j = 0; j < resultStatements[i].veclen; j++){
			tmpString = NULL;
			ov_string_print(&tmpString, "{%s}", resultStatements[i].value[j]);
			ov_string_append(JsonOutput, tmpString);
			ov_string_setvalue(&tmpString, NULL);
			if (j < resultStatements[i].veclen - 1)
				ov_string_append(JsonOutput, ",");
		}
		ov_string_append(JsonOutput, "]}");
		if (i < componentIDs.veclen - 1)
			ov_string_append(JsonOutput, ",");
	}
	ov_string_append(JsonOutput, "]}");

	FINALIZE:
	Ov_SetDynamicVectorLength(&tokenIndex, 0, UINT);
	Ov_SetDynamicVectorLength(&endpointStruct, 0, STRING);
	ov_string_setvalue(&componentID, NULL);
	ov_string_setvalue(&securityKey, NULL);

	for (OV_UINT i = 0; i < componentIDs.veclen; i++){
		Ov_SetDynamicVectorLength(&resultStatements[i], 0, STRING);
	}
	if (resultStatements)
		free(resultStatements);

	for (OV_UINT i = 0; i < componentIDs.veclen; i++){
		ov_string_setvalue(&components[i].componentID, NULL);
		for (OV_UINT j = 0; j < components[i].endpointsSize; j++){
			ov_string_setvalue(&components[i].endpoints[j].protocolType, NULL);
			ov_string_setvalue(&components[i].endpoints[j].endpointString, NULL);
		}
		free(components[i].endpoints);
		components[i].endpoints = NULL;
	}
	if (components)
		free(components);
	Ov_SetDynamicVectorLength(&componentIDs, 0, STRING);

	for (OV_UINT i = 0; i < searchtagsSize; i++){
		ov_string_setvalue(&searchtags[i].tag, NULL);
		ov_string_setvalue(&searchtags[i].value, NULL);
	}
	if (searchtags)
		free(searchtags);

	for (OV_UINT i = 0; i < searchStatementSize; i++){
		ov_string_setvalue(&searchStatements[i].carrierID, NULL);
		ov_string_setvalue(&searchStatements[i].propertyID, NULL);
		ov_string_setvalue(&searchStatements[i].expressionSemantic, NULL);
		ov_string_setvalue(&searchStatements[i].relation, NULL);
		ov_string_setvalue(&searchStatements[i].value, NULL);
		ov_string_setvalue(&searchStatements[i].submodel, NULL);
	}
	if (searchStatements)
		free(searchStatements);
	return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_ACCESS DSServices_DSSearchServiceType1_getaccess(
	OV_INSTPTR_ov_object	pobj,
	const OV_ELEMENT		*pelem,
	const OV_TICKET			*pticket
) {
    /*    
    *   local variables
    */

	switch(pelem->elemtype) {
		case OV_ET_VARIABLE:
			if(pelem->elemunion.pvar->v_offset >= offsetof(OV_INST_ov_object,__classinfo)) {
				if(pelem->elemunion.pvar->v_vartype == OV_VT_CTYPE)
					return OV_AC_NONE;
				else{
					if(pelem->elemunion.pvar->v_flags == 256) { // InputFlag is set
						return OV_AC_READWRITE;
					}
					/* Nicht FB? */
					if(pelem->elemunion.pvar->v_varprops & OV_VP_SETACCESSOR) {
						return OV_AC_READWRITE;
					}
					return OV_AC_READ;
				}
			}
		break;
		default:
		break;
	}
	return ov_object_getaccess(pobj, pelem, pticket);
}

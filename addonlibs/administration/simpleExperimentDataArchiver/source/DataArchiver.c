/******************************************************************************
 *
 *   FILE
 *   ----
 *   DataArchiver.c
 *
 *   History
 *   -------
 *   2017-10-10   File created
 *
 *******************************************************************************
 *
 *   This file is generated by the 'acplt_builder' command
 *
 ******************************************************************************/

#ifndef OV_COMPILE_LIBRARY_simpleExperimentDataArchiver
#define OV_COMPILE_LIBRARY_simpleExperimentDataArchiver
#endif

#include "simpleExperimentDataArchiver.h"
#include "libov/ov_macros.h"
#include <unistd.h>
#include "fb_namedef.h"
#include "open62541.h"
#include <pthread.h>
#include "opcua_helpers.h"

#include "libov/ov_debug.h"

#define OV_DEBUG

#if OV_SYSTEM_NT
#include <windows.h>
#endif

#if OV_SYSTEM_NT
#define SLEEP(TIME) Sleep(TIME);
#endif
#if OV_SYSTEM_LINUX
#define SLEEP(TIME) sleep(TIME);
#endif

#define MUTEX_LOCK(MUTEX)\
		pthread_mutex_lock(MUTEX);\

#define MUTEX_UNLOCK(MUTEX)\
		pthread_mutex_unlock(MUTEX);\

/* typedefs */
/* thread exchange-data */
typedef struct subData {
	UA_Variant data;
	UA_Boolean newDataAvailable;
	UA_NodeId subscriptionNodeId;
	UA_DateTime timeStamp;
	UA_UInt32 monId;
	UA_UInt32 subId;
	UA_StatusCode uaStatusCodeSub;
} subData;

typedef struct opcua_connection {
	UA_Boolean connected;
	UA_Boolean error;
	UA_UInt16 subDataVecLength;
	subData *subDataVec;
	UA_StatusCode uaStatusCodeCon;
} opcua_connection;

/*Subscription settings values from UA_SubscriptionSettings_standard */
UA_SubscriptionSettings SubscriptionSettings = {
		0.0, /* .requestedPublishingInterval 500.0 */
		30, /* .requestedLifetimeCount 10000 */
		10, /* .requestedMaxKeepAliveCount 1 */
		1024, /* .maxNotificationsPerPublish 10 */
		true, /* .publishingEnabled true */
		0 /* .priority 0 */
};

typedef enum {
	CONFIGURE = 0,
	PREPARECONNECTION = 1,
	WAITING = 3,
	CONNECTED = 4,
	CLEANUP = 6,
	FAULT = 99
} DataArchiverState_Types;

#define FREEMEMORY()\
		MUTEX_LOCK(&pthreadData->mutex);\
		if(pthreadData->connectionState.subDataVecLength > 0){\
			Ov_HeapFree(pthreadData->connectionState.subDataVec);\
			pthreadData->connectionState.subDataVecLength = 0;\
		}\
		MUTEX_UNLOCK(&pthreadData->mutex);\
		Ov_SetDynamicVectorLength(&pinst->v_NodeIDNSindex,0,UINT);\
		Ov_SetDynamicVectorLength(&pinst->v_NodeIDIdentifier,0,STRING);\
		Ov_SetDynamicVectorLength(&pinst->v_NodeIDIdType,0,UINT);\

#define DESTROYTHREAD() \
		if(&pthreadData->mutex){\
			MUTEX_LOCK(&pthreadData->mutex);\
			pthreadData->connectionState.error = FALSE;\
			pthreadData->connectionState.connected = FALSE;\
			MUTEX_UNLOCK(&pthreadData->mutex);\
			pthread_mutex_destroy(&pthreadData->mutex);\
			pthread_mutex_init(&pthreadData->mutex,NULL);\
		}\

typedef struct thread_data{
	opcua_connection connectionState;
	pthread_mutex_t mutex;
	UA_Boolean thread_run;
	pthread_t thread;
	OV_STRING serverEndpointUrl;
}thread_data;

/* helper functions */
OV_BOOL initSubData(subData *Data, OV_INT NSI, OV_STRING NODE_ID, UA_IdType Type) {
	if (!Data) {
		// error
		return FALSE;
	}
	UA_Variant_init(&(Data->data));
	UA_DateTime_init(&Data->timeStamp);
	Data->monId = 0;
	Data->newDataAvailable = FALSE;
	Data->uaStatusCodeSub = UA_STATUSCODE_GOOD;

	switch (Type){
	case UA_IDTYPE_STRING:{ // 1
		Data->subscriptionNodeId = UA_NODEID_STRING(NSI, NODE_ID);
		return TRUE;
	}
	case UA_IDTYPE_NUMERIC:{ // 0
		Data->subscriptionNodeId = UA_NODEID_NUMERIC(NSI,strtol(NODE_ID,NULL,10));
		return TRUE;
	}
	case UA_IDTYPE_GUID:{ // 2
		//TODO: implement
		return FALSE;
	}
	case UA_IDTYPE_OPAQUE:{ // 3
		//TODO: implement
		return FALSE;
	}
	default:{
		// TODO: implement
		return FALSE;
	}
	}
	return TRUE;
}

int subscription_UpdateDate(subData * data, UA_DataValue *value, pthread_mutex_t* pmutex) {

	UA_StatusCode retval = UA_STATUSCODE_GOOD; //TODO: Add proper error handling

	MUTEX_LOCK(pmutex);
	if ((value->hasValue && value->value.data != NULL) && (value->sourceTimestamp > data->timeStamp)) {
		data->newDataAvailable = true;
		UA_Variant_deleteMembers(&data->data);
		UA_Variant_copy(&value->value, &data->data);
		data->timeStamp = value->sourceTimestamp;
		MUTEX_UNLOCK(pmutex);
		return retval;
	}
	MUTEX_UNLOCK(pmutex);
	return retval;
}

static void handler_c1NewData(UA_UInt32 monId, UA_DataValue *value,
		void *context) {

	thread_data* pthreadData = context;
	UA_StatusCode retval = UA_STATUSCODE_GOOD;

	/* update data of monitored items */
	for (OV_UINT ItemIterator = 0; ItemIterator < pthreadData->connectionState.subDataVecLength;ItemIterator++) {
		if(pthreadData->connectionState.subDataVec[ItemIterator].monId == monId){
			retval = subscription_UpdateDate(&(pthreadData->connectionState.subDataVec[ItemIterator]), value, &pthreadData->mutex);
			if (retval != UA_STATUSCODE_GOOD) {
				// TODO: Add error handling
			}
		}
	}
}

int subscription_DeleteMonitoredItem(subData *data) {
	UA_StatusCode retval = UA_STATUSCODE_GOOD;
	if(!data){
		return retval;
	}
	UA_Variant_deleteMembers(&data->data);
	data->monId = 0;
	data->newDataAvailable = FALSE;
	data->subId = 0;
	// TODO: Set NodeID to NULL
	data->timeStamp = 0;
	return retval;
}

int subscription_AddMonitoredItem(UA_NodeId* NodeId, UA_UInt32 *monId, UA_UInt32 subId, UA_Client *client, thread_data* pthreadData) {
	UA_UInt32 tmpMonId = 0;
	UA_StatusCode retval = UA_STATUSCODE_GOOD;

	*monId = 0;

	retval = UA_Client_Subscriptions_addMonitoredItem(client, subId, *NodeId, UA_ATTRIBUTEID_VALUE, &handler_c1NewData, (void*) pthreadData, &tmpMonId);

	if (retval != UA_STATUSCODE_GOOD)
		return retval;
	*monId = tmpMonId;
	return retval;
}

/* creates new opc ua client
 * connects client to endpoint
 * registers subscriptions */
static int opcua_subscription_client(thread_data* pthreadData) {
	/*create new opc ua client */

	UA_Client *client = UA_Client_new(UA_ClientConfig_standard);
	MUTEX_LOCK(&pthreadData->mutex);
	OV_STRING tmpEndpoint = NULL;
	ov_string_setvalue(&tmpEndpoint, pthreadData->serverEndpointUrl);
	MUTEX_UNLOCK(&pthreadData->mutex);
	/* Connect to a server */
	UA_StatusCode retval = UA_Client_connect(client, tmpEndpoint);
	ov_string_setvalue(&tmpEndpoint,"");
	if (retval != UA_STATUSCODE_GOOD) {
		// connection failed
		MUTEX_LOCK(&pthreadData->mutex);
		pthreadData->connectionState.error = TRUE;
		pthreadData->connectionState.uaStatusCodeCon = retval;
		MUTEX_UNLOCK(&pthreadData->mutex);
		goto deleteClient;
	}

	/* create new subscription */
	UA_UInt32 subId = 0;
	retval = UA_Client_Subscriptions_new(client, SubscriptionSettings, &subId);
	if (retval != UA_STATUSCODE_GOOD) {
		// create new subscription failed
		MUTEX_LOCK(&pthreadData->mutex);
		pthreadData->connectionState.error = TRUE;
		pthreadData->connectionState.uaStatusCodeCon = retval;
		MUTEX_UNLOCK(&pthreadData->mutex);
		goto deleteClient;
	}
	MUTEX_LOCK(&pthreadData->mutex);
	/* add monitored items to subscription */
	for (OV_UINT ItemIterator = 0; ItemIterator < pthreadData->connectionState.subDataVecLength; ItemIterator++) {
		pthreadData->connectionState.subDataVec[ItemIterator].uaStatusCodeSub = subscription_AddMonitoredItem(&pthreadData->connectionState.subDataVec[ItemIterator].subscriptionNodeId, &pthreadData->connectionState.subDataVec[ItemIterator].monId, subId, client, pthreadData);
	}

	pthreadData->connectionState.connected = true;
	UA_Boolean run = pthreadData->thread_run;
	MUTEX_UNLOCK(&pthreadData->mutex);

	while ( run && retval == UA_STATUSCODE_GOOD) {
		retval = UA_Client_Subscriptions_manuallySendPublishRequest(client);
		if ( retval != UA_STATUSCODE_GOOD){
			MUTEX_LOCK(&pthreadData->mutex);
			pthreadData->connectionState.error = TRUE;
			pthreadData->connectionState.uaStatusCodeCon = retval;
			MUTEX_UNLOCK(&pthreadData->mutex);
			break;
		}
		if (UA_Client_getState(client) != UA_CLIENTSTATE_CONNECTED){
			MUTEX_LOCK(&pthreadData->mutex);
			pthreadData->connectionState.error = TRUE;
			pthreadData->connectionState.uaStatusCodeCon = UA_STATUSCODE_BADNOTCONNECTED;
			MUTEX_UNLOCK(&pthreadData->mutex);
			break;
		}
		SLEEP(1);
		MUTEX_LOCK(&pthreadData->mutex);
		run = pthreadData->thread_run;
		MUTEX_UNLOCK(&pthreadData->mutex);
	}

	/* Delete the subscription */
	UA_Client_Subscriptions_remove(client, subId);

	/* delete  monitored items from subscription */
	MUTEX_LOCK(&pthreadData->mutex);
	for (OV_UINT ItemIterator = 0; ItemIterator < pthreadData->connectionState.subDataVecLength;ItemIterator++) {
		retval = subscription_DeleteMonitoredItem(&pthreadData->connectionState.subDataVec[ItemIterator]);
		if (retval != UA_STATUSCODE_GOOD) {
		}
	}
	MUTEX_UNLOCK(&pthreadData->mutex);

	deleteClient: UA_Client_disconnect(client);
	UA_Client_delete(client);
	return (int) retval;
}

/* thread function */
static void* thread_fcn(void*ptr){
	//create opc ua client

	thread_data* pthreadData = ptr;
	opcua_subscription_client(pthreadData);

	MUTEX_LOCK(&pthreadData->mutex);
	pthreadData->connectionState.connected = FALSE;

	MUTEX_UNLOCK(&pthreadData->mutex);
	pthread_exit(0);
}

OV_RESULT writeResultToVariable(thread_data * pthreadData, OV_INSTPTR_simpleExperimentDataArchiver_DataArchiver pDataArchiver){
	OV_RESULT result = OV_ERR_OK;
	OV_INSTPTR_simpleExperimentDataArchiver_Variable pVariable = NULL;
	UA_StatusCode retval = UA_STATUSCODE_GOOD;

	MUTEX_LOCK(&pthreadData->mutex);
	OV_UINT Iterator = 0;
	Ov_ForEachChildEx(simpleExperimentDataArchiver_SubscriptionResult,pDataArchiver,pVariable,simpleExperimentDataArchiver_Variable){
		if(pVariable->v_varValue.value.vartype != OV_VT_VOID){
			Ov_SetAnyValue(&pVariable->v_varValue,NULL);
		}

		if(&pthreadData->connectionState.subDataVec[Iterator].newDataAvailable &&
				pthreadData->connectionState.subDataVec[Iterator].newDataAvailable &&
				pthreadData->connectionState.subDataVec[Iterator].uaStatusCodeSub == UA_STATUSCODE_GOOD){
			retval = ov_VariantToAny(&pthreadData->connectionState.subDataVec[Iterator].data,&pVariable->v_varValue);
			simpleExperimentDataArchiver_Variable_varRes_set(pVariable,retval);
			if(retval == UA_STATUSCODE_GOOD){
				pVariable->v_varValue.time = ov_1601nsTimeToOvTime(pthreadData->connectionState.subDataVec[Iterator].timeStamp);
				pVariable->v_varTime = ov_1601nsTimeToOvTime(pthreadData->connectionState.subDataVec[Iterator].timeStamp);
			}
		} else {
			simpleExperimentDataArchiver_Variable_varRes_set(pVariable,pthreadData->connectionState.subDataVec[Iterator].uaStatusCodeSub);
		}
		Iterator++;
	}
	MUTEX_UNLOCK(&pthreadData->mutex);

	return result;
}

/**************************************************************************
 ************************** GET / SET *************************************
 **************************************************************************/
OV_DLLFNCEXPORT OV_BOOL simpleExperimentDataArchiver_DataArchiver_run_get(
		OV_INSTPTR_simpleExperimentDataArchiver_DataArchiver          pobj
) {
	return pobj->v_run;
}

OV_DLLFNCEXPORT OV_RESULT simpleExperimentDataArchiver_DataArchiver_run_set(
		OV_INSTPTR_simpleExperimentDataArchiver_DataArchiver          pobj,
		const OV_BOOL  value
) {
	/*set proctime to now to execute fb immediately after run command */
	ov_time_gettime(&pobj->v_proctime);
	pobj->v_run = value;
	return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_UINT simpleExperimentDataArchiver_DataArchiver_state_get(
		OV_INSTPTR_simpleExperimentDataArchiver_DataArchiver pobj) {
	return pobj->v_state;
}

OV_DLLFNCEXPORT OV_RESULT simpleExperimentDataArchiver_DataArchiver_state_set(
		OV_INSTPTR_simpleExperimentDataArchiver_DataArchiver pobj,
		const OV_UINT value) {
	switch (value) {
	case CONFIGURE:
		simpleExperimentDataArchiver_DataArchiver_stateName_set(pobj,"CONFIGURE");
		break;
	case PREPARECONNECTION:
		simpleExperimentDataArchiver_DataArchiver_stateName_set(pobj,"PREPARECONNECTION");
		break;
	case WAITING:
		simpleExperimentDataArchiver_DataArchiver_stateName_set(pobj,"WAITING");
		break;
	case CONNECTED:
		simpleExperimentDataArchiver_DataArchiver_stateName_set(pobj,"CONNECTED");
		break;
	case CLEANUP:
		simpleExperimentDataArchiver_DataArchiver_stateName_set(pobj,"CLEANUP");
		break;
	case FAULT:
		simpleExperimentDataArchiver_DataArchiver_stateName_set(pobj,"FAULT");
		break;
	default:
		simpleExperimentDataArchiver_DataArchiver_stateName_set(pobj,"NOVALIDSTATE");
		break;
	}
	/* TODO: execute fb immediately after state change */
	pobj->v_state = value;
	return OV_ERR_OK;
}
OV_DLLFNCEXPORT OV_RESULT simpleExperimentDataArchiver_DataArchiver_stateName_set(
		OV_INSTPTR_simpleExperimentDataArchiver_DataArchiver          pobj,
		const OV_STRING  value
) {
	return ov_string_setvalue(&pobj->v_stateName,value);
}
OV_DLLFNCEXPORT OV_BOOL simpleExperimentDataArchiver_DataArchiver_reset_get(
		OV_INSTPTR_simpleExperimentDataArchiver_DataArchiver          pobj
) {
	return pobj->v_reset;
}

OV_DLLFNCEXPORT OV_RESULT simpleExperimentDataArchiver_DataArchiver_reset_set(
		OV_INSTPTR_simpleExperimentDataArchiver_DataArchiver          pobj,
		const OV_BOOL  value
) {
	/*set proctime to now to execute fb immediately after reset command */
	ov_time_gettime(&pobj->v_proctime);
	pobj->v_reset = value;
	return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_RESULT simpleExperimentDataArchiver_DataArchiver_errorMsg_set(
		OV_INSTPTR_simpleExperimentDataArchiver_DataArchiver          pobj,
		const OV_STRING  value
) {
	return ov_string_setvalue(&pobj->v_errorMsg,value);
}

OV_DLLFNCEXPORT OV_RESULT simpleExperimentDataArchiver_DataArchiver_serverEndpointUrl_set(
		OV_INSTPTR_simpleExperimentDataArchiver_DataArchiver pobj,
		const OV_STRING value) {

	if(simpleExperimentDataArchiver_DataArchiver_state_get(pobj) == CONFIGURE){
		return ov_string_setvalue(&pobj->v_serverEndpointUrl, value);
	} else {
		return ov_string_setvalue(&pobj->v_serverEndpointUrl, value);
	}

}

/*******************************************************************************
 ****************************** CONSTRUCTOR ************************************
 *******************************************************************************/


OV_DLLFNCEXPORT OV_RESULT simpleExperimentDataArchiver_DataArchiver_constructor(
		OV_INSTPTR_ov_object 	pobj
) {
	/*
	 *   local variables
	 */
	OV_INSTPTR_simpleExperimentDataArchiver_DataArchiver pinst = Ov_StaticPtrCast(simpleExperimentDataArchiver_DataArchiver, pobj);
	OV_RESULT    result;
	thread_data *pthreadData = NULL;

	/* do what the base class does first */
	result = fb_functionblock_constructor(pobj);
	if(Ov_Fail(result))
		return result;

	pinst->v_state = CONFIGURE;
	pinst->v_run = FALSE;
	pinst->v_reset = FALSE;

	/* allocate memory and initialize */
	if(!pinst->v_threadDataHndl){
		pthreadData = Ov_HeapMalloc(sizeof(thread_data));
		if(!pthreadData){
			return OV_ERR_GENERIC;
		}
		pinst->v_threadDataHndl = (OV_INT) pthreadData;
		pthreadData->connectionState.connected = FALSE;
		pthreadData->connectionState.error = FALSE;
		pthreadData->connectionState.subDataVecLength = 0;
		pthreadData->connectionState.subDataVec = NULL;
		pthreadData->connectionState.uaStatusCodeCon = UA_STATUSCODE_GOOD;
		pthread_mutex_init(&pthreadData->mutex,NULL);
		pthreadData->serverEndpointUrl = NULL;
		pthreadData->thread_run = FALSE;
	} else {
		return OV_ERR_GENERIC;
	}

	return OV_ERR_OK;
}

/*******************************************************************************
 ****************************** TYPEMETHOD *************************************
 *******************************************************************************/

OV_DLLFNCEXPORT void simpleExperimentDataArchiver_DataArchiver_typemethod(
		OV_INSTPTR_fb_functionblock pfb, OV_TIME *pltc) {

	/*
	 *   local variables
	 */
	OV_INSTPTR_simpleExperimentDataArchiver_DataArchiver pinst = Ov_StaticPtrCast(simpleExperimentDataArchiver_DataArchiver, pfb);
	DataArchiverState_Types state = simpleExperimentDataArchiver_DataArchiver_state_get(pinst); /* current state of the fb*/

	OV_STRING tmpfilename = NULL;
	OV_STRING tmpStr = NULL;
	OV_STRING valueStr = NULL;
	OV_STRING strMeasData = NULL;

	int ret = 0;

	thread_data *pthreadData = NULL;

	OV_INSTPTR_simpleExperimentDataArchiver_Variable pVariable = NULL;


#undef Finally
#define Finally() \
		ov_string_setvalue(&tmpfilename,NULL); \
		ov_string_setvalue(&tmpStr,NULL); \
		ov_string_setvalue(&valueStr,NULL); \
		ov_string_setvalue(&strMeasData,NULL); \


#define CHECKCONNECTION() \
		if(!&pthreadData->mutex){\
			state = FAULT;\
			ov_string_setvalue(&pinst->v_errorMsg, "Not connected or Error");\
		} else {\
			MUTEX_LOCK(&pthreadData->mutex);\
			if (!pthreadData->connectionState.connected || pthreadData->connectionState.error) {\
				ov_time_gettime(&pinst->v_lastConnectionTry);\
				state = FAULT;\
				ov_string_print(&pinst->v_errorMsg, "Not connected or Error - UA_STATUSCODE: %x",pthreadData->connectionState.uaStatusCodeCon);\
				MUTEX_UNLOCK(&pthreadData->mutex);\
				break;\
			}\
			MUTEX_UNLOCK(&pthreadData->mutex);\
		}\

#define RESETINPUT();\
		pinst->v_run = FALSE;\

	/* get threadData pointer */
	if(!pinst->v_threadDataHndl){
		state = FAULT;
		ov_string_setvalue(&pinst->v_errorMsg, "Constructor of FB failed");
	} else {
		pthreadData = (thread_data *) pinst->v_threadDataHndl;
	}

	if(pinst->v_reset){
		state = CLEANUP;
	}

	switch (state) {
	/* Configuration of EndpointURL and NodeId only in this state */
	case CONFIGURE:
		if(pinst->v_run){
			state = PREPARECONNECTION;
		}
		break;

		/* STATE: PREPARECONNECTION
		 * check inputs
		 * prepare connection */
	case PREPARECONNECTION:
		pthread_mutex_init (&pthreadData->mutex, NULL);
		MUTEX_LOCK(&pthreadData->mutex);
		ov_string_setvalue(&pthreadData->serverEndpointUrl,pinst->v_serverEndpointUrl);
		if (ov_string_compare(pthreadData->serverEndpointUrl,"") == OV_STRCMP_EQUAL) {
			ov_string_setvalue(&pinst->v_errorMsg,"PREPARECONNECTION: serverEndpointUrl missing");
			state = FAULT;
			break;
		}

		/* Initialize data: */
		OV_UINT childcount = 0;
		Ov_ForEachChildEx(simpleExperimentDataArchiver_SubscriptionResult,pinst,pVariable,simpleExperimentDataArchiver_Variable){
			Ov_SetDynamicVectorLength(&pinst->v_NodeIDNSindex,childcount + 1,UINT);
			Ov_SetDynamicVectorLength(&pinst->v_NodeIDIdentifier,childcount + 1,STRING);
			Ov_SetDynamicVectorLength(&pinst->v_NodeIDIdType,childcount + 1,UINT);

			pinst->v_NodeIDNSindex.value[childcount] = simpleExperimentDataArchiver_Variable_NodeIDNSindex_get(pVariable);
			ov_string_setvalue(&pinst->v_NodeIDIdentifier.value[childcount],simpleExperimentDataArchiver_Variable_NodeIDIdentifier_get(pVariable));
			pinst->v_NodeIDIdType.value[childcount] = simpleExperimentDataArchiver_Variable_NodeIDIdType_get(pVariable);

			childcount++;
		}

		/* compare length */
		if (((pinst->v_NodeIDIdType.veclen != pinst->v_NodeIDIdentifier.veclen)
				|| (pinst->v_NodeIDIdentifier.veclen
						!= pinst->v_NodeIDNSindex.veclen))
				|| pinst->v_NodeIDIdType.veclen == 0) {
			ov_string_setvalue(&pinst->v_errorMsg,"PREPARECONNECTION: mismatch between NodeIDType, NodeIDIdentifier, NodeIDNSindex");
			state = FAULT;
			break;
		}
		/* check for NULL entry*/
		OV_UINT index = 0;
		while (index < pinst->v_NodeIDIdentifier.veclen &&
				ov_string_compare(pinst->v_NodeIDIdentifier.value[index], "")!= OV_STRCMP_EQUAL) {
			index++;
		}
		if (index != pinst->v_NodeIDIdentifier.veclen) {
			ov_string_setvalue(&pinst->v_errorMsg,
					"PREPARECONNECTION: One NodeIDIdentifier is empty");
			state = FAULT;
			break;
		}

		/* allocate memory
		 * free in state CLEANUP OR FAULT */
		pthreadData->connectionState.subDataVecLength =  pinst->v_NodeIDIdentifier.veclen;
		pthreadData->connectionState.subDataVec = Ov_HeapMalloc( pinst->v_NodeIDIdentifier.veclen  * sizeof(subData));
		if(!pthreadData->connectionState.subDataVec){
			ov_string_setvalue(&pinst->v_errorMsg,"PREPARECONNECTION: Out of memory");
			state = FAULT;
			break;
		}

		for(OV_UINT Iterator = 0; Iterator < pthreadData->connectionState.subDataVecLength; Iterator++){
			if (initSubData(&(pthreadData->connectionState.subDataVec[Iterator]),
					pinst->v_NodeIDNSindex.value[Iterator],
					pinst->v_NodeIDIdentifier.value[Iterator],
					pinst->v_NodeIDIdType.value[Iterator])){
			} else {
				ov_string_setvalue(&pinst->v_errorMsg, "PREPARECONNECTION: initSubData failed");
				state = FAULT;
				break;
			}
		}

		if(state == FAULT){
			break;
		}
		/* Connect to server:
		 * start subscription thread
		 * thread_fcn starts OPC UA client,
		 * connects to server
		 * and registers subscriptions */
		pthreadData->thread_run = TRUE;
		MUTEX_UNLOCK(&pthreadData->mutex);
		ret = pthread_create(&pthreadData->thread, NULL, thread_fcn, (void*) pthreadData);
		ov_time_gettime(&pinst->v_lastConnectionTry);
		if (ret) {
			ov_string_setvalue(&pinst->v_errorMsg, "PREPARECONNECTION: thread failed");
			state = FAULT;
		} else {
			state = WAITING;
		}

		break;

		/*****************************************************************************
		 *  STATE: WAITING
		 * wait for connection */
	case WAITING:
		MUTEX_LOCK(&pthreadData->mutex);
		if(pthreadData->connectionState.error){
			ov_string_print(&pinst->v_errorMsg, "UA_STATUSCODE: %x",pthreadData->connectionState.uaStatusCodeCon);
			state = FAULT;
		}
		if(pthreadData->connectionState.connected){ //TODO: add timeout count
			state = CONNECTED;
		}
		MUTEX_UNLOCK(&pthreadData->mutex);
		break;
		/*****************************************************************************
		 *  STATE: CONNECTED
		 * update  variables */
	case CONNECTED:
		/* check connection */
		CHECKCONNECTION();

		writeResultToVariable(pthreadData,pinst);
		if (!pinst->v_run || pinst->v_reset) {
			state = CLEANUP;
		}
		break;
		/**************************************************************************************************
		 * STATE: FAULT
		 * in case of error, wait for reset */
	case FAULT:
		RESETINPUT();

		/* stop thread / opc ua client */
		MUTEX_LOCK(&pthreadData->mutex);
		pthreadData->thread_run = FALSE;
		if(pthreadData->connectionState.connected){
			/* wait for thread to stop */
			/* execute fb immediately */
			ov_time_gettime(&pinst->v_proctime);
			MUTEX_UNLOCK(&pthreadData->mutex);
			break;
		}
		MUTEX_UNLOCK(&pthreadData->mutex);

		/* free memory */
		FREEMEMORY();

		/* destroy thread */
		DESTROYTHREAD();

		/* TODO: retry after give time in case of connection error */

		if(pinst->v_reset){
			state = CONFIGURE;
			if(ov_string_compare(pinst->v_errorMsg,"")!=OV_STRCMP_EQUAL){
				printf("%s",pinst->v_errorMsg);
			}
			ov_string_setvalue(&pinst->v_errorMsg,NULL);
			/* reset reset command */
			pinst->v_reset = FALSE;
		}
		break;

		/**************************************************************************************************
		 * STATE: CLEANUP*/
	case CLEANUP:
		/* reset reset command */
		pinst->v_reset = FALSE;
		RESETINPUT();

		/* stop thread / opc ua client */
		MUTEX_LOCK(&pthreadData->mutex);
		pthreadData->thread_run = FALSE;
		if(pthreadData->connectionState.connected){
			/* wait for thread to stop */
			/* execute fb immediately */
			ov_time_gettime(&pinst->v_proctime);
			MUTEX_UNLOCK(&pthreadData->mutex);
			break;
		}
		MUTEX_UNLOCK(&pthreadData->mutex);

		/* free memory */
		FREEMEMORY();

		/* destroy thread */
		DESTROYTHREAD();

		state = CONFIGURE;
		ov_string_setvalue(&pinst->v_errorMsg,"");
		break;
		/**/
	default:
		state = FAULT;
		ov_string_setvalue(&pinst->v_errorMsg, "invalid state: default switch case");
		break;
	}

	simpleExperimentDataArchiver_DataArchiver_state_set(pinst, state);
	Finally();
	return;
}

/******************************************************************************************************
 ****************************************** DESTRUCTOR ************************************************
 */

OV_DLLFNCEXPORT void simpleExperimentDataArchiver_DataArchiver_destructor(
		OV_INSTPTR_ov_object 	pobj
) {
	/*
	 *   local variables
	 */
	OV_INSTPTR_simpleExperimentDataArchiver_DataArchiver pinst = Ov_StaticPtrCast(simpleExperimentDataArchiver_DataArchiver, pobj);
	thread_data * pthreadData = (thread_data *) pinst->v_threadDataHndl;
	OV_UINT timeOut = 0;

	/* free memory */
	MUTEX_LOCK(&pthreadData->mutex);
	pthreadData->thread_run = FALSE;
	while(pthreadData->connectionState.connected == TRUE && timeOut > 10000){
		// wait for connection end
		timeOut++;
	}
	MUTEX_UNLOCK(&pthreadData->mutex);
	pthread_join(pthreadData->thread,NULL);

	pthreadData->connectionState.connected = FALSE;
	pthreadData->connectionState.error = FALSE;
	pthreadData->connectionState.subDataVecLength = 0;
	pthreadData->connectionState.subDataVec = NULL;

	ov_string_setvalue(&pthreadData->serverEndpointUrl,NULL);

	FREEMEMORY();

	Ov_HeapFree(pthreadData);

	pinst->v_threadDataHndl = 0;

	/* destroy object */
	fb_functionblock_destructor(pobj);

	return;
}

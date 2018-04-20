#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cJSON.h>
#include <wrp-c.h>
#include "crud_tasks.h"
#include "crud_internal.h"

int processCrudRequest( wrp_msg_t *reqMsg, wrp_msg_t **responseMsg)
{
    cJSON *response = NULL;
    wrp_msg_t *resp_msg = NULL;
    char *str= NULL;
    int ret = -1;
    
    resp_msg = ( wrp_msg_t *)malloc( sizeof( wrp_msg_t ) );  
    memset(resp_msg, 0, sizeof(wrp_msg_t));
    
    resp_msg->msg_type = reqMsg->msg_type;
    resp_msg->u.crud.transaction_uuid = reqMsg->u.crud.transaction_uuid;
    resp_msg->u.crud.source = reqMsg->u.crud.dest;
    resp_msg->u.crud.dest = reqMsg->u.crud.source;
    resp_msg ->u.crud.path =  reqMsg->u.crud.path;

    switch( reqMsg->msg_type ) 
    {
    
	case WRP_MSG_TYPE__CREATE:
	    ParodusInfo( "CREATE request\n" );
	    
	    ret = createObject( reqMsg, &resp_msg );
	    
	    if(ret == 0)
	    {
		    response = cJSON_CreateObject();
		    cJSON_AddNumberToObject(response, "statusCode", 200);
		    cJSON_AddStringToObject(response, "message", "Success");
		                    
		    if(response != NULL)
		    {
			str = cJSON_PrintUnformatted(response);
			ParodusInfo("Payload Response: %s\n", str);

			resp_msg ->u.crud.payload = (void *)str;
			resp_msg ->u.crud.payload_size = strlen(str);

			*responseMsg = resp_msg;
			ParodusInfo("(*responseMsg)->u.crud.payload : %s\n", (char* )(*responseMsg)->u.crud.payload);
			ParodusInfo("(*responseMsg)->u.crud.payload_size : %lu\n", (*responseMsg)->u.crud.payload_size);
			cJSON_Delete(response);
		    }
	    }
	    else
	    {
	    	ParodusError("Failed to create object in config JSON\n");
	    	response = cJSON_CreateObject();
		cJSON_AddNumberToObject(response, "statusCode", 520);
		cJSON_AddStringToObject(response, "message", "Failure");
		if(response != NULL)
		{
			str = cJSON_PrintUnformatted(response);
			ParodusInfo("Payload Response: %s\n", str);

			resp_msg ->u.crud.payload = (void *)str;
			resp_msg ->u.crud.payload_size = strlen(str);

			*responseMsg = resp_msg;
			ParodusInfo("(*responseMsg)->u.crud.payload : %s\n", (char* )(*responseMsg)->u.crud.payload);
			ParodusInfo("(*responseMsg)->u.crud.payload_size : %lu\n", (*responseMsg)->u.crud.payload_size);
			cJSON_Delete(response);
		}
	    }
	    break;
	    
	case WRP_MSG_TYPE__RETREIVE:
	    ParodusInfo( "RETREIVE request\n" );
            ret = retrieveObject( reqMsg, &resp_msg );
	    
	    if(ret == 0)
	    {
		    response = cJSON_CreateObject();
		    cJSON_AddNumberToObject(response, "statusCode", 200);
		    cJSON_AddStringToObject(response, "message", "Success");
		    
		    cJSON_AddStringToObject(response, "data", (resp_msg)->u.crud.payload);
		                    
		    if(response != NULL)
		    {
			str = cJSON_PrintUnformatted(response);
			ParodusInfo("Payload Response: %s\n", str);

			resp_msg ->u.crud.payload = (void *)str;
			resp_msg ->u.crud.payload_size = strlen(str);

			*responseMsg = resp_msg;
			ParodusInfo("(*responseMsg)->u.crud.payload : %s\n", (char* )(*responseMsg)->u.crud.payload);
			ParodusInfo("(*responseMsg)->u.crud.payload_size : %lu\n", (*responseMsg)->u.crud.payload_size);
			cJSON_Delete(response);
		    }
	    }
	    else
	    {
	    	ParodusError("Failed to retrieve object in config JSON\n");
	    	response = cJSON_CreateObject();
		cJSON_AddNumberToObject(response, "statusCode", 520);
		cJSON_AddStringToObject(response, "message", "Failure");
		if(response != NULL)
		{
			str = cJSON_PrintUnformatted(response);
			ParodusInfo("Payload Response: %s\n", str);

			resp_msg ->u.crud.payload = (void *)str;
			resp_msg ->u.crud.payload_size = strlen(str);

			*responseMsg = resp_msg;
			ParodusInfo("(*responseMsg)->u.crud.payload : %s\n", (char* )(*responseMsg)->u.crud.payload);
			ParodusInfo("(*responseMsg)->u.crud.payload_size : %lu\n", (*responseMsg)->u.crud.payload_size);
			cJSON_Delete(response);
		}
	    }
	    break;
	    
	case WRP_MSG_TYPE__UPDATE:
	    ParodusInfo( "UPDATE request\n" );
            response = cJSON_CreateObject();
            cJSON_AddNumberToObject(response, "statusCode", 200);
            cJSON_AddStringToObject(response, "message", "Success");
                            
	    if(response != NULL)
            {
		str = cJSON_PrintUnformatted(response);
		ParodusInfo("Payload Response: %s\n", str);


		resp_msg ->u.crud.payload = (void *)str;
		resp_msg ->u.crud.payload_size = strlen(str);

		*responseMsg = resp_msg;
		ParodusInfo("(*responseMsg)->u.crud.payload : %s\n", (char* )(*responseMsg)->u.crud.payload);
		ParodusInfo("(*responseMsg)->u.crud.payload_size : %lu\n", (*responseMsg)->u.crud.payload_size);
		cJSON_Delete(response);
	    }
	    break;
	    
	case WRP_MSG_TYPE__DELETE:
	    ParodusInfo( "DELETE request\n" );
	    
	    ret = deleteObject( reqMsg, &resp_msg );
	    
	    if(ret == 0)
	    {
	    
		    response = cJSON_CreateObject();
		    cJSON_AddNumberToObject(response, "statusCode", 200);
		    cJSON_AddStringToObject(response, "message", "Success");
		                    
		    if(response != NULL)
		    {
			str = cJSON_PrintUnformatted(response);
			ParodusInfo("Payload Response: %s\n", str);


			resp_msg ->u.crud.payload = (void *)str;
			resp_msg ->u.crud.payload_size = strlen(str);

			*responseMsg = resp_msg;
			ParodusInfo("(*responseMsg)->u.crud.payload : %s\n", (char* )(*responseMsg)->u.crud.payload);
			ParodusInfo("(*responseMsg)->u.crud.payload_size : %lu\n", (*responseMsg)->u.crud.payload_size);
			cJSON_Delete(response);
		    }
	     }
	     else
	     {
	     		ParodusError("Failed to delete object in config JSON\n");
		    	response = cJSON_CreateObject();
			cJSON_AddNumberToObject(response, "statusCode", 520);
			cJSON_AddStringToObject(response, "message", "Failure");
			if(response != NULL)
			{
				str = cJSON_PrintUnformatted(response);
				ParodusInfo("Payload Response: %s\n", str);

				resp_msg ->u.crud.payload = (void *)str;
				resp_msg ->u.crud.payload_size = strlen(str);

				*responseMsg = resp_msg;
				ParodusInfo("(*responseMsg)->u.crud.payload : %s\n", (char* )(*responseMsg)->u.crud.payload);
				ParodusInfo("(*responseMsg)->u.crud.payload_size : %lu\n", (*responseMsg)->u.crud.payload_size);
				cJSON_Delete(response);
			}
	     	
	     }
	      
	     break;
	    
	default:
	    ParodusInfo( "Unknown msgType for CRUD request\n" );
	    break;
     }
        
    return  0;
}


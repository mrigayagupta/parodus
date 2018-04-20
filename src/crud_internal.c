/**
 * Copyright 2016 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cJSON.h>
#include <wrp-c.h>
#include "crud_tasks.h"
#include "crud_internal.h"

#define CRUD_CONFIG_FILE  "parodus_cfg.json"


static int writeToJSON(char *data)
{
    FILE *fp;
    fp = fopen(CRUD_CONFIG_FILE , "w+");
    if (fp == NULL) 
    {
        ParodusError("Failed to open file %s\n", CRUD_CONFIG_FILE );
        return 0;
    }
    fwrite(data, strlen(data), 1, fp);
    fclose(fp);
    return 1;
}

static int readFromJSON(char **data)
{
    FILE *fp;
    int ch_count = 0;
    fp = fopen(CRUD_CONFIG_FILE, "r+");
    if (fp == NULL) 
    {
        ParodusError("Failed to open file %s\n", CRUD_CONFIG_FILE);
        return 0;
    }
    fseek(fp, 0, SEEK_END);
	ch_count = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	*data = (char *) malloc(sizeof(char) * (ch_count + 1));
	fread(*data, 1, ch_count,fp);
	(*data)[ch_count] ='\0';
	fclose(fp);
	return 1;
}


int createObject( wrp_msg_t *reqMsg, wrp_msg_t **response )
{
	char *destVal = NULL;
	char objName[100] = {'\0'};
	char tagName[100] = {'\0'};
	char *temp_ptr = NULL;
	char *out = NULL;
	cJSON *parameters = NULL, *paramObj = NULL;

	printf("Inside createObject\n");
	ParodusInfo("resp_msg->u.crud.source is %s\n", reqMsg->u.crud.dest);
	ParodusInfo("reqMsg->u.crud.payload is %s\n", (char *)reqMsg->u.crud.payload);

	if(reqMsg->u.crud.dest !=NULL)
	{
	   destVal = strdup(reqMsg->u.crud.dest);
	   printf("destVal is %s\n", destVal);

	    if( (destVal != NULL))
	    {
	    	temp_ptr = strtok(destVal , "/");
		temp_ptr = strtok(NULL , "/"); 
		temp_ptr = strtok(NULL , "/");

		parStrncpy(tagName, temp_ptr, sizeof(tagName));
		ParodusInfo("tagName is %s\n", tagName);

		parStrncpy(objName, strtok(NULL , "/"), sizeof(objName));
		ParodusInfo("Object name parsed from CREATE request:%s\n", objName);
	    }
	    else
	    {
		ParodusError("Unable to parse object details from CREATE request\n");
	    }
	    
           cJSON* res_obj = cJSON_CreateObject();
	   cJSON_AddItemToObject(res_obj , tagName, parameters = cJSON_CreateArray());
	   cJSON_AddItemToArray(parameters, paramObj = cJSON_CreateObject());
	   cJSON_AddStringToObject(paramObj, objName, reqMsg->u.crud.payload);
	
           out = cJSON_PrintUnformatted(res_obj );
                
            ParodusInfo("out : %s\n",out);
            int status = writeToJSON(out);
            if(status == 1)
            {
                ParodusInfo("Data is successfully added to JSON\n");
            }
            else
            {
                ParodusError("Failed to add data to JSON\n");
            }
            printf("response:%s\n", (char *)(*response)->u.crud.payload);
	    
	}    
	
    return 0;
}



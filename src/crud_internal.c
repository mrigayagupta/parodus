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

	ParodusInfo("Inside createObject\n");
	ParodusInfo("resp_msg->u.crud.source is %s\n", reqMsg->u.crud.dest);
	ParodusInfo("reqMsg->u.crud.payload is %s\n", (char *)reqMsg->u.crud.payload);

	if(reqMsg->u.crud.dest !=NULL)
	{
	   destVal = strdup(reqMsg->u.crud.dest);
	   ParodusInfo("destVal is %s\n", destVal);

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


int retrieveObject( wrp_msg_t *reqMsg, wrp_msg_t **response )
{

	char *destVal = NULL;
	cJSON *paramArray = NULL, *parameters = NULL, *childObj = NULL;
	cJSON *json = NULL;
	char *jsonData = NULL;
	char *child_ptr,*obj[5],*resValue = NULL;
	int objlevel = 1, i = 1, j, found = 0, status;
	
	
	cJSON *jsonresponse = cJSON_CreateObject();
	
	status = readFromJSON(&jsonData);
	if(status == 1)
        {
        	ParodusInfo("Data from JSON: %s\n", jsonData);
        	json = cJSON_Parse( jsonData );
        	if( !json ) 
		{
		    ParodusError( "Error before: [%s]\n", cJSON_GetErrorPtr() );
		    (*response)->u.crud.status = -1;
		    return -1;
		} 
		else
		{
			if(reqMsg->u.crud.dest !=NULL)
			{
			   destVal = strdup(reqMsg->u.crud.dest);
			   ParodusInfo("destVal is %s\n", destVal);

			    if( (destVal != NULL))
			    {
			    	child_ptr = strtok(destVal , "/");
			    	
			    	//Get the 1st object
				obj[0] = strdup( child_ptr );
				ParodusInfo( "parent is %s\n", obj[0] );
				
				
				while( child_ptr != NULL ) 
				{
				    child_ptr = strtok( NULL, "/" );
				    if( child_ptr != NULL ) {
				        obj[i] = strdup( child_ptr );
				        ParodusInfo( "child obj[%d]:%s\n", i, obj[i] );
				        i++;
				    }
				}
				
				objlevel = i;
				ParodusInfo( " Number of object level %d\n", objlevel );
				
				paramArray = cJSON_GetObjectItem( json, obj[2] );
				if( paramArray != NULL ) 
				{
					int itemSize = cJSON_GetArraySize( paramArray );
                    			ParodusInfo( "itemSize %d\n", itemSize );
                    			if( itemSize == 0 ) //TODO check this scenario to get from paroduscfg
                    			{
                    				resValue = cJSON_GetObjectItem( json, obj[2] )->valuestring;
                        			ParodusInfo( "resValue is %s\n", resValue );
                        			(*response)->u.crud.payload = resValue;
                        			(*response)->u.crud.status = 1;
                    			
                    			}
                    			else
                    			{
                    				ParodusInfo("cJSON_GetObjectItem( json, obj[2] is %s\n", cJSON_GetObjectItem( json, obj[2])->string);
                    				//to retrieve top level tags object
                    				if( strcmp( cJSON_GetObjectItem( json, obj[2] )->string, obj[objlevel - 1] ) == 0 ) 
                    				{
                    					ParodusInfo("top level tags object\n");
                    					
                    					ParodusPrint("obj[objlevel - 1] is %s\n", obj[objlevel - 1]);
                    					cJSON_AddItemToObject( jsonresponse, obj[objlevel - 1] , parameters = cJSON_CreateArray() );
                    					
                    					
                    					for( i = 0; i < itemSize; i++ ) 
                    					{
                    						cJSON* subitem = cJSON_GetArrayItem( paramArray, i );
                                				int subitemSize = cJSON_GetArraySize( subitem );
                                				
                                				cJSON_AddItemToArray(parameters, childObj = cJSON_CreateObject());
                                
                                				for( j = 0 ; j < subitemSize ; j++ ) 
								{
								    ParodusInfo( " %s : %s \n", cJSON_GetArrayItem( subitem, j )->string, cJSON_GetArrayItem( subitem, j )->valuestring );
								    cJSON_AddStringToObject( childObj, cJSON_GetArrayItem( subitem, j )->string, cJSON_GetArrayItem( subitem, j )->valuestring );
								}
                    					}
                    					
                    				}
                    				else
                    				{
                    				
                    				    //to traverse through total number of objects in array
                    				    for( i = 0 ; i < itemSize ; i++ ) 
                    				    {
                    					cJSON* subitem = cJSON_GetArrayItem( paramArray, i );
                    					int subitemSize = cJSON_GetArraySize( subitem );
                    					
                    					//to traverse through each subitem in each test element
                    					for( j = 0 ; j < subitemSize ; j++ ) 
                    					{
                    					
                    						ParodusPrint("obj[objlevel - 1] is %s\n", obj[objlevel - 1]);
                    						ParodusInfo("cJSON_GetArrayItem( subitem, j )->string:%s\n", cJSON_GetArrayItem( subitem, j )->string);
                    						if( strcmp( cJSON_GetArrayItem( subitem, j )->string, obj[objlevel - 1] ) == 0 ) 
                    						{
                    							ParodusInfo( " %s : %s \n", cJSON_GetArrayItem( subitem, j )->string, cJSON_GetArrayItem( subitem, j )->valuestring );
                                            				cJSON_AddStringToObject( jsonresponse, cJSON_GetArrayItem( subitem, j )->string, cJSON_GetArrayItem( subitem, j )->valuestring );
                                            				ParodusInfo("Retrieve: requested object found \n");
                                            				found = 1;
                                            				break;
                    						
                    						}
                    						else
                    						{
                    							ParodusError("array object not found\n");
                    						}
                    					
                    					}
                    					if(found)
                    					{
                    						ParodusPrint("break, since array object is found\n");
                    						break;
                    				
                    					}
                    					else
                    					{
                    					       ParodusError("Unable to retrieve requested object\n");
                    					       return -1;
                    					}
                    				     }
                    				}
                    				
                    				char *str1 = cJSON_PrintUnformatted( jsonresponse );
                            			ParodusInfo( "jsonResponse %s\n", str1 );
                            			(*response)->u.crud.status = 1;
                            			(*response)->u.crud.payload = str1;
                    			
                    				
                    			}
                    			
				}
				
			    }
			    else
			    {
				ParodusError("Unable to parse object details from RETRIEVE request\n");
				return -1;
			    }
			}
			    
		     }
		}

	return 0;
}

int deleteObject( wrp_msg_t *reqMsg, wrp_msg_t **response )
{
	char *destVal = NULL;
	cJSON *paramArray = NULL, *json = NULL;
	char *jsonData = NULL;
	char *child_ptr,*obj[5], *out = NULL;
	int i = 1, j ,status =0, objlevel=1, found =0;
	int itemSize = 0, delete_status = 0;
	
	status = readFromJSON(&jsonData);
	if(status == 1)
        {
        	ParodusInfo("Data from JSON: %s\n", jsonData);
        	json = cJSON_Parse( jsonData );
        	if( !json ) 
		{
		    ParodusError( "Error before: [%s]\n", cJSON_GetErrorPtr() );
		    (*response)->u.crud.status = -1;
		    return -1;
		} 
		else
		{
			if(reqMsg->u.crud.dest !=NULL)
			{
			   destVal = strdup(reqMsg->u.crud.dest);
			   ParodusInfo("destVal is %s\n", destVal);

			    if( (destVal != NULL))
			    {
			    	child_ptr = strtok(destVal , "/");
			    	
			    	//Get the 1st object
				obj[0] = strdup( child_ptr );
				ParodusInfo( "parent is %s\n", obj[0] );
				
				while( child_ptr != NULL ) 
				{
				    child_ptr = strtok( NULL, "/" );
				    if( child_ptr != NULL ) 
				    {
				        obj[i] = strdup( child_ptr );
				        ParodusInfo( "child obj[%d]:%s\n", i, obj[i] );
				        i++;
				    }
				}
				
                		objlevel = i;
                		ParodusInfo( " Number of object level %d\n", objlevel );
                		
                		paramArray = cJSON_GetObjectItem( json, obj[2] );
                		
				if( paramArray != NULL ) 
				{
					itemSize = cJSON_GetArraySize( paramArray );
					if( itemSize == 0 ) //TODO check this scenario to remove from paroduscfg
                    			{
                    			
						if(cJSON_GetObjectItem( json, obj[2] )->valuestring)
						{
						    cJSON_DeleteItemFromObject( json, obj[2] );
						    (*response)->u.crud.status = 1;
						     
						}
                    				
                    				ParodusInfo("delete single key value pair from tags..\n");
                    			}
                    			else
                    			{
                    				//top level tags object	
                    				if( strcmp( cJSON_GetObjectItem( json, obj[2] )->string, obj[objlevel - 1] ) == 0 ) 
                    				{
                    					ParodusInfo("Top level tags object delete not supported\n");
                    					cJSON_DeleteItemFromObject(json,"tags");
                    					return -1;
                    				}
                    				else
                    				{
                    				    //deletes internal objects from tags array object.
                    				    
                    				    //to traverse through total number of objects in array
                    				    for( i = 0 ; i < itemSize ; i++ ) 
                    				    {
                    					cJSON* subitem = cJSON_GetArrayItem( paramArray, i );
                    					int subitemSize = cJSON_GetArraySize( subitem );
                    					
                    					//to traverse through subitem in each test element
                    					for( j = 0 ; j < subitemSize ; j++ ) 
                    					{
                    					
                    						ParodusInfo("subitemSize:%d\n", subitemSize);
                    						
                    						if( strcmp( cJSON_GetArrayItem( subitem, j )->string, obj[objlevel - 1] ) == 0 ) 
                    						{
                    							ParodusInfo( " %s : %s \n", cJSON_GetArrayItem( subitem, j )->string, cJSON_GetArrayItem( subitem, j )->valuestring );
                                            				ParodusInfo("Delete: requested object found \n");	
                                            				if(subitemSize == 1)
                                            				{
                                            					ParodusInfo("deleting from paramArray\n");
                                            					cJSON_DeleteItemFromArray(paramArray, i);
                                            				}
                                            				else
                                            				{
                                            					ParodusInfo("deleting from subitem\n");
                                            					cJSON_DeleteItemFromArray(subitem, j);
                                            				}
                                            				found = 1;
                                            				break;
                    						
                    						}
                    						else
                    						{
                    							ParodusError("array object not found\n");
                    						}
                    					
                    					}
                    					if(found)
                    					{
                    						ParodusPrint("break, since array object is found\n");
                    						break;
                    					}
                    					else
                    					{
                    					       ParodusError("Unable to delete requested object\n");
                    					}
                    				     }
                    				   
                    				   }
                    					
                    			     }
                    			}
                    		}
			     }
				
			 }
	}	    
	
	out = cJSON_Print( json );
	ParodusInfo("%s\n",out);
	
	delete_status = writeToJSON(out);
	if(delete_status == 1)
	{
		ParodusInfo("Deleted Data is successfully updated to JSON\n");
	}
	else
	{
		ParodusError("Failed to update deleted data to JSON\n");
	}
            
	cJSON_Delete( json );
	free( out );
	free( jsonData );
       	return 0;

}


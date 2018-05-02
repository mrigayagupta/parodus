/**
 * Copyright 2018 Comcast Cable Communications Management, LLC
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
/**
 * @file parodus_interface.h
 *
 * @description Declares parodus-to-parodus API.
 *
 */
 
#ifndef _PARODUS_INTERFACE_H_
#define _PARODUS_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* None */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/**
 *  Starts the listener thread.
 *
 *  @param thread if not NULL the thread id is returned here, ignored otherwise
 *  @param url    URL to be listening on
 *
 *  @return the result of thread creation
 */
int start_listening_to_spokes(pthread_t *thread, const char *url);

/**
 *  Send to hub parodus.
 * 
 *  @param url  hub parodus URl
 *  @param msg  notification
 *  @param size size of notification
 *
 *  @return whether operation succeeded, or not.
 */
bool send_to_hub(const char *url, const char *msg, size_t size);

/**
 *  Receive from spoke parodus.
 *
 *  @param url spoke parodus URL
 *  @param msg pointer to buffer pointer with notification from spoke parodus.
 */
ssize_t receive_from_spoke(const char *url, char **msg);

/**
 *  Stops and cleans up the listener thread.
 *
 */
void stop_listening_to_spokes(void);

#ifdef __cplusplus
}
#endif


#endif /* _PARODUS_INTERFACE_H_ */

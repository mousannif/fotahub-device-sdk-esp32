/*
 *  Copyright (C) 2022 FotaHub Inc. All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of the FotaHub(R) Device SDK program (https://fotahub.com)
 */
#include "DemoFOTAUpdateWorkflow.h"

#include "DemoProductInfo.h"
#include "DemoFOTAUpdateWorkflowPersistence.h"
#include "DemoFOTAUpdateSelfTest.h"
#include <string.h>

enum DemoFOTAUpdateWorkflowEngine__inevents {
  DemoFOTAUpdateWorkflowEngine_connectionStatusChanged__event,
  DemoFOTAUpdateWorkflowEngine_updateVersionChanged__event,
  DemoFOTAUpdateWorkflowEngine_updateStatusChanged__event
};
typedef enum DemoFOTAUpdateWorkflowEngine__inevents DemoFOTAUpdateWorkflowEngine__inevents_t;

enum DemoFOTAUpdateWorkflowEngine__states {
  DemoFOTAUpdateWorkflowEngine___initializing__state = -2,
  DemoFOTAUpdateWorkflowEngine_idle__state = 0,
  DemoFOTAUpdateWorkflowEngine_connected__state,
  DemoFOTAUpdateWorkflowEngine_downloading__state,
  DemoFOTAUpdateWorkflowEngine_restarting__state,
  DemoFOTAUpdateWorkflowEngine_applying__state,
  DemoFOTAUpdateWorkflowEngine_rollingBack__state
};
typedef enum DemoFOTAUpdateWorkflowEngine__states DemoFOTAUpdateWorkflowEngine__states_t;

struct DemoFOTAUpdateWorkflowEngine__data {
  DemoFOTAUpdateWorkflowEngine__states_t __currentState;
  FirmwareUpdateInfo_t updateInfo;
};
typedef struct DemoFOTAUpdateWorkflowEngine__data DemoFOTAUpdateWorkflowEngine__data_t;

static void DemoFOTAUpdateWorkflowEngine__init(DemoFOTAUpdateWorkflowEngine__data_t *instance, bool postponeInitialExecution);

static bool DemoFOTAUpdateWorkflowEngine__execute(DemoFOTAUpdateWorkflowEngine__data_t *instance, DemoFOTAUpdateWorkflowEngine__inevents_t event, void **arguments);

static inline void DemoFOTAUpdateWorkflowEngine_connected_EntryAction(DemoFOTAUpdateWorkflowEngine__data_t *instance);

static inline void DemoFOTAUpdateWorkflowEngine_restarting_EntryAction(DemoFOTAUpdateWorkflowEngine__data_t *instance);

static DemoFOTAUpdateWorkflowEngine__data_t updateWorkflowEngine;

void fotaUpdateWorkflow_init(void)
{
  DemoFOTAUpdateWorkflowEngine__init(&updateWorkflowEngine, false);
}

void fotahub_onConnectionStatusChanged(ConnectionStatus_t status)
{
  ConnectionStatus_t ___DemoFOTAUpdateWorkflowEngine_connectionStatusChanged_status__arg = status;
  void *___DemoFOTAUpdateWorkflowEngine_connectionStatusChanged__args[1] = { &___DemoFOTAUpdateWorkflowEngine_connectionStatusChanged_status__arg };
  DemoFOTAUpdateWorkflowEngine__execute(&updateWorkflowEngine, DemoFOTAUpdateWorkflowEngine_connectionStatusChanged__event, ___DemoFOTAUpdateWorkflowEngine_connectionStatusChanged__args);
}

void fotahub_onFirmwareUpdateStatusChanged(FOTAUpdateStatus_t status)
{
  FOTAUpdateStatus_t ___DemoFOTAUpdateWorkflowEngine_updateStatusChanged_status__arg = status;
  void *___DemoFOTAUpdateWorkflowEngine_updateStatusChanged__args[1] = { &___DemoFOTAUpdateWorkflowEngine_updateStatusChanged_status__arg };
  DemoFOTAUpdateWorkflowEngine__execute(&updateWorkflowEngine, DemoFOTAUpdateWorkflowEngine_updateStatusChanged__event, ___DemoFOTAUpdateWorkflowEngine_updateStatusChanged__args);
}

void fotaUpdateWorkflow_onFirmwareUpdateVersionChanged(char *updateVersion)
{
  char *___DemoFOTAUpdateWorkflowEngine_updateVersionChanged_updateVersion__arg = updateVersion;
  void *___DemoFOTAUpdateWorkflowEngine_updateVersionChanged__args[1] = { &___DemoFOTAUpdateWorkflowEngine_updateVersionChanged_updateVersion__arg };
  DemoFOTAUpdateWorkflowEngine__execute(&updateWorkflowEngine, DemoFOTAUpdateWorkflowEngine_updateVersionChanged__event, ___DemoFOTAUpdateWorkflowEngine_updateVersionChanged__args);
}

static void DemoFOTAUpdateWorkflowEngine__init(DemoFOTAUpdateWorkflowEngine__data_t *instance, bool postponeInitialExecution)
{
  instance->updateInfo.binaryKind = BINARY_KIND_DEFAULT;
  instance->updateInfo.binaryName = BINARY_NAME_DEFAULT;
  instance->updateInfo.version = NULL;
  instance->updateInfo.partitionIdx = PARTITION_IDX_NONE;
  if (!postponeInitialExecution) 
  {
    instance->__currentState = loadState();
    switch (instance->__currentState)
    {
      case DemoFOTAUpdateWorkflowEngine_connected__state:
      {
        DemoFOTAUpdateWorkflowEngine_connected_EntryAction(instance);
        break;
      }
      case DemoFOTAUpdateWorkflowEngine_restarting__state:
      {
        DemoFOTAUpdateWorkflowEngine_restarting_EntryAction(instance);
        break;
      }
      default: {
        break;
      }
    }
  }
  else
  {
    instance->__currentState = DemoFOTAUpdateWorkflowEngine___initializing__state;
  }
}

static bool DemoFOTAUpdateWorkflowEngine__execute(DemoFOTAUpdateWorkflowEngine__data_t *instance, DemoFOTAUpdateWorkflowEngine__inevents_t event, void **arguments)
{
  bool __outstandingEvent;
  do {
    __outstandingEvent = false;
    switch (instance->__currentState)
    {
      case DemoFOTAUpdateWorkflowEngine___initializing__state:
      {
        /* 
         * enter initial state
         */
        instance->__currentState = loadState();
        switch (instance->__currentState)
        {
          case DemoFOTAUpdateWorkflowEngine_connected__state:
          {
            DemoFOTAUpdateWorkflowEngine_connected_EntryAction(instance);
            break;
          }
          case DemoFOTAUpdateWorkflowEngine_restarting__state:
          {
            DemoFOTAUpdateWorkflowEngine_restarting_EntryAction(instance);
            break;
          }
          default: {
            break;
          }
        }
        __outstandingEvent = true;
        break;
      }
      case DemoFOTAUpdateWorkflowEngine_idle__state:
      {
        switch (event)
        {
          case DemoFOTAUpdateWorkflowEngine_connectionStatusChanged__event:
          {
            if ((*((ConnectionStatus_t *)((arguments[0])))) >= CONNECTION_STATUS_CONNECTIVITY_AVAILABLE) 
            {
              /* 
               * enter target state
               */
              instance->__currentState = DemoFOTAUpdateWorkflowEngine_connected__state;
              DemoFOTAUpdateWorkflowEngine_connected_EntryAction(instance);
            }
            break;
          }
          case DemoFOTAUpdateWorkflowEngine_updateVersionChanged__event:
          {
            /* 
             * transition actions
             */
            printf("Cannot handle firmware over-the-air update request (missing network connection)\r\n");
            
            /* 
             * enter target state
             */
            break;
          }
          default: {
            break;
          }
        }
        break;
      }
      case DemoFOTAUpdateWorkflowEngine_connected__state:
      {
        switch (event)
        {
          case DemoFOTAUpdateWorkflowEngine_connectionStatusChanged__event:
          {
            if ((*((ConnectionStatus_t *)((arguments[0])))) < CONNECTION_STATUS_CONNECTIVITY_AVAILABLE) 
            {
              /* 
               * enter target state
               */
              instance->__currentState = DemoFOTAUpdateWorkflowEngine_idle__state;
            }
            break;
          }
          case DemoFOTAUpdateWorkflowEngine_updateVersionChanged__event:
          {
            if (strcmp((*((char **)((arguments[0])))), DEMO_PRODUCT_FIRMWARE_VERSION) != 0) 
            {
              /* 
               * transition actions
               */
              bool __transitionResult = true;
              DemoFOTAUpdateWorkflowEngine__states_t __targetState = DemoFOTAUpdateWorkflowEngine_downloading__state;
              
              /* 
               * Update internal firmware update info
               */
              instance->updateInfo.version = (*((char **)((arguments[0]))));
              
              /* 
               * Start firmware update download
               */
              if (!fotahub_downloadFirmwareUpdate(&instance->updateInfo)) 
              {
                __targetState = DemoFOTAUpdateWorkflowEngine_connected__state;
                __transitionResult = false;
              }
              
              if (instance->__currentState == DemoFOTAUpdateWorkflowEngine_connected__state) 
              {
                /* 
                 * enter target state
                 */
                instance->__currentState = __targetState;
                switch (__targetState)
                {
                  case DemoFOTAUpdateWorkflowEngine_connected__state:
                  {
                    DemoFOTAUpdateWorkflowEngine_connected_EntryAction(instance);
                    break;
                  }
                  default: {
                    break;
                  }
                }
              }
              
              if (!__transitionResult) 
              {
                return false;
              }
              break;
            }
            if (strcmp((*((char **)((arguments[0])))), DEMO_PRODUCT_FIRMWARE_VERSION) == 0) 
            {
              /* 
               * transition actions
               */
              printf("Ignoring firmware over-the-air update request to version %s as this version is already running\r\n", (*((char **)((arguments[0])))));
              
              /* 
               * enter target state
               */
              DemoFOTAUpdateWorkflowEngine_connected_EntryAction(instance);
            }
            break;
          }
          default: {
            break;
          }
        }
        break;
      }
      case DemoFOTAUpdateWorkflowEngine_downloading__state:
      {
        switch (event)
        {
          case DemoFOTAUpdateWorkflowEngine_updateStatusChanged__event:
          {
            if ((*((FOTAUpdateStatus_t *)((arguments[0])))) == FOTA_UPDATE_STATUS_DOWNLOAD_SUCCEEDED) 
            {
              /* 
               * transition actions
               */
              bool __transitionResult = true;
              DemoFOTAUpdateWorkflowEngine__states_t __targetState = DemoFOTAUpdateWorkflowEngine_restarting__state;
              
              /* 
               * Persist FOTA update state and apply downloaded firmware update
               */
              saveState(DemoFOTAUpdateWorkflowEngine_applying__state);
              if (!fotahub_applyFirmwareUpdate(&instance->updateInfo)) 
              {
                saveState(DemoFOTAUpdateWorkflowEngine_idle__state);
                __targetState = DemoFOTAUpdateWorkflowEngine_connected__state;
                __transitionResult = false;
              }
              
              if (instance->__currentState == DemoFOTAUpdateWorkflowEngine_downloading__state) 
              {
                /* 
                 * enter target state
                 */
                instance->__currentState = __targetState;
                switch (__targetState)
                {
                  case DemoFOTAUpdateWorkflowEngine_restarting__state:
                  {
                    DemoFOTAUpdateWorkflowEngine_restarting_EntryAction(instance);
                    break;
                  }
                  case DemoFOTAUpdateWorkflowEngine_connected__state:
                  {
                    DemoFOTAUpdateWorkflowEngine_connected_EntryAction(instance);
                    break;
                  }
                  default: {
                    break;
                  }
                }
              }
              
              if (!__transitionResult) 
              {
                return false;
              }
              break;
            }
            if ((*((FOTAUpdateStatus_t *)((arguments[0])))) == FOTA_UPDATE_STATUS_CONNECTIVITY_UNAVAILABLE) 
            {
              /* 
               * enter target state
               */
              instance->__currentState = DemoFOTAUpdateWorkflowEngine_idle__state;
              break;
            }
            if ((*((FOTAUpdateStatus_t *)((arguments[0])))) != FOTA_UPDATE_STATUS_DOWNLOAD_SUCCEEDED || (*((FOTAUpdateStatus_t *)((arguments[0])))) != FOTA_UPDATE_STATUS_CONNECTIVITY_UNAVAILABLE) 
            {
              /* 
               * enter target state
               */
              instance->__currentState = DemoFOTAUpdateWorkflowEngine_connected__state;
              DemoFOTAUpdateWorkflowEngine_connected_EntryAction(instance);
            }
            break;
          }
          default: {
            break;
          }
        }
        break;
      }
      case DemoFOTAUpdateWorkflowEngine_restarting__state:
      {
        switch (event)
        {
          default: {
            break;
          }
        }
        break;
      }
      case DemoFOTAUpdateWorkflowEngine_applying__state:
      {
        switch (event)
        {
          case DemoFOTAUpdateWorkflowEngine_connectionStatusChanged__event:
          {
            if ((*((ConnectionStatus_t *)((arguments[0])))) >= CONNECTION_STATUS_CONNECTIVITY_AVAILABLE) 
            {
              /* 
               * transition actions
               */
              validateFirmwareUpdateApplying();
              
              if (instance->__currentState == DemoFOTAUpdateWorkflowEngine_applying__state) 
              {
                /* 
                 * enter target state
                 */
              }
            }
            break;
          }
          case DemoFOTAUpdateWorkflowEngine_updateStatusChanged__event:
          {
            if ((*((FOTAUpdateStatus_t *)((arguments[0])))) == FOTA_UPDATE_STATUS_APPLICATION_SUCCEEDED) 
            {
              /* 
               * transition actions
               */
              bool __transitionResult = true;
              
              /* 
               * Reset persisted FOTA update state
               */
              saveState(DemoFOTAUpdateWorkflowEngine_idle__state);
              if (!fotahub_confirmFirmwareUpdate(&instance->updateInfo)) 
              {
                __transitionResult = false;
              }
              
              if (instance->__currentState == DemoFOTAUpdateWorkflowEngine_applying__state) 
              {
                /* 
                 * enter target state
                 */
                instance->__currentState = DemoFOTAUpdateWorkflowEngine_connected__state;
                DemoFOTAUpdateWorkflowEngine_connected_EntryAction(instance);
              }
              
              if (!__transitionResult) 
              {
                return false;
              }
              break;
            }
            if ((*((FOTAUpdateStatus_t *)((arguments[0])))) == FOTA_UPDATE_STATUS_APPLICATION_FAILED) 
            {
              /* 
               * transition actions
               */
              bool __transitionResult = true;
              DemoFOTAUpdateWorkflowEngine__states_t __targetState = DemoFOTAUpdateWorkflowEngine_restarting__state;
              
              /* 
               * Persist FOTA update state and roll back applied firmware update
               */
              saveState(DemoFOTAUpdateWorkflowEngine_rollingBack__state);
              if (!fotahub_rollBackFirmwareUpdate(&instance->updateInfo)) 
              {
                saveState(DemoFOTAUpdateWorkflowEngine_idle__state);
                __targetState = DemoFOTAUpdateWorkflowEngine_connected__state;
                __transitionResult = false;
              }
              
              if (instance->__currentState == DemoFOTAUpdateWorkflowEngine_applying__state) 
              {
                /* 
                 * enter target state
                 */
                instance->__currentState = __targetState;
                switch (__targetState)
                {
                  case DemoFOTAUpdateWorkflowEngine_restarting__state:
                  {
                    DemoFOTAUpdateWorkflowEngine_restarting_EntryAction(instance);
                    break;
                  }
                  case DemoFOTAUpdateWorkflowEngine_connected__state:
                  {
                    DemoFOTAUpdateWorkflowEngine_connected_EntryAction(instance);
                    break;
                  }
                  default: {
                    break;
                  }
                }
              }
              
              if (!__transitionResult) 
              {
                return false;
              }
            }
            break;
          }
          default: {
            break;
          }
        }
        break;
      }
      case DemoFOTAUpdateWorkflowEngine_rollingBack__state:
      {
        switch (event)
        {
          case DemoFOTAUpdateWorkflowEngine_connectionStatusChanged__event:
          {
            if ((*((ConnectionStatus_t *)((arguments[0])))) >= CONNECTION_STATUS_CONNECTIVITY_AVAILABLE) 
            {
              /* 
               * transition actions
               */
              validateFirmwareUpdateRollingback();
              
              if (instance->__currentState == DemoFOTAUpdateWorkflowEngine_rollingBack__state) 
              {
                /* 
                 * enter target state
                 */
              }
            }
            break;
          }
          case DemoFOTAUpdateWorkflowEngine_updateStatusChanged__event:
          {
            /* 
             * transition actions
             */
            /* 
             * Reset persisted FOTA update state
             */
            saveState(DemoFOTAUpdateWorkflowEngine_idle__state);
            
            /* 
             * enter target state
             */
            instance->__currentState = DemoFOTAUpdateWorkflowEngine_connected__state;
            DemoFOTAUpdateWorkflowEngine_connected_EntryAction(instance);
            break;
          }
          default: {
            break;
          }
        }
        break;
      }
      default: {
        break;
      }
    }
  }
  while (__outstandingEvent);
  return true;
}

static inline void DemoFOTAUpdateWorkflowEngine_connected_EntryAction(DemoFOTAUpdateWorkflowEngine__data_t *instance)
{
  printf("Ready for firmware over-the-air update\r\n");
}

static inline void DemoFOTAUpdateWorkflowEngine_restarting_EntryAction(DemoFOTAUpdateWorkflowEngine__data_t *instance)
{
  printf("Waiting for restart\r\n");
}

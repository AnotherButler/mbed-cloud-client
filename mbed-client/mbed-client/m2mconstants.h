/*
 * Copyright (c) 2015-2020 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef M2MCONSTANTS_H
#define M2MCONSTANTS_H

/** \file m2mconstants.h
 * \brief File defining all the constants used across mbed-client.
 */

#include "m2mconfig.h"
#include <stdint.h>

const int MAX_VALUE_LENGTH = 256;
const int BUFFER_LENGTH = 1152;

const uint8_t MINIMUM_REGISTRATION_TIME = 60; //in seconds
const uint8_t ONE_SECOND_TIMER = 1;
const uint8_t MAX_ALLOWED_STRING_LENGTH = 64;
const uint8_t MAX_ALLOWED_ERROR_STRING_LENGTH = 64;
const uint16_t CONFIG_BOOLEAN_ITEM_SIZE = 4;
const uint16_t MAX_FIRMWARE_PACKAGE_URI_PATH_LEN = 255;
const uint32_t HANDSHAKE_TIMEOUT_MSECS = (30 * 60 * 1000);

// XXX:
//                               <name></><inst-id></><res-name>
//#define MAX_OBJECT_INSTANCE_NAME (255 + 1 + 5 + 1 + 255 + 1 + 5)
//                           <name></><inst-id></><inst-id><zero-terminator>
#define MAX_OBJECT_PATH_NAME (255 + 1 + 5 + 1 + 5 + 1)

// values per: draft-ietf-core-observe-16
// OMA LWM2M CR ref.
#define START_OBSERVATION 0
#define STOP_OBSERVATION 1

#define COAP "coap://"
#define COAPS  "coaps://"
#define BOOTSTRAP_URI "bs"
// PUT attributes to be checked from server
#define EQUAL  "="
#define AMP  "&"
#define PMIN  "pmin"
#define PMAX  "pmax"
#define GT  "gt"
#define LT  "lt"
#define ST_SIZE  "st"
#define CANCEL  "cancel"

// Different query parameters
#define QUERY_PARAM_IEP "iep"
#define QUERY_PARAM_EST "est"
#define QUERY_PARAM_AID "aid"

// Different query parameter values
#define QUERY_VALUE_TRUE "true"

//LWM2MOBJECT NAME/ID
#define M2M_SECURITY_ID  "0"
#define M2M_SERVER_ID  "1"
#define M2M_ACCESS_CONTROL_ID  "2"
#define M2M_DEVICE_ID  "3"
#define M2M_CONNECTIVITY_MONITOR_ID  "4"
#define M2M_FIRMWARE_ID  "5"
#define M2M_LOCATION_ID  "6"
#define M2M_CONNECTIVITY_STATISTICS_ID  "7"
#define RESERVED_ID  "8"

//OMA RESOURCE TYPE
#define OMA_RESOURCE_TYPE  "" //oma.lwm2m

//DEVICE RESOURCES
#define DEVICE_MANUFACTURER  "0"
#define DEVICE_DEVICE_TYPE  "17"
#define DEVICE_MODEL_NUMBER  "1"
#define DEVICE_SERIAL_NUMBER  "2"
#define DEVICE_HARDWARE_VERSION  "18"
#define DEVICE_FIRMWARE_VERSION  "3"
#define DEVICE_SOFTWARE_VERSION  "19"
#define DEVICE_REBOOT  "4"
#define DEVICE_FACTORY_RESET  "5"
#define DEVICE_AVAILABLE_POWER_SOURCES  "6"
#define DEVICE_POWER_SOURCE_VOLTAGE  "7"
#define DEVICE_POWER_SOURCE_CURRENT  "8"
#define DEVICE_BATTERY_LEVEL  "9"
#define DEVICE_BATTERY_STATUS  "20"
#define DEVICE_MEMORY_FREE  "10"
#define DEVICE_MEMORY_TOTAL  "21"
#define DEVICE_ERROR_CODE  "11"
#define DEVICE_RESET_ERROR_CODE  "12"
#define DEVICE_CURRENT_TIME  "13"
#define DEVICE_UTC_OFFSET  "14"
#define DEVICE_TIMEZONE  "15"
#define DEVICE_SUPPORTED_BINDING_MODE  "16"
#define BINDING_MODE_UDP  "U"
#define BINDING_MODE_UDP_QUEUE  "UQ"
#define BINDING_MODE_SMS  "S"
#define BINDING_MODE_SMS_QUEUE  "SQ"
#define ERROR_CODE_VALUE  "0"

//SECURITY RESOURCES
#define SECURITY_M2M_SERVER_URI  "0"
#define SECURITY_BOOTSTRAP_SERVER  "1"
#define SECURITY_SECURITY_MODE  "2"
#define SECURITY_PUBLIC_KEY  "3"
#define SECURITY_SERVER_PUBLIC_KEY  "4"
#define SECURITY_SECRET_KEY  "5"
#define SECURITY_SMS_SECURITY_MODE  "6"
#define SECURITY_SMS_BINDING_KEY  "7"
#define SECURITY_SMS_BINDING_SECRET_KEY  "8"
#define SECURITY_M2M_SERVER_SMS_NUMBER  "9"
#define SECURITY_SHORT_SERVER_ID  "10"
#define SECURITY_CLIENT_HOLD_OFF_TIME  "11"

#define SECURITY_OPEN_CERTIFICATE_CHAIN  "12"
#define SECURITY_CLOSE_CERTIFICATE_CHAIN  "13"
#define SECURITY_READ_CERTIFICATE_CHAIN  "14"

//SERVER RESOURCES
#define SERVER_PATH_PREFIX "1/0/"
#define SERVER_SHORT_SERVER_ID  "0"
#define SERVER_LIFETIME  "1"
#define SERVER_DEFAULT_MIN_PERIOD  "2"
#define SERVER_DEFAULT_MAX_PERIOD  "3"
#define SERVER_DISABLE  "4"
#define SERVER_DISABLE_TIMEOUT  "5"
#define SERVER_NOTIFICATION_STORAGE  "6"
#define SERVER_BINDING  "7"
#define SERVER_REGISTRATION_UPDATE  "8"
#define SERVER_LIFETIME_PATH SERVER_PATH_PREFIX SERVER_LIFETIME

//FIRMWARE RESOURCES
#define FIRMWARE_PATH_PREFIX "5/0/"
#define FIRMWARE_PACKAGE  "0"
#define FIRMWARE_PACKAGE_URI  "1"
#define FIRMWARE_UPDATE  "2"
#define FIRMWARE_STATE  "3"
#define FIRMWARE_UPDATE_SUPPORTED_OBJECTS  "4"
#define FIRMWARE_UPDATE_RESULT  "5"
#define FIRMWARE_PACKAGE_NAME  "6"
#define FIRMWARE_PACKAGE_VERSION  "7"
#define FIRMWARE_PACKAGE_URI_PATH FIRMWARE_PATH_PREFIX FIRMWARE_PACKAGE_URI

// Error Strings

#define ERROR_REASON_1 "No security object found for Bootstrap"
#define ERROR_REASON_3 "Bootstrap feature is disabled"
#define ERROR_REASON_4 "No security object found for Registration"
#define ERROR_REASON_5 "Registration not allowed for now, try later"
#define ERROR_REASON_6 "Unregistration not allowed for now, try later"
#define ERROR_REASON_7 "Client is not connected, cannot send data now"
#define ERROR_REASON_8 "LWM2M server rejected client registration"
#define ERROR_REASON_9 "Client in reconnection mode %s"
#define ERROR_REASON_10 "Client cannot connect anymore %s"
#define ERROR_REASON_13 "LWM2M server URL is not correctly formed"
#define ERROR_REASON_14 "LWM2M server address is not set correctly in client"
#define ERROR_REASON_15 "Failed to do full registration because of missing parameters in registration"
#define ERROR_REASON_16 "Cannot unregister as client is not registered"
#define ERROR_REASON_17 "Incoming CoAP message parsing failed"
#define ERROR_REASON_18 "Sending reg-update failed as lifetime is less than 60 sec"
#define ERROR_REASON_19 "LWM2M server URL is not correctly formed"
#ifndef MBED_CLIENT_DISABLE_BOOTSTRAP_FEATURE
#define ERROR_REASON_2 "Bootstrap not allowed for now, try later"
#define ERROR_REASON_11 "Bootstrap server URL is not correctly formed"
#define ERROR_REASON_12 "Bootstrap resource is not correctly formed"
#define ERROR_REASON_20 "BS PUT fails :%s"
#define ERROR_REASON_21 "BS DEL fails :%s"
#define ERROR_REASON_22 "BS FIN fails :%s"
#define ERROR_REASON_23 "Bootstrap SecureConnection failed"
#define ERROR_REASON_26 "BS FIN fails: missing EST iep param"
#endif
#define ERROR_REASON_24 "LWM2M server rejected client unregistration (not-found)"
#define ERROR_REASON_25 "Failed to allocate registration message"
#define ERROR_REASON_27 "Call already in progress"
#define ERROR_REASON_28 "Message sending timed out"
#define ERROR_REASON_29 "Call not allowed for now, wrong state"

#define COAP_ERROR_REASON_1 "bad-request"
#define COAP_ERROR_REASON_2 "bad-option"
#define COAP_ERROR_REASON_3 "request-entity-incomplete"
#define COAP_ERROR_REASON_4 "precondition-failed"
#define COAP_ERROR_REASON_5 "request-entity-too-large"
#define COAP_ERROR_REASON_6 "unsupported-content-format"
#define COAP_ERROR_REASON_7 "response-unauthorized"
#define COAP_ERROR_REASON_8 "response-forbidden"
#define COAP_ERROR_REASON_9 "not-acceptable"
#define COAP_ERROR_REASON_10 "not-found"
#define COAP_ERROR_REASON_11 "method-not-allowed"
#define COAP_ERROR_REASON_12 "message-sending-failed"
#define COAP_ERROR_REASON_13 "service-unavailable"
#define COAP_ERROR_REASON_14 "internal-server-error"
#define COAP_ERROR_REASON_15 "bad-gateway"
#define COAP_ERROR_REASON_16 "gateway-timeout"
#define COAP_ERROR_REASON_17 "proxying-not-supported"
#define COAP_NO_ERROR        "no-error"

#define ERROR_SECURE_CONNECTION "SecureConnectionFailed"
#define ERROR_DNS               "DnsResolvingFailed"
#define ERROR_NETWORK           "NetworkError"
#define ERROR_NO                "No error"
#define ERROR_NO_MEMORY         "Memory allocation failed"
#define ERROR_FAILED_TO_READ_CREDENTIALS "Failed to read credentials"

#define MAX_RECONNECT_TIMEOUT        604800
#define RECONNECT_INCREMENT_FACTOR   2

#define AUTO_OBS_TOKEN_MIN 1
#define AUTO_OBS_TOKEN_MAX 1023

#define RESPONSE_RANDOM_FACTOR  1.5   /**< Resending random factor, value is specified in IETF CoAP specification */

#define DEFAULT_M2MVERSION "1.0"

// TLV serializer / deserializer
const uint8_t TYPE_RESOURCE = 0xC0;
const uint8_t TYPE_MULTIPLE_RESOURCE = 0x80;
const uint8_t TYPE_RESOURCE_INSTANCE = 0x40;
const uint8_t TYPE_OBJECT_INSTANCE = 0x0;

const uint8_t ID8 = 0x0;
const uint8_t ID16 = 0x20;

const uint8_t LENGTH8 = 0x08;
const uint8_t LENGTH16 = 0x10;
const uint8_t LENGTH24 = 0x18;

const uint8_t COAP_CONTENT_OMA_PLAIN_TEXT_TYPE = 0;
const uint8_t COAP_CONTENT_OMA_TLV_TYPE_OLD = 99;
const uint16_t COAP_CONTENT_OMA_TLV_TYPE = 11542;
const uint16_t COAP_CONTENT_OMA_JSON_TYPE = 11543;
const uint8_t COAP_CONTENT_OMA_OPAQUE_TYPE = 42;

#endif // M2MCONSTANTS_H

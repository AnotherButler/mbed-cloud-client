/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
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
#include "mbed-client/m2mconstants.h"
#include "mbed-client/m2mresource.h"
#include "mbed-client/m2mobservationhandler.h"
#include "include/m2mreporthandler.h"
#include "include/m2mtlvserializer.h"
#include "include/m2mtlvdeserializer.h"
#include "mbed-trace/mbed_trace.h"

#include <stdlib.h>

#define TRACE_GROUP "mClt"

M2MResource::M2MResource(M2MObjectInstance &parent,
                         const String &resource_name,
                         M2MBase::Mode resource_mode,
                         const String &resource_type,
                         M2MBase::DataType type,
                         const uint8_t *value,
                         const uint8_t value_length,
                         char *path,
                         bool multiple_instance,
                         bool external_blockwise_store)
: M2MResourceBase(resource_name, resource_mode, resource_type, type, value, value_length,
                      path, external_blockwise_store, multiple_instance),
  _parent(parent)
#ifndef DISABLE_DELAYED_RESPONSE
  ,_delayed_token(NULL),
  _delayed_token_len(0),
  _delayed_response(false)
#endif
{
    M2MBase::set_base_type(M2MBase::Resource);
    M2MBase::set_operation(M2MBase::GET_ALLOWED);
    M2MBase::set_observable(false);
    if (multiple_instance) {
        M2MBase::set_coap_content_type(COAP_CONTENT_OMA_TLV_TYPE);
    }

}

M2MResource::M2MResource(M2MObjectInstance &parent,
                         const lwm2m_parameters_s* s,
                          M2MBase::DataType type)
: M2MResourceBase(s, type),
  _parent(parent)
#ifndef DISABLE_DELAYED_RESPONSE
  ,_delayed_token(NULL),
  _delayed_token_len(0),
  _delayed_response(false)
#endif
{
    // verify, that the client has hardcoded proper type to the structure
    assert(base_type() == M2MBase::Resource);
}

M2MResource::M2MResource(M2MObjectInstance &parent,
                         const String &resource_name,
                         M2MBase::Mode resource_mode,
                         const String &resource_type,
                         M2MBase::DataType type,
                         bool observable,
                         char *path,
                         bool multiple_instance,
                         bool external_blockwise_store)
: M2MResourceBase(resource_name, resource_mode, resource_type, type,
                      path,
                      external_blockwise_store,multiple_instance),
  _parent(parent)
#ifndef DISABLE_DELAYED_RESPONSE
  ,_delayed_token(NULL),
  _delayed_token_len(0),
  _delayed_response(false)
#endif
{
    M2MBase::set_base_type(M2MBase::Resource);
    M2MBase::set_operation(M2MBase::GET_PUT_ALLOWED);
    M2MBase::set_observable(observable);
    if (multiple_instance) {
        M2MBase::set_coap_content_type(COAP_CONTENT_OMA_TLV_TYPE);
    }
}


M2MResource::~M2MResource()
{
    if(!_resource_instance_list.empty()) {
        M2MResourceInstance* res = NULL;
        M2MResourceInstanceList::const_iterator it;
        it = _resource_instance_list.begin();
        for (; it!=_resource_instance_list.end(); it++ ) {
            //Free allocated memory for resources.
            res = *it;
            delete res;
        }
        _resource_instance_list.clear();
    }
#ifndef DISABLE_DELAYED_RESPONSE
    free(_delayed_token);
#endif

    free_resources();
}

bool M2MResource::supports_multiple_instances() const
{
    M2MBase::lwm2m_parameters_s* param = M2MBase::get_lwm2m_parameters();
    return param->multiple_instance;
}

#ifndef DISABLE_DELAYED_RESPONSE
void M2MResource::set_delayed_response(bool delayed_response)
{
    _delayed_response = delayed_response;
}

bool M2MResource::send_delayed_post_response(sn_coap_msg_code_e code)
{
    bool success = false;
    if(_delayed_response) {
        success = true;
        // At least on some unit tests the resource object is not fully constructed, which would
        // cause issues if the observation_handler is NULL. So do the check before dereferencing pointer.
        M2MObservationHandler* obs = observation_handler();
        if (obs) {
            obs->send_delayed_response(this, code);
        }
    }
    return success;
}

void M2MResource::get_delayed_token(uint8_t *&token, uint8_t &token_length)
{
    token_length = 0;
    if(token) {
        free(token);
        token = NULL;
    }
    if(_delayed_token && _delayed_token_len > 0) {
        token = alloc_copy(_delayed_token, _delayed_token_len);
        if(token) {
            token_length = _delayed_token_len;
        }
    }
}
#endif

bool M2MResource::remove_resource_instance(uint16_t inst_id)
{
    tr_debug("M2MResource::remove_resource(inst_id %d)", inst_id);
    bool success = false;
    if(!_resource_instance_list.empty()) {
        M2MResourceInstance* res = NULL;
        M2MResourceInstanceList::const_iterator it;
        it = _resource_instance_list.begin();
        int pos = 0;
        for ( ; it != _resource_instance_list.end(); it++, pos++ ) {
            if(((*it)->instance_id() == inst_id)) {
                // Resource found and deleted.
                res = *it;
                delete res;
                _resource_instance_list.erase(pos);
                set_changed();
                success = true;
                break;
            }
        }
    }
    return success;
}

M2MResourceInstance* M2MResource::resource_instance(uint16_t inst_id) const
{
    tr_debug("M2MResource::resource(resource_name inst_id %d)", inst_id);
    M2MResourceInstance *res = NULL;
    if(!_resource_instance_list.empty()) {
        M2MResourceInstanceList::const_iterator it;
        it = _resource_instance_list.begin();
        for ( ; it != _resource_instance_list.end(); it++ ) {
            if(((*it)->instance_id() == inst_id)) {
                // Resource found.
                res = *it;
                break;
            }
        }
    }
    return res;
}

const M2MResourceInstanceList& M2MResource::resource_instances() const
{
    return _resource_instance_list;
}

uint16_t M2MResource::resource_instance_count() const
{
    return (uint16_t)_resource_instance_list.size();
}

#ifndef DISABLE_DELAYED_RESPONSE
bool M2MResource::delayed_response() const
{
    return _delayed_response;
}
#endif

M2MObservationHandler* M2MResource::observation_handler() const
{
    const M2MObjectInstance& parent_object_instance = get_parent_object_instance();

    // XXX: need to check the flag too
    return parent_object_instance.observation_handler();
}

void M2MResource::set_observation_handler(M2MObservationHandler *handler)
{
    M2MObjectInstance& parent_object_instance = get_parent_object_instance();

    // XXX: need to set the flag too
    parent_object_instance.set_observation_handler(handler);
}
#if defined (MBED_CONF_MBED_CLIENT_ENABLE_OBSERVATION_PARAMETERS) && (MBED_CONF_MBED_CLIENT_ENABLE_OBSERVATION_PARAMETERS == 1)
bool M2MResource::handle_observation_attribute(const char *query)
{
    tr_debug("M2MResource::handle_observation_attribute - is_under_observation(%d)", is_under_observation());
    bool success = false;
    M2MReportHandler *handler = M2MBase::report_handler();
    if (!handler) {
        handler = M2MBase::create_report_handler();
    }

    if (handler) {
        if (resource_instance_type() == M2MResourceBase::FLOAT) {
            handler->init_float_values(get_value_float());
        } else if (resource_instance_type() == M2MResourceBase::INTEGER) {
            handler->init_int_values(get_value_int());
        }
        success = handler->parse_notification_attribute(query,
                M2MBase::base_type(), resource_instance_type());
        if (success) {
            if (is_under_observation()) {
                handler->set_under_observation(true);
            }
            else {
                handler->start_timers();
            }
        }
        else {
            handler->set_default_values();
        }

        if (success) {
            if(!_resource_instance_list.empty()) {
                M2MResourceInstanceList::const_iterator it;
                it = _resource_instance_list.begin();
                for ( ; it != _resource_instance_list.end(); it++ ) {
                    M2MReportHandler *report_handler = (*it)->report_handler();
                    if(report_handler && is_under_observation()) {
                        report_handler->set_notification_trigger();
                    }
                }
            }
        }
    }
    return success;
}
#endif
void M2MResource::add_observation_level(M2MBase::Observation observation_level)
{
    M2MBase::add_observation_level(observation_level);
    if(!_resource_instance_list.empty()) {
        M2MResourceInstanceList::const_iterator inst;
        inst = _resource_instance_list.begin();
        for ( ; inst != _resource_instance_list.end(); inst++ ) {
            (*inst)->add_observation_level(observation_level);
        }
    }
}

void M2MResource::remove_observation_level(M2MBase::Observation observation_level)
{
    M2MBase::remove_observation_level(observation_level);
    if(!_resource_instance_list.empty()) {
        M2MResourceInstanceList::const_iterator inst;
        inst = _resource_instance_list.begin();
        for ( ; inst != _resource_instance_list.end(); inst++ ) {
            (*inst)->remove_observation_level(observation_level);
        }
    }
}

void M2MResource::add_resource_instance(M2MResourceInstance *res)
{
    tr_debug("M2MResource::add_resource_instance()");
    if(res) {
        _resource_instance_list.push_back(res);
        set_changed();
    }
}

sn_coap_hdr_s* M2MResource::handle_get_request(nsdl_s *nsdl,
                                               sn_coap_hdr_s *received_coap_header,
                                               M2MObservationHandler *observation_handler)
{
    tr_info("M2MResource::handle_get_request()");
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CONTENT;
    sn_coap_hdr_s * coap_response = NULL;
    if(supports_multiple_instances()) {
        coap_response = sn_nsdl_build_response(nsdl,
                                               received_coap_header,
                                               msg_code);
        if(received_coap_header) {
            // process the GET if we have registered a callback for it
            if ((operation() & M2MBase::GET_ALLOWED) != 0) {
                if(coap_response) {
                    bool content_type_present = false;
                    bool is_content_type_supported = true;

                    if (received_coap_header->options_list_ptr &&
                            received_coap_header->options_list_ptr->accept != COAP_CT_NONE) {
                        content_type_present = true;
                        coap_response->content_format = received_coap_header->options_list_ptr->accept;
                    }

                    // Check if preferred content type is supported
                    if (content_type_present) {
                        if (coap_response->content_format != COAP_CONTENT_OMA_TLV_TYPE_OLD &&
                            coap_response->content_format != COAP_CONTENT_OMA_TLV_TYPE) {
                            is_content_type_supported = false;
                        }
                    }

                    if (is_content_type_supported) {
                        if(!content_type_present &&
                           (M2MBase::coap_content_type() == COAP_CONTENT_OMA_TLV_TYPE ||
                            M2MBase::coap_content_type() == COAP_CONTENT_OMA_TLV_TYPE_OLD)) {
                            coap_response->content_format = sn_coap_content_format_e(M2MBase::coap_content_type());
                        }

                        tr_debug("M2MResource::handle_get_request() - Request Content-type: %d", coap_response->content_format);

                        uint8_t *data = NULL;
                        uint32_t data_length = 0;
                        // fill in the CoAP response payload
                        if(COAP_CONTENT_OMA_TLV_TYPE == coap_response->content_format ||
                           COAP_CONTENT_OMA_TLV_TYPE_OLD == coap_response->content_format) {
                            set_coap_content_type(coap_response->content_format);
                            data = M2MTLVSerializer::serialize(this, data_length);
                        }

                        coap_response->payload_len = data_length;
                        coap_response->payload_ptr = data;

                        coap_response->options_list_ptr = sn_nsdl_alloc_options_list(nsdl, coap_response);
                        if (coap_response->options_list_ptr) {
                            coap_response->options_list_ptr->max_age = max_age();
                        }

                        if(received_coap_header->options_list_ptr) {
                            if(received_coap_header->options_list_ptr->observe != -1) {
                                handle_observation(nsdl, *received_coap_header, *coap_response, observation_handler, msg_code);
                            }
                        }
                    } else {
                        tr_error("M2MResource::handle_get_request() - Content-Type %d not supported", coap_response->content_format);
                        msg_code = COAP_MSG_CODE_RESPONSE_NOT_ACCEPTABLE;
                    }
                }
            } else {
                tr_error("M2MResource::handle_get_request - Return COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
                // Operation is not allowed.
                msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
            }
        }
        if(coap_response) {
            coap_response->msg_code = msg_code;
        }
    } else {
        coap_response = M2MResourceBase::handle_get_request(nsdl,
                            received_coap_header,
                            observation_handler);
    }
    return coap_response;
}

sn_coap_hdr_s* M2MResource::handle_put_request(nsdl_s *nsdl,
                                               sn_coap_hdr_s *received_coap_header,
                                               M2MObservationHandler *observation_handler,
                                               bool &execute_value_updated)
{
    tr_info("M2MResource::handle_put_request()");
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CHANGED; // 2.04
    sn_coap_hdr_s * coap_response = NULL;
    if (supports_multiple_instances() ||
        (received_coap_header->content_format == COAP_CONTENT_OMA_TLV_TYPE ||
         received_coap_header->content_format == COAP_CONTENT_OMA_TLV_TYPE_OLD)) {
        coap_response = sn_nsdl_build_response(nsdl,
                                               received_coap_header,
                                               msg_code);
        // process the PUT if we have registered a callback for it
        if (received_coap_header) {
            uint16_t coap_content_type = 0;
            bool content_type_present = false;
            if (received_coap_header->content_format != COAP_CT_NONE && coap_response) {
                content_type_present = true;
                coap_content_type = received_coap_header->content_format;
            }
            if (received_coap_header->options_list_ptr &&
                received_coap_header->options_list_ptr->uri_query_ptr) {
                char *query = (char*)alloc_string_copy(received_coap_header->options_list_ptr->uri_query_ptr,
                                                        received_coap_header->options_list_ptr->uri_query_len);
                if (query){
                    msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
                    tr_info("M2MResource::handle_put_request() - query %s", query);
                    // if anything was updated, re-initialize the stored notification attributes
#if defined (MBED_CONF_MBED_CLIENT_ENABLE_OBSERVATION_PARAMETERS) && (MBED_CONF_MBED_CLIENT_ENABLE_OBSERVATION_PARAMETERS == 1)
                    if (!handle_observation_attribute(query)){
                        tr_debug("M2MResource::handle_put_request() - Invalid query");
                        msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST; // 4.00
                    }
#else
                    msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST; // 4.00
#endif
                    free(query);
                }
            } else if ((operation() & M2MBase::PUT_ALLOWED) != 0) {
                if (!content_type_present &&
                   (M2MBase::coap_content_type() == COAP_CONTENT_OMA_TLV_TYPE ||
                    M2MBase::coap_content_type() == COAP_CONTENT_OMA_TLV_TYPE_OLD)) {
                    coap_content_type = COAP_CONTENT_OMA_TLV_TYPE;
                }

                tr_debug("M2MResource::handle_put_request() - Request Content-type: %d", coap_content_type);

                if (COAP_CONTENT_OMA_TLV_TYPE == coap_content_type || COAP_CONTENT_OMA_TLV_TYPE_OLD == coap_content_type) {
                    set_coap_content_type(coap_content_type);
                    M2MTLVDeserializer::Error error = M2MTLVDeserializer::None;
                    if (supports_multiple_instances()) {
                        error = M2MTLVDeserializer::deserialize_resource_instances(received_coap_header->payload_ptr,
                                                                         received_coap_header->payload_len,
                                                                         *this,
                                                                         M2MTLVDeserializer::Put);
                    } else {
                        if ((strcmp(uri_path(), FIRMWARE_PACKAGE_URI_PATH) == 0) && received_coap_header->payload_len > MAX_FIRMWARE_PACKAGE_URI_PATH_LEN) {
                            // Firmware object uri path is limited to be max MAX_FIRMWARE_PACKAGE_URI_PATH_LEN bytes
                            error = M2MTLVDeserializer::NotAccepted;
                        } else {
                            error = M2MTLVDeserializer::deserialize_resource(received_coap_header->payload_ptr,
                                                                            received_coap_header->payload_len,
                                                                            *this,
                                                                            M2MTLVDeserializer::Put);
                        }
                    }
                    switch (error) {
                        case M2MTLVDeserializer::None:
                            if (observation_handler) {
                                String value = "";
                                if (received_coap_header->uri_path_ptr != NULL &&
                                    received_coap_header->uri_path_len > 0) {

                                    value.append_raw((char*)received_coap_header->uri_path_ptr,received_coap_header->uri_path_len);
                                }
                                execute_value_updated = true;
                            }
                            msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
                            break;
                        case M2MTLVDeserializer::NotFound:
                            msg_code = COAP_MSG_CODE_RESPONSE_NOT_FOUND;
                            break;
                        case M2MTLVDeserializer::NotAllowed:
                            msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
                            break;
                        case M2MTLVDeserializer::NotValid:
                            msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
                            break;
                        case M2MTLVDeserializer::OutOfMemory:
                            msg_code = COAP_MSG_CODE_RESPONSE_REQUEST_ENTITY_TOO_LARGE;
                            break;
                        case M2MTLVDeserializer::NotAccepted:
                            msg_code = COAP_MSG_CODE_RESPONSE_NOT_ACCEPTABLE;
                            break;
                    }
                } else {
                    msg_code =COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT;
                }
            } else {
                // Operation is not allowed.
                tr_error("M2MResource::handle_put_request() - COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
                msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
            }
        } else {
            msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
        }
        if (coap_response) {
            coap_response->msg_code = msg_code;
        }
    } else {
        coap_response = M2MResourceBase::handle_put_request(nsdl,
                            received_coap_header,
                            observation_handler,
                            execute_value_updated);
    }
    return coap_response;
}


sn_coap_hdr_s* M2MResource::handle_post_request(nsdl_s *nsdl,
                                                sn_coap_hdr_s *received_coap_header,
                                                M2MObservationHandler */*observation_handler*/,
                                                bool &/*execute_value_updated*/,
                                                sn_nsdl_addr_s *address)
{
    tr_info("M2MResource::handle_post_request()");
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CHANGED; // 2.04
    sn_coap_hdr_s * coap_response = sn_nsdl_build_response(nsdl,
                                                           received_coap_header,
                                                           msg_code);

    // process the POST if we have registered a callback for it
    if(received_coap_header) {
        if ((operation() & M2MBase::POST_ALLOWED) != 0) {
#ifndef MEMORY_OPTIMIZED_API
            const String &obj_name = object_name();
            const String &res_name = name();
            M2MResource::M2MExecuteParameter exec_params(obj_name, res_name, object_instance_id());
#else
            M2MResource::M2MExecuteParameter exec_params(object_name(), name(), object_instance_id());
#endif
#ifdef MBED_CLOUD_CLIENT_EDGE_EXTENSION
            exec_params.set_resource(this);
#endif

            uint16_t coap_content_type = 0;
            if(received_coap_header->payload_ptr) {
                if(received_coap_header->content_format != COAP_CT_NONE) {
                    coap_content_type = received_coap_header->content_format;
                }

                if(coap_content_type == COAP_CT_TEXT_PLAIN) {
                    exec_params._value = received_coap_header->payload_ptr;
                    if (exec_params._value) {
                        exec_params._value_length = received_coap_header->payload_len;
                    }
                } else {
                    msg_code = COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT;
                }
            }
            if(COAP_MSG_CODE_RESPONSE_CHANGED == msg_code) {
                tr_debug("M2MResource::handle_post_request - Execute resource function");
#ifndef DISABLE_DELAYED_RESPONSE
                if (coap_response) {
                    if(_delayed_response) {
                        if(received_coap_header->token_len) {
                            free(_delayed_token);
                            _delayed_token = NULL;
                            _delayed_token_len = 0;
                            _delayed_token = alloc_copy(received_coap_header->token_ptr, received_coap_header->token_len);
                            if(_delayed_token) {
                                _delayed_token_len = received_coap_header->token_len;
                            }
                        }
                    } else {
    #endif
                        uint32_t length = 0;
                        get_value(coap_response->payload_ptr, length);
                        coap_response->payload_len = length;
    #ifndef DISABLE_DELAYED_RESPONSE
                    }
                }
#endif
                execute(&exec_params);
            }

        } else { // if ((object->operation() & SN_GRS_POST_ALLOWED) != 0)
            tr_error("M2MResource::handle_post_request - COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
            msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED; // 4.05
        }
    } else { //if(object && received_coap_header)
        tr_error("M2MResource::handle_post_request - COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
        msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED; // 4.01
    }
    if(coap_response) {
        coap_response->msg_code = msg_code;
    }
    return coap_response;
}

M2MBase *M2MResource::get_parent() const
{
    return (M2MBase *) &get_parent_object_instance();
}

M2MObjectInstance& M2MResource::get_parent_object_instance() const
{
    return _parent;
}

uint16_t M2MResource::object_instance_id() const
{
    const M2MObjectInstance& parent_object_instance = get_parent_object_instance();
    return parent_object_instance.instance_id();
}

M2MResource& M2MResource::get_parent_resource() const
{
    return (M2MResource&)*this;
}

const char* M2MResource::object_name() const
{
    const M2MObjectInstance& parent_object_instance = _parent;
    const M2MObject& parent_object = parent_object_instance.get_parent_object();

    return parent_object.name();
}

#ifdef MBED_CLOUD_CLIENT_EDGE_EXTENSION
bool M2MResource::get_manifest_check_status()
{
    return _status;
}

void M2MResource::set_manifest_check_status(bool status)
{
    _status = status;
}
#endif

#ifdef MEMORY_OPTIMIZED_API
M2MResource::M2MExecuteParameter::M2MExecuteParameter(const char *object_name, const char *resource_name,
                                                      uint16_t object_instance_id) :
_object_name(object_name),
_resource_name(resource_name),
_value(NULL),
_value_length(0),
_object_instance_id(object_instance_id)
{
}
#else
M2MResource::M2MExecuteParameter::M2MExecuteParameter(const String &object_name,
                                                      const String &resource_name,
                                                      uint16_t object_instance_id) :
_object_name(object_name),
_resource_name(resource_name),
_value(NULL),
_value_length(0),
_object_instance_id(object_instance_id)
{
}
#endif

// These could be actually changed to be inline ones, as it would likely generate
// smaller code at application side.

const uint8_t *M2MResource::M2MExecuteParameter::get_argument_value() const
{
    return _value;
}

uint16_t M2MResource::M2MExecuteParameter::get_argument_value_length() const
{
    return _value_length;
}

#ifdef MEMORY_OPTIMIZED_API
const char* M2MResource::M2MExecuteParameter::get_argument_object_name() const
{
    return _object_name;
}

const char* M2MResource::M2MExecuteParameter::get_argument_resource_name() const
{
    return _resource_name;
}
#else
const String& M2MResource::M2MExecuteParameter::get_argument_object_name() const
{
    return _object_name;
}

const String& M2MResource::M2MExecuteParameter::get_argument_resource_name() const
{
    return _resource_name;
}
#endif

uint16_t M2MResource::M2MExecuteParameter::get_argument_object_instance_id() const
{
    return _object_instance_id;
}
#ifdef MBED_CLOUD_CLIENT_EDGE_EXTENSION
void M2MResource::M2MExecuteParameter::set_resource(M2MResource* resource)
{
    _resource = resource;
}

M2MResource * M2MResource::M2MExecuteParameter::get_resource()
{
    return _resource;
}
#endif

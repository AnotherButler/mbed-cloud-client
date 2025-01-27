/*
 * Copyright (c) 2015-2021 Pelion. All rights reserved.
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
#ifndef M2M_OBJECT_INSTANCE_H
#define M2M_OBJECT_INSTANCE_H

#include "mbed-client/m2mvector.h"
#include "mbed-client/m2mresource.h"

//FORWARD DECLARATION
typedef Vector<M2MResource *> M2MResourceList;
typedef Vector<M2MResourceInstance *> M2MResourceInstanceList;


class M2MObject;

/** \file m2mobjectinstance.h \brief header for M2MObjectInstance */

/**
 * This the LwM2M object instance class.
 * You can use it to represent any defined LWM2M object instances.
 *
 * This class also holds all resource instances associated with
 * the given object.
 *
 * Constructor and destructor are private which means
 * that these objects can be created or
 * deleted only through function provided by M2MObject.
 */
class M2MObjectInstance : public M2MBase
{

friend class M2MObject;

private:
    /**
     * \brief Constructor
     * \param name Name of the object
     */
    M2MObjectInstance(M2MObject& parent,
                      const String &resource_type,
                      char *path,
                      bool external_blockwise_store = false);

    M2MObjectInstance(M2MObject& parent, const lwm2m_parameters_s* static_res);

    // Prevents the use of default constructor.
    M2MObjectInstance();

    // Prevents the use of assignment operator.
    M2MObjectInstance& operator=( const M2MObjectInstance& /*other*/ );

    // Prevents the use of copy constructor.
    M2MObjectInstance( const M2MObjectInstance& /*other*/ );

    /**
     * Destructor
     */
    virtual ~M2MObjectInstance();

public:

    /**
     * \brief TODO!
     * \return M2MResource The resource for managing other client operations.
     * \deprecated Please use M2MInterfaceFactory::create_resource() instead.
     */
    M2MResource* create_static_resource(const lwm2m_parameters_s* static_res,
                                        M2MResourceInstance::ResourceType type);

    /**
     * \brief Creates a static resource for a given mbed Client Inteface object. With this, the
     * client can respond to server's GET methods with the provided value.
     * \param resource_name The name of the resource.
     * \param resource_type The type of the resource.
     * \param value A pointer to the value of the resource.
     * \param value_length The length of the value in the pointer.
     * \param multiple_instance A resource can have
     *        multiple instances, default is false.
     * \param external_blockwise_store If true CoAP blocks are passed to application through callbacks
     *        otherwise handled in mbed-client-c.
     * \return M2MResource The resource for managing other client operations.
     * \deprecated Please use M2MInterfaceFactory::create_resource() instead.
     */
    M2MResource* create_static_resource(const String &resource_name,
                                        const String &resource_type,
                                        M2MResourceInstance::ResourceType type,
                                        const uint8_t *value,
                                        const uint8_t value_length,
                                        bool multiple_instance = false,
                                        bool external_blockwise_store = false);

    /**
     * \brief Creates a dynamic resource for a given mbed Client Inteface object. With this,
     * the client can respond to different queries from the server (GET,PUT etc).
     * This type of resource is also observable and carries callbacks.
     * \param resource_name The name of the resource.
     * \param resource_type The type of the resource.
     * \param observable Indicates whether the resource is observable or not.
     * \param multiple_instance The resource can have
     *        multiple instances, default is false.
     * \param external_blockwise_store If true CoAP blocks are passed to application through callbacks
     *        otherwise handled in mbed-client-c.
     * \return M2MResource The resource for managing other client operations.
     * \deprecated Please use M2MInterfaceFactory::create_resource() instead.
     */
    M2MResource* create_dynamic_resource(const String &resource_name,
                                         const String &resource_type,
                                         M2MResourceInstance::ResourceType type,
                                         bool observable,
                                         bool multiple_instance = false,
                                         bool external_blockwise_store = false);

    /**
     * \brief Creates a dynamic resource for a given mbed Client Inteface object. With this,
     * the client can respond to different queries from the server (GET,PUT etc).
     * This type of resource is also observable and carries callbacks.
     * \param resource_name The name of the resource.
     * \param resource_type The type of the resource as null-terminated string.
     * \param observable Indicates whether the resource is observable or not.
     * \param multiple_instance The resource can have
     *        multiple instances, default is false.
     * \param external_blockwise_store If true CoAP blocks are passed to application through callbacks
     *        otherwise handled in mbed-client-c.
     * \return M2MResource The resource for managing other client operations.
     * \deprecated Please use M2MInterfaceFactory::create_resource() instead.
     */
    M2MResource* create_dynamic_resource(const uint16_t resource_name,
                                         const char *resource_type,
                                         M2MResourceInstance::ResourceType type,
                                         bool observable,
                                         bool multiple_instance = false,
                                         bool external_blockwise_store = false);

    /**
     * \brief TODO!
     * \return M2MResource The resource for managing other client operations.
     * \deprecated Please use M2MInterfaceFactory::create_resource() instead.
     */
    M2MResource* create_dynamic_resource(const lwm2m_parameters_s* static_res,
                                        M2MResourceInstance::ResourceType type,
                                        bool observable);

    /**
     * \brief Creates a static resource instance for a given mbed Client Inteface object. With this,
     * the client can respond to server's GET methods with the provided value.
     * \param resource_name The name of the resource.
     * \param resource_type The type of the resource.
     * \param value A pointer to the value of the resource.
     * \param value_length The length of the value in pointer.
     * \param instance_id The instance ID of the resource.
     * \param external_blockwise_store If true CoAP blocks are passed to application through callbacks
     *        otherwise handled in mbed-client-c.
     * \return M2MResourceInstance The resource instance for managing other client operations.
     * \deprecated Please use M2MInterfaceFactory::create_resource() instead.
     */
    M2MResourceInstance* create_static_resource_instance(const String &resource_name,
                                                         const String &resource_type,
                                                         M2MResourceInstance::ResourceType type,
                                                         const uint8_t *value,
                                                         const uint8_t value_length,
                                                         uint16_t instance_id,
                                                         bool external_blockwise_store = false);

    /**
     * \brief Creates a dynamic resource instance for a given mbed Client Inteface object. With this,
     * the client can respond to different queries from the server (GET,PUT etc).
     * This type of resource is also observable and carries callbacks.
     * \param resource_name The name of the resource.
     * \param resource_type The type of the resource.
     * \param observable Indicates whether the resource is observable or not.
     * \param instance_id The instance ID of the resource.
     * \param external_blockwise_store If true CoAP blocks are passed to application through callbacks
     *        otherwise handled in mbed-client-c.
     * \return M2MResourceInstance The resource instance for managing other client operations.
     * \deprecated Please use M2MInterfaceFactory::create_resource() instead.
     */
    M2MResourceInstance* create_dynamic_resource_instance(const String &resource_name,
                                                          const String &resource_type,
                                                          M2MResourceInstance::ResourceType type,
                                                          bool observable,
                                                          uint16_t instance_id,
                                                          bool external_blockwise_store = false);

    /**
     * \brief Removes the resource with the given name.
     * \param name The name of the resource to be removed.
     * Note: this will be removed in next version, please use the
     * remove_resource(const char*) version instead.
     * \return True if removed, else false.
     * \deprecated String based APIs are deprecated. Please use the id or path instead.
     */
    bool remove_resource(const String &name);

    /**
     * \brief Removes the resource with the given name.
     * \param name The name of the resource to be removed.
     * \return True if removed, else false.
     */
    bool remove_resource(const char *name);

    /**
     * \brief Removes the resource instance with the given name.
     * \param resource_name The name of the resource instance to be removed.
     * \param instance_id The instance ID of the instance.
     * \return True if removed, else false.
     */
    bool remove_resource_instance(const String &resource_name,
                                          uint16_t instance_id);

    /**
     * \brief Returns the resource with the given name.
     * \param name The name of the requested resource.
     * \return Resource reference if found, else NULL.
     */
    M2MResource* resource(const uint16_t resource_id) const;

    /**
     * \deprecated String based APIs are deprecated. Please use resource_id or path instead.
     */
    M2MResource* resource(const String &name) const;

    M2MResource* resource(const char *resource) const;

    /**
     * \brief Returns a list of M2MResourceBase objects.
     * \return A list of M2MResourceBase objects.
     */
    const M2MResourceList& resources() const;

    /**
     * \brief Returns the total number of resources with the object.
     * \return Total number of the resources.
     */
    uint16_t resource_count() const;

    /**
     * \brief Returns the total number of single resource instances.
     * Note: this will be removed in next version, please use the
     * resource_count(const char*) version instead.
     * \param resource The name of the resource.
     * \return Total number of the resources.
     * \deprecated String based APIs are deprecated. Please use resource_id or path instead.
     */
    uint16_t resource_count(const String& resource) const;

    /**
     * \brief Returns the total number of single resource instances.
     * \param resource The name of the resource.
     * \return Total number of the resources.
     */
    uint16_t resource_count(const char *resource) const;

    /**
     * \brief Adds the observation level for the object.
     * \param observation_level The level of observation.
     * \deprecated Internal API, subject to be modified or removed.
     */
    virtual void add_observation_level(M2MBase::Observation observation_level);

    /**
     * \brief Removes the observation level from the object.
     * \param observation_level The level of observation.
     * \deprecated Internal API, subject to be modified or removed.
     */
    virtual void remove_observation_level(M2MBase::Observation observation_level);

    /**
     * \brief Returns the Observation Handler object.
     * \return M2MObservationHandler object.
     * \deprecated Internal API, subject to be modified or removed.
    */
    virtual M2MObservationHandler* observation_handler() const;

    /**
     * \brief Sets the observation handler
     * \param handler Observation handler
     * \deprecated Internal API, subject to be modified or removed.
    */
    virtual void set_observation_handler(M2MObservationHandler *handler);

    /**
     * \brief Handles GET request for the registered objects.
     * \param nsdl The NSDL handler for the CoAP library.
     * \param received_coap_header The CoAP message received from the server.
     * \param observation_handler The handler object for sending
     * observation callbacks.
     * return sn_coap_hdr_s The message that needs to be sent to the server.
     * \deprecated Internal API, subject to be modified or removed.
     */
    virtual sn_coap_hdr_s* handle_get_request(nsdl_s *nsdl,
                                              sn_coap_hdr_s *received_coap_header,
                                              M2MObservationHandler *observation_handler = NULL);
    /**
     * \brief Handles PUT request for the registered objects.
     * \param nsdl The NSDL handler for the CoAP library.
     * \param received_coap_header The CoAP message received from the server.
     * \param observation_handler The handler object for sending
     * observation callbacks.
     * \param execute_value_updated True will execute the "value_updated" callback.
     * \return sn_coap_hdr_s The message that needs to be sent to server.
     * \deprecated Internal API, subject to be modified or removed.
     */
    virtual sn_coap_hdr_s* handle_put_request(nsdl_s *nsdl,
                                              sn_coap_hdr_s *received_coap_header,
                                              M2MObservationHandler *observation_handler,
                                              bool &execute_value_updated);

    /**
     * \brief Handles POST request for the registered objects.
     * \param nsdl The NSDL handler for the CoAP library.
     * \param received_coap_header The CoAP message received from the server.
     * \param observation_handler The handler object for sending
     * observation callbacks.
     * \param execute_value_updated True will execute the "value_updated" callback.
     * \return sn_coap_hdr_s The message that needs to be sent to server.
     * \deprecated Internal API, subject to be modified or removed.
     */
    virtual sn_coap_hdr_s* handle_post_request(nsdl_s *nsdl,
                                               sn_coap_hdr_s *received_coap_header,
                                               M2MObservationHandler *observation_handler,
                                               bool &execute_value_updated,
                                               sn_nsdl_addr_s *address = NULL);

    inline M2MObject& get_parent_object() const;

    /** callback used from M2MResource/M2MResourceInstance
     * \deprecated Internal API, subject to be modified or removed.
     */
    void notification_update(M2MBase::Observation observation_level);

protected:
    virtual M2MBase *get_parent() const;

private:

    /**
     * \brief Utility function to map M2MResourceInstance ResourceType
     * to M2MBase::DataType.
     * \param resource_type M2MResourceInstance::ResourceType.
     * \return M2MBase::DataType.
     */
    M2MBase::DataType convert_resource_type(M2MResourceInstance::ResourceType);

private:

    M2MObject      &_parent;

    M2MResourceList     _resource_list; // owned

    friend class Test_M2MObjectInstance;
    friend class Test_M2MObject;
    friend class Test_M2MDevice;
    friend class Test_M2MSecurity;
    friend class Test_M2MServer;
    friend class Test_M2MInterfaceFactory;
    friend class Test_M2MNsdlInterface;
    friend class Test_M2MTLVSerializer;
    friend class Test_M2MTLVDeserializer;
    friend class Test_M2MBase;
    friend class Test_M2MResource;
    friend class Test_M2MResourceInstance;
    friend class Test_M2MReportHandler;
    friend class TestFactory;
    friend class Test_M2MInterfaceImpl;
};

inline M2MObject& M2MObjectInstance::get_parent_object() const
{
    return _parent;
}

#endif // M2M_OBJECT_INSTANCE_H

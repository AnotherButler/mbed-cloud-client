// ----------------------------------------------------------------------------
// Copyright 2016-2018 ARM Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------------------------------------------------------

#include "factory_configurator_client.h"
#include "fcc_sotp.h"
#include "key_config_manager.h"
#include "pv_error_handling.h"
#include "fcc_verification.h"
#include "storage.h"
#include "fcc_defs.h"
#include "fcc_malloc.h"
#include "common_utils.h"
#include "pal.h"

/**
* Device general info
*/
const char g_fcc_use_bootstrap_parameter_name[] = "mbed.UseBootstrap";
const char g_fcc_endpoint_parameter_name[] = "mbed.EndpointName";
const char g_fcc_account_id_parameter_name[] = "mbed.AccountID";
const char g_fcc_first_to_claim_parameter_name[] = "mbed.FirstToClaim";

/**
* Device meta data
*/
const char g_fcc_manufacturer_parameter_name[] = "mbed.Manufacturer";
const char g_fcc_model_number_parameter_name[] = "mbed.ModelNumber";
const char g_fcc_device_type_parameter_name[] = "mbed.DeviceType";
const char g_fcc_hardware_version_parameter_name[] = "mbed.HardwareVersion";
const char g_fcc_memory_size_parameter_name[] = "mbed.MemoryTotalKB";
const char g_fcc_device_serial_number_parameter_name[] = "mbed.SerialNumber";
/**
* Time Synchronization
*/
const char g_fcc_current_time_parameter_name[] = "mbed.CurrentTime";
const char g_fcc_device_time_zone_parameter_name[] = "mbed.Timezone";
const char g_fcc_offset_from_utc_parameter_name[] = "mbed.UTCOffset";
/**
* Bootstrap configuration
*/
const char g_fcc_bootstrap_server_ca_certificate_name[] = "mbed.BootstrapServerCACert";
const char g_fcc_bootstrap_server_crl_name[] = "mbed.BootstrapServerCRL";
const char g_fcc_bootstrap_server_uri_name[] = "mbed.BootstrapServerURI";
const char g_fcc_bootstrap_device_certificate_name[] = "mbed.BootstrapDeviceCert";
const char g_fcc_bootstrap_device_private_key_name[] = "mbed.BootstrapDevicePrivateKey";
/**
* LWm2m configuration
*/
const char g_fcc_lwm2m_server_ca_certificate_name[] = "mbed.LwM2MServerCACert";
const char g_fcc_lwm2m_server_crl_name[] = "mbed.LwM2MServerCRL";
const char g_fcc_lwm2m_server_uri_name[] = "mbed.LwM2MServerURI";
const char g_fcc_lwm2m_device_certificate_name[] = "mbed.LwM2MDeviceCert";
const char g_fcc_lwm2m_device_private_key_name[] = "mbed.LwM2MDevicePrivateKey";
/**
* Firmware update
*/
const char g_fcc_update_authentication_certificate_name[] = "mbed.UpdateAuthCert";
const char g_fcc_class_id_name[] = "mbed.ClassId";
const char g_fcc_vendor_id_name[] = "mbed.VendorId";

static bool g_is_fcc_initialized = false;
bool g_is_session_finished = true;

fcc_status_e fcc_init(void)
{
    palStatus_t pal_status;

    SA_PV_LOG_INFO_FUNC_ENTER_NO_ARGS();

    if (g_is_fcc_initialized) {
        // No need for second initialization
        return FCC_STATUS_SUCCESS;
    }
    
    pal_status = pal_init();
    SA_PV_ERR_RECOVERABLE_RETURN_IF((pal_status != PAL_SUCCESS), FCC_STATUS_ERROR, "Failed initializing PAL (%" PRIu32 ")", pal_status);
    
    //Initialize output info handler
    fcc_init_output_info_handler();

    g_is_fcc_initialized = true;

    SA_PV_LOG_INFO_FUNC_EXIT_NO_ARGS();

    return FCC_STATUS_SUCCESS;
}

fcc_status_e fcc_finalize(void)
{
    fcc_status_e fcc_status = FCC_STATUS_SUCCESS;
    kcm_status_e kcm_status = KCM_STATUS_SUCCESS;

    SA_PV_LOG_INFO_FUNC_ENTER_NO_ARGS();

    SA_PV_ERR_RECOVERABLE_RETURN_IF((!g_is_fcc_initialized), FCC_STATUS_NOT_INITIALIZED, "FCC not initialized");

    //FIXME: add relevant error handling - general task for all APIs.
    //It is okay to finalize KCM here since it's already initialized beforehand.
    kcm_status = kcm_finalize();
    if (kcm_status != KCM_STATUS_SUCCESS) {
        fcc_status = FCC_STATUS_ERROR;
        SA_PV_LOG_ERR("Failed finalizing KCM");
    }

    //Finalize output info handler
    fcc_clean_output_info_handler();

    //Finalize PAL
    pal_destroy();

    g_is_fcc_initialized = false;
    g_is_session_finished = true;

    SA_PV_LOG_INFO_FUNC_EXIT_NO_ARGS();

    return fcc_status;
}

fcc_status_e fcc_storage_delete()
{
    kcm_status_e status = KCM_STATUS_SUCCESS;
    sotp_result_e sotp_status = SOTP_SUCCESS;

    SA_PV_LOG_INFO_FUNC_ENTER_NO_ARGS();

    SA_PV_ERR_RECOVERABLE_RETURN_IF((!g_is_fcc_initialized), FCC_STATUS_NOT_INITIALIZED, "FCC not initialized");

    status = storage_reset();
    SA_PV_ERR_RECOVERABLE_RETURN_IF((status == KCM_STATUS_ESFS_ERROR), FCC_STATUS_KCM_STORAGE_ERROR, "Failed in storage_reset. got ESFS error");
    SA_PV_ERR_RECOVERABLE_RETURN_IF((status != KCM_STATUS_SUCCESS), FCC_STATUS_ERROR, "Failed storage reset");

    sotp_status = sotp_reset();
    SA_PV_ERR_RECOVERABLE_RETURN_IF((sotp_status != SOTP_SUCCESS), FCC_STATUS_STORE_ERROR, "Failed to reset sotp storage ");
    
    SA_PV_LOG_INFO_FUNC_EXIT_NO_ARGS();
    return FCC_STATUS_SUCCESS;
}

fcc_output_info_s* fcc_get_error_and_warning_data(void)
{
    SA_PV_LOG_INFO_FUNC_ENTER_NO_ARGS();

    SA_PV_ERR_RECOVERABLE_RETURN_IF((!g_is_fcc_initialized), NULL, "FCC not initialized");

    SA_PV_LOG_INFO_FUNC_EXIT_NO_ARGS();

    return get_output_info();
}

bool fcc_is_session_finished(void)
{
    SA_PV_LOG_INFO_FUNC_ENTER_NO_ARGS();

    return g_is_session_finished;
}

fcc_status_e fcc_verify_device_configured_4mbed_cloud(void)
{
    fcc_status_e  fcc_status =  FCC_STATUS_SUCCESS;
    bool use_bootstrap = false;
    bool success = false;

    SA_PV_LOG_INFO_FUNC_ENTER_NO_ARGS();

    SA_PV_ERR_RECOVERABLE_RETURN_IF((!g_is_fcc_initialized), FCC_STATUS_NOT_INITIALIZED, "FCC not initialized");

    /*Initialize fcc_output_info_s structure.
    In case output indo struct is not empty in the beginning of the verify process we will clean it.*/
    fcc_clean_output_info_handler();

    //Check entropy initialization
    success = fcc_is_entropy_initialized();
    SA_PV_ERR_RECOVERABLE_RETURN_IF((success != true), fcc_status = FCC_STATUS_ENTROPY_ERROR, "Entropy is not initialized");

    //Check time synchronization
    fcc_status = fcc_check_time_synchronization();
    SA_PV_ERR_RECOVERABLE_RETURN_IF((fcc_status != FCC_STATUS_SUCCESS), fcc_status, "Failed to check time synhronization");

    //Get bootstrap mode
    fcc_status = fcc_get_bootstrap_mode(&use_bootstrap);
    SA_PV_ERR_RECOVERABLE_RETURN_IF((fcc_status != FCC_STATUS_SUCCESS), fcc_status, "Failed to get bootstrap mode");

    // Check general info
    fcc_status = fcc_check_device_general_info();
    SA_PV_ERR_RECOVERABLE_RETURN_IF((fcc_status != FCC_STATUS_SUCCESS), fcc_status, "Failed to check general info");

    //Check device meta-data
    fcc_status = fcc_check_device_meta_data();
    SA_PV_ERR_RECOVERABLE_RETURN_IF((fcc_status != FCC_STATUS_SUCCESS), fcc_status, "Failed to check configuration parameters");

    //Check device security objects
    fcc_status = fcc_check_device_security_objects(use_bootstrap);
    SA_PV_ERR_RECOVERABLE_RETURN_IF((fcc_status != FCC_STATUS_SUCCESS), fcc_status, "Failed to check device security objects");

    //Check firmware integrity
    fcc_status = fcc_check_firmware_update_integrity();
    SA_PV_ERR_RECOVERABLE_RETURN_IF((fcc_status != FCC_STATUS_SUCCESS), fcc_status, "Failed to check device security objects");

    SA_PV_LOG_INFO_FUNC_EXIT_NO_ARGS();

    return fcc_status;
}

fcc_status_e fcc_entropy_set(const uint8_t *buf, size_t buf_size)
{
    fcc_status_e fcc_status;
    SA_PV_LOG_INFO_FUNC_ENTER_NO_ARGS();

    SA_PV_ERR_RECOVERABLE_RETURN_IF((!g_is_fcc_initialized), FCC_STATUS_NOT_INITIALIZED, "FCC not initialized");

    fcc_status = fcc_sotp_data_store(buf, buf_size, SOTP_TYPE_RANDOM_SEED);
    SA_PV_ERR_RECOVERABLE_RETURN_IF((fcc_status == FCC_STATUS_INTERNAL_ITEM_ALREADY_EXIST), FCC_STATUS_ENTROPY_ERROR, "Entropy already exist in storage");
    SA_PV_ERR_RECOVERABLE_RETURN_IF((fcc_status != FCC_STATUS_SUCCESS), fcc_status, "Failed to set entropy");

    SA_PV_LOG_INFO_FUNC_EXIT_NO_ARGS();
    return FCC_STATUS_SUCCESS;
}

fcc_status_e fcc_rot_set(const uint8_t *buf, size_t buf_size)
{
    fcc_status_e fcc_status;
    SA_PV_LOG_INFO_FUNC_ENTER_NO_ARGS();

    SA_PV_ERR_RECOVERABLE_RETURN_IF((!g_is_fcc_initialized), FCC_STATUS_NOT_INITIALIZED, "FCC not initialized");

    fcc_status = fcc_sotp_data_store(buf, buf_size, SOTP_TYPE_ROT);
    SA_PV_ERR_RECOVERABLE_RETURN_IF((fcc_status == FCC_STATUS_INTERNAL_ITEM_ALREADY_EXIST), FCC_STATUS_ROT_ERROR, "RoT already exist in storage");
    SA_PV_ERR_RECOVERABLE_RETURN_IF((fcc_status != FCC_STATUS_SUCCESS), fcc_status, "Failed to set RoT");

    SA_PV_LOG_INFO_FUNC_EXIT_NO_ARGS();
    return FCC_STATUS_SUCCESS;
}

fcc_status_e fcc_time_set(uint64_t time)
{
    palStatus_t pal_status;

    SA_PV_ERR_RECOVERABLE_RETURN_IF((!g_is_fcc_initialized), FCC_STATUS_NOT_INITIALIZED, "FCC not initialized");

    pal_status = pal_osSetStrongTime(time);
    SA_PV_ERR_RECOVERABLE_RETURN_IF((pal_status != PAL_SUCCESS), FCC_STATUS_ERROR, "Failed to set new EPOCH time (pal_status = %" PRIu32 ")", pal_status);

    return FCC_STATUS_SUCCESS;
}

fcc_status_e fcc_is_factory_disabled(bool *is_factory_disabled)
{
    fcc_status_e fcc_status;
    int64_t factory_disable_flag = 0;
    size_t data_actual_size_out = 0;

    SA_PV_LOG_INFO_FUNC_ENTER_NO_ARGS();
    SA_PV_ERR_RECOVERABLE_RETURN_IF((!g_is_fcc_initialized), FCC_STATUS_NOT_INITIALIZED, "FCC not initialized");
    SA_PV_ERR_RECOVERABLE_RETURN_IF((is_factory_disabled == NULL), FCC_STATUS_INVALID_PARAMETER, "Invalid param is_factory_disabled");

    fcc_status = fcc_sotp_data_retrieve((uint8_t *)(&factory_disable_flag), sizeof(factory_disable_flag), &data_actual_size_out, SOTP_TYPE_FACTORY_DONE);
    SA_PV_LOG_INFO("fcc_status: %d, factory_disable_flag:%" PRIuMAX "\n", fcc_status, factory_disable_flag);
    SA_PV_ERR_RECOVERABLE_RETURN_IF((fcc_status != FCC_STATUS_SUCCESS && fcc_status != FCC_STATUS_ITEM_NOT_EXIST), fcc_status, "Failed for fcc_sotp_buffer_retrieve");
    SA_PV_ERR_RECOVERABLE_RETURN_IF(((factory_disable_flag != 0) && (factory_disable_flag != 1)), FCC_STATUS_FACTORY_DISABLED_ERROR, "Failed for fcc_sotp_buffer_retrieve");

    // If we get here - it must be either "0" or "1"
    *is_factory_disabled = (factory_disable_flag == 1) ? true : false;
    SA_PV_LOG_INFO_FUNC_EXIT_NO_ARGS();
    return FCC_STATUS_SUCCESS;
}


fcc_status_e fcc_factory_disable(void)
{
    fcc_status_e fcc_status;
    int64_t factory_disable_flag = 1;

    SA_PV_LOG_INFO_FUNC_ENTER_NO_ARGS();

    SA_PV_ERR_RECOVERABLE_RETURN_IF((!g_is_fcc_initialized), FCC_STATUS_NOT_INITIALIZED, "FCC not initialized");

    fcc_status = fcc_sotp_data_store((uint8_t *)(&factory_disable_flag), sizeof(factory_disable_flag), SOTP_TYPE_FACTORY_DONE);
    SA_PV_ERR_RECOVERABLE_RETURN_IF((fcc_status == FCC_STATUS_INTERNAL_ITEM_ALREADY_EXIST), FCC_STATUS_FACTORY_DISABLED_ERROR, "FCC already disabled in storage");
    SA_PV_ERR_RECOVERABLE_RETURN_IF((fcc_status != FCC_STATUS_SUCCESS), fcc_status, "Failed for fcc_sotp_buffer_store");

    SA_PV_LOG_INFO_FUNC_EXIT_NO_ARGS();
    return FCC_STATUS_SUCCESS;
}


fcc_status_e fcc_trust_ca_cert_id_set(void)
{
    fcc_status_e fcc_status = FCC_STATUS_SUCCESS;
    fcc_status_e output_info_fcc_status = FCC_STATUS_SUCCESS;
    uint8_t attribute_data[PAL_CERT_ID_SIZE] = {0};
    size_t size_of_attribute_data = 0;
    bool use_bootstrap = false;

    SA_PV_LOG_INFO_FUNC_ENTER_NO_ARGS();

    SA_PV_ERR_RECOVERABLE_RETURN_IF((!g_is_fcc_initialized), FCC_STATUS_NOT_INITIALIZED, "FCC not initialized");

    fcc_status = fcc_get_bootstrap_mode(&use_bootstrap);
    SA_PV_ERR_RECOVERABLE_RETURN_IF((fcc_status != FCC_STATUS_SUCCESS), fcc_status, "Failed to get bootstrap mode");


    //For now this API relevant only for bootstrap certificate.
    if (use_bootstrap == true) {
        fcc_status = fcc_get_certificate_attribute_by_name((const uint8_t*)g_fcc_bootstrap_server_ca_certificate_name,
            (size_t)(strlen(g_fcc_bootstrap_server_ca_certificate_name)),
            CS_CERT_ID_ATTR,
            attribute_data,
            sizeof(attribute_data),
            &size_of_attribute_data);
        SA_PV_ERR_RECOVERABLE_GOTO_IF((fcc_status != FCC_STATUS_SUCCESS), fcc_status = fcc_status, exit, "Failed to get ca id");
 
        fcc_status = fcc_sotp_data_store(attribute_data, size_of_attribute_data, SOTP_TYPE_TRUSTED_TIME_SRV_ID);
        SA_PV_ERR_RECOVERABLE_GOTO_IF((fcc_status == FCC_STATUS_INTERNAL_ITEM_ALREADY_EXIST), (fcc_status = FCC_STATUS_CA_ERROR), exit, "CA already exist in storage");
        SA_PV_ERR_RECOVERABLE_GOTO_IF((fcc_status != FCC_STATUS_SUCCESS), fcc_status = fcc_status, exit, "Failed to set ca id");
    }

exit:
    if (fcc_status != FCC_STATUS_SUCCESS) {
        output_info_fcc_status = fcc_store_error_info((const uint8_t*)g_fcc_bootstrap_server_ca_certificate_name, strlen(g_fcc_bootstrap_server_ca_certificate_name), fcc_status);
        SA_PV_ERR_RECOVERABLE_RETURN_IF((output_info_fcc_status != FCC_STATUS_SUCCESS),
            fcc_status = FCC_STATUS_OUTPUT_INFO_ERROR,
            "Failed to set ca identifier error  %d",
            fcc_status);
    }

    SA_PV_LOG_INFO_FUNC_EXIT_NO_ARGS();
    return fcc_status;
}

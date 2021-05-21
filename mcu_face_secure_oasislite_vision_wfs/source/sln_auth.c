/*
 * Copyright 2018 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pin_mux.h"
#include "clock_config.h"

#include "FreeRTOS.h"

#include "mbedtls/base64.h"
#include "mbedtls/md.h"
#include "mbedtls/md_internal.h"
#include "mbedtls/x509_crt.h"

#include "sln_flash.h"
#if RTVISION_BOARD
#include "flexspi_hyper_flash.h"
#elif RT106F_ELOCK_BOARD
#include "flexspi_qspi_flash.h"
#endif

#include "sln_auth.h"

#define KIB_ADDR(index) (0x400 * (index + 1))
#define PRDB_ADDR(index) (0x400 * (index + 1) + 0x80)

#define KIB_LEN     0x20U
#define RESV_LEN    0x60U
#define PRDB_LEN    0xD0U

#define PARAM_SECTOR SECTOR_SIZE//0x1000U // Set to 4 KB paramter sector size if avilable

static mbedtls_x509_crt s_caChain;
static mbedtls_x509_crt s_verifCert;

__attribute__((section(".bss.$SRAM_OC_NON_CACHEABLE"))) 
__attribute__((aligned(4))) uint8_t s_tempBuffer[PARAM_SECTOR] = {0};

static bool is_buffer_empty(uint8_t *buf, uint32_t len)
{
    bool isEmpty = false;
    uint32_t isOnesCount = 0U;
    uint32_t isZeroCount = 0U;

    for (uint32_t idx = 0; idx < len; idx++)
    {
        if (0x00U == buf[idx])
        {
            isZeroCount++;
        }
        else if (0xFFU == buf[idx])
        {
            isOnesCount++;
        }
    }

    if ((len == isOnesCount) || (len == isZeroCount))
    {
        isEmpty = true;
    }

    return isEmpty;
}

__attribute__((section(".data.$SRAM_OC_NON_CACHEABLE")))
static int32_t write_to_nvm(uint32_t address, uint8_t *buf, uint32_t len)
{
    int32_t status = SLN_AUTH_OK;

    // Erase and write to sector in one go
    status = SLN_Write_Sector(address, buf, len);

    return status;
}

int32_t SLN_AUTH_check_context(int32_t context)
{
    int32_t status = SLN_AUTH_OK;
    uint8_t tempKIB[KIB_LEN] = {0};
    uint8_t tempPRDB[PRDB_LEN] = {0};

    if ((SLN_CRYPTO_CTX_NONE >= context) || (SLN_CRYPTO_CTX_COUNT <= context))
    {
        status = SLN_AUTH_INVALID_ARG;
    }

    if (SLN_AUTH_OK == status)
    {
        // Read out KIBx
        status = SLN_Read_Flash_At_Address(KIB_ADDR(context), tempKIB, KIB_LEN);
    }

    if (SLN_AUTH_OK == status)
    {
        // Check if KIBx is empty
        bool isEmpty = false;
        
        isEmpty = is_buffer_empty(tempKIB, KIB_LEN);

        if (isEmpty)
        {
            configPRINTF(("Context %d KIB empty!\r\n", context));
            status = SLN_AUTH_NO_CONTEXT;
        }
    }

    if (SLN_AUTH_OK == status)
    {
        // Read out PRDBx
        status = SLN_Read_Flash_At_Address(PRDB_ADDR(context), tempPRDB, PRDB_LEN);
    }

    if (SLN_AUTH_OK == status)
    {
        // Check if PRDBx is empty
        bool isEmpty = false;
        
        isEmpty = is_buffer_empty(tempPRDB, PRDB_LEN);

        if (isEmpty)
        {
            configPRINTF(("Context %d PRDB empty!\r\n", context));
            status = SLN_AUTH_NO_CONTEXT;
        }
    }

    return status;
}


int32_t SLN_AUTH_swap_context(int32_t context)
{
    int32_t status = SLN_AUTH_OK;
    uint8_t tempKIB[KIB_LEN] = {0};
    uint8_t tempPRDB[PRDB_LEN] = {0};

    if ((SLN_CRYPTO_CTX_NONE >= context) || (SLN_CRYPTO_CTX_COUNT <= context))
    {
        status = SLN_AUTH_INVALID_ARG;
    }

    if (SLN_AUTH_OK == status)
    {
        /* Reading everything including boostrap to ensure we don't brick the device */
        status = SLN_Read_Flash_At_Address(0x0, s_tempBuffer, PARAM_SECTOR);
    }

    if (SLN_AUTH_OK == status)
    {
        /* Only read the KIB data so we keep the boostrap untouched */
        uint32_t backUpKibAddr = BACKUP_REGION + KIB_ADDR(context);
        uint8_t backUpKIB[KIB_LEN] = {0};

        // Read out KIBx
        status = SLN_Read_Flash_At_Address(backUpKibAddr, backUpKIB, KIB_LEN);

        if (SLN_AUTH_OK == status)
        {
            // Copy into s_tempBuffer
            SLN_ram_memcpy(&s_tempBuffer[KIB_ADDR(context)], backUpKIB, KIB_LEN);
        }
    }

    if (SLN_AUTH_OK == status)
    {
        /* Only read the PRDB data so we keep the boostrap untouched */
        uint32_t backUpPrdbAddr = BACKUP_REGION + PRDB_ADDR(context);
        uint8_t backUpPRDB[PRDB_LEN] = {0};

        // Read out PRDBx
        status = SLN_Read_Flash_At_Address(backUpPrdbAddr, backUpPRDB, PRDB_LEN);

        if (SLN_AUTH_OK == status)
        {
            // Copy into s_tempBuffer
            SLN_ram_memcpy(&s_tempBuffer[PRDB_ADDR(context)], backUpPRDB, PRDB_LEN);
        }
    }

    if (SLN_AUTH_OK == status)
    {
        // Read out KIBx
        status = SLN_Read_Flash_At_Address(KIB_ADDR(context), tempKIB, KIB_LEN);
    }

    if (SLN_AUTH_OK == status)
    {
        // Read out PRDBx
        status = SLN_Read_Flash_At_Address(PRDB_ADDR(context), tempPRDB, PRDB_LEN);
    }

    if (SLN_AUTH_OK == status)
    {
        if ((!is_buffer_empty(&s_tempBuffer[KIB_ADDR(context)], KIB_LEN)) || (!is_buffer_empty(&s_tempBuffer[PRDB_ADDR(context)], PRDB_LEN)))
        {
            // Write new data
            status = write_to_nvm(0x0, s_tempBuffer, PARAM_SECTOR);
        }
        else
        {
            status = SLN_AUTH_NO_CONTEXT;
        }
    }

    if (SLN_AUTH_OK == status)
    {
        // Read out contexts
        status = SLN_Read_Flash_At_Address(BACKUP_REGION, s_tempBuffer, PARAM_SECTOR);
    }

    if (SLN_AUTH_OK == status)
    {
        // Copy into s_tempBuffer
        SLN_ram_memcpy(&s_tempBuffer[KIB_ADDR(context)], tempKIB, KIB_LEN);

        // Copy into s_tempBuffer
        SLN_ram_memcpy(&s_tempBuffer[PRDB_ADDR(context)], tempPRDB, PRDB_LEN);
    }

    if (SLN_AUTH_OK == status)
    {
        // Write new data
        status = write_to_nvm(BACKUP_REGION, s_tempBuffer, PARAM_SECTOR);
    }

    return status;
}

int32_t SLN_AUTH_backup_context(int32_t context)
{
    int32_t status = SLN_AUTH_OK;
    uint8_t tempKIB[KIB_LEN] = {0};
    uint8_t tempPRDB[PRDB_LEN] = {0};

    if ((SLN_CRYPTO_CTX_NONE >= context) || (SLN_CRYPTO_CTX_COUNT <= context))
    {
        status = SLN_AUTH_INVALID_ARG;
    }

    if (SLN_AUTH_OK == status)
    {
        // Read out contexts
        status = SLN_Read_Flash_At_Address(BACKUP_REGION, s_tempBuffer, PARAM_SECTOR);
    }

    if (SLN_AUTH_OK == status)
    {
        // Read out KIBx
        status = SLN_Read_Flash_At_Address(KIB_ADDR(context), tempKIB, KIB_LEN);
    }

    if (SLN_AUTH_OK == status)
    {
        // Read out PRDBx
        status = SLN_Read_Flash_At_Address(PRDB_ADDR(context), tempPRDB, PRDB_LEN);
    }

    if (SLN_AUTH_OK == status)
    {
        // Copy into s_tempBuffer
        SLN_ram_memcpy(&s_tempBuffer[KIB_ADDR(context)], tempKIB, KIB_LEN);

        // Copy into s_tempBuffer
        SLN_ram_memcpy(&s_tempBuffer[PRDB_ADDR(context)], tempPRDB, PRDB_LEN);
    }

    if (SLN_AUTH_OK == status)
    {
        // Write new data
        status = write_to_nvm(BACKUP_REGION, s_tempBuffer, PARAM_SECTOR);
    }

    return status;
}

int32_t SLN_AUTH_restore_context(int32_t context)
{
    int32_t status = SLN_AUTH_OK;

    if ((SLN_CRYPTO_CTX_NONE >= context) || (SLN_CRYPTO_CTX_COUNT <= context))
    {
        status = SLN_AUTH_INVALID_ARG;
    }

    if (SLN_AUTH_OK == status)
    {
        // Read out current context configs
        status = SLN_Read_Flash_At_Address(0x0, s_tempBuffer, PARAM_SECTOR);
    }

    if (SLN_AUTH_OK == status)
    {
        uint32_t backUpKibAddr = BACKUP_REGION + KIB_ADDR(context);
        uint8_t backUpKIB[KIB_LEN] = {0};

        // Read out KIBx
        status = SLN_Read_Flash_At_Address(backUpKibAddr, backUpKIB, KIB_LEN);

        if (SLN_AUTH_OK == status)
        {
            // Copy into s_tempBuffer
            SLN_ram_memcpy(&s_tempBuffer[KIB_ADDR(context)], backUpKIB, KIB_LEN);
        }
    }

    if (SLN_AUTH_OK == status)
    {
        uint32_t backUpPrdbAddr = BACKUP_REGION + PRDB_ADDR(context);
        uint8_t backUpPRDB[PRDB_LEN] = {0};

        // Read out PRDBx
        status = SLN_Read_Flash_At_Address(backUpPrdbAddr, backUpPRDB, PRDB_LEN);

        if (SLN_AUTH_OK == status)
        {
            // Copy into s_tempBuffer
            SLN_ram_memcpy(&s_tempBuffer[PRDB_ADDR(context)], backUpPRDB, PRDB_LEN);
        }
    }

    if (SLN_AUTH_OK == status)
    {
        if ((!is_buffer_empty(&s_tempBuffer[KIB_ADDR(context)], KIB_LEN)) || (!is_buffer_empty(&s_tempBuffer[PRDB_ADDR(context)], PRDB_LEN)))
        {
            // Write new data
            status = write_to_nvm(0x0, s_tempBuffer, PARAM_SECTOR);
        }
        else
        {
            status = SLN_AUTH_NO_CONTEXT;
            configPRINTF(("Not copying in an empty context %d\r\n", context));
        }
    }

    return status;
}

int32_t SLN_AUTH_save_context(int32_t context, uint32_t *buffer)
{
    int32_t status = SLN_AUTH_OK;

    if ((SLN_CRYPTO_CTX_NONE >= context) || (SLN_CRYPTO_CTX_COUNT <= context) || (NULL == buffer))
    {
        status = SLN_AUTH_INVALID_ARG;
    }

    if (SLN_AUTH_OK == status)
    {
        // Read out current context configs
        status = SLN_Read_Flash_At_Address(0x0, s_tempBuffer, PARAM_SECTOR);
    }

    if (SLN_AUTH_OK == status)
    {
        // Copy into s_tempBuffer
        SLN_ram_memcpy(&s_tempBuffer[KIB_ADDR(context)], buffer, KIB_LEN + RESV_LEN + PRDB_LEN);
    }

    if (SLN_AUTH_OK == status)
    {
        // Write new data
        status = write_to_nvm(0x0, s_tempBuffer, PARAM_SECTOR);
    }

    return status;
}

int32_t SLN_AUTH_full_save_context(uint32_t *ctx0, uint32_t *ctx1)
{
    int32_t status = SLN_AUTH_OK;

    if ((NULL == ctx0) || (NULL == ctx1))
    {
        status = SLN_AUTH_INVALID_ARG;
    }

    if (SLN_AUTH_OK == status)
    {
        // Read out current context configs
        status = SLN_Read_Flash_At_Address(0x0, s_tempBuffer, PARAM_SECTOR);
    }

    if (SLN_AUTH_OK == status)
    {
        // Copy context 0 into s_tempBuffer
        SLN_ram_memcpy(&s_tempBuffer[KIB_ADDR(0)], ctx0, KIB_LEN + RESV_LEN + PRDB_LEN);

        // Copy context 1 into s_tempBuffer
        SLN_ram_memcpy(&s_tempBuffer[KIB_ADDR(1)], ctx1, KIB_LEN + RESV_LEN + PRDB_LEN);
    }

    if (SLN_AUTH_OK == status)
    {
        // Write new data
        status = write_to_nvm(0x0, s_tempBuffer, PARAM_SECTOR);
    }

    return status;
}


int32_t SLN_AUTH_Verify_Cert(uint8_t *caPem, uint8_t *vfPem)
{
    int32_t ret = SLN_AUTH_OK;  // SLN_AUTH return code
    int32_t status = 0;         // mbedTLS status code
    uint32_t flags = 0;         // mbedTLS verification flags

    mbedtls_x509_crt_init(&s_caChain);
    mbedtls_x509_crt_init(&s_verifCert);

    if ((NULL == caPem) || (NULL == vfPem))
    {
        ret = SLN_AUTH_NULL_PTR;
    }

    if (SLN_AUTH_OK == ret)
    {
        // Parse CA Cert
        status = mbedtls_x509_crt_parse(&s_caChain, caPem, safe_strlen((char *)caPem, MAX_CERT_LEN) + 1);

        if (status)
        {
            configPRINTF(("ERROR: Could not parse CA certificate, -0x%X.\r\n", -status));
            ret = SLN_AUTH_BAD_CERT;
        }
    }

    if (SLN_AUTH_OK == ret)
    {
        // Parse Verification/Signing Cert
        status = mbedtls_x509_crt_parse(&s_verifCert, vfPem, safe_strlen((char *)vfPem, MAX_CERT_LEN) + 1);

        if (status)
        {
            configPRINTF(("ERROR: Could not parse verification certificate, -0x%X.\r\n", -status));
            ret = SLN_AUTH_BAD_CERT2;
        }
    }

    if (SLN_AUTH_OK == ret)
    {
        // Verify Verification/Signing Certificate
        status = mbedtls_x509_crt_verify(&s_verifCert, &s_caChain, NULL, NULL, &flags, NULL, NULL);

        if (status)
        {
            configPRINTF(("ERROR: Could not verify verification certificate, -0x%X.\r\n", -status));
            configPRINTF(("Flags: 0x%X\r\n", flags));
            ret = SLN_AUTH_INVALID_CERT;
        }
    }

    mbedtls_x509_crt_free(&s_caChain);
    mbedtls_x509_crt_free(&s_verifCert);

    return ret;
}

int32_t SLN_AUTH_Verify_Signature(uint8_t *vfPem, uint8_t *msg, size_t msglen, uint8_t *msgsig)
{
    int32_t ret = SLN_AUTH_OK;   // SLN_AUTH return code
    int32_t status = 0;         // mbedTLS status code

    mbedtls_x509_crt_init(&s_verifCert);

    if ((NULL == vfPem) || (NULL == msg) || (NULL == msgsig))
    {
        ret = SLN_AUTH_NULL_PTR;
    }

    if (SLN_AUTH_OK == ret)
    {
        // Parse Verification Cert
        status = mbedtls_x509_crt_parse(&s_verifCert, vfPem, safe_strlen((char *)vfPem, MAX_CERT_LEN) + 1);

        if (status)
        {
            configPRINTF(("ERROR: Could not parse verification certificate, -0x%X.\r\n", -status));
            ret = SLN_AUTH_BAD_CERT;
        }
    }

    if (SLN_AUTH_OK == ret)
    {
        // SHA256 hash of data
        mbedtls_md_context_t mdCtx;
        mbedtls_md_type_t mdType = MBEDTLS_MD_SHA256;
        mbedtls_md_info_t *mdInfo = (mbedtls_md_info_t *)mbedtls_md_info_from_type(mdType);
        char *hash = NULL;

        mbedtls_md_init(&mdCtx);
        mbedtls_md_setup(&mdCtx, mdInfo, 1);

        // Allocate hash memory
        hash = (char *)pvPortMalloc(mdCtx.md_info->size);

        if (NULL == hash)
        {
            configPRINTF(("ERROR: Could not allocate memory for hash.\r\n"));
            ret = SLN_AUTH_NO_MEM;
        }

        if (SLN_AUTH_OK == ret)
        {
            // Hash message
            mbedtls_md(mdCtx.md_info, msg, msglen, (uint8_t *)hash);

            // Now verify the signature for the given hash of the data
            status = mbedtls_pk_verify(&(s_verifCert.pk), mdCtx.md_info->type, (uint8_t *)hash, mdCtx.md_info->size, msgsig, RSA_SIG_LEN);

            if (status)
            {
                configPRINTF(("ERROR: Could not authenticate message, -0x%X.\r\n", -status));
                ret = SLN_AUTH_INVALID_SIG;
            }
        }

        if (NULL != hash)
        {
            for (uint32_t idx = 0; idx < mdCtx.md_info->size; idx++)
            {
                hash[idx] = 0x00; // Wipe hash data
            }
        }
        vPortFree(hash);
        hash = NULL;

        mbedtls_md_free(&mdCtx);
    }

    mbedtls_x509_crt_free(&s_verifCert);

    return ret;
}

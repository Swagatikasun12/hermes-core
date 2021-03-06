/*
* Copyright (c) 2017 Cossack Labs Limited
*
* This file is a part of Hermes-core.
*
* Hermes-core is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Hermes-core is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
*
*/



#ifndef HERMES_KEY_STORE_FUNCTION_H
#define HERMES_KEY_STORE_FUNCTION_H

#include <hermes/rpc/param_pack.h>
#include <hermes/rpc/client.h>
#include <hermes/key_store/db.h>

#include <stdint.h>
#include <stdlib.h>

// hm_key_store_check_backend return HM_SUCCESS if backend hasn't NULL on all functions that expected by interface
// otherwise HM_FAIL
// should be used on server creation
uint32_t hm_key_store_check_backend(hm_ks_db_t *db);

#define hm_key_store_set_rtoken_NAME "hm_key_store_set_rtoken"

uint32_t hm_key_store_set_rtoken(
        hm_ks_db_t *db,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length,
        const uint8_t *read_token, const size_t read_token_length);

#define hm_key_store_set_wtoken_NAME "hm_key_store_set_wtoken"

uint32_t hm_key_store_set_wtoken(
        hm_ks_db_t *db,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length,
        const uint8_t *wtoken, const size_t wtoken_length);

#define hm_key_store_get_rtoken_NAME "hm_key_store_get_rtoken"

uint32_t hm_key_store_get_rtoken(
        hm_ks_db_t *db,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        uint8_t **read_token, size_t *read_token_length,
        uint8_t **owner_id, size_t *owner_id_length);

#define hm_key_store_get_wtoken_NAME "hm_key_store_get_wtoken"

uint32_t hm_key_store_get_wtoken(
        hm_ks_db_t *db,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        uint8_t **write_token, size_t *write_token_length,
        uint8_t **owner_id, size_t *owner_id_length);

#define hm_key_store_del_rtoken_NAME "hm_key_store_del_rtoken"

uint32_t hm_key_store_del_rtoken(
        hm_ks_db_t *db,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length);

#define hm_key_store_del_wtoken_NAME "hm_key_store_del_wtoken"

uint32_t hm_key_store_del_wtoken(
        hm_ks_db_t *db,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length);

#define hm_key_store_get_indexed_rights_NAME "hm_key_store_get_indexed_rights"

uint32_t hm_key_store_get_indexed_rights(
        hm_ks_db_t *db,
        const uint8_t *block_id, const size_t block_id_length,
        const size_t index, uint8_t **user_id,
        size_t *user_id_length, uint32_t *rights_mask);

//proxies
uint32_t hm_key_store_set_rtoken_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length,
        const uint8_t *read_token, const size_t read_token_length);

uint32_t hm_key_store_set_wtoken_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length,
        const uint8_t *write_token, const size_t write_token_length);

uint32_t hm_key_store_get_rtoken_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        uint8_t **read_token, size_t *read_token_length,
        uint8_t **owner_id, size_t *owner_id_length);

uint32_t hm_key_store_get_wtoken_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        uint8_t **write_token, size_t *write_token_length,
        uint8_t **owner_id, size_t *owner_id_length);

uint32_t hm_key_store_del_rtoken_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length);

uint32_t hm_key_store_del_wtoken_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length);

uint32_t hm_key_store_get_indexed_rights_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block_id, const size_t block_id_length,
        const size_t index, uint8_t **user_id,
        size_t *user_id_length, uint32_t *rights_mask);

//stubs
uint32_t hm_key_store_set_rtoken_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data);

uint32_t hm_key_store_set_wtoken_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data);

uint32_t hm_key_store_get_rtoken_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data);

uint32_t hm_key_store_get_wtoken_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data);

uint32_t hm_key_store_del_rtoken_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data);

uint32_t hm_key_store_del_wtoken_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data);

uint32_t hm_key_store_get_indexed_rights_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data);

#endif //HERMES_KEY_STORE_FUNCTION_H

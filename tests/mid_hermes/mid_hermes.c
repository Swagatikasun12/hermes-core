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


#include <common/test_utils.h>

#include <hermes/common/errors.h>
#include <themis/themis.h> 

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include <pthread.h>

#include <assert.h>
#include "../common/common.h"
#include "../common/test_transport.h"
#include "../common/test_data_store_db.h"  
#include "../common/test_key_store_db.h"  
#include "../common/test_credential_store_db.h"  

#include <hermes/data_store/server.h>
#include <hermes/key_store/server.h>
#include <hermes/credential_store/server.h>
#include <hermes/mid_hermes/mid_hermes.h>



#define CS_PIPE_NAME "/tmp/hermes_core_test_cs_pipe" 
#define SC_PIPE_NAME "/tmp/hermes_core_test_sc_pipe"
#define DC_PIPE_NAME "/tmp/hermes_core_test_ds_pipe" 
#define CD_PIPE_NAME "/tmp/hermes_core_test_sd_pipe"
#define KC_PIPE_NAME "/tmp/hermes_core_test_ks_pipe" 
#define CK_PIPE_NAME "/tmp/hermes_core_test_sk_pipe"

#define MAX_USERS_IN_TESTS 16
#define MAX_BLOCKS_IN_TESTS 256
#define MAX_COMMAND_LENGTH 256
#define MAX_KEY_LENGTH 256

void* data_store_server(void* param){
  UNUSED(param);
  hm_rpc_transport_t* transport = hm_test_transport_create(DC_PIPE_NAME, CD_PIPE_NAME, true);
  if(!transport){
    testsuite_fail_if(true, "server transport initializing");
    return (void*)TEST_FAIL;
  }
  hm_ds_db_t* db=hm_test_ds_db_create();
  if(!db){
    testsuite_fail_if(true, "data db initializing");
    hm_test_transport_destroy(transport);
    return (void*)TEST_FAIL;
  }
  hm_data_store_server_t* s=hm_data_store_server_create(transport, db);
  if(!s){
    hm_test_ds_db_destroy(&db);
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "data store server creation");
    return (void*)TEST_FAIL;
  }
  testsuite_fail_if(false, "data store server");
  while(true){
    if(HM_SUCCESS!=hm_data_store_server_call(s)){
      hm_data_store_server_destroy(&s);
      hm_test_ds_db_destroy(&db);
      hm_test_transport_destroy(transport);
      testsuite_fail_if(true, "data store server calling");
      return (void*)TEST_FAIL;
    }
  }
  hm_data_store_server_destroy(&s);
  hm_test_ds_db_destroy(&db);
  hm_test_transport_destroy(transport);
  return (void*)TEST_SUCCESS;
}

void* credential_store_server(void* param){
  UNUSED(param);
  hm_rpc_transport_t* transport = hm_test_transport_create(SC_PIPE_NAME, CS_PIPE_NAME, true);
  if(!transport){
    testsuite_fail_if(true, "server transport initializing");
    return (void*)TEST_FAIL;
  }
  hm_cs_db_t* db=hm_test_cs_db_create();
  if(!db){
    testsuite_fail_if(true, "credential db initializing");
    hm_test_transport_destroy(transport);
    return (void*)TEST_FAIL;
  }
  hm_credential_store_server_t* s=hm_credential_store_server_create(transport, db);
  if(!s){
    hm_test_cs_db_destroy(&db);
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "credential store server creation");
    return (void*)TEST_FAIL;
  }
  testsuite_fail_if(false, "credential store server");
  while(true){
    if(HM_SUCCESS!=hm_credential_store_server_call(s)){
      hm_credential_store_server_destroy(&s);
      hm_test_cs_db_destroy(&db);
      hm_test_transport_destroy(transport);
      testsuite_fail_if(true, "credential store server calling");
      return (void*)TEST_FAIL;
    }
  }
  hm_credential_store_server_destroy(&s);
  hm_test_cs_db_destroy(&db);
  hm_test_transport_destroy(transport);
  return (void*)TEST_SUCCESS;
}

void* key_store_server(void* param){
  UNUSED(param);
  hm_rpc_transport_t* transport = hm_test_transport_create(KC_PIPE_NAME, CK_PIPE_NAME, true);
  if(!transport){
    testsuite_fail_if(true, "server transport initializing");
    return (void*)TEST_FAIL;
  }
  hm_ks_db_t* db=hm_test_ks_db_create();
  if(!db){
    testsuite_fail_if(true, "key db initializing");
    hm_test_transport_destroy(transport);
    return (void*)TEST_FAIL;
  }
  hm_key_store_server_t* s=hm_key_store_server_create(transport, db);
  if(!s){
    hm_test_ks_db_destroy(&db);
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "key store server creation");
    return (void*)TEST_FAIL;
  }
  testsuite_fail_if(false, "key store server");
  while(1){
    if(HM_SUCCESS!=hm_key_store_server_call(s)){
      hm_key_store_server_destroy(&s);
      hm_test_ks_db_destroy(&db);
      hm_test_transport_destroy(transport);
      testsuite_fail_if(true, "key store server calling");
      return (void*)TEST_FAIL;
    }
  }
  hm_key_store_server_destroy(&s);
  hm_test_ks_db_destroy(&db);
  hm_test_transport_destroy(transport);
  return (void*)TEST_SUCCESS;
}

typedef struct user_type{
  uint8_t id[USER_ID_LENGTH];
  uint8_t private_key[MAX_KEY_LENGTH];
  size_t private_key_length;  
}user_t;

typedef struct block_type{
  uint8_t id[BLOCK_ID_LENGTH];
  uint8_t data[MAX_DATA_LENGTH];
  size_t data_length;
  uint8_t meta[MAX_META_LENGTH];
  size_t meta_length;
}block_t;

void gen_block(block_t* b){
  assert(b);
  assert(SOTER_SUCCESS==soter_rand(b->id, BLOCK_ID_LENGTH));
  b->data_length=MAX_DATA_LENGTH;
  assert(SOTER_SUCCESS==soter_rand(b->data, b->data_length));
  b->meta_length=MAX_META_LENGTH;
  assert(SOTER_SUCCESS==soter_rand(b->meta, b->meta_length));
}

typedef struct users_type{
  user_t users[MAX_USERS_IN_TESTS];
} users_t; 

void gen_users(users_t* u){
  int i=1;
  while(i<=MAX_USERS && i<=MAX_USERS_IN_TESTS){
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "find . -maxdepth 1 -name \"*.priv\" | sed '%iq;d' | awk '{printf substr($0, 3, 2*%i)}'", i, USER_ID_LENGTH);
    FILE* f=popen(command, "r");
    if(f){
      if(fgets(command, sizeof(command), f)){
        assert(strlen(command)==2*USER_ID_LENGTH);
        hexdecimal_string_to_bin_array(command, 2*USER_ID_LENGTH, u->users[i-1].id, USER_ID_LENGTH);
        sprintf(command, "%s.priv", command);
        FILE* kf=fopen(command, "rb");
        assert(kf);
        u->users[i-1].private_key_length=fread(u->users[i-1].private_key, 1, MAX_KEY_LENGTH, kf);
        fclose(kf);
      }
      pclose(f);
    }
    ++i;
  }
}

typedef struct blocks_type{
  block_t blocks[MAX_BLOCKS_IN_TESTS];
}blocks_t;

void gen_blocks(blocks_t* b){
  int i=0;
  for(;i<MAX_BLOCKS_IN_TESTS; ++i){
    gen_block(&(b->blocks[i]));
  }
}

void* client(void* param){
  UNUSED(param);
  hm_rpc_transport_t* credential_store_transport = hm_test_transport_create(CS_PIPE_NAME, SC_PIPE_NAME, false);
  hm_rpc_transport_t* data_store_transport = hm_test_transport_create(CD_PIPE_NAME, DC_PIPE_NAME, false);
  hm_rpc_transport_t* key_store_transport = hm_test_transport_create(CK_PIPE_NAME, KC_PIPE_NAME, false);
  sleep(3); //wait for credential store initialisation
  users_t users;
  gen_users(&users);
  blocks_t* blocks = malloc(sizeof(blocks_t));
  gen_blocks(blocks);
  mid_hermes_t* mh=NULL;
  if(!credential_store_transport
     || !data_store_transport
     || !key_store_transport
     || !(mh=mid_hermes_create(
                               users.users[0].id,
                               USER_ID_LENGTH,
                               users.users[0].private_key,
                               users.users[0].private_key_length,
                               key_store_transport,
                               data_store_transport,
                               credential_store_transport))){
    testsuite_fail_if(true, "client transport initializing");
    hm_test_transport_destroy(credential_store_transport);
    hm_test_transport_destroy(data_store_transport);
    hm_test_transport_destroy(key_store_transport);
    return (void*)TEST_FAIL;
  }
  int j=0, i=0;
  uint8_t* id = NULL;
  size_t id_length;
  for(;i<MAX_BLOCKS_IN_TESTS;++i){
    uint8_t* block=blocks->blocks[i].id;
    size_t block_id_length=BLOCK_ID_LENGTH;
    testsuite_fail_if(HM_SUCCESS!=mid_hermes_create_block(mh, &block, &(block_id_length), blocks->blocks[i].data, blocks->blocks[i].data_length, blocks->blocks[i].meta, blocks->blocks[i].meta_length), "data store client sync calling");

    testsuite_fail_if(HM_SUCCESS!=mid_hermes_create_block(mh, &id, &id_length, blocks->blocks[i].data, blocks->blocks[i].data_length, blocks->blocks[i].meta, blocks->blocks[i].meta_length), "data store client sync calling");
    testsuite_fail_if(!id, "block creation didn't return generated id");
    // free allocated memory by mid_hermes_create_block
    free(id);
    id = NULL;
  }
  for(i=1;i<MAX_USERS_IN_TESTS;++i){
    for(j=0;j<(MAX_BLOCKS_IN_TESTS/MAX_USERS_IN_TESTS);++j){
      testsuite_fail_if(HM_SUCCESS!=mid_hermes_grant_read_access(mh, blocks->blocks[i*j].id, BLOCK_ID_LENGTH, users.users[i].id, USER_ID_LENGTH), "data store client sync calling");
    }
  }
  for(i=1;i<MAX_USERS_IN_TESTS;++i){
    for(j=0;j<(MAX_BLOCKS_IN_TESTS/MAX_USERS_IN_TESTS);++j){
      testsuite_fail_if(HM_SUCCESS!=mid_hermes_grant_update_access(mh, blocks->blocks[i*j].id, BLOCK_ID_LENGTH, users.users[i].id, USER_ID_LENGTH), "data store client sync calling");
    }
  }
  for(i=1;i<MAX_USERS_IN_TESTS;++i){
    for(j=0;j<(MAX_BLOCKS_IN_TESTS/MAX_USERS_IN_TESTS);++j){
      testsuite_fail_if(HM_SUCCESS!=mid_hermes_deny_update_access(mh, blocks->blocks[i*j].id, BLOCK_ID_LENGTH, users.users[i].id, USER_ID_LENGTH), "data store client sync calling");
    }
  }
  // free allocated memory by gen_blocks
  free(blocks);
  return (void*)TEST_SUCCESS;
}

int mid_hermes_general_flow(void){
  mkfifo(CS_PIPE_NAME, 0666);
  mkfifo(SC_PIPE_NAME, 0666);
  mkfifo(CD_PIPE_NAME, 0666);
  mkfifo(DC_PIPE_NAME, 0666);
  mkfifo(CK_PIPE_NAME, 0666);
  mkfifo(KC_PIPE_NAME, 0666);
  pthread_t credential_server_thread;
  if(pthread_create(&credential_server_thread, NULL, credential_store_server, NULL)){
    testsuite_fail_if(true, "creating credential server thread");
    return TEST_FAIL;
  }
  pthread_t data_server_thread;
  if(pthread_create(&data_server_thread, NULL, data_store_server, NULL)){
    testsuite_fail_if(true, "creating data server thread");
    return TEST_FAIL;
  }
  pthread_t key_server_thread;
  if(pthread_create(&key_server_thread, NULL, key_store_server, NULL)){
    testsuite_fail_if(true, "creating key server thread");
    return TEST_FAIL;
  }
  pthread_t client_thread;
  if(pthread_create(&client_thread, NULL, client, NULL)){
    testsuite_fail_if(true, "creating client thread");
    return TEST_FAIL;
  }
  int res;
  pthread_join(client_thread, (void**)(&res));
  unlink(CS_PIPE_NAME);
  unlink(SC_PIPE_NAME);
  unlink(CD_PIPE_NAME);
  unlink(DC_PIPE_NAME);
  unlink(CK_PIPE_NAME);
  unlink(KC_PIPE_NAME);
  return res;
}

void mid_hermes_tests(void){
  testsuite_fail_if((bool)mid_hermes_general_flow(), "mid hermes general flow");
}

int main(int argc, char *argv[]){
  UNUSED(argc);
  UNUSED(argv);
  system("rm *.priv");
  testsuite_start_testing();
  testsuite_enter_suite("mid hermes test");

  testsuite_run_test(mid_hermes_tests);

  testsuite_finish_testing();
  system("rm *.priv");
  return testsuite_get_return_value();
}

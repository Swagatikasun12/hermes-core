/*
 * Copyright (c) 2017 Cossack Labs Limited
 *
 * This file is part of Hermes.
 *
 * Hermes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hermes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Hermes.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <common/test_utils.h>

#include <common/test_utils.h>
#include <hermes/common/errors.h>

#include <hermes/credential_store/server.h>
#include <hermes/credential_store/client.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include <pthread.h>

#include "../common/test_transport.h"
#include "../common/test_credential_store_db.h"  

#define CS_PIPE_NAME "/tmp/hermes_core_test_cs_pipe" 
#define SC_PIPE_NAME "/tmp/hermes_core_test_sc_pipe"
#define CS_TEST_DB_FILE_NAME "tests/cs_test_db"
#define CS_TEST_CORRECT_ID1 "user1"
#define CS_TEST_CORRECT_ID1_KEY "user1_public_key_data"
#define CS_TEST_CORRECT_ID2 "user2"
#define CS_TEST_CORRECT_ID2_KEY "user2_public_key_data"
#define CS_TEST_CORRECT_ID3 "user3"
#define CS_TEST_CORRECT_ID3_KEY "user3_public_key_data"
#define CS_TEST_INCORRECT_ID "user4"

void* server(void* param){
  hm_rpc_transport_t* transport = hm_test_transport_create(SC_PIPE_NAME, CS_PIPE_NAME, true);
  if(!transport){
    testsuite_fail_if(true, "server transport initializing");
    return (void*)1;
  }
  hm_cs_db_t* db=hm_test_cs_db_create(CS_TEST_DB_FILE_NAME);
  if(!db){
    hm_test_transport_destroy(transport);
    return (void*)1;
  }
  hm_credential_store_server_t* s=hm_credential_store_server_create(transport, db);
  if(!s){
    hm_test_cs_db_destroy(&db);
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "credential store server creation");
    return (void*)1;
  }
  int i=0;
  for(;i<4;++i){
    if(HM_SUCCESS!=hm_credential_store_server_call(s)){
      hm_credential_store_server_destroy(&s);
      hm_test_cs_db_destroy(&db);
      hm_test_transport_destroy(transport);
      testsuite_fail_if(true, "credential store server calling");
      return (void*)1;
    }
  }
  hm_credential_store_server_destroy(&s);
  hm_test_cs_db_destroy(&db);
  hm_test_transport_destroy(transport);
  return NULL;
}

void* client(void* param){
  hm_rpc_transport_t* transport = hm_test_transport_create(CS_PIPE_NAME, SC_PIPE_NAME, false);
  if(!transport){
    testsuite_fail_if(true, "client transport initializing");
    return (void*)1;
  }
  hm_credential_store_client_sync_t* c=hm_credential_store_client_sync_create(transport);
  if(!c){
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "credential store client sync creation");
    return (void*)1;
  }
  uint8_t* key=NULL;
  size_t key_length=0;
  if(HM_SUCCESS!=hm_crerential_store_client_sync_call_get_pub_key_by_id(c, (const uint8_t*)CS_TEST_CORRECT_ID1, sizeof(CS_TEST_CORRECT_ID1), &key, &key_length)){
    hm_credential_store_client_sync_destroy(&c);
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "credential store client sync calling");
    return (void*)1;
  }
  if(0!=strcmp(CS_TEST_CORRECT_ID1_KEY, (const char*)key)){
    testsuite_fail_if(true, "credential store client sync res");    
  }
  if(HM_SUCCESS!=hm_crerential_store_client_sync_call_get_pub_key_by_id(c, (const uint8_t*)CS_TEST_CORRECT_ID2, sizeof(CS_TEST_CORRECT_ID2), &key, &key_length)){
    hm_credential_store_client_sync_destroy(&c);
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "credential store client sync calling");
    return (void*)1;
  }
  if(0!=strcmp(CS_TEST_CORRECT_ID2_KEY, (const char*)key)){
    testsuite_fail_if(true, "credential store client sync res");    
  }
  if(HM_SUCCESS==hm_crerential_store_client_sync_call_get_pub_key_by_id(c, (const uint8_t*)CS_TEST_INCORRECT_ID, sizeof(CS_TEST_INCORRECT_ID), &key, &key_length)){
    hm_credential_store_client_sync_destroy(&c);
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "credential store client sync calling with incorrect id");
    return (void*)1;
  }
  if(HM_SUCCESS!=hm_crerential_store_client_sync_call_get_pub_key_by_id(c, (const uint8_t*)CS_TEST_CORRECT_ID3, sizeof(CS_TEST_CORRECT_ID3), &key, &key_length)){
    hm_credential_store_client_sync_destroy(&c);
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "credential store client sync calling");
    return (void*)1;
  }
  if(0!=strcmp(CS_TEST_CORRECT_ID3_KEY, (const char*)key)){
    testsuite_fail_if(true, "credential store client sync res");    
  }
  //  free(key);
  hm_credential_store_client_sync_destroy(&c);
  hm_test_transport_destroy(transport);
  return NULL;
}

int credential_store_general_flow(){
  mkfifo(SC_PIPE_NAME, 0666);
  mkfifo(CS_PIPE_NAME, 0666);
  pthread_t client_thread;
  if(pthread_create(&client_thread, NULL, client, NULL)){
    testsuite_fail_if(true, "creating client thread");
    return -11;
  }
  pthread_t server_thread;
  if(pthread_create(&server_thread, NULL, server, NULL)){
    testsuite_fail_if(true, "creating server thread");
    return -11;
  }
  int res1, res2;
  pthread_join(server_thread, (void**)(&res1));
  pthread_join(client_thread, (void**)(&res2));
  unlink(SC_PIPE_NAME);
  unlink(CS_PIPE_NAME);
  if(res1 || res2){
    return -1;
  }
  return 0;
}

void credential_store_tests(){
  testsuite_fail_if(credential_store_general_flow(), "credential store general flow");
}

int main(int argc, char *argv[]){
  testsuite_start_testing();
  testsuite_enter_suite("credential_store test");

  testsuite_run_test(credential_store_tests);

  testsuite_finish_testing();
  return testsuite_get_return_value();
}

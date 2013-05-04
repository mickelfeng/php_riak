/*
   Copyright 2013 Trifork A/S
   Author: Kaspar Bach Pedersen

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <riack.h>

#define PHP_RIAK_EXTNAME "riak"
#define PHP_RIAK_VERSION "0.1"

/////////////////////////////////////////////////
// Macros

#define PUSH_PARAM(arg) zend_vm_stack_push(arg TSRMLS_CC)
#define POP_PARAM() (void)zend_vm_stack_pop(TSRMLS_C)
#define PUSH_EO_PARAM()
#define POP_EO_PARAM()
 
#define CALL_METHOD_BASE(classname, name) zim_##classname##_##name
 
#define CALL_METHOD_HELPER(classname, name, retval, thisptr, num, param) \
  PUSH_PARAM(param); PUSH_PARAM((void*)num);                            \
  PUSH_EO_PARAM();                                                      \
  CALL_METHOD_BASE(classname, name)(num, retval, NULL, thisptr, 0 TSRMLS_CC); \
  POP_EO_PARAM();                       \
  POP_PARAM(); POP_PARAM();
 
#define CALL_METHOD(classname, name, retval, thisptr)                  \
  CALL_METHOD_BASE(classname, name)(0, retval, NULL, thisptr, 0 TSRMLS_CC);
 
#define CALL_METHOD1(classname, name, retval, thisptr, param1)         \
  CALL_METHOD_HELPER(classname, name, retval, thisptr, 1, param1);
 
#define CALL_METHOD2(classname, name, retval, thisptr, param1, param2) \
  PUSH_PARAM(param1);                                                   \
  CALL_METHOD_HELPER(classname, name, retval, thisptr, 2, param2);     \
  POP_PARAM();
 
#define CALL_METHOD3(classname, name, retval, thisptr, param1, param2, param3) \
  PUSH_PARAM(param1); PUSH_PARAM(param2);                               \
  CALL_METHOD_HELPER(classname, name, retval, thisptr, 3, param3);     \
  POP_PARAM(); POP_PARAM();

#define CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(CLIENT, STATUS) if ( STATUS != RIACK_SUCCESS) { throw_exception(CLIENT,  STATUS TSRMLS_CC); return; }

#define HASH_GET_INTO_RIACK_STRING_OR_ELSE(HT, KEY, ZVAL_PP, RIACK_STR) if ((zend_hash_find(HT, KEY, sizeof(KEY), (void**)&ZVAL_PP) == SUCCESS) \
     && (Z_TYPE_P(*ZVAL_PP) == IS_STRING)) { \
       RIACK_STR.len = Z_STRLEN_P(*ZVAL_PP); \
       RIACK_STR.value = Z_STRVAL_P(*ZVAL_PP); } else

/////////////////////////////////////////////////
// Functions

PHP_MINIT_FUNCTION(riak);
PHP_MSHUTDOWN_FUNCTION(riak);
	
void le_riack_clients_pefree(zend_rsrc_list_entry *rsrc TSRMLS_DC);
void throw_exception(struct RIACK_CLIENT* client, int errorStatus TSRMLS_DC);

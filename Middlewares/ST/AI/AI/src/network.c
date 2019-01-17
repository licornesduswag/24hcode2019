/**
  ******************************************************************************
  * @file    network.c
  * @author  AST Embedded Analytics Research Platform
  * @date    Fri Jan 18 11:46:45 2019
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */



#include "ai_network.h"

#include "ai_platform_interface.h"
#include "ai_math_helpers.h"

#include "core_common.h"
#include "layers.h"

#undef AI_TOOLS_VERSION_MAJOR
#undef AI_TOOLS_VERSION_MINOR
#undef AI_TOOLS_VERSION_MICRO
#define AI_TOOLS_VERSION_MAJOR 3
#define AI_TOOLS_VERSION_MINOR 3
#define AI_TOOLS_VERSION_MICRO 0

#undef AI_TOOLS_API_VERSION_MAJOR
#undef AI_TOOLS_API_VERSION_MINOR
#undef AI_TOOLS_API_VERSION_MICRO
#define AI_TOOLS_API_VERSION_MAJOR 1
#define AI_TOOLS_API_VERSION_MINOR 1
#define AI_TOOLS_API_VERSION_MICRO 0

#undef AI_NET_OBJ_INSTANCE
#define AI_NET_OBJ_INSTANCE g_network
 
#undef AI_NETWORK_MODEL_SIGNATURE
#define AI_NETWORK_MODEL_SIGNATURE     "ca0e6feba5c9197af6ae5e1a95a7b79b"

#ifndef AI_TOOLS_REVISION_ID
#define AI_TOOLS_REVISION_ID     "(rev-)"
#endif

#undef AI_TOOLS_DATE_TIME
#define AI_TOOLS_DATE_TIME   "Fri Jan 18 11:46:45 2019"

#undef AI_TOOLS_COMPILE_TIME
#define AI_TOOLS_COMPILE_TIME    __DATE__ " " __TIME__

#undef AI_NETWORK_N_BATCHES
#define AI_NETWORK_N_BATCHES         (1)

/**  Forward network declaration section  *************************************/
AI_STATIC ai_network AI_NET_OBJ_INSTANCE;


/**  Forward network arrays declarations  *************************************/
AI_STATIC ai_array input_0_output_array;   /* Array #0 */
AI_STATIC ai_array Hidden_Layer_output_array;   /* Array #1 */
AI_STATIC ai_array Hidden_Layer_nl_output_array;   /* Array #2 */
AI_STATIC ai_array Output_Layer_output_array;   /* Array #3 */
AI_STATIC ai_array Output_Layer_nl_output_array;   /* Array #4 */


/**  Forward network tensors declarations  ************************************/
AI_STATIC ai_tensor input_0_output;   /* Tensor #0 */
AI_STATIC ai_tensor Hidden_Layer_output;   /* Tensor #1 */
AI_STATIC ai_tensor Hidden_Layer_nl_output;   /* Tensor #2 */
AI_STATIC ai_tensor Output_Layer_output;   /* Tensor #3 */
AI_STATIC ai_tensor Output_Layer_nl_output;   /* Tensor #4 */


/**  Forward network tensor chain declarations  *******************************/
AI_STATIC_CONST ai_tensor_chain Hidden_Layer_chain;   /* Chain #0 */
AI_STATIC_CONST ai_tensor_chain Hidden_Layer_nl_chain;   /* Chain #1 */
AI_STATIC_CONST ai_tensor_chain Output_Layer_chain;   /* Chain #2 */
AI_STATIC_CONST ai_tensor_chain Output_Layer_nl_chain;   /* Chain #3 */


/**  Subgraph network operators tensor chain declarations  *********************************/


/**  Subgraph network operators declarations  *********************************/


/**  Forward network layers declarations  *************************************/
AI_STATIC ai_layer_dense Hidden_Layer_layer; /* Layer #0 */
AI_STATIC ai_layer_nl Hidden_Layer_nl_layer; /* Layer #1 */
AI_STATIC ai_layer_dense Output_Layer_layer; /* Layer #2 */
AI_STATIC ai_layer_nl Output_Layer_nl_layer; /* Layer #3 */


/**  Arrays declarations section  *********************************************/
AI_ARRAY_OBJ_DECLARE(
  input_0_output_array, AI_DATA_FORMAT_FLOAT, 
  NULL, NULL, 784,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  Hidden_Layer_output_array, AI_DATA_FORMAT_FLOAT, 
  NULL, NULL, 200,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  Hidden_Layer_nl_output_array, AI_DATA_FORMAT_FLOAT, 
  NULL, NULL, 200,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  Output_Layer_output_array, AI_DATA_FORMAT_FLOAT, 
  NULL, NULL, 12,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  Output_Layer_nl_output_array, AI_DATA_FORMAT_FLOAT, 
  NULL, NULL, 12,
  AI_STATIC)


/**  Activations tensors declaration section  *********************************/
AI_TENSOR_OBJ_DECLARE(
  input_0_output,
  AI_SHAPE_INIT(1, 1, 784, 1),
  AI_STRIDE_INIT(3136, 3136, 4, 4),
  &input_0_output_array,
  AI_STATIC)
AI_TENSOR_OBJ_DECLARE(
  Hidden_Layer_output,
  AI_SHAPE_INIT(1, 1, 200, 1),
  AI_STRIDE_INIT(800, 800, 4, 4),
  &Hidden_Layer_output_array,
  AI_STATIC)
AI_TENSOR_OBJ_DECLARE(
  Hidden_Layer_nl_output,
  AI_SHAPE_INIT(1, 1, 200, 1),
  AI_STRIDE_INIT(800, 800, 4, 4),
  &Hidden_Layer_nl_output_array,
  AI_STATIC)
AI_TENSOR_OBJ_DECLARE(
  Output_Layer_output,
  AI_SHAPE_INIT(1, 1, 12, 1),
  AI_STRIDE_INIT(48, 48, 4, 4),
  &Output_Layer_output_array,
  AI_STATIC)
AI_TENSOR_OBJ_DECLARE(
  Output_Layer_nl_output,
  AI_SHAPE_INIT(1, 1, 12, 1),
  AI_STRIDE_INIT(48, 48, 4, 4),
  &Output_Layer_nl_output_array,
  AI_STATIC)





/* Layer #0: "Hidden_Layer" (Dense) */
  

/* Weight tensor #1 */
AI_ARRAY_OBJ_DECLARE(
  Hidden_Layer_weights_array, AI_DATA_FORMAT_FLOAT, 
  NULL, NULL, 156800,
  AI_STATIC)

AI_TENSOR_OBJ_DECLARE(
  Hidden_Layer_weights,
  AI_SHAPE_INIT(1, 1, 200, 784),
  AI_STRIDE_INIT(627200, 627200, 3136, 4),
  &Hidden_Layer_weights_array,
  AI_STATIC)

/* Weight tensor #2 */
AI_ARRAY_OBJ_DECLARE(
  Hidden_Layer_bias_array, AI_DATA_FORMAT_FLOAT, 
  NULL, NULL, 200,
  AI_STATIC)

AI_TENSOR_OBJ_DECLARE(
  Hidden_Layer_bias,
  AI_SHAPE_INIT(1, 1, 200, 1),
  AI_STRIDE_INIT(800, 800, 4, 4),
  &Hidden_Layer_bias_array,
  AI_STATIC)


AI_TENSOR_CHAIN_OBJ_DECLARE(
  Hidden_Layer_chain, AI_STATIC_CONST, 
  AI_TENSOR_LIST_ENTRY(&input_0_output),
  AI_TENSOR_LIST_ENTRY(&Hidden_Layer_output),
  AI_TENSOR_LIST_ENTRY(&Hidden_Layer_weights, &Hidden_Layer_bias),
  AI_TENSOR_LIST_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  Hidden_Layer_layer, 0,
  DENSE_TYPE,
  dense, forward_dense,
  &AI_NET_OBJ_INSTANCE, &Hidden_Layer_nl_layer, AI_STATIC,
  .tensors = &Hidden_Layer_chain, 
)

/* Layer #1: "Hidden_Layer_nl" (Nonlinearity) */
  


AI_TENSOR_CHAIN_OBJ_DECLARE(
  Hidden_Layer_nl_chain, AI_STATIC_CONST, 
  AI_TENSOR_LIST_ENTRY(&Hidden_Layer_output),
  AI_TENSOR_LIST_ENTRY(&Hidden_Layer_nl_output),
  AI_TENSOR_LIST_EMPTY,
  AI_TENSOR_LIST_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  Hidden_Layer_nl_layer, 0,
  NL_TYPE,
  nl, forward_sigmoid,
  &AI_NET_OBJ_INSTANCE, &Output_Layer_layer, AI_STATIC,
  .tensors = &Hidden_Layer_nl_chain, 
)

/* Layer #2: "Output_Layer" (Dense) */
  

/* Weight tensor #1 */
AI_ARRAY_OBJ_DECLARE(
  Output_Layer_weights_array, AI_DATA_FORMAT_FLOAT, 
  NULL, NULL, 2400,
  AI_STATIC)

AI_TENSOR_OBJ_DECLARE(
  Output_Layer_weights,
  AI_SHAPE_INIT(1, 1, 12, 200),
  AI_STRIDE_INIT(9600, 9600, 800, 4),
  &Output_Layer_weights_array,
  AI_STATIC)

/* Weight tensor #2 */
AI_ARRAY_OBJ_DECLARE(
  Output_Layer_bias_array, AI_DATA_FORMAT_FLOAT, 
  NULL, NULL, 12,
  AI_STATIC)

AI_TENSOR_OBJ_DECLARE(
  Output_Layer_bias,
  AI_SHAPE_INIT(1, 1, 12, 1),
  AI_STRIDE_INIT(48, 48, 4, 4),
  &Output_Layer_bias_array,
  AI_STATIC)


AI_TENSOR_CHAIN_OBJ_DECLARE(
  Output_Layer_chain, AI_STATIC_CONST, 
  AI_TENSOR_LIST_ENTRY(&Hidden_Layer_nl_output),
  AI_TENSOR_LIST_ENTRY(&Output_Layer_output),
  AI_TENSOR_LIST_ENTRY(&Output_Layer_weights, &Output_Layer_bias),
  AI_TENSOR_LIST_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  Output_Layer_layer, 1,
  DENSE_TYPE,
  dense, forward_dense,
  &AI_NET_OBJ_INSTANCE, &Output_Layer_nl_layer, AI_STATIC,
  .tensors = &Output_Layer_chain, 
)

/* Layer #3: "Output_Layer_nl" (Nonlinearity) */
  


AI_TENSOR_CHAIN_OBJ_DECLARE(
  Output_Layer_nl_chain, AI_STATIC_CONST, 
  AI_TENSOR_LIST_ENTRY(&Output_Layer_output),
  AI_TENSOR_LIST_ENTRY(&Output_Layer_nl_output),
  AI_TENSOR_LIST_EMPTY,
  AI_TENSOR_LIST_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  Output_Layer_nl_layer, 1,
  NL_TYPE,
  nl, forward_sigmoid,
  &AI_NET_OBJ_INSTANCE, &Output_Layer_nl_layer, AI_STATIC,
  .tensors = &Output_Layer_nl_chain, 
)


AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE,
  AI_BUFFER_OBJ_INIT(AI_BUFFER_FORMAT_U8,
                     1, 1, 637648, 1,
                     NULL),
  AI_BUFFER_OBJ_INIT(AI_BUFFER_FORMAT_U8,
                     1, 1, 852, 1,
                     NULL),
  &input_0_output, &Output_Layer_nl_output,
  &Hidden_Layer_layer, 0)


AI_DECLARE_STATIC
ai_bool network_configure_activations(
  ai_network* net_ctx, const ai_buffer* activation_buffer)
{
  AI_ASSERT(net_ctx &&  activation_buffer && activation_buffer->data)

  ai_ptr activations = AI_PTR(AI_PTR_ALIGN(activation_buffer->data, 4));
  AI_ASSERT( activations )
  AI_FLAG_SET(net_ctx->flags, AI_NETWORK_FLAG_OUT_COPY);

  {
    /* Updating activations (byte) offsets */
    input_0_output_array.data = NULL;
  input_0_output_array.data_start = NULL;
  Hidden_Layer_output_array.data = activations + 0;
  Hidden_Layer_output_array.data_start = activations + 0;
  Hidden_Layer_nl_output_array.data = activations + 0;
  Hidden_Layer_nl_output_array.data_start = activations + 0;
  Output_Layer_output_array.data = activations + 800;
  Output_Layer_output_array.data_start = activations + 800;
  Output_Layer_nl_output_array.data = activations + 800;
  Output_Layer_nl_output_array.data_start = activations + 800;
  
  }
  return true;
}

AI_DECLARE_STATIC
ai_bool network_configure_weights(
  ai_network* net_ctx, const ai_buffer* weights_buffer)
{
  AI_ASSERT(net_ctx &&  weights_buffer && weights_buffer->data)

  ai_ptr weights = AI_PTR(weights_buffer->data);
  AI_ASSERT( weights )

  {
    /* Updating weights (byte) offsets */
    Hidden_Layer_weights_array.format |= AI_FMT_FLAG_CONST;
  Hidden_Layer_weights_array.data = weights + 0;
  Hidden_Layer_weights_array.data_start = weights + 0;
  Hidden_Layer_bias_array.format |= AI_FMT_FLAG_CONST;
  Hidden_Layer_bias_array.data = weights + 627200;
  Hidden_Layer_bias_array.data_start = weights + 627200;
  Output_Layer_weights_array.format |= AI_FMT_FLAG_CONST;
  Output_Layer_weights_array.data = weights + 628000;
  Output_Layer_weights_array.data_start = weights + 628000;
  Output_Layer_bias_array.format |= AI_FMT_FLAG_CONST;
  Output_Layer_bias_array.data = weights + 637600;
  Output_Layer_bias_array.data_start = weights + 637600;
  
  }

  return true;
}

/**  PUBLIC APIs SECTION  *****************************************************/

AI_API_ENTRY
ai_bool ai_network_get_info(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if ( report && net_ctx )
  {
    ai_network_report r = {
      .model_name        = AI_NETWORK_MODEL_NAME,
      .model_signature   = AI_NETWORK_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = {AI_TOOLS_API_VERSION_MAJOR, AI_TOOLS_API_VERSION_MINOR,
                            AI_TOOLS_API_VERSION_MICRO, 0x0},

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 161320,
      .n_inputs          = AI_NETWORK_IN_NUM,
      .inputs            = AI_BUFFER_OBJ_INIT(
                              AI_BUFFER_FORMAT_FLOAT,
                              1,
                              1,
                              784,
                              1, NULL),
      .n_outputs         = AI_NETWORK_OUT_NUM,
      .outputs           = AI_BUFFER_OBJ_INIT(
                              AI_BUFFER_FORMAT_FLOAT,
                              1,
                              1,
                              12,
                              1, NULL),
      .activations       = net_ctx->activations,
      .weights           = net_ctx->params,
      .n_nodes           = 0,
      .signature         = net_ctx->signature,
    };

    AI_FOR_EACH_NODE_DO(node, net_ctx->input_node)
    {
      r.n_nodes++;
    }

    *report = r;

    return ( r.n_nodes>0 ) ? true : false;
  }
  
  return false;
}

AI_API_ENTRY
ai_error ai_network_get_error(ai_handle network)
{
  return ai_platform_network_get_error(network);
}

AI_API_ENTRY
ai_error ai_network_create(
  ai_handle* network, const ai_buffer* network_config)
{
  return ai_platform_network_create(
    network, network_config, 
    &AI_NET_OBJ_INSTANCE,
    AI_TOOLS_API_VERSION_MAJOR, AI_TOOLS_API_VERSION_MINOR, AI_TOOLS_API_VERSION_MICRO);
}

AI_API_ENTRY
ai_handle ai_network_destroy(ai_handle network)
{
  return ai_platform_network_destroy(network);
}

AI_API_ENTRY
ai_bool ai_network_init(
  ai_handle network, const ai_network_params* params)
{
  ai_network* net_ctx = ai_platform_network_init(network, params);
  if ( !net_ctx ) return false;

  ai_bool ok = true;
  ok &= network_configure_weights(net_ctx, &params->params);
  ok &= network_configure_activations(net_ctx, &params->activations);
  
  return ok;
}


AI_API_ENTRY
ai_i32 ai_network_run(
  ai_handle network, const ai_buffer* input, ai_buffer* output)
{
  return ai_platform_network_process(network, input, output);
}

AI_API_ENTRY
ai_i32 ai_network_forward(ai_handle network, const ai_buffer* input)
{
  return ai_platform_network_process(network, input, NULL);
}

#undef AI_NETWORK_MODEL_SIGNATURE
#undef AI_NET_OBJ_INSTANCE
#undef AI_TOOLS_VERSION_MAJOR
#undef AI_TOOLS_VERSION_MINOR
#undef AI_TOOLS_VERSION_MICRO
#undef AI_TOOLS_API_VERSION_MAJOR
#undef AI_TOOLS_API_VERSION_MINOR
#undef AI_TOOLS_API_VERSION_MICRO
#undef AI_TOOLS_DATE_TIME
#undef AI_TOOLS_COMPILE_TIME


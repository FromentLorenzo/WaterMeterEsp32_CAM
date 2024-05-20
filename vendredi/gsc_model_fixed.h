#define SINGLE_FILE
/**
  ******************************************************************************
  * @file    defines.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, Université Côte d'Azur, LEAT, France
  * @version 2.1.0
  * @date    10 january 2024
  * @brief   Global C pre-processor definitions to use to build all source files (incl. CMSIS-NN)
  */

/* CMSIS-NN round mode definition */
#if defined(WITH_CMSIS_NN) || defined(WITH_NMSIS_NN)


#define ARM_NN_TRUNCATE 1
#define RISCV_NN_TRUNCATE 1

#endif // defined(WITH_CMSIS_NN) || defined(WITH_NMSIS_NN)
/**
  ******************************************************************************
  * @file    number.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 1.0.0
  * @date    2 february 2021
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NUMBER_H__
#define __NUMBER_H__

#include <stdint.h>
#include <stddef.h>
#include <math.h>

#ifdef TRAPV_SHIFT
#include <limits.h>
#include <stdio.h>
#include <assert.h>
#endif

#ifdef WITH_CMSIS_NN
#include "arm_nnfunctions.h"
#endif

#define _clamp_to(type, number) clamp_to_number_t_ ## type (number)
#define clamp_to(type, number) _clamp_to(type, number)
#define _scale(type, number, scale_factor, round_mode) scale_number_t_ ## type (number, scale_factor, round_mode)
#define scale(type, number, scale_factor, round_mode) _scale(type, number, scale_factor, round_mode)
#define _scale_and_clamp_to(type, number, scale_factor, round_mode) scale_and_clamp_to_number_t_ ## type (number, scale_factor, round_mode)
#define scale_and_clamp_to(type, number, scale_factor, round_mode) _scale_and_clamp_to(type, number, scale_factor, round_mode)

typedef enum {
  ROUND_MODE_NONE,
  ROUND_MODE_FLOOR,
  ROUND_MODE_NEAREST,
} round_mode_t;

// Idea 1: Write the smallest min max interval of the net, could be an issue for hybrid int type network
// Idea 2: listing any interval and add type in name in a switch case like <- better but painfull
// #define NUMBER_MIN		// Max value for this numeric type
// #define NUMBER_MAX		// Min value for this numeric type

// // Idea 1: List of all types and write any corresponding function 
// typedef  number_t;		// Standard size numeric type used for weights and activations
// typedef  long_number_t;	// Long numeric type used for intermediate results

#define NUMBER_MIN_INT32_T -2147483648
#define NUMBER_MAX_INT32_T 2147483647

static inline int64_t min_int32_t(
    int64_t a,
    int64_t b) {
	if (a <= b)
		return a;
	return b;
}

static inline int64_t max_int32_t(
    int64_t a,
    int64_t b) {
	if (a >= b)
		return a;
	return b;
}

static inline int64_t scale_number_t_int32_t(
  int64_t number, int scale_factor, round_mode_t round_mode) {


  if (scale_factor <= 0) {
#ifdef TRAPV_SHIFT
    // Check for possible overflow of left shift
    if (number > INT64_MAX >> -scale_factor) {
      fprintf(stderr,
              "Error: scale() overflow, number=%ld, scale_factor=%d, limit=%d\n",
              number,
              scale_factor,
              INT16_MAX >> -scale_factor);
      assert(number <= INT64_MAX >> -scale_factor);
    }
#endif
    // No rounding to apply when shifting left
    return number << - scale_factor;
  } else {
    if (round_mode == ROUND_MODE_NEAREST) {
      number += (1 << (scale_factor - 1)); // +0.5 in fixed-point
    }
    return number >> scale_factor;
  }
}
static inline int32_t clamp_to_number_t_int32_t(
  int64_t number) {
	return (int32_t) max_int32_t(
      NUMBER_MIN_INT32_T,
      min_int32_t(
        NUMBER_MAX_INT32_T, number));
}
static inline int32_t scale_and_clamp_to_number_t_int32_t(
  int64_t number, int scale_factor, round_mode_t round_mode) {
#ifdef WITH_CMSIS_NN
  // Not really CMSIS-NN but use SSAT anyway
  if (scale_factor <= 0) {
    // No rounding to apply when shifting left
    return __SSAT(number << - scale_factor, sizeof(int32_t) * 8);
  } else {
    if (round_mode == ROUND_MODE_NEAREST) {
      number += (1 << (scale_factor - 1)); // +0.5 in fixed-point
    }
    return __SSAT(number >> scale_factor, sizeof(int32_t) * 8);
  }
#else
  number = scale_number_t_int32_t(number, scale_factor, round_mode);
  return clamp_to_number_t_int32_t(number);
#endif
}

#define NUMBER_MIN_INT16_T -32768
#define NUMBER_MAX_INT16_T 32767

static inline int32_t min_int16_t(
    int32_t a,
    int32_t b) {
	if (a <= b)
		return a;
	return b;
}

static inline int32_t max_int16_t(
    int32_t a,
    int32_t b) {
	if (a >= b)
		return a;
	return b;
}

static inline int32_t scale_number_t_int16_t(
  int32_t number, int scale_factor, round_mode_t round_mode) {


  if (scale_factor <= 0) {
#ifdef TRAPV_SHIFT
    // Check for possible overflow of left shift
    if (number > INT32_MAX >> -scale_factor) {
      fprintf(stderr,
              "Error: scale() overflow, number=%d, scale_factor=%d, limit=%d\n",
              number,
              scale_factor,
              INT16_MAX >> -scale_factor);
      assert(number <= INT32_MAX >> -scale_factor);
    }
#endif
    // No rounding to apply when shifting left
    return number << - scale_factor;
  } else {
    if (round_mode == ROUND_MODE_NEAREST) {
      number += (1 << (scale_factor - 1)); // +0.5 in fixed-point
    }
    return number >> scale_factor;
  }
}
static inline int16_t clamp_to_number_t_int16_t(
  int32_t number) {
	return (int16_t) max_int16_t(
      NUMBER_MIN_INT16_T,
      min_int16_t(
        NUMBER_MAX_INT16_T, number));
}
static inline int16_t scale_and_clamp_to_number_t_int16_t(
  int32_t number, int scale_factor, round_mode_t round_mode) {
#ifdef WITH_CMSIS_NN
  // Not really CMSIS-NN but use SSAT anyway
  if (scale_factor <= 0) {
    // No rounding to apply when shifting left
    return __SSAT(number << - scale_factor, sizeof(int16_t) * 8);
  } else {
    if (round_mode == ROUND_MODE_NEAREST) {
      number += (1 << (scale_factor - 1)); // +0.5 in fixed-point
    }
    return __SSAT(number >> scale_factor, sizeof(int16_t) * 8);
  }
#else
  number = scale_number_t_int16_t(number, scale_factor, round_mode);
  return clamp_to_number_t_int16_t(number);
#endif
}




static inline void int64_t_to_float(int64_t * tabint, float * tabfloat, long tabdim, int scale_factor){
  for (int i=0; i<tabdim; i++){
    tabfloat[i] = (float)tabint[i] / (1<<scale_factor);
  }
}

static inline void int32_t_to_float(int32_t * tabint, float * tabfloat, long tabdim, int scale_factor){
  for (int i=0; i<tabdim; i++){
    tabfloat[i] = (float)tabint[i] / (1<<scale_factor);
  }
}

static inline void int16_t_to_float(int16_t * tabint, float * tabfloat, long tabdim, int scale_factor){
  for (int i=0; i<tabdim; i++){
    tabfloat[i] = ((float)tabint[i]) / (1<<scale_factor);
  }
}

static inline void int8_t_to_float(int8_t * tabint, float * tabfloat, long tabdim, int scale_factor){
  for (int i=0; i<tabdim; i++){
    tabfloat[i] = ((float)tabint[i]) / (1<<scale_factor);
  }
}
#endif //__NUMBER_H__

#ifdef __cplusplus
} // extern "C"
#endif
/**
  ******************************************************************************
  * @file    conv2d.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    14 december 2022
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _CONV2D_2_H_
#define _CONV2D_2_H_

#ifndef SINGLE_FILE
#include "number.h"
#endif

#define INPUT_CHANNELS      1
#define INPUT_HEIGHT        28
#define INPUT_WIDTH         28
#define CONV_FILTERS        32
#define CONV_KERNEL_SIZE_Y  3
#define CONV_KERNEL_SIZE_X  3
#define CONV_STRIDE_Y       1
#define CONV_STRIDE_X       1

#define ZEROPADDING_TOP     0
#define ZEROPADDING_BOTTOM  0
#define ZEROPADDING_LEFT    0
#define ZEROPADDING_RIGHT   0

#define CONV_OUTHEIGHT     ( ( (INPUT_HEIGHT - CONV_KERNEL_SIZE_Y + ZEROPADDING_TOP + ZEROPADDING_BOTTOM) / CONV_STRIDE_Y ) + 1 )
#define CONV_OUTWIDTH      ( ( (INPUT_WIDTH - CONV_KERNEL_SIZE_X + ZEROPADDING_LEFT + ZEROPADDING_RIGHT) / CONV_STRIDE_X ) + 1 )


typedef int16_t conv2d_2_output_type[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS];

#if 0
void conv2d_2(
  const number_t input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],               // IN
  const number_t kernel[CONV_FILTERS][CONV_KERNEL_SIZE_X][CONV_KERNEL_SIZE_Y][INPUT_CHANNELS], // IN

  const number_t bias[CONV_FILTERS],						                // IN

  number_t output[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS]);               // OUT
#endif

#undef INPUT_CHANNELS
#undef INPUT_WIDTH
#undef INPUT_HEIGHT
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_STRIDE_X
#undef CONV_STRIDE_Y
#undef ZEROPADDING_TOP
#undef ZEROPADDING_BOTTOM
#undef ZEROPADDING_LEFT
#undef ZEROPADDING_RIGHT
#undef CONV_OUTWIDTH
#undef CONV_OUTHEIGHT

#endif//_CONV2D_2_H_
/**
  ******************************************************************************
  * @file    conv2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 november 2021
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "conv2d_2.h"
#include "number.h"
#endif

#ifdef WITH_CMSIS_NN
#include "arm_nnfunctions.h"
#elif defined(WITH_NMSIS_NN)
#include "riscv_nnfunctions.h"
#endif

#define INPUT_CHANNELS      1
#define INPUT_HEIGHT        28
#define INPUT_WIDTH         28
#define CONV_FILTERS        32
#define CONV_KERNEL_SIZE_Y  3
#define CONV_KERNEL_SIZE_X  3
#define CONV_STRIDE_Y       1
#define CONV_STRIDE_X       1
#define CONV_GROUPS         1
#define CHANNELS_PER_GROUP  (INPUT_CHANNELS / CONV_GROUPS)
#define FILTERS_PER_GROUP   (CONV_FILTERS / CONV_GROUPS)

#define ZEROPADDING_TOP     0
#define ZEROPADDING_BOTTOM  0
#define ZEROPADDING_LEFT    0
#define ZEROPADDING_RIGHT   0

#define CONV_OUTHEIGHT     ( ( (INPUT_HEIGHT - CONV_KERNEL_SIZE_Y + ZEROPADDING_TOP + ZEROPADDING_BOTTOM) / CONV_STRIDE_Y ) + 1 )
#define CONV_OUTWIDTH      ( ( (INPUT_WIDTH - CONV_KERNEL_SIZE_X + ZEROPADDING_LEFT + ZEROPADDING_RIGHT) / CONV_STRIDE_X ) + 1 )

#define ACTIVATION_RELU

// For fixed point quantization
#define WEIGHTS_SCALE_FACTOR 7
#define BIASES_SCALE_FACTOR 7
#define TMP_SCALE_FACTOR 7
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void conv2d_2(
  const NUMBER_T input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],               // IN
  const NUMBER_T kernel[CONV_FILTERS][CONV_KERNEL_SIZE_X][CONV_KERNEL_SIZE_Y][INPUT_CHANNELS / CONV_GROUPS], // IN

  const NUMBER_T bias[CONV_FILTERS],						                // IN

  NUMBER_T output[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS]) {               // OUT

#if !defined(WITH_CMSIS_NN) && !defined(WITH_NMSIS_NN)
  unsigned short pos_x, pos_y, z, k; 	// loop indexes for output volume
  unsigned short x, y;
  int input_x, input_y;
  LONG_NUMBER_T	kernel_mac;
  LONG_NUMBER_T tmp;
  static LONG_NUMBER_T	output_acc[CONV_OUTHEIGHT][CONV_OUTWIDTH];

  for (k = 0; k < CONV_FILTERS; k++) { 
    for (pos_y = 0; pos_y < CONV_OUTHEIGHT; pos_y++) { 
      for (pos_x = 0; pos_x < CONV_OUTWIDTH; pos_x++) { 
        output_acc[pos_y][pos_x] = 0;

        for (z = 0; z < INPUT_CHANNELS / CONV_GROUPS; z++) {
          kernel_mac = 0; 
            
          for (y = 0; y < CONV_KERNEL_SIZE_Y; y++) {
            input_y = pos_y * CONV_STRIDE_Y - ZEROPADDING_TOP + y;

            for (x = 0; x < CONV_KERNEL_SIZE_X; x++) {
              input_x = pos_x * CONV_STRIDE_X - ZEROPADDING_LEFT + x;

              if (input_x < 0 || input_x >= INPUT_WIDTH || input_y < 0 || input_y >= INPUT_HEIGHT) // ZeroPadding2D
                tmp = 0;
              else
                tmp = (LONG_NUMBER_T)input[input_y][input_x][z + (k / FILTERS_PER_GROUP) * CHANNELS_PER_GROUP] * (LONG_NUMBER_T)kernel[k][y][x][z];
              kernel_mac = kernel_mac + tmp;
            }
          }

          output_acc[pos_y][pos_x] = output_acc[pos_y][pos_x] + kernel_mac;

        }
      }
    }

    for (pos_y = 0; pos_y < CONV_OUTHEIGHT; pos_y++) { 
      for (pos_x = 0; pos_x < CONV_OUTWIDTH; pos_x++) { 
        // Scale for possible additional precision of bias
        output_acc[pos_y][pos_x] = scale(NUMBER_T, output_acc[pos_y][pos_x],  WEIGHTS_SCALE_FACTOR - TMP_SCALE_FACTOR, OUTPUT_ROUND_MODE);

        // Scale bias to match accumulator
        output_acc[pos_y][pos_x] += scale(NUMBER_T, (LONG_NUMBER_T)bias[k], BIASES_SCALE_FACTOR - TMP_SCALE_FACTOR - INPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);


#ifdef ACTIVATION_LINEAR
        output[pos_y][pos_x][k] = scale_and_clamp_to(NUMBER_T, output_acc[pos_y][pos_x], INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
#elif defined(ACTIVATION_RELU) || defined(ACTIVATION_RELU6)
        // Activation function: ReLU
        if (output_acc[pos_y][pos_x] < 0) {
          output[pos_y][pos_x][k] = 0;
        } else {
#if defined(ACTIVATION_RELU6)
        if (output_acc[pos_y][pos_x] > scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE)) {
          output_acc[pos_y][pos_x] = scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE);
        }
#endif
          output[pos_y][pos_x][k] = scale_and_clamp_to(NUMBER_T, output_acc[pos_y][pos_x], INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
        }
#else
#error "Unsupported activation function"
#endif
      }
    }
  }
#else

#if BIASES_SCALE_FACTOR > WEIGHTS_SCALE_FACTOR
#error "CMSIS-NN does not support BIASES_SCALE_FACTOR larger than WEIGHTS_SCALE_FACTOR"
#endif

  static q15_t bufferA[INPUT_HEIGHT*INPUT_WIDTH*INPUT_CHANNELS];
#ifdef WITH_CMSIS_NN
  arm_convolve_HWC_q15_basic_nonsquare(
#elif defined(WITH_NMSIS_NN)
  riscv_convolve_HWC_q15_basic_nonsquare(
#endif
                                      (q15_t*)input, //Im_in
                                      INPUT_WIDTH, //dim_im_in_x
                                      INPUT_HEIGHT, //dim_im_in_y
                                      INPUT_CHANNELS, //ch_im_in
                                      (q15_t*)kernel, //wt
                                      CONV_FILTERS, //ch_im_out
                                      CONV_KERNEL_SIZE_X, //dim_kernel_x
                                      CONV_KERNEL_SIZE_Y, //dim_kernel_y
                                      ZEROPADDING_LEFT, //padding_x, left and right must be equal
                                      ZEROPADDING_TOP, //padding_y, top and bottom must be equal
                                      CONV_STRIDE_X, //stride_x
                                      CONV_STRIDE_Y, //stride_y
                                      (q15_t*)bias, //bias
                                      INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - BIASES_SCALE_FACTOR, //bias_shift
                                      INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, //out_shift
                                      (q15_t*)output, //Im_out
                                      CONV_OUTWIDTH, //dim_im_out_x
                                      CONV_OUTHEIGHT, //dim_im_out_y
                                      bufferA, //bufferA
                                      NULL //bufferB, unused
                                      );
#ifdef ACTIVATION_RELU
#ifdef WITH_CMSIS_NN
  arm_relu_q15((q15_t*)output, CONV_FILTERS * CONV_OUTHEIGHT * CONV_OUTWIDTH);
#elif defined(WITH_NMSIS_NN)
  riscv_relu_q15((q15_t*)output, CONV_FILTERS * CONV_OUTHEIGHT * CONV_OUTWIDTH);
#endif
#elif !defined(ACTIVATION_LINEAR)
#error "Unsupported activation with CMSIS-NN"
#endif


#endif
}

#undef INPUT_CHANNELS
#undef INPUT_WIDTH
#undef INPUT_HEIGHT
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_STRIDE_X
#undef CONV_STRIDE_Y
#undef CONV_GROUPS
#undef CHANNELS_PER_GROUP
#undef FILTERS_PER_GROUP
#undef ZEROPADDING_TOP
#undef ZEROPADDING_BOTTOM
#undef ZEROPADDING_LEFT
#undef ZEROPADDING_RIGHT
#undef CONV_OUTWIDTH
#undef CONV_OUTHEIGHT
#undef ACTIVATION_RELU
#undef WEIGHTS_SCALE_FACTOR
#undef BIASES_SCALE_FACTOR
#undef TMP_SCALE_FACTOR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
#undef NUMBER_T
#undef LONG_NUMBER_T
/**
  ******************************************************************************
  * @file    weights/conv2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#include <stdint.h>

#define INPUT_CHANNELS     1
#define CONV_FILTERS       32
#define CONV_KERNEL_SIZE_Y 3
#define CONV_KERNEL_SIZE_X 3
#define CONV_GROUPS        1


const int16_t conv2d_2_bias[CONV_FILTERS] = {-11, -4, -6, -7, 4, 8, 13, 8, -2, -11, -10, 6, 17, -9, 10, -7, -9, 3, -5, 3, -1, -3, -9, 22, 0, 1, 3, -14, -6, -14, -3, -5}
;


const int16_t conv2d_2_kernel[CONV_FILTERS][CONV_KERNEL_SIZE_Y][CONV_KERNEL_SIZE_X][INPUT_CHANNELS / CONV_GROUPS] = {{{{18}
, {31}
, {17}
}
, {{8}
, {11}
, {-15}
}
, {{2}
, {-41}
, {-14}
}
}
, {{{-10}
, {20}
, {40}
}
, {{-51}
, {18}
, {42}
}
, {{-71}
, {-21}
, {16}
}
}
, {{{4}
, {-38}
, {31}
}
, {{5}
, {-2}
, {8}
}
, {{1}
, {-49}
, {40}
}
}
, {{{-13}
, {-45}
, {-4}
}
, {{2}
, {20}
, {13}
}
, {{23}
, {21}
, {-1}
}
}
, {{{28}
, {6}
, {-38}
}
, {{-16}
, {2}
, {28}
}
, {{-65}
, {39}
, {16}
}
}
, {{{-11}
, {-13}
, {13}
}
, {{-73}
, {14}
, {11}
}
, {{5}
, {16}
, {19}
}
}
, {{{17}
, {25}
, {-59}
}
, {{18}
, {11}
, {-25}
}
, {{26}
, {34}
, {-68}
}
}
, {{{28}
, {-45}
, {17}
}
, {{20}
, {-22}
, {31}
}
, {{-1}
, {-3}
, {-44}
}
}
, {{{39}
, {45}
, {-1}
}
, {{41}
, {10}
, {9}
}
, {{-91}
, {-48}
, {-9}
}
}
, {{{-9}
, {-3}
, {-3}
}
, {{-2}
, {5}
, {-3}
}
, {{-10}
, {-2}
, {4}
}
}
, {{{-8}
, {-13}
, {17}
}
, {{-13}
, {-12}
, {-20}
}
, {{22}
, {28}
, {7}
}
}
, {{{31}
, {27}
, {38}
}
, {{6}
, {-19}
, {13}
}
, {{-47}
, {-8}
, {-54}
}
}
, {{{-34}
, {2}
, {48}
}
, {{-64}
, {19}
, {36}
}
, {{-28}
, {-16}
, {-2}
}
}
, {{{6}
, {10}
, {23}
}
, {{10}
, {10}
, {2}
}
, {{10}
, {15}
, {16}
}
}
, {{{35}
, {-18}
, {29}
}
, {{-47}
, {5}
, {29}
}
, {{0}
, {-39}
, {-11}
}
}
, {{{2}
, {-21}
, {-43}
}
, {{7}
, {10}
, {23}
}
, {{21}
, {10}
, {13}
}
}
, {{{-26}
, {-19}
, {-37}
}
, {{-4}
, {-5}
, {11}
}
, {{24}
, {34}
, {21}
}
}
, {{{0}
, {-22}
, {-77}
}
, {{34}
, {23}
, {-39}
}
, {{34}
, {39}
, {-11}
}
}
, {{{26}
, {29}
, {-53}
}
, {{-19}
, {40}
, {-14}
}
, {{22}
, {-26}
, {7}
}
}
, {{{9}
, {-10}
, {9}
}
, {{-3}
, {-11}
, {15}
}
, {{38}
, {-49}
, {-12}
}
}
, {{{27}
, {0}
, {-6}
}
, {{39}
, {-72}
, {4}
}
, {{-42}
, {25}
, {20}
}
}
, {{{-47}
, {25}
, {0}
}
, {{-19}
, {-2}
, {-42}
}
, {{21}
, {31}
, {46}
}
}
, {{{-34}
, {-23}
, {-23}
}
, {{9}
, {11}
, {12}
}
, {{19}
, {23}
, {18}
}
}
, {{{38}
, {-24}
, {-8}
}
, {{-6}
, {-61}
, {29}
}
, {{15}
, {-38}
, {29}
}
}
, {{{-67}
, {-16}
, {6}
}
, {{9}
, {-11}
, {1}
}
, {{40}
, {8}
, {21}
}
}
, {{{-65}
, {31}
, {18}
}
, {{-3}
, {37}
, {-23}
}
, {{-31}
, {19}
, {3}
}
}
, {{{-10}
, {-46}
, {-41}
}
, {{10}
, {8}
, {-18}
}
, {{41}
, {31}
, {9}
}
}
, {{{3}
, {10}
, {16}
}
, {{-6}
, {-10}
, {-12}
}
, {{-10}
, {-12}
, {-19}
}
}
, {{{8}
, {33}
, {-78}
}
, {{31}
, {-1}
, {-23}
}
, {{-8}
, {11}
, {27}
}
}
, {{{-25}
, {1}
, {21}
}
, {{8}
, {2}
, {2}
}
, {{-3}
, {15}
, {10}
}
}
, {{{-61}
, {-40}
, {-28}
}
, {{16}
, {4}
, {6}
}
, {{37}
, {37}
, {20}
}
}
, {{{-4}
, {-18}
, {-11}
}
, {{-2}
, {-28}
, {-27}
}
, {{-6}
, {29}
, {37}
}
}
}
;

#undef INPUT_CHANNELS
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_GROUPS
/**
  ******************************************************************************
  * @file    maxpool2d.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    21 april 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _MAX_POOLING2D_2_H_
#define _MAX_POOLING2D_2_H_

#ifndef SINGLE_FILE
#include "number.h"
#endif

#define INPUT_CHANNELS  32
#define INPUT_HEIGHT    26
#define INPUT_WIDTH     26
#define POOL_SIZE_Y     2
#define POOL_SIZE_X     2
#define POOL_STRIDE_Y   2
#define POOL_STRIDE_X   2
#define POOL_PAD_Y      0 // Unsupported
#define POOL_PAD_X      0 // Unsupported
#define POOL_HEIGHT	    ( ( (INPUT_HEIGHT - POOL_SIZE_Y + (2*POOL_PAD_Y) ) / POOL_STRIDE_Y ) + 1 )
#define POOL_WIDTH	    ( ( (INPUT_WIDTH - POOL_SIZE_X + (2*POOL_PAD_X) ) / POOL_STRIDE_X ) + 1 )

typedef int16_t max_pooling2d_2_output_type[POOL_HEIGHT][POOL_WIDTH][INPUT_CHANNELS];

#if 0
void max_pooling2d_2(
  const number_t input[INPUT_SAMPLES][INPUT_CHANNELS], 	    // IN
  number_t output[POOL_LENGTH][INPUT_CHANNELS]); 	// OUT
#endif

#undef INPUT_CHANNELS
#undef INPUT_WIDTH
#undef INPUT_HEIGHT
#undef POOL_SIZE_X
#undef POOL_SIZE_Y
#undef POOL_STRIDE_X
#undef POOL_STRIDE_Y
#undef POOL_PAD_X
#undef POOL_PAD_Y
#undef POOL_HEIGHT
#undef POOL_WIDTH

#endif//_MAX_POOLING2D_2_H_
/**
  ******************************************************************************
  * @file    maxpool2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 november 2021
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "max_pooling2d_2.h"
#include "number.h"
#endif

#define INPUT_CHANNELS  32
#define INPUT_HEIGHT    26
#define INPUT_WIDTH     26
#define POOL_SIZE_Y     2
#define POOL_SIZE_X     2
#define POOL_STRIDE_Y   2
#define POOL_STRIDE_X   2
#define POOL_PAD_Y      0 // Unsupported
#define POOL_PAD_X      0 // Unsupported
#define POOL_HEIGHT	    ( ( (INPUT_HEIGHT - POOL_SIZE_Y + (2*POOL_PAD_Y) ) / POOL_STRIDE_Y ) + 1 )
#define POOL_WIDTH	    ( ( (INPUT_WIDTH - POOL_SIZE_X + (2*POOL_PAD_X) ) / POOL_STRIDE_X ) + 1 )

#define ACTIVATION_LINEAR

// For fixed point quantization
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void max_pooling2d_2(
  const NUMBER_T input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS], 	    // IN
  NUMBER_T output[POOL_HEIGHT][POOL_WIDTH][INPUT_CHANNELS]) {	// OUT

  unsigned short pos_x, pos_y, k; 	// loop indexes for output volume
  unsigned int x, y;
  LONG_NUMBER_T max, tmp;

  for (k = 0; k < INPUT_CHANNELS; k++) 
    for (pos_y = 0; pos_y < POOL_HEIGHT; pos_y++) {
      for (pos_x = 0; pos_x < POOL_WIDTH; pos_x++) {
#ifdef ACTIVATION_LINEAR
        max = input[pos_y*POOL_STRIDE_Y][pos_x*POOL_STRIDE_X][k];
        x = 1;
#elif defined(ACTIVATION_RELU)
        max = 0;
        x = 0;
#else
#error "Unsupported activation function"
#endif
        for (y = 0; y < POOL_SIZE_Y; y++) {
          for (; x < POOL_SIZE_X; x++) {
            tmp = input[(pos_y*POOL_STRIDE_Y)+y][(pos_x*POOL_STRIDE_X)+x][k];
            if (max < tmp)
              max = tmp;
          }
          x = 0;
        }

        output[pos_y][pos_x][k] = scale_and_clamp_to(NUMBER_T, max, INPUT_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
      }
    }
}

#undef INPUT_CHANNELS  
#undef INPUT_WIDTH
#undef INPUT_HEIGHT
#undef POOL_SIZE_X
#undef POOL_SIZE_Y
#undef POOL_STRIDE_X
#undef POOL_STRIDE_Y
#undef POOL_PAD_X
#undef POOL_PAD_Y
#undef POOL_WIDTH
#undef POOL_HEIGHT
#undef ACTIVATION_LINEAR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
/**
  ******************************************************************************
  * @file    conv2d.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    14 december 2022
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _CONV2D_3_H_
#define _CONV2D_3_H_

#ifndef SINGLE_FILE
#include "number.h"
#endif

#define INPUT_CHANNELS      32
#define INPUT_HEIGHT        13
#define INPUT_WIDTH         13
#define CONV_FILTERS        32
#define CONV_KERNEL_SIZE_Y  3
#define CONV_KERNEL_SIZE_X  3
#define CONV_STRIDE_Y       1
#define CONV_STRIDE_X       1

#define ZEROPADDING_TOP     0
#define ZEROPADDING_BOTTOM  0
#define ZEROPADDING_LEFT    0
#define ZEROPADDING_RIGHT   0

#define CONV_OUTHEIGHT     ( ( (INPUT_HEIGHT - CONV_KERNEL_SIZE_Y + ZEROPADDING_TOP + ZEROPADDING_BOTTOM) / CONV_STRIDE_Y ) + 1 )
#define CONV_OUTWIDTH      ( ( (INPUT_WIDTH - CONV_KERNEL_SIZE_X + ZEROPADDING_LEFT + ZEROPADDING_RIGHT) / CONV_STRIDE_X ) + 1 )


typedef int16_t conv2d_3_output_type[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS];

#if 0
void conv2d_3(
  const number_t input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],               // IN
  const number_t kernel[CONV_FILTERS][CONV_KERNEL_SIZE_X][CONV_KERNEL_SIZE_Y][INPUT_CHANNELS], // IN

  const number_t bias[CONV_FILTERS],						                // IN

  number_t output[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS]);               // OUT
#endif

#undef INPUT_CHANNELS
#undef INPUT_WIDTH
#undef INPUT_HEIGHT
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_STRIDE_X
#undef CONV_STRIDE_Y
#undef ZEROPADDING_TOP
#undef ZEROPADDING_BOTTOM
#undef ZEROPADDING_LEFT
#undef ZEROPADDING_RIGHT
#undef CONV_OUTWIDTH
#undef CONV_OUTHEIGHT

#endif//_CONV2D_3_H_
/**
  ******************************************************************************
  * @file    conv2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 november 2021
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "conv2d_3.h"
#include "number.h"
#endif

#ifdef WITH_CMSIS_NN
#include "arm_nnfunctions.h"
#elif defined(WITH_NMSIS_NN)
#include "riscv_nnfunctions.h"
#endif

#define INPUT_CHANNELS      32
#define INPUT_HEIGHT        13
#define INPUT_WIDTH         13
#define CONV_FILTERS        32
#define CONV_KERNEL_SIZE_Y  3
#define CONV_KERNEL_SIZE_X  3
#define CONV_STRIDE_Y       1
#define CONV_STRIDE_X       1
#define CONV_GROUPS         1
#define CHANNELS_PER_GROUP  (INPUT_CHANNELS / CONV_GROUPS)
#define FILTERS_PER_GROUP   (CONV_FILTERS / CONV_GROUPS)

#define ZEROPADDING_TOP     0
#define ZEROPADDING_BOTTOM  0
#define ZEROPADDING_LEFT    0
#define ZEROPADDING_RIGHT   0

#define CONV_OUTHEIGHT     ( ( (INPUT_HEIGHT - CONV_KERNEL_SIZE_Y + ZEROPADDING_TOP + ZEROPADDING_BOTTOM) / CONV_STRIDE_Y ) + 1 )
#define CONV_OUTWIDTH      ( ( (INPUT_WIDTH - CONV_KERNEL_SIZE_X + ZEROPADDING_LEFT + ZEROPADDING_RIGHT) / CONV_STRIDE_X ) + 1 )

#define ACTIVATION_RELU

// For fixed point quantization
#define WEIGHTS_SCALE_FACTOR 7
#define BIASES_SCALE_FACTOR 7
#define TMP_SCALE_FACTOR 7
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void conv2d_3(
  const NUMBER_T input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],               // IN
  const NUMBER_T kernel[CONV_FILTERS][CONV_KERNEL_SIZE_X][CONV_KERNEL_SIZE_Y][INPUT_CHANNELS / CONV_GROUPS], // IN

  const NUMBER_T bias[CONV_FILTERS],						                // IN

  NUMBER_T output[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS]) {               // OUT

#if !defined(WITH_CMSIS_NN) && !defined(WITH_NMSIS_NN)
  unsigned short pos_x, pos_y, z, k; 	// loop indexes for output volume
  unsigned short x, y;
  int input_x, input_y;
  LONG_NUMBER_T	kernel_mac;
  LONG_NUMBER_T tmp;
  static LONG_NUMBER_T	output_acc[CONV_OUTHEIGHT][CONV_OUTWIDTH];

  for (k = 0; k < CONV_FILTERS; k++) { 
    for (pos_y = 0; pos_y < CONV_OUTHEIGHT; pos_y++) { 
      for (pos_x = 0; pos_x < CONV_OUTWIDTH; pos_x++) { 
        output_acc[pos_y][pos_x] = 0;

        for (z = 0; z < INPUT_CHANNELS / CONV_GROUPS; z++) {
          kernel_mac = 0; 
            
          for (y = 0; y < CONV_KERNEL_SIZE_Y; y++) {
            input_y = pos_y * CONV_STRIDE_Y - ZEROPADDING_TOP + y;

            for (x = 0; x < CONV_KERNEL_SIZE_X; x++) {
              input_x = pos_x * CONV_STRIDE_X - ZEROPADDING_LEFT + x;

              if (input_x < 0 || input_x >= INPUT_WIDTH || input_y < 0 || input_y >= INPUT_HEIGHT) // ZeroPadding2D
                tmp = 0;
              else
                tmp = (LONG_NUMBER_T)input[input_y][input_x][z + (k / FILTERS_PER_GROUP) * CHANNELS_PER_GROUP] * (LONG_NUMBER_T)kernel[k][y][x][z];
              kernel_mac = kernel_mac + tmp;
            }
          }

          output_acc[pos_y][pos_x] = output_acc[pos_y][pos_x] + kernel_mac;

        }
      }
    }

    for (pos_y = 0; pos_y < CONV_OUTHEIGHT; pos_y++) { 
      for (pos_x = 0; pos_x < CONV_OUTWIDTH; pos_x++) { 
        // Scale for possible additional precision of bias
        output_acc[pos_y][pos_x] = scale(NUMBER_T, output_acc[pos_y][pos_x],  WEIGHTS_SCALE_FACTOR - TMP_SCALE_FACTOR, OUTPUT_ROUND_MODE);

        // Scale bias to match accumulator
        output_acc[pos_y][pos_x] += scale(NUMBER_T, (LONG_NUMBER_T)bias[k], BIASES_SCALE_FACTOR - TMP_SCALE_FACTOR - INPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);


#ifdef ACTIVATION_LINEAR
        output[pos_y][pos_x][k] = scale_and_clamp_to(NUMBER_T, output_acc[pos_y][pos_x], INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
#elif defined(ACTIVATION_RELU) || defined(ACTIVATION_RELU6)
        // Activation function: ReLU
        if (output_acc[pos_y][pos_x] < 0) {
          output[pos_y][pos_x][k] = 0;
        } else {
#if defined(ACTIVATION_RELU6)
        if (output_acc[pos_y][pos_x] > scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE)) {
          output_acc[pos_y][pos_x] = scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE);
        }
#endif
          output[pos_y][pos_x][k] = scale_and_clamp_to(NUMBER_T, output_acc[pos_y][pos_x], INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
        }
#else
#error "Unsupported activation function"
#endif
      }
    }
  }
#else

#if BIASES_SCALE_FACTOR > WEIGHTS_SCALE_FACTOR
#error "CMSIS-NN does not support BIASES_SCALE_FACTOR larger than WEIGHTS_SCALE_FACTOR"
#endif

  static q15_t bufferA[INPUT_HEIGHT*INPUT_WIDTH*INPUT_CHANNELS];
#ifdef WITH_CMSIS_NN
  arm_convolve_HWC_q15_basic_nonsquare(
#elif defined(WITH_NMSIS_NN)
  riscv_convolve_HWC_q15_basic_nonsquare(
#endif
                                      (q15_t*)input, //Im_in
                                      INPUT_WIDTH, //dim_im_in_x
                                      INPUT_HEIGHT, //dim_im_in_y
                                      INPUT_CHANNELS, //ch_im_in
                                      (q15_t*)kernel, //wt
                                      CONV_FILTERS, //ch_im_out
                                      CONV_KERNEL_SIZE_X, //dim_kernel_x
                                      CONV_KERNEL_SIZE_Y, //dim_kernel_y
                                      ZEROPADDING_LEFT, //padding_x, left and right must be equal
                                      ZEROPADDING_TOP, //padding_y, top and bottom must be equal
                                      CONV_STRIDE_X, //stride_x
                                      CONV_STRIDE_Y, //stride_y
                                      (q15_t*)bias, //bias
                                      INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - BIASES_SCALE_FACTOR, //bias_shift
                                      INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, //out_shift
                                      (q15_t*)output, //Im_out
                                      CONV_OUTWIDTH, //dim_im_out_x
                                      CONV_OUTHEIGHT, //dim_im_out_y
                                      bufferA, //bufferA
                                      NULL //bufferB, unused
                                      );
#ifdef ACTIVATION_RELU
#ifdef WITH_CMSIS_NN
  arm_relu_q15((q15_t*)output, CONV_FILTERS * CONV_OUTHEIGHT * CONV_OUTWIDTH);
#elif defined(WITH_NMSIS_NN)
  riscv_relu_q15((q15_t*)output, CONV_FILTERS * CONV_OUTHEIGHT * CONV_OUTWIDTH);
#endif
#elif !defined(ACTIVATION_LINEAR)
#error "Unsupported activation with CMSIS-NN"
#endif


#endif
}

#undef INPUT_CHANNELS
#undef INPUT_WIDTH
#undef INPUT_HEIGHT
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_STRIDE_X
#undef CONV_STRIDE_Y
#undef CONV_GROUPS
#undef CHANNELS_PER_GROUP
#undef FILTERS_PER_GROUP
#undef ZEROPADDING_TOP
#undef ZEROPADDING_BOTTOM
#undef ZEROPADDING_LEFT
#undef ZEROPADDING_RIGHT
#undef CONV_OUTWIDTH
#undef CONV_OUTHEIGHT
#undef ACTIVATION_RELU
#undef WEIGHTS_SCALE_FACTOR
#undef BIASES_SCALE_FACTOR
#undef TMP_SCALE_FACTOR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
#undef NUMBER_T
#undef LONG_NUMBER_T
/**
  ******************************************************************************
  * @file    weights/conv2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#include <stdint.h>

#define INPUT_CHANNELS     32
#define CONV_FILTERS       32
#define CONV_KERNEL_SIZE_Y 3
#define CONV_KERNEL_SIZE_X 3
#define CONV_GROUPS        1


const int16_t conv2d_3_bias[CONV_FILTERS] = {-14, -15, -6, -2, -18, -5, -6, -7, -2, -10, -10, 9, -8, 0, -7, -3, 4, -6, -6, -1, -5, -6, -9, 3, -7, -3, -12, -7, -7, -4, -6, -3}
;


const int16_t conv2d_3_kernel[CONV_FILTERS][CONV_KERNEL_SIZE_Y][CONV_KERNEL_SIZE_X][INPUT_CHANNELS / CONV_GROUPS] = {{{{-16, -30, -27, 10, 4, -36, 8, -20, -35, -2, 1, -41, -41, -10, -39, 2, -2, 33, 5, -35, -4, 7, 8, -9, 10, -29, 20, -14, 52, 0, 22, 2}
, {-17, -44, -2, 20, -22, -21, 14, -35, -70, 2, -9, -52, -27, -4, -42, 7, 30, 34, -21, -13, 8, 21, 11, -3, 46, -9, 49, -5, 25, -28, 40, 14}
, {-6, 1, -9, -6, -36, -8, -10, -13, -21, -19, -5, 2, -13, -29, -8, 3, -3, 10, -12, 2, -25, -2, -8, -19, 25, -20, 23, 2, -18, -23, 18, 5}
}
, {{5, -22, -22, 7, -11, -38, -18, 13, 11, 3, 7, 33, -16, -9, 10, 5, -7, -23, -9, 5, -33, -25, -6, -27, -42, -45, -15, 1, -34, -14, -19, -16}
, {5, 21, 4, 8, 33, -32, -20, -10, 31, -11, -6, 20, -6, -11, 18, -14, 11, -1, -6, -11, -27, -52, 4, -22, -29, -29, 10, -6, -22, -5, -1, -38}
, {-17, 13, -30, -11, -3, -3, 29, -14, -8, -3, -9, -24, -18, -22, -20, 1, 3, 36, 17, -38, -28, -38, -1, -7, -2, 2, 5, 1, 24, -5, -19, -25}
}
, {{6, 13, -13, -47, -21, 6, 30, 38, 24, -9, -8, 20, -1, -13, 22, -31, -24, 16, 2, 16, 12, 1, -18, 25, -23, -28, 1, -2, 17, -28, -45, -9}
, {-11, 29, 40, -44, -7, 18, 10, 23, 10, -16, -9, 17, 35, -11, 39, -35, -34, 7, -26, 17, -16, 23, -31, 21, -9, 25, 2, 5, -15, -11, -27, -16}
, {-16, 36, -5, -29, 2, 4, 25, 0, 7, 15, -6, 22, 13, 5, 24, -12, -9, 27, 21, 7, -11, -29, -4, -6, -33, 4, 4, -1, 8, -3, -28, -2}
}
}
, {{{15, -22, -39, -9, -24, -41, 8, -4, 7, -13, -12, 7, -34, -9, -20, -5, -26, -7, 28, -16, -13, -54, -6, -46, -26, -31, -32, 17, 5, -12, -21, -43}
, {4, -23, -16, -45, 3, -17, 22, 9, 8, -11, -21, -8, -15, -40, -18, -31, -47, 11, 14, -11, 39, -50, -36, 15, -41, -9, -14, -19, 32, -36, -37, -22}
, {-40, -13, -9, -12, 7, 20, -1, 28, -18, -14, -23, -20, 0, -28, -9, 7, -6, 20, -8, 16, 9, -4, -19, 45, -9, -11, 43, -22, 28, -20, -1, 7}
}
, {{-24, 2, -43, -8, 6, -6, -7, -4, 34, -17, -5, 12, -10, -25, 13, -14, -12, 16, 8, 13, -9, 8, -14, 9, 4, -6, 9, -24, 20, -18, -25, -9}
, {-34, -5, 6, -7, 19, 11, -2, 12, 5, 0, -20, -24, 7, -14, -13, 10, -4, 1, -25, 3, 19, 3, -7, 1, 11, 3, 8, -19, 1, -9, -1, 3}
, {-39, 0, 37, 15, 5, 4, -14, 0, 2, -3, 2, -8, -9, -2, 7, 6, 6, 26, -23, -9, 19, 22, -2, 2, 8, 24, 15, -18, 20, 7, 6, 18}
}
, {{-4, -10, 12, 3, 16, -12, -12, -5, 8, -12, -23, -10, -22, -27, 1, -6, 20, 5, -10, -10, 3, 1, -12, -10, 7, 2, 5, -14, 4, -6, 8, -1}
, {-10, -11, 13, 9, 17, 3, -41, -15, -18, -1, 15, -1, -3, 10, -3, 1, 30, 9, 4, -33, 2, 45, 26, -9, 22, 11, 38, 17, 14, 14, 34, 18}
, {18, -9, -8, 25, -37, 15, -9, -39, 0, 4, 22, -20, -8, -7, -12, 8, 21, 17, -11, -8, -29, 32, 15, -14, 18, -6, 20, -2, -12, 10, 39, -4}
}
}
, {{{-5, -35, -14, -12, -56, -7, 1, -8, -16, -1, -6, -21, -6, 0, -31, -23, -7, 3, 0, -4, -27, -14, -8, -40, -27, 18, -30, -12, -2, -2, -22, 0}
, {-6, -19, 5, -11, 15, -19, 6, 20, -9, -13, -18, -23, -30, 1, -14, 0, -32, -4, 14, -3, 6, -12, -23, 32, -45, -5, -43, -12, 24, -24, -23, -21}
, {-19, -8, 5, 4, 12, -4, 11, 12, -28, 1, -5, -31, 6, -14, 1, 4, 6, 5, 8, -30, 12, -9, 7, -20, 3, 21, 9, -3, 15, -9, -6, 3}
}
, {{9, -38, 24, 8, 4, -18, 17, 18, 12, -12, -2, 5, -27, 18, 13, 11, -12, 11, 15, 8, 4, 38, 1, 19, -7, -10, -9, 8, 12, 1, -15, 18}
, {-3, 22, 5, 3, 65, 38, -19, 9, 17, -1, 27, 26, 5, -5, 10, 1, 7, -15, 51, 31, 26, 27, -3, 3, 15, 28, -1, 1, 12, -7, -19, 12}
, {1, -14, -17, -7, -24, 4, 11, -21, -14, 4, 7, -2, -7, 10, -25, -4, -1, 5, 10, 8, -22, 6, -17, -31, 5, -11, -15, -5, -2, 1, 5, 3}
}
, {{-4, -20, 4, 20, 17, 16, -4, -8, -11, -6, 4, -36, -9, 3, -6, 20, 12, 16, 43, -15, 5, -5, -8, -24, -5, -4, 12, -13, 44, -3, 12, -4}
, {25, 7, 6, 5, -19, -8, -35, 9, -20, 13, 6, -7, 15, 14, -12, -10, 17, -33, 7, -20, -32, 29, 20, -22, 33, 52, -3, 2, -16, 23, 27, -30}
, {2, -12, -10, -2, -2, -40, -23, -5, -17, -13, -5, -7, -26, 16, -31, 4, -15, -27, 1, -17, -2, -12, -9, -5, 7, -31, -26, 13, -16, 7, 1, 16}
}
}
, {{{-2, 19, 41, -2, 42, 31, 13, 46, 10, -8, 6, 4, 21, -4, 25, -3, -10, 14, 10, 39, 55, 23, -8, 53, 11, 36, 9, -21, 1, -3, -5, 10}
, {-17, -33, -12, -9, -10, -8, -4, -19, -34, -13, 1, -47, -11, -9, -57, 2, -13, -21, -22, 5, -7, 0, -21, 6, -13, -14, -2, 7, 17, -3, -12, 12}
, {-8, -10, -34, -12, -4, 0, -5, -23, -19, -1, -19, -15, -6, -13, -33, -14, 9, -10, -9, -22, -58, 14, 8, -16, -5, -21, 9, -2, -6, 5, 13, -18}
}
, {{0, 4, 7, 9, -12, 3, 1, 39, -19, -1, 2, -15, 17, -5, 3, -11, 0, -23, -20, 11, 35, 23, 1, 28, 19, 34, 7, 2, -9, 8, 10, -10}
, {-15, -12, 10, -9, -14, -1, -19, -2, -40, -7, -10, -45, -18, -3, -18, -2, -2, -6, -13, -9, -1, -5, -7, 11, -12, -11, 11, -15, 6, -16, 1, 14}
, {0, -6, -17, -6, -35, -1, 20, -12, -33, 6, -10, -16, 15, -4, 2, -16, -10, 3, -12, 2, 1, 3, 0, -15, 2, -29, -4, -18, 2, 1, -3, 9}
}
, {{-18, 10, -2, -17, 33, -9, -19, 30, 15, -3, -23, 14, -8, -9, -2, 7, -9, -37, -20, 16, 15, -20, -11, -2, -15, 10, -33, 9, -38, -12, 0, 3}
, {3, -14, -25, 15, 6, -38, -13, 26, 9, -9, 1, 23, 3, -8, 4, -5, -11, -14, -3, -4, -5, -32, -1, -25, -16, -26, -25, 7, -32, 6, 2, 10}
, {-2, -11, -25, 24, 12, -43, 5, 5, 17, 10, -3, 15, -15, 4, 7, 8, 15, 15, 11, -14, -19, -10, 8, -15, -8, -15, -8, -4, -4, -11, 17, -20}
}
}
, {{{-26, 7, -8, -9, -29, -29, 25, -1, -14, -1, -10, 0, 10, -16, -8, -15, 7, 59, -1, -7, 10, 13, -2, -20, 16, -33, 51, -5, 32, -23, 19, 13}
, {-16, 19, 38, -24, -17, 15, -9, 3, -16, -9, 9, 14, 35, -27, 17, -35, -20, 19, -11, -6, 27, 13, -24, 27, 10, 18, 34, -13, 24, -17, -30, 4}
, {-29, 15, 8, -8, 39, 10, -4, 14, 15, -4, 5, 6, 25, -16, 10, -17, -2, 6, -16, -14, 52, 18, -22, 9, -11, 17, 19, -26, -3, -18, -30, 8}
}
, {{8, -1, -6, 14, 6, -14, -22, -4, 4, -1, -4, -22, -28, -7, 7, 12, -6, 21, -5, -13, -23, -17, -3, -25, 5, -38, 14, -7, 2, 4, 23, -6}
, {4, -16, 5, 27, -10, -5, -24, -13, 5, 9, 15, -7, -11, 9, -25, 22, 36, 15, 13, -19, 15, 10, 10, -2, 35, 16, 24, 7, 30, 3, 44, -8}
, {9, -13, 19, 18, -45, 12, -23, -35, 17, -2, 3, -9, 17, -7, -19, -18, -5, -16, 11, -30, -28, 3, -9, -38, 45, 20, -9, 17, -14, 9, 24, -36}
}
, {{-6, 15, -27, -8, -20, -24, -17, 12, 7, 6, -5, 17, -25, 9, -12, 7, 5, 2, -52, 27, -52, -10, 8, -7, -11, -15, 12, 1, -26, 4, -6, -25}
, {-5, -11, 6, -7, 12, -2, -8, -8, 33, -8, 7, 19, -20, -7, -8, 8, -7, -19, -14, 1, 7, -12, 7, -16, -29, -6, -12, 10, -29, 2, 3, 15}
, {1, -23, 4, -8, 20, -29, -6, 1, 29, 0, 13, 24, -24, -5, -11, -2, 4, -6, 14, 15, 44, -17, -18, -15, -32, -26, -4, -9, -6, -18, -25, 26}
}
}
, {{{5, -15, -17, -30, -18, -6, 18, 19, 16, -3, -20, 20, -5, -24, 10, -32, -22, 8, 20, 19, 15, -20, -18, 38, -15, -18, -3, 0, 13, -24, -36, -18}
, {-17, -3, 5, -16, 14, 20, -16, -3, 1, -15, -11, -2, 8, -20, -1, -9, 17, 0, -27, 14, 9, -8, 1, 29, 15, -8, 10, -5, -19, -21, -11, 3}
, {-18, 20, 23, 2, 23, 23, -10, -5, 3, -11, -8, -15, 11, 3, 7, -1, 7, 1, 4, -32, 10, 4, 6, -17, 33, 23, 24, -7, -18, -9, 3, -6}
}
, {{-9, 1, -3, -15, -21, 6, 9, 10, -33, 0, -1, -31, -4, -10, -18, 9, -15, 5, -12, 0, -9, 11, -3, 25, 16, -1, 2, -21, 4, -18, -3, -8}
, {-3, 11, 32, 22, -24, 18, 1, 10, -20, -9, 4, -22, -3, -10, -8, 5, 18, 9, -15, -2, -9, 27, -1, 8, 37, 19, 15, 4, 24, -9, 23, -1}
, {-5, 6, -9, 2, -12, 3, -19, -38, -70, 9, 6, -73, -1, 6, -35, 1, 12, -4, -8, -13, 11, 16, 1, -3, 26, 14, -9, -9, 7, 10, 20, 4}
}
, {{-5, 10, -10, 9, 32, -4, -46, 19, -6, -6, 9, -24, -6, 4, -18, 9, 7, -21, -28, -12, 34, -2, 5, 18, -3, 10, -21, -5, 5, -5, -1, 8}
, {15, -17, -2, 11, -28, 21, -45, -34, -47, 8, 3, -42, -7, -5, -33, 3, 8, -6, -46, -62, -39, -11, 23, -27, 18, -11, -2, 20, -23, 6, 24, 8}
, {3, -14, -4, -3, -22, -39, -19, -17, 1, 14, 0, -9, -22, 15, -33, -7, 9, -15, 33, 5, -15, -21, 7, -29, -18, 0, -15, 18, 12, 8, 4, -29}
}
}
, {{{4, -24, -9, -22, -49, -45, -43, -9, 23, 6, -16, 7, -26, 2, -7, -15, 2, -53, -24, 6, -17, -28, -15, -49, -13, -16, -37, -9, -25, 2, -20, -13}
, {16, -20, -3, -23, -7, -24, 3, 30, 24, -15, 0, 22, -33, -8, 11, -7, -9, -16, -14, 29, 17, 17, 5, 14, -26, 3, -14, 4, -7, -5, -30, 2}
, {4, 6, 21, -14, 45, 14, 25, 7, 52, 8, -10, 26, -9, -5, 25, 11, -11, -3, 64, 28, 40, -24, -15, 33, -20, 4, -38, -7, 31, 2, -4, -9}
}
, {{4, -3, 0, -10, 28, 2, -19, 14, 5, 0, -11, 28, -10, -15, 14, -6, -15, -35, -38, -6, 27, -7, -5, 11, -12, -9, -4, -1, -26, -14, -17, -4}
, {-16, 1, 19, 6, 30, 4, 16, 5, 16, 1, 2, 12, 1, 6, 23, 8, 30, 44, 46, -14, 18, 13, 17, 12, 12, 11, 30, -19, 16, 7, 13, -13}
, {-3, 6, 15, 18, -27, 33, 11, 8, -24, 8, 8, -9, 9, -5, 26, 3, -4, 7, 10, -11, 29, 25, 0, -2, 24, 32, 10, -23, 16, -2, 20, 0}
}
, {{-25, -8, -16, 24, -22, -9, -33, -22, -74, -3, 9, -40, -24, 1, -44, 17, 22, 18, -33, -34, -50, -3, -3, -25, 20, 0, 22, 14, -21, 7, 27, -13}
, {-6, -11, -24, -6, -23, -9, -27, -47, -44, 7, -1, -46, -21, 17, -48, 10, -9, -4, -20, -21, -32, 9, -8, -5, 13, -8, 9, -3, -12, 13, 2, -1}
, {-2, -31, -45, 8, -26, -7, -13, -47, -46, 1, -10, -38, -22, -9, -55, 11, 11, -20, -25, -46, -43, -17, 19, -55, -5, -8, 15, -9, -18, -8, 17, -11}
}
}
, {{{-14, -22, 27, -30, -14, -16, 53, 2, -47, -20, -27, -12, 9, -28, -15, -23, -15, 40, 31, 7, -6, -28, -32, 31, 1, -17, 26, -7, 30, -17, -18, -8}
, {4, 40, 6, -21, 18, 33, 1, -13, -18, 5, -13, -8, 56, -22, 29, -21, -1, -21, -7, 9, -9, 12, 0, -5, 18, 45, -1, -14, -18, -10, -21, -1}
, {0, 18, -6, -8, 15, -15, 15, -24, -6, 1, -9, -20, 6, -3, -42, 7, 13, 10, -10, -28, -11, 0, -8, -12, 3, 27, 12, 7, 14, 0, 6, 0}
}
, {{11, -12, 31, -22, -33, -22, 45, 34, -20, -3, -4, 3, 11, -22, -1, -11, -12, 38, 33, 23, -1, -33, -6, 29, 7, -48, -16, -1, 16, -15, -20, -9}
, {-25, 19, -11, -12, -12, 12, 2, -1, -8, 10, -17, 5, 22, -18, 5, -32, -10, 17, 2, -3, -5, -3, -19, -8, -2, 44, -21, -23, -14, 3, -14, -27}
, {-17, 14, 13, -3, 6, 19, -8, -22, -23, 10, -11, -21, 18, -9, -18, -13, -1, 8, -4, -24, -6, -6, -15, -4, -10, 5, -27, -17, -15, 11, -2, 7}
}
, {{-9, -27, 14, -16, -59, -29, 54, 22, -45, -17, -22, -8, -4, -30, -7, -17, -33, 34, 8, 28, -9, -58, -32, 25, -32, -66, 9, -9, 15, -17, -38, -41}
, {-37, 52, 37, -9, 29, 8, 2, 15, -26, -9, -37, 3, 50, -28, 27, -8, -29, -22, -1, 29, 5, -39, -20, 7, -15, 19, -54, -31, -21, -15, -46, -32}
, {-9, 37, 8, 3, 35, 16, -41, 2, 39, -3, 8, 35, 20, 13, 6, 22, 13, -23, -20, -17, 10, -3, 27, -36, -37, 6, -12, 2, -18, 14, -4, 10}
}
}
, {{{-13, -8, -21, -14, -5, -15, 28, -23, -44, -7, -5, -41, -2, -18, -29, 8, -17, 29, 5, 0, -26, 3, 0, -15, -5, 12, 1, -7, 13, -15, -15, -18}
, {-14, 10, 39, -12, -4, 22, -14, 17, -19, -12, -2, -19, 30, -14, 13, -30, -7, -11, -14, 8, 23, 9, -15, 34, 20, 47, -13, -7, 2, -4, -24, 6}
, {10, 26, -30, -15, 11, 7, -17, -18, 0, 0, 1, -9, 2, -2, 7, 1, 7, -44, -36, -23, -10, 18, 9, -25, 3, 19, -43, 16, -26, 1, -9, 30}
}
, {{-4, -24, -6, 3, -23, -18, 25, 6, -60, 1, 3, -21, 5, -1, -28, 4, 8, 40, 12, 8, -12, 6, -3, -9, -1, -10, 32, 6, 26, -4, 14, 2}
, {-9, 20, 21, -1, 21, -1, 13, 22, -29, 7, 13, 16, 17, -16, 14, -8, -15, 11, 7, 22, -8, 0, -21, 12, -11, 26, -21, 4, -19, -13, -25, -18}
, {14, 13, -8, -1, 15, -9, -29, 10, 12, 3, -6, 13, 20, 0, 4, -7, 5, -40, -13, 8, -2, -11, -10, -2, -8, 9, -25, 2, 11, -6, -10, 12}
}
, {{9, -22, -22, 2, -12, -46, 29, -10, -26, 12, -7, -45, -27, 7, -60, 19, 14, 40, 32, 24, -15, -20, 17, -28, -22, -69, 20, -12, 28, -10, -1, -12}
, {-32, 25, 5, -19, 0, -13, 19, 20, -16, 1, -28, 12, 12, -15, 22, -21, -33, 29, 36, 26, 6, -43, -27, 15, 16, 15, 8, -14, 32, -10, -15, -35}
, {-20, 40, 30, -20, 25, 9, -34, 22, 5, 2, -31, 7, 14, -9, 22, -4, -20, -25, -26, 18, 26, -12, -4, 3, -22, 22, -21, -28, -34, -6, -35, -7}
}
}
, {{{13, -22, -15, 11, 13, -1, -18, -28, 18, 6, -2, -26, -22, 10, -7, -2, -7, -1, 4, -16, -4, -24, -5, -9, -15, -31, -10, 10, -2, 13, -13, -13}
, {-13, -43, -21, -4, 38, -46, 64, 26, -54, -11, -11, -36, -73, 4, -26, 14, 11, 86, 61, 18, 0, 5, 14, 10, 17, 2, 57, -24, 67, -18, 18, -1}
, {-25, 28, 40, 5, -21, -15, 1, 3, -44, -1, -7, -37, 25, -23, 28, -27, -20, 46, -8, 31, -27, -14, -15, 21, 41, 28, 45, -32, -6, -20, 23, -32}
}
, {{12, -9, -29, 5, -22, -24, -45, -14, 9, 11, 6, 4, -26, 5, -25, -6, -8, -32, -34, -7, -15, -2, 6, -17, -20, -5, -24, 23, -37, -5, -10, -15}
, {-5, -25, -44, 0, -3, 10, 16, 4, 17, 5, 13, 7, -40, 2, -19, 8, 5, 0, 37, -19, 10, 43, -2, -35, -26, -10, 3, 7, 9, -3, 3, 14}
, {-7, 10, 32, 1, -7, 16, 10, 10, -22, -13, 3, -11, 38, -4, 6, -4, -6, 42, 6, 19, 37, -19, -23, 23, -2, -25, 32, -33, -1, -6, -13, -1}
}
, {{-1, 22, -22, -8, -6, -4, -63, -19, -2, 0, -21, -3, -9, -10, -1, -6, -4, -41, -43, -17, -6, -19, -4, -27, -14, -35, -21, 0, -16, 5, -26, -7}
, {5, -20, -27, -6, 7, -9, 7, -19, 20, 10, 5, 11, -38, 1, -17, 19, 10, -10, 13, -11, -2, 3, 1, -19, -10, 9, 22, 17, 25, -6, 22, 3}
, {4, -36, -17, 13, -26, -14, -13, 21, 19, -14, 10, -3, -33, -9, -11, 0, 6, 0, -3, -3, 12, 15, 14, 12, 11, 5, 19, -5, 15, 1, 6, 3}
}
}
, {{{-11, -16, -33, 2, 18, -49, 20, -13, -1, -1, -1, -18, -24, -1, -7, 34, 7, 36, 30, -7, -17, -1, 19, -13, -24, -19, 28, -4, 37, -12, 20, 4}
, {-11, -17, 38, 13, -3, 2, 22, 39, -13, -19, 6, -6, 4, -12, 19, 5, 8, 17, 23, 29, 34, 18, 13, 19, 30, 44, 26, -17, 4, -5, 26, 6}
, {-6, -5, 10, -26, 19, -5, 0, -9, -9, -15, -12, 13, 2, -20, 9, -16, -15, -7, -1, 8, 9, -8, -11, -26, -7, 5, -9, 2, 4, -1, -28, 1}
}
, {{17, -1, 7, -22, 9, 6, -15, -19, -10, 11, -6, -8, 2, -6, 14, -7, -4, -33, -23, -7, -9, 6, -18, -6, -11, -20, -23, 2, -15, 14, -14, -3}
, {7, -10, 22, 23, 24, -5, 9, 19, -9, -11, 17, -11, -13, 5, -2, 7, 7, 14, 39, 10, 16, 24, 8, 24, 1, -2, 7, -17, 29, -13, 1, 19}
, {-17, 3, 25, -21, 4, 5, 27, 18, -15, -12, -2, 0, 15, -17, 8, -10, -8, 24, 29, 8, 2, -2, -15, 19, 3, 31, 5, -10, 21, -12, -12, -18}
}
, {{9, -27, -4, 8, -31, -33, -26, -30, -39, 3, -6, -38, -28, 11, -34, 13, 1, -21, -32, -32, -23, -1, 5, -4, 10, -8, -3, 6, -15, -5, 8, 5}
, {-6, -32, -39, 6, 1, -36, 2, -46, -15, 9, 2, -36, -21, 11, -45, -1, 19, 4, -9, -30, -11, 1, 18, -23, 4, -33, 17, -1, 7, -8, 25, -7}
, {14, -8, -21, -7, -29, -9, 22, 5, -46, -19, -16, 16, -10, -15, -24, -15, 2, 26, 27, 18, 2, -21, -5, 1, 13, -10, 22, 13, 22, -13, 7, -35}
}
}
, {{{-7, -22, -37, 10, 6, -35, 46, 25, -13, -2, -1, -15, -24, 17, -34, 27, 15, 48, 42, -5, 14, 3, 3, 19, 18, -25, 38, -6, 51, 3, 31, 30}
, {-35, 3, 7, -10, -20, -3, 15, -7, -48, -27, 6, -23, 2, -33, 1, -5, -18, 26, -5, 5, -4, -8, -17, 8, 10, 15, 35, -21, 8, -25, -18, -1}
, {-31, 38, 26, -12, -11, 17, -27, 4, -23, 10, 1, 18, 21, -5, 10, -3, -4, -24, -41, 12, -18, -5, 4, 9, -5, 23, -6, 9, -50, 8, -25, -20}
}
, {{17, -45, -41, 26, -27, -29, 45, 6, 15, 3, 17, 15, -47, 18, -28, 32, 32, 38, 39, 13, 3, -5, 25, -19, 5, -17, 24, 6, 23, 11, 19, -15}
, {13, -20, -15, -48, -1, -15, 13, 25, 19, -26, -31, 6, -4, -44, -9, -52, -51, 10, 13, 4, -1, -47, -60, 11, -3, 14, 6, -19, -9, -51, -25, -57}
, {-26, 2, 7, -32, -9, -7, -9, 23, -2, 0, -15, 6, 7, -19, 22, -55, -29, 14, -20, 6, 11, 7, -22, 14, 6, 3, 1, -4, -23, -19, -20, -5}
}
, {{33, -24, 9, -23, 40, -32, 36, 10, 37, -16, -45, 39, -8, -2, 41, -21, -21, 3, 26, 10, 38, -25, -25, 19, -13, -20, -7, 25, 33, -11, -11, -21}
, {-33, -17, -11, -10, 1, -5, -16, 16, -10, 0, -46, -6, 4, -36, 14, -34, -12, 1, -41, -4, 8, -11, -28, 11, -25, 8, -16, -33, -7, -46, -19, -5}
, {-37, 13, 28, -30, 12, -12, -23, 24, 2, 10, -18, 4, -6, -33, 24, -37, -26, -3, -42, 4, 29, -15, -11, 16, -11, 1, -20, -25, -8, -22, -20, -9}
}
}
, {{{-18, 8, 8, -6, -14, 2, -9, -15, -5, 0, -15, 4, 1, -2, 11, -16, -2, 8, -21, -5, -9, -9, -5, 8, 4, 4, 11, -9, -6, -21, 4, 7}
, {-1, -6, 2, -4, 6, -2, -19, 1, 1, -6, 2, 4, -6, -5, -9, 2, -5, -10, -16, -5, -1, 7, -7, -2, 4, -3, 8, 7, -6, -6, -7, -11}
, {-20, -23, -7, -4, -17, -5, 6, -10, 2, -2, -18, -11, -16, 4, 1, -2, -8, 2, 5, -11, -7, -2, -9, -3, -14, 2, 4, -12, -9, -13, -10, -7}
}
, {{-9, -9, -1, -14, -4, -13, 4, -24, -2, -2, -10, -18, -9, -1, -20, -4, -14, 12, -15, -26, -15, 4, -17, 4, 3, -17, -3, 5, 6, -19, -3, 4}
, {-17, 0, 4, 7, -18, -7, -4, -22, -3, -10, -6, -23, 0, 4, -15, -5, -10, 4, 3, -8, -18, -1, 4, -15, -6, 5, -13, -20, -18, -13, -10, -11}
, {-2, -6, 1, -8, -4, -19, 0, -7, -1, -18, -7, -13, -20, -13, 1, -9, 4, 2, 4, -5, 4, -3, -8, -10, -20, -7, 2, 3, 2, -7, -18, -8}
}
, {{-16, -4, -15, -7, 3, -23, -9, -1, -8, -8, -15, -12, -17, 4, 4, -11, 3, -19, -1, -10, -15, -4, -5, -10, 2, -14, -24, -4, -21, -14, -7, -1}
, {5, -18, -19, -10, -8, -14, -2, -15, -2, -20, 4, -12, -9, 1, 2, -1, -17, 4, -18, -15, -9, -22, -6, -22, -7, -19, -20, -13, -13, -19, -2, -8}
, {-1, 2, -1, -20, -13, -17, -5, -13, 13, -18, -7, 12, 3, 1, -2, -3, 2, -4, 4, 10, -10, -12, 2, 1, -6, -13, -15, -5, -11, -22, 6, -2}
}
}
, {{{28, -62, 8, 0, -11, -27, 41, -4, 12, -5, 27, 4, -39, 22, -21, 5, 2, -13, 17, -1, 13, 14, 14, -17, -11, 42, -36, 12, 7, 20, -12, 8}
, {29, -21, -1, 13, 58, 19, 17, 27, 31, -11, -7, 17, -27, -20, -5, -4, 11, -6, 49, 8, 80, 11, -7, 35, -25, -7, -28, -12, 43, -14, -14, -10}
, {-5, -17, 14, -2, 15, 31, -37, 12, -30, -12, 2, -18, 10, 5, 20, -7, 19, -49, -9, -12, 1, 21, 2, 10, 41, 27, 11, -10, -29, 8, 7, 2}
}
, {{-3, -30, -1, 5, 50, -12, 39, -2, 25, 3, -4, 2, -21, 4, -2, 16, -4, 1, 48, -23, 23, -2, -7, 15, -33, 14, -11, 11, 55, -3, -3, -22}
, {-11, -19, 7, 3, -15, 12, 2, 24, -1, -3, -11, -25, 3, -5, 13, -12, -15, -24, -10, 7, -14, 21, -7, 9, 25, 39, -28, -2, -32, -4, 13, -11}
, {-1, -11, -11, -12, -17, -30, -23, -8, -23, 5, -23, -13, -17, -4, -50, 1, 5, -25, -30, -17, -29, -25, 4, -10, 7, -10, -30, -3, -24, -6, -18, -11}
}
, {{-4, 9, -3, -14, -12, 12, 9, 18, -7, 0, 11, -19, 1, -3, -3, -19, -20, 1, 6, -15, -26, 10, -15, 0, -2, -2, -16, 8, 4, -10, -26, 5}
, {4, -2, 13, -15, -10, -28, 30, 11, 4, 3, -25, 4, 2, -17, -4, -7, -23, 7, -16, 20, -18, -30, -7, -4, -28, -34, -30, -17, -6, -12, -29, -20}
, {-26, 22, 10, -18, 23, 20, -4, -18, -10, 10, -3, 5, 5, -8, -8, 1, 5, -13, -24, -26, -13, -16, -10, -20, -15, 4, -26, -18, -24, -2, -8, -8}
}
}
, {{{21, 13, 5, 11, 7, -30, -25, 6, 13, 16, 6, 47, -16, 20, 27, -1, 6, -27, 1, 2, -8, 0, -6, -35, -24, 12, -49, 14, -54, 13, -18, 20}
, {11, 9, -15, 21, 7, -30, -45, -3, 48, 8, 20, 43, -43, 8, 8, 18, 24, -77, -22, -3, 13, 4, 9, -38, -7, -33, -74, 6, -41, 12, 15, 30}
, {25, -11, -36, 12, -8, -41, -4, -16, 39, -5, -3, 14, -54, 13, -6, 21, 3, -15, -1, -7, 20, -24, 8, 9, -28, -39, -25, 6, -3, 5, -9, -19}
}
, {{-19, 5, 20, -25, 1, -1, 27, -3, -6, -7, 1, 12, 2, -15, 10, -33, -29, 22, 25, -2, -22, 8, -20, 5, -11, 6, 7, -9, 11, -6, -23, -27}
, {-4, -1, 17, -20, 27, 4, -8, 12, 23, 0, -5, 41, 7, -10, 50, -16, -13, -9, -11, 28, 5, -20, -13, 39, -22, 22, 1, 4, -26, -16, -9, -24}
, {-2, 13, 18, 2, 26, -8, -13, -29, 21, -8, -3, -7, -4, -7, 17, 5, -15, -15, -2, -19, 14, -21, -3, -15, -13, 11, -14, -14, 10, 5, -8, -12}
}
, {{-29, -31, -10, -11, -34, -11, 28, -17, -75, -14, -5, -59, -7, -15, -39, -7, 7, 38, -6, -17, -29, -13, 0, 3, -1, -15, 23, -7, 11, -2, 3, -12}
, {-12, -8, 8, -18, -37, -26, 16, 58, -38, -18, -25, 0, 4, -21, -1, -13, -17, 34, 9, 18, -22, 0, -18, -1, -11, -1, 12, -8, 13, -22, -6, -22}
, {-6, 45, 33, -4, 51, 22, -36, 11, 2, -5, -19, 13, 46, -18, 29, -25, -15, -25, -4, 12, -1, 12, -11, -12, -13, 34, -34, 8, -4, 4, -16, 9}
}
}
, {{{-8, 1, 14, -7, 25, 5, 36, 5, 4, 0, 14, -28, 12, 10, 6, 19, 12, 42, 47, -23, 12, 4, -4, -5, -3, -5, 23, -11, 52, 12, 7, -7}
, {-1, -4, 19, 10, 13, 12, 10, 40, -16, -12, -5, 15, 14, -14, 26, 1, 10, 29, 23, 10, 39, 29, -4, 36, 31, 43, 22, 2, 43, -7, 19, -21}
, {-7, 11, 24, 2, 33, 41, -23, -2, 26, 8, 13, 18, 22, -18, 36, -22, -24, -22, 0, 20, 45, 29, -8, -9, 17, 52, 5, -12, -13, -7, -11, -7}
}
, {{2, 5, -30, -3, 7, -2, -29, -8, 3, 2, 2, 12, -5, -5, -3, -8, -11, -38, -2, -17, -31, -15, 3, -38, -15, -4, -25, 3, -4, -9, -21, -7}
, {13, -13, -13, 18, 31, -22, -16, 14, 28, -2, -4, 7, -20, 2, -2, 2, 5, -6, 12, 6, -2, -22, 6, 2, -6, -20, -1, 13, -6, -14, 3, 6}
, {3, -46, -18, 6, -15, -6, 6, 2, -2, -13, -3, 6, -39, 9, -5, 3, 8, -9, -7, -30, -14, -5, -10, -17, 14, -7, -9, -9, 9, 13, 13, 3}
}
, {{-10, -14, -36, -6, -27, -34, -10, -9, -18, -6, -25, 8, -19, 8, 1, 7, 13, 1, -17, 22, -27, -14, -2, -19, -30, -16, 8, -5, -27, -1, -7, -9}
, {-9, 5, 10, -10, 14, 0, -24, -11, 5, 9, -16, 14, 14, -11, -2, -11, -4, -13, -17, 4, 4, 3, -6, 8, -6, 13, -7, 0, -5, 10, 0, -14}
, {-2, 0, -24, -4, -9, -28, 27, -3, -15, 5, -4, -1, -10, 4, 4, -7, 8, 23, -5, 17, 0, -5, 11, -10, 4, -32, 2, -4, -8, -14, -3, 6}
}
}
, {{{-19, -5, 16, -26, -22, -2, 9, 16, -28, -15, -3, -40, 12, -27, 9, -15, -28, -19, -6, 1, -4, -12, -18, 11, -14, 3, -13, -26, -9, -17, -40, -18}
, {5, 0, -4, -5, 8, 4, -3, -31, -38, 9, 11, -35, 6, -6, -32, -14, 10, -23, -11, -10, -9, 42, 7, -5, 23, 40, -2, 12, -4, 7, -9, 14}
, {0, -29, 10, 16, 22, 1, -10, -28, -7, -1, 3, -27, -20, 12, -19, 19, 1, 1, -12, -16, -10, 6, 14, 3, 12, -10, 8, 2, 12, 5, 5, -6}
}
, {{-8, -8, 17, -13, 4, -2, -16, 11, -11, 9, -7, -13, 10, -3, -5, -10, -20, -36, 11, 26, 26, -19, -24, 33, -13, -5, -27, -19, -15, -1, -32, -9}
, {-8, -18, 7, 0, -14, 7, -34, 8, -16, -4, -4, -22, 3, -2, -1, -2, 11, -28, 0, -1, -19, 15, 6, 4, 7, 23, -27, 6, -9, 0, 3, 13}
, {9, 11, -4, 27, -38, 6, -15, 20, 18, -2, 13, -12, 2, 15, 13, 11, 11, -3, -17, -4, 0, 6, 18, -20, 27, -18, 14, 7, -3, 11, 29, 13}
}
, {{-9, 9, 28, 6, 23, 18, 38, 76, 6, 5, -28, 40, 22, -14, 32, 12, 1, -11, -8, 55, -24, -52, 12, -1, 12, 17, -5, -5, -24, 4, -6, -22}
, {15, 13, 0, 15, 60, -46, -3, 28, 59, 2, -8, 33, 0, -6, 8, 22, 5, -4, 2, 1, 37, -29, 11, -30, -23, 10, -9, 9, 1, -1, 4, 1}
, {-7, -10, -14, 9, 5, -17, -1, -17, 19, -4, 2, 28, -21, -9, 0, 5, 9, -6, -5, -10, -19, -3, 13, -18, 13, -11, 1, -1, -1, -15, 24, -6}
}
}
, {{{0, -21, -28, 10, -4, -33, -16, -8, -6, 7, 7, -10, -22, 6, -19, 1, -7, -9, -31, -9, -18, -11, 12, -19, -14, -33, -27, 2, -18, -8, -5, 18}
, {-1, -26, -22, -7, -8, -17, 10, -14, 7, -7, -14, 4, -27, -9, -15, 4, 11, 23, 41, 8, -4, 7, -12, -21, -19, 6, 12, 18, 20, -18, -6, -21}
, {-6, 13, 11, -32, 35, 14, -1, 32, 8, -14, -17, 9, 1, -41, 11, -48, -24, -3, 3, 17, 37, -10, -39, 29, -18, 13, -14, -26, 23, -26, -34, -17}
}
, {{-5, -44, -35, -15, -49, -34, 4, 2, 10, 10, -19, 21, -38, -4, -12, -14, -27, -2, 15, 17, 7, -30, -17, -12, -19, -15, -32, 5, -20, -2, -4, 2}
, {9, -22, 0, -19, 14, -15, 19, 0, 7, 2, -35, -1, -37, -33, -22, -12, -22, -16, -5, 10, 28, 10, -24, 36, -9, -28, -26, 0, 20, -41, -17, 1}
, {-37, 19, 15, -16, 43, 22, -9, 17, -5, 14, -11, -43, 25, -20, 19, -6, 2, -21, -13, 15, 25, 38, -1, 34, 4, 29, 2, -26, 1, 0, 3, 19}
}
, {{17, -25, 21, -31, 9, -24, 26, 45, 48, -6, -34, 27, -24, -13, 24, -21, -34, -18, 1, 33, 37, -58, -19, 23, -46, -20, -61, 3, -10, -7, -42, -7}
, {-22, 8, 27, 8, 56, 28, 3, 11, 23, -12, 5, 5, 28, -10, 12, -15, 1, -40, 9, -3, 24, -17, -14, 35, 10, 10, -17, -20, 16, -2, -12, 0}
, {-13, 9, 0, 21, 15, 36, -38, -10, -17, 11, -1, -25, 31, 3, -27, -2, 12, -36, 4, -34, -17, 8, 22, -7, 28, 19, -23, -7, 3, 11, 22, 8}
}
}
, {{{4, -20, -62, -2, -11, -34, 8, -48, 5, 12, -6, -23, -22, -2, -49, 9, 19, 13, 13, -20, -39, -16, 7, -54, -20, -41, 19, 0, 25, 0, 4, -3}
, {4, -27, 8, -1, -27, -20, 26, 24, -18, -13, -8, 2, -7, 1, 10, -1, -6, 20, 7, 17, 18, 1, -16, 38, 5, 14, 21, -7, 22, -19, -8, -19}
, {-42, 18, -10, -27, 41, -3, -39, -31, -14, 2, -3, -34, -7, -12, -17, -7, -20, 2, -24, -17, -3, -9, -17, -13, -14, 16, -21, -31, -13, -5, -36, -24}
}
, {{-1, -25, 0, 10, -14, -14, 7, 5, 0, -8, -9, -11, -24, -5, -16, 1, 7, 7, -18, -12, 4, 5, 16, -24, 0, -28, -10, -15, -13, -1, 11, 15}
, {-6, -20, 32, 6, 12, -1, 28, 32, 16, -3, -6, 6, -33, -4, 5, 4, -1, 33, 33, 23, 35, 15, -22, 38, -19, 11, 4, -3, 24, -8, -29, -9}
, {-34, 14, 8, -6, 2, 13, -53, -17, -29, 8, -5, -32, 24, -12, 34, -20, -18, -35, -16, -29, -2, 8, -2, -26, 18, 53, -34, -17, -29, -8, -10, -18}
}
, {{25, -4, -9, -12, 5, -27, -11, -1, 39, -3, -11, 11, -29, -6, -1, 0, -14, -8, 11, 17, 13, -1, -14, -9, -29, -5, -23, 7, 13, 8, -20, -24}
, {12, -25, 12, 8, 14, -7, -26, 29, 12, -6, 29, -15, -11, 5, -11, 4, 4, 2, 35, 0, 22, 32, 0, 9, 19, -7, 5, -4, 37, 13, 16, 32}
, {5, -23, -63, 4, -56, -5, -33, -35, -29, 3, -6, -11, -15, 12, -33, -1, -3, -33, -32, -22, -44, 5, 7, -43, 15, -5, -20, 14, -27, 8, 13, -5}
}
}
, {{{4, 15, 18, -20, 5, 0, -4, 10, 5, 5, 8, 30, 35, -24, 37, -19, -36, -27, -32, 11, -32, -4, -12, -19, -1, 38, -13, 6, -60, 2, -36, -36}
, {14, 48, 19, -5, 26, -6, -14, 16, 27, 0, 3, 37, 49, 8, 48, 2, -7, -62, -4, 22, -5, 10, -12, -6, -32, 20, -28, 29, -50, 11, -19, -2}
, {16, 24, 7, -5, 2, -27, -5, 33, 26, -2, 7, 49, 2, 9, 52, 19, 6, -3, 21, 22, -14, -21, 6, -6, -13, -19, 0, 27, -6, -6, -9, -4}
}
, {{2, 2, -10, -22, -7, -21, 17, 2, 13, -12, -13, 17, 18, -39, 8, -27, -24, 11, 2, -10, -6, -36, -20, -8, 2, -13, 16, -3, 4, -25, -17, 4}
, {-5, 10, 24, -54, 8, 12, -5, -6, 19, -5, -35, 11, 2, -27, 6, -29, -22, 14, -24, -5, -13, -7, -49, 9, -20, 27, -1, -15, -10, -36, -39, -13}
, {5, -12, -4, -26, -5, -4, 24, 11, 9, -12, -17, 12, -1, -35, 4, -43, -37, -10, -8, 11, -1, -39, -29, 15, -33, -5, 6, -5, -23, -38, -37, -27}
}
, {{-4, 13, 31, -4, 4, 7, 3, -4, -8, -5, -25, 1, -5, -31, 1, -18, 1, 18, 2, -2, 14, 2, -15, 19, 41, -22, 28, -27, -2, -34, 2, -8}
, {-43, -25, 15, -6, 22, 3, 14, 10, -24, 8, 11, -34, -7, -33, 5, 3, -6, 22, -14, -7, 0, 10, 9, 6, 11, 9, 11, -38, 14, -11, 5, 16}
, {-28, 0, 11, -6, -4, -5, 31, 13, -26, -6, -22, -7, -13, -26, -5, -10, -7, 10, 25, 25, 13, -3, -15, 38, -1, -35, 22, -21, 11, -30, 0, -16}
}
}
, {{{-2, 6, -9, -10, -16, -8, -15, -15, -6, 1, -6, -6, 13, -3, -9, -6, -7, -8, 8, -2, 11, -3, -5, -14, -3, -9, -7, -4, -11, 5, -14, -9}
, {3, 6, -5, 3, -3, -12, -5, -7, -8, -11, -15, 1, 7, -5, 6, 2, -13, -9, -2, -10, 1, -15, -4, -9, -12, 3, -1, 1, -5, -15, -7, 0}
, {-6, -9, -4, 5, -2, -2, -11, -8, 7, -14, -5, 0, 2, -13, 9, -4, -6, -1, -2, 2, -2, 7, 0, 0, -6, -13, 8, 1, -12, -3, 7, -15}
}
, {{-5, -11, 7, -5, -10, -8, -7, 5, -12, -4, -2, -8, 0, -9, -3, -15, 1, -1, -9, -14, 7, 11, -15, 1, 1, -4, -9, 7, 6, 8, -1, -10}
, {3, 0, -8, 2, 6, 3, -12, -11, 1, -1, -10, -12, 8, -15, -4, -8, -15, -13, 6, -5, 5, 11, 2, -1, 0, 9, -10, 2, 9, -2, -6, 8}
, {-13, 1, -13, 3, 3, -11, -11, 5, 1, -14, -8, -4, -12, -2, -4, -1, -3, -1, -4, -5, -2, 2, 10, 6, -1, -12, 4, -1, -15, 7, -15, -2}
}
, {{-9, 4, 2, 1, -7, -10, 4, -15, 11, 8, -15, 6, -8, 3, -1, -11, 1, -7, 1, -7, -11, -3, -10, 12, 2, -3, 0, 2, -11, 6, 4, 11}
, {-1, -9, -11, 7, -1, 2, 4, 0, -7, -10, 1, 5, -4, -13, -10, -16, -6, -4, -16, 7, -9, 7, -5, 5, -2, -8, 5, -3, -15, -12, -12, -7}
, {-5, -11, -13, -5, -14, 1, -18, -14, 9, -13, -3, 3, -8, 2, -10, -7, 4, -9, -3, -5, 2, 7, -3, 1, -4, -3, -1, -10, 8, -8, 1, -10}
}
}
, {{{-3, 39, 24, -9, 30, 11, 6, 14, -8, 8, -2, 1, 17, -8, 16, -36, -24, -13, -1, 22, 17, -28, -31, 8, -40, 36, -41, 2, -28, 3, -47, -14}
, {14, 21, 37, -14, 28, -7, -3, 28, 9, -5, -25, 17, 6, 5, 21, 0, -32, 0, 22, 2, 18, -20, -22, 14, -28, 2, -24, 4, 5, -9, -31, -15}
, {0, 9, -18, -14, 32, 14, -16, 16, 8, 13, -2, 11, 8, -3, 1, -9, 13, 1, -5, -14, -11, 16, 10, -13, -15, 32, -26, 18, -6, 8, -18, 10}
}
, {{-5, 21, -18, 0, -38, 24, 8, 6, -7, 3, -14, 1, 13, 5, 3, -5, -6, 31, -1, -5, -8, 10, 3, -24, 12, 6, 17, -17, 17, 0, 3, -11}
, {-9, 9, -3, -3, -37, -10, 12, 31, 2, -1, 1, 26, 17, -22, 17, -11, -2, 24, -14, -1, -1, -7, -18, 1, -6, 10, 15, -12, 1, -21, -17, -10}
, {-7, 0, -17, -12, -20, 9, 4, 5, 29, -6, 2, 17, 2, -21, 11, -8, -17, 25, 15, 7, -29, 0, -14, -14, -12, 4, 1, -8, 16, -16, -23, -22}
}
, {{-10, -19, -19, -13, -32, -21, -25, -38, -39, -7, 10, -32, -39, -4, -53, 12, 9, 10, -25, -40, 4, -2, 22, -17, 6, -34, 5, -3, 22, -14, 11, 16}
, {-8, -12, -34, 23, -22, -10, -12, -36, -30, 10, 15, -20, -40, 0, -17, 17, 29, 34, -31, -41, 26, 32, 7, -16, 27, -37, 42, -5, 13, -11, 37, 22}
, {-10, -51, -21, 23, -10, -3, -6, -31, -21, 7, 1, -25, -43, 0, -33, 21, 22, 15, 5, -15, 7, 15, 22, 6, 39, -27, 37, -6, 4, 6, 24, 15}
}
}
, {{{6, -25, -3, 4, -36, -18, -14, -13, -22, -9, 15, -18, -17, -13, -36, -6, 3, -26, -11, -35, -68, 10, 18, -33, -3, -3, -28, -6, -26, 12, 4, -4}
, {18, -23, -41, 1, -22, -28, -21, 2, 8, 13, -9, 38, -64, 2, 2, -6, -20, -43, -23, 1, 19, -11, 1, -30, -17, -29, -51, 15, -39, 1, -4, 8}
, {18, -40, -26, -2, 29, -28, -11, 1, 47, -1, -7, 19, -56, 12, 0, -11, -16, -46, 31, 21, 23, -14, -25, -4, -65, -11, -52, 16, 19, -10, -19, -22}
}
, {{18, -12, 21, 2, 4, -43, 12, 12, 14, -1, 5, 23, -16, 10, -7, -6, 0, -7, 12, -5, 28, -4, 4, -7, -21, 1, -22, 8, 14, -5, -20, -12}
, {16, -24, 33, 6, 50, 3, 14, 15, 26, -4, 19, 22, -25, 4, -6, 5, -2, -16, 33, -1, 43, 35, 0, 34, -8, 64, -17, -15, 12, -1, 0, -4}
, {-19, -22, 5, -10, 16, 26, 3, 9, 12, 10, 0, -12, -14, -23, 13, -18, 3, -15, 22, 9, 40, 22, -18, 22, 18, -6, -18, -34, 5, -15, -5, -1}
}
, {{-13, 7, 19, 11, 13, 10, -6, 11, 18, 5, 4, -13, 0, -15, 29, 3, 17, -15, 23, -19, 22, -3, 3, 37, 14, 12, -7, -19, 35, -10, -2, -31}
, {-29, 3, 15, 16, 25, 10, 7, 0, -7, -16, -18, -22, 2, -3, 21, 4, 7, -1, 28, 3, 20, -22, 8, 32, 9, 30, -21, -18, -3, -13, -5, -34}
, {5, 3, 2, 21, 0, 2, 2, -2, -40, -10, -21, -18, 13, -4, -20, 12, 5, -5, 13, -12, -27, -7, 4, -4, 28, 31, -17, 9, 1, 0, 4, -22}
}
}
, {{{26, -55, -46, -11, 2, -45, 28, 8, 40, 9, 14, 21, -57, 9, -8, -9, -23, 7, 33, 12, 38, -16, 7, 14, -29, -8, -1, 13, 23, -15, -26, 18}
, {10, -50, -24, -39, 12, -14, 35, 8, 18, -16, -15, 13, -46, -48, -11, -18, -18, -20, 10, 1, 32, -22, -28, 42, -32, -50, -23, -10, 21, -58, -35, -2}
, {-36, -13, 1, 17, 49, 25, -5, -10, -13, -2, 6, -28, 23, -21, -10, 15, 19, 1, 5, -18, 36, 20, 19, 35, 52, 2, 40, -27, 32, -2, 36, 18}
}
, {{6, -60, -1, -8, -10, -36, 2, 16, 24, -20, -4, 14, -28, -18, 18, -29, -22, -13, 1, 8, 61, -42, -30, 19, 7, -13, -5, 4, 4, -29, -13, -17}
, {-40, -23, 0, -3, 10, 0, 5, 2, -11, -7, -32, -10, -15, -29, 23, -27, -10, -16, 0, -16, 5, -1, -13, 24, -6, 6, 0, -29, -9, -15, 2, -9}
, {-25, 5, 9, 16, -2, 26, -53, 23, -21, 4, -4, -21, 16, -10, -3, -13, 17, -44, -41, 3, -1, 10, -5, 27, 34, 29, 10, 8, -43, 14, 0, -4}
}
, {{-25, 23, 14, -34, 9, 2, 10, 23, -21, -18, -42, -1, 17, -45, 20, -33, -27, 10, 7, 9, -13, -76, -21, 17, -17, -42, -21, -34, -6, -36, -33, -34}
, {-27, 25, 14, -8, 23, 20, 13, 26, 5, 12, -35, 27, 40, -28, 21, -34, -30, -18, -15, 6, -14, -44, -13, 7, -11, 26, -21, -4, -13, -26, -14, -31}
, {-1, 11, -5, 1, 15, 2, -47, 13, 55, -2, -10, 37, 11, -5, 13, 3, 11, -48, -41, 2, 3, 9, -3, -31, -22, 15, -8, -2, -58, 6, -5, 8}
}
}
, {{{-3, -27, 19, 19, 27, -4, 28, 42, -22, -2, 0, -7, -26, -3, 5, 36, 18, 43, 67, 22, 41, -2, 4, 54, 19, -11, 30, -7, 50, -15, 17, -5}
, {-20, -17, 30, 27, 1, 32, 13, -15, -4, 2, 1, -4, 4, 9, -8, 5, 11, 19, 31, -19, 6, 36, 19, -10, 47, 31, 28, -16, 24, 11, 25, 4}
, {-8, -1, 26, 9, 9, 8, -19, 8, -30, 10, -1, -22, 11, 10, 27, 7, -4, -24, 14, -1, 0, -4, 3, 29, 13, 31, -11, 7, -5, 9, -8, -15}
}
, {{-16, -23, -5, -1, 11, 5, -26, -4, 8, -4, 9, 6, -29, -15, 2, 3, 5, -27, -24, -13, 25, -31, 8, -18, -13, -21, -5, -8, -6, 1, -13, 0}
, {6, -27, -58, -1, -24, -11, -14, -3, 0, 5, 12, 19, -3, -2, -3, -6, -4, -21, -1, -16, -39, -16, 4, -30, -22, 6, -25, 8, -29, -7, 13, 6}
, {-5, 2, 4, -20, -15, 4, -8, 13, 23, -5, -11, -1, -9, 8, 21, -3, -3, -42, -8, 22, -32, -14, -10, -19, -12, -5, -45, -2, -11, 4, -29, 2}
}
, {{13, -1, -5, 6, -3, 0, -3, -12, -11, -5, -4, -27, -10, -2, -15, -8, -1, -20, -18, -28, -19, -17, 9, -24, -2, 13, -3, 5, -9, 11, -13, -6}
, {7, -50, -20, -35, -51, -46, 35, -10, 7, 2, -13, 14, -62, -4, -6, -22, -32, 6, 19, 1, 1, -13, -7, -4, -28, -53, -16, 0, -7, -15, -8, -10}
, {-12, 21, 28, -20, 19, 27, -19, -1, -5, 5, -17, 2, 18, -7, 13, -14, 1, -3, -5, -22, 0, 14, -17, 26, -12, 14, -3, 7, -17, -11, 2, -15}
}
}
, {{{-13, -11, 11, 5, 1, 7, 7, 1, 3, -14, -4, -3, 3, -5, 12, -8, 8, -4, -12, -3, 8, -3, 9, 15, 1, 5, -11, -10, -8, 5, 6, 8}
, {-7, -2, -4, -15, -1, 1, -2, -7, 3, -9, -17, -2, -9, 8, -1, 9, 3, 1, -1, 2, -1, -10, -10, 7, 6, -8, -9, -7, 2, -2, -5, 10}
, {8, -17, -6, -17, -15, 4, -14, 2, 4, -10, -8, -11, -13, 6, -13, 9, 5, -4, -2, -5, -18, 7, -10, -5, -2, -14, -6, 0, -11, 5, -13, 2}
}
, {{-8, 10, -4, -3, 3, -12, -11, -12, 6, -5, -14, 4, 4, -3, -8, -2, -14, -17, -8, -1, 2, -15, -6, -4, 4, -1, 5, 7, 6, -1, -13, -3}
, {-13, -12, -8, 4, -11, -1, -15, -5, 4, 4, 3, -11, 9, 0, -4, 1, -8, -2, 9, 9, -13, -7, -16, -2, 3, 12, 0, 10, 8, -10, -5, 2}
, {-12, 0, -8, -15, -6, -4, -8, -17, 4, 8, -18, -1, -1, -7, -6, 6, -3, -10, -11, 0, -2, -1, -16, -18, 4, 7, 6, 8, -3, -5, -11, 1}
}
, {{-3, -5, 5, -6, -15, -7, 4, 1, -9, -12, -11, -2, -5, -14, -12, -16, -1, 6, -9, 7, 10, -5, -14, -18, -11, -10, -8, 7, 6, 8, 5, -11}
, {3, -3, 6, 5, -14, 10, -10, 0, 7, -10, -7, -1, -10, -1, -11, -7, 7, -15, -15, 6, 1, -13, -8, -11, 11, -11, -14, -4, -16, -9, -15, 3}
, {-5, 11, -14, 7, -12, 0, 8, -3, 4, -9, -7, -7, 0, -8, 0, 6, -3, -6, 9, -12, -12, 8, -5, -5, -15, 2, -20, 6, -2, -10, -7, 3}
}
}
, {{{-21, -35, 0, -2, -10, -46, 29, -5, -38, -9, 7, -51, -14, -14, -55, 8, -6, 42, 39, 10, -6, -32, 10, -25, -18, -18, 24, -2, 35, -6, 9, -21}
, {-11, 22, 33, -21, 39, 27, 10, 4, -57, -20, -17, -38, 29, -31, -17, -7, -33, 27, -1, 11, 21, 1, -35, 26, -16, 40, -20, -23, 32, -18, -22, -21}
, {-10, -5, -4, 13, 20, 28, -23, 6, -18, -5, 16, -33, 26, -20, -8, -3, 10, -1, -37, -2, -2, 11, -12, -14, -2, 23, 7, 5, -21, -3, 17, 2}
}
, {{20, 14, -15, -18, 20, 0, 21, 16, 45, 5, -19, 18, 4, -12, 33, -2, -6, 22, 7, 6, 27, 7, -2, 9, -19, -25, 3, 1, 6, 3, -5, 8}
, {-21, 19, -20, 2, -16, -4, -9, 44, -16, 13, 3, -9, 32, -3, 20, -18, -3, 1, -15, -2, -9, 2, -11, -2, 0, 30, -13, -20, -4, -16, 1, -7}
, {-19, -1, 16, -12, -14, -29, 14, 27, 11, 13, -29, 14, -2, -22, -10, -13, -15, 0, -14, 6, -8, -7, -22, -11, -20, 1, -19, 15, -13, -1, -10, 4}
}
, {{-5, 5, -17, 6, 7, -8, -15, 13, -39, 8, 1, -10, -6, -20, -14, 7, 17, -8, -11, -22, -1, 4, 20, -4, 3, -18, 17, 3, -3, -9, 20, 8}
, {-9, -36, -11, 5, -47, -4, -18, -30, -14, 6, 6, 9, -41, -7, -10, 0, 20, 22, -13, -34, 13, 30, 25, -5, 10, -8, 20, -9, 36, -15, 17, 22}
, {-12, -9, 6, 2, 23, 17, 18, -12, 8, 2, 27, -4, -19, -21, 4, 10, 9, 4, 9, 10, 27, 1, -9, 30, 36, -30, 25, -3, 2, -24, 23, 16}
}
}
, {{{21, 4, 24, -15, 13, 34, 1, 13, 38, -17, 3, 21, 10, -9, 30, -20, -34, -13, 0, 38, 25, -11, -28, 20, -2, 7, 9, -6, 16, 1, -35, 7}
, {-1, 13, 1, -17, 27, 13, 13, 18, 32, -6, -6, 18, 15, -12, 0, -1, -16, -5, 20, -18, 22, 32, -6, -2, -7, 11, 5, 5, 36, 4, 0, 8}
, {-7, 9, 26, 5, 20, 12, 8, 26, 5, 6, -2, 5, 1, 1, 20, -19, -14, -3, 7, 30, 51, 17, -8, 22, 10, 7, 23, -6, 17, -17, -1, -4}
}
, {{17, -20, 13, 25, 9, -4, 3, 31, -11, 12, -10, 1, -11, 0, -10, 2, -8, 24, -8, 4, 39, -6, 2, 6, 22, 2, 19, -6, 33, -11, 12, -20}
, {-5, -26, -19, 5, -27, -7, -14, -20, -34, 4, -12, -9, -10, -8, -10, 12, 7, 14, -5, -21, -30, 8, 6, -29, 24, -5, 9, 7, -6, 5, 19, -3}
, {-12, -27, -10, 4, -18, 4, 5, -17, -7, -6, 0, -43, -12, -6, -19, 6, 9, 10, -8, -12, -3, 5, 17, -4, 20, -13, 30, -6, -12, 7, 19, 7}
}
, {{-4, 11, -28, -25, 12, -18, -46, 14, -2, -2, -3, 2, -13, 3, -4, -4, -21, -21, -25, -19, -41, -7, -16, -22, -27, -12, -39, -8, -24, -17, -23, -15}
, {14, -8, -4, 2, -26, -21, -24, 13, 6, 0, -10, 37, -15, -16, 10, -3, -2, 2, -25, 12, -18, -14, -2, -22, -31, -8, -7, 16, -38, 0, -3, -7}
, {13, -22, -56, 5, -9, -27, -10, 0, 18, -11, 1, 29, -29, -15, 9, -13, -5, -15, -20, 24, 8, -24, -1, -28, -17, -38, -24, -7, -13, -13, -19, -19}
}
}
, {{{14, -6, -13, -2, 11, -30, -11, -11, 16, 11, 0, -14, -26, 0, -2, 4, -3, -10, 8, -4, 11, -4, 5, 3, -28, -29, -21, 7, 4, -1, -15, 5}
, {-10, -40, -41, 10, -1, -12, -39, -45, -5, 2, -3, -24, -28, -1, -33, 2, 19, -6, -2, -39, -11, -3, -10, -34, 1, -27, 27, -4, 10, 6, 1, -6}
, {-2, -25, 5, -4, -4, -5, -31, -12, -15, -3, 1, -14, -12, 11, -18, 21, 20, 29, 6, -25, -23, -6, 22, -26, 29, -14, 46, 13, 14, 5, 40, 20}
}
, {{22, -3, 21, -5, -31, -1, 7, 37, 39, -2, -2, 33, 10, 6, 25, -13, -33, -21, -5, 48, 16, -15, -16, 1, -44, 13, -43, -4, -21, -4, -25, -32}
, {-3, 30, -4, -19, 7, -22, -1, 24, 36, 10, -15, 24, -6, -11, 15, -10, -32, -21, 4, 18, 2, -32, -22, -8, -43, 0, -60, 13, -30, -17, -21, -16}
, {-1, 8, 0, -4, -7, -29, -28, 19, 14, 3, -20, 44, -2, 0, 28, -11, -23, -15, -6, 18, -13, -30, -20, -8, -40, -3, -60, 13, -30, 4, -27, -21}
}
, {{-4, -53, 17, 22, -35, -3, 7, 4, -22, -7, 7, -18, -24, -4, -45, 9, 15, 10, 9, -12, 29, 4, 1, 18, 5, -29, 25, -13, 7, 1, 0, -4}
, {-8, -7, -3, 12, 0, 23, 18, -14, -37, 6, 3, -46, 7, -5, -41, -8, 22, 28, -11, -11, 2, 44, 3, 15, 25, 28, 34, -18, 30, 5, 22, 20}
, {-18, -9, 12, 2, -19, 19, -17, -32, -23, -12, -1, -41, 9, 6, -17, -8, -8, 5, -15, -8, -2, 14, -6, 3, 8, 5, 34, -17, 18, 2, 6, 18}
}
}
, {{{2, -16, -19, -5, 2, -10, -31, -25, -27, -1, 12, -27, -20, 8, -16, 2, 10, -3, -16, -56, -19, 13, 9, -13, 22, 1, 25, -7, 16, 6, 22, 12}
, {4, -41, -1, 3, -16, -19, -38, -58, -16, 11, 0, -52, -44, 5, -56, 18, 13, 14, -24, -31, -31, 4, 22, -30, 32, -42, 21, -3, -1, 9, 26, 2}
, {-7, -24, -18, -3, -32, -16, -9, -29, -22, -10, 6, -5, -18, -11, -38, 6, 6, 10, -25, -23, -40, -3, 15, -30, 38, -15, 29, 7, 8, 10, 22, 2}
}
, {{-1, -1, -10, 16, 11, 15, -29, 2, 33, -3, 16, 29, 3, 2, 27, 16, 14, -26, -3, -4, -21, 8, 20, -28, -7, -5, -9, 1, -29, 8, 21, -8}
, {22, 7, -8, 15, 16, -11, -58, -7, 37, 2, 4, 26, -16, -5, 14, -5, -1, -18, 20, 4, 1, -21, -3, -5, 7, 1, 0, 4, -37, -4, 24, -21}
, {-7, 7, -10, 9, 27, 3, -27, 5, 24, -3, -12, 13, -26, 5, 15, 12, 20, -8, -1, -7, -31, -24, 6, -21, -23, -9, -3, -2, 2, -11, 1, 1}
}
, {{1, 46, 58, -40, 31, -4, 29, 9, 28, 4, -34, 38, 25, -22, 36, -23, -30, 27, -10, 17, 53, -23, -29, 52, -22, -3, -3, -4, 8, -18, -35, -16}
, {-5, 11, 3, -38, 34, 23, 23, 27, 18, -2, -15, 29, 34, -9, 30, -35, -47, -1, -5, 11, 15, 8, -36, 17, -40, 25, -14, 4, -6, -9, -31, -3}
, {-5, 14, 0, -13, 5, 11, 0, 37, 32, -8, -3, 23, 16, 5, 42, -18, -16, -21, 14, 51, 19, 19, -19, 3, -36, 23, -8, -14, -18, 4, -42, -18}
}
}
, {{{9, 24, 13, -8, -36, 10, 30, -26, -40, -3, 7, 14, 31, 10, -8, -10, -17, 15, -2, -26, -33, 7, -9, -37, 4, 14, -4, 7, -15, -2, -19, -2}
, {27, -18, -27, -16, 17, -8, -4, -1, 17, -5, -29, 11, -15, -1, 4, -10, -16, -15, 9, 25, 35, -11, -37, 25, -59, 13, -17, 3, 12, -33, -42, -7}
, {-1, 0, 22, -14, 32, 7, 13, 16, -4, 2, -10, -2, -3, -10, 14, -25, -9, -15, 11, 6, 32, -30, -23, 35, -8, -5, -12, -15, 13, -18, -1, 2}
}
, {{30, -29, 7, 7, -23, -24, 17, 1, 46, -1, 5, 19, -15, 4, 8, -19, -18, -12, 40, 5, 44, -17, -6, -2, -15, 20, -30, 2, 5, 1, -16, -7}
, {-25, -39, -15, -22, -10, -8, 8, -1, 7, -17, -16, -4, -20, -23, 8, -34, -30, -20, 0, -21, 8, -13, -20, 30, -11, -12, -11, -29, -6, -28, -16, -7}
, {-29, 14, 18, -10, 18, 23, 0, -15, -32, 8, 5, -30, 10, -2, 22, -1, -1, -15, -12, -11, 15, 20, 6, 43, 8, 37, -11, -17, 0, -7, -15, -11}
}
, {{0, -15, 6, -25, 5, -13, 12, 30, 21, -8, -22, 20, -3, -43, 23, -17, -33, -25, -3, 7, 11, -50, -30, 19, -16, -14, -40, -13, -11, -44, -34, -6}
, {-34, 28, 23, 7, 31, 18, -13, -6, -5, -15, -21, -11, 16, -31, 7, -9, 6, -25, -26, -13, 5, -4, 2, 20, -14, -8, -28, -16, -20, -12, 2, -12}
, {2, 22, 22, -7, -16, 22, -15, 12, -20, 4, -8, -16, 9, -9, 16, 5, -1, -28, -21, -19, -15, 4, 17, -4, 28, 21, -22, 4, -14, 11, 14, -11}
}
}
, {{{-14, -13, 7, -13, -2, 1, 41, 17, -38, -5, -11, -5, -12, -28, 4, -1, -4, -2, 11, 3, -7, -14, -17, 8, -15, -25, -5, 2, 28, -17, -20, -30}
, {-9, 25, 4, 7, 17, 23, -29, -18, -19, -2, 14, -30, 23, -11, 2, -7, -1, -48, -16, 0, -7, 23, -8, 3, 2, 34, -9, 2, -6, 16, -2, 14}
, {-2, -44, -25, 13, -22, -7, 10, -37, -3, 11, -8, -18, -37, -13, -67, 19, 17, 22, 16, -14, -20, -5, 4, -10, 5, -16, 3, -11, 6, -16, 21, -11}
}
, {{-10, 12, 7, -19, 14, 1, 24, 19, -4, -10, -7, -26, 4, -28, 5, -18, -13, 13, 5, 34, 5, -3, -32, 29, -10, 5, -4, -11, -2, -32, -24, -1}
, {-8, 7, -3, -5, 9, 2, -32, 6, -20, 12, 0, -1, 19, 10, 22, -10, -10, -20, -37, -32, -21, 23, -3, 0, 4, 36, -26, 9, 11, 1, -1, 3}
, {-9, 0, -15, 3, 15, -14, -8, -10, 18, 16, 3, -19, -9, 4, -15, 9, 28, 26, 17, -9, 14, 7, 7, -2, 16, -26, 23, -7, 4, -11, 28, 2}
}
, {{-27, 22, 48, -12, 0, -3, 62, 44, -42, -9, -35, 23, 9, -31, 19, -10, -10, 38, 35, 41, 6, -32, -15, 32, 6, -3, 12, -14, 17, -17, -19, -41}
, {10, 40, 6, -3, 55, 16, -24, 23, 36, 7, -4, 14, 29, 9, 28, 1, -21, -10, -18, 21, 7, -24, -7, -40, -24, 16, -39, 8, -26, 6, -27, -13}
, {-3, 5, -17, -3, 7, -27, -19, 1, 17, 0, 13, 29, -12, 4, -6, 5, 3, -24, -17, -17, -15, 9, -8, -45, -24, -19, -12, 9, -14, 1, 4, 11}
}
}
}
;

#undef INPUT_CHANNELS
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_GROUPS
/**
  ******************************************************************************
  * @file    maxpool2d.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    21 april 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _MAX_POOLING2D_3_H_
#define _MAX_POOLING2D_3_H_

#ifndef SINGLE_FILE
#include "number.h"
#endif

#define INPUT_CHANNELS  32
#define INPUT_HEIGHT    11
#define INPUT_WIDTH     11
#define POOL_SIZE_Y     2
#define POOL_SIZE_X     2
#define POOL_STRIDE_Y   2
#define POOL_STRIDE_X   2
#define POOL_PAD_Y      0 // Unsupported
#define POOL_PAD_X      0 // Unsupported
#define POOL_HEIGHT	    ( ( (INPUT_HEIGHT - POOL_SIZE_Y + (2*POOL_PAD_Y) ) / POOL_STRIDE_Y ) + 1 )
#define POOL_WIDTH	    ( ( (INPUT_WIDTH - POOL_SIZE_X + (2*POOL_PAD_X) ) / POOL_STRIDE_X ) + 1 )

typedef int16_t max_pooling2d_3_output_type[POOL_HEIGHT][POOL_WIDTH][INPUT_CHANNELS];

#if 0
void max_pooling2d_3(
  const number_t input[INPUT_SAMPLES][INPUT_CHANNELS], 	    // IN
  number_t output[POOL_LENGTH][INPUT_CHANNELS]); 	// OUT
#endif

#undef INPUT_CHANNELS
#undef INPUT_WIDTH
#undef INPUT_HEIGHT
#undef POOL_SIZE_X
#undef POOL_SIZE_Y
#undef POOL_STRIDE_X
#undef POOL_STRIDE_Y
#undef POOL_PAD_X
#undef POOL_PAD_Y
#undef POOL_HEIGHT
#undef POOL_WIDTH

#endif//_MAX_POOLING2D_3_H_
/**
  ******************************************************************************
  * @file    maxpool2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 november 2021
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "max_pooling2d_3.h"
#include "number.h"
#endif

#define INPUT_CHANNELS  32
#define INPUT_HEIGHT    11
#define INPUT_WIDTH     11
#define POOL_SIZE_Y     2
#define POOL_SIZE_X     2
#define POOL_STRIDE_Y   2
#define POOL_STRIDE_X   2
#define POOL_PAD_Y      0 // Unsupported
#define POOL_PAD_X      0 // Unsupported
#define POOL_HEIGHT	    ( ( (INPUT_HEIGHT - POOL_SIZE_Y + (2*POOL_PAD_Y) ) / POOL_STRIDE_Y ) + 1 )
#define POOL_WIDTH	    ( ( (INPUT_WIDTH - POOL_SIZE_X + (2*POOL_PAD_X) ) / POOL_STRIDE_X ) + 1 )

#define ACTIVATION_LINEAR

// For fixed point quantization
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void max_pooling2d_3(
  const NUMBER_T input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS], 	    // IN
  NUMBER_T output[POOL_HEIGHT][POOL_WIDTH][INPUT_CHANNELS]) {	// OUT

  unsigned short pos_x, pos_y, k; 	// loop indexes for output volume
  unsigned int x, y;
  LONG_NUMBER_T max, tmp;

  for (k = 0; k < INPUT_CHANNELS; k++) 
    for (pos_y = 0; pos_y < POOL_HEIGHT; pos_y++) {
      for (pos_x = 0; pos_x < POOL_WIDTH; pos_x++) {
#ifdef ACTIVATION_LINEAR
        max = input[pos_y*POOL_STRIDE_Y][pos_x*POOL_STRIDE_X][k];
        x = 1;
#elif defined(ACTIVATION_RELU)
        max = 0;
        x = 0;
#else
#error "Unsupported activation function"
#endif
        for (y = 0; y < POOL_SIZE_Y; y++) {
          for (; x < POOL_SIZE_X; x++) {
            tmp = input[(pos_y*POOL_STRIDE_Y)+y][(pos_x*POOL_STRIDE_X)+x][k];
            if (max < tmp)
              max = tmp;
          }
          x = 0;
        }

        output[pos_y][pos_x][k] = scale_and_clamp_to(NUMBER_T, max, INPUT_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
      }
    }
}

#undef INPUT_CHANNELS  
#undef INPUT_WIDTH
#undef INPUT_HEIGHT
#undef POOL_SIZE_X
#undef POOL_SIZE_Y
#undef POOL_STRIDE_X
#undef POOL_STRIDE_Y
#undef POOL_PAD_X
#undef POOL_PAD_Y
#undef POOL_WIDTH
#undef POOL_HEIGHT
#undef ACTIVATION_LINEAR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
/**
  ******************************************************************************
  * @file    flatten.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 january 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _FLATTEN_1_H_
#define _FLATTEN_1_H_

#ifndef SINGLE_FILE
#include "number.h"
#endif

#define OUTPUT_DIM 800

typedef int16_t flatten_1_output_type[OUTPUT_DIM];

#if 0
void flatten_1(
  const number_t input[5][5][32], 			      // IN
	number_t output[OUTPUT_DIM]); 			                // OUT
#endif

#undef OUTPUT_DIM

#endif//_FLATTEN_1_H_
/**
  ******************************************************************************
  * @file    flatten.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 2.0.0
  * @date    26 november 2021
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "flatten_1.h"
#include "number.h"
#endif

#define OUTPUT_DIM 800

#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t

static inline void flatten_1(
  const NUMBER_T input[5][5][32], 			      // IN
	NUMBER_T output[OUTPUT_DIM]) {			                // OUT

  NUMBER_T *input_flat = (NUMBER_T *)input;

  // Copy data from input to output only if input and output don't point to the same memory address already
  if (input_flat != output) {
    for (size_t i = 0; i < OUTPUT_DIM; i++) {
      output[i] = input_flat[i];
    }
  }
}

#undef OUTPUT_DIM
#undef NUMBER_T
#undef LONG_NUMBER_T
/**
  ******************************************************************************
  * @file    fc.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 january 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _DENSE_2_H_
#define _DENSE_2_H_

#ifndef SINGLE_FILE
#include "number.h"
#include <stdint.h>
#endif

#define INPUT_SAMPLES 800
#define FC_UNITS 128

typedef int16_t dense_2_output_type[FC_UNITS];

#if 0
void dense_2(
  const number_t input[INPUT_SAMPLES], 			      // IN
	const number_t kernel[FC_UNITS][INPUT_SAMPLES],  // IN

	const number_t bias[FC_UNITS],			              // IN

	number_t output[FC_UNITS]); 			                // OUT
#endif

#undef INPUT_SAMPLES
#undef FC_UNITS

#endif//_DENSE_2_H_
/**
  ******************************************************************************
  * @file    fc.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 1.0.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "dense_2.h"
#include "number.h"
#endif

#ifdef WITH_CMSIS_NN
#include "arm_nnfunctions.h"
#elif defined(WITH_NMSIS_NN)
#include "riscv_nnfunctions.h"
#endif

#define INPUT_SAMPLES 800
#define FC_UNITS 128
#define ACTIVATION_RELU

// For fixed point quantization
#define WEIGHTS_SCALE_FACTOR 7
#define BIASES_SCALE_FACTOR 7
#define TMP_SCALE_FACTOR 7
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void dense_2(
  const NUMBER_T input[INPUT_SAMPLES], 			      // IN
	const NUMBER_T kernel[FC_UNITS][INPUT_SAMPLES],  // IN

	const NUMBER_T bias[FC_UNITS],			              // IN

	NUMBER_T output[FC_UNITS]) {			                // OUT

#if !defined(WITH_CMSIS_NN) && !defined(WITH_NMSIS_NN)
  unsigned short k, z; 
  LONG_NUMBER_T output_acc;

  for (k = 0; k < FC_UNITS; k++) { 
    output_acc = 0;
    for (z = 0; z < INPUT_SAMPLES; z++) 
      output_acc = output_acc + ((LONG_NUMBER_T)kernel[k][z] * (LONG_NUMBER_T)input[z]);

    output_acc = scale(NUMBER_T, output_acc, WEIGHTS_SCALE_FACTOR - TMP_SCALE_FACTOR, OUTPUT_ROUND_MODE);

    output_acc += scale(NUMBER_T, (LONG_NUMBER_T)bias[k], BIASES_SCALE_FACTOR - TMP_SCALE_FACTOR - INPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);


    // Activation function
#ifdef ACTIVATION_LINEAR
    // Linear (MEANS NONE)
    output[k] = scale_and_clamp_to(NUMBER_T, output_acc, INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
#elif defined(ACTIVATION_RELU) || defined(ACTIVATION_RELU6)
    // ReLU
    if (output_acc < 0) {
      output[k] = 0;
    } else {
#if defined(ACTIVATION_RELU6)
      if (output_acc > scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE)) {
        output_acc = scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE);
      }
#endif
      output[k] = scale_and_clamp_to(NUMBER_T, output_acc, INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
    }
#else
#error "Unsupported activation function"
#endif
  }
#else

#if BIASES_SCALE_FACTOR > WEIGHTS_SCALE_FACTOR
#error "CMSIS-NN does not support BIASES_SCALE_FACTOR larger than WEIGHTS_SCALE_FACTOR"
#endif

  static q15_t bufferA[INPUT_SAMPLES];
#ifdef WITH_CMSIS_NN
  arm_fully_connected_q15(
#elif defined(WITH_NMSIS_NN)
  riscv_fully_connected_q15(
#endif
                             (q15_t*)input,
                             (q15_t*)kernel,
                             INPUT_SAMPLES,
                             FC_UNITS,
                             INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - BIASES_SCALE_FACTOR,
                             INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - OUTPUT_SCALE_FACTOR,
                             (q15_t*)bias,
                             (q15_t*)output,
                             (q15_t*)bufferA);
#ifdef ACTIVATION_RELU
#ifdef WITH_CMSIS_NN
  arm_relu_q15((q15_t*)output, FC_UNITS);
#elif defined(WITH_NMSIS_NN)
  riscv_relu_q15((q15_t*)output, FC_UNITS);
#endif
#elif !defined(ACTIVATION_LINEAR)
#error "Unsupported activation with CMSIS-NN"
#endif


#endif
}

#undef INPUT_SAMPLES
#undef FC_UNITS
#undef ACTIVATION_RELU
#undef WEIGHTS_SCALE_FACTOR
#undef BIASES_SCALE_FACTOR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
#undef NUMBER_T
#undef LONG_NUMBER_T
/**
  ******************************************************************************
  * @file    weights/fc.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 1.0.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#include <stdint.h>

#define INPUT_SAMPLES 800
#define FC_UNITS 128


const int16_t dense_2_bias[FC_UNITS] = {2, 9, -10, 7, -5, 9, -2, 23, 47, -7, 21, 11, -6, -6, 3, -7, 10, -5, 6, 69, 20, 41, 22, -1, -6, 25, 18, -12, -9, 18, -1, -10, -5, -9, 2, 25, 5, 0, -2, -5, 1, 3, -3, -3, -4, -5, -8, -12, 26, -5, -5, 8, -7, -6, -1, 0, 21, -8, -2, -7, 13, 10, -3, -2, -1, 30, -5, 30, 14, -4, -4, 24, 6, 1, -4, 25, -3, -3, -5, 11, -10, 25, -5, -2, 16, 6, 7, -3, -7, 24, 12, 14, -2, 60, 2, 0, -10, 18, 8, 4, 15, -2, -4, -6, -11, 8, 6, 0, -8, 21, -9, 18, -8, 21, -3, 25, 23, -4, -6, -4, 6, 14, -6, 18, -4, 15, -2, -11}
;

const int16_t dense_2_kernel[FC_UNITS][INPUT_SAMPLES] = {{0, 13, 16, -7, 23, 27, 13, -5, -13, -6, -14, -35, -6, 13, 15, 8, -23, 22, -1, -36, 6, -13, 16, 40, -2, 4, -5, 23, 24, -27, 26, -16, -37, 27, 16, 37, -20, 23, 3, 0, 1, -28, -1, -23, -5, 28, -24, -18, -1, 22, -7, -59, -3, 1, -6, 21, 2, 7, 20, 11, -31, -24, 15, -6, -29, 8, 12, 2, -35, -6, -17, -4, 15, 21, 3, 30, -11, 17, -22, 12, 16, 3, 17, -33, 6, -6, -10, 20, -5, -13, 5, -16, -45, -25, 3, 14, -13, -5, -11, 17, -2, 0, 0, 21, 11, -21, 6, -18, 6, -14, -8, 6, 27, -18, -12, 9, -7, 26, -4, -23, 21, -4, -14, 13, 5, 12, -25, 21, 29, -3, -9, -27, -15, 0, -6, -2, -8, 26, 21, -25, -9, 1, 25, -17, -29, -13, -19, 16, 11, 37, 8, -16, -4, 8, -1, 1, 27, 14, -1, -1, 10, -16, -10, -30, -6, -1, 2, -38, 2, -11, -13, -20, 6, 15, -25, -4, -23, -1, -24, -22, -3, -29, -4, 16, 29, 4, -19, -6, -3, 3, 12, -29, -2, -14, -12, 8, -13, -28, -11, 16, 9, 6, 3, -6, -12, -9, -11, -7, 15, 6, -13, -9, -6, -18, -10, -1, 1, -8, -11, -5, 13, -13, -10, 15, 4, -8, 23, -5, 8, 18, 4, 30, 15, 26, 15, 6, -3, -5, 25, 3, 30, 2, 5, 21, -2, 5, -5, 9, -30, 5, 12, 4, 42, 11, -18, 31, 11, 8, -5, 15, 15, 19, 6, 3, -18, -1, -4, 4, 2, -16, 23, -2, -5, 10, 14, -15, -5, 19, -4, 10, 6, -8, 17, 15, 49, 18, 14, 11, 28, -8, -10, -1, 22, 19, -22, -5, -2, -16, -20, 0, -1, -14, -14, 10, -10, -22, -28, 2, 9, -6, -1, -6, 5, 1, -9, 14, 11, 24, 0, -23, -3, -2, 10, -9, 4, -11, 4, -33, -32, -13, -22, -33, 3, -18, 0, -22, 4, -4, -13, 1, 5, 7, 9, -26, 0, -1, -15, 2, -15, -1, -7, -12, -2, 23, -6, 3, 13, -4, 23, -16, -28, 19, 3, 13, -4, -20, -15, -9, -16, -16, -2, 18, -4, 8, -23, -3, 0, -6, -5, 8, -7, -7, 1, -40, 18, 14, -12, -3, 25, 3, 4, 3, 1, 13, 25, 0, 2, -10, -11, 27, -25, -7, -15, -2, 5, 10, -18, -21, 0, -5, 9, 14, -17, -10, -8, -26, 22, 19, 0, 4, -11, -4, 16, -36, -2, -15, 0, -11, 4, -19, 27, 9, -15, -20, 17, 5, -7, 4, 9, 6, 20, -7, -16, 2, 16, 40, -2, -30, -14, -32, -15, 2, 12, -14, -30, -10, 1, -24, -7, -11, -7, -7, 11, -16, -4, -21, -31, 21, 4, -4, 11, -8, -1, 3, -13, -13, -13, 16, -9, -18, 13, -3, -10, 3, -11, -6, -2, 3, -6, -6, -33, -5, 7, 9, -10, -15, 16, 4, -1, 18, -2, -2, 5, -7, 10, 4, 4, -15, 15, 9, 2, -2, 9, -18, -23, 21, -37, -9, -33, -7, -33, -23, -33, -10, -3, -19, 3, -27, 1, -1, -18, -9, 0, -9, 19, 21, -4, -2, -16, -12, 1, 2, -3, 7, -10, 7, 8, 4, 8, -4, -18, -27, -11, -2, -1, -43, -5, -1, -7, -29, -8, 8, -2, -6, -7, -2, 15, 7, 1, -2, 4, -10, 11, 0, -2, -15, 12, -22, -27, -1, 14, -19, -4, -8, 2, 11, 24, -19, 4, 17, 4, -3, -1, -31, 2, 1, -5, 11, -13, -19, 2, 2, 10, 3, -1, 5, -34, -21, 5, -8, 22, -5, -41, -16, 14, 7, 12, 5, -5, 9, 5, -18, 7, -16, 12, -4, -2, 1, -6, 1, 2, 10, -3, 5, 6, -18, -5, 6, -3, 5, -28, -7, -14, 0, 10, 29, 21, -2, 16, -15, 11, -18, -4, 11, 12, 0, -16, -4, 16, -3, -1, 19, -9, 1, 13, -5, 2, 6, -3, -23, -12, -21, -25, 2, 8, -9, -27, -9, 1, 2, 10, 4, 5, -13, -15, 16, 18, -9, -4, 2, 0, -29, 0, 12, 16, -7, 15, 4, 0, -15, 15, -7, 7, 26, 8, -28, 18, -4, 7, -6, -13, -8, -4, 4, -8, -20, -15, 4, 19, 23, 21, -2, -10, -20, 0, 0, 10, 14, -1, 4, -20, 1, 0, 5, 6, 21, 27, 4, 19, 4, 33, 1, 0, -26, -20, 12, -11, 15, 1, 3, -11, 22, 11, -7, 6, -20, -5, 6, -21, -7, 7, -9, 4, 23, 21, 11, -29, -16, 17, 7, 18, -2, 22, 14, 23, 8, 5, 19, 9, 46, -8, 0, -2, 20, -22, 26, 27, -12, 6, 20, -4, -31, 20, -11, 29, 4, 20, 8, 8, -13}
, {7, 11, -9, -7, -10, 6, 1, 1, 12, 3, 0, 10, -6, 15, 13, -2, -1, 8, 1, 18, 8, 7, -1, -1, 8, -3, -2, 12, -14, 1, 0, 10, 26, -12, 2, 6, -7, -10, 2, -1, -6, 2, 7, -2, -8, -8, -6, 19, 7, -9, -3, 9, 1, 20, -9, -1, 22, -7, -6, 2, -11, 13, -12, 9, 9, -16, 2, -7, -16, -18, -3, -11, -13, -5, 4, -4, -3, -7, -10, -1, 5, -30, -4, 2, -9, 7, 1, -21, 29, -3, -3, -19, 15, -1, -23, -8, 23, 21, -9, -23, -22, 0, 10, -15, -20, -5, -16, 19, -9, -7, -19, -27, -11, 8, -3, -11, -5, -9, -10, -5, 4, -8, 4, -16, -12, -8, 6, -35, 13, 7, 8, -12, 8, -1, 32, -8, -15, 11, -14, -12, -12, -9, -8, 7, -6, 6, -2, -3, 1, 7, 12, 3, 6, 2, -6, 13, 1, 7, 4, -6, -4, 10, -12, -8, 4, 5, -2, 13, 13, 5, -6, -18, -7, -27, 0, 6, 0, 4, 4, -1, 2, 9, 2, -31, -1, 5, -4, -13, 20, 0, -8, 16, 15, -19, -10, 0, -8, -8, 6, 12, 16, 19, 7, 15, 2, -18, 7, -7, -8, -17, 8, 11, 2, 10, -13, -18, -1, 4, -5, -4, 30, 9, -27, -6, 1, -7, -19, -3, -5, 2, -10, -7, -1, -7, 7, 1, -13, 12, 9, -9, 0, -1, 11, -8, 4, 6, 2, 5, 2, 6, -7, -1, 2, 21, -1, -2, 8, 7, -7, -12, -5, 2, -15, -20, -11, -14, -26, -14, -4, -11, 4, -8, -9, 6, -11, 5, -8, 1, -5, 10, -9, -13, -1, -1, 18, 7, -1, -14, -18, 33, 1, -26, 6, 0, 25, -7, -9, -8, -13, -26, 2, 4, -18, 12, -15, 10, -4, -15, -6, -3, 20, 17, 4, -5, 11, 10, 2, -11, -5, -13, 19, -7, 1, -2, 10, -7, -10, -18, -2, -1, -14, -12, 2, 1, 3, 12, 17, 11, -1, -5, -3, -7, 9, -12, 13, -7, -15, 18, 1, -3, 4, -27, 20, -1, 5, -7, 23, 0, 7, -13, -2, -2, -1, -1, -7, 20, -5, 7, -26, 8, 32, -14, 7, -18, 14, 6, 1, -17, 9, 19, -5, 0, 10, -19, 7, -18, -1, 11, 22, 12, 6, -1, 1, -8, 17, 9, -14, 15, -7, 16, -7, 16, 4, -1, 11, -10, -2, 4, -11, 3, 15, 39, -13, 6, 21, -1, 16, -20, 16, 9, 10, -32, -12, 4, 7, -8, 3, 7, 3, 4, 31, 9, 9, -10, -15, 16, 9, 9, 15, 12, 1, -11, -4, 23, 20, 16, 1, 10, -9, -29, -16, -10, 11, -25, -25, 6, 1, -16, 6, -23, 4, -3, 24, 14, 1, -27, -32, -9, -6, -2, -18, 0, 15, 0, -16, -9, 1, 33, 1, -2, 8, -12, 3, 1, 0, 14, 22, -10, -3, 8, 23, 3, -7, -8, -8, -13, -26, 9, 18, -20, -4, 32, 17, -6, -24, 3, 2, -8, 0, -23, 5, -20, -9, -9, 9, 13, -23, 2, -10, 14, 6, -13, -1, 3, -18, 24, 21, 4, 18, 13, 19, -24, 2, -6, 34, 21, -7, -10, 10, 13, -1, -17, 5, 42, -11, 3, -2, 11, 5, -18, -2, -10, 2, 4, 3, 17, -2, 12, 4, -6, -11, 7, 1, -14, 8, 6, 10, 6, -6, -14, -3, -2, -14, -10, 3, 10, -7, 1, -16, -4, -12, -17, 6, 5, 20, 4, 8, 26, -16, -13, -7, -10, -8, -22, -16, 17, -5, 7, -15, 2, 1, 1, 5, -24, -1, -18, -24, 1, 2, -13, 22, 5, -4, -1, 1, 7, 16, 3, -26, 11, -9, -11, 6, -25, -11, 5, -23, 1, -10, 15, -4, 3, 2, -12, 0, -20, 0, 3, 9, 4, -1, 1, -6, -16, 4, -7, 15, -18, -25, 18, 25, -13, 11, -19, -44, 7, -33, -16, -17, -12, 9, 14, -20, -21, 12, -3, 8, 2, -2, -24, -16, -28, -4, 7, 17, -11, 29, 1, 1, 19, 27, 26, 12, 7, -13, 20, -2, 22, -6, -11, 15, 0, -11, 3, -14, -20, 30, -2, 29, 17, 17, -18, -13, -9, 10, -6, 1, 10, 9, 4, -17, 4, -4, 22, -5, -1, 1, 11, -21, -3, 9, -11, 1, -2, -4, -5, -10, -27, 0, -5, 7, 2, 21, -8, -6, 12, 14, -2, -9, 8, -15, 5, 0, 8, 2, 5, -5, -30, -4, 8, -17, -8, 18, 3, 9, -8, -6, -11, -8, -13, 4, -11, 16, 1, 9, 17, -6, -7, 11, 25, 21, -4, 11, 16, 11, -5, -18, -33, 0, -27, 2, -8, -23, 5, 9, 20, 7, -19, -8, 34, 20, 4, -5, -5, 14, 22, 32, 3, 13, 7}
, {17, -19, -28, 8, -2, -28, -22, -10, -17, -5, -15, -25, 2, -17, 6, -22, 0, 10, 10, -18, 10, -1, -24, -13, 10, -7, -5, -30, 5, 1, -16, -31, 34, -25, -20, 3, 14, -14, -38, 17, 4, -7, -17, -1, -5, -6, -10, -23, 2, 0, 3, -9, 5, -22, -23, -17, 12, -10, -16, -38, -16, 5, -3, 19, 6, -11, -1, 29, 7, -12, 6, 14, -10, 5, 5, 29, 2, -9, -21, -7, 9, 6, -14, -7, 5, 6, 11, -7, -42, -7, -1, -33, -23, 15, 11, 29, -11, 12, 10, -13, 29, 9, 8, 15, 25, 28, 16, 24, 12, 13, 2, -4, 8, 3, 4, -3, 10, -11, 6, 22, -60, -1, 16, 20, -12, -7, 18, -4, -24, -10, -4, 9, 19, -2, 1, 25, 15, 23, 13, 9, -6, 2, 13, -14, 14, -2, -5, 8, 0, 3, -9, 4, 0, -15, -17, 14, -11, -18, 15, 28, -2, -23, 2, -19, -24, -15, 2, -6, -15, -15, -15, -3, -7, 6, -18, -3, -12, -13, 15, 4, 7, 10, 6, 17, -18, -10, 2, -6, 0, -9, -5, -19, -13, -10, -8, -13, -3, 7, 0, 37, 17, -5, -2, 7, 6, 10, 21, -2, 5, -2, -14, 0, 9, 1, 21, 15, -5, 6, 2, 10, 10, -9, 15, 15, 20, 7, -17, -10, -11, 8, 12, -4, -1, 8, 12, 11, 10, 11, 27, 17, 1, 7, -1, 7, -1, 17, 17, 16, 2, 7, 9, 15, 12, -16, 2, -9, 15, 14, 2, -13, -1, -3, -7, 9, -6, 12, 12, 11, -11, -30, 9, 14, 5, -22, -12, 24, 6, -5, -2, -4, 30, 0, 9, -2, 13, -10, -12, 8, 19, -18, 0, 7, -3, -20, -21, 24, 10, 19, -6, -4, 0, -10, 19, 1, 4, -12, 4, 2, 3, 3, 1, -15, -24, -17, -24, -7, 3, -2, -14, 0, -10, 30, 19, -13, -2, 12, 17, 0, -1, -14, 5, -4, -3, 9, 7, -5, -8, 8, 31, 1, 10, 35, 30, 11, -5, 3, 22, 1, 9, -14, 9, 31, 6, 30, 17, -5, 18, 18, 19, 14, 5, 0, 34, -11, -10, 25, -6, 12, 0, 8, 17, 20, 7, 13, -1, 2, -2, 4, 23, 1, 6, 2, 6, 4, -4, -11, -11, 17, -11, 21, -31, 12, 1, 17, 5, 5, -2, 2, -22, 11, 7, -11, -11, -6, 2, -8, -15, -3, 6, -1, 8, -7, 0, 7, 0, 14, -3, -5, -2, -20, 2, 1, 25, -3, 0, 9, 14, 9, 7, -3, -17, 14, -1, 24, 12, -6, -7, 1, 8, 0, 9, -7, 22, -36, 17, 18, 15, 2, 28, 25, 15, 17, -16, 5, 11, -6, -1, 16, 15, -29, -7, -2, -7, -5, 20, 46, 26, 26, 0, 5, 34, -18, 9, -14, 4, -7, -16, 13, 10, 21, 5, 6, 3, -1, 32, 11, -6, -3, -21, 6, 6, -19, -11, 8, -29, 50, 14, 4, -4, 11, -10, -9, -6, 0, 10, 1, 20, 34, -3, 6, 17, -15, 13, -24, -11, -20, 1, 3, -34, -23, -13, -9, -12, -6, -3, -22, -6, 8, -25, -8, -19, 18, -7, -21, -13, -16, 15, 4, -16, -7, -24, 9, 3, -50, -17, -4, 6, -6, 8, 19, -19, 9, -7, -18, -13, -11, 7, 15, -15, -2, -5, 31, 3, -52, -1, -8, -2, 23, -4, -11, -8, -2, -3, -12, 32, 0, -26, 0, 11, 6, 22, 29, 10, -7, -13, -5, 14, -4, -1, -2, -26, 14, -14, -1, 11, -11, -2, -1, -9, 23, 17, 2, -7, 29, -21, -9, 5, -13, -18, -13, -29, 8, 8, -10, 25, -9, -2, -4, 24, 2, -3, 8, -8, 35, -11, -21, 0, 26, -3, 8, -15, 20, 4, 4, -16, 3, -13, -13, 4, -15, -2, 7, -6, 6, -8, 1, -8, 3, 18, -37, -18, 0, -8, 6, 15, 11, 21, 12, 15, 6, 1, 14, 14, 6, -17, 5, -15, -4, -15, 37, 23, 4, -10, 12, 8, -15, -4, -4, 23, -24, -18, -17, -1, -16, 17, -18, -7, -7, -5, -7, 3, 4, 9, 19, -3, -6, -21, -10, 4, 1, 23, 3, -8, -32, -6, 8, -9, 7, 4, -10, -16, -5, 15, -21, 12, 2, -3, 16, 19, 0, 11, 4, -3, -17, 6, 21, -2, 12, -3, -5, 4, -6, 38, 17, 4, -18, 33, -13, 7, -9, -5, -8, 4, -2, -14, 10, -4, -3, 5, 8, 0, -14, 0, -45, -18, -13, 5, 16, -12, -7, -21, -11, -14, 3, 12, 44, -21, 12, 3, -4, -24, -16, 19, -28, 7, -7, 1, 0, -5, 39, 1, -13, -12, 8, -13, -1, -22, 3, -4, 19, -3, 31, 4, 2, 6, -4, -18, 27, 16, -26}
, {-5, 31, 19, -16, -1, 3, 3, -30, -40, 12, -14, -55, -6, -16, 4, -29, -14, 16, -17, -16, 3, -3, 23, -18, -12, 5, -9, 3, 19, -3, -1, -20, 3, 23, 17, -2, 0, 16, -21, 6, -1, -5, 8, 1, -2, 25, 11, 13, -6, 5, 29, 7, 7, -18, 10, 4, 3, 11, 10, -2, 12, -7, 3, -9, 6, -7, 9, -3, -7, -16, -25, 5, -7, 28, 22, 10, -11, 16, -14, 28, 9, -17, -9, -17, 7, -11, -18, 20, 31, 1, 11, -13, 10, -2, -4, 3, 23, -28, -5, -7, -7, -7, 14, 8, 0, -10, -7, -14, -8, 6, -3, 10, 19, -10, -5, 2, 10, -8, -4, -22, 25, -2, -13, 6, 14, 3, -5, 12, 4, -22, -8, -11, 7, -20, -7, 13, 4, 0, 6, -12, 1, -19, 23, -10, -20, -11, -10, 23, 3, -2, 2, -6, -12, -3, -11, -16, -9, -3, -1, 0, 11, -10, -13, 2, -4, 3, 10, -38, -14, 15, -15, -36, -7, -4, -8, -12, -20, 11, 1, -22, -4, -10, 14, -19, 11, -6, -17, 8, -9, 34, 15, -16, -8, -3, -3, -4, -4, -12, -2, 1, -3, 14, 4, 1, -7, -7, -27, -6, 19, 27, 3, -5, 2, 6, 4, -3, 7, 3, 1, 6, -10, 7, -3, 23, -1, -2, 9, -1, 1, 9, 6, 9, 10, 14, 7, 1, -3, 20, 4, 5, 1, 8, 22, -8, -7, 7, 2, 5, -9, 7, 11, 13, 20, -14, 1, 7, 0, 8, -4, 4, -6, 8, 4, -1, 12, -7, -1, 13, -2, -4, 7, -11, 8, -7, -9, 8, -1, -9, -11, -4, 11, -1, 11, 15, 19, 10, -12, 8, 7, -24, -6, -9, 2, -8, -13, 20, 27, 2, -7, 10, -17, -1, 19, 2, 11, -8, -19, 12, 4, 5, 0, -9, -19, 8, -28, 0, 4, 36, 2, 6, -15, -7, -11, -18, 1, 5, 20, -3, 7, 1, -17, -12, -3, -19, 1, -21, -2, 9, -13, -7, -1, -1, 24, -20, -18, 4, -13, -8, -2, -7, 0, -20, -9, 29, 11, 6, 2, 11, 43, -15, -1, -3, 7, -5, 7, 7, -12, -3, 2, 2, 26, 4, -5, 18, 13, -8, 2, 9, -2, 23, -3, 0, -15, -11, -3, -13, -12, -15, 8, -12, -15, -1, -15, 0, -5, 3, -2, 14, -21, 4, -22, -6, -28, 3, -2, -3, -26, -7, 15, 4, -19, 4, -6, 1, -21, -22, 0, -3, 6, -17, -21, -5, 6, -6, -9, 2, 0, -9, -3, -6, 8, 10, -26, 7, 19, 3, 4, -22, 5, -23, 8, 6, -30, -26, 7, 6, -6, -25, -22, 1, 22, 3, -4, -1, 1, -2, 11, -4, 2, -2, 5, 22, -6, -12, -11, 24, 12, -7, -10, 7, 5, 22, -13, 11, 4, -5, 10, -21, 23, -10, -3, 19, 23, -6, 12, 8, 24, 13, -1, 12, -19, -2, -17, 18, 18, 2, -15, 26, 8, 12, -9, 5, 10, 13, 1, 9, 4, 31, 12, 0, 5, -16, -3, -18, -5, 2, 7, -5, -15, -12, -20, 5, 5, -27, -3, -12, 9, -9, 3, -14, 15, -15, -9, 0, -2, -4, 22, 11, -3, 14, 2, 7, -13, -4, 4, -6, -1, 0, 0, -8, -10, -31, -8, -28, -27, -19, 3, -7, 12, -32, -14, 6, -10, -11, -9, -23, 10, 3, -10, 8, -5, -5, 4, 1, -14, 10, -18, -5, 19, -2, 5, -8, 6, -7, 5, -4, -7, -12, -8, 28, -12, 3, -3, -45, -11, -11, 5, -23, -2, -8, 13, -3, 9, 15, -2, 10, -23, -7, -8, -6, -10, 26, -10, -10, -5, -18, -13, 1, 6, -11, 5, 12, -16, 18, -3, -10, -4, 2, -9, -3, 6, 5, 34, 0, -4, 5, -10, 9, 4, -3, -9, 12, -3, -22, 8, 32, 20, -3, -17, 0, 7, 8, -19, -16, -19, 7, -40, -12, -9, 1, -8, 3, -22, -15, 14, 10, -10, 6, -9, -36, -4, -31, -30, -3, 22, 6, -11, 2, 6, -7, -3, 31, 10, -28, 6, -9, 10, 3, -10, 8, 28, -44, -4, 17, 2, -29, -14, 1, 18, -14, 16, -6, -30, 8, 2, -23, 21, -1, -21, -19, -11, -29, -8, -11, 0, -21, -15, 7, -16, 20, 20, 0, 7, 13, -6, 1, 4, -27, 21, 1, 6, -7, 28, 0, -13, -1, 16, -17, 27, -7, -6, -9, 14, -11, -1, -15, 13, 0, 4, 17, -8, 20, -18, -33, -7, 0, -2, 3, 24, -27, 15, 10, 6, -2, 8, 9, -3, -26, -1, -4, 26, 12, 18, -10, 26, 1, 13, 31, 14, 12, -5, 22, 19, 11, 3, -1, 5, 0, -5, 2, 30, 7, 30, -8, -3, 11, 16, 24, 11, 2}
, {0, 2, 19, 8, -9, 13, 9, -7, -1, 36, 4, 29, -3, 11, -22, 24, -15, -9, 33, -4, 2, -3, -19, -33, -2, -5, -8, 30, 7, 2, -18, -24, 19, -17, 1, -12, 12, -23, -2, 9, 2, -8, 20, 1, -9, -13, 5, 15, 11, -14, -2, 22, -8, 3, 5, -6, 45, -1, 6, -15, -18, 6, -27, 17, -4, -2, 10, -1, -21, 2, 12, -16, -20, -48, -1, -39, -13, -1, 20, -12, 1, 24, 0, 26, -1, 24, 29, -11, 0, 6, -23, 7, 2, 13, 7, -7, -24, 18, 16, -6, 6, 25, 18, -20, 0, -12, -17, -19, 0, 47, 3, 4, -17, -4, -20, -2, -3, 5, 6, 13, 8, -7, 1, 22, -11, 6, 0, -15, -16, 10, 22, 17, 22, 43, -11, -6, 0, 3, -13, -4, -3, 28, -25, 53, 7, 25, 11, -27, -6, -19, 8, 6, 28, -12, 3, 40, -3, -25, 4, 7, -3, -5, -2, 1, -1, 5, -15, 5, -8, -1, 3, -1, 3, 7, 6, -9, -12, -1, 6, 0, -1, -13, 1, 15, -11, -11, -3, -10, -16, 3, 3, -27, 9, -2, 9, 12, -6, -9, -4, 0, 3, -7, 6, -3, -1, 16, -2, 7, -15, 23, -2, -21, -9, -9, 20, -6, 2, 9, -10, 9, -21, 3, -1, -27, -13, -8, 11, -1, 5, -6, 9, -9, -7, -5, 2, 28, -1, 15, -7, 10, 15, -27, -4, -6, -2, 8, -29, -7, 25, -5, -5, 1, 4, -13, -15, -19, -2, 3, 8, 18, 7, 8, 12, -5, 18, 11, 10, -35, 11, -24, -3, 6, 21, -16, -14, -1, -2, -1, -25, -31, -12, 8, 18, 10, -6, 3, -39, 18, -1, 13, -11, 3, 7, -7, 16, -7, 3, -46, -9, -25, 7, -17, 27, -6, 10, -21, -14, -23, -4, -1, 3, -29, -4, -2, -7, -1, -9, 14, -30, 11, 8, 0, 0, 11, -32, -14, -23, 24, 12, -19, -7, 9, 6, 20, 1, -3, 11, 19, 19, 4, -3, -8, 16, 32, 10, -11, 14, -26, -9, -7, 17, 16, -3, -7, 16, 5, -16, 29, 18, -6, -10, -8, -12, -15, -12, -1, -48, -10, -3, -7, 2, -7, -5, -5, -20, 1, -10, 3, 1, -3, -11, -5, 2, 12, -14, -8, -1, -1, 5, 5, -4, -14, -5, 13, 20, -2, 4, 2, -14, 8, 1, -31, 9, -31, 4, 11, -25, -18, 5, -11, -4, -10, 0, -4, -21, -12, 11, -42, -32, -6, 1, -34, -30, 9, 27, 7, 31, -9, 3, -18, 29, -2, -1, -16, -19, 15, -9, 11, 10, -21, 11, -1, -26, -21, 21, 3, 9, 11, 31, -14, -15, -9, -8, -16, -21, 19, 15, 0, 37, 18, -7, -5, 33, 3, 1, -21, -1, 7, 1, -17, 20, 0, 17, -12, -1, -21, 0, 29, -1, 31, -5, -3, -10, 6, 3, 14, -10, 5, 15, 4, 1, -4, -3, 6, -10, -2, 26, -27, -13, 12, 5, -5, -16, 0, -1, -8, 27, 20, 14, 1, -9, 19, 3, -9, 0, 7, 7, -17, -25, -3, 1, 10, -9, 1, -8, -25, 12, -19, 20, -27, -19, 15, -6, -21, -6, -35, -8, -1, -4, -13, 16, -4, -18, 15, 14, -7, -31, 17, -8, -23, 19, -5, 1, 9, -11, 5, -4, -19, -2, -13, 2, 13, -14, 14, -4, 3, 1, -17, -10, -3, 12, -9, 0, 20, -4, -2, 19, 24, 12, -17, 11, 11, 21, 15, 18, 31, 24, 53, 5, 5, -2, -6, -25, 22, 17, -3, 0, -4, -1, 8, -11, 2, 8, -7, 7, -7, 20, -3, -19, 31, -9, 17, 5, 3, 15, 1, 11, 17, 19, 27, -10, 11, 0, 15, 3, 3, 20, 22, 8, 16, -9, 7, 0, -5, 21, 14, -8, -21, 8, 4, -1, -1, 14, 1, -1, -18, -13, 23, 13, -4, -12, -1, -4, 20, 21, 16, 44, 3, 25, 22, 8, -4, 33, 14, -15, 3, 7, -18, -7, 28, 13, 16, 22, -2, 2, -14, -1, -6, 10, 0, -8, -4, -5, 9, 2, 9, -7, 11, 5, -2, 6, 24, 6, -1, 5, 20, 16, -1, -12, -4, -18, 8, 5, -14, -1, 7, -27, 23, 13, -4, 13, -5, -1, 8, -14, -11, 12, 12, 12, -13, -1, -26, -21, 11, -3, -8, -10, 21, -9, 0, 22, 10, 4, 10, -2, -15, -1, 24, 7, 1, 11, 22, 15, -3, -8, -10, -16, -17, 11, -7, -5, 18, 9, 22, 18, 9, -3, 9, 3, 26, -1, -7, 7, 14, 11, 5, 12, 2, -9, 22, -26, 2, 5, 7, 25, -14, -26, -26, -12, -44, 6, -4, -8, -8, -13, -21, -30, -15, -1, 26, -16, 0, -16, -1, 12, 23, 25, -7, -28, -14}
, {-8, -24, 1, 9, -32, -5, 1, -5, -16, -15, -10, -27, 5, -41, 0, -36, 5, -3, 3, 14, 8, -4, 7, -10, 4, 2, -31, -35, 20, 12, 2, -4, 10, -21, -17, -5, -51, 2, -11, -20, -5, 1, 0, -8, -6, -7, 45, -33, -38, 22, 8, 19, -4, 3, 26, -14, -22, -4, -24, -13, 31, 4, -2, -29, 2, 10, 14, 2, 0, 15, -25, 18, 3, -6, -29, -15, 7, 12, -2, 2, -2, -1, 0, 12, -9, 2, -18, 14, 7, 2, 0, 7, 11, -10, 13, 8, 14, -25, 2, 12, -33, -24, -23, 4, -5, 7, -6, -5, -1, -29, -11, 5, 16, 6, 1, -14, -9, 9, -19, -4, -2, 5, -22, -9, 4, -11, 8, 11, 21, -10, -6, -9, -17, -20, -11, 3, -2, 13, 17, 4, 9, 3, 3, -24, 5, -10, 9, 14, -8, 5, -2, -16, -15, 8, 17, -35, 16, 30, -7, -1, -5, 25, 27, 10, -1, 22, 19, 32, 8, -9, 1, 14, 8, 24, -20, -2, 12, 1, 12, 7, 6, 8, -6, 45, 10, 2, 6, 4, 8, 1, 27, 39, -12, 16, 8, 16, 19, 12, 19, -2, -7, 8, -1, -10, -12, -2, -40, -1, 6, 5, 14, -4, -3, 13, -24, -16, -5, 0, 11, 22, -3, 6, 0, 0, -18, 2, 9, 2, 20, 0, -16, 8, 2, -1, -10, -22, -1, -10, -20, -11, -3, -12, -1, -5, 3, -4, -8, -23, -7, -1, 14, -1, 2, -10, -21, 25, -10, -11, 12, -12, -24, -15, 5, 8, 7, -3, -15, 6, 4, 15, -18, -5, -12, 22, -4, -4, -6, 7, 2, 4, -9, 5, 13, 0, -3, -6, 17, -12, 11, 4, -4, -7, 12, 9, -4, 2, 1, 25, 19, 11, 1, 16, -5, 14, 11, 8, 9, -2, -10, 9, -16, 11, 25, -10, 7, -2, 13, -6, 6, -4, -4, -1, -12, 4, 4, -4, -20, -3, -14, 1, 1, 1, -6, 0, -8, 5, 25, -18, -21, 20, -6, -8, -17, 5, 5, -3, -3, 18, 2, -9, -23, 10, -3, -15, 6, -12, -13, -12, -10, -7, -8, -2, -26, -5, -5, -15, 23, 5, -6, 14, -11, 1, -7, 5, 5, 4, 7, 12, -29, -24, 18, 5, 11, -23, 4, -13, 4, -6, 0, -34, -3, -22, -13, -14, -14, -3, -6, -19, 7, -22, -1, 7, -1, 16, -7, -5, 22, 5, 2, -6, -15, -6, 18, 31, 1, -21, 5, 17, 20, -4, 3, 8, 45, 17, -7, 0, -6, -1, 2, 6, -12, 2, 13, -2, 8, -11, -6, 21, 5, 9, 4, -16, 13, 18, 1, 10, -10, 0, -2, 2, 1, 1, 29, 11, 46, -17, -2, 22, 9, -37, -9, -1, 12, -2, 14, -21, 11, -5, 10, 27, 21, 8, -10, 3, 17, 23, 16, 27, -26, -15, -2, -2, -13, 9, -23, 16, 4, -9, 6, -3, -14, -9, 6, -17, 14, -26, 11, 21, -7, -1, -2, 2, 13, 41, -4, -4, 7, -30, 18, -9, 4, 7, 3, 21, 4, 20, 2, 23, 32, 5, 19, -2, 7, 4, -8, 1, 19, 0, 3, 33, 6, 28, 9, 22, 31, 10, 22, 8, 14, 11, 4, 5, 7, -8, 18, 23, 11, -8, 7, -4, 18, 17, 9, -12, 7, 6, 2, 0, 13, 8, 9, -2, -15, 33, -3, 18, 18, 0, 22, -3, 16, 0, -12, 7, 1, 2, 17, -4, 4, 1, 20, 5, 27, -6, -22, -12, 1, -14, 3, 4, 11, 14, 3, -19, 1, 8, -5, -4, 11, -14, 13, 5, -23, 22, -10, 15, -8, 0, 11, -5, -2, -11, 28, 8, 29, -17, 0, 0, 44, 17, 8, 2, 16, 34, -15, -3, -2, 28, -4, 29, 11, 4, 8, 3, -5, 20, 7, 12, 6, -18, -10, 0, -2, -9, 9, 8, -1, 8, 16, 7, -12, 29, 4, 0, 22, -9, -1, -9, -4, 6, 8, -2, -5, 29, -9, -3, -4, -6, -9, -24, 7, 21, -8, -11, 11, -2, 20, 3, 0, -31, 6, 44, 14, 12, 6, 5, -2, 7, -2, 0, 8, -17, 7, -4, 2, 0, -25, 6, -4, -5, 2, -16, -13, -6, 5, -9, 12, 3, -8, 10, -16, -26, 6, 2, 10, -8, -5, 1, -1, 14, -34, -2, -17, 23, -2, -7, -23, -22, 23, 9, -9, 5, -7, -10, -8, -25, -4, -33, 6, -5, -16, -3, -11, 16, -2, -4, 13, -4, -2, 26, 7, -1, -4, -10, -15, -1, -9, -36, 16, -8, 19, 3, -20, -7, -16, -3, 2, 11, 0, 1, -4, 11, 6, -21, -27, 20, 3, 18, 20, 30, 6, 7, -1, 4, 19, 13, 37, 1, 3, -8, 28, 2, 27, 7, -13, 4, 12, 35, 18, 5}
, {-1, 6, -10, 2, 3, 32, 3, 17, -8, 11, 1, 2, 3, 9, -5, -3, 0, -4, 1, -11, -8, -7, 9, 5, -9, -12, 14, -4, 5, -9, 14, -4, 2, 18, 25, -3, -9, 9, 6, 4, -15, -6, 16, 1, 1, 19, -2, 20, -6, 14, 6, -5, -4, -19, 16, 24, -12, 4, 3, -18, 10, -5, 17, 7, -1, 13, 8, -19, -36, 7, 13, -16, 6, 1, 14, -18, -15, 11, 24, -1, -14, 8, 20, 14, -3, 6, 11, -3, 34, -7, 1, 14, -22, 2, -7, 4, 13, 9, -5, -3, 9, 18, 3, -3, -7, -18, 3, -23, 5, 4, -5, 8, -7, -3, 2, 13, 2, 1, -20, -5, 4, -4, 3, -1, -7, 16, -22, 3, -13, -8, -4, -14, -12, -6, -27, -3, 2, -7, -5, -7, -6, 0, -1, 17, 2, -37, 5, -12, 4, 5, -4, 5, 16, 3, 3, -17, 2, 7, -9, 3, -6, -9, -14, -5, -8, -17, 2, 10, 10, -4, -4, 7, -12, -6, 24, -6, -22, -1, -2, 20, 4, 14, 26, 0, 7, -14, 3, 9, 15, -1, 9, -25, -13, -16, 2, 1, -8, -11, -2, 17, 20, -1, 13, 9, -3, 3, 13, 1, 18, 5, 6, -1, 5, 1, 8, 40, -4, -7, 0, 1, -5, 0, -7, 37, -6, 2, 19, -4, -8, -3, -10, 7, 20, 4, -11, 7, -7, 15, 25, -1, 23, 2, 4, 8, -6, 10, -9, 15, -7, -7, -4, 8, -14, 9, 13, 26, 18, -6, 13, 21, -6, -9, -1, 9, 2, 6, -11, -11, -3, -1, -7, -12, -1, -1, -6, -4, -1, 6, 3, -19, -19, 0, -8, 3, 10, 13, -2, 6, -6, -5, -4, 4, 9, -3, 11, -9, -18, 6, -1, -3, -14, 9, -6, 1, -2, -23, -3, -17, -11, 6, -7, -23, 1, 4, 4, 14, 22, -14, -17, -7, -2, 12, -15, 7, 8, -17, 13, -2, 17, 15, 13, 4, -9, 20, -12, 9, -5, 13, 4, -16, 3, 7, 5, -7, -1, -13, 15, 2, -6, 6, 8, 5, -12, -6, -4, 15, 0, 1, 13, 10, 22, 26, 13, 15, -2, -8, -9, 1, -2, 24, 30, 17, 1, 7, -12, -14, -5, -12, 21, -11, -2, 1, 0, 0, -5, 14, -13, -10, 20, 1, -3, 11, 20, 6, 7, 20, -2, -4, -7, 13, -3, -12, 4, 34, -5, 5, -4, -5, -8, -12, 25, 4, 1, -6, -14, 13, 4, -14, -2, 12, 10, -10, -12, 0, 17, -15, -3, -19, -7, 0, 35, -2, 4, -1, -11, 11, 8, -9, 14, 9, 4, 2, -19, 8, 0, 4, 14, 0, 19, -6, -16, 7, -16, -22, -19, -12, 1, 16, 1, 26, -6, 4, 3, 4, -10, -12, -17, 4, 1, -15, -1, -15, 21, 5, -20, -10, 3, 22, 5, -26, 1, 11, 0, 0, -8, -16, 18, 19, 9, 19, 4, 14, -14, 27, 16, 18, 6, 3, 16, 13, -11, -6, -5, 44, -8, -9, 0, 6, 9, 9, 6, 20, 0, -6, 8, 22, 4, 17, 13, -4, -3, 17, 18, 7, 0, 2, -10, 8, 4, 9, 5, -17, 4, 1, -14, -3, 5, -7, -4, 14, -8, -2, -24, 20, -6, 12, -19, 18, 17, 0, -3, -11, -7, 8, -14, -10, -22, -19, 3, 1, -15, -9, -7, 27, 3, 14, -18, -37, -2, -1, 18, -3, 1, 2, -32, -7, -2, 8, -9, -9, -2, 1, 8, -16, -2, 5, 5, 23, -17, 20, 7, -9, -15, -14, 9, 12, -5, 12, -5, -12, -18, 6, 24, -11, 5, 6, -15, -10, -20, 18, 0, 10, -24, -15, -1, -15, -4, -2, -19, 21, -4, 7, 6, -15, -9, -8, -12, -4, 8, 9, -6, -1, -19, -11, 21, 0, 19, -21, 3, -15, -7, -24, -10, 2, -14, 11, 5, 29, 31, -3, 8, 16, 2, 7, 16, -9, 67, 26, 26, 9, -9, -5, 25, 16, -6, -8, 5, -19, 15, 42, 26, 19, 15, -42, 15, 6, -6, -6, -13, 31, 37, -15, -15, 7, 1, 15, 5, -1, 30, 5, -6, 5, 1, -45, 27, 31, 4, 6, -27, -4, -35, 22, 30, 33, -7, 3, 10, 2, 10, 2, 9, 30, 7, -17, -33, 20, -4, 8, 26, 1, 23, 18, -6, 2, 7, -15, 7, 13, -24, -8, -27, 11, -44, 13, 30, 27, 6, 4, -15, 7, 21, 1, 10, -27, -10, -8, 4, -15, -10, -18, -11, 5, -1, 10, 9, 2, 2, -3, -21, 9, 15, 2, -6, 16, 6, -11, -11, -16, -19, -4, 7, 8, 0, 13, -17, -13, 10, 20, 16, -9, -13, 1, 1, -4, 8, -6, 15, -21, 4, -18, -30, -36, -24, -14, 28, -19, 19, -14, -41, 14}
, {31, -22, -27, 2, -27, -22, -6, -8, 11, -6, -10, 13, 1, -15, -1, -29, 33, -5, 3, 8, -9, 26, -4, -64, -9, 2, 2, -22, 11, 37, -22, 33, 40, -25, 0, -22, -6, -26, 2, 14, -2, 10, -22, -14, 2, -12, 16, -16, 19, -9, -29, 10, 0, 7, 6, -6, 0, 5, -14, 0, 18, 11, -12, 10, 20, -7, 2, 6, 24, -16, 4, -3, -17, -12, -3, -33, -3, -19, 5, -24, -9, 7, -1, 19, -5, -18, 19, -50, -14, 9, -11, 26, 16, 13, 0, -2, -11, 14, 33, -8, 10, -3, -18, -3, -6, 15, -3, 35, -1, 9, -8, 18, -11, 13, 39, -13, 6, -34, 12, 0, -3, -2, 4, -4, -10, -13, 13, -13, -13, 19, 5, 3, -5, 35, 3, 6, -1, 12, -17, 14, 6, 16, -45, 34, 15, 15, 15, -16, 5, -4, -15, 25, 36, 14, 17, 1, -17, -20, 11, 7, -2, 25, 33, 20, -4, 23, 10, 3, -11, 1, 14, -11, 6, 7, 13, -2, 23, -7, 14, 10, 7, 21, -11, -4, -3, 9, 22, 18, 20, 5, -9, 15, -3, -1, 20, -6, 15, 7, -8, -4, -15, -16, -1, -14, 6, 7, -9, 23, -17, -17, -15, -8, 3, 23, 10, -8, 11, -2, 16, 9, -15, -12, 1, -39, -3, -14, -14, -7, 1, -4, 4, -14, -7, -9, -7, 14, -8, -7, -30, 13, -16, 10, -13, -14, -7, -8, 19, -5, 25, -1, -11, -4, -19, -18, 21, -24, -28, -4, 23, 9, -17, 11, 13, -19, -8, -7, -5, -7, -7, 43, -11, 10, -7, -13, 8, 10, 6, -14, 17, 12, 13, -7, 0, -10, -18, -18, -3, -27, -7, 12, 14, 14, -25, 4, -8, 4, -13, -12, -8, -29, -2, -2, -5, -4, 8, -37, -18, -9, -6, 0, 1, -5, -6, 4, -16, -10, -4, 19, -9, 10, 3, -5, 10, 17, -4, 7, -1, -2, -2, -6, 3, 14, 8, 28, 14, 18, 14, -1, 8, 34, 0, 16, 3, 12, 34, 8, 4, -11, 31, 9, 0, 9, 5, -17, -7, -14, -35, -18, -4, 5, -10, -9, -6, 1, -7, -6, 22, -1, 9, 8, -22, -9, -2, 7, 11, 7, 8, 9, -12, -33, 5, 1, -4, -7, -14, -10, -8, -9, -24, -2, -7, -14, -8, -9, -8, -5, 10, 20, -3, -6, 21, -7, -3, -6, -1, -10, -1, 16, 3, 13, -16, -14, -1, -1, 4, -2, -1, 5, -16, 3, 8, -10, -3, 13, 8, 23, 13, 9, 7, 5, -5, 2, 8, -10, -5, 3, 5, 16, -1, 11, -1, 12, 15, -2, 4, -2, -13, 11, 26, -4, 10, -3, -8, 0, 31, -6, 0, 13, -16, -12, -1, -12, 5, -19, 12, -2, -8, 9, -6, 15, 30, -16, -2, -6, -12, -7, 12, 14, -1, 30, -14, 8, -5, 7, 15, 8, -11, -29, -6, -7, 4, 4, 7, -3, -18, 5, 17, -10, 6, 0, -11, 10, 2, -10, 16, 11, 8, 18, -5, -2, -3, 2, -18, 11, -5, -8, 11, 16, 10, 3, -8, 17, 12, 7, 6, 13, -25, 5, -21, -12, -1, 17, 2, 0, -10, 12, -9, 3, 18, -12, -3, -22, 12, -25, 3, 8, -2, -3, 0, 0, 8, 13, -2, -13, 13, -1, 6, -12, 11, 4, 6, -19, 9, 7, -4, 5, -3, -21, -6, 5, -7, 5, -11, -7, -5, -1, -10, 32, 25, -15, 23, 29, 31, 10, 9, 7, 3, 19, 7, 3, 2, 8, -11, 41, 20, -1, -2, 24, 7, 21, -18, -1, -15, -2, 12, -4, 19, 12, 6, 3, 24, 21, 29, 25, 20, -4, 4, 4, 15, 13, 1, 19, 7, 36, 2, 14, -2, 21, -3, 27, -1, -4, 14, 12, 13, 51, 24, -4, 13, -12, 21, -4, -3, 35, -10, -25, -14, 4, 5, -4, -30, 22, 10, 8, 3, -8, 29, 17, -10, 7, 0, -13, 7, 19, -9, 2, -5, -9, -4, 33, 29, 15, 10, 0, -14, 20, 16, 4, -7, -24, -32, -14, -23, -4, -4, -19, -17, 3, -3, -5, -9, 11, -1, -32, -21, 25, -4, 13, -24, 14, -13, 19, 1, -28, -6, -8, 1, 10, 8, 9, -7, -3, 4, 9, 6, -6, 3, 3, -28, -3, 2, 34, 9, -9, 8, -36, -6, -6, 3, 10, -11, 5, -17, -5, 16, -17, -20, 8, -1, 22, -6, 14, -22, 1, -2, -10, -1, 5, 13, 14, -2, -3, -5, 15, 3, 6, 5, -36, 3, 23, 31, 14, -7, 3, -11, -13, 18, 14, 7, -7, -10, 17, -23, 7, -18, 4, -21, -8, -16, -11, 10, -2, -26, 7, 3, 4, -18, -19, -9, -11, -11, 30, 8, 10, -9, 10, -24, -21, 5, 2}
, {21, -29, -3, -7, 23, -12, -14, 20, 34, 19, 28, 21, 8, 19, 16, 8, -33, 5, 25, 8, 8, -13, -7, 4, 28, 4, -4, 13, 9, -4, -10, 6, 35, -25, 4, 17, 0, -17, -26, 16, 22, 24, -2, 4, 12, 18, 13, 8, 14, -38, -1, -5, -6, 39, -18, -2, 9, -7, -21, -16, -3, 5, -17, 24, -4, -1, -28, -25, -26, -28, -42, -8, 4, -40, -4, -1, -3, -39, -21, -9, -23, -28, -57, 0, 3, -22, -15, -3, 12, 3, -20, -31, 1, -23, -7, -22, -14, -16, -20, -58, 3, -4, 13, -3, 6, -9, -19, 2, 5, 2, -43, -31, -31, -15, -13, -12, 8, -7, -35, -8, -14, -16, -17, -10, -16, -29, -5, -16, 8, -15, 16, -7, -12, -25, 3, -6, -8, -1, 3, -10, -4, -6, -12, -6, -15, 23, 7, -2, 4, 4, -1, -7, 15, 2, -13, 9, 1, -9, -15, 9, -22, 6, 7, -13, -14, -13, -21, 9, -13, 5, -10, 0, -9, 7, -16, -8, -16, 11, -2, -12, -1, -41, -40, -6, 11, -11, 4, -18, -2, -37, 5, -2, -15, -14, -10, 3, -4, -19, -35, 40, 10, -2, -10, -7, 6, 0, -41, -11, -16, -5, -4, 2, 8, 0, -28, -3, 13, 3, 23, -30, -9, -34, -8, 10, -16, -19, -27, -9, -2, -28, -21, 12, 5, -21, -29, -10, -2, -37, -17, -29, -20, 2, -19, 6, 0, -11, -24, -1, -26, -1, -17, -22, -28, -19, 1, -5, -10, -15, -17, -22, -15, -23, -12, -9, -4, -22, -13, -6, 7, -27, -20, -30, -21, -13, -10, -11, -12, -23, -6, 2, -18, -10, -7, -20, -3, -33, -2, -2, -27, -13, 11, -18, 3, 7, 21, 3, 6, 16, -6, 2, 1, 18, -19, -20, -33, 11, -3, -9, -7, -8, -12, -7, 2, -16, -14, 5, -3, -37, -5, -10, -8, 22, 5, 4, 10, -11, -27, 1, 7, -57, -37, -2, -2, -40, -3, -37, 12, -8, 3, 4, 6, 5, 11, -18, -32, 0, -4, -19, -24, -11, -8, 11, 12, 0, -13, -18, -12, -22, -2, 5, -2, -28, -50, -5, 6, -15, -6, -57, 9, 10, -15, -2, -9, -12, 9, 0, -40, -5, -4, -7, -9, 9, -8, 2, 4, -21, 16, -19, -15, -10, -5, -12, -2, -7, -33, -7, -9, 10, -9, -34, 0, -20, 15, -3, 3, -3, 15, 4, -21, -7, -23, -18, 18, 5, -1, -7, -6, -9, -24, -7, -21, -26, -10, 4, -2, -22, -34, -5, 2, -34, -22, -21, -10, -24, -34, -10, 8, -15, -10, 3, -1, -12, -8, -19, -11, -19, 1, -11, -11, -8, -19, -24, -10, -30, -4, -3, -7, -13, -18, -10, -6, -17, -27, -25, 7, -3, -5, 0, 5, -6, -8, 5, -1, -3, -15, -19, 2, -13, -7, -1, -5, -8, 12, -5, -3, 5, 22, -10, 8, 3, 5, 2, -11, 16, -8, -7, 10, -22, 4, 1, 4, 31, 27, 2, 10, -2, 15, 8, 27, 15, 0, -2, 5, 8, 29, 2, 6, -5, 23, -8, -5, -4, -9, 11, -7, 27, -18, 5, 1, -14, -2, 21, -12, 7, -14, 3, 9, -10, 15, 17, 14, 0, -4, 0, 3, 0, 2, 20, 3, 16, 12, -11, -16, 13, 0, 13, 3, -1, 7, 5, 16, 23, -2, 2, 8, 6, 9, -6, 31, -3, -3, 8, 10, -2, -10, -4, -3, -3, -9, 4, 11, -16, 8, -18, -21, -31, -40, 13, -5, -23, 15, -36, 31, -37, 7, 5, 0, 3, 14, 4, 2, -9, 15, -14, 8, 21, -8, 2, -10, -24, -14, -21, -10, -6, 7, -26, -17, 0, -29, 1, -1, -3, -2, -14, -4, -4, -18, 5, 2, -32, -7, 32, -40, 2, -10, -11, 0, -5, 11, -16, -5, -10, 17, -13, -22, -19, -8, -12, -12, 6, 0, -17, -3, 12, -3, -6, -2, 6, -15, 0, 0, -21, 14, -22, 18, -8, 8, 5, -8, -6, 14, 1, 0, 14, -12, -8, -6, 5, 3, 1, -21, -20, -17, 0, 2, 0, -5, -13, 22, -11, -8, 13, -7, -2, 15, -2, 18, -10, -1, 5, 22, 1, 10, 10, 6, 6, 28, -33, 5, 22, 49, -45, -29, 15, 35, 0, 4, 5, 36, 2, 16, -5, 51, 8, 8, 21, 31, 18, 18, -1, -10, 11, 27, 18, 7, -15, -16, 24, 33, 31, 11, 20, 17, -33, -16, 2, 13, 1, 1, 23, -4, 11, -10, -9, 9, -3, 0, 28, 25, 8, 17, -8, -9, 27, 7, 22, 3, -39, -2, -5, 51, -9, -17, 16, -35, -3, -10, -33, -24, 26, 4, 18, 7, -11, 2, 4, 19, 6, 8, -3, 11, -10, -15, -1, 0, -39, 30, 8, 9, 11}
, {15, -11, 6, -11, -6, -2, 0, -13, -19, 16, 9, -29, -12, -9, -20, 11, 18, -25, -7, 1, -7, 0, -13, 15, 13, -4, -17, 3, 23, 4, 5, 14, 52, 6, -15, -13, 24, 8, -25, -23, 1, -5, 4, 21, 0, -11, 4, 32, -12, 0, 26, 22, 5, -11, -6, -10, 27, 6, -5, -10, 18, 12, -11, -12, 11, -18, 7, -11, 40, 15, -27, 20, -3, 9, 19, -4, -12, 3, -22, 28, -12, 17, 6, -26, -7, -15, -20, 1, 15, 5, 2, 25, -5, 12, 10, -12, -16, -11, 6, 15, -13, -6, -6, -7, -6, -14, -8, 7, -15, -13, -12, -25, 11, -20, -16, -6, 1, 7, 5, 0, -13, 15, -19, 4, 12, -28, -17, 4, 18, -18, -9, -6, 3, -19, -13, 18, 9, -26, 10, 6, -11, -22, 37, -15, 0, -26, -15, 5, -5, -2, -2, -13, -1, 6, -3, -7, 27, 19, -9, 7, -3, 18, -9, 22, -13, 4, -7, -6, 22, -8, 6, 8, 0, 8, 26, -2, 26, -9, -2, 14, -2, -12, -1, -17, 4, 0, 13, 0, 12, 17, 22, 20, 13, 3, -12, -7, -3, 0, -4, -3, -19, -15, -15, 2, -1, 0, 5, -7, 19, 17, 6, 17, -5, 6, -6, -1, -5, 9, -14, -14, -23, 21, 27, 9, -8, 9, 26, -11, 5, 17, -5, -11, 3, -5, -13, -24, 2, 13, 15, 7, -17, 16, 7, -23, -4, -1, 21, 14, -14, -8, 15, 9, 15, -1, -3, 16, -13, -23, -5, -5, -15, -31, -7, -7, -7, 30, 11, 2, 2, 3, -11, 14, -11, -46, 2, 10, 10, 9, -33, -19, 9, -3, 12, 1, -9, -19, -25, -17, -1, -45, -14, -20, 7, -6, 11, 16, 36, 9, 23, 34, -14, 17, 38, 4, -17, -9, 10, 44, 2, 11, -16, 21, 3, 6, 0, -2, 8, -25, 9, 0, -15, -11, -25, 11, 8, -3, 6, 24, 14, 6, 13, 8, -8, 4, 9, -4, -28, 23, 1, -8, 4, -23, 25, 11, -22, 4, -3, -2, -18, -14, 14, 0, -25, 13, 32, 17, 8, 47, 7, 20, 7, -1, -17, 6, -14, 4, 14, -7, 21, 15, 0, -6, 4, 13, 10, -4, -6, 9, 1, 13, -10, -6, 9, 17, -6, 11, 6, 14, -10, 8, -23, 1, -7, -8, -10, -8, -6, 0, -4, -1, 22, -9, -14, -14, -5, -26, -3, -2, 8, 10, -39, -20, 7, -1, 2, 31, -11, -15, 3, -9, 1, 11, -3, -17, -22, 23, 3, -6, 2, -17, -27, -7, -18, 13, 10, -14, -5, -2, 1, -15, 6, 1, -8, -16, -20, -2, 21, -11, 14, 25, 26, -5, 41, 18, 22, 3, 0, 40, 7, 36, -14, 4, -36, 7, 7, 16, 37, -19, -9, 13, -12, -22, 11, -4, 46, 33, 10, -37, -1, 3, -16, 2, -4, -4, 3, -4, 10, 34, 16, 19, 18, 11, -17, 26, 2, -9, -20, 30, 6, -29, 1, -18, 22, 27, -15, -3, 3, -18, -10, -27, -4, -14, -26, 2, 10, 12, 8, 22, 2, 25, 31, -2, -5, 11, -15, 21, -9, 8, 26, 11, 29, -16, -9, -2, 15, -8, 7, -1, 7, 10, 11, -1, 0, 29, -1, -18, -8, 31, -31, -10, -22, 18, -4, -59, -45, 20, -11, -5, -17, -16, 24, -8, -26, -2, -11, 12, 10, -7, 8, -4, -8, -22, 15, 19, -1, 20, 25, 1, -15, -11, -12, 4, 13, -20, -25, -9, -6, -28, -8, -9, 9, -14, 2, -12, 5, -12, 0, 1, 8, 13, -21, -3, 0, 9, 1, 18, -5, -8, 11, 9, -15, -3, -13, 27, -1, -13, -15, 0, -3, -42, -10, 4, 1, 11, 11, 3, -15, -34, 6, 18, 14, -10, 5, 6, 10, 17, 4, 10, -14, -3, -10, -18, -16, -15, 1, 15, -7, 17, 9, 30, 0, 5, -11, -13, 5, -8, -12, 11, -11, -22, 4, -25, -12, 7, -1, -3, -4, 8, -19, -4, 14, -17, -17, -4, 14, 4, 7, 1, -14, 28, 33, 10, 14, 32, 8, 36, 3, 10, 33, 15, 31, -23, -9, 4, 16, 5, -23, -6, 14, -23, -26, 0, 6, 9, -12, 23, 26, -31, -10, 8, 14, 4, -4, 6, -10, 0, -14, -1, 8, -12, -3, 26, -12, 16, 6, 10, 15, -17, 18, 11, -3, 3, 37, -3, 14, 13, 7, -2, 8, -10, 2, 12, 23, -5, 10, 3, 32, -32, -1, -3, 9, 20, -27, -22, 0, 4, 0, 6, 24, -43, 5, 12, -8, 13, 4, -20, -25, -22, 14, -49, 4, 5, 26, -27, -8, 27, 33, 23, 41, 6, -4, 10, 40, 17, 21, -21, 20, 20, 4, -3, 19, -23, 28, 11, -22, 4, 5, 21, -24, 24}
, {18, 17, 43, -21, 20, -8, 15, 12, 14, 47, 18, -8, -3, 17, -15, 42, 21, -3, 24, -18, -8, 10, 1, 38, 23, -1, 35, -7, -1, -2, 12, 35, 27, -20, 29, 20, 11, -9, -44, 25, 33, 27, 20, -4, 8, 2, -5, 47, 25, -12, -8, -5, -8, -7, -17, -7, 45, -1, 23, -9, -4, 2, -9, 20, 41, -9, -14, 7, 19, -17, -24, 11, 15, 30, 9, 23, 11, -23, -30, 10, -7, -17, -4, -17, 5, -10, -9, -9, 14, 2, 6, -12, 13, 11, 6, -9, 0, -20, 23, -13, 10, 5, 6, -8, 5, 12, 18, 6, 5, 9, 19, 2, 3, -3, 11, -5, 8, 5, 15, -7, -10, -1, 8, -6, 6, -5, 9, 9, -8, -3, -17, 27, -10, 4, -12, 11, -5, -5, -10, -8, -4, 6, 6, 5, 12, 9, -7, -16, -5, -13, 8, 2, 14, -11, 2, -7, -14, 1, -6, 4, 1, 3, -10, 13, 3, 8, 0, -8, -10, 3, 0, 1, -12, -15, 16, 6, -1, 15, 3, 16, -3, -17, 5, -27, -2, -6, 1, -7, -5, 19, -10, 27, -25, 15, -1, 9, 2, 3, -4, -18, 4, -1, -3, -2, -1, -8, 25, 13, 11, 25, 5, 4, -8, 12, 11, 5, -15, -7, 3, 0, -27, -8, 0, 10, -21, 10, 15, -8, 4, 2, 4, -5, -4, -14, 8, 8, -3, 3, 3, 8, 6, -4, -10, -1, 7, -13, 19, 20, -1, 4, 3, 20, 6, -26, 17, -14, -12, -31, -3, -11, -4, -30, 19, -5, 8, 17, 4, 10, -4, 6, -4, 9, 11, -39, 1, 11, 7, -12, -16, -2, 28, 2, -18, 11, -9, -2, -28, -13, -5, -39, -6, 8, -2, -28, -1, 7, 21, 7, -4, 0, -6, -4, 13, -10, 10, -10, 3, 25, 4, 7, 14, -6, -7, -7, -16, -9, 6, 11, -6, 15, -11, 8, 3, -10, -9, 1, -3, -6, 20, 8, 8, -3, -6, 3, 5, -19, -27, 15, -3, -5, 8, -6, -6, 9, -25, -9, 12, -16, -9, -15, -6, 0, -20, -6, 17, 14, 8, 29, -10, 6, 1, 5, 15, -20, -3, 24, 1, -2, 32, 15, 18, -26, 3, 5, -6, -7, 7, -8, 5, -9, -10, -16, 5, 32, -10, -2, -10, 8, -36, 8, -27, 7, 7, -16, -12, -19, -8, 3, -13, -5, 19, -5, 6, -7, 5, -7, -12, -3, 6, -2, -32, -27, -18, 0, 12, 13, -3, -16, 4, -26, -9, 10, 5, 20, 4, 28, 12, 28, -8, 19, -27, 4, -18, 9, 42, 12, 0, -3, 1, -24, -10, -6, 16, -33, 0, -33, 19, 9, -3, 32, 32, -1, -17, 21, 1, 23, 5, 37, -13, 10, -6, -5, -14, -9, -7, 40, 28, 2, 7, -7, -5, 11, -9, 7, 22, -9, 2, -60, 24, 7, -12, -1, -13, -2, 9, -4, 1, 4, -3, 0, -3, 2, -11, -2, 1, 7, -3, -29, -9, 1, 9, -6, -23, -19, -6, -1, -20, 26, -4, 12, -20, -11, 15, 2, -14, -13, 16, 15, 5, 10, 17, 13, 4, -5, -9, -19, 9, -4, -8, -7, 19, 13, -6, 6, -19, -23, -7, -7, 6, -6, 18, 12, -13, -12, -11, -56, -9, 1, -3, -6, -9, 14, -1, -12, -27, -2, -9, -1, -12, 2, -5, -13, -1, -28, -1, -5, 13, -10, -43, 2, 3, -11, -1, -7, 0, -8, 1, -3, 18, -5, -4, 16, 3, 3, 1, -15, 1, 1, -5, 8, 7, -15, 11, 30, 19, -18, -6, -18, 3, 30, -6, -5, -4, 11, -1, 7, 32, 2, 10, 16, -5, 13, -3, 22, -17, 6, -12, -16, -3, -38, 2, 1, -15, 4, 25, -11, 2, -70, -8, -12, 1, -7, -3, 6, -29, 22, -5, 16, -22, 6, 18, -1, -15, 3, 1, -10, 3, 7, -15, 13, 5, 16, 0, -19, -13, 3, -30, 2, -27, 0, -5, -15, -1, -1, 1, 3, -15, 7, 0, -15, 8, -23, -1, 10, -5, -4, 8, 14, 14, -17, -9, 10, 13, 0, 0, -17, 3, 3, -13, -7, 15, 1, -8, 10, -7, -31, -1, -10, 5, 6, -5, -7, -19, 3, -9, 10, 24, 6, -2, 8, -2, -30, -12, 8, 5, -17, -4, 7, -18, 2, -22, 2, 18, 7, 2, 23, 10, -42, 7, -6, 18, -5, 15, -17, -4, -21, -25, 7, 2, -3, -11, 11, 20, 13, 4, 10, 19, 27, -3, 14, 13, 1, 0, -11, -25, 21, 1, 13, 2, -18, 3, 5, 5, -10, -16, -37, -9, -1, -18, -19, -14, 23, 4, -12, -8, 15, 6, 0, -9, 6, 12, -6, 27, -20, -8, -7, -2, 3, 2, -3, -9, -15, 32, -9, -40, 2, -23, -3, -23, 0}
, {3, -6, -10, -5, -31, -45, -35, 1, 16, 9, -5, 12, 0, -24, 8, -22, -25, -2, -2, -4, 3, -7, -3, -17, 0, 0, 16, -38, -11, -1, -15, -4, 1, -31, -31, -21, 4, -18, -10, 24, 1, 8, 13, 16, -3, -20, 14, -4, 14, -1, 4, 12, 1, -28, 3, -9, -14, -4, -16, -37, -35, 1, 3, 17, 6, -32, 12, 31, 1, -13, -5, 14, 5, -16, 19, 3, -3, 24, 19, -6, 22, 5, 2, 4, 2, -17, 29, 4, 25, 0, -28, -22, 0, 26, 1, 12, 16, 7, 35, 17, -40, -20, 6, -5, -14, -14, 5, 4, 10, 27, 0, -13, -7, 13, 10, -34, 0, -48, 33, 17, -27, 12, -17, -13, 15, -1, 14, 3, 1, 14, 0, -3, 14, -4, 15, -8, -5, -2, -20, -5, 15, -4, -6, 12, 17, 3, 4, -26, -2, -11, -4, -17, 6, 6, -2, 8, -10, -21, -2, 4, 7, 12, 0, 11, 5, -3, 6, -4, -20, 6, 8, -20, 0, -30, -5, -2, -8, 5, 0, -9, 3, 16, -3, 4, -15, 4, 2, 11, 3, 5, 0, -18, -4, 4, -1, 2, 11, 9, 11, -3, 12, 10, -5, 8, 14, -15, 14, 14, -11, -19, 6, -4, -4, -1, -11, -11, -2, 2, 14, 12, -22, 0, -14, -32, -4, -11, -13, -18, -10, -8, 7, -4, 21, 8, 4, 10, -5, 5, 12, 7, -25, 10, 17, 14, 4, 2, 12, -3, 12, 2, 4, 16, -32, -26, -2, -38, -32, -2, 24, 5, 8, 22, 1, 13, 12, 8, -7, -15, 19, 11, 7, 11, -9, 14, -4, 19, 1, -7, 21, 18, 18, 2, 4, 24, -11, -25, 6, 17, 4, 3, -5, -1, -10, -2, 6, -4, 11, -1, 3, 7, 3, -3, 4, -22, -10, -12, -11, -20, -5, 1, -22, -25, -10, -4, -18, -2, -22, 7, -14, 3, 19, -33, -6, -10, -13, -11, -10, -8, -10, -2, 1, 5, 5, -16, 5, 6, 11, -33, -35, 8, -8, -18, -3, -18, 20, -4, -34, 2, 10, 16, -20, -17, 15, -12, -2, -10, -10, -12, -11, 21, 14, 9, -4, -1, 11, -3, 20, -14, -11, 3, -8, -9, -1, -6, 38, 36, -7, 4, -6, -19, 19, 16, 9, -21, -14, -7, 3, -3, 3, 20, 16, 0, -2, -3, -3, -7, 17, 5, 3, 6, 4, 5, 2, 18, 5, -13, 0, 9, 2, 8, 11, 9, -2, -13, 12, 11, -18, -7, -9, 10, 14, -2, -17, -3, 5, -22, 2, -29, 23, 2, -16, -5, 15, -18, -11, 0, 4, 28, -36, 5, -1, 7, 10, 0, 5, -27, -13, 9, 21, -19, 0, -16, -7, -14, 38, 22, 6, -7, -1, -17, 0, -24, 12, -2, 4, -14, 2, -10, -6, 20, -2, 3, -6, 9, -25, -3, 45, 21, -4, 10, 2, -3, 30, -4, 10, 29, 20, -13, -15, 5, 25, -3, 10, -13, -7, -12, -25, 5, 16, -1, 6, 30, 9, -14, -11, -2, 12, 3, 2, -15, 11, -12, -13, 32, 16, 1, -3, 14, 7, 18, 17, 15, 12, 14, -9, 17, 2, 12, -19, -14, 17, -25, -6, 2, -14, 38, 5, -2, 21, 8, -1, -26, 15, -1, -13, -23, 3, 17, 7, 6, -31, 2, 3, -11, 4, -1, 0, -9, -17, -7, 21, -6, 0, 4, 3, -17, -22, -22, -16, 12, -6, 4, -6, 7, 4, 26, -7, -19, 5, -12, 5, 2, 22, -2, -2, 18, 3, 8, 17, -26, -22, -8, 9, -9, 4, 10, 8, -16, 10, -10, -8, 0, -19, 6, 8, 0, -11, -5, -6, -18, -2, 1, 37, 8, -25, -5, 1, 30, 18, -1, -3, 8, 3, 11, -11, 1, 23, -10, -4, 2, -3, -18, 7, 3, -1, 14, 7, -10, 18, 2, 3, 0, 0, -6, 7, -32, -1, -30, -37, 8, -14, -33, 6, -19, -31, -5, -37, -11, -29, -21, -13, -15, -46, -45, 21, 8, -16, -14, 8, -16, -34, -22, 6, -30, -27, -8, 36, -18, 14, -18, 7, 28, 17, -15, 7, -24, -17, 22, 2, -25, 9, -8, -8, 2, -21, -37, 17, -1, 1, -11, 19, 11, -35, -27, 5, -13, -20, 17, 2, -25, -3, 18, 23, 1, 23, 3, 23, 7, 12, -21, 10, 0, 10, -12, 4, -10, -5, 13, -3, 11, 17, -9, -13, 0, -3, -3, -14, -1, 11, 13, -35, 11, -29, 22, 31, -1, 1, 2, 18, 6, 12, -17, 21, 20, 21, 14, 6, 6, 41, 23, -5, 8, 3, -19, -7, 7, 16, 28, -25, 6, 16, 6, -29, -8, 15, 5, 0, -19, -1, -20, 5, 6, -9, -24, 26, 6, -11, -10, 3, -4, 25, 12, -32, 11, 20, -15, 14, -4, 4, 23}
, {-2, -8, -7, -3, 6, -2, -3, -2, 3, 11, -3, -12, -14, -21, -6, 7, 5, -6, 19, -5, -6, 18, 1, -33, 8, -7, -17, -5, 25, 22, 3, -23, 50, -44, -6, 13, -2, -41, -59, -16, -8, -22, -21, -17, -2, -8, 15, -26, 0, -17, 12, -6, 3, -19, 0, -31, 2, 1, -27, -35, 11, 28, -5, -1, 2, -23, 7, 12, 17, -3, 5, 1, 1, -11, -43, -10, 1, 7, -19, -20, 2, 9, 2, -11, 8, -21, -3, -5, -23, 5, -12, -5, -20, 12, 0, 16, -28, 15, 11, -7, 12, -2, 13, 11, 5, 8, 8, -6, -3, 9, -7, 10, 2, -5, 6, 6, 2, 5, -5, 2, -24, -1, 9, 21, -5, -10, 0, 2, -6, 11, 10, 1, 5, 9, -12, 14, 1, -11, -7, 2, -6, -1, -22, 11, 14, -11, -5, -1, -6, 18, -15, 13, 22, -10, 12, 14, -17, -23, 3, 4, -5, 6, -5, -1, -3, 7, 7, 5, 15, -2, -9, 12, -17, 17, -6, 21, 2, 6, 11, 9, 7, -1, 9, 44, 14, 11, 0, -4, -10, 7, 35, -11, -6, -23, -10, 12, 5, 1, 8, 2, -3, -11, -10, -9, 0, 13, 5, -3, 3, 2, 7, 2, 6, 3, 15, 1, 10, 5, -2, 1, 4, -4, 17, 6, -3, 2, 6, 2, 5, 9, 15, 18, 13, 9, -12, 0, -2, 9, 19, -5, 4, 28, 5, -16, -10, 2, -3, 15, -19, -7, 8, 18, 25, -8, 20, 30, 7, -2, -5, 6, 3, -7, 6, -1, 7, 15, 14, -11, -21, -17, 7, 6, 7, -18, -10, -6, -6, -4, -17, -24, 1, 7, 20, 7, 26, -2, -32, -5, 27, -33, -7, -10, 13, 7, -5, -5, -8, -16, -5, -13, -1, -22, 18, -1, 3, -42, -27, -8, 7, 10, -14, -31, 12, 6, -12, 4, 27, 15, -24, -4, -3, -4, 9, 20, -21, 12, -1, 30, 16, -5, -3, 16, -2, 22, 15, 3, -3, 7, 23, 21, 8, 5, 12, 37, 10, -5, 9, -25, 4, -20, 28, 34, -12, 6, 18, 6, -16, 13, 13, -10, -8, 6, 1, -6, -11, -2, -1, 3, -9, 18, -1, 13, 1, 6, 2, 6, -2, 0, -22, -2, -2, -20, 3, -4, -10, -2, -2, -10, -12, 10, -21, 13, 13, 20, 4, 17, -24, -4, -25, -5, 3, -16, -17, -3, 3, -16, -35, 7, 3, 11, 1, -26, 6, -21, -6, 11, 27, -34, 0, -1, 2, -17, 10, -5, 1, 18, 5, -32, -21, -7, 8, 10, 18, -8, -8, -1, -8, -8, -1, -29, 22, -3, -12, -36, 14, 23, -7, 8, 45, -26, -37, 8, -3, -15, -20, 1, 5, 5, -2, 22, -18, -17, 24, 9, 30, -5, 1, 30, -8, -1, 17, -16, 11, 2, 2, -19, 6, 49, -11, 3, -16, -9, -4, 10, 8, 14, -4, 24, 4, -12, -17, 23, -1, 10, -21, 15, 8, -3, 10, 24, 8, 4, -2, 40, -22, 5, 20, 4, 4, -26, 12, 15, -35, -14, -11, 12, -2, -8, -13, 7, -7, 10, -2, -6, -18, -10, -14, -8, -14, 15, -3, 7, 4, -27, -5, -3, 2, 11, -2, -4, -12, -16, -3, -7, -8, 11, -1, 15, -13, 3, -2, 1, -1, -1, -42, -13, -8, -21, -16, -28, 7, 6, -25, 7, -8, 8, -10, 5, -20, 0, 11, -14, -14, -9, -6, 2, -35, 12, -7, -1, 10, 14, 34, -13, -7, -1, 15, -11, -4, -10, -10, 13, -27, 26, 20, 7, 3, 15, 11, 3, 0, -5, 2, 6, -35, -26, 22, -20, -22, -1, -20, -14, -3, 7, 37, -1, 11, 5, 24, 13, -17, 6, 1, 9, -9, -16, -5, 32, -8, 31, 8, 15, -11, 4, 7, 30, -18, -14, 15, -17, -13, -24, -3, 21, -5, -2, -21, 17, 28, -22, -5, 8, -1, -4, 15, -11, 40, -9, 2, 32, 0, -5, 13, 15, -4, 2, 13, -8, -15, 42, 0, 10, 5, 18, 9, -45, -15, 8, -9, -2, -11, -13, -10, 8, -7, 1, 17, -10, 26, 16, 26, 2, -9, 16, 14, 28, -15, 1, 18, -19, 11, 17, 11, -16, -20, -10, 3, -7, 2, -3, -3, -16, -11, -39, -15, -31, -15, 5, 16, 1, 17, -17, -25, -15, 3, -21, 9, -19, 16, 9, -15, 30, 39, 14, -11, -17, 25, -18, 8, -6, 5, 6, 8, -37, -49, 6, -5, -33, -6, 5, -43, 3, -20, -31, -8, -42, -5, -33, -11, -8, -5, -7, -25, 28, -28, 6, -18, -26, 20, -4, -16, -17, 9, -28, -15, 1, 18, 4, 12, 15, -1, 5, -7, 6, -26, 10, -23, 20, 0, 8, 11, 13, 11, 11, -10, 15, -9, 26, 25, -33}
, {27, -43, -28, 20, -14, -18, -30, 23, 23, -25, -1, 25, -2, -15, 2, -8, 24, -36, -18, 17, -9, 2, -37, -14, 10, 7, 7, -5, 11, 6, -14, 25, 40, -31, -53, -16, -12, -30, 2, -22, -2, 11, -4, -6, 5, -57, 5, -32, 1, -7, -10, -4, -8, -4, -3, -38, -28, -2, -25, -40, 17, 4, -24, -3, 22, 13, 17, -10, 0, 9, 13, 37, 22, 6, -17, 16, 9, -3, -1, -14, -25, 9, 10, -11, 2, 15, -7, 8, -39, 2, 6, -7, -16, -21, 17, 0, -27, 5, 6, 22, 10, -10, 3, 29, 12, 17, 15, 32, 3, -19, -33, 1, 5, 34, 32, -23, -10, 21, -22, 27, -45, -2, 24, 0, -23, -34, 23, 13, -10, -1, -1, 21, -4, -6, -7, 11, 21, 14, -10, 23, 6, 9, -13, -8, 19, -11, 17, 2, -8, 18, -16, -11, -17, 2, 10, -9, -1, -13, -9, 5, -6, -2, -11, 2, -4, -16, 1, 46, 34, -17, 4, 28, 4, -11, 8, 3, 12, -20, 20, 4, -7, 30, -26, 23, -12, 0, 35, 1, 31, -27, -1, 23, -13, 7, -7, -10, 11, 13, 16, 16, 0, 24, 10, 18, 1, -13, 33, -12, -11, -13, 4, 35, -7, 5, -3, -4, -26, -4, -5, 17, 16, -5, -3, 0, 7, 20, 1, 3, 21, 7, 5, 14, 8, 15, -9, -56, 7, -17, 20, 0, -8, -7, -4, -3, -1, 0, -12, 10, 12, -9, 14, 0, -16, 10, 6, 20, -2, -19, 15, 9, 2, 3, -18, -25, -19, 13, -4, -18, 6, 17, -2, 24, -6, 13, 4, 16, 2, 3, 16, -17, -9, 7, -16, -7, -10, 18, -13, -16, -20, 20, 3, -6, -9, -8, 9, -8, -24, 19, 33, 6, 0, -5, -4, 18, -1, -2, 13, 8, -6, -3, -17, -2, 6, -1, 15, 0, -7, -17, 1, -18, 12, 1, 15, 19, 6, 15, -16, 19, -10, 11, 13, 27, 4, 25, -2, 11, 0, 1, -7, 19, -10, 16, -15, 12, 3, 4, 14, 18, 3, 9, 0, 11, -1, 5, -14, -15, 5, -14, -21, 1, -2, 2, 10, -3, 7, -18, 24, 9, -5, 23, 18, 24, 1, 0, -8, 22, 15, -8, -4, -13, 19, 18, 25, -22, 1, 2, 15, -9, 22, -9, 11, -4, -7, -7, 0, -16, 6, -15, 9, -12, 27, 1, 8, 2, -7, 8, 19, -10, 9, 1, 2, 1, -2, -2, 14, 2, 6, 19, 1, 4, 13, 10, 40, 9, -20, -6, -10, -19, 1, 16, -25, 3, 28, 9, 6, -8, -10, 29, 1, -14, -4, -11, 14, 26, -13, 19, 8, -7, 33, -4, 5, -13, 18, 3, 46, -32, -24, 16, 9, -32, -4, -15, 6, -6, 5, -30, -10, 8, 9, 10, 23, -36, 3, -4, -14, 18, 16, 42, -23, -18, -1, -19, -7, 23, -10, 12, 0, 0, -11, -10, -11, -15, 3, -21, -12, 2, 2, 24, 4, 8, -8, -13, 5, 24, 1, -11, 2, -10, -11, 1, 25, 12, -13, 16, 10, 4, -6, 23, 6, 4, 7, -13, -5, -2, -13, 15, -6, 13, -4, 11, -11, 10, -1, -8, 8, 9, 19, 5, 0, 14, -7, 6, 11, -3, -16, 3, 0, 5, -2, 27, 13, 20, 13, 9, 21, -13, -5, 0, -13, 11, -8, 27, -3, 2, 1, 12, -4, 2, 15, 10, 4, 0, -24, -7, 3, 1, 3, -16, -9, 14, 9, 7, 3, -11, -15, -10, -14, -55, 15, -8, -16, -7, 14, 6, 2, -7, 8, 5, 12, -17, 5, -1, -41, 27, -10, 0, -15, -9, 5, -4, -20, -30, 11, -10, 30, -14, -10, 4, 3, 15, -3, 7, -13, 6, -17, -2, -13, 17, -9, 12, 1, -19, 0, -6, -29, 3, -15, 1, 7, -26, -21, -11, -6, 8, 0, -3, 3, 17, 24, -13, 2, 16, 12, 14, 21, -27, 33, -3, -10, 11, -3, -2, 7, 43, -17, 0, 10, -4, 8, 19, 4, 23, -3, -6, 0, -10, 12, -13, -10, -7, 1, 2, -10, 4, -2, -1, 7, -9, 19, -18, -10, 4, -6, -14, -8, 5, -12, 4, -8, 1, 16, 18, 4, -3, 10, -15, -5, 1, -5, 3, -26, -3, -1, 3, -12, 23, 0, -3, -12, -10, -1, 14, -8, 3, -6, -28, -4, 17, 14, 2, -17, -4, 9, 3, 7, 3, 5, -13, -19, 18, -4, -14, -17, 6, -5, 7, -4, -4, 3, 1, -3, -19, 13, 6, -4, -1, 0, -22, 1, 30, -3, -9, -9, -5, -19, 29, 7, 5, 6, -13, -20, 3, -1, -10, -13, 0, -16, -18, -10, -6, -2, 4, -18, -9, 7, 0, -8, -26, 7, -19, 7, -2, 1, -7, -4, -11, -13, 31, 7, 0}
, {-13, -6, -8, -1, -38, -28, -25, -20, -12, -24, -22, -10, 14, -21, -36, -45, -4, -12, -28, -5, -1, -12, -14, -15, -19, 6, -33, -34, -25, -14, -8, -13, -5, 9, 1, -28, -16, 5, 10, 17, 2, 18, 17, -16, 12, 6, 10, -13, -16, -7, 1, 4, 7, -5, -4, 19, 4, 1, 0, -17, -2, -20, 10, -3, -45, -27, 28, -2, -5, 10, -10, -5, 16, -4, 3, -9, 1, 23, 8, 3, 2, 13, 24, 11, -1, -9, 5, 2, 7, -5, -10, -2, 36, -5, 10, 13, -14, -14, -1, 14, -38, -5, -3, 13, 7, -31, -11, 24, -5, 28, 14, -38, -8, -23, 0, 6, 6, -1, 4, -2, -25, -10, -2, -15, 13, -6, -5, -25, 3, 10, 18, -28, -36, 12, 17, -1, -8, -15, -21, -31, 12, -3, -1, -39, -6, 14, -19, -3, 2, 3, 4, 0, -38, 6, 2, -23, -5, -8, 5, -6, -7, -25, -5, -1, -16, -15, -19, -24, -14, -15, -23, -10, 16, -21, -11, -39, -12, -11, -18, -8, -6, -18, -11, -17, -21, 4, -20, -19, -29, -12, -7, -22, -19, 11, -5, -19, -2, -16, -18, -7, 6, 30, -5, -5, 15, -16, -36, -9, -5, -10, -5, -29, 3, -9, -8, -4, 3, 9, 3, -18, -28, -14, -8, -1, 6, 9, -16, 13, 1, -22, -10, -18, -9, 3, 9, 36, -2, 6, -50, 20, 13, -5, 24, -16, 0, 11, -12, -10, -2, 8, 13, -21, -1, 9, -1, 9, 16, 16, 0, -5, -3, 15, -12, -5, -4, -19, -6, -3, 6, -4, -2, -9, 3, 15, -1, -10, 9, -10, 11, 30, 7, 10, 19, -20, -15, 26, 15, -13, 10, -3, -5, -21, 16, 2, -13, 11, 4, -23, -7, -7, 7, -28, -17, -15, 8, 3, -17, -16, 5, -2, 3, -1, -13, 4, -14, -10, -18, 25, 9, 9, 5, 18, 7, 0, 4, 0, -8, 1, 3, -19, -6, -17, 4, -22, -2, -20, 17, 3, -5, -6, -10, 1, -10, -18, 5, 10, 25, -2, -5, 11, -7, 8, 3, -25, -1, 7, -15, -14, 17, -11, -9, -13, -4, 2, 6, -23, -20, -2, 13, -8, -5, -27, -8, 28, 18, -8, -11, 0, 19, 8, 14, 5, -10, 31, 11, -6, 8, -10, 1, 2, 27, -22, -11, -7, 10, -6, 9, 14, 26, 5, 7, -19, 7, -5, 9, 25, 10, 5, 0, -9, 17, 28, 26, 14, -22, -22, 2, -22, -9, -7, 12, -29, -23, 3, 21, 6, 11, 4, 4, -14, 35, 9, 13, 1, -4, 2, 1, 3, 12, 17, -28, 11, 0, 3, 6, -7, -5, 11, -22, -14, 17, 3, -12, 9, -13, 21, 27, -24, -19, 7, -2, 32, 40, -6, 8, 13, -1, 3, -11, 4, 10, 31, -23, 11, -11, -6, 16, 17, 27, 32, 21, 2, 30, 2, 0, 24, -4, 14, 7, -7, 3, 5, 13, -10, 12, 21, 14, -23, -20, 9, -4, 17, -9, -23, 16, 0, -12, 37, 12, 30, -2, 30, 37, -11, -10, -16, 30, -27, 25, -16, -21, 10, 23, -19, 6, -18, 15, 14, -2, -26, -2, 25, 1, 36, -17, -7, -23, -10, -2, 26, 24, 24, -26, 3, 36, 6, -3, -32, 35, -24, 18, -36, -21, 23, 17, -4, 14, -12, 5, 16, -20, -9, -17, 8, -5, 16, -17, -36, 3, 8, -5, 15, 17, 16, -9, -24, -17, 31, 19, -22, 11, 1, 32, 31, 28, 24, 12, 65, -1, 10, 2, 6, -15, 19, 17, 14, -9, 12, -4, 11, -12, -6, 14, -19, -8, -39, 18, -8, -35, -14, 17, 20, 13, 11, 5, 25, 20, 13, 32, 6, 1, 21, 0, 22, 10, -9, 5, 22, -4, 7, -38, -1, 22, -4, 28, 13, -16, -38, 8, 22, -5, 17, 13, 6, -7, 10, 11, 1, -2, 13, 8, -5, 12, 3, 8, -11, 13, 12, 11, 29, 3, 18, 13, -23, 11, -3, 12, -13, 28, 4, -8, 3, 33, 13, 3, -14, 0, 17, -10, -25, -19, -8, -10, -25, 15, -17, 8, 2, -6, -19, -12, 23, 3, 3, 7, -29, 29, 4, 2, 8, 4, 30, -31, -11, 13, 16, -4, -7, -9, 18, 19, 3, -14, 10, -13, -1, 18, -13, 1, -18, -17, 19, 4, -8, -9, -12, -3, -6, -8, 1, 11, -9, -32, 7, 28, -6, -27, 16, 5, 13, -2, 19, 15, -2, 0, -3, 4, -5, -4, 12, -30, 7, -18, 8, -4, -26, 3, -9, -12, -8, 8, -2, 19, 9, -24, -33, -10, -28, -34, 26, -4, 6, 3, 4, -2, -9, -23, -4, 16, -19, 16, -18, -39, -12, 0, 1, -12, -25, 2, 14, -32, 15, -5, 3, -9, 15, -6, -46, -2, -25}
, {-4, 0, 3, 6, -5, -5, -5, -9, 7, 2, 1, -1, -10, 5, -8, 10, -5, -4, -6, 3, 6, 9, -2, 2, 5, -8, -9, -10, 0, 6, 1, -6, -7, -1, -8, -1, -1, -5, -2, -2, 5, -8, -6, -1, 8, 10, 1, 4, -8, -3, 7, 4, 4, -8, 10, -6, 3, -4, 3, 3, -13, 0, 4, -9, -4, -3, -14, -4, 6, 1, -8, -7, -5, -13, -2, 7, -6, 0, 4, -8, -1, 0, -9, 2, -2, -10, -3, -8, -4, -7, 5, -3, -18, 2, 2, -1, -11, 1, -7, -2, -9, -11, -9, -8, -9, 4, -6, 3, 4, -1, -13, -7, -10, -2, 5, 3, -3, 1, -9, 0, -12, 6, 3, -11, -5, -2, 3, -16, -3, 7, 3, -7, -13, 2, -2, 4, 6, -1, 4, -7, 7, 4, 0, -2, 4, -4, -11, 7, 6, -6, 1, -7, 0, -1, 5, -2, -3, 0, -13, 4, 4, -3, 1, -1, -5, -10, -6, 1, -10, -5, 1, 6, 2, -7, -7, 3, 7, -1, -7, 7, 4, -4, -4, 9, -13, -2, -8, -11, 2, -10, 6, -4, 5, -6, -7, -4, -2, 6, -2, -7, 7, 1, -1, -2, 2, -7, 10, -3, 5, -8, -4, 0, -8, 7, 4, 2, 1, -2, 4, 3, -13, -3, 0, 2, -3, 2, 0, 0, 8, 9, -10, -11, -1, -2, -13, 4, -6, -8, -12, 1, -2, 4, -9, 7, 6, -5, -6, -8, 3, 2, 8, -12, -12, -10, 4, -1, 1, -3, -12, -3, -3, 1, -1, 6, -1, -8, -4, 1, -3, 7, -4, 3, 2, -10, -11, 1, -2, -1, -3, 1, -4, 4, -2, -1, 1, 3, 7, 1, -8, -2, -7, -6, 3, 6, -12, 9, 7, 3, 4, 3, 10, -2, 3, -5, -2, -6, 2, 3, -7, -11, -7, -2, -10, 3, 7, -4, -13, -7, 1, 8, -12, -2, 4, -2, 3, 6, -10, 6, -3, -7, -7, -5, -10, -8, -4, 4, -8, -6, 5, 1, -6, 3, -10, -7, -9, -3, -7, 8, -12, -7, -10, -1, -8, -8, 2, 1, 2, -9, -8, 4, -2, 0, -12, 6, 1, -12, -7, -7, -6, -11, -1, 8, 6, -2, -1, -6, -11, 4, 8, -1, 2, -8, 4, 8, -3, 5, -10, -8, -12, -8, -1, 2, -3, -1, 0, -9, -4, 2, -1, -10, 3, -4, -13, 5, 7, 5, 0, 5, 0, 4, 6, 2, 3, -2, -5, -5, -1, -6, 7, 7, 5, -9, -7, -6, 0, 2, 5, 8, -5, -9, 0, -8, 7, -2, -5, 8, -3, -11, -7, -9, 7, -6, 6, 4, -14, 0, 3, -6, -2, 0, -1, -5, -8, 7, -2, -11, -5, -9, 0, 6, -5, -2, 8, -7, 1, -9, -8, -6, -3, -5, 5, -5, -1, -6, -7, -2, -8, 4, -7, -1, 5, 4, 8, -1, 6, 7, -12, 4, -12, -7, -12, 9, 2, -2, -1, 5, -7, -2, -9, -3, 3, 1, 5, -1, -9, -8, 1, -9, 5, 2, -2, -4, -6, -6, -6, 6, -14, -7, -5, 2, 5, -12, 0, 2, -7, 2, -5, -12, 3, 8, -3, -2, 2, 1, 3, 2, -1, -6, -2, 0, -6, -3, 5, -4, -11, 8, -4, -8, -7, 6, -8, -7, -1, 2, -3, 9, 3, 1, -2, 5, -6, -8, -4, -4, -1, 4, 1, -2, 4, 4, -10, 5, -5, 3, 5, -3, -2, 5, -9, 5, -9, -3, -1, 8, -9, -11, -13, 0, 1, -3, 1, -4, -6, 4, 7, -10, 5, 1, -8, 9, -11, 5, -12, 1, -15, -12, -3, -8, -10, -10, 6, -6, -8, 3, 6, -8, -5, -12, 4, -7, 3, 2, 7, 4, 3, 3, -6, 0, -8, -10, -12, -6, -12, -3, 11, 4, -16, 5, -1, -9, -11, 8, -12, 4, -6, 7, 2, 7, -3, -5, -9, 8, -1, 8, -9, 3, 2, -11, -3, 4, 0, -2, 0, 1, -12, -2, -6, 5, -11, 1, -8, -5, -9, -7, -3, -8, -10, 1, 4, -6, -3, -16, -5, -5, -5, 8, -14, -1, 1, -4, 3, -6, -3, -2, 1, -7, 2, -12, 1, -12, 0, -10, -1, -1, -11, -10, 2, -6, -8, 6, 2, 0, -7, -4, -4, 7, 6, 1, 6, -7, -6, -4, 1, -6, -4, -12, 3, 2, 0, -6, -15, 3, -5, -8, -11, 6, -8, 0, 7, -10, -9, 5, -9, 10, 2, -11, 4, 7, -8, -9, 2, 7, -14, -8, -8, -8, 5, 3, -1, 2, 2, -10, -10, 6, -1, -12, -4, -2, -7, 0, -5, 5, -3, -10, -11, -3, -3, 4, 1, 4, 9, 6, -1, 5, -8, -6, -7, -7, 9, -4, 7, -3, -5, -7, -6, -8, -11, 3, 8, -7}
, {-15, 1, 6, 0, -1, 15, 19, -14, 20, 8, -11, 16, -4, -6, 7, -2, -31, 17, -6, 20, 9, -7, 3, -2, 2, 2, -23, -8, -23, -7, -12, -19, -14, -20, -12, 14, -3, 3, -22, 12, -4, -22, -14, 15, -11, 11, -2, -28, 9, 3, 12, -22, 9, -18, -9, 28, 13, -9, -13, 22, 4, 18, 6, 24, -3, -6, 3, -6, -6, -17, -1, -10, 6, -13, -5, -3, 15, 17, -11, 1, 20, 6, 1, 2, 8, -12, 7, 6, 6, -6, 4, -5, 30, -3, 14, -8, 12, -14, -7, -12, -35, -24, -14, -5, -17, -32, -23, -22, -7, -11, 5, 20, 8, -21, -19, -14, 9, -17, -2, 13, 21, -10, -20, -2, -10, 11, 4, 8, 21, -28, -7, -16, -18, -17, -24, -11, -23, 8, -12, -8, -3, 3, -11, 10, 0, -11, -8, -23, 7, -35, -1, -22, -3, -1, -16, -17, -14, 8, -18, 5, -2, 1, -10, -8, 0, 4, 5, -2, -2, 9, -8, 1, -12, -4, -28, 4, -29, 11, -4, 6, -9, 1, 9, 20, -2, 2, -9, -4, -20, -4, 15, -2, -13, 16, 23, -8, -6, 13, 9, 17, -5, -5, 6, -21, -2, 24, -4, 1, -5, 18, 17, -28, 1, 1, 20, -9, 0, 1, 12, 13, 0, -8, 36, 26, 5, -5, 0, -4, -7, 7, 8, 3, -18, -6, 6, 2, 6, 7, 0, 16, 6, -5, 0, 1, 2, 21, 6, 9, 16, -11, -6, 1, 26, -5, 0, -12, -12, -6, 6, -3, 10, 20, -14, 9, -4, -38, -9, -20, -9, -13, -8, -15, -28, -16, -5, -15, -9, 12, -7, -16, -1, 8, 20, 2, -5, -28, 0, 3, -12, -11, 12, 3, -13, -12, 0, -10, -14, -7, -32, 2, 13, 12, 15, -22, -2, 6, -4, 17, -2, 4, 7, -21, -38, 3, -15, -14, -8, -15, -9, -13, -12, -15, 12, -9, -24, -12, -6, -5, -15, 3, 0, -4, -7, 7, -17, -6, 14, -22, -4, 15, 3, 5, 5, 15, -13, 1, -15, 4, 1, -9, -13, 1, 16, -4, 2, 22, 4, 0, -7, -16, -3, 5, 6, -2, 8, 17, -10, -2, -1, -20, -21, 4, 3, -2, 5, 10, 3, -10, -13, 1, 4, -9, -5, 9, -4, 3, -8, -2, -4, -10, -8, -11, -14, -14, -11, -8, 9, -9, -11, 5, -11, -12, -1, -25, 5, -1, -8, -14, -8, 4, -3, -13, 13, -5, -5, -3, 6, 4, -3, 20, 0, -23, -10, -3, -8, -10, 4, -21, 16, -19, -10, 11, -16, 0, -2, -22, -2, -8, -13, -7, -1, -7, -16, -17, -6, 2, -6, 1, 1, -1, 12, -11, 1, -2, 0, 6, 1, -7, 5, -18, 1, 8, 14, 3, -15, -1, 1, -2, -1, 6, -3, -19, -7, 4, 3, -27, -5, -10, -2, 6, -6, 4, 5, 6, -23, -1, -15, -4, -17, -1, 5, -21, -10, -6, -15, -3, -16, -13, -2, -5, -8, 1, -13, -5, -34, -13, 0, -8, -3, -20, -3, 8, 0, -9, 2, 4, -7, 12, -26, 13, -3, -23, -22, -23, -1, -22, -8, -26, 0, 0, 0, 9, -10, -13, -4, -11, -4, 4, 0, -13, 12, -16, -21, 6, 3, -15, -10, -4, -10, -18, -4, 2, 18, -20, -6, 16, 1, -7, 14, -6, 4, -5, -27, 3, -5, -21, -4, -19, -16, 3, -18, -11, -3, -3, -6, 24, 3, -19, 11, -12, 5, -18, -7, 1, -16, 7, 4, -14, 0, -22, -18, -4, -16, 0, 14, -17, 4, -23, 5, -13, -44, -2, 13, -7, -24, -3, -5, -2, 10, -5, 17, -25, -9, 2, -23, 13, 28, 5, 9, 15, 5, 19, 6, -1, -4, -2, 28, -8, 4, 7, -9, 28, 7, 1, -6, 7, -18, 2, 7, -1, -4, -6, 15, 8, 2, -10, -14, 10, -18, -13, -9, 16, 5, 10, -31, 1, 10, -37, 4, -13, 0, -2, -23, 1, -22, 6, 9, 6, -5, -19, -2, 21, -28, 22, 6, -18, -8, -4, -1, -15, -19, 35, 15, -25, 2, -5, -40, 9, -18, -8, 13, -5, 7, 22, -27, -31, -1, -2, 12, 0, -1, -59, -22, -38, -24, 13, -9, 8, -5, -3, 19, -52, -10, 16, 24, -7, 12, -13, -13, -2, -30, -26, -6, 21, -3, 22, -21, -22, -4, -9, 26, 3, 7, -31, -19, -27, 0, -7, 16, -20, 14, 11, 4, -13, -12, 17, 3, -2, -1, -4, 21, -8, 19, -28, 3, 3, -9, 0, 13, -30, -4, 1, -17, 4, 8, 11, -29, -4, 17, 8, -1, 6, -9, 1, -3, 10, 7, 2, -16, 14, 10, 5, -4, -8, -1, 39, 19, 6, -8, 1, 1, -6, -1, -12, 9, -6, -16, -4, 12, 0}
, {-22, -14, -23, -10, -29, -31, -43, -10, -17, -55, -68, -2, 4, -38, -35, -47, -24, -12, -25, 0, 4, -36, -34, -4, -33, 0, -25, -42, -23, -7, -13, -17, -2, -53, -32, -18, -45, -70, -65, 6, -32, -54, -67, -7, 13, -22, -36, -51, -32, -44, -74, -3, -9, -56, -49, -1, 4, 6, -24, -59, -45, -34, -26, -17, -15, -38, -52, -15, -39, -45, -4, -29, -22, 34, 13, -1, 14, -31, -21, -33, 5, -2, -4, -31, -3, -12, -9, 0, 1, 8, -24, -36, -1, 9, -8, -20, 4, 28, 3, 12, -12, 2, 0, -5, -15, -43, -10, -9, 7, 46, -6, -16, 7, -29, -18, -8, 7, -17, 14, -20, 9, -3, 0, -2, 9, 16, -28, -5, -11, 27, 2, -17, 22, 14, 41, -47, -26, -24, 7, -7, 9, -33, 12, 14, 1, 22, 1, -5, -3, 20, 16, -14, 34, -5, -18, -7, -12, 2, 20, -32, -26, -44, 0, -3, -17, -6, -9, -6, 7, -35, -39, 4, 12, -16, -1, -35, 6, -32, -3, -8, 7, 0, -11, -4, -14, -5, -25, -11, -18, -2, -29, -30, -21, -42, -26, -16, -39, -41, -35, -1, -18, -47, -48, -22, 17, -27, -27, -54, 3, -28, -16, -7, -11, -8, -24, -19, -58, 12, -43, -40, -30, 6, -36, -3, 9, -9, 19, -25, -16, -32, 11, -12, -20, 17, 0, 7, 13, 4, 24, 2, 4, 20, 0, -10, 6, -13, 11, -4, -36, -2, -23, -31, 11, 2, 1, -34, 16, 11, 3, 30, 14, 6, -11, 2, 3, -5, 16, -11, 7, -17, 18, 13, 9, -11, -12, 11, 9, -3, 14, 17, 37, 9, 15, 1, 14, 4, -8, 19, 16, 10, 13, -9, 10, 10, 16, -21, -8, -37, -5, -14, 14, 2, -4, 15, -10, -7, -8, -18, 0, -7, 7, 4, 14, 12, -22, 13, -4, 2, -10, -19, 16, 6, 12, -14, -10, 20, -4, -7, 7, -20, -17, 14, 3, -22, 21, -2, 24, 2, 23, 2, 5, -13, 6, -6, -4, 8, -1, -20, 4, 19, 8, 5, -22, 2, 12, -16, -16, -24, 7, -20, -19, -21, -7, -3, 7, 3, -5, -5, -16, 5, 1, 1, -11, -3, 36, 7, -10, 5, -15, -5, 7, 1, 19, -19, -3, 13, 7, 8, -11, 19, 14, -12, 9, 9, 12, 1, 0, 23, 9, 23, 9, 26, 17, 14, 8, 9, 13, 5, 9, 14, 3, 9, -12, 1, 21, -19, 18, -10, -3, 14, 6, 0, -39, 1, 25, -4, 21, 2, -2, -4, 9, -3, 21, 11, -3, 37, -4, 33, -9, 4, 10, 14, 29, -11, 18, 2, 8, 19, 18, -36, -31, 9, -41, -11, -39, 10, 23, -30, -23, 2, 0, -17, 24, -15, 12, -25, -30, 7, 4, 4, -43, 2, -11, 3, 0, -33, 25, 34, -10, 27, 0, 14, 34, -5, 28, 29, 26, 25, 36, 4, 5, 2, 3, 26, 50, 16, -7, 8, 24, -1, -3, 22, -2, 0, 9, 8, 21, 19, 1, 9, 8, -17, -1, -2, 5, 14, -5, 4, 7, -12, -7, -4, 8, -39, -5, 1, 6, 7, 6, -14, -1, -31, -11, -8, 0, -3, 18, -7, -8, 2, -16, -1, 5, 12, 2, 12, -12, 9, -15, 12, 3, 14, 27, -4, -4, -6, 0, 12, 18, -4, 5, 17, 9, 10, -1, -6, 3, 7, -14, 10, 9, -6, -18, 10, 6, 19, -6, 24, 17, 19, 11, 29, 33, 12, 21, 19, 0, 27, -5, 18, -12, 0, 1, 9, 19, 0, 3, 17, -3, 17, -10, 9, 22, 16, 19, -9, 10, 27, -31, -16, 7, 15, 14, 12, 8, -1, 19, 18, 5, -7, 13, 2, -22, -2, -34, -10, -6, -6, 5, 9, -45, 5, -3, 10, 5, 10, -29, -30, -4, -9, 8, 9, 9, -19, -11, -3, 2, 17, 25, 12, 6, 30, 2, 2, 4, 20, 25, 11, 28, 17, -7, 11, -1, -14, 37, 0, 1, -21, 9, 30, -4, -6, 18, 13, 7, 12, 2, 6, 4, -10, -22, -2, -7, -41, -12, -3, 13, -5, 10, 10, -9, 10, -2, 11, 6, -9, 9, -2, 13, 24, 11, 29, 11, 14, 7, 18, 2, -2, 43, 18, 22, 14, 7, 24, 31, 27, 12, 0, -41, 23, 14, -2, 24, -6, -9, 16, -3, -13, -16, 5, 17, 1, 7, -29, 15, -6, -10, -22, -6, 23, -8, 8, -1, -11, -7, -17, 13, -16, 18, 22, -14, 8, 10, -42, -17, -21, 3, 8, -29, -19, 4, 1, 7, 15, -11, -18, -14, 14, 7, -11, -6, -11, -18, -13, 2, -29, -39, -38, -15, -33, 13, -18, -12, -9, -29, -18, -33, -35, 4, -1, -13, 13, -5, -2, -8, -2, 35, 1, -5, -37}
, {25, -16, -13, -1, -20, -48, -15, -25, -12, 3, 18, -15, 5, -50, 10, -28, 11, -30, -3, 35, -7, 26, -12, -52, 1, -1, 15, -15, 16, 23, -29, 4, 49, -20, -39, -10, -5, -33, -7, 4, 2, -25, -29, 5, 14, -36, 13, -22, -5, -31, 0, 31, -7, 0, -9, -48, -2, 3, -15, -16, 13, 19, -13, 6, 3, -28, -17, -5, -19, -42, 19, 3, -20, 19, 8, 20, 18, -19, -8, -40, -15, -19, 1, 8, 7, -16, -13, -6, -26, 2, -21, -54, 15, 5, 6, -5, -3, 5, 25, -9, 2, 2, -6, 37, 38, 28, 12, 44, -4, 7, -7, 17, 21, 17, 17, -10, 2, -8, 12, 19, -3, 20, 30, 1, -22, -8, 12, 18, -9, -3, -4, 18, 7, -7, 20, 27, 21, 17, 4, 21, 15, -8, -7, 29, 25, -5, 5, 9, 10, 20, -11, 9, 8, 0, -17, 6, -12, -21, 24, 18, 33, -6, -4, -1, 7, 1, 7, -37, -50, -10, -5, -19, 3, -17, -12, 1, 1, -7, -2, -26, -4, 13, -24, 2, -20, 2, -4, 3, 6, 16, -11, -14, -12, -6, -8, -25, 13, 9, 19, -4, -12, -10, -25, -6, 1, -16, 5, -19, -6, -36, -21, 1, 9, 6, -5, -5, -14, 14, -6, 6, 2, -4, -26, -32, 2, -2, -2, -10, -9, -4, 39, -17, -22, 30, 6, 5, 13, 19, 7, 22, -24, 20, 5, -3, -3, 7, 24, 6, 21, 5, 6, 14, -9, -18, 20, -43, -36, 22, -7, 15, 26, 25, 1, 22, 27, 4, 14, 16, 2, -6, -1, 15, -7, 15, 5, 15, 9, 15, 5, -2, 35, 3, 34, 19, 2, -51, -4, 3, -11, 6, 0, 18, -3, 10, 9, 0, -1, 12, 5, -15, 13, -22, -6, 9, -9, -11, -1, -2, -9, 11, -4, -34, 0, 10, -24, 16, -11, -35, -6, -15, -16, 4, 25, -20, -32, 0, -9, 1, -7, 9, -25, 2, -5, -2, 13, -11, 2, -16, 17, 12, 10, 8, 7, -19, -8, 2, 5, -14, 17, 10, -9, -19, 4, -24, -19, -8, -3, -46, -22, -23, -15, -6, 1, -42, 3, -18, 18, 3, 4, 2, -14, 14, 9, -4, 9, -37, 6, 0, -8, -26, 4, 5, 11, -26, -2, 18, 2, -4, 5, 2, 0, 7, -13, 16, 13, 0, 2, 11, -18, 5, 14, 4, -5, -5, 10, -11, -9, 2, 16, -3, 6, -19, 4, 3, -1, -2, 19, 1, 6, -10, -8, 3, -19, 0, -1, -2, 15, -3, -1, -23, -31, 6, 9, 6, 3, -27, -6, -15, -12, -14, 5, -7, 20, -43, 12, 10, 4, 8, 7, -27, -41, 6, -27, -23, -23, -5, -19, -2, -17, -38, -6, -8, -19, -6, 6, -19, -28, -14, 10, -36, -2, -37, 8, -5, -31, -28, -14, 9, -27, 1, -14, -4, -14, -8, 13, 4, -11, -7, -18, -1, -17, -5, -8, 2, 7, 2, -10, 13, -8, 4, 10, 5, 0, -10, -6, -3, -4, 16, 4, 6, 19, -34, -2, -2, -3, -11, 14, -11, -10, -24, 5, 0, 4, 1, 17, -13, -5, -10, -9, -7, 13, 3, 7, -5, -3, 7, 6, 1, -15, -9, -2, -8, 3, -18, -34, -5, 2, -8, -6, 5, 5, 0, 12, -17, 6, -16, 12, 15, -7, -18, 12, 8, 5, -24, -4, -6, 11, 4, 7, -1, -22, -1, -14, -10, 10, 11, -3, -16, 23, 8, 15, 13, 3, 8, 1, -9, -10, -10, 14, -12, -8, -2, 10, 6, 14, -12, -4, -3, 1, 13, -5, 5, -17, 9, 3, -3, 5, 6, -20, -13, -8, -2, -27, -38, 10, -8, 3, -9, -19, 11, 4, -1, -33, -28, -7, -4, -8, 9, -9, 7, -2, 3, -31, 8, -12, -26, -15, 10, 30, -17, -17, 10, -8, -8, 2, 7, 10, 1, 11, -3, -1, -4, -5, 17, 4, 6, -5, -8, 9, 21, -10, 15, -2, -7, -11, -4, -12, 7, -29, -3, -10, -11, 0, 27, -11, -3, -6, -8, -4, -30, -20, 5, -19, -13, 3, -12, -1, 7, -4, -5, -4, 9, 3, 12, -8, -1, 0, 15, 12, 6, 15, 11, -4, -11, 13, -11, -20, -16, -9, -25, -23, -16, 3, -16, 5, 3, 16, 0, -8, 3, -17, 4, 19, -9, 5, 12, -17, -2, 11, 12, 19, 2, 30, -19, -3, -16, 2, -4, -5, 5, -18, 3, -7, -7, -1, 2, 12, -8, 13, 10, 9, 3, -5, -3, -15, 0, 7, 14, 3, 9, 6, 16, 7, 2, 4, 5, 6, 16, 7, 5, 23, 5, -3, -11, 29, -9, -7, 4, 1, -8, 11, 9, -14, 16, 6, 2, -28, -10, 5, -3, 15, 38, 28, 12, -13, 2, 24, 13, 2, -8}
, {-12, 0, 13, 14, -1, 16, 15, 4, -10, 13, 0, -15, -2, 11, 5, 12, -10, 9, 8, -9, -8, 6, 17, -7, 6, -4, -7, 1, -9, -18, -1, -3, -3, 13, 18, 23, 8, 21, -3, 9, 4, -30, -22, -6, -7, 1, -11, -8, 11, 8, -10, -16, -8, 22, -8, -5, -6, 1, -6, 19, -3, -3, -4, 16, 0, -18, -4, 7, 7, 4, -8, -3, -28, -7, -9, -19, -8, 8, -9, -5, -3, -5, -20, -27, 4, -5, -4, -14, -6, -2, -9, 17, -13, 3, -17, 5, 0, -3, -14, -12, -1, -8, -9, 0, -4, -19, -15, -13, 9, -15, -6, -21, -6, 0, -18, -14, 3, -6, 3, 14, -12, 1, -18, 9, -3, -16, -6, -19, -1, -16, 14, -11, 10, -6, -9, -10, 6, 14, -10, 2, 8, 0, -5, 3, -4, -14, 2, -17, 0, -12, -30, -14, -2, 3, -7, 5, -10, -8, 1, -14, 0, -1, 4, 28, -5, -10, -6, 0, 8, 2, -12, 9, 9, 20, 3, -3, 41, 2, -4, -10, 0, -2, -7, 13, 17, -8, -8, 4, -5, 17, 2, 34, -3, -9, 5, 4, -3, 3, -10, -3, -4, -20, -3, 0, -3, -12, -1, -10, -3, -6, -8, 11, 5, 5, -7, -13, 8, -5, 3, -8, 7, 13, 3, -7, 0, -9, -7, -5, -13, -14, -11, -10, -22, -6, -32, 0, -8, -13, -4, -14, -9, -21, -7, 1, 5, 0, 14, -16, -7, -3, -6, -9, 1, 1, -12, -7, -15, 2, 3, -10, -12, -12, -10, -15, -30, -6, -11, -8, -5, -7, -1, -13, -2, 3, -30, -14, 1, -4, 3, 13, -27, 7, -11, -21, -9, -4, -6, -15, -16, 0, -6, -22, -11, -18, -17, 0, -6, -8, -1, 19, -7, -1, -6, -7, -6, 2, -2, 5, -2, -1, 4, -9, -20, -10, 8, -13, 19, -22, -1, 7, 13, 16, 32, 23, -18, 20, 4, 31, 26, -17, 10, 34, 5, 17, 33, -11, 16, 29, 29, 8, 0, 13, 26, 20, -5, 4, 7, 7, 14, 22, 7, 18, -2, 16, 12, 4, -15, 4, 2, 17, 4, -11, -16, 4, -8, -3, -3, -13, 7, -5, 10, 1, 7, 4, 5, -1, -5, -2, -15, 8, 5, -4, -20, 22, 2, -20, -8, -11, -4, -9, 9, -8, -13, -13, -3, -3, -2, -4, 11, -5, -19, -2, -21, -10, -5, -7, -3, -12, -5, 7, 3, -8, 11, -11, -10, -1, 0, -12, 0, -11, -19, -1, -12, -17, -2, -19, -10, -11, -3, -6, 9, -12, -8, -13, -17, 5, -9, -16, -29, -22, 4, 9, -26, 3, 5, -8, -13, -4, -7, -13, 21, -7, 13, -1, 0, 6, -5, -20, 30, 19, -5, 7, 7, 0, -1, 13, 11, -4, -5, 0, -25, -12, 10, -7, -2, 8, 14, -8, -6, 3, -4, 25, 29, 38, 21, 27, 8, 28, 18, -1, 24, 3, -6, 37, 10, 17, 14, 15, 36, -2, 9, 2, -5, 15, 30, -10, 26, 18, 23, -16, 27, 35, -27, 0, -3, 6, -1, -4, -6, 6, -2, -20, -8, -13, -3, -8, -8, 5, -3, -19, -19, -18, 0, 20, -10, -7, 7, 5, -8, 3, 1, -9, -21, -6, -3, -6, 8, -20, -4, -7, -37, -18, -23, 5, -7, -24, -7, 3, -17, 0, 18, 2, -5, -6, 8, -12, -10, -17, -16, 0, -2, -17, 8, -3, -14, 11, -20, 2, 4, -2, -3, -12, -2, -19, -3, -3, -12, 2, -1, -13, 0, -13, -3, 5, 16, -16, -1, -21, -10, -25, -6, 6, -1, -10, 5, -13, -10, -20, -9, -7, 29, 1, -12, 6, -3, -26, -13, 9, 24, -7, 0, 10, -3, 10, -18, 16, 2, -4, -7, -5, 10, -22, -5, -5, 10, -8, 5, -6, -16, -7, -4, 8, 26, 25, -1, 11, -15, 17, -5, -8, 11, -1, -8, 13, -8, 14, 20, -27, 7, -13, 8, 10, -19, 14, 31, -7, 2, 4, 19, -9, -4, 36, -14, -23, -11, 3, 6, -10, -12, -14, -26, -26, -7, -19, -8, -8, -6, -10, -3, -1, -10, -19, -10, -10, -13, -16, -6, -4, -26, -19, -1, 2, -10, -12, -14, -18, 6, 9, -6, -1, 17, -27, -6, -19, -10, -18, -8, 16, 1, -3, -14, -21, -27, -15, 6, -21, -3, -9, -9, -6, -22, 1, -16, -17, -18, -23, -12, -3, 4, -4, 8, 14, -4, -17, -2, 11, 2, -12, -1, 10, 7, -2, -15, -4, 14, -22, -3, -14, -1, 1, 6, 6, -16, 7, -16, -20, 2, -2, 1, -1, -2, 18, -4, 9, -2, -23, -28, 1, 21, -3, 8, 1, -12, 15, -19, -1, 0, -27, -6, -5, -8, -11, 8, -4, -27, 10, -23, -26, -7, -19}
, {-7, 17, 29, -24, 8, 0, 15, -14, -15, 23, 16, -12, -5, 6, -9, 8, -12, 15, 16, -23, -6, -11, 9, 0, -2, -4, 1, 25, 9, 9, -1, -24, 14, 15, 12, 15, 8, 11, -30, 20, 11, -3, 0, 24, 3, 0, -24, 32, 3, -6, 10, -1, -7, 5, -11, 3, 60, 13, 19, -15, -13, 4, -4, 17, -5, -33, -2, 10, -8, -1, -10, 27, -4, 5, 7, 25, 2, -1, -22, 11, 21, 5, -5, -17, -6, 12, 6, 9, 2, -3, -7, -12, -6, 8, 14, 16, -28, -8, -7, 5, -13, -1, 30, -16, -7, -25, -10, -27, -1, 5, 19, -2, 16, -21, -25, 6, -2, 22, 3, 7, -21, 8, -10, 26, 12, 0, -11, 16, 4, -16, -5, 18, -5, -2, -11, -15, -10, 3, 5, -34, -2, 4, 8, -7, 2, -1, -30, -9, -4, 14, 12, -19, 5, 6, 0, 28, 5, 7, -1, -6, 6, -2, -27, 9, -9, 1, -3, -30, -17, 1, -14, -24, -10, -1, -2, -12, -5, -2, -20, -20, 1, -31, 2, -11, 3, 10, -9, 8, -1, 9, -23, -6, -14, -3, -13, 12, -15, 2, -3, 9, 0, -10, -3, 7, -7, 9, -28, -1, 7, 18, 9, -23, 3, -21, 0, -4, 16, 4, -23, 4, -17, -16, 22, 16, 4, 2, 11, 6, 6, 17, 20, 7, 4, 12, 16, -9, 2, 8, 13, 13, 11, 6, 13, -12, 1, 15, -3, -4, 9, -7, 7, 19, 22, -5, 5, 4, 23, -13, -19, 3, 20, 1, -9, 16, 18, 0, 7, 3, -4, -24, 6, 17, 15, -18, -9, 8, -5, 4, -28, -29, -3, 1, 15, 20, 27, 14, -12, 22, 26, -15, -19, -17, 17, -13, -2, 3, 15, -6, -18, -13, -7, -36, 27, 2, -19, -11, -24, 21, 5, 8, 19, -38, -6, -3, -24, 12, 6, 19, -20, -7, -21, 8, 5, -11, -21, -10, 10, -5, -10, 6, -12, -19, -7, -25, -17, -9, -27, 19, 1, -14, -1, 12, 10, -20, -10, -4, -3, -14, 6, -33, 26, -36, -14, 20, 23, 11, 17, 23, -5, -14, -27, -10, 4, -26, -11, 11, -48, -2, -15, -11, 2, -14, 9, 2, -28, -18, -5, -3, -6, 14, -25, -29, -15, -2, 4, -8, 8, 5, 7, 1, -7, 6, 0, 12, 9, -24, -7, 4, -12, -3, -8, 4, -5, -31, -1, -6, -13, -27, -3, -4, -22, -10, -1, -17, -13, -15, -13, -10, 34, -12, -6, -10, -17, 0, 8, -5, 17, 3, -3, 8, 19, 6, -9, 24, 32, 16, -1, 0, 12, -8, 12, -10, -13, -31, 20, 11, 22, 11, -1, 5, 13, 12, -17, -12, -1, -1, 5, -4, -3, 3, 1, 11, 1, 12, -8, 17, 29, 0, -4, -16, 17, -2, -1, 14, -9, -11, -7, -2, 18, 8, -17, -9, -6, -14, 8, 1, -6, 4, 1, 0, -37, 10, 2, 21, -16, -4, -8, -5, -23, -24, 0, -11, -17, 1, -19, -6, -5, -2, -9, -7, -18, -15, 4, -34, -15, 10, -24, -26, -46, -13, -16, -19, -26, -41, 11, -27, -8, -36, -4, -24, -10, -27, 5, -24, 4, -54, 16, 12, -35, -19, 6, -1, -16, 9, -16, -20, 2, -9, -25, -21, -13, -5, 1, 10, -15, -1, -2, 0, 1, -17, -8, 11, -2, -28, -1, -19, 10, 4, -8, 2, -18, -30, -15, 8, 10, 8, -7, -2, 3, -1, 9, 9, 2, 12, 1, 2, 13, -1, -2, 18, 2, 11, -11, -8, 22, -8, -7, 0, 2, 25, 19, 1, 11, 15, -9, -3, 3, 2, 7, -12, -5, 8, -46, -19, -19, -5, -1, 8, -15, -21, -7, 3, 1, -11, 13, 8, -5, -7, 0, -15, 4, 0, 2, -6, -18, -6, -14, 16, 5, 1, -25, 0, 20, -10, -9, 7, 1, 19, 3, -25, -2, -8, -2, -12, -5, 4, -19, -16, 14, 3, -2, 12, -14, -14, -14, -4, -1, -19, -14, -29, -11, -7, -15, 21, 17, -21, -16, -13, 29, -22, -12, 27, 26, -21, 4, -2, 19, -13, -5, -13, 36, -17, 2, 37, 11, -24, -16, -7, 18, -25, 28, -21, -31, -7, -1, -7, 2, -14, -24, -12, 7, -23, -7, 23, 14, -22, -15, 5, 5, -4, -10, -25, -9, 8, -2, 31, -13, -26, -3, 14, 31, -10, 59, -18, -11, -14, 12, 9, -2, -24, 0, 8, 18, -4, -10, 10, 7, 10, 10, 10, -5, 13, 2, -19, -11, -17, -2, 10, 2, -28, -7, 1, 0, 6, -5, -7, -16, 5, 12, 21, 6, 6, 11, -6, 32, 4, 0, 2, -7, 14, 9, 10, 10, -9, -20, -2, -9, -4, 3, 20, 16, 12, 17, 6, -9, 17, -1, 13, 7, -16}
, {-15, 11, 16, -6, -7, -4, 8, 2, -12, -20, -4, -1, -3, 4, 10, -16, 12, 4, 9, -7, 0, -2, 8, -16, 15, -7, -12, -20, -3, 11, -6, 5, -14, 4, 13, -3, -7, -2, -7, 1, -10, -25, -4, -16, 6, 12, -4, -10, -12, 1, 7, 1, 8, -11, 1, -4, 10, 2, -11, -1, 8, -2, -4, -1, -12, -8, -6, -5, -19, -10, -14, 0, -13, 1, -16, -3, 2, 4, -12, 7, 3, 3, -11, 0, -10, -30, -26, 1, -1, -11, 0, -2, -9, -25, -1, 12, -12, -15, -8, -16, -7, -20, -19, -13, -19, 3, -14, -2, 3, -5, -8, -4, -14, -22, -5, -15, 3, -18, -12, -1, -9, -8, -23, -10, -9, -4, -7, -19, 2, -3, -4, -16, 7, -16, -25, -2, -11, -3, -9, -3, -5, 3, -23, -18, -2, 7, 14, -5, 4, -24, -3, 3, 7, -12, -16, -13, -5, -14, 7, 0, -3, -14, -2, 11, -4, 10, 6, -4, -20, 9, 14, -5, -3, 3, -10, -4, -1, 5, 10, 4, -9, -4, 27, -1, 12, 4, 3, 6, 2, 5, 18, -13, -9, 10, 12, 14, 4, -8, -16, -2, -8, -14, -1, -15, 7, -5, -9, 4, -9, -16, 8, -9, -1, -4, 5, 2, -1, 3, 2, 1, -4, -18, -2, -25, -16, -2, 3, -6, -21, 4, -24, -15, -4, -21, -21, -3, 0, -1, -1, -27, -6, -12, -12, -18, -8, -8, -11, 8, -23, -1, 4, -4, -12, 5, 3, -1, -2, -11, -1, -11, -29, -2, 2, -3, 2, -8, -2, 1, -3, -6, 11, -27, 4, 6, 3, -5, 7, 1, 10, 1, -10, 5, 4, -11, -9, -5, 1, -1, -4, 7, -3, 5, -8, 10, 3, -8, -3, -3, 3, -9, -6, 5, 0, 5, -12, 17, 5, -7, -5, -4, 6, 7, 6, -12, -1, -3, -20, -2, 8, -7, -20, -10, 11, 4, 5, -8, -3, -9, -22, 1, -18, -5, -8, 3, -12, 6, -9, -8, -2, 0, -7, -6, -15, 1, -5, -5, -4, 12, 1, -15, -9, -9, -5, -12, -11, -9, -5, -19, -23, -1, -4, 1, 3, -8, -4, 8, -8, -1, 10, 3, -9, -4, -2, -11, 2, 4, -3, 1, -1, -19, -13, -9, -14, -8, -10, 6, 2, -7, -25, 1, 5, 0, 1, 1, -1, -4, 0, -9, 7, -8, 7, 5, 12, 3, -8, -9, 13, -3, -8, -2, -13, -21, -3, 1, 7, -4, -7, 3, 10, 10, 2, -9, 7, -14, -14, 12, -3, -12, -7, -7, 12, -7, -4, 4, -12, 1, -4, -13, 13, -5, -1, -10, -2, -6, 11, 7, 1, 2, -9, -5, 0, -7, 2, 0, -4, -8, 2, -17, 2, 10, 8, -7, -14, 0, 1, -21, -10, 3, -2, -12, 13, -20, 8, -14, -5, 10, 0, -17, -21, 7, -10, -3, 23, -11, -6, -5, -2, -16, 1, 3, -9, -10, 7, 0, -14, -10, 12, -2, -11, -4, 6, -6, 12, -9, -4, -15, -7, 3, -11, -2, 6, 11, -7, 7, 8, -7, -5, 11, 9, -8, -14, -4, -19, -6, -7, 6, -9, -8, 2, 3, 10, -12, 5, -18, 10, 4, -14, 1, 2, 2, -24, -23, 2, -19, -12, -5, 6, 3, -1, -4, 0, -10, 1, 5, -11, -14, -2, -5, -1, 8, -4, -13, 7, 1, 5, -9, -9, 5, -2, -3, -1, 8, -11, -15, -10, -15, -11, -4, -25, -2, 8, 0, 2, -8, -7, 7, 18, -17, -8, -5, -14, 11, -13, -1, -9, -4, -2, -13, -12, -2, 7, 6, -5, -4, -14, -13, -12, -10, 5, -8, -15, -5, 17, -6, -17, -1, -14, -8, -10, -7, 6, -25, -16, -6, 9, -2, -9, 11, -11, 7, -13, -12, -9, 0, -3, -3, -16, 0, -8, 5, -8, 5, 4, 4, 9, 6, 4, -17, -12, -2, 4, 4, 1, 6, 10, 11, -17, 6, 9, -3, 7, 0, -1, -7, -3, -11, -19, 3, -9, 0, -11, 10, -12, -13, 3, 2, -18, -2, 3, -7, -16, -22, -19, 1, 2, -10, -24, 2, -11, -1, 2, -8, -4, -9, -24, -8, 8, 3, 8, 2, -14, -12, -10, -13, -5, -4, -16, 4, -12, -1, -20, -9, -14, -9, -14, -13, 9, -19, -22, -15, -4, -10, -14, 1, -11, -11, -10, 2, -14, -4, 14, -19, -21, -13, 5, -6, -1, 19, 28, -15, -3, 0, -8, -7, 11, 3, 4, -8, 3, -3, 7, 0, 26, -2, 3, 1, 0, 6, 32, -10, -15, 3, 3, 5, 23, 27, -17, 15, 20, 8, -4, -3, 2, -11, 22, -9, 5, -21, 0, 1, 5, -6, 12, -23, 24, 5, -2, 9, -3, -6, 19, 11, -7, -5, 10, -14, 18, 36, 7, -19}
, {3, 30, 46, -12, 41, 29, 20, 5, 13, -1, 7, 7, -4, 52, 10, 33, 5, 29, 12, -1, 9, -4, 4, 30, 11, -13, 22, 38, -14, -6, 15, 8, -30, -13, 23, -3, -2, -6, -12, 22, 33, -2, 6, -4, 6, 24, 6, 7, 33, -7, -4, -6, 7, 12, -9, 19, 12, -1, 28, 18, 13, -1, 17, 30, -4, -23, -3, -6, -8, -30, -13, -13, 6, 13, 10, -18, 2, -1, -5, -9, 10, -9, -4, 16, -10, 11, 1, -17, -10, 2, 0, 4, 18, 7, -3, 5, 16, -24, -9, -13, 3, -6, -2, -5, 2, 8, 14, -26, 4, 6, 11, 18, 9, -5, -10, 24, 4, 21, 8, -16, 29, 7, 1, 4, 19, 10, 11, 8, 13, 28, 18, -2, -6, 16, -1, -9, -8, -5, 13, -7, -5, 23, 10, -3, -4, 35, -2, 0, 3, -3, 20, 30, -3, -8, 14, 10, 2, 8, 26, -8, -3, 1, 3, -10, 4, 13, 4, -32, -30, -6, 11, -27, -3, 5, -7, 7, -19, 7, 8, -13, -8, 10, 0, 17, -1, -9, -15, -7, -14, -18, -4, 29, -29, 9, 25, 3, -4, 6, 0, -4, -10, 4, 6, -40, -1, 5, 0, 11, -1, 14, -2, -5, 2, 9, 9, -1, -5, 10, 5, 5, -11, -25, -15, 1, -10, -8, 8, 10, 15, -9, 13, -18, -15, -6, 21, 14, -2, 8, -31, -2, 1, -8, -6, 9, -7, 23, 6, -21, 10, -2, 0, -2, 18, -15, -6, -10, -11, 7, 9, -2, 19, 9, 33, -13, 2, -8, 14, 3, -6, 7, -3, -2, -3, -5, 0, 6, 8, -2, 15, 15, 19, -11, -1, 7, -3, -6, -11, -14, 12, -5, 12, -2, 10, 6, 13, 7, 2, -4, -7, 15, 3, 2, 19, 7, 14, 1, -6, 1, -4, 3, 35, -8, 6, 4, -13, -9, 12, 35, 13, 8, 9, -30, 6, 8, 0, 4, -12, -42, -22, -4, -22, -23, -13, 8, -37, -7, 21, -23, -23, -44, -7, -25, -42, -32, 5, -4, -11, -3, -6, 7, -21, 5, 13, -26, -2, -7, -3, 7, 23, -13, -31, -2, 16, -18, 0, -5, -47, 3, 1, -23, -26, -35, 8, 8, -15, -31, 10, 5, 4, 7, -12, -3, -29, 13, 9, -18, 5, -3, -3, -19, -3, -21, 1, 5, 12, -12, 5, 6, 3, -2, -10, -25, -9, -19, -1, 14, 2, 11, 8, -7, -11, 4, 16, 7, -13, -25, 5, -10, -7, -3, -3, -24, -2, 10, 6, 21, 7, 28, -10, 0, 2, 4, -2, -14, 2, -7, 1, -3, 8, -4, 7, 2, -2, -25, 11, -14, 21, -5, -3, 29, 27, 5, 0, 23, 7, 13, 6, 1, -4, 21, -6, 14, -7, 13, 12, 27, 10, -10, 1, -23, 22, 44, 2, -4, 12, 9, -3, -11, 29, 26, 5, -8, -24, -10, -12, -17, -15, -22, 3, -35, -29, -9, -8, -19, 18, -51, 23, -46, -46, 26, -9, -8, -23, -33, -10, 7, -8, -6, 15, -1, -24, 14, 34, -18, -21, -22, -8, -47, -5, -10, -18, 11, -6, -35, 10, -34, 17, -30, -3, -58, -20, 27, 6, 14, -17, -40, -30, -8, -18, -7, 28, 9, -42, -23, 16, -35, -1, -40, 10, -30, 27, -8, 1, 8, 16, -14, -5, -23, 6, 2, -21, -3, 16, -9, -2, -2, -7, -14, -19, 1, -9, -15, 7, 18, -19, -20, 3, 21, 25, -2, 5, -9, 18, 7, 28, 17, 9, 31, 10, 4, 1, -3, 8, 24, 12, -20, -6, -25, -1, 30, 0, 3, 25, -18, -11, -11, 23, 10, -5, 16, 11, 7, -21, 19, 9, 29, 27, 3, 13, 14, -7, 3, -5, 5, 11, 8, 8, -18, 9, -4, -8, 29, 18, 4, -10, 10, -14, 5, 5, 14, 7, 12, -1, 0, -10, 17, -11, 7, 2, -8, -18, 4, -9, -9, -14, 1, -4, -7, -8, 2, 3, 7, -10, -11, 1, -7, -11, 12, 14, -16, 2, -4, 14, 17, -3, 0, -6, 15, 13, -31, -19, 33, -11, -23, 6, -20, -4, -15, -12, -17, 13, 7, -10, 20, -14, -47, 14, 3, 1, 14, 20, -2, -36, -29, 19, 10, 20, -18, -12, -4, 16, -46, -14, 21, 9, -29, -8, -1, -8, 8, -34, -17, 15, -11, -6, 7, -12, -40, 13, 2, 33, -9, 8, -11, -27, -48, -8, -2, 13, -15, -3, 14, 15, 4, 6, -1, -1, 24, 7, 6, -1, -5, -16, 13, 9, -2, -10, -1, -15, -18, 12, -14, -4, 8, -11, -9, 11, 1, 16, 16, -21, 25, -3, 7, -9, 15, -4, -5, -39, 8, 6, -16, 6, -14, 9, 19, -12, 12, 2, -11, -2, 26, 24, -6, -3, 28, -16, -26, 6, 1}
, {2, 13, 5, -10, 14, 16, 5, -1, -4, -15, 3, -8, -2, 11, -3, -8, -4, 12, 21, -13, 2, 5, 1, -2, 4, -15, -4, 12, 10, 4, -4, 0, -26, -9, 7, -3, -6, -4, 17, -15, -6, -16, 3, -4, 5, -11, 0, -17, -17, 9, -23, -15, -4, -22, 3, 3, -23, 3, -16, -15, -16, -21, 3, -23, 5, 20, 23, -14, 2, 8, 15, -9, 27, 26, 17, -7, -4, 27, 20, 16, -23, 26, 29, 2, -6, 22, 23, 17, -10, -8, -4, 0, 8, 13, 4, 10, 19, -18, -8, 20, 6, -20, 9, 0, 6, -30, -2, -6, 6, -23, 23, 14, 22, -26, -37, 11, -4, 17, -9, -20, 19, 1, 0, -5, 14, 6, -20, 24, -7, 19, -7, -11, -3, -13, 12, -4, -7, -18, 15, 13, -8, 4, 17, -17, -1, -14, 1, 18, -12, 19, 6, 14, -1, -14, 7, -18, 18, 22, -3, -6, 16, 3, -19, -9, 0, 4, -2, -15, 1, -10, 2, 1, -1, 16, 21, 0, -11, -11, -3, 0, -1, -8, 3, 7, 5, -13, -11, -4, 13, 3, 4, -18, -29, 9, -8, -12, -27, -18, -21, 3, -4, -9, -23, -5, -1, -17, 20, -27, -24, 6, -16, 1, -1, -17, -8, 22, -21, -4, -1, -23, -28, 3, 10, -2, 9, 11, 2, -3, 20, 9, 17, 13, 17, 18, 18, 25, -10, 6, 3, 16, -6, 17, 26, 0, 7, -42, 15, 27, 6, -14, -15, 9, 4, 1, 25, 6, 18, 0, 14, 21, 14, -15, 3, 9, 0, 20, 10, 26, -2, 4, -10, 10, -24, -4, 4, -5, -9, -16, -9, -6, 11, 5, -15, 27, -3, -2, -3, -15, -14, 2, -3, 6, -9, 8, 16, 20, 3, 20, 13, 1, 5, 18, 3, 3, 3, -8, 13, 6, -8, 2, -10, 14, 12, -1, -6, 22, 0, -33, -9, 17, -4, -4, -16, -1, -23, -8, -13, 0, -27, -9, 2, -9, -12, -21, -22, -22, -22, -16, -9, -4, 4, 1, -22, -16, -4, -13, -15, -18, -5, -18, -3, -22, -23, -24, -24, -14, -12, -10, -43, 12, 7, -1, -5, 18, 6, -13, -18, -17, -8, 25, -2, 2, -1, -7, -7, 14, -27, 0, -6, -35, -20, -26, 11, 19, -24, -1, 1, -2, -31, 5, -4, 10, 9, -30, -13, 9, 6, -3, -1, -26, -3, 10, 12, 13, 1, -2, -9, 15, -11, -12, -5, -13, -9, -10, 8, 21, -3, -4, -8, -15, -13, -7, 5, 8, 4, 1, -9, -11, -5, -20, 1, 9, 11, 15, 1, 23, -10, 9, 19, 20, 5, 6, 5, -20, 17, 2, 6, -17, 0, 14, 31, -8, 1, -6, -18, -3, -9, -4, -15, 16, 1, -7, -40, 7, 9, 0, 14, -19, -7, 5, 8, -1, 5, -11, 14, 19, 24, -1, -9, 20, -11, 7, -22, 2, -12, 6, -2, -11, 2, 1, 10, -34, 6, -14, 7, -18, -13, 2, -15, 2, -10, -5, -4, -15, 6, 2, 6, -6, 4, -4, -30, -23, 6, 14, -5, -26, -13, 26, -8, 9, 27, 2, -10, 1, 3, 19, -8, -9, -4, 14, 4, -11, -1, 10, -13, 12, -19, -3, 5, -8, -7, -15, 12, 20, -3, -10, -4, -4, 3, -5, 0, 0, -13, -20, 1, -4, -3, -6, -31, 9, -23, -37, -18, -2, -2, -23, -29, -2, -5, -2, 7, -4, -36, -32, -26, -7, 14, -1, 3, -23, 4, 1, 31, 10, 6, 5, 5, 17, 9, 0, -6, 9, -12, 34, 4, 6, -3, 5, -16, 16, -8, 3, 10, -19, -6, -3, 20, -19, -19, 10, -10, 11, -11, 12, 5, -11, -20, -11, -3, -11, 4, 2, -1, -1, 26, -11, -1, -23, -6, 14, -15, -15, 10, -3, -1, 14, 20, -14, -21, 6, 4, 5, 1, -2, -12, -2, -6, -9, -4, 23, 10, -26, 4, -6, 7, -31, -1, 11, -10, 2, 0, 1, 5, -1, -3, -8, -9, -8, 11, 10, 19, -3, -16, 2, -1, 22, 13, -2, -5, 10, 32, 16, 0, 26, 13, -15, -1, -3, 18, -25, 0, -14, 1, 10, -17, 3, -17, 3, -2, -18, 0, -13, -4, 10, -15, -15, -44, -4, -37, -33, -7, -8, -12, -28, -34, -2, 6, -39, -13, -36, -25, 0, -31, -3, 3, 0, -23, -11, -30, 6, -17, -20, -3, -20, 9, -3, -15, 10, -2, -20, -10, 3, 6, 6, -8, -1, -6, -11, -6, -21, -40, -7, -8, 26, 0, -27, -8, -3, -22, 25, -12, -7, 0, -1, -7, -18, 21, -8, 2, -26, 14, 14, 16, -2, 13, -14, 0, 8, 35, -16, 2, 21, 8, 16, 4, -9, 35, -30, -8, -13, 18, -14, 3, -10, -12, 9, 43, 20, -22, 14}
, {8, -2, 3, -5, -2, 8, -12, 3, -3, 1, -2, 0, -5, 5, 4, -7, -8, -2, 1, -13, -6, 5, -13, -7, 4, 5, 5, -2, 8, 3, -10, 5, -5, -10, -1, 0, -9, 10, -1, -3, 4, -12, -10, 3, 4, -12, 1, -2, 7, -9, 3, 1, 0, 6, 7, 2, -2, 2, -7, -11, -2, -7, 5, 5, -9, 2, -9, 8, 5, -1, -13, 6, -6, 0, -7, 2, -1, -1, 4, -14, -10, -6, 0, -10, -3, -7, 0, -13, 5, -8, -5, -7, 5, 0, -4, -1, 8, -13, 4, -1, 7, -3, -6, 1, -4, 2, 8, -6, -9, -8, 3, -10, 5, 8, -6, -1, 3, -14, -4, 5, 3, 2, -4, 5, -9, -6, 0, -7, -11, 3, -7, 1, 6, -5, 2, -6, 5, 0, 4, 7, -7, -5, 4, -1, 8, -10, 3, -7, -10, -2, 6, 5, 6, -4, 5, -11, 0, 4, 4, 7, 7, 9, -2, -13, -5, 5, 0, 1, -4, -9, -10, -3, -5, -4, 1, -5, -8, 6, 3, 9, -11, -4, -9, -4, -3, 0, 0, -6, -7, -5, -6, 2, 6, 0, 2, -5, 1, -10, 3, 2, -3, 6, -12, -8, 6, 7, 3, -8, -7, 3, -7, -10, 3, 7, -9, 2, -6, 2, 6, -13, 6, -5, -5, 6, -3, -8, -6, -8, 3, 2, -14, -5, -7, -11, -9, 8, -10, 5, -5, 6, -1, -2, -5, -12, 1, 5, -3, 8, -7, 3, 5, 7, -6, 6, -12, -10, -4, -1, -3, -5, -9, 5, -5, -10, 1, 3, 6, 0, -5, -6, -8, -8, -5, -2, -9, 3, 7, 1, -1, -7, 4, -4, -9, 6, -4, 2, 4, 7, -7, -4, -4, -2, -6, 8, 2, 6, 0, 8, -8, 1, -9, 6, 3, -10, 7, 4, -9, 9, 5, -4, 4, 7, -13, -1, 10, -12, -9, -12, 8, 5, -8, 6, -11, 5, -3, -8, -8, 0, -6, -1, 5, 6, -11, -7, 7, -5, -7, -5, -6, 3, 2, -7, -8, -6, 1, -2, 8, 6, -3, 8, -5, -5, -9, -3, 5, -3, -3, -5, -8, -10, 6, -1, -8, -7, 10, 1, 3, -9, 5, 4, -12, -8, -11, 4, 7, 3, 2, -3, 2, 1, 6, -9, -3, 9, 6, -3, 8, -4, -7, -7, -11, -12, -7, -8, -7, -3, 2, -8, -6, 2, -6, 2, -5, -1, 0, 2, -1, 2, -8, 8, 7, 6, -4, -4, -12, 5, -7, 7, 4, -1, 2, -5, -9, -4, 7, 0, -1, 8, 3, 2, -1, 2, -4, 8, -8, 3, -9, -6, -11, -4, -1, -5, 3, -5, 4, -3, -1, 6, -4, -7, 4, -9, -3, -11, -15, -8, 7, -8, 8, 3, 8, -5, 6, -1, -11, -7, 1, 8, -5, -4, -9, -8, 0, -6, 2, 3, -7, -2, 8, 4, 4, -10, -9, -10, -4, -10, -8, 8, 3, -7, -7, -8, 4, 1, 7, 4, -6, -2, 2, -5, -11, 0, 7, 8, -10, 2, 0, 6, -8, -9, -2, -10, 4, 7, -9, 6, -7, 4, -5, -11, -1, 1, -12, 6, 1, 7, -2, -9, 2, -2, -6, -7, 5, 4, 4, -4, 5, -2, 6, -5, -10, 5, 1, -7, -3, 4, -9, 2, 6, -3, -6, 1, 2, 5, 0, 7, -9, -8, 7, -7, -3, 8, 5, -6, 0, -5, 2, 6, -2, -2, -2, 1, -9, -5, -10, -7, -3, -7, -6, -10, -4, -8, -4, 2, 3, -4, -9, 7, -5, -10, -10, -8, -5, -6, 1, -2, -5, 6, 0, 0, -5, -5, 6, 3, -8, 6, 2, -8, -1, -6, -7, 5, 1, -8, -5, -5, 7, -8, 6, 7, 2, 2, 7, 9, 2, -10, -2, 8, 8, -6, 0, -6, -3, 1, 8, 6, 5, -2, 4, 5, 5, -11, -6, 7, -2, -9, 2, -3, 3, 7, -4, -11, -9, -7, -3, -11, -8, 2, -13, 1, -3, -7, 5, 8, 2, 4, -6, 6, 1, -6, -9, 0, -9, -5, -8, -2, -7, -6, -5, 0, -6, 3, -10, -9, -10, -2, -1, 5, -11, -4, -12, 6, 8, -8, -8, 4, -6, -10, -10, 1, -3, -6, -1, 2, 6, -8, -1, -9, -7, -10, 0, 4, -12, -6, -8, 4, 5, -2, 5, -1, 0, 0, 0, 5, -9, -9, 7, 4, 4, 6, 4, -10, -7, -12, -11, -12, 2, 0, 4, -11, -2, 5, -12, -13, 5, 5, -12, 3, 2, -6, -6, -7, -3, 0, -7, 5, -5, -1, -3, 0, 6, 2, -7, -13, 3, -8, -11, 5, -5, -9, -11, -13, -2, -2, 2, -10, 4, -11, 6, -7, -4, -8, 7, 5, -11, -6, -4, -5, -5, 0, -3, 5, -3, -10, 2, -3, 3, -5, -11, 6}
, {-15, -6, 18, 12, 22, 34, 22, 8, 25, -28, 5, 57, 1, 7, 6, 31, -1, 9, 12, 2, 8, -20, -11, 9, 18, -10, 2, 24, -14, -13, 20, -7, -28, -9, 14, 9, -23, 4, 7, 19, 13, 4, 8, 9, -3, 1, -2, -16, 5, 8, 16, -1, 6, 22, 8, 17, -2, 6, 8, 4, -17, -11, 22, 14, -32, 0, 21, 24, -46, -22, -10, -11, 1, -42, -23, -2, 14, 36, 27, -22, 24, 9, 4, 28, 8, 22, 13, 32, -10, 5, -16, -24, -21, -11, 0, 26, -11, -3, -7, -12, -29, 11, 13, -20, -35, -14, -23, -34, -5, 12, -11, 11, -8, -8, -15, 5, 1, 11, 3, 12, 22, 2, -12, -5, 18, 8, 0, -25, 9, 4, 14, -3, -22, 11, 10, -28, -31, -6, -3, -11, -2, 32, -20, 15, 1, 10, -1, -5, 2, 12, 20, 11, -9, 7, -6, 23, 19, 15, 2, -17, 0, -17, 8, 16, 8, 18, 19, 29, 17, -13, -4, -1, 3, 21, -14, -7, 0, -17, -2, 7, -5, 15, 8, 23, 5, -5, 13, -3, 2, 3, -9, 7, -1, 17, 12, 28, 5, 12, 14, 9, 4, 28, 20, 8, -8, 26, -17, -4, 3, -6, 9, -9, -1, 10, 18, -15, 8, 8, 9, 28, 30, -13, -12, 0, 15, -2, -2, 19, 18, 3, -9, 3, -2, 2, 5, -12, -1, -7, 3, -3, 7, -17, 9, 19, -8, 16, -7, -23, -2, 7, -7, -7, 5, 14, -18, -5, 2, 7, 5, -18, 9, 21, 8, 11, -8, -1, -14, -17, 5, -3, 3, -11, 12, 29, 3, -15, 8, 26, 4, 23, -18, 9, 4, 0, 30, 15, 10, -14, 4, 30, 4, -1, -1, 8, 9, -20, -35, -16, -11, -14, 2, -13, -15, -14, 5, 18, -4, -16, -6, 11, 14, 0, -13, -4, 11, -2, 25, 5, 4, -7, 7, -19, 26, 18, 6, 2, -7, -35, -22, 6, -1, -19, 8, 6, -9, 22, 22, -7, -2, 11, -3, -20, -21, -27, 23, 7, 9, 15, 3, 7, -7, -22, 38, 4, -6, 4, 23, -3, -11, -27, -24, -6, -6, 10, -6, -13, -18, 2, -20, -22, -10, 29, 7, -17, -9, -2, 26, 5, 5, 11, 2, 17, -13, -35, 10, 5, 6, 12, 29, 17, 31, -13, 4, 4, 23, -3, 9, -1, 14, 8, -27, -10, 5, -22, 7, 12, 16, -8, -10, 2, 18, 16, 1, 4, -11, -21, 6, 12, 5, 48, 10, -2, -1, -15, 16, -15, 14, -5, 2, 14, 25, 11, 38, -13, -20, 7, 8, 12, 1, 17, 10, -4, -5, 43, 17, 35, 2, -1, 7, -13, -31, -2, 21, -3, 4, -8, 8, -29, 8, -11, 4, 6, 46, 20, 15, -53, -37, 18, 5, 0, 13, 3, 4, -9, -37, 18, 19, 70, -6, 8, 4, -12, -19, 10, -25, -18, -14, -21, -14, -3, -19, -10, 16, -14, -7, -18, 23, 2, -11, 13, 7, -6, 6, 11, -11, 4, -6, -13, 10, 3, -12, 33, 11, -8, -2, 14, -34, -12, -10, -5, -10, -20, -24, 3, 13, 3, 6, -15, 0, 3, -20, 2, 7, -3, 3, 16, -13, -4, -3, -18, 5, -3, 13, 13, 6, 8, 2, -9, -2, -9, 18, -5, 2, 48, 29, -15, -3, 2, 0, 38, -10, 3, 15, 1, -4, 7, 1, 6, 23, -7, 1, -8, -13, 1, 2, -9, -4, -13, -16, 17, -4, -23, -4, 4, 17, 14, 15, 22, -5, -2, -9, 6, -2, -4, -6, 16, 7, -9, -23, -2, 8, -2, 0, -21, -13, -8, -9, 5, -15, -7, 7, -10, -4, -11, 23, 16, 0, 8, 12, 35, -7, -12, 5, -21, -6, -12, 4, 44, -10, 0, -9, -11, -12, -2, 3, -27, -23, -13, -7, 1, -6, 22, 7, 16, -24, -12, -17, 4, 22, -3, 1, -7, 12, -7, 21, -23, 25, 10, 21, 26, 1, 45, 5, 20, 14, -1, 25, -17, 18, -6, -1, 36, 5, 22, 14, -16, -30, 3, 19, -18, -5, 3, -12, 2, -11, -3, -16, -7, -12, -3, -4, 11, -11, 24, -17, 4, 22, 1, 5, -10, 29, -6, 12, 11, -1, -34, -6, -1, 5, -10, 27, -14, 0, 43, 8, 1, 8, 0, -1, 5, -12, -47, -5, -16, 1, 5, -33, -11, 5, -7, -11, -1, -8, 22, -27, -15, 9, 1, 6, -2, 25, 10, -15, 14, -15, 18, -17, 11, 4, -9, -19, -15, 17, 7, 14, -12, 5, -1, -20, 5, -5, 4, -10, 3, -2, 8, 0, 4, 13, 59, -7, -7, -17, 30, -4, -11, -27, -5, -20, -11, 0, -29, -27, -23, -18, 22, 4, -13, -7, 18, -14, -5, -27, -6, 26, -9, 21, -6, -13, -18}
, {-8, 0, 10, -1, -5, -2, -7, 11, 8, 11, 6, 19, -9, -16, -10, -2, -10, -3, 13, 4, 0, -13, -17, 7, 7, -5, -2, -16, -28, 1, -7, -31, 13, -20, -23, -11, -6, 5, -9, 0, -8, -33, -5, 21, 13, -15, -12, -6, 2, 2, 8, 3, 8, 5, 5, -5, 22, -4, -12, -8, -19, 10, -11, 3, -18, -24, 20, -6, -21, -5, -15, -12, -19, -42, 9, -5, -8, 25, 12, 9, 6, -2, 0, -6, 3, 5, 6, 26, 19, -1, -36, -16, 8, 11, 9, -17, -7, 12, -8, -6, -38, 6, 19, -33, -44, -30, -23, -31, -7, 24, -6, 8, 9, -21, -31, -28, 7, -34, -3, 7, 26, -5, -41, -2, 11, 16, -13, -14, 16, -19, -13, -2, -4, -8, -14, -31, -20, -9, 1, -38, 1, 1, -15, 17, -5, 4, -8, -33, -11, -20, 12, -12, 6, -4, -17, 18, -2, 20, -17, -24, -2, 13, -2, 9, 8, 12, 6, 2, -13, 0, 8, -16, -5, -2, -1, -12, -20, 9, -1, -9, -1, 5, 15, -17, -18, -20, -6, -4, 4, -9, -6, 0, 6, 0, 11, 21, 7, -2, 9, -6, -5, 0, -10, -8, -9, 21, -29, 6, 0, 2, 7, -10, -8, -3, 10, -3, 1, -16, -1, 9, 4, 0, 7, -1, -1, 0, -1, -10, 4, -1, 8, -7, -1, 0, 18, -5, -2, -9, -8, 0, -4, 9, 18, -14, 2, 7, -7, -37, 7, -4, -12, 0, -18, 5, -23, -20, -20, -7, -5, 6, 0, 9, -2, 2, 2, -8, -11, -19, -6, -11, -8, -3, 6, 12, -4, 1, -3, 7, 3, -12, -17, -12, 2, 1, 3, -20, -9, 4, -23, 15, 18, -7, -12, -1, 19, -1, -4, -4, 10, 7, 2, 8, 2, -13, -32, 23, 28, 23, -9, 6, 23, -3, -8, -3, 7, -15, 4, -19, 5, -11, 4, -42, 10, -13, 14, -6, -17, -28, -23, 5, -21, -3, -1, -1, 2, -5, 3, -15, -12, -10, -2, -29, -22, -36, 9, -13, -28, 7, 0, -13, -26, -31, 26, -16, -6, -2, -2, -2, -20, -9, -4, -4, -4, -7, 4, -11, -1, -6, 10, -19, -10, 8, -3, -11, 1, -9, 4, -10, -19, -6, 10, 9, -9, -13, -11, -2, 9, 7, -10, 5, 17, 1, 4, 5, -3, -9, -4, -2, 19, -4, -2, 8, 0, -13, 3, -9, 14, -11, -12, -16, 16, -5, 6, 6, -7, -18, -6, -1, 8, 20, 12, -6, 2, -22, -7, -34, -10, -30, 3, 20, -3, -5, 7, -1, -19, -7, -1, -2, 9, 2, -4, -11, -19, 22, 9, 0, -12, -7, -9, -19, -26, -21, 15, -3, -3, -9, -5, -17, 11, -15, -5, 9, -8, 10, -2, -4, -18, -22, -5, -24, -7, 5, 12, -15, -21, 26, -10, 2, -7, -20, -4, 3, 19, 2, -7, -17, 1, -9, -11, -5, -7, -12, -2, -9, 18, -30, -3, 21, 16, -23, 5, 16, 15, 10, 1, -16, -7, -29, -5, -6, 2, 21, -3, 12, 25, 15, -9, 4, 2, 4, -7, 3, 4, -1, -4, 11, 9, -17, 14, 5, 7, -20, 2, 3, 8, -1, 25, 3, 6, 8, -1, -2, -12, 27, -15, -6, -14, -10, 5, -17, -2, 9, 20, 16, 30, 16, 0, 12, -25, 2, -3, -11, 5, 15, 8, 3, -10, -12, 24, -9, 9, 0, -18, -7, 1, 12, 20, -25, 12, 14, 5, -22, -27, 1, 3, 7, 0, 5, -2, -10, 4, -4, 2, -10, 2, 7, -9, 8, 3, -12, 0, -1, -16, -1, 2, 10, -8, -4, 13, 11, 7, -12, -1, -21, -21, -11, 22, 10, -30, -24, 5, -9, 2, -41, -15, 5, 11, 1, -4, -13, 10, -20, -29, 0, 4, -36, 2, 2, -11, 4, -3, -5, 7, -14, 17, 12, 17, -6, 11, -13, -2, -19, -9, -5, 9, 15, -3, -14, 9, -16, 0, 19, -4, -45, 2, -3, 14, -1, 23, -8, -13, 15, 13, -6, -6, -4, -11, -21, 14, -4, 5, 25, 12, 11, 10, 3, -18, 0, 0, 3, 2, -1, -6, 30, -11, -12, 8, -5, 12, -7, 23, -4, -12, -1, 10, -25, -4, -8, 2, -11, 20, 22, 4, 21, 13, 22, 6, -9, 14, -15, -21, -40, -24, -5, -5, 5, -25, -8, -14, 1, 3, 8, -3, 8, -9, -1, 10, -10, 20, -15, -3, -1, -3, 2, -7, 22, 0, -11, -5, -1, 3, 16, 9, 10, 21, 19, -2, -15, 13, 15, 7, -10, 8, 19, -8, 10, 3, 3, -6, 10, 12, 20, -10, 4, 14, 4, 6, 4, 6, -22, -7, 0, 17, -8, 5, 31, 31, 9, -2, 24, 5, -23, -18, -6, 35, -10, 26, 1, 6, 17}
, {19, -32, -22, 2, -28, -40, -16, 0, 3, 26, 1, -16, -2, -32, -13, -10, 23, -40, -14, 22, 4, 12, -37, -27, -2, -14, -24, -4, 10, 26, -34, 35, 58, -8, -12, -5, 39, -9, 2, -18, -2, 38, 18, -10, -10, -14, 17, 22, -8, -11, 6, 33, 3, 5, -1, -36, 21, 9, -17, -14, 19, 26, -18, -13, 44, 13, 1, 8, 56, 8, 12, 20, 3, -19, 13, -32, -12, -3, 15, 4, -30, 6, -6, 12, -1, 5, 20, -15, 17, 3, -9, 41, 20, 27, 3, -33, -15, 25, 29, 4, 4, 3, 1, -15, -14, 11, 6, 10, -7, -2, 17, 12, -2, 15, 15, -2, 1, 2, 10, 5, -1, -7, 8, 15, -5, 8, 11, -37, -4, 18, 10, 14, 14, 22, 2, -9, -3, 1, 5, -12, 3, -11, -9, 19, 22, 15, 21, 9, -4, 5, 5, -8, 22, 2, 13, -1, -16, 12, -4, 2, 5, 16, 2, 1, -2, -2, -3, 17, 11, 14, 2, 22, -6, -17, 23, -1, 0, 10, -2, 28, -8, 10, 2, -16, -19, -4, 24, 7, 30, 3, 0, 14, -2, 19, 11, 3, 11, 7, -6, -14, -7, -4, 7, -15, 12, -3, 6, 15, -16, -6, 1, 6, 3, 2, 4, -1, -6, -1, 1, 7, -9, 7, 19, -20, -7, 1, 14, -15, 6, 6, -1, -6, -1, -29, -1, -5, 1, 16, 7, 25, -9, 3, 7, -20, -8, -8, 8, 13, 9, 2, 2, 11, -13, -12, -1, -8, 1, -11, 1, 0, 11, -26, 10, -23, 8, 6, 5, 2, -8, 38, -2, 21, -19, -20, -6, 0, 1, 3, -1, -11, 11, -11, 4, 10, -14, -16, -12, -23, -34, -17, 7, 8, 9, -7, 16, -5, 10, -14, -6, -14, 4, 6, 29, 5, -2, -37, 1, 17, 1, -8, -21, -11, 1, 2, -8, 16, -13, -27, -14, 13, -6, -37, -14, 6, -9, 2, -35, 54, 28, 6, 12, 26, 2, 10, 4, 1, -16, -1, -18, 1, 1, -49, 7, 43, 2, -3, -18, -1, -7, 3, 18, 16, -25, -26, 20, 7, -30, 19, -35, 13, 4, 6, -7, 5, 12, 17, 2, 0, 5, 22, -6, -25, 5, -8, 17, 12, 6, -8, -18, -33, -6, -16, 32, 8, -6, -15, 13, -2, -32, -10, -15, 12, -15, -17, -19, -13, 6, 1, -14, -26, 24, 17, 13, 10, -3, -22, 16, 12, -3, -14, -20, -36, -17, -2, 21, 32, -4, -14, 2, -23, -15, -6, -35, 15, -13, 37, -3, -9, -3, 6, -38, -21, -20, 7, 10, -7, -7, 13, 5, -17, -9, -3, 18, -17, 13, -19, 15, 20, 23, -15, 3, 16, -16, 2, -10, 9, 3, 17, 7, 23, -4, -18, -30, -8, 18, -13, 3, -10, -3, 14, -24, -21, -2, 2, 26, -2, -1, -43, -14, 13, -34, 4, 1, -10, 6, -2, -7, 29, 17, 0, 17, 17, 3, 4, -12, -23, -21, 32, 24, -19, 7, 18, 21, 6, -30, -5, 27, -34, -29, -27, 6, 6, -58, 26, 13, -9, -6, 29, 16, 32, 24, -1, 9, 8, -2, 30, -19, -5, 2, 29, -5, -9, -8, -20, 1, 11, 7, 3, 2, -18, -19, -16, 11, 7, 2, 7, -1, 6, -14, 12, -6, 32, 6, -29, -29, 10, -7, -7, -14, 0, 14, 12, 5, -15, 0, -19, 5, -10, -22, 0, -13, 2, -10, -11, 16, 9, 7, 25, 19, 0, -20, 15, -10, -11, -22, 12, -4, -7, 10, -17, 3, -8, 1, 25, 7, -27, 8, -11, 6, 14, -3, -6, -14, 18, -4, -4, 32, 15, -16, 18, -1, -8, 17, 17, 1, -10, -14, 2, -29, -28, -8, 18, -14, 2, -10, 3, -6, -41, -3, 0, -15, -7, -1, 2, -6, 16, -13, -22, -18, -2, 14, 12, -5, -14, 7, -12, -3, 24, -11, -7, -6, 21, -10, 3, 19, 1, -10, 7, -11, 2, -11, -24, -12, 7, 1, -11, 7, 16, -6, 12, 26, -8, -16, -7, -10, 6, 43, 0, -5, -12, -15, 2, -2, -7, -7, -9, -28, -3, -6, 21, -1, -21, -11, -29, -14, 35, -19, -9, -6, -7, -36, 1, 8, -43, -24, 28, 14, 28, -10, 20, -5, 16, -1, -22, 4, -1, 8, 11, 4, -6, -2, 14, 5, 12, -1, -11, 16, -5, -6, -9, 8, -3, -4, -11, 6, 5, -40, 13, -16, 14, -27, 0, -1, 12, 27, 16, 9, -16, 10, 3, -5, -1, 11, -3, -3, -12, -9, -3, 15, 15, -4, -7, -7, -6, -11, -41, 1, 18, -13, -54, -26, 20, 6, -24, -21, 7, -7, -8, 20, -28, 11, -3, -3, -1, -2, -44, -21, -19, -6, -25, -19, -32, 17, -8, -33, 5, -20, 5, -33, 19}
, {-12, 30, 6, -1, 13, 8, 21, -11, -3, 13, 13, -20, -4, 13, 19, -4, -37, 10, -12, -12, 8, 8, 17, 17, -28, -7, 6, 9, 9, -29, 26, 9, -22, 18, 21, -13, 4, 13, 18, 13, 1, 12, 3, -1, 7, 33, 10, 17, -8, -9, 5, -9, -1, 0, 27, 28, -7, -6, 13, 3, 14, -17, 3, 7, 6, 18, -6, -20, -17, -6, 21, -10, -6, 15, 23, -6, -6, 2, 29, 20, -16, 15, 13, 1, 7, 0, 3, -4, 43, 1, -1, 19, 12, 6, 0, -13, 34, 0, 1, 6, 19, 23, -11, -5, 14, -7, 11, -14, -5, 15, 11, 6, -2, -22, -7, 18, 0, 3, -7, -31, 24, -3, 5, 10, 7, 32, -8, -1, -9, -16, -1, -8, -13, 0, -28, 12, 6, -10, 3, 2, 6, -16, -4, -14, -14, -15, 0, -13, 3, -6, 3, -2, -7, -3, -10, -27, -10, 5, -6, -6, -1, -20, -12, -8, 0, -5, -8, -18, -9, 14, 0, -6, 3, -20, -2, 6, -9, 4, 9, 9, -8, -13, 9, -23, -1, -5, -14, -5, -21, 12, 8, -29, -13, 12, 0, -12, -2, -11, -22, -4, 8, 0, -2, 19, 3, 3, -8, -1, -2, 12, -3, 6, -11, 12, 10, 34, -22, -5, 6, -2, -6, -2, -6, 16, -7, 14, 16, 6, -1, 5, -4, 1, -12, 2, -7, 7, -3, 11, -13, 6, 22, -10, -6, 19, 2, 5, -23, -2, -17, -5, 10, 10, -5, -5, 10, 4, -9, 14, 12, 12, 8, 9, -10, -11, 8, -9, 1, 1, -9, -2, -3, 2, 2, -12, 16, -2, -6, -5, -2, -22, 0, -4, 9, 5, -16, 2, 16, 5, -26, -26, -8, 10, -1, 10, -16, 3, -14, -14, 5, -23, 2, -1, -2, 4, -4, -28, -16, -13, -4, -1, -27, -37, -15, -11, 8, 6, -26, -14, -16, -7, -11, 6, 0, 4, -6, -15, 14, -10, 20, 2, 1, -18, 2, 5, -10, -13, -17, 8, 20, -14, 5, 22, -12, -14, -22, -9, 19, -9, -7, -14, -11, 11, -6, 1, 12, 10, 12, 9, -3, 10, 28, 12, 18, 20, -13, -2, -5, -4, 6, 11, 15, -19, -3, 18, -26, -20, 7, -9, 14, -8, 14, 6, -15, 38, 3, 2, -20, -1, 13, -12, 8, 1, 20, -5, 3, 8, 2, -24, -15, -7, -4, -21, -10, 34, 2, 7, -14, -8, -5, -13, -1, 4, -3, 7, -10, 15, -1, -12, -9, 11, 8, 1, -6, -6, 16, -14, 7, -3, -17, -6, 48, 4, -13, -33, -18, -2, -2, -10, -4, -8, 11, -2, -11, -6, -7, 9, -5, 0, -20, -13, -14, 1, -28, -24, -32, -5, 2, 4, -16, 34, 6, 11, -13, -8, -17, 13, -15, -12, -5, -22, 2, -8, -1, 6, -8, -6, -18, -33, -1, -14, 21, -1, -1, 1, 9, -7, 9, -4, 11, 13, -2, 10, -14, 15, 21, 20, 6, -14, -7, 14, -1, -19, -14, 6, 16, -12, -6, 18, 6, 11, -3, 1, 24, -16, 4, -13, 18, 1, 9, -16, 9, 14, 23, 1, -2, -9, -3, 23, 1, 6, 9, 18, -4, 27, -15, -17, -9, 5, 5, 9, 14, 7, -24, -24, 5, 11, -11, -3, 13, -17, -3, 7, -12, -1, -23, -3, -15, -17, 29, 3, -9, -21, -3, 28, 5, 17, -3, -32, -13, 4, 19, 2, 17, 9, -19, -3, 12, 10, -4, -11, -24, -5, -10, 5, 3, -6, -1, 26, 0, -5, 26, -6, -21, -24, -9, -16, -1, 0, -16, 16, -14, 0, 7, -28, -2, 2, -5, -4, -18, 21, 11, 7, -14, -13, -4, 11, -5, 6, -20, 0, 9, 12, -16, -22, 11, -1, 1, -47, -1, -10, -9, -4, -14, -7, 6, -18, -5, -17, -7, -5, -2, -19, -5, 2, -8, 0, 1, 16, 0, -4, -11, 4, -4, 2, 8, 6, 42, 30, 3, 9, 1, -29, 8, 10, -16, 2, -22, -3, 11, 16, 8, 5, 29, -22, 10, 16, -3, -4, -27, 36, 24, -14, -9, 14, 3, 24, 5, 11, 12, 21, -8, 8, 2, -46, 27, 35, 1, -1, -35, 0, -24, 22, 35, 25, -12, 21, 24, 5, -4, -6, -4, 19, -2, -31, -30, 0, 1, -8, -1, -2, 31, 6, 17, 0, 7, -6, 15, 1, -24, -2, -6, -8, -48, 7, -1, 27, -15, -3, -11, 5, 21, 2, 1, -33, -19, -6, -6, -17, 3, -33, -24, 4, -16, 9, 19, -3, -9, -1, -40, -43, 2, -13, -7, 4, 9, -10, -26, -18, -23, -35, -8, 13, 10, 5, -4, -13, 0, 12, 19, -30, -8, 18, -6, 2, -6, -31, 5, -8, -3, -13, -10, -24, -22, -3, 13, -2, 28, -25, -42, 13}
, {2, -9, 7, 2, -1, -7, 8, 5, 10, 34, 27, 36, 3, 7, -28, 47, 1, -13, 12, 19, 1, -1, -26, -11, 32, -6, 19, 6, -19, 5, -9, 4, 40, -11, 2, -16, -15, -16, -9, 28, 18, -4, 13, 32, 4, -11, -13, 24, -7, -3, 11, 32, 8, 7, -36, 1, 40, 8, 18, -31, -27, -5, -2, 8, -2, -39, 3, 21, -15, -11, 12, 26, 4, -21, 13, 26, 11, -5, -8, -23, 3, -2, -4, -8, -1, 6, 18, -14, -24, 1, -6, -25, -15, 6, -3, 5, -29, 23, 9, -20, 3, 8, 36, -6, -11, 9, -16, 16, 4, 11, -3, -1, 17, -3, 6, -1, -1, 24, 6, 28, -40, -2, 6, 20, 5, 0, 16, 3, -18, 6, 18, 22, 8, 31, 7, -16, -11, 9, -9, -2, -8, 9, -9, 22, 14, 28, 3, -25, 10, 19, 7, 33, 2, 3, 1, 32, 10, -16, 8, -17, 3, 4, -24, 6, -14, -21, -9, 2, -12, 11, -12, 7, 0, -32, -34, -14, 3, -8, -9, -22, -5, -14, -17, 0, -22, -3, 5, -12, 3, 1, -18, 1, 16, -19, -8, 2, 3, 6, -13, -3, -8, -10, -1, -8, 7, 11, -9, -9, 3, -1, 5, 19, 4, -14, 12, -16, -24, 5, -12, -4, 20, -9, 4, -15, 1, 4, 12, -11, 5, 6, 16, -3, 1, 14, 10, -15, -8, 15, 5, 6, 18, 19, 3, 6, -9, -3, -3, -7, 13, -6, -5, 4, 12, 5, 20, -3, 12, -1, 1, 9, 29, -12, 4, -14, -3, 9, 12, -23, 15, -2, 0, 6, -1, -26, -18, -14, 6, 16, -17, -32, 17, -9, 0, 15, 36, 16, -22, 14, 10, 9, -22, 13, 2, -16, -6, -9, -5, -36, -31, -29, 7, -29, -8, 7, -15, -35, -23, -26, 7, 13, -15, 1, 4, -5, -13, 7, 7, -2, -29, -1, -14, 1, 15, 1, -12, -8, 1, -13, -20, 8, 1, 10, 8, 7, 1, 6, -6, -12, 9, -15, -3, 0, 18, 3, -9, 1, -5, -1, 25, -3, 7, -5, 6, 19, 22, -2, 19, 38, 2, -15, 6, 9, 4, -24, 14, 26, -9, 5, -17, 2, 27, 3, -1, -14, -9, -12, -11, 6, 2, 17, -15, 6, -3, 0, 0, -6, 19, 22, -5, 23, -12, 15, 5, 8, 22, -25, -11, 12, -7, 20, 9, 3, 2, -25, 6, -7, -17, -15, 13, 2, 1, 6, -1, 6, 8, 21, 16, -4, 3, -1, -3, -4, -30, 1, -12, 2, 2, 17, -2, -7, 1, -2, 9, 12, 13, 6, 9, -9, 11, -9, 14, -1, 1, -16, 19, 20, 18, 6, 34, -31, 10, -14, -16, 7, -5, 12, -1, -1, -11, 10, 0, -14, 0, -3, -6, 6, 14, 3, 6, -10, 4, -28, 10, 4, -4, -34, -16, 4, -4, 9, -14, -25, -6, -6, 14, -2, -23, -8, -7, -2, -11, -5, 3, 13, -6, 4, -18, -24, 2, -9, -2, -1, -11, -4, -3, 3, -7, 17, -3, -4, -4, -7, 3, -8, -19, -3, -15, 0, -33, 1, -7, -2, -24, -24, 9, -17, -16, -19, -6, -10, -8, -26, -9, -26, -3, -36, 15, 2, -34, -9, -9, -9, 8, 4, -14, -25, -8, 2, -29, 0, -6, 7, 8, 12, -8, 2, 0, -1, -8, 1, -4, 20, 6, -34, 6, -4, 12, 7, -16, -4, -2, -3, 8, -6, 10, -7, -6, -14, 8, 9, -13, -7, -24, -10, -5, -1, -9, -7, 2, 10, -6, -3, 6, 12, 9, -2, -2, 3, 4, 32, -2, -7, 1, 7, 15, -15, 16, -3, -25, 5, 2, -7, -19, -6, 19, -4, -9, -14, -29, -27, 6, 2, -18, -22, -15, -5, 4, 10, -1, -6, 3, -7, -28, 12, -2, -17, -6, -13, 4, 0, -17, 5, -9, -10, -9, -7, -1, 10, -2, -8, 4, -22, 4, -10, -10, -20, 9, -14, 9, -1, 4, 24, 4, 7, -7, 6, 4, -13, -3, 12, -17, -12, 20, 2, 10, -17, -4, -2, 21, -5, -8, -6, 2, -20, 5, -22, -6, -6, 5, 6, 6, 4, 5, 17, -17, -1, -14, -7, 12, -7, 18, 1, -3, -22, 10, -14, -10, -16, 14, -9, 0, 25, 1, 13, 1, 19, 0, 23, -1, 0, -8, -11, -8, -17, -1, 16, -20, 15, 21, -4, 2, 13, 31, 11, 3, 0, -4, 15, 14, 4, 17, 27, 10, 12, -16, 20, -12, 9, -3, -3, -4, 8, 20, 1, -4, -17, 8, 7, 7, 12, -32, -9, 5, -2, 10, 0, -8, 8, 2, 8, 14, -4, 2, 17, 23, -18, -59, -40, -19, -48, 2, -8, -17, -3, -31, -28, -22, -40, -4, 12, -19, -8, 12, 3, 8, 20, -5, -15, -10, -15}
, {-5, -11, 22, 5, -5, -1, 0, -20, -35, -2, -3, -17, -9, 12, -10, 10, -2, -7, -12, 1, 7, -12, -12, 0, 20, -14, -15, 8, 2, -11, -8, -36, 27, -9, -14, -3, -12, -8, 17, -18, -1, 7, 20, -11, 2, -14, 7, 5, -28, -15, -20, 12, -3, -1, -2, -21, 17, 6, -28, -15, -11, 13, -20, -6, -8, 10, -4, -9, -24, -5, 4, -5, -16, -27, -10, 5, -13, 12, 15, 3, 0, 4, -2, 4, 10, 7, 21, 14, -11, 8, -22, -24, -17, 15, 5, -11, -5, 41, -5, -16, 13, 30, 31, -10, -12, -35, -15, -14, 1, -14, -18, -10, 9, -10, -19, -9, -8, 1, -7, 27, 5, -9, 10, 16, 4, 19, -1, -23, -8, 6, 18, 10, 10, 23, 2, -23, -8, 3, -10, -25, 8, -4, -21, 32, 11, -11, -16, -27, 0, 7, 5, -7, 19, -1, 0, 20, -3, -2, 6, -8, -7, -29, -21, 12, -15, -19, -31, -16, -5, -5, -10, -21, 6, -25, 8, -12, -6, -10, -18, -15, -6, -24, -18, -5, -11, -10, -17, -30, -8, -12, 4, -34, 20, -18, -11, -5, -8, -23, 0, 15, 9, -1, 11, 12, -6, -1, -10, -29, 7, 5, -5, 4, 6, -22, -2, -3, -19, -5, -21, -12, 8, -10, -9, -7, -8, 9, 9, 8, -2, 0, 7, 12, 2, -13, -13, -3, 0, 12, 13, -5, 15, -16, 6, 17, 4, 4, -21, 1, -25, 5, -6, 6, 12, 7, 4, 21, 18, 4, -10, 4, 29, 20, -5, -7, 1, -17, -6, -35, -8, -29, 4, -11, 10, -31, -9, 29, 4, 23, -20, -33, -16, 0, -5, 19, 24, 14, -23, -3, 5, 23, 2, 3, -5, -4, 6, -18, -6, -40, -5, -42, -1, -4, 14, 10, -14, -20, -15, -7, 9, 26, -3, -7, -6, -8, -4, 6, 15, 7, -11, -23, 5, -13, -3, 3, 6, -6, 6, 10, -6, -18, -6, -8, 7, -20, 5, -16, -4, 21, 6, 3, 1, 0, 18, -8, -7, 4, -12, 1, 2, -16, 8, -29, 9, 24, 26, 0, 16, 4, 5, 9, 21, 15, -6, 10, 3, 25, 12, 0, -39, 19, 36, 21, 3, 2, 15, 18, -12, -6, 12, 11, 3, -11, 20, -22, 5, -7, 12, 11, 8, -1, 2, 12, 9, 9, 11, 15, 0, 2, 4, 12, -3, -24, 2, 20, -9, 3, -29, 6, 9, -7, 12, 24, 6, -4, -4, 8, 16, -31, -20, 20, -1, -35, -6, -14, 10, -7, 6, -2, -1, -16, 37, 12, 0, -14, -22, 16, -8, 2, 2, -2, 25, 9, -31, -4, 23, 31, 4, -4, 5, -45, -9, 2, 0, -15, -16, 5, 3, -7, 4, -8, 4, -8, 29, 4, -6, -31, -19, 11, -4, -7, -14, -17, 11, -10, -14, -12, 14, 27, -9, -8, -2, -13, 18, -4, 2, -3, 6, 24, 8, 10, 7, -7, 7, 34, 2, -4, -10, 13, 29, -12, -2, -15, 1, 30, -14, -13, 15, -20, -9, -7, 16, 2, -7, -8, 2, 24, -6, 15, -19, 13, -2, 4, 13, 14, -7, 19, -30, 2, -2, 17, 7, -27, 0, -15, 4, 4, 14, -12, -6, -6, -27, -19, -2, 36, -13, 4, -29, 17, -22, -20, -12, -16, 20, 19, -2, 23, -4, -4, 11, -22, 1, -18, -4, 29, 5, -1, -6, -14, -13, 4, 15, -13, -6, -13, -10, 9, -3, -21, -5, -1, -19, -4, -8, 6, 5, -2, -3, 4, -2, 2, 5, -10, -9, -14, 8, -3, 4, 4, -14, 10, -11, -6, -10, -32, 4, -1, -10, 14, -22, -6, -9, -1, -24, 2, -12, 8, -11, 13, -24, 13, -9, 5, -31, -25, -18, -13, -3, 2, -4, -8, -35, -2, -6, -13, -2, -21, -28, -17, 15, -3, -4, -12, -1, 8, -2, -10, 4, 18, 22, 2, 15, -17, 6, -5, 4, 0, 7, -5, 2, -17, 3, 12, 1, -22, -3, -5, 14, -19, -24, -3, -27, -12, 21, -25, -2, 7, -14, -28, -23, 35, 26, 7, -7, -5, -11, 21, -7, 8, 3, 8, -3, -9, -10, -15, 4, 17, -21, -4, 14, -29, -17, 18, 14, 3, 1, -4, -8, 6, 3, 3, -7, -1, 2, 13, -28, 11, -8, -9, -19, 12, 16, -7, -3, -1, -3, 4, -15, -8, -17, 7, -2, 24, 3, 9, -7, 23, 1, 29, 6, 24, 12, 16, -4, -11, -16, 0, -7, -29, -2, -30, -16, 7, -3, -5, 26, -9, -3, 25, -23, -36, 8, -6, 20, 0, 42, 15, -24, -13, 5, 7, 28, -9, 0, 26, 36, -23, -15, -40, -1, -32, -20, -8, 1, -24, 0, -3, 9, -20, 2, 1, -2, -7, -13, -14, 19, -13, 48, 6, -9, -3}
, {-9, 19, 20, 5, 18, 41, 21, 14, 22, -22, -1, 6, -1, 25, 14, 24, 13, 12, -6, -10, 0, -5, 18, 43, 34, -10, 7, 22, 3, -9, 15, 18, -49, -1, 23, 35, -2, 10, 37, -4, -2, 11, 10, -18, 0, 16, -13, 6, 11, -8, -33, -4, -8, 13, 25, 35, -28, 5, -9, 58, 14, -15, 7, 2, -3, 7, -23, -23, 15, -5, 20, -24, 16, 0, 10, -12, -4, 2, 4, 26, -3, -7, -3, 5, 9, 17, -14, 11, 17, 9, 10, 26, 1, -9, -20, -7, 36, -31, -10, -2, -6, -6, 14, -11, -11, -19, -14, -3, 2, -20, 12, 9, 0, -1, -3, 29, 8, 23, 5, -14, 23, 4, -12, 8, 20, 27, -15, -10, 17, 7, 8, -1, 10, 5, 19, -21, -13, -9, 2, -5, -6, 15, 20, -19, -11, 5, 1, 9, 0, 29, 26, -9, -26, 2, 24, 6, 19, 39, -5, -7, 3, 0, -6, -15, 18, 0, 9, -1, 25, 15, 2, 11, 1, -9, -20, 18, 16, 16, 12, 4, -6, 17, 19, -38, 14, 2, 7, -2, -22, -5, -16, 16, 6, 13, 1, 7, -13, -22, -12, -8, 2, 3, 5, -16, 3, -13, 4, 3, 10, -3, -12, 13, 8, 5, -3, -39, -2, 2, 8, -2, 3, 9, -5, 7, 15, 22, -3, -20, 10, 7, 6, -2, -7, -18, -6, 4, 4, -8, -21, -15, 13, -3, 9, 17, 6, 1, -16, -26, -22, -2, 9, -3, 27, 13, 0, -3, -1, -2, -2, -28, 3, -3, 18, -5, -15, 1, -11, -24, -5, 9, 12, -5, 5, 12, 20, -26, 8, 4, -6, 6, -22, 6, 3, -3, 2, 22, 13, -2, 6, 1, 12, 8, 14, 16, 0, -3, -15, -16, 7, 1, -3, 15, -5, 12, 15, 18, -3, 15, -2, 4, 9, 14, 1, -7, -2, 20, 11, -2, 6, 8, 7, 15, -4, 11, 10, -5, 3, -3, 12, 20, 14, -16, 1, -1, -2, 14, 5, 24, -11, -25, -7, -11, -9, -7, 4, -5, 10, 14, 1, 1, -4, 0, 11, -2, 5, 23, 14, 11, 15, -28, -24, -7, 5, -28, 5, -19, -4, -4, 18, -13, -21, -38, -6, -13, -2, -26, 4, 3, -2, 16, -17, 6, -13, 28, 10, -13, 5, 14, 18, -5, 4, -13, 1, 16, 4, -3, -9, -4, 12, 10, -16, -9, 4, -14, 0, 13, 1, -1, 4, -3, -20, 31, 1, 12, -10, 2, 21, -14, -2, 20, -5, -9, 11, -8, 7, 13, -3, 2, -2, 12, 1, 1, 11, -8, -13, -20, -9, -14, -1, 2, 4, 14, -10, 31, -25, 16, 6, 5, -38, 5, 2, -2, 15, 13, 0, 4, 3, -3, 6, 12, -8, 3, -8, 9, -9, -18, -1, -3, -3, -3, -10, 4, -5, -9, 1, 22, -14, -18, -1, -7, -2, -1, 6, 1, -16, 15, 17, 18, 13, 3, 9, -23, 5, -17, 2, -14, -10, -7, -19, -8, 3, 10, 5, -17, -4, -3, -11, -5, 8, -1, -11, 12, 17, -3, -16, -6, -1, -12, 18, -7, 9, 2, 20, 2, 5, 0, 39, -7, -2, -4, 6, 14, 3, 33, 9, -12, -27, 9, 2, 2, 32, 23, -6, 2, 37, 12, 13, -14, -9, -8, 16, -11, -9, 11, 3, -15, 2, -15, 30, 20, 12, -3, 0, 8, 9, 20, 10, 1, 10, -5, -3, -8, 12, 20, -9, -2, 7, -11, -17, 3, -27, -25, -32, 3, 15, -8, -6, 24, -7, -18, 5, -13, -13, -4, 0, 6, -3, -1, 6, -2, -4, 2, 16, -37, -15, -2, 17, -2, 10, -4, 4, -10, -6, 5, -25, 11, 10, 12, -17, -15, 0, -11, 11, -11, 22, 17, -2, -14, -2, -16, 0, -1, -5, -4, 8, -22, 5, 2, 4, 13, -8, -15, 6, -18, 9, 1, 1, -7, -18, 13, -4, -16, 8, 10, -26, -4, -47, -10, -6, -15, -5, -9, -21, -16, 10, -10, 5, 1, 12, -32, -17, -17, 2, 2, 13, -9, -3, 17, 6, 8, -4, 20, 19, 5, 6, 7, -21, 10, -20, 0, 1, -9, 8, -8, 1, -10, 25, -5, 6, 8, -12, -27, 2, -3, -2, -7, 16, 13, -3, 14, 16, -22, -2, 18, 11, -15, -1, 16, 13, 9, -18, -13, 11, 14, -1, 13, 9, -18, 9, 7, 1, 4, -46, -17, 8, -10, 5, -12, -18, 3, 13, -16, -9, 0, -2, -9, 1, 29, -11, 2, -17, -4, -25, 10, 4, -13, 9, -12, -43, 14, -8, 8, 10, 3, -46, -31, 11, -20, -11, -8, -11, 23, -5, 5, -25, 0, 13, 20, -24, -4, -10, -3, 7, 1, -6, 6, 8, -6, 9, -13, -27, -7, 0, 4, -2, 12, -17, -44, 4, -4}
, {-2, -1, 7, -2, -2, 3, -5, -3, 4, -11, 5, -8, -3, -7, -5, 4, -8, -4, -13, 8, 0, 6, -4, -3, -2, -4, 4, -9, -5, -8, -1, 0, -3, -7, 5, -2, -4, -5, 7, 5, -11, 9, -4, -11, 9, -9, -7, -2, 4, -1, -12, 2, 7, -1, 4, 4, -1, 6, 4, -13, -5, -13, -1, -4, 4, 3, 6, 5, -3, -1, 4, 5, -13, -5, -9, -11, 6, -1, -8, 1, -8, 8, 1, -2, -2, 5, 0, 0, -4, -8, -11, 7, -10, -6, -13, -4, 6, -1, -7, -10, -2, -7, -10, 7, -6, -7, -4, 7, 1, -10, -2, -8, -4, 7, 0, 9, -8, -12, 2, 5, -1, -10, -6, -2, 6, -10, -9, -3, 2, -5, -8, -12, -12, -11, 6, 2, 0, 2, 5, 0, -11, -9, 2, -1, -13, -11, 3, 0, -9, 3, -4, 6, -8, -1, 0, 4, -4, 0, -5, -2, -10, -3, -4, -6, -10, -8, -2, 3, -2, -2, -5, 7, 6, 3, -3, -1, -8, -9, 6, -2, 6, 6, -9, 9, 3, 3, 6, -11, -7, 7, -14, -7, -1, -14, 5, -8, 0, -2, 3, -4, 5, -8, -4, 0, 10, 0, -6, -3, -3, -6, -14, -3, 3, -9, -4, 9, -7, 8, 5, 2, -11, 0, -14, -3, -6, 2, -7, -10, -12, -1, -6, -7, 4, 8, -8, 6, 2, -14, 3, 0, -7, 5, 2, 0, -11, 7, 7, 7, 5, 6, 2, -8, -13, -2, 1, -14, -1, 10, 6, -14, -11, -11, -8, -6, -13, 0, 4, 7, 9, 5, 1, 4, 0, -8, 4, -8, -10, -1, -5, 2, -4, -6, -6, -8, -11, -3, -6, -1, -2, 8, 0, -11, -5, 7, -1, 9, 1, 6, -7, -9, 9, -13, 6, -5, -6, 5, -11, -1, 0, -11, -6, 0, -10, 4, -11, -3, 3, 4, 4, 4, 5, 7, 1, -3, -10, -12, 3, 3, -8, 1, -9, -1, -6, 4, 3, -2, 7, 6, -1, 0, 8, -3, -12, -2, 3, -6, 8, -6, -1, -3, 4, -10, 4, 8, 6, -9, 5, -2, 3, 7, 5, -11, -1, -5, 6, -12, -11, 6, -11, -3, -9, 3, 4, 8, -6, 4, -14, 8, 5, -12, -10, 7, 8, 8, -6, -6, 5, -13, 5, -9, -1, -2, -10, -2, -6, -1, -11, 5, 8, -11, -10, -7, -5, 2, -3, -6, -11, 3, -1, -6, 2, -6, -5, -9, -4, -9, 5, 7, 4, 1, -2, 7, 0, -6, -2, -1, 1, -6, 2, -15, -14, -9, 4, -9, 1, -1, -2, 3, 1, -7, -2, -5, 3, 5, -10, 3, 7, -7, -10, -7, -4, 7, 4, -3, 4, -11, 3, -8, 5, -11, -9, 1, -11, 8, -12, -11, -13, -7, -8, -4, -2, -6, 3, -3, -3, -11, -3, -4, 6, -11, -1, 4, -11, -2, 1, -3, -5, 1, -6, 5, -11, -4, -7, -4, -7, 2, 6, 0, -12, 9, 8, 3, -7, 6, -11, -11, 9, -1, -3, -4, -7, -9, -8, 0, -10, -1, -2, -9, -8, 6, -10, -6, -10, 0, -3, 5, 1, 1, -3, -2, -11, -4, 1, -11, -3, -8, -11, -1, 6, -2, -8, -3, -7, -6, -10, 6, -13, -9, -11, -2, 3, -12, -12, -2, -4, 4, 5, -6, 3, -1, 2, 5, -10, 3, 5, -9, -6, -8, -6, 2, 11, 5, -10, 0, -1, -6, 1, -3, -1, -3, 2, -8, -12, -4, 2, 3, 1, -10, 5, -11, 3, 4, -10, 8, 3, -1, 2, 3, -12, -2, 4, -12, -2, -8, 0, 4, -11, -5, 1, 4, -10, -7, -4, -4, -4, -3, 0, -6, -7, 6, 5, 1, -1, 1, -6, -6, -6, -10, -8, 3, -2, 3, -11, 9, -3, -1, 2, 2, -6, -2, -7, -11, -4, -5, -9, -4, 5, 8, -11, 2, -6, 5, 1, 1, -9, -2, -7, -6, -11, -4, 5, 6, 3, 4, -12, -6, -9, -11, 5, 3, -3, 7, -6, -3, 2, 0, -11, -10, -2, -2, -6, -8, -2, 6, 4, 5, 6, 5, -5, -13, 6, -6, -11, -9, -13, -8, -5, 3, -5, 4, -7, -7, -3, -2, 3, 3, -8, -7, -2, -9, -11, -2, -3, 6, -3, -5, -7, -2, 3, -4, 4, -10, -9, -4, -1, 7, -1, 6, -8, -11, -5, 6, 1, 3, -10, 2, -10, -2, -7, -4, -11, 8, 6, -4, -8, -6, 0, 4, 4, -14, 2, -5, -12, 7, -12, -1, -10, -10, 2, 6, 3, -2, 0, -8, 0, -10, 3, -3, -8, -9, -7, 5, -4, -4, 4, -5, -7, -7, -12, -4, -7, 6, -9, 3, 2, 3, 5, -2, 0, -2, -13, -3, -2, -1, -3, -6, -7, -2, -4, -2}
, {4, -8, 7, 7, 21, 14, -19, -1, 2, 19, 16, 11, -8, 11, -6, 17, 4, -5, 10, 6, -3, 0, -18, -3, 19, 4, 12, 3, 13, 0, -8, 9, 0, 0, -3, 7, -9, -4, -14, 9, -11, -20, -17, -6, -1, -1, -19, -3, -9, -8, -19, -11, -8, -2, -28, 13, -1, -7, 5, -17, -15, -17, -5, -11, -11, -9, -10, -10, -12, -20, -18, 2, 4, -22, -17, 0, 8, -20, -1, -17, -6, -3, -18, 4, 5, -11, -14, 1, -5, 4, -3, -17, -3, 8, -10, -21, 14, -3, 6, -4, -21, -22, -13, -14, -18, -9, -15, 9, -2, -6, -1, -13, 13, -1, -6, -5, 7, 3, 4, -11, -25, 7, -9, -26, -9, 12, 0, 4, -3, -4, -2, -20, -16, -18, -28, 13, -5, 4, -16, 0, 5, -2, -15, 1, 2, -9, -9, 5, -10, -2, -11, 7, 7, 2, -10, -16, 1, -4, -4, 3, 14, -9, -4, -2, -9, -20, -17, 11, 8, -14, -14, 12, -3, -3, -4, -6, 14, 0, -11, 5, 9, -12, -28, 8, 1, -5, 7, -9, 1, -1, -9, -3, 5, -8, -18, -4, -9, -13, -24, -4, 1, -17, -6, -4, 7, -32, -31, -15, -5, -2, -13, 10, -7, -9, -23, 0, -9, -4, -8, -18, -10, -2, 0, -3, -5, 8, 3, -8, -11, -12, -17, -16, -21, -13, -21, -7, -5, -4, -16, -16, -5, -2, 1, -3, -8, -12, 7, -12, -23, -7, -2, -6, -10, -9, -10, -3, -11, 3, 18, -13, -17, -5, -11, -22, -10, 5, -10, -9, -9, 4, -5, 5, -2, -22, -8, 0, -5, -1, 0, -25, -16, -6, 5, -12, -9, -2, -14, -20, -13, 20, 4, -3, 7, -7, -4, 4, 0, -18, -3, 1, 0, 1, 6, -13, -7, 6, -4, 9, 4, -1, 17, -11, -18, -10, -4, 1, 15, -10, 3, -1, 8, 9, 8, 9, 7, -7, -18, 8, 1, -23, -3, -5, -2, -9, -5, -11, -4, -6, 2, 0, -1, -4, -12, -7, -6, 4, 1, 5, -2, 11, 0, -6, 17, -3, -3, 1, -3, -2, -16, 9, -3, 8, -9, -11, -4, -8, 1, -18, 9, 1, -11, 0, 5, -4, -3, -2, -27, -7, -9, 8, 9, -9, 1, 20, 2, -13, 13, -6, 0, 10, -8, -6, 3, -10, -14, -13, -10, -16, -9, -14, 6, -19, -3, -12, -5, -9, -3, 3, -22, -2, -4, 10, -2, -4, -14, 10, -5, -2, -4, -14, 1, -20, -12, -3, -16, -6, -9, 1, 0, -16, 6, -14, 4, -6, -14, -4, -6, -17, -6, -15, 0, -9, -8, 13, -2, -3, -6, -10, -3, 14, 8, -12, -4, 12, 2, -6, -1, -7, 5, 3, -7, 17, -8, 3, 2, 3, 4, -3, -9, -5, 17, 4, -5, -11, 2, 9, -5, 2, 8, 7, 6, -4, 10, -7, -7, 5, 11, -15, 4, 5, 14, 7, -4, -2, 0, -1, 12, 2, 1, -9, 4, -1, 9, -10, -5, 0, -4, -5, 7, 16, 5, 5, 2, 10, 5, 2, 1, 15, 4, -23, -2, 17, -12, -7, -1, 7, 4, -4, 9, 15, 15, 3, -4, 2, 21, 11, -2, -5, 4, -1, 30, 10, -12, 7, 21, 4, -15, 8, -8, -10, 8, -1, -12, -29, -2, -7, -1, -19, 2, 5, 7, 0, -22, 1, 7, -3, -29, -11, -20, 1, 12, -5, -2, 18, -21, 13, -6, -34, -10, 4, -9, 16, 1, -16, -2, 27, 30, -9, 9, -18, -23, -5, -3, -22, -19, -12, 5, -12, -30, -11, -13, 1, -7, 16, -20, -6, 4, -19, 12, -7, 14, 6, 11, 1, 8, 28, 1, -15, 6, -11, 6, 20, 9, 7, 16, 18, 19, 0, -3, -29, 25, -16, 30, -3, -4, 25, 22, 24, -4, 14, 16, 7, 15, -2, 12, 9, 3, -17, -4, -3, 12, 7, 5, 8, -10, 13, 19, 1, 3, -8, 4, -7, 8, 12, -10, -10, -6, 2, 19, -5, 2, 5, -11, 0, 9, 1, 11, 11, 33, 11, -17, -12, 9, 0, -7, 2, -15, 26, 8, -5, -5, -9, -5, 10, -2, 4, 5, 1, 6, 22, -21, -21, 10, -3, -13, -9, 0, -1, 26, -1, 14, -29, 4, -7, 12, -9, -6, 15, 7, 20, -41, -37, 6, -5, 4, 17, -5, -23, -12, -10, -22, 21, 18, -12, -23, -26, -25, -24, -22, 0, 10, -40, 12, 9, -5, 6, 8, -8, -10, -20, 2, -4, 0, 21, -14, -1, -3, -4, -21, -5, -37, -9, 12, 3, -6, -22, 6, 10, -37, 23, -9, 17, 36, 30, 26, 26, 4, 30, 14, -29, -2, 21, -9, 5, 0, 19, 12, 1, 4, 24, 12, 20, 10, -8, 18, 35, -13, -26, -15, 13}
, {-18, 3, 2, -13, -27, 5, -13, -39, -21, -9, -1, -38, -5, -41, -12, -2, 8, -11, -3, -34, -10, 3, -1, -5, 1, 5, -23, -30, 13, 4, -12, -28, -13, 17, 15, -2, -19, 19, 9, -13, 10, 1, 1, 21, 16, 21, -16, 7, 5, 33, 27, -13, 1, -8, 7, 18, 4, 7, 6, 5, -1, -11, 35, -24, -10, 19, 1, 10, -19, 21, 0, 18, -2, -9, -34, 30, 5, 15, -27, -7, 13, -13, -1, -5, -3, 0, -32, 17, 3, 9, 29, 0, -9, -15, 14, -4, -10, -9, -3, 17, 3, -45, 25, -13, 20, -11, 12, -31, 2, -35, 14, -27, 18, -9, -12, -1, -3, 12, 12, 8, -52, 9, -16, -10, 14, -10, -20, 15, 17, -50, -19, -10, -23, -41, -28, 9, 24, 3, 8, -19, -5, 4, 29, -27, 14, -32, 0, 5, 7, -10, 11, -30, 0, -4, 2, -19, 14, 33, -32, 20, 19, 4, -2, 4, -5, 8, 25, -36, -29, 10, 9, -36, -4, 21, -1, 9, -10, -5, 3, -10, -10, -15, 22, 13, 21, 4, -6, 16, 11, 15, 4, -7, -6, 19, 22, 25, 15, 30, 11, 3, -15, -4, 14, 9, 6, 10, 8, 5, 13, 22, 26, 8, -5, -12, 0, 3, 8, 3, -3, 7, 0, -7, 13, 15, -7, -12, -7, -2, -20, -4, -35, 0, 4, -17, -27, -24, 12, -11, 6, -33, 5, -24, -35, -5, 5, -24, -5, 1, 1, -4, 0, 1, -1, 5, -16, 18, 7, -15, -19, -9, -10, -30, -4, -6, 11, 12, 0, 38, 6, 9, -14, 1, -5, -7, -4, 8, -1, -23, -18, -15, 21, 4, -13, -26, -2, -7, 13, 0, -1, -20, 19, 1, -2, 7, 9, 28, 31, 37, 26, 31, 9, 20, 5, 10, 2, 15, 15, 48, -3, 27, -17, 11, 4, 4, 15, 0, -8, -11, 23, -2, -10, -24, -26, -13, 16, -14, 7, -15, -16, 11, -1, -4, 2, -8, -9, -2, -25, -2, -9, -5, 4, -9, -7, -15, 4, 7, -22, 23, -8, -3, 4, -7, 10, 22, -3, 12, 10, 2, -20, 8, 12, 1, 5, 18, -1, 15, 15, 9, 1, 5, 12, 1, 2, -14, 4, 8, 1, 7, -12, 9, -15, 6, -8, -12, 2, -11, 9, 10, -3, -4, -17, 20, -2, -32, -34, 20, 8, -13, -6, -18, 1, -2, 1, 1, 5, -12, 1, -19, -4, 10, -20, -9, -12, 4, 6, 16, -5, 25, 22, -2, -10, 24, 31, -5, -2, -25, -21, 19, 3, 0, -13, -26, 8, 21, 21, -18, -3, -13, 9, 9, -16, -9, 0, 6, -31, 22, 11, -22, -10, 21, 22, -9, 36, 16, 19, -22, -13, 22, -12, 4, 3, 11, -11, 14, 4, 33, 17, 11, -9, 22, 12, 9, 4, -1, 32, 27, -9, -4, 18, -19, -15, 10, -23, -18, -7, -27, -7, 3, -1, 1, -22, 7, 4, 4, 18, -30, -10, 26, 19, 4, 5, -1, 13, 36, -1, 11, -11, -33, -15, 9, 19, -11, -4, 24, 15, 28, -15, 14, 2, 10, 1, -14, -3, 11, 14, 13, -8, 6, 0, 14, 4, 3, -4, -12, 15, 4, 32, 1, 19, 16, -23, -11, 26, 7, -20, -9, 8, 11, 8, 9, -25, 2, 27, -18, -5, 2, 14, 0, -8, -10, 24, -2, -13, 8, -3, -5, 5, 12, 6, -7, 13, -3, 2, -7, -5, 30, -6, -8, 25, 20, 4, 23, 10, -16, -18, -11, -11, -43, 2, 32, 3, 9, 15, -20, 10, 12, -8, -8, 19, 2, 4, -7, -15, 23, 15, 1, -11, 0, 1, 17, 0, 7, 0, -2, -4, -20, -25, -24, -5, -12, 8, -1, -10, 19, -24, 7, -10, -10, -9, 12, 8, 18, 20, 7, -4, 1, 12, 6, -6, -21, -15, 8, -5, -9, 22, 31, 4, 9, 7, 10, 3, 12, 8, -9, 6, -1, -32, 11, -1, -32, -11, -1, 4, 7, -13, 4, 22, 20, -10, -13, 5, -16, -31, -19, -4, 4, 9, -13, 1, 10, 29, 12, 33, 3, 9, 8, 3, -3, 12, -1, 14, -45, -4, 4, 26, -18, -45, 8, -1, -11, -19, -17, -8, 22, -13, 3, -4, -22, 2, -13, 5, 7, 14, -5, 15, -17, -1, -8, -3, 5, -9, -14, 2, 19, 3, 7, 0, -21, -7, -7, -7, 1, 13, -19, -21, 11, 8, -6, 3, -1, 16, -12, 4, 16, -9, 8, -3, -19, -8, 6, 9, 21, 2, -18, -25, 18, -1, 16, 2, -7, 6, -6, 14, 7, 22, -2, -21, 6, 5, -10, 0, -2, 9, -11, -3, 9, 8, 10, 3, 4, -6, -17, -1, -2, 7, -2, -5, -33, -3, -14, -2, -31, 8, -7, -21, -20, -13, 15, -16, -15}
, {4, 23, 3, 0, 6, -10, 21, -3, 11, -10, -3, -8, 3, 22, 35, -1, -10, 6, -1, -8, 0, 10, 31, -23, 9, -2, 12, 24, -8, -10, -3, -23, -37, 11, 0, 10, -20, 0, 18, -9, -11, -4, 3, -32, -9, 13, 5, -20, 5, 3, 2, -11, -5, 15, 22, -6, -11, -3, -10, 11, -19, -9, 17, 4, 9, 19, 15, 1, -26, -13, 25, -28, 2, 25, -4, 23, -5, 13, 15, -11, 12, 1, 3, 12, 2, 6, 24, 32, 8, -2, 1, -31, -11, 3, 7, 9, 15, 3, -7, 3, 9, 6, 14, 11, 24, 24, 17, 9, -6, -15, -13, 41, -11, -6, 3, 10, 8, 14, -21, -19, 33, 2, 8, -7, -12, 6, 0, 1, 7, 14, 12, 0, 1, 5, 12, 24, 13, 20, 12, 13, 4, 5, -5, 20, 6, 15, 9, 28, 4, 27, -26, 3, 8, 1, 9, 5, -6, -29, 4, 15, 15, 8, 8, -25, 16, 4, 3, -26, -24, 9, 3, -41, -7, -17, -13, 17, -19, 6, 0, 2, -10, -2, 7, -29, 22, -10, -13, -4, 15, 8, -13, -20, -5, -5, 2, -19, 1, 8, 12, -20, -10, 11, 12, -30, 6, -3, 21, -6, -12, 1, 1, -5, -6, 16, 10, 5, -9, 3, -10, 8, 28, 2, -12, -29, -8, 4, 12, 16, 11, 11, 25, -10, 2, 28, 19, 6, 12, 11, 9, 18, 28, -8, -5, -15, -4, 2, -20, -14, 4, -1, 11, 3, 10, -19, -16, 14, -19, 11, -16, 12, 29, 23, 19, 6, -5, -25, -3, -6, -3, -3, -8, -2, 2, -19, -2, -5, -8, 3, -4, -48, 6, -2, -4, 18, 1, 10, -31, -8, 19, 13, -6, -12, 20, 2, 14, -15, -20, -14, 7, -20, 2, -18, -15, 26, -4, -32, -17, -37, -5, 3, -9, -25, 26, 7, -6, -1, 4, 28, -29, -1, 2, -13, 0, -19, -35, -18, -12, -22, -23, -7, -17, -14, -7, -4, -6, -6, -13, -27, -12, -5, 4, -21, 0, 11, 11, 4, -42, -13, 8, 19, -17, -16, 9, 1, 1, -13, -9, -12, -5, -21, -26, 3, 12, -12, -4, 3, -6, 10, -33, -9, 18, -3, 6, -17, -1, 6, 13, -10, -9, 0, 25, 15, -10, -32, 12, -36, -16, 8, -7, -19, -6, -3, -8, 8, -2, -12, -8, -5, -30, -4, -31, -14, -5, -19, 4, -14, -34, -2, 6, 2, -7, -10, -4, -18, -11, -31, 9, -38, -27, -17, 0, -25, -9, -30, -6, -1, -5, -31, 1, -31, 5, 1, -30, -39, -17, 8, -8, 5, 7, -7, 4, 15, -20, -24, 10, 0, -21, -17, 4, -43, -15, -17, -17, -29, -29, 8, -5, 7, 15, -10, 3, -13, -7, 3, -13, -9, -21, 5, 6, -27, -12, 7, 10, -4, -14, -43, 11, 2, 10, -19, -7, -3, 0, 7, -11, -9, -5, -2, -16, -8, -23, -18, -2, 26, -20, -6, 9, -17, -19, 7, 8, -4, -16, -15, -5, -1, -12, 9, 5, -9, -9, -19, -7, -17, -12, -2, -10, -3, -11, -9, -17, 6, -1, 20, -5, -10, -17, -15, -3, -23, 11, 5, 2, -13, -2, -23, -6, 6, -15, -14, 2, -3, -5, 11, -5, -9, -3, -1, -25, -16, -14, -21, -13, 3, -25, -7, -8, 3, 9, -24, -15, -19, -10, -12, 3, -30, -9, -10, -1, 6, -17, -5, 14, 9, -31, 9, 7, -4, -15, -30, -7, -12, 9, -7, 2, 2, 1, -16, -7, -14, -2, -18, -1, 10, 0, -14, 9, -24, 7, -21, 13, -5, -20, -27, 20, 1, -2, -19, -5, 22, 25, 1, 4, -5, 22, -14, -17, 4, -25, 10, -7, 10, -26, -12, -10, -3, 28, -20, -9, -17, -7, -20, -12, -5, -15, -9, -4, -2, 14, -3, -11, -20, -3, -13, 2, 20, 8, -14, -18, -2, 12, -16, 1, 22, 4, 15, -7, -2, -1, 16, 5, -1, -7, 13, -21, -5, -17, -2, 11, -27, 3, -5, 8, -29, 2, 17, 0, -21, -21, 15, 9, -6, 2, -9, -2, -6, -1, -4, -15, 0, -24, 7, 0, -27, 4, -2, 15, 5, -29, 1, 5, -2, -14, 14, -12, -33, 0, 22, -4, -17, -17, 12, 11, -12, -15, 15, 1, 8, 6, 27, 18, -7, 32, -18, 6, -26, 22, 28, -9, 1, -35, -2, -18, 12, 5, 14, 2, 8, 22, 16, 5, 21, -2, 1, -3, -13, -16, 8, -13, 16, 2, 14, 16, 39, 16, -6, 7, 16, -6, 46, 20, -12, 14, 1, -3, 35, 18, 4, -5, 7, 9, 5, -27, 32, 7, -18, -26, -10, -18, -17, 5, 4, -29, 3, -12, 8, 0, -6, -8, 1, 6, 7, 5, -8, 19, -10, 11, 2, 15, -6}
, {-16, -19, -24, -5, -42, -34, -43, -24, -10, -11, -28, -5, 11, -56, 6, -53, -12, -5, -27, -5, -6, -16, -26, -22, -35, 2, -35, -47, -27, 2, 0, -40, -18, -30, -23, 1, -8, -32, -25, -3, 0, -5, -13, -15, 10, -6, 12, -27, 11, -9, -9, -12, 5, -27, 7, -10, -9, 0, -18, 6, -6, 17, -14, -1, 4, -17, -3, 6, 10, -5, -3, -8, -20, -11, 2, 8, 18, 16, -14, 3, 16, -7, -6, 0, 8, -10, 8, 5, -14, 12, -23, 11, 17, 20, 1, 6, 11, -12, 18, 6, -24, -16, -14, -2, 9, -9, 12, -13, -4, 31, 16, 14, 5, -4, -4, -1, -8, -31, 17, 4, 15, 12, -12, -7, 2, 10, 13, 17, 21, -16, -16, 7, -28, -26, -16, 12, 9, -3, -2, -28, 9, -17, 7, -14, 0, -1, -21, 0, -7, -14, -13, -20, -7, -6, -22, -19, -4, 7, -5, 13, -5, -1, 10, -4, -1, 5, -10, -18, -23, -15, -5, -13, -6, -18, -16, -29, 9, 6, -4, -12, -2, 1, 8, -5, -39, -4, -11, 1, -27, 16, -1, 11, 1, 21, 9, -8, 8, -3, 14, 3, -7, 4, -4, -9, -8, 6, -5, 15, 3, 3, 10, -28, -5, 9, 6, -30, 13, 9, 4, 1, -26, 6, 2, -1, 3, 6, -17, -14, 3, -3, 14, -23, -6, -5, 8, 23, 11, 8, -8, 8, -39, -5, 5, -15, -5, 3, 22, -22, -5, 14, -12, 6, -2, -33, -10, -41, -14, 6, -15, -20, -2, 10, -11, 12, 11, 13, 12, 13, -6, -6, -3, 12, -5, 11, 5, -7, 3, 6, 0, 16, 26, 12, 9, -4, -21, -36, 11, -3, -7, -15, -2, -1, 11, -3, -11, 27, 29, 6, 2, 36, 6, -21, 22, -25, 12, 1, 17, 18, -4, 5, 3, -11, -34, 0, -19, -21, -17, -6, 10, 16, -6, -10, 4, -20, -10, 25, -1, -4, -15, 3, -3, 11, -5, 1, 6, -9, 15, -23, -23, 18, -2, -11, -8, 5, -2, -4, -31, 15, 6, -4, -13, 0, 19, -15, -1, -6, -12, -6, -13, 1, -16, -29, -14, -12, 3, 5, 10, 7, -2, -13, -23, -7, -2, 9, 24, 0, -2, 2, -5, -7, 1, 13, 6, -2, -1, -10, 5, -19, -35, -23, -4, -11, -17, -6, -14, -15, -3, 3, -1, -14, -2, -5, -13, -35, -6, 1, 7, 8, 1, 8, -10, -14, 5, 11, -14, -23, -22, 0, 17, -1, -2, 7, 8, 0, -10, 13, -5, -8, 9, -5, -19, 2, -12, -5, -5, -21, 5, -7, -3, 8, -11, -5, -2, -14, 5, -23, -9, -13, 6, 14, -8, -13, 6, 1, 28, 15, 3, -27, 9, 10, 4, 14, 5, 8, -3, 11, 21, -6, 11, 8, -2, 44, -14, 9, 16, 5, -6, -17, -2, -2, -11, 3, 16, -1, 5, 21, 6, 6, -7, -3, -17, 9, 2, 0, -3, -23, -1, -5, 14, -5, -5, 11, 5, -19, -2, 13, 16, -4, 16, 0, -9, -1, 9, -1, -3, 3, 1, -18, 6, -3, 21, 14, 3, -2, -3, -21, 21, -19, 2, -14, 7, 12, 0, -9, -3, -7, 6, 8, 8, 8, 9, 6, -5, -6, 12, 1, 3, -18, 8, -15, -13, -2, 13, -12, -17, -5, 10, -6, -9, -16, 6, 3, 0, 2, 5, -8, 1, 7, -11, 6, -15, 6, -5, 13, 10, 29, -8, -13, -1, 0, 23, 16, 18, 9, 7, 21, 17, 4, -4, -4, -3, 19, 10, -7, 6, -11, 1, -24, -9, -1, 6, 14, -9, 17, -12, -5, -2, 11, 3, -5, -2, 9, 36, -6, 4, 10, 8, 0, 23, -8, -7, 6, 6, 11, -1, -7, 6, 0, 2, -6, 7, -4, 26, 0, 3, 29, -17, 3, -1, -3, 14, 18, 19, 12, 12, 19, -11, -18, -9, -4, -16, 0, 0, -19, -9, -1, -21, -17, -8, 7, 1, 7, -18, -10, 14, -7, 10, 12, 1, -20, -12, -2, -26, 34, -14, -24, 5, 6, 17, -40, -26, 59, 25, -15, 19, -19, -4, -6, -17, 6, 23, -18, -3, 48, 0, -28, -18, 9, 26, 19, 27, -25, -21, -17, 11, -4, 1, 7, 5, -12, 17, -39, -1, -4, 25, -26, 5, -10, -7, 1, -12, -9, -8, 21, -3, 24, -22, -17, -1, 17, 30, 12, 21, 1, -22, -23, 6, -19, 12, -19, -24, -9, -12, 8, -1, 1, 20, 6, 4, 14, 30, 1, -26, -26, -5, 25, 5, -14, 25, -24, 3, 5, 2, 0, -8, 3, 0, -7, 8, 11, -3, 9, 3, -4, 7, 10, 0, -1, 12, 2, 15, -3, 19, 9, -6, 7, 1, 29, -8, 12, 28, 38, 18, -4, -19, 10, 0, 10, 30, -13}
, {6, -35, -6, -5, -18, -13, 12, -20, -3, 10, -1, -13, 2, 17, 15, -17, 14, -6, -12, 36, 9, 5, 0, -33, 34, 15, -30, 34, 8, 9, -2, 5, 34, -6, 0, -7, -5, 7, 1, -25, -4, 17, -1, -8, 9, 6, 21, 20, -30, -1, 2, 22, 9, 11, 13, -18, 13, 17, -4, 12, 8, 12, -11, -26, 25, 27, 10, 16, 57, 26, 20, 2, -5, 2, 9, 5, -3, -16, 6, 16, -6, -5, 9, 2, 0, -13, -1, 6, 8, 13, -2, 24, 15, -17, -3, 0, -7, 7, 15, -10, 10, -12, -8, -11, 1, 11, -14, 33, -6, -7, -27, 18, -20, 16, 10, -17, 4, 24, 5, -15, 5, 17, 14, 20, -3, -23, -5, -31, -5, -11, -5, 7, 13, 14, 19, -11, 3, 30, 6, -6, 5, 0, 2, 21, 1, 14, -1, 13, 10, 35, 6, -9, 11, 7, 6, 6, -2, -8, 8, 7, -5, 25, -6, 10, 2, -12, -10, 18, 12, 14, -14, 28, -10, -2, -17, -3, 6, 21, -1, -1, -1, -9, -1, -21, -9, -2, 4, -22, -9, -9, -10, 30, -26, 5, -4, -7, 8, -7, -13, -2, -11, 6, -14, -24, -6, 0, -6, -9, -7, -1, 18, -3, 10, -4, -19, -10, -14, 10, 3, 0, 0, -6, -1, -12, -5, 19, 3, 19, 12, 5, 2, 1, -14, -17, -10, -21, 4, 13, 3, 9, 15, 1, 13, -11, 8, -13, -4, 13, 8, 4, 5, -8, 8, -4, 11, 19, 1, -2, 1, -23, 5, 7, 6, -24, 4, -17, -3, -17, -2, 4, 1, 7, -9, -7, -6, -27, 4, -23, 10, -26, -4, 1, -1, -4, -18, 18, 4, -24, 4, 12, -2, -9, 12, -6, 12, -9, -13, -26, -14, -29, 9, -3, -1, 4, -5, -2, -9, -18, -2, -18, 8, 19, 22, 0, -12, -5, -19, -7, -9, -13, 1, 14, 0, -5, -16, 14, -15, -1, -1, -5, 0, 1, -7, 2, 8, 14, 6, -10, -12, 4, 1, -14, -14, 2, 2, 1, -17, -2, -16, 5, -16, -1, -23, 2, 19, -18, -6, 6, 4, 8, -9, -8, -9, 0, -5, 20, -13, 22, -1, 10, 13, -11, 5, 3, 21, 9, 4, 4, 3, 8, -6, -9, 16, -7, -14, -18, -2, 17, -8, -5, -5, -18, -22, -11, -6, -25, -2, -6, -10, -8, -5, 5, -3, -18, 3, -1, 0, 10, 6, 10, -27, 13, -3, -5, 15, -12, 4, -13, 8, 0, -6, -13, -21, 12, 9, 23, 2, 2, 10, 12, 5, -10, -10, 1, 8, 11, -3, -6, 14, 1, 10, -1, 0, -16, 13, 14, 8, 4, 4, 7, 2, 2, -11, 12, 16, 14, 1, -24, -15, -20, -3, -3, 4, -27, 6, -11, 4, -22, -8, 5, 1, -4, -15, 7, 24, -15, 5, 8, -14, 11, -21, 0, 4, -3, 3, 9, 8, -1, -10, 1, 1, -18, 3, -17, 1, -5, -1, -14, 3, -8, 8, 20, -1, -26, -14, 11, 9, -16, -2, 3, -5, -7, 5, 9, -10, -8, -1, -10, 8, 10, 16, -12, 13, 8, -4, -5, 7, -1, -3, 3, -7, -3, 8, 18, 1, -23, 19, -3, -6, -5, 16, -1, 8, -4, 9, -4, -5, -7, -28, -9, 10, -6, 21, 2, -7, 28, 2, 7, 45, -13, 8, 10, 3, 0, -5, -6, 34, -5, 4, 0, -6, -20, 23, 9, 5, 20, -2, 14, 5, 7, -4, -6, 13, 7, -10, -4, 4, -8, 6, 3, 4, 10, 0, 5, 5, -2, -9, 3, -6, -22, 24, 9, 4, -5, -5, -1, 8, 16, 3, -14, -16, -3, -9, -8, 8, -16, -5, -6, 1, -15, 5, -3, -20, -2, 2, 7, -11, 10, -6, 19, -15, -16, -7, -2, -20, 10, -14, 8, -1, -5, -1, -11, -10, -17, 5, -2, 8, -22, -33, -1, 2, -28, 9, -7, -36, 12, -64, -23, -15, -10, -9, 3, -6, -34, 9, 8, -6, 0, -6, -41, -12, -31, -11, 5, 10, -6, 12, -1, 2, -25, -23, 38, 3, -36, 3, 7, -7, 12, -52, 2, 8, -15, 8, 6, -26, -12, 6, 3, 12, 21, -9, -45, -22, -34, -10, 0, -11, 26, 17, 23, -28, 8, -5, -28, 5, 15, 0, 15, -25, 19, 0, 1, 8, 6, 8, 1, -11, 14, 12, 16, 14, 3, -4, -14, 9, 5, 8, -2, 8, -2, 19, 18, 7, -20, -11, -7, -2, -31, -2, 3, -3, 8, 30, -8, 0, -28, -7, -3, -8, -15, -7, 1, -10, 6, -5, 27, 19, 29, 10, 7, 2, -7, 15, -7, 9, -11, -24, -15, -31, -21, 6, -6, -17, -7, -3, -19, -23, -15, 1, -1, 17, -9, 19, 10, -15, 19, -12, 23, 0, -14}
, {-2, -10, -9, 9, -8, -8, -15, 19, 20, -5, -14, -37, 4, -13, -12, 0, 19, 6, 3, -23, -4, -6, 1, 10, 6, -1, 6, -23, -12, 9, 10, -1, 19, 3, -27, 30, -38, 13, 10, -12, -35, -40, -21, -1, -11, -12, -19, -33, 23, 1, -12, -15, -5, -6, -4, -38, -9, 0, -3, -20, -10, 1, -27, -1, -18, -12, 12, -33, -25, -1, 1, 21, 11, 6, -4, 60, 0, 8, -22, -7, -22, 13, 6, 10, -7, 19, 16, 15, -53, -8, -1, -24, -12, -1, 14, -30, -39, 11, 6, -1, -9, 13, -2, 18, 15, -35, -20, -5, 6, -3, 0, -7, 33, -5, -7, -14, -9, -16, 7, 28, -13, 2, -6, -5, -5, -20, 20, 31, 7, -29, -15, 10, -19, -29, -6, -11, 10, -1, -12, -27, 7, -21, 12, -17, 5, -1, -17, 1, -2, -14, -15, -19, -10, -8, -34, 4, 5, -6, -10, 7, -6, -4, -7, 14, -5, -6, 9, -9, 3, -1, 4, -35, -3, 24, 4, 1, -10, -16, 12, -17, -2, 4, -10, 38, 15, -3, -7, 20, 28, -9, 6, -20, 28, -7, -21, 20, -2, -8, 21, 13, 10, 0, -9, 23, 0, -8, 22, -17, 7, -13, -14, 8, -6, -15, 12, -6, 8, 10, -3, 12, 2, -9, -7, 13, 9, 11, -5, -3, 8, 1, 9, 6, 15, 24, 5, 2, 0, -8, 16, 6, -11, 24, 6, 21, -8, -7, 10, -5, -8, -8, 5, -2, -31, 1, -4, 2, -12, 13, -3, 17, -11, 1, -4, 25, -1, 11, -2, 18, 6, -14, 0, -2, 17, -2, -11, 29, -3, -1, -9, 15, -10, 9, -14, -2, -7, -21, -9, 18, -19, -1, 6, -16, -18, -2, 1, 8, 1, 8, -6, 19, -3, 9, -2, -16, -12, 17, 10, 25, 1, 0, 0, 13, -27, 9, -11, -9, -13, -17, 17, 0, -4, -13, 19, -15, 18, -6, 24, -10, -10, 5, 12, 0, -3, 8, -8, 3, -6, -15, -4, 5, -2, -6, 14, -19, 13, 11, -9, 23, 18, 12, -11, -11, 15, 8, 5, 5, 22, -1, -2, -6, 2, 5, 1, 18, -7, -18, -6, 2, 6, 3, 3, 21, 9, -27, -14, 10, -4, -6, -3, 2, -2, -11, -7, -38, -1, 7, 1, 5, 6, 26, -3, 34, 19, 20, 6, 10, -7, -1, -4, 14, -6, 6, 1, -14, 3, -29, 2, -16, -8, 10, 1, 16, -3, -20, 0, -3, -13, 27, 10, -1, 4, 22, -4, -9, -4, 2, 3, -2, 3, 4, -10, 6, -3, 29, 22, -11, -7, -14, 5, 4, 14, -6, 0, 1, -32, -4, 15, -8, 1, 11, 6, 3, 5, 13, 8, -16, -4, -9, 3, -6, -10, 5, -9, 2, 7, 23, -18, 3, -8, 2, 0, -5, -3, 2, 2, 6, -14, 36, 12, -7, -8, -5, 5, 4, 10, -21, -10, 2, 2, -14, -15, 1, -9, 21, -4, 18, 0, 24, 34, 13, -4, -32, 10, 33, 14, -9, -13, -5, -13, -4, 20, 14, -6, 10, 15, 16, -29, 3, -23, -9, -16, -20, -19, 13, -6, 10, -10, 10, 8, 25, 17, -15, 3, -26, 28, 6, 22, 8, -9, -2, -31, -15, 20, -12, -43, 10, 17, 14, -6, 15, -22, 8, 11, 12, 2, 3, -9, 16, -27, -9, 4, 8, 14, -3, 6, -3, -1, 21, 6, -8, -2, -8, -21, -27, 16, 8, -11, -27, -11, 19, 13, 14, -20, -3, 16, -1, -5, -28, -12, 16, 9, 3, -1, -10, 2, 12, -7, -3, 4, 5, 14, -1, -6, 7, -12, 0, -6, 4, -2, -8, -6, 9, -4, -19, 10, -11, -14, -22, -6, 0, -11, 2, -18, 1, -8, -20, -12, 11, 6, -20, 1, 24, 2, 3, -33, 3, -15, 0, -10, 14, -5, -1, 5, 15, -3, -3, 11, 4, 32, 0, 20, 0, -3, 10, 14, -10, 8, 10, 23, 6, -7, 8, 1, 15, 1, 4, 22, -12, -3, 13, -10, 14, -8, -11, 9, 8, -32, -11, -7, 15, 9, -20, 3, 16, 2, 11, 10, -17, 31, 17, 1, -6, -6, 20, 14, 25, 4, -13, 12, -19, 10, 1, 17, 17, 10, -12, -10, -23, -12, -22, 15, 12, -8, 13, 2, 16, -10, -8, -14, -2, 12, 2, -1, -21, 8, -6, -5, 14, 5, -3, -2, 3, 33, 17, 0, 12, 16, -1, -9, 2, 19, -7, -5, 2, -1, 29, 16, 18, -6, -5, 19, 1, 1, -25, -1, -3, -10, 15, 5, -19, 8, 6, 17, 14, 8, 19, -25, 1, -1, 9, 13, -1, -13, -2, 14, 21, 13, -12, 2, 23, -6, 7, 8, -8, 18, 20, -6, 0, 0, 13, 19, -26, -24, 4, 6, -22, -1, 43, 7, 12}
, {3, -52, -45, -10, -46, -44, -41, -26, -9, -28, -31, -19, 0, -86, -13, -87, 14, -43, -55, 14, -7, -17, -48, -32, -18, -2, -34, -55, -4, 25, -23, -25, 44, -58, -51, 1, -27, -38, -23, -18, -24, -6, -20, -21, 3, -39, 16, -36, 7, -29, -4, 2, -9, -26, -13, -56, -34, -8, -62, -23, 24, 9, -34, -4, 34, -7, 4, -4, 18, -1, -27, 18, 7, 12, 16, 35, -9, -7, -17, 9, -29, 5, 2, -24, 2, -39, -14, 2, -10, -11, 0, -19, 24, 14, 10, -21, -10, 14, 20, 15, -22, -8, -21, 20, 24, 19, 8, 10, -5, -10, -24, -9, 19, 1, 12, -40, -12, -27, -2, 14, -21, 4, 2, -8, -19, -12, 13, 8, -1, -25, -13, 19, 0, -15, 4, 25, 13, 15, -8, 7, 10, -36, -11, -24, 18, 4, 3, 5, -9, 5, -15, -7, 4, 0, -13, -18, -16, -23, 11, 17, -5, -5, -13, 4, -19, -5, 6, -21, -40, -2, 0, -30, 4, -53, -20, -19, -4, -15, -20, -14, -3, -1, -38, -8, -29, 5, 7, -19, 13, -1, -9, -10, -1, -13, -19, -22, 17, -1, 15, 6, 5, -12, -7, 7, -11, -30, 19, -19, -3, -18, -6, -1, -4, 7, -16, -7, -23, 7, -5, -7, 13, 23, -5, -11, 11, 6, 1, -16, 3, 16, 21, -10, 8, 16, 4, -11, 0, 11, 16, 10, 2, 18, 27, -18, -12, -1, 21, 12, -5, 7, 23, -1, -21, 1, 18, -14, -32, 3, 6, 14, -2, 16, -2, 12, 7, 9, 14, 9, 4, 7, 11, 13, 3, 19, -5, 13, -5, 11, 12, 9, 28, 6, 7, -2, -18, -24, 17, 16, -21, -5, 6, -6, -16, 0, -23, 11, 18, 3, -16, 18, 1, 10, -8, -10, -11, 2, 6, 13, 2, 3, -24, 10, -11, -3, 10, -3, -23, -19, 24, 0, -12, -19, -10, -15, -21, 5, 4, -2, 1, -1, -6, 31, 8, -4, 25, -14, 3, -2, -6, 30, -5, -8, 10, -28, 1, 2, -17, -1, 26, 6, -17, -14, -18, 0, -6, -21, 1, -23, -1, 18, 16, 5, -7, 21, 4, -1, 46, 11, -20, 19, 28, 13, -10, 12, 55, 26, 0, 0, 0, -5, 16, 2, 32, -36, -9, -5, 12, 1, -6, -7, -7, -8, -14, 1, -10, -14, 2, 14, 11, 1, 13, 13, -2, 26, -3, -10, 8, 5, 8, -6, 3, -14, 17, 3, 15, 4, -6, 23, 13, -2, -4, -1, -7, 12, -4, 5, -4, 3, 5, -3, -19, -5, -5, 11, -5, -1, -7, 13, -5, 11, -5, -8, 20, -5, -16, -3, 1, -10, 4, -19, -21, -20, 15, 10, 3, -10, 2, -10, 1, -39, 1, -15, -1, 0, 12, -24, -9, 6, 3, 5, -24, -25, -12, -2, 2, -1, 6, 0, -27, -14, -11, 8, 26, -17, 14, 28, 28, -14, -9, 5, 6, 6, -7, 14, 1, 5, -43, 35, 18, -17, -8, 10, 28, 23, 0, 9, 7, 6, -14, -32, 34, -35, -44, 31, 14, 9, -1, 17, -1, 26, 16, -15, 6, 19, 0, 35, -12, 3, 17, 33, 26, -9, -7, -8, 28, 37, 12, -8, 14, 9, -28, -17, 21, 24, -35, 5, -5, 11, 0, -7, -27, 4, -1, -3, -3, 8, 7, 7, -19, -7, 16, 12, 3, 4, -2, 8, 4, 13, 10, -1, -2, 11, -22, -6, 13, 34, -7, -8, 3, 3, 30, 16, -1, 6, 13, 8, 1, -32, -9, -3, -7, -1, 10, -22, -11, 4, 4, 9, -9, -7, 1, 5, -11, 31, 6, 21, -25, 11, 7, -11, -13, -11, 25, -16, -4, 2, 18, -2, 17, -2, -1, -5, 0, 15, 2, 1, -3, 26, 5, 20, 13, -11, -8, 8, -2, 11, 7, 41, 11, 5, 1, -1, -2, -9, -9, -9, 1, -18, -14, 14, 5, 10, 0, -19, -11, -6, -52, -3, -21, -15, -12, -15, -27, -4, 1, -3, -2, -5, -5, -15, -15, -37, -28, -2, -3, -10, 25, 9, 8, -3, -1, 8, 18, -3, 7, -12, -3, 9, 4, 12, 11, -25, 4, 7, -2, 10, -9, -3, 20, 5, 8, -19, 3, -1, -11, 8, -1, 8, 4, -8, 0, -28, -11, -11, 11, -14, 7, -3, -19, 5, 7, -7, 5, 3, 5, 16, -19, -15, 9, -3, -9, 10, 31, -1, -9, 6, 22, 4, 3, -12, -14, 10, 21, -2, 3, 18, 30, 13, 8, 13, 21, 13, -19, -10, -8, 17, -7, 31, 18, 7, 18, 6, -1, -4, 9, 15, -1, 12, 1, 24, 33, -9, 30, 10, 23, 10, 25, 41, 45, -7, -7, 6, 41, 33, 20, 26, 20, 13, -6, 25, 44, 22, 22, 1, 13, 32, 23, 36, 16, 9}
, {13, -12, 8, 12, -14, -2, 8, -1, -14, 13, 4, -24, 3, -10, -32, 13, 26, -10, 9, 25, 3, -8, -6, -17, 7, 7, -20, 22, 9, 12, -6, 29, 48, 20, -1, -26, 22, 6, -3, -31, 10, -3, 26, 1, 6, -10, 17, 31, -17, 8, 26, 37, 9, -6, 3, -2, 18, 5, -6, 11, 28, 17, 6, -29, 6, -7, 12, -3, 23, 35, -22, 21, 7, -10, -2, -5, -3, -7, -14, 19, -15, 2, 7, -10, 8, 12, -26, 13, 3, 12, 13, 17, 7, -7, 3, -5, -19, -12, -17, 2, -23, -30, -11, -7, -8, -14, -19, -9, -1, -41, 7, -32, 12, -7, -9, 11, -7, 29, 4, -11, -28, 2, -5, -5, 0, -4, -1, 15, 11, -19, -1, 0, -4, -9, -3, -5, 0, -4, -3, 2, 5, 4, 9, -7, 1, -9, -11, 2, 4, -6, 11, -15, -13, -1, 13, -9, 6, 28, -13, 1, -18, 17, -4, 35, -4, 13, -12, 12, 25, 0, -5, 19, -2, 9, 23, 1, 19, 6, 0, 10, 9, -4, -9, 20, -10, -5, 16, 2, 5, 1, 19, 49, -12, 13, -2, -1, -3, 17, -9, -20, -9, 4, -9, -4, -5, 5, -17, 5, 3, 18, 15, 16, -7, -2, -13, -5, 15, 8, 5, -8, -14, -5, 8, -7, -5, 16, -5, 2, 25, 19, -23, 15, -9, -10, -2, -44, 13, -15, -18, -4, -9, -8, 8, 3, 7, 4, -4, -4, 5, 0, 27, -6, 12, 4, 9, 29, -6, -16, -6, -24, 7, -24, -16, -13, 23, -2, 2, -7, -5, 11, 5, -1, -6, -12, -6, 5, -7, 16, -28, -23, -20, 3, 3, 8, 7, 13, -9, 7, 8, -16, -12, -1, 39, 0, 0, 0, 18, -2, 27, 22, -10, 17, 45, 6, -10, -18, 7, 48, 7, 11, -13, 2, 21, 4, 22, 7, 22, 6, -9, -2, -17, -9, -13, 7, 2, -12, -22, 5, 1, 6, 9, 13, 7, 4, -18, 5, -10, 20, -4, -12, 1, -15, -4, 3, -3, 8, -8, -10, -14, -20, 3, 1, -14, -14, -4, 16, -19, 26, -17, 12, -5, 2, -20, -5, -11, -3, 6, -6, 3, 3, 10, -28, 0, 0, -24, -11, -15, 11, -14, 6, -15, -9, 3, 8, 7, -7, 8, 8, 5, -16, -18, 1, -12, 3, -8, -3, 7, -11, -7, -15, 16, 6, -10, -17, -3, -27, 2, 10, 3, 2, -24, -8, 0, 12, 5, 13, -1, 9, 23, 4, -7, 0, 6, 19, 13, 19, -4, 26, -4, 12, -37, -3, 5, 10, 36, 6, 9, -8, -3, -3, 2, 0, 13, 4, -11, -1, 24, 6, 21, 24, -11, 12, 18, 4, 26, -6, -6, 33, 22, 18, -5, -13, -8, 10, 1, 8, 21, -8, 0, -2, 0, -8, 5, 4, 26, 4, 7, -29, -9, -13, -21, -8, -22, -17, -18, -17, -16, 21, 23, 12, -6, 9, -5, 4, 12, -36, 3, -8, 6, 9, 4, -6, 19, 8, -29, 10, 7, -45, 9, -26, 6, -5, -30, 6, -10, 11, -24, 0, 15, 4, 24, 4, 6, -18, 10, 2, -10, -30, 5, 12, -4, -9, 2, -4, 15, -16, -1, 6, 4, -28, 11, -16, -13, 17, -5, 0, -1, -7, -38, -10, 18, 11, 11, -6, -24, 14, -5, -3, -11, -15, 19, -1, -4, 21, -5, 15, 14, 4, -11, 14, 12, -18, -6, 10, 4, 15, 13, 16, 11, 19, -6, 10, 0, 0, -32, -11, 4, -20, 0, 17, 18, 6, 4, -20, 15, -5, -1, -9, 14, -3, 17, 9, -3, 13, 8, 20, 15, 14, 21, 12, -20, -8, 1, 18, -4, -32, -4, -7, 4, -25, -5, 15, 2, 16, 7, 0, -12, 3, -2, 21, 6, 6, -12, -1, 14, 19, 5, 12, 6, -10, -4, -7, -15, -10, 11, -5, -15, 22, -9, 5, 1, 13, -13, 8, -16, 13, -17, -19, -35, 15, -2, -30, -19, 23, -1, 4, -6, -1, -16, -22, 3, 4, -17, -10, 17, -7, -2, 2, 8, 0, 13, 41, 33, 2, 12, 13, -11, 4, 13, -7, 33, -34, 9, -3, -16, -3, -12, -4, -3, -27, -47, -28, -2, -12, 6, 0, 21, -7, -17, 15, -9, -5, -5, -2, -2, -9, 2, 1, 1, 8, -1, -6, -9, 31, -2, -5, -5, -38, 34, -2, 14, 12, 19, -6, -23, -7, 12, -11, -2, -10, 5, 1, 16, -8, -12, -8, 11, -36, -2, 0, -10, 7, -10, -23, -14, 5, -10, -3, 23, -36, -2, -3, -13, -9, 5, -4, -16, -11, 11, -7, -13, 23, 28, -15, -18, -1, 5, 19, 18, 9, 1, -9, 21, 13, 2, -2, -12, 16, 1, -4, 3, 7, 24, 3, -17, 11, -27, 15, -7, 1}
, {-10, -17, -29, -16, -17, -35, -18, -30, -24, -15, -15, -13, -3, -27, -5, -41, -35, 5, -24, -26, -5, -21, -4, 7, -22, -2, -30, -36, 5, -13, 3, -50, -37, 17, 18, 21, 6, 24, -2, 17, 10, -9, -22, 14, -6, 25, -43, -14, 7, 10, -2, -70, -4, -15, -16, 11, -35, -7, 29, -3, -30, -39, 12, 6, -61, -5, 1, 24, -41, -9, 11, 7, -6, 17, 15, 17, 3, 16, -30, -1, 31, -14, 8, -16, -8, -23, 9, 10, -33, 2, 2, -9, -48, -30, 0, 18, -14, 3, -5, 4, 24, 29, 0, 8, 18, -2, 18, -21, 2, 13, 5, 0, 8, -19, -3, -33, 0, -34, -1, -10, 31, 1, -1, 5, -10, 3, -15, 23, -15, -9, -2, 8, -26, -8, -5, -17, -15, 21, -7, -31, -2, -29, 3, -20, 1, 1, -16, -12, -9, -18, 3, -14, -12, -8, -38, -11, 7, -1, -13, 10, 2, -4, 9, -12, -10, 14, 1, -19, -20, -8, -28, -28, 1, 6, -27, -9, -30, 14, -19, -8, 3, -7, 22, 1, 11, 5, -27, -5, -5, -5, -6, -10, 22, 1, 5, -3, -5, 8, 4, -26, -10, 10, 5, -31, -1, -11, 4, -6, -5, 0, -12, -35, 2, -6, -1, -20, 1, 3, 11, 5, 6, -29, -6, 5, 17, -1, 2, -8, 6, -6, 9, -5, 2, 18, -1, 5, 12, -17, 12, -6, -7, 2, 0, -4, 5, 6, -11, -24, -5, 7, -14, 0, 3, -4, -14, -32, -8, 25, -6, 18, -1, 9, -2, 20, -16, 17, -5, 11, 2, -3, 7, -18, 13, 9, 18, 5, 4, 7, 4, 3, 13, -6, 14, -11, 5, -12, 13, 24, -9, 2, 12, -8, -18, -10, -20, 2, -20, 6, -13, 8, 4, -19, 0, -16, 1, 28, 12, -2, -6, -19, 21, 3, -32, 2, -15, -21, -14, 13, 10, -1, 6, -33, -2, -1, -19, -8, -13, -42, -13, -21, -35, -1, 7, -3, -9, -6, -3, -42, -25, -18, 3, -13, -36, -24, 6, 4, -33, 5, -2, 13, -24, -25, 15, -12, -23, 2, 10, -35, -13, -42, -25, -6, -9, -20, -3, -12, -37, -1, -8, -35, -34, -7, -2, -15, -16, -13, 14, 3, -32, -1, -9, -12, -7, -41, -1, 21, -7, -4, 14, 5, 10, 20, -1, 11, -2, -10, 8, -11, -1, 6, -11, 10, 12, 9, -4, 3, 7, -24, 12, 4, -5, -4, 4, -6, -13, -19, -2, 12, -7, 3, 30, -5, 11, -7, 0, -19, 2, -27, -2, 24, 10, 8, -13, 1, 13, -15, 0, -6, 15, 5, 6, 1, 5, 14, 11, 11, 4, -6, -16, -31, 20, 1, 14, 4, 6, -3, -1, -49, 0, -30, -3, 23, 13, 18, -4, -16, -10, 13, -5, 18, 12, -6, 5, 0, 2, 30, 10, 11, -12, -15, 14, -16, -1, 5, -13, -14, 4, -6, -8, -17, -23, 0, 4, 4, 9, 0, 22, 9, 1, 2, -11, -25, 0, -12, 4, -2, -14, -15, -5, 14, -5, 15, 2, -21, -12, 0, -33, -24, -20, 5, -14, -37, -49, -26, 19, -14, -7, -34, 2, 7, -12, -9, 7, -34, 6, 6, -21, -4, -19, -17, -16, 8, -13, -12, -8, 7, -1, -7, 16, 8, -9, -4, -10, 6, 16, -42, 0, 10, -26, 7, -9, -15, -9, -19, 9, -12, -5, 7, 12, -4, -26, 9, -6, 11, -12, -3, 18, 2, 3, -10, 26, 10, -1, -14, 0, 9, 18, -4, 14, 6, -10, 16, -15, -9, 2, 2, 2, -3, 1, -4, 25, 6, 2, 5, -5, 6, -17, -13, 19, 4, 32, 6, 1, 9, -1, 1, 5, 6, 21, -24, 7, -2, 23, 12, 9, -14, 24, -18, 6, -6, -3, -13, 30, -11, -8, 7, 1, 17, -24, 21, -4, 7, 22, 3, 17, 11, 13, -12, 14, -18, -1, -22, 11, 19, 6, 5, -8, 8, 15, -25, -5, 36, 9, 5, 23, -8, -7, 19, 6, -12, -13, 0, -31, -6, 9, 17, -12, -5, 1, 5, 17, -2, 22, -28, 10, 4, 20, -4, 3, -1, 6, -7, 5, 29, 23, 6, -6, -6, 7, -17, 12, 10, 8, 9, -9, -4, -1, -1, 1, -16, 17, 18, 19, -3, 3, 7, 7, -19, 46, 0, 12, 3, 3, -21, 6, 7, 9, 11, -49, -7, -13, -8, 10, 30, 6, 27, 18, -1, 4, 3, 20, -8, 17, -5, -19, 24, -3, 21, 7, 16, 13, 17, 12, 21, 8, -6, 2, -8, 7, 13, 15, -9, 1, 11, 22, 15, 2, -6, 11, -2, -8, 12, -4, -2, -7, -6, 6, 8, -5, 19, 2, 12, -1, 17, -17, 14, 27, 2, -6, 2, -18, -42, -1, -13, 21, 0, -14, -13, 11, -9}
, {-23, 21, 12, 3, -1, 15, 16, -20, -37, -17, -27, -27, 13, 4, 33, -31, -10, 0, -3, 34, -7, 3, 16, -26, -12, -6, -7, 29, 12, -12, -10, -27, -37, 15, -13, -7, -22, -18, -9, -40, -20, -4, -22, -29, -4, 8, 16, -22, -45, -5, -12, 20, 4, 4, 2, -7, -32, 13, -13, 21, 10, -15, -5, -44, -4, 14, -10, -27, 17, -4, 0, -4, 6, 16, -1, -11, 1, -4, 15, 2, -13, 9, 2, -5, 1, 2, -16, 12, 6, 9, -12, -14, 1, -21, -1, -5, 23, -8, 2, 8, -17, -17, -17, 15, 7, -2, -11, -6, 1, -4, -7, 14, 17, 1, -11, 15, 5, -6, 1, -9, 25, -4, -9, 6, 1, -1, 7, 0, -2, 2, -13, -8, 10, -4, 4, -10, 6, 3, 13, -12, 7, 17, -8, 3, -4, 13, 9, 21, -6, -3, -9, -1, 15, 8, 8, -10, -2, 8, 10, -15, 17, 7, 0, -17, -2, 14, 5, -18, -32, -17, -15, -11, 5, 18, -46, 5, -4, 3, -3, -14, -1, 2, 1, -15, 16, 1, -10, 1, -9, 24, -1, -5, -7, 3, -11, -16, -7, -15, -14, -3, -31, -7, -20, -34, -2, -19, -45, -22, 6, 13, -21, -45, -9, -20, -14, -37, 4, 15, -15, -4, -18, 23, -4, -6, -16, 13, 8, 6, 14, 7, 2, -16, -13, 0, 1, 10, -4, 8, -30, -18, -8, 7, 18, -7, 8, -5, -3, -12, -24, 12, 2, -3, -1, 1, 4, -14, -17, -6, -4, -15, 10, 24, 11, -1, 4, -10, 8, 0, 1, -6, -3, -2, -3, 21, 9, -21, 6, 4, -4, -4, -18, -7, 12, 10, -12, 8, 11, -15, 23, 3, 0, -1, 3, 7, -5, 6, -4, -12, 1, -5, 9, 3, 3, 3, -1, 1, -25, -13, 0, -3, -4, -10, 12, 12, -20, -11, -20, 22, 5, 4, 18, 8, 3, 2, -13, -1, -2, -12, -22, -18, -11, -19, 10, -26, -1, -15, 22, -5, -11, 1, -6, 3, -12, -31, -5, -4, -4, 4, -1, 15, -16, -9, -11, -25, -4, -18, -9, -7, 10, -23, -30, -19, -26, -4, -4, -11, -9, -4, -5, -15, -21, -27, -5, 27, 19, 4, -3, 7, 3, 10, 13, 1, -8, -29, 9, -7, -7, 12, -4, -44, -2, -28, -24, -15, -1, 14, 7, -12, -6, -8, -4, -21, -9, -11, 0, 10, 10, 19, -7, 6, -6, -1, 15, 24, -11, -39, 25, -3, -9, 8, -23, -20, -4, 1, 9, 25, 2, 1, -5, -8, 5, 10, 2, -11, -20, -6, 6, 7, 8, -3, 4, -9, 2, -13, 5, 11, -18, -14, -27, -6, -4, 0, 3, -7, 13, -3, 20, -22, -10, -32, -3, 4, 14, 5, 4, -14, 0, -6, -9, 10, 11, 20, -20, 7, -12, -2, -5, 10, -11, -8, -1, 6, 12, 8, 3, 18, -3, -40, -7, 0, 0, -15, 5, -15, -2, -15, 16, -23, -11, 12, -5, 24, 4, -35, 9, 13, 10, 20, 15, -9, -14, 1, 8, 2, -8, 0, 2, -17, 24, -6, 18, 10, 11, 23, 0, -4, 13, 6, 0, -14, -6, 24, 8, 21, 15, -1, 10, -4, 6, 14, 17, 1, 12, -6, 25, 7, 10, -12, 3, -14, 14, -8, 2, -4, 10, -8, 15, 4, 43, -2, 1, -1, -6, 27, 8, 18, 12, -9, 26, 11, -6, 16, 13, 15, -1, 0, -12, -3, 3, -4, -4, -6, 15, 24, 21, 8, -4, 24, -4, 8, 17, -1, -4, -1, -15, 2, 2, -3, -8, -13, 2, -5, -2, 0, -20, 7, 0, 2, 4, 1, 5, 10, 12, -6, 1, 10, 11, 11, 29, 21, 0, 8, 14, 25, 2, 28, -1, 25, 9, 4, 0, -4, 21, 2, 1, 5, -13, 16, 22, 7, -1, 4, -12, -1, 5, -6, -1, -13, -24, 3, 0, -24, 2, -6, -16, 3, -38, -19, -13, 26, 1, 5, -23, -5, 22, 1, -22, -7, -10, -43, 2, 3, -21, 20, -6, -9, 10, 20, 5, -39, -20, 43, 6, -29, -2, -2, -15, 8, -35, -18, -8, -16, 7, 5, -10, -35, 4, 7, 6, 23, 8, -17, -21, -22, 6, -5, 15, 5, 0, -6, -14, -35, -1, 2, 0, -28, 11, -13, -13, 16, -6, -6, 2, 18, 9, 14, -2, -22, 12, 11, 10, -4, -21, 0, -2, -20, -11, -6, -8, -7, -7, -9, -12, -2, 15, -17, -2, 13, 14, 6, 13, 2, -7, 9, 3, 9, -10, -15, 5, -4, -11, 5, 1, -17, -16, 2, 24, -1, 12, 15, -9, -2, 12, 10, -12, 8, -12, -6, -25, 10, -4, -6, 2, 5, 12, 27, -8, 29, -2, -8, 16, 58, 32, 5, -5, 24, -7, 4, 40, -12}
, {-15, -41, -34, -8, -24, -20, -42, -35, -18, -19, -42, -12, -17, -49, 5, -46, -22, 0, -28, 0, 7, -50, 14, -32, -5, 20, -47, -32, 3, 13, -2, -28, -14, 4, -12, -16, -37, -18, -43, 18, -32, -22, -38, 3, 11, -17, 2, -52, -35, 14, -21, -30, -7, -60, 3, 9, 1, 18, -39, -41, -4, -18, 5, -8, -16, 17, 13, -22, -26, -6, -15, -19, 11, 10, -3, -22, -4, -22, 4, -32, -19, 24, 33, -41, 2, -11, 4, -5, 18, 8, -16, -16, 14, -3, 10, -11, 11, -5, -23, 27, 4, -14, -5, 20, 10, -17, -13, -8, 1, 14, -6, -24, 5, -26, -13, -17, 1, 8, 2, -23, 4, 2, -11, -5, 11, 17, -24, 6, 7, -32, 4, -14, -10, -18, 1, -29, 0, 3, -2, -48, 13, -14, -6, -32, -4, -21, 17, -2, 7, -6, 13, -19, 6, 1, -27, -44, 9, 15, -2, 0, -5, -11, -30, -9, 6, -11, -12, -25, -4, -9, 2, 0, -3, 0, 31, -9, -25, 27, 17, -10, 0, -6, 19, 5, -6, 1, -7, -3, -2, -8, 23, -37, -31, -35, -20, 19, -49, -1, -35, 29, 28, -13, -20, 13, 5, 17, -7, -46, 19, 7, -3, -3, -1, -28, -5, 36, -6, 12, -5, -38, -28, 10, 3, 23, -6, -22, -6, 29, -38, -11, -41, 40, 35, -11, -6, 22, -10, 21, 9, 0, 35, 25, 16, -10, 4, -24, -1, 42, -11, 17, -5, -32, 7, 21, 43, 41, 8, -5, 6, 21, -12, 11, -30, 14, 12, -15, 2, 5, -8, 5, 14, -6, 0, -2, -8, 1, -5, -26, 22, -8, 1, 17, -9, -11, -11, 14, 13, 11, 2, -23, 2, -6, 6, 9, 1, 1, 9, 19, -3, 9, 5, 23, -24, 7, -7, -7, 1, -17, 2, -22, -10, -5, 19, 16, -17, 19, -22, -6, 3, -13, 11, 28, -13, -16, 8, -7, -10, 5, 31, 26, 13, -16, -12, -23, 18, 17, -4, 12, 27, 7, -1, 33, 6, -24, 0, 3, 4, -15, -9, 18, 2, -9, -10, 12, 14, 8, -5, -20, 10, 31, 45, 8, 21, 38, -3, -11, 30, -4, 4, 53, 50, -21, -5, 31, 33, 2, -44, 19, 31, -23, -2, 3, 23, 6, 2, 20, 5, -10, 28, 0, 9, 19, 31, 2, -2, 26, -10, 11, 20, 4, 31, -8, 16, 53, 7, 22, -5, 11, -28, 4, 38, 22, -7, 0, 3, 26, -6, -12, -8, 2, 10, -6, 9, 23, 4, 2, -10, 1, -11, -2, 31, 15, 8, -4, 6, 23, 8, 24, 2, -5, -10, 7, 7, -7, 3, 11, 2, 6, -20, -10, 24, 11, -32, -10, -7, 2, -3, -3, -12, 7, 5, 11, 1, -7, 10, 30, -11, -5, 0, 3, 23, -20, -4, 3, -14, -1, 19, -4, 1, -10, -1, 2, 5, -6, 12, 22, 27, 10, 2, 26, 30, 2, 2, 11, 25, 18, -19, 14, 19, -1, 0, 17, 13, 12, -5, 6, -11, 33, 4, -4, 25, -17, -7, 10, 24, -9, 32, 35, 34, 2, 25, 33, 34, 0, 1, 25, 10, 44, -4, 13, 40, 3, 3, 37, 5, 24, 9, 13, 28, 23, -11, -12, 3, 5, -9, -1, -6, 21, 18, 21, 8, -8, -8, 6, 27, -14, 2, 21, 7, 5, -8, -1, -5, 16, -8, 20, 2, -13, -4, 11, 35, 24, -28, -21, 14, 10, -7, -3, 23, 7, 5, 7, 22, 8, 7, -8, -1, -14, -6, 7, 6, 3, 12, -2, 8, -7, -11, 29, -9, 3, -1, 6, 19, 5, -21, -28, -11, 10, -25, -1, 3, 22, -15, 3, -18, 0, -4, 4, 5, 21, 2, 19, -5, 1, 12, 4, -12, -28, -6, 15, -12, 38, 10, 14, -20, 12, 17, -31, 15, 6, -5, -6, -6, -22, -7, -16, 18, 0, -16, 14, 5, -14, -16, -13, -23, -4, -33, -8, -17, -27, 1, -2, -22, -26, -1, 2, 3, -2, -6, -27, -22, -27, -12, -1, 30, 7, 23, 9, 12, -13, 21, 26, 28, -41, -6, -13, 8, 26, -1, -25, 8, -10, -11, 10, -9, -56, -10, 14, 0, 15, 1, -25, -28, -13, -20, -2, -2, 7, 27, 1, -5, -20, -12, 11, 20, -22, 1, -7, 4, 0, -15, -46, -3, 0, 5, 0, -25, -62, -14, 3, -7, -5, -19, -37, -24, -13, -26, -5, 19, 12, 0, 18, -11, -11, 4, 12, 16, -13, -13, -4, -46, 15, 8, -27, 1, -17, -8, 3, -20, -13, 17, 16, 4, -8, 23, -39, -19, 2, -33, -9, -1, -16, 13, 11, 8, -2, -19, -7, 13, -6, 3, -12, -54, 2, -26, -6, -21, -20, -7, 12, -39, -5, 9, 6, 6, 1, 6, -49, -3, -10}
, {-9, -1, 7, -5, -4, 0, 19, 23, -7, -13, 2, 8, 7, 17, 5, -6, 20, 16, -3, 35, -9, 16, 21, 18, 5, 4, 12, 16, 29, -1, 17, 41, -2, -7, -7, 0, -6, -10, 24, 0, 5, 6, 7, -15, 4, 18, 11, 5, 5, 4, -22, 20, -6, 9, -1, 39, 10, -11, 11, 32, 22, 4, 33, 7, 16, 16, -15, -11, 20, -4, 13, -20, -14, 20, -3, -5, 3, 9, 5, 9, -9, 7, 3, 21, 0, -4, -22, -15, 13, 6, 1, 24, 4, 4, -5, -7, 25, 6, -11, 12, 2, 5, -2, 8, -1, 16, 9, 8, 5, -29, -8, -3, -1, 15, 10, 11, 0, 4, -11, -18, 13, -6, -4, -8, 3, 3, 20, 8, 7, 14, 14, 2, 2, -2, -6, 10, 10, 0, 10, 9, -6, 22, 5, -20, 2, 15, 26, 8, 4, -3, 13, -5, -22, 0, 13, -8, -3, 7, 7, 8, 3, -6, -13, -27, -4, 4, 6, 6, 15, -13, 0, 25, -2, -8, -3, 12, -2, -17, -1, 38, -11, -2, -15, 41, 1, 6, 0, 12, 1, -1, 11, 12, 12, -4, -14, -38, -6, -4, -18, 7, -6, -5, -1, 0, -7, -17, -7, -6, -6, 2, -22, 7, 4, 2, -18, 12, -15, 6, -4, 8, -23, 0, 5, -7, 6, 2, 3, 10, 9, 3, 14, 13, 10, -11, -8, 13, 5, -18, -23, -10, -10, 6, -5, 8, 2, -1, -17, -19, -1, 7, 0, 10, -15, -9, -12, 18, -16, 9, 12, 14, 15, 8, 29, -14, 12, 5, 4, -8, -2, 16, -3, 12, 19, 13, 14, -15, 1, 2, 14, 24, -6, -6, -3, 2, -20, 25, 11, -10, 23, 6, 2, 8, -4, 9, -4, -4, -28, 6, 15, 8, 5, 18, 0, 7, 7, 2, -7, -16, 8, 6, 23, -13, 21, 4, 19, 18, 9, 19, -8, 9, 12, -1, -15, 0, -9, 8, -22, 8, -2, -23, -15, 5, -1, 12, -15, -6, 2, -14, -2, 21, 2, -4, -26, 4, -10, -9, 31, -1, -9, -12, 3, 13, 10, -17, -22, -17, -17, -9, 20, 3, -4, -4, -10, -6, -7, -17, 1, -3, -6, -14, -12, -5, -10, 9, -11, -6, 6, -7, 11, -14, 12, -9, -3, 1, -11, -8, -19, -14, -4, 4, -6, -10, -3, 5, -12, -12, -2, -15, -7, -10, -18, -11, -18, -2, 8, 26, -1, -6, 1, -13, 19, 0, -3, -7, -8, -11, -5, -6, -7, -3, 5, -13, 1, -6, 14, 16, 15, -13, -9, -16, 13, 5, -4, 1, 5, 6, -1, 22, 18, -15, 11, -5, 12, 8, 12, -12, -6, -19, 9, 17, 18, 0, 7, -8, 12, -3, 1, -5, -2, 7, 4, 5, 15, 4, 14, 5, 10, 18, -4, 21, 20, 8, 5, 1, 4, 7, 21, -4, 2, 0, 12, 4, -9, -5, -22, -21, -15, -4, 2, -13, -23, -6, -3, -25, -12, -10, 35, -34, -26, 20, -10, -14, -16, -8, 9, 6, -8, -16, -13, 10, -21, 41, -3, -30, -21, -23, -9, -16, -3, 1, 3, 10, -2, -9, 6, -10, -3, -11, -20, -13, -13, -2, -2, -14, -18, -4, -9, -10, -8, -16, -22, -3, 7, -10, -6, -9, 12, -5, -11, -4, 0, -7, -1, -19, -14, -18, 4, -10, 4, -6, -22, -6, 5, -32, 8, -3, 11, -7, 8, 5, -6, -9, -29, 11, -16, -14, -2, 11, 7, -13, -10, 2, 22, -24, -6, 18, 6, -8, 11, 19, -9, 2, -15, 4, 15, -21, -5, -11, -9, -29, 19, 0, -11, 10, -18, -9, 0, -1, 9, 4, -7, 7, 15, 4, -6, 15, 25, 23, 29, 30, 9, -4, 25, 33, 7, 6, 3, 25, -8, 9, -3, 11, 26, -9, 22, 27, 16, -6, 19, -9, 2, 3, -12, 17, -16, -11, -19, 11, 2, -29, -23, 11, 10, -7, 13, -24, 12, 8, -10, 46, 0, -7, 12, 14, -8, -6, -13, -13, -14, -1, 0, 10, -2, -25, -2, 0, -23, -8, -20, -4, -4, 3, -23, -7, -1, -5, 8, -17, -6, 22, 2, -19, 7, -25, 14, -11, -8, -4, -24, -19, -5, -15, -1, -14, -29, -16, 18, 24, -14, 17, -1, -6, 0, -13, -17, -40, -8, -5, -14, -11, 20, 23, 15, -14, -11, -24, 12, -9, -5, -5, -4, -4, 1, -1, 1, 9, -12, -5, 2, 11, -10, -20, -2, -20, -20, -13, 1, -13, 0, 12, -30, -2, -34, 3, 0, -12, -11, -14, -26, -20, -6, -6, -24, 0, -20, -19, -1, -39, 13, -6, -29, -18, 2, -14, -23, -6, 7, 7, -9, 23, 1, -11, 8, 15, 2, 3, 12, 21, -11, -18, -17, 19, 2, -4, -9, 8, 3, -14, 10, -12}
, {-3, 18, 1, 14, 2, 0, -6, 14, 9, 13, 0, 15, 6, 9, 6, 1, 22, 0, 6, 1, 6, 13, 14, -5, -4, 9, 3, 2, 0, 20, -6, 2, -2, 5, 1, -6, -6, 2, 8, -5, -25, -26, 0, -12, 5, 4, -7, 9, 0, 3, 4, -9, -8, 2, 1, -2, 9, 17, 13, 12, -8, 0, 2, -9, -21, -9, -3, -18, 9, 1, 16, -3, -28, 9, -3, -17, -5, 4, 4, 7, 1, -20, -4, -1, -8, -2, -3, 0, 10, 3, -6, 11, -10, -21, -7, -12, -11, -8, 4, 3, 17, -3, -4, -5, -10, 8, 19, -12, -3, 14, -19, 14, -18, -23, -10, -4, 4, 2, -25, -6, 10, 15, 9, -5, -11, -14, -15, -11, -11, -8, -5, 7, -21, -5, -13, -12, -1, 13, 4, -8, 0, -7, -11, 27, -13, -6, -7, -18, -2, -6, -12, -11, 24, 10, -6, -15, -20, -10, -11, -9, 10, 0, 24, 10, 7, 6, -5, 3, 20, 6, 10, -3, -3, 29, 11, 9, -1, -1, 1, 8, 0, 9, 0, 1, -2, -2, 17, 5, -7, 3, -5, -1, 16, -3, -1, 6, -16, 2, -14, -17, -16, -9, -10, -23, 1, -1, 6, 0, -23, -23, -9, -9, 6, -7, -2, -15, 7, 9, -6, 8, 3, 6, -25, -23, 16, -33, -12, 3, -6, -9, -19, -10, -12, -2, 1, -1, -9, -5, -17, 2, -14, -16, -19, -13, -6, -16, -23, -6, 0, -7, -33, -16, -6, 1, -5, -36, -6, -17, -10, -3, -15, -9, -21, 0, -7, -17, -10, -7, -4, -14, -4, -2, -3, 4, -7, 6, 6, -26, -7, -6, -8, -6, -10, -7, -11, 0, -12, -14, -8, -9, 2, -20, -28, -8, -21, -6, 1, -1, 3, -5, 4, 2, -12, -18, 2, 3, 10, -11, 9, -2, -4, 4, -22, 1, 4, -7, -14, -10, -6, -4, -6, 5, -13, -4, 0, -9, -22, -9, -8, -8, 3, -1, 5, 0, -10, -2, -12, -4, -14, 0, 7, -2, -12, -5, -5, 8, 10, -21, -3, 10, -7, 2, -15, 5, 0, -10, -24, -3, -8, -5, -17, -26, -16, -1, -10, -16, -5, -19, -10, -6, 0, -8, 9, -2, -2, 0, -23, 0, -7, 2, -26, 9, 6, -12, -24, -11, -2, -10, -23, -11, -4, -1, -2, -20, -13, 5, 4, -5, -15, -29, -6, -11, -2, -8, 7, 6, -4, -6, -18, -6, -3, -13, -21, -10, -4, 0, -4, 4, 9, -1, -3, -7, -10, -10, -1, -7, -3, 5, -2, -10, -6, -18, 6, 2, -2, 6, -10, -2, -7, -10, -10, -1, 7, 2, -24, -9, -8, -3, -4, -3, -1, -10, -5, -2, -1, -2, -2, -1, 3, 8, -2, 0, 0, -9, -5, 2, -6, 8, -2, -2, -10, -6, -7, 5, -6, 3, -15, -17, 7, 0, 4, 8, 6, -2, 1, 1, -6, 6, -1, -3, -6, 7, -1, 10, 3, 4, -5, 7, 6, -2, 7, -9, -4, 8, 8, 8, 9, -1, -8, 4, -1, -7, 4, -11, 4, -9, 8, -6, -2, 10, -13, -17, -18, 13, 8, 9, 0, -12, 3, 9, 6, -5, 4, 4, -12, 14, -10, 8, -7, -6, -12, -1, -2, 8, -11, -13, 5, -13, -17, -5, -17, -23, -11, -7, -1, -19, 8, -6, -10, -5, -9, -7, -24, -21, -5, -7, -11, 4, -4, 7, 0, -18, -30, -4, -1, -14, -6, -3, 5, -12, -15, -12, -20, -25, 3, -1, -7, -9, -6, -11, 5, -7, 1, -12, -11, -6, 5, -13, -9, -10, 11, 10, -10, 9, -12, -10, -18, -12, 1, 3, 12, -2, 8, -9, -10, 0, 9, -30, -7, -4, 7, -10, -1, 2, 5, 3, -2, -13, 6, -21, 9, -9, -7, 9, -23, -9, -18, 9, -9, 8, 8, 3, 16, 3, 17, -4, -6, -13, 17, 0, -16, 12, -3, -6, 3, -2, 21, 25, -1, 1, 8, -12, 2, -2, 5, -2, 3, -1, -5, 10, 18, 3, 6, 4, 0, -6, 14, -10, 3, -8, 4, -3, -16, 11, -6, -1, -4, -8, 4, -4, 5, 7, -5, -13, -5, -6, -15, 9, -4, 4, 5, 3, -12, 11, 4, 0, 5, 2, -7, -16, -12, -16, -15, -12, -16, -17, 2, -12, -10, -12, -6, 13, 11, 3, 4, -14, -6, 0, -9, 6, 4, -24, 0, 11, 6, 7, 7, -4, -15, -5, -10, -10, -3, -15, -3, -15, 2, -18, 5, -2, -1, -3, -4, 15, 4, 10, -2, 4, -1, -3, -15, 7, 12, -18, -2, -12, -10, -16, 4, -11, 1, 0, -10, -4, 5, 7, 12, 8, 19, -7, 2, 8, 11, -1, -7, 4, -5, 6, -9, -11, -8, 16, -9, 15, 7, -2, 2, -4, -1, -1}
, {-19, -10, -24, -6, -13, -26, -17, -15, -5, -31, -15, -63, 7, -21, 6, -25, -23, -3, -7, -23, 4, -6, -5, 5, -17, 10, -27, -43, 7, -14, -4, -23, -41, 16, 0, 15, -12, 7, -11, -18, -13, -6, -24, -22, 7, -10, -11, -37, -3, -5, -7, -25, -2, -4, -6, -29, -47, -7, -10, -3, -7, -5, 3, -11, -28, 14, 17, -15, -3, -16, -8, 13, 2, 43, 0, 19, 1, -3, -8, -16, -16, 9, 12, -11, 4, -13, -11, 8, -36, 7, 19, -14, -18, -21, 12, -12, -6, -13, 1, 9, -11, 0, 3, 34, 25, 7, 20, -11, -5, -19, 2, -18, 13, -8, 14, 7, -9, 0, -4, -19, 7, 9, -1, -11, 2, -14, -11, 23, -4, -30, -19, -6, -29, -28, -13, 21, 28, 20, 16, -15, 3, -21, 22, -58, -19, -18, -9, 32, 8, -9, -11, -13, -31, -8, -16, -50, 0, -12, -6, 18, -6, -23, -11, -26, -20, -14, 11, -27, -7, -22, -20, -27, 2, 1, -21, -7, -9, -3, 2, -24, 5, -18, -22, 15, 25, -3, -26, 3, 5, 2, 21, -15, 2, -15, -4, -20, 7, -1, 22, -4, -2, 12, -7, -7, 4, -20, 16, 2, 3, 0, -5, -19, -11, 0, 9, 9, 3, -3, 5, 14, 1, 16, -4, 21, 5, -6, 4, 9, -8, 2, -2, 3, 31, 25, 10, -7, 10, 3, 4, -15, 0, 6, 4, -9, -5, 7, 14, 15, -15, -9, 16, 17, -4, 5, -1, 9, -13, 2, 15, 17, -31, 7, -7, 3, -1, -12, 13, 45, -8, 6, -2, -1, 13, 17, 0, -5, 1, -9, 14, 25, 20, 6, -1, -14, -26, -9, 28, 17, 0, -25, 4, -5, -24, -9, -27, 36, 11, 40, 13, 30, 11, 16, -16, 3, 23, 36, 8, 17, -4, -26, -2, 3, -8, 6, -10, -15, -16, 4, 28, 17, -8, 11, -10, -2, -14, 2, 19, -22, 18, -5, -7, -22, 3, -20, 22, -17, 0, -11, -4, 19, -4, 32, 16, 7, -7, 7, -7, 7, -11, 8, -11, -2, -4, 1, -14, 0, -11, -21, -8, 11, -2, 12, 3, -1, 8, 2, 29, 8, 40, -3, -5, 4, 5, 2, 4, 3, 4, 4, 1, 4, -5, 2, 6, 24, 4, -13, -32, -13, 3, -22, 10, -2, -3, 3, -5, 7, 11, -6, -3, 3, 13, -7, -34, 8, 6, 3, 11, -10, 6, 12, 13, 10, 16, 10, -6, 2, 3, 10, -12, -2, 1, 23, 42, -7, -3, -4, -9, 1, -11, 6, -6, -4, -2, -7, 15, -12, -7, -12, 4, 11, 1, -5, 20, 1, -20, -8, -6, -27, -33, 33, 23, 15, 13, 16, 18, -7, 1, -16, -19, 0, 12, 21, -18, -15, 10, 30, 27, 12, -2, 0, 6, 16, -11, 5, 11, 21, -8, -22, 12, -5, 19, 16, 14, -8, 14, 7, 42, -16, -12, 5, 1, 7, -3, -17, -4, 16, 1, 8, 3, 19, -1, 12, 8, -6, -1, 5, -1, 27, -4, -5, 23, -11, 13, 4, 10, -15, 27, 8, 12, -12, -8, 1, 21, 4, 1, 5, 2, 31, -5, 1, 0, 28, -3, 17, 4, 19, -12, 2, 9, 27, -18, 9, 9, -19, -14, -7, 19, -2, 32, 19, -14, -4, -27, -6, 11, -12, 10, -3, 6, 11, -10, -1, -6, -9, -1, -3, 7, 12, 9, -3, -25, 37, -6, 2, 4, 2, -22, -7, 12, -1, 27, 5, 20, -10, 6, -11, -5, -2, -2, 23, -19, -1, -7, -20, -4, -6, 6, -2, 13, -20, 16, -6, -15, 6, -23, -1, -20, -1, 15, 5, 18, 3, 6, 14, 17, 3, -7, -12, 38, 7, 1, 1, 15, 27, 16, -1, 8, 14, 7, -3, 11, 17, 32, 8, 5, 6, 0, 4, -6, 12, 2, -16, -7, 14, 3, 11, -4, -31, -4, -12, 4, -5, -6, -2, -21, 2, -38, -6, -9, -6, -3, -12, -12, 15, 4, 6, -11, 4, -8, -5, -18, -13, -6, -4, 4, -6, 1, 8, -5, -16, -2, 0, 13, -13, 5, -24, 5, -6, -3, -8, -10, -6, 3, 1, 7, -21, -6, -6, -3, 8, 17, 0, -19, -2, 13, -11, 0, -11, -16, -4, -12, -34, -7, -25, 9, -20, 8, -9, -9, 10, 10, 4, 4, -5, 1, -4, 7, -15, -8, -7, 5, -13, 18, 3, -7, -4, 0, 4, 10, 2, -12, -25, -7, -14, -3, 2, 16, 16, 6, 27, 15, 7, -4, -12, -14, 0, 4, -4, 2, 0, 9, 11, 11, 6, -3, 7, 4, -12, -3, -12, 6, -5, 19, -9, -27, 14, 25, 22, 3, 46, 12, -4, 13, 9, 8, 15, 14, 11, -6, -11, 21, 6, 5, -4, 3, -8, -2, 20, 16, 6}
, {-11, -25, -20, -5, -19, -4, -21, -27, -2, -2, -20, -6, -5, -10, -20, -20, 13, -16, -12, 1, -3, 3, -5, -16, -6, 8, -16, -8, 7, -5, -5, -9, 3, 2, 0, -35, -7, -4, 3, 1, -16, -4, 5, 2, 1, -12, -7, -3, -5, 1, 13, -18, 4, -6, 13, -4, -4, 5, 6, -20, -23, 5, 3, -3, 4, -9, -4, 15, -12, 21, -2, -11, -20, -25, -4, -5, -4, 9, -4, -15, -3, -12, 4, -22, 7, 5, -3, -8, 8, -3, 8, -15, -11, -2, -16, 14, -9, -7, 14, -5, -9, 0, -7, -18, 5, -1, -3, 1, -8, -25, 6, -14, 3, -14, 11, -9, -5, 14, 14, -7, -14, -3, -12, -7, 14, -7, 0, -12, -2, 8, 10, -26, -10, -12, -2, -8, 3, 7, -2, -7, 8, 0, -8, 5, 0, -18, -1, -15, -9, -12, 10, -16, -2, 1, -7, -14, 8, 8, -17, 2, -2, 2, -3, -22, -2, -7, 1, -16, -11, -15, -13, -7, 5, -12, -6, -10, 16, -12, -9, 4, -8, -9, 8, 17, -18, -8, 10, 0, 20, 3, 4, -7, -2, -4, 6, -5, 11, 20, 6, -22, -8, -10, -7, -1, -8, 10, -26, 3, 3, -10, 5, -1, -2, 10, 5, 7, 12, -2, -8, 0, 4, 9, -4, -6, 3, -1, -13, -7, -10, 1, -24, -16, -26, -15, -7, -22, 3, -15, -22, 0, -22, -16, -29, -21, -2, -12, -13, -27, 4, 6, -8, 1, -7, -4, -1, -22, 6, -13, -9, -7, -4, 4, 3, -11, 0, -2, 0, -19, -9, 8, -11, -2, -1, -1, -1, 9, -4, 10, -16, -22, 11, -6, -21, -14, -40, 8, -2, 2, 6, -12, 1, 7, 7, -8, 7, -8, -3, -1, 14, -6, -9, -7, -9, 12, -22, 5, 15, -21, -10, -12, -17, -18, 9, -6, -9, 14, -24, 5, 4, 0, -6, 11, -9, 3, -2, -22, 0, -12, -3, -2, -8, 6, -1, 2, 12, -3, -3, -2, 14, -6, 1, 18, -11, -16, 1, -2, 15, -2, 3, 13, 5, -8, 1, 0, -9, -7, -22, -1, -7, -7, -14, -19, -14, -16, -8, -9, -1, -9, -16, -20, -19, 3, -3, 3, -12, -8, -5, 12, -12, -3, -6, 12, -5, 0, -6, -3, 11, -21, -21, -2, 0, -23, -18, -12, -8, -16, 4, -11, 4, -7, 3, -1, -8, -7, 3, 11, 0, -1, -3, -3, -8, 1, 3, 11, -9, -8, -2, -5, 4, -14, -10, -12, 21, -7, -10, -18, -3, 3, 0, 9, -4, -5, -14, -7, 7, 13, 5, 8, 0, -16, -3, 4, -10, 2, 12, -16, 8, -18, -26, -12, 0, -7, -9, -12, 2, -1, -3, -11, -9, -13, 7, 4, 12, -14, -15, -13, -12, -4, 6, 2, 13, -10, 11, 0, 5, -4, -7, -23, -3, -12, 5, 10, -11, -9, 8, 10, -5, 3, 9, -7, -20, 10, -6, 13, 21, 0, 7, 2, 0, 22, 3, 5, 5, -8, -1, 4, -7, 1, -11, -14, 13, 12, -16, -21, 1, -10, -9, 6, -15, -19, -1, 0, -3, -1, 9, -8, 11, 5, 6, -8, -12, -13, 8, -10, -6, -4, -5, 11, -19, 2, 3, 8, 5, -16, 3, 0, 21, 2, 16, 0, 22, -3, -2, -8, 3, 5, 8, -1, 1, -2, 20, 14, 3, -29, 6, -15, 10, -21, -6, 6, -26, -8, 18, 15, -9, 3, 7, -6, 5, -15, -16, -11, -23, -7, 14, -9, -1, -20, -4, 7, 18, -8, 0, -23, 11, -11, -4, 8, 6, -6, 4, 4, -11, 10, 5, 0, -16, -3, -3, -25, -14, -6, -8, -13, -29, -5, 12, 6, -20, -5, -6, -3, -1, -8, -8, -7, -1, -6, 6, -10, -7, -23, -20, 1, -20, -16, 3, 8, 0, -23, -19, 2, -3, -24, 4, -12, -17, 14, 19, -17, -2, 9, 7, -8, -4, 12, 12, 5, 7, 16, -7, -7, -8, 8, -1, -6, -6, -8, 1, -7, 9, 10, 1, -7, -5, 4, 5, 10, 14, -2, -7, -1, 14, -5, 8, -4, -1, 9, -16, -2, 15, 6, 2, 1, 1, -3, -16, -3, -29, -12, -1, -9, -4, -3, -25, -8, 10, -11, -30, 14, -3, 14, -12, -3, -8, -3, -8, 15, -14, -23, -16, 6, -12, -13, 9, -8, -9, -1, 5, -2, -5, 9, -17, -12, 7, 5, -12, -19, -1, 11, -22, 0, 1, -19, 12, 5, 9, -4, -8, -7, 4, 1, -16, 2, 5, -1, 6, 0, 6, -29, 10, 11, 2, -9, 11, 1, -12, -34, -10, -17, 9, 4, -11, -10, -28, -2, 7, -30, 1, 12, -10, 12, 5, 0, 18, 0, 0, -9, 2, -6, -3, 6, -21, 7, 5, -8, 11, -18, 3, 17}
, {13, -11, 23, -6, 28, 2, 4, 9, 15, 16, 22, 15, -11, 4, -22, 31, 7, -23, 21, 10, 0, 7, -21, -1, 35, 6, 27, 24, -18, -6, -2, 18, 8, -22, 1, 15, -17, -11, -16, 8, 36, 19, 2, 16, 5, -14, -2, 22, -4, 3, -4, -1, 8, 3, -30, -34, 20, 2, 14, -21, -5, -4, -5, -1, -7, -19, 10, 16, 10, -1, -9, 11, 10, 16, 5, 18, 15, -12, -31, -7, -13, -20, -9, -22, -3, -6, -8, -12, -19, 0, 1, -20, -19, -1, -9, -1, -28, -13, 11, -6, 8, 5, 9, 2, 26, 35, 2, 20, 10, -4, -2, 1, 19, 11, 17, -10, -3, 11, 11, 11, -49, 3, 17, -2, -14, -19, 17, 1, 9, -8, 7, 26, -10, -12, 1, -7, -5, 17, -4, 1, -4, 9, 17, -7, 13, 32, 6, 15, -10, 6, -1, -2, 11, -7, -9, 21, 1, -8, 15, -12, 8, -20, -20, 5, -12, -6, -1, 0, -8, -8, -1, -18, 6, -22, -12, -8, 22, -12, 1, -6, -9, -14, -24, -47, -13, 11, -12, -17, 12, -7, -34, 27, 3, 1, -15, -11, -2, 1, -8, -23, -13, 8, -13, -6, 9, -8, 19, -12, -3, -8, 0, 15, -4, -7, 3, -13, -18, 8, -8, -6, -4, -6, 5, -20, -4, 4, -5, 6, 12, -4, 8, -6, 1, 16, 14, 3, -1, 19, 10, 20, -4, -6, 0, -16, -3, -21, 17, 13, 30, -4, -11, -3, 4, -11, 7, 6, 4, -4, 0, -7, 7, -19, -5, -4, 19, 10, 3, -15, 4, -13, -11, 22, -3, -17, -6, -6, -9, 3, -11, -9, 16, 3, -10, 15, 7, -7, -16, 3, -2, 12, 1, -8, -3, 6, 14, -8, -14, -6, -2, -14, -2, 2, 4, -1, -13, -23, -12, 26, 7, -10, 11, 17, 7, 7, 3, -5, -5, -9, -18, -24, -21, 18, 14, 5, 2, -8, -9, -18, -20, -4, -13, -6, 12, -7, 1, 4, -11, -4, 18, -9, -5, -1, 3, -22, -12, 4, -6, 2, 1, -15, -4, -17, -19, 33, 17, -1, 4, 17, 3, 3, -12, -10, -6, -21, 6, 1, -12, 15, -9, 12, 21, -9, -8, 6, 2, 2, -4, 11, -10, 17, -10, -5, 6, -6, 4, 0, 23, 16, -7, 10, -13, 1, -7, 6, 1, -19, 17, 8, -22, -9, 0, 16, 24, -16, -9, -19, -11, -7, 0, -4, -10, -3, -4, 9, 11, -11, 9, 4, 42, -18, -17, 20, 2, 5, -8, 8, 4, 43, -2, 15, -9, -7, -7, 34, 30, 12, 5, -3, 15, -13, 10, -8, 12, -17, 14, 16, 20, 6, 4, 10, 8, 9, -4, 1, 10, -9, -28, 1, -14, -59, 4, -11, -22, -26, 17, 10, -4, -5, 10, -14, 12, -24, -11, -4, 0, -2, -4, 7, -5, 4, -3, -17, -29, -13, 8, -8, -21, -18, -21, -14, -10, -24, 5, -8, -33, 14, -6, -13, -15, 2, -8, -7, 1, -38, 5, -1, -14, 5, -5, 2, -7, -34, 5, -3, -3, -13, -21, -5, -18, -12, -22, -12, -33, -7, 0, -9, 4, -18, -16, -27, -18, 10, -4, -8, -7, -6, 8, 11, -7, -25, 3, 13, -9, -22, -4, -22, 9, -2, -22, 10, 10, -6, 15, 19, -6, 9, 8, 17, 5, 8, 5, 35, 32, -39, 7, -7, 25, 37, -6, 2, -16, 6, 13, -10, 26, 7, 2, 5, 8, 19, -15, 9, -3, 5, -4, -12, 3, -33, 11, 14, 2, 7, 21, -5, -3, -7, -2, 19, 7, 29, 43, 9, 1, 30, 4, 0, 4, 6, 13, -18, 2, -1, -34, -9, 6, 3, -11, -26, -21, -22, 4, -3, 4, -11, 2, 2, -16, -8, 2, -9, 6, -12, -2, -1, -30, -16, -6, 30, -4, 10, -4, -3, 4, -12, 6, -11, 18, -5, -37, -19, -17, -30, 5, 7, -41, 3, -35, -28, -14, -19, 10, 23, 3, -22, -5, -4, -14, 9, 3, -23, -19, -24, -21, 33, -5, -8, 12, 20, 31, -50, -31, 5, 7, -27, 13, -23, -10, 4, -36, 5, 19, -14, -7, 29, -10, -39, 5, -10, 14, 20, 27, -36, -15, -49, 2, 2, 6, -14, 9, 3, -20, -15, 4, 12, 17, -9, 0, 7, -21, 13, -24, -10, 1, -10, 1, 10, -8, -11, 18, -1, 10, -4, 45, -7, -13, -9, 11, 16, -12, -4, -3, 8, -6, 20, -9, 20, 12, 2, 15, 2, 3, 5, 24, -25, -18, -11, -8, 17, -6, -18, -3, 10, 3, 5, -8, 21, 2, 23, 2, 13, 10, 6, 27, 5, 20, 8, -15, 3, -6, 17, 7, -5, -13, 18, -17, -9, 3, 5, 10, 11, 2, 14, 26, 9, -23, 25, -28, 1, 19, -9}
, {-15, -7, -17, -14, -7, 10, -5, 1, -8, 1, 4, -5, -7, -10, -3, -5, -12, -11, -15, -2, -4, 11, 0, -7, -8, -14, 6, -15, -10, 0, -3, -17, -33, 6, -1, -13, -5, 1, -3, -12, -28, -30, 1, -21, 2, -4, 3, -4, -8, -7, -8, 3, 4, -1, -2, -9, -11, -8, 15, -5, -5, 2, 2, -2, -24, 4, -2, 1, -8, -4, -7, -27, -22, -17, -11, -7, 6, -9, 1, -19, 23, -20, -8, -12, 1, 3, -25, -12, -13, 0, 3, -2, 19, 5, -13, -13, -17, -3, -28, -20, -25, -26, -35, -9, -18, 17, 3, -8, 0, -1, -18, 11, -22, 0, -37, -20, 1, -24, -11, -7, 21, -11, -6, -30, -21, -21, -17, -21, 0, -6, 9, -32, 0, -6, -6, -3, 14, 1, 0, -3, -6, -5, -2, -14, -2, 10, -1, -18, 7, -22, -3, 2, -32, -1, 4, -23, -11, -14, 8, 9, -11, -5, -11, -9, -4, -3, 2, 4, 2, -13, -4, 18, 7, -6, 6, -6, -12, -7, -10, -10, 4, -2, 0, 13, -10, -4, 8, -1, -18, -8, 1, -1, -15, -13, 8, -3, -19, 6, 6, -10, 1, -5, 8, -8, -7, -16, 28, -14, -11, -21, 2, 2, -9, -25, 5, 2, -14, -14, -11, -15, 11, -9, -3, -15, -7, -36, 14, -6, -27, 2, -19, -14, -29, -18, -3, -21, 3, 2, 8, -7, -1, -32, -14, -6, 7, -12, 5, -6, -14, -10, -22, -16, 7, 13, -13, 5, -1, -2, 11, -23, -14, -1, -10, -26, -19, -21, -2, -17, 3, 0, 0, -18, -11, 0, -4, -9, -1, -3, 12, 3, -22, -11, -7, -7, -12, -21, -15, -6, -3, -9, 12, -5, 23, 8, 3, -13, 5, 4, 8, -20, -5, 21, -1, 5, -11, 10, 15, -3, 0, 5, -1, 1, -3, -7, 6, 12, 6, -15, -15, -8, 5, 2, -7, -8, -14, -14, -18, -4, -3, 8, -6, 8, 1, 6, -5, 4, 3, -11, 4, -1, 0, 0, 7, 2, 1, 0, -1, -8, 15, -12, -9, -10, -9, -11, -8, -1, -19, -2, 8, 3, -4, 8, 17, -6, 0, 7, -8, 22, -8, -15, -1, 3, 5, -21, -17, -14, 8, -2, -4, 22, -15, -12, -10, -2, -38, -10, -17, 7, -12, -22, -7, -18, -7, -7, 3, -10, -2, -23, -6, 4, 2, -7, -8, -7, 4, -7, -11, -5, 9, -4, -7, -8, -15, -12, -16, -13, 3, -14, -1, -12, -3, 8, -9, -21, -10, -9, 4, -23, 2, 9, -4, 6, -12, -1, -16, -5, -10, -21, 0, -13, -1, -12, -11, 5, 5, -1, -5, 1, -9, -6, -16, -12, 4, -11, -22, -1, -11, 24, 2, -7, 5, 0, -27, 3, -3, -10, -5, -2, 8, 2, -11, -5, -1, -3, -4, -6, -10, 1, -5, -8, -2, -2, 0, -7, -14, -17, -25, 1, 0, 2, -15, -7, -10, -14, -3, 1, -8, 7, 6, 1, 6, 4, 4, 8, -12, -4, -14, -7, -16, -5, 5, -8, -2, -18, -1, -12, -10, -1, -25, 0, -16, -18, -3, -6, -5, -6, -21, -1, -18, -15, -2, 0, -10, -1, -10, -7, -32, -17, -12, -8, -24, -21, 6, 4, -6, -9, -13, 0, -14, 5, -26, -5, -7, 18, 6, -20, -10, -9, -14, -9, -17, -4, -6, 18, -4, -7, -3, 1, -8, -2, -12, -8, -16, 6, -8, -25, -9, 2, -15, -11, -2, -19, -20, 14, 12, -23, -20, -19, -5, -16, 12, -13, 14, 3, -24, -2, 0, -8, 1, 1, -1, -12, -15, -14, -11, -3, -1, 32, -11, -19, -9, -15, -22, -6, -10, -11, -9, 3, -14, -11, 5, -5, -8, -5, -9, -16, -8, 2, -11, -10, -25, -9, -15, -1, -5, -27, -32, -12, -6, -14, -11, -6, -3, -5, -13, -15, -9, -8, -7, -4, -15, -8, 3, -11, -11, -8, -13, 3, -14, -5, 2, -14, 3, -9, -11, -8, -16, -14, -5, -6, -11, 8, 4, -21, -7, 6, -9, -6, -16, -16, -11, -27, -18, -17, 8, 4, -8, -14, -15, -5, -6, 2, -11, -26, -8, -5, -13, -3, 0, -3, -13, -9, -24, 2, -17, -25, -17, 4, -41, -19, 11, -5, 9, 5, 6, -1, -3, -23, -21, -24, -24, 0, -18, -12, 3, -19, -3, -19, -18, -4, -12, -31, -12, -16, -6, -10, -4, -19, 63, 0, 18, 14, 13, 18, 11, -9, 24, 12, 8, 24, 43, -15, 26, -6, 67, -7, 6, -8, 47, -5, -21, -3, -2, 27, 61, 36, -17, 51, -17, -16, -10, 16, -8, -5, -22, -13, -12, -14, -22, -8, -3, 19, -15, -35, 21, -6, -5, 0, 7, -15, 32, -13, -39, -13, 9, -34, -1, 22, 13, -2}
, {-10, -33, -30, 11, -47, -47, -38, -4, -18, -19, -31, -45, -5, -39, 22, -70, -20, -12, -25, -9, 9, 1, 13, -6, -13, 5, -33, -35, 27, 14, 10, -10, -3, -7, -22, 10, -52, -10, 0, -13, -23, -19, -19, -26, 4, -18, 27, -72, -31, 5, -11, -22, -1, -27, 24, -37, -53, 8, -46, -14, 12, -6, -9, -19, 16, 27, 0, -14, -7, 11, -10, -5, 5, 11, -8, 1, -4, 26, 4, -8, -17, 3, 8, -9, 7, -18, -7, 18, -10, -8, 4, -11, -8, -6, 0, -10, 26, -9, 9, 2, -9, -4, -17, 22, 22, 20, 7, 16, 6, -2, -13, 18, 18, -6, 9, -26, 8, -18, -14, -1, 21, -8, -15, -1, -7, 2, -4, 16, 3, -30, -8, -10, 10, -35, -29, 35, 31, 1, 2, -5, 7, -20, -11, -13, -1, -34, -1, 8, -10, -2, -21, -21, 3, 2, -9, -43, -8, -15, -6, 34, 5, 2, 17, -15, -4, -1, 16, -13, -12, -7, -3, -2, 1, 10, -6, 1, -6, 12, 11, -15, -9, 1, 2, 39, 19, 8, 5, 15, -5, -1, 15, -10, -7, -15, 13, 7, -7, 8, 15, 9, 5, 6, 2, -2, 4, -4, 0, -3, -6, -11, 12, -19, 4, 1, -10, 4, -3, 7, 4, 2, 5, 6, 3, 22, 1, -3, 16, 10, -10, 12, 12, 7, 19, 18, -14, -10, 7, 13, 4, -13, -12, 10, 13, -7, 5, 6, 14, 16, -23, -9, 1, -3, -8, -5, -2, 34, -15, 4, 13, 7, -18, 8, -12, 10, 8, -2, -7, 31, 2, 13, -7, 4, -5, 8, 4, 6, -9, -1, 7, 0, 19, 2, 10, 5, -6, -21, 28, -1, -1, -15, 4, -3, -19, 9, -25, 15, 19, 16, 18, 7, -1, -4, -8, -2, -8, 26, 8, 5, 2, -6, -33, 16, 2, -2, 6, -5, -17, -16, 23, -2, -12, 2, -2, -1, -7, 16, 14, 5, 10, -3, -6, 5, -2, 16, 28, -2, 23, -6, 7, 28, -3, 16, 4, 2, 12, -4, -6, 2, -3, -2, 3, 3, 8, -9, -9, 6, -14, -28, -7, 5, 12, 19, -9, 19, 5, 1, 46, 6, 0, 14, 16, -1, 5, 4, 8, 20, 5, 5, -11, -2, 9, 9, 22, -32, 20, -13, -8, -6, -1, -41, -15, -22, -14, 1, -3, 14, -2, -28, 8, 6, 7, -5, -20, 22, 1, 0, 3, -1, 8, 1, 12, -13, 21, 11, -17, -6, 6, 36, -1, 10, -1, -9, 15, 8, -7, -10, -21, -12, -4, 0, -3, 3, -4, 15, -17, 3, -4, -10, -6, 21, 13, 8, 0, -5, -9, 21, -14, -6, -11, 4, -9, 9, 5, 6, 8, -5, -15, -12, -13, -27, 3, 4, 5, 7, 16, -11, -20, 5, -2, -4, 7, 0, -2, -8, -5, 6, 15, 8, 4, -14, -7, 8, 11, 2, 7, 35, 21, -2, -13, -9, 15, 9, -2, 18, -1, -5, -12, 12, 18, 11, 5, 4, 3, 25, 10, 5, 25, 14, 10, -10, 21, -2, -21, 21, 14, 14, 3, 14, 27, 7, 5, -8, 23, 8, 1, 21, -5, 16, -1, 22, 21, 5, 1, 7, 19, 20, 18, 8, 25, 13, -10, -24, 23, 1, -13, 21, 11, 5, 4, -15, -9, -13, 6, -12, 7, 2, 8, -1, 20, -12, 9, -12, -19, 27, -2, 16, 6, -1, 30, 3, 0, 11, -1, 10, 3, 25, 4, -3, -16, -10, 18, 9, 17, 4, 5, -9, -6, -13, -11, 11, 14, 2, 7, -31, -10, 13, 2, -14, -2, -8, -11, 3, 0, 14, 1, 16, -2, -5, -8, 0, 0, 4, 17, -7, 28, -13, 4, -2, 31, 21, -2, 12, 4, 1, -9, -1, -8, 40, 0, 31, 13, -9, 12, -6, -2, 10, 4, 2, 6, 5, -13, -25, -29, -5, 8, -3, 5, -2, 23, -6, 0, 6, 0, -4, 2, -2, 1, -12, -7, -22, 7, -26, -10, 25, 5, -7, -19, -1, -4, -1, -9, 2, 4, -27, 10, -7, 12, -7, -15, 17, 10, 4, 4, -5, -9, 15, 4, 10, 5, -3, -3, -7, 0, -14, 3, 9, -15, -11, -4, -5, -10, 2, 1, 12, -13, 2, -6, 3, 11, -2, -14, 0, -3, -10, -8, 8, -11, -6, -5, 6, 15, 17, 1, 23, 6, -3, 0, 13, -4, -1, -1, 5, -12, -14, 15, 14, -12, -9, 16, 18, -15, 0, -10, 1, 7, -4, -1, -7, -8, 10, 12, -6, 7, -9, 11, 20, 2, -17, 4, 14, 19, 6, 14, 10, 17, 13, 3, 7, -7, -9, 20, 11, 2, 8, 2, 3, 9, 33, 30, 18, 3, 12, -6, 13, 20, 35, 36, 2, -2, -21, 35, 6, 0, 6, 10, 6, 15, 36, 12, 20}
, {-4, 1, 4, 2, -23, 11, -22, -13, -22, 4, -13, -28, -7, -40, -21, -20, -9, -3, -15, -18, 1, -12, -12, 17, 4, -17, -14, -40, 15, 10, 0, -51, 14, -20, -10, 3, -7, 4, 1, 5, -15, -40, -16, 25, -2, -10, -13, 4, 17, 13, 21, -10, 0, -23, 19, 13, 0, -5, -24, -15, -11, 3, 0, 7, 6, -27, 15, 7, -24, -1, -15, 17, 9, -14, -5, 15, 2, 36, 16, 27, 17, 10, 2, 3, -7, -3, 12, 24, 21, -6, -15, 7, -10, 16, 4, -7, -14, -13, -6, -6, -29, -17, 0, -13, -30, -36, -14, -41, 3, 9, 9, 8, 22, -7, -28, -7, -2, -40, 12, 23, 24, 5, -40, -8, 6, 17, -3, 9, 16, -29, -23, 3, -11, -14, -23, -25, -4, -5, -3, -17, 6, -14, 2, -5, -9, -13, -22, -21, -7, -15, -7, -18, -6, 0, -30, -19, -3, 7, -21, -5, 15, -6, -8, 11, 2, 7, 19, -13, 0, -2, 10, -11, 3, 18, -9, 6, -9, -1, -13, -20, 4, 9, 14, -6, 7, -15, 0, 8, 11, 4, 11, -25, 10, 5, 19, 12, -6, 11, 1, 20, 2, 6, 6, 10, 2, 13, -12, 1, 1, -12, 17, -15, 4, -11, 3, 2, -3, -16, 3, 13, -1, -28, 22, 21, -10, -4, -9, -16, 8, 6, -9, -4, 16, 6, 18, -15, -10, -13, 16, -3, 5, 3, 0, 6, -6, 16, 8, -21, -2, -10, 4, 1, -25, -13, -23, -18, -6, 8, -1, 9, -5, 18, -5, 22, 1, 8, -16, 6, -5, 5, 2, -10, -1, 7, 13, 11, -9, -3, 21, 21, 3, -14, -6, -6, -5, -33, 12, 16, -31, -6, 15, -10, -3, 2, 8, 9, 6, 15, 0, 26, -2, 19, -12, -19, -15, 46, 24, -1, -4, 8, 8, 3, -35, -2, 4, -3, 9, -16, 21, -2, -12, -24, 1, -13, 1, 0, 23, -12, 0, 1, -14, 2, 2, -1, 18, -3, -6, -3, 5, 7, 7, -4, -2, -29, -5, -5, -5, 22, 9, -18, -8, -3, 7, -18, -8, 10, 6, -20, -33, 6, -8, -5, -22, 21, -10, -24, -9, -15, 5, -15, -16, 23, -5, -34, -4, 14, 0, -2, -4, 17, 10, 3, 0, -11, -12, -9, -7, -7, 4, -9, 12, 4, 0, 10, -15, 10, 8, -8, 14, -9, 4, 0, 4, -4, -10, -18, -3, 4, -19, -4, -1, -5, -1, -14, -11, -2, -25, 20, -12, 17, 11, 10, 10, -5, 9, -20, -14, -27, -1, 12, 1, 4, 15, 2, -19, -20, -5, -11, -9, 16, 20, -12, 2, 26, -23, -22, -14, 12, -6, -2, -9, -5, 12, 5, 6, -13, -7, 7, 13, -4, 2, 26, -2, 26, -15, -3, -8, -17, 8, -22, 8, 0, 19, -15, -20, 41, 13, 11, -14, -5, 20, -19, 21, -4, -3, -2, 4, 10, 9, 10, -1, 2, 0, 9, 20, 11, -15, 23, 12, 2, -4, -16, 0, 21, 30, -11, -21, -28, 1, -3, -1, 24, -7, 3, 19, 17, -36, -7, -23, -5, -4, -14, -4, 23, -1, 5, -9, -18, 4, 8, -1, -6, 8, -14, 11, -5, 4, -17, -8, -15, -6, 4, -5, 19, -5, 2, 6, -8, 12, -12, -18, 13, 5, 0, 8, 11, -3, -1, -20, 3, 26, 8, 3, 30, -2, 2, -14, 4, -8, 3, 5, -22, -21, 8, -11, 5, -1, -18, -7, -1, -7, -15, -7, -13, -3, 5, 4, 9, -11, -5, 1, 6, -5, -15, -6, 23, -10, -4, 3, -6, -15, -14, -12, -7, -6, 6, -3, -4, 4, -17, -10, -11, 14, -5, -10, 4, 22, 25, -30, -2, 5, -8, 22, -21, 2, -12, 11, -3, 2, -10, -6, -5, -8, -4, -2, -8, -9, 7, -3, 19, -10, 25, 1, 7, -7, -18, -5, -8, 23, 10, -7, -9, 4, -20, 15, -25, -10, 29, 30, 0, 3, 23, 0, -21, -3, -13, 8, -13, 4, -9, -9, 4, 4, 3, -8, 15, -14, -9, 27, 18, 14, 20, 33, 19, 6, 25, 9, 2, 1, 12, 23, -7, 5, 30, 17, 7, -7, -6, 19, -24, 5, 6, 8, 19, 1, 0, -9, -4, -8, -7, 23, 26, 23, 2, 8, 10, 3, 3, 36, -21, 11, -18, -7, 3, -8, 0, -1, -1, -17, -15, 2, 12, 18, 3, -12, 4, -3, -11, 23, -6, -14, 3, 11, -4, 6, 34, -1, -8, 4, -3, -7, 1, -5, -5, 14, 24, 9, -9, 5, -7, 4, 2, -10, 8, -20, -18, -19, -4, -39, -15, 25, 11, -1, -6, 0, 26, 28, 53, 47, 10, 0, 9, 15, -7, 7, 22, 51, 18, 4, 15, 7, -61, -26, -6, 24, -25, 32, 22, -12, 25}
, {0, 3, -13, -7, -7, -10, -4, -9, 8, -12, -20, 1, 5, 14, -4, 3, 0, 1, -7, 2, -1, 5, -15, -2, -8, 0, 9, -9, 0, -14, -1, -14, -6, -8, -7, 0, -12, 2, 1, -2, -7, -3, 4, 1, 5, -6, -12, -9, -3, -18, -7, -8, -9, -14, -16, -17, -5, 1, -18, -22, -16, 1, -5, 1, -20, -6, 5, -7, 0, -16, 1, -4, 7, -2, 1, -3, 0, -5, 3, -17, 0, 1, -4, 18, -7, 19, -6, -4, -14, -5, -10, -23, -5, 1, -11, 2, -13, -19, -4, -13, -9, 6, 4, -2, -9, 4, -1, -10, 8, -11, -6, 1, -2, -18, -10, 1, -11, -4, -12, -16, -14, 8, -15, -2, -21, -23, -27, -14, 3, -9, -7, 9, 4, -2, -5, -8, 6, 1, -12, 4, -5, -13, -9, 11, -16, -7, -1, -9, 7, -9, -13, -9, 11, -4, 1, 11, 6, -22, -6, -23, 3, -8, -10, -5, -2, -8, -17, -11, -7, -5, -10, -10, -4, -6, 1, -7, -13, -2, -10, 3, 4, -21, -10, -4, -1, 6, 6, -20, -12, 0, 7, -6, -9, -24, -22, -2, -21, -6, 3, -2, -1, -4, -16, 10, 3, -23, 2, -11, -11, -19, -13, -3, 0, -18, -9, -19, -6, -10, -14, -14, -13, 1, -5, -2, -5, -13, -10, 5, 0, -1, -7, 3, 4, -5, 3, -4, 1, -7, -17, -14, -26, -7, 5, -10, -4, -3, -5, -9, -13, 2, 3, -7, -19, 19, -2, 3, 4, -16, -22, -13, -24, -3, -24, -30, -26, -5, -28, 1, -6, -2, -17, -19, 9, -12, -17, -20, -1, -3, -3, -4, -14, 10, -15, -8, -12, -22, -11, 16, 3, 0, -12, -29, 5, -6, 5, 4, -8, -6, 7, 5, 1, -7, -22, 3, -19, 6, -4, -5, 1, -20, 2, 1, -3, 0, 1, -17, -20, -4, -11, 1, -4, -4, -22, -5, -9, -9, 0, 1, -3, -21, 1, -6, 4, -12, -11, -2, -7, -12, -1, 8, -4, -3, 6, 4, -1, -4, -5, -6, -23, -7, -7, -3, -28, -3, 7, -16, -10, 4, 8, -9, 2, 0, -14, 13, 5, -4, -3, -14, -9, -12, 0, -3, -11, -6, 3, -7, -22, 7, -1, -12, -1, -7, 0, 3, -10, -9, -18, 10, 5, -13, -7, -10, -8, 0, 4, -19, 4, -3, -16, -14, -5, -8, -14, -3, -3, -17, 1, -21, -3, -11, -13, 12, -7, -22, -9, 10, 0, -11, -23, -4, -3, -18, 4, -8, -7, -15, -25, -9, -8, -25, -1, -16, -14, -8, -9, -7, 7, -12, 4, 0, 2, 15, -21, -2, -20, -13, -6, -5, 5, -7, -15, -7, 2, 4, 7, -1, -9, -9, -13, -3, -3, 8, 5, -14, 11, 7, 1, 3, 6, -11, 0, -1, -12, -7, 7, -1, -4, 2, -7, -1, -3, 7, 2, -2, -7, 3, 3, -1, -9, -14, 6, 3, -8, -9, -3, -7, -12, 6, 1, 3, -3, 2, 5, 7, -1, 7, -6, 2, -14, -5, 3, -9, -10, -5, -6, 6, -7, -16, -13, -7, 3, -2, 13, 3, 8, -3, 3, 8, -16, -4, -13, -15, 0, -9, -14, 1, 2, -5, -12, -6, 1, -10, -14, 2, 5, -3, 21, -12, -19, -1, -22, 10, 17, 8, 12, -15, -4, -13, 27, 1, 0, -1, 12, -22, 6, -8, 20, -22, 14, -3, -14, 8, 15, 4, -13, 17, 12, -6, -5, -8, -2, -10, 13, 0, -2, -18, -6, 0, -8, 8, 18, -14, 8, 0, -10, 18, -4, -12, 1, 6, -12, -7, 8, -14, -9, 9, 7, 7, -1, -2, -18, -8, 0, 5, -7, 7, -17, 0, 6, -5, 2, -1, 6, -4, -7, -1, 5, 4, -11, 0, 7, 0, -13, 8, -18, -9, -1, -10, -1, -12, 0, 5, 1, -12, -4, 0, -3, -10, -10, -20, -16, -5, -7, 7, -12, -11, -12, 1, -14, 2, 4, 2, 7, 5, -5, 3, -5, -4, -10, -17, 4, 8, -11, -13, -6, -7, 13, -11, -6, -4, -3, 14, -5, -8, -1, -21, -4, -15, -14, -4, -19, -14, 3, -23, -22, -13, 7, -1, -7, 11, 2, 15, -14, -11, 21, 6, 1, -2, 7, 0, -3, 4, 0, 27, 13, 11, 5, 13, 4, 24, -21, 4, 6, 7, -5, -6, 12, 2, 16, 3, 17, 4, -18, -12, 10, -12, -14, 12, -18, 4, -17, -3, -14, 8, 21, -6, -7, -18, 7, -5, -3, 2, -8, -14, -4, 11, 8, 0, 10, 12, -2, 10, 11, 1, -13, 0, -7, -9, 0, 7, 20, 2, -7, 7, 5, 0, -21, -15, -15, -14, -8, 6, -5, -18, 26, 2, 6, -8, -9, 11, 16, -2, -5, 9, -16, -11, 28, 36, -5, 14}
, {-1, 9, 12, 7, 1, -10, -5, -5, -10, -7, 8, 7, -6, -12, -5, -8, 8, 8, 0, 8, -12, -2, 0, 5, 4, 0, 8, 0, -7, -1, -6, 3, -8, -11, -10, -11, -5, 8, -10, 0, 5, 8, 0, -1, 9, -5, 1, -10, -5, -9, 7, -12, 4, 7, 1, -11, 0, 6, 10, -8, -5, -7, -7, -3, 4, -8, -7, -11, 8, 9, -11, 4, -10, -4, -9, -4, -5, -1, 4, -1, 5, -2, -12, -2, -2, -12, 1, 2, -8, -6, 10, 10, -15, -6, -11, 2, 1, -5, 3, -9, 3, 7, 2, 5, -9, 5, 3, -7, 6, -7, -11, -3, -2, 1, 0, 4, -8, -12, 4, 1, -15, -3, -10, 3, -17, -12, -6, 1, -9, -5, -10, -6, -11, 6, -4, 7, -10, -4, -8, -6, -8, -8, 3, 1, -6, 8, -3, -7, -3, 4, -2, 5, -1, 0, -5, 6, 0, 1, -8, -2, -1, 6, 4, -7, -7, -10, 1, -4, 4, -10, 7, -9, -1, 4, -8, -11, -1, 4, 0, -10, 5, -5, 3, 5, 0, -5, -1, -6, 0, 8, 7, 3, -10, -3, -16, -12, 2, -5, -12, 1, -5, -5, -10, 0, 5, -13, -12, -4, 1, -3, 1, -5, -8, -4, -4, 6, 2, -1, -2, -1, 0, -8, 3, -9, -7, 3, -3, -3, -4, 7, -3, -5, -2, -3, 4, 7, -5, 8, -1, -9, 3, 4, 2, -11, -7, 3, 2, 2, 5, 6, -9, -4, 4, 5, 3, 8, -3, -6, 1, 6, -7, 0, -1, 3, -5, -11, 3, -11, 1, 5, -6, -4, 2, 0, -5, 1, -4, -3, -8, 5, -13, 9, -1, -9, 4, 7, -8, -1, 6, -5, -2, -5, -7, 5, -5, 0, -10, 0, -1, 5, 0, -12, 4, 1, 7, -7, 6, -3, -4, -2, 2, 4, 2, 2, -8, -3, -12, 3, 7, 5, 6, -11, -5, -10, 7, 7, -5, -11, 5, -12, 1, -5, 5, -10, -9, -2, 9, -12, -9, -5, -7, 6, -5, 6, -4, 1, 1, -9, -10, -5, 3, 0, -5, -10, 4, 5, -12, -11, -11, 0, 1, -3, -10, 5, -7, -5, 6, -2, 5, -9, 0, -2, -3, 2, -14, 7, -2, 7, -5, -1, 3, -1, 3, -8, -2, 8, 2, -4, 7, -2, -5, 4, -6, -9, 0, 5, 1, -14, -3, -7, -3, -7, -11, -1, -7, 3, -3, 2, -14, -8, -6, 6, -12, -4, -1, 1, 0, -6, -10, -10, -3, -11, -2, 2, 2, 1, -12, 4, -3, -11, -12, 0, 5, 4, 9, -4, -2, -8, -6, 5, -1, 5, 4, 6, 1, -3, -2, -7, -10, 7, -2, 7, 8, 9, -7, -8, 3, 3, -7, -3, 6, -15, 7, -6, -6, -5, -3, -4, 8, 8, -11, -10, -2, -1, -5, 7, -2, -1, -8, 10, 4, -10, -11, -11, -8, 3, 5, 2, -6, 1, -8, -10, 0, -5, 4, -7, 3, -8, -3, -7, 4, -8, 4, -3, 0, -8, -7, 0, -2, -2, -8, -7, 6, -9, 4, -10, 5, 7, 8, -11, 2, -7, 9, -10, -6, 1, -11, 7, -11, -6, 7, -1, 8, -1, -12, -4, -1, -3, 2, -1, -10, -7, 1, 6, -7, -9, 3, -7, 5, -4, 5, -8, -7, -7, 1, 4, 7, -8, -3, -5, -8, -8, -4, -8, 1, 7, -10, 0, 2, -5, -7, 5, 2, -12, 6, -1, 1, 10, 4, -3, -1, -9, -8, -9, 9, -8, -3, -5, -5, -1, 2, -1, 2, 0, 7, -8, -4, 4, -4, -3, 1, 4, 8, 6, -8, -10, -5, -7, -3, -8, 2, -7, -11, -3, -5, 2, -5, -9, 1, 1, -5, -5, -5, -11, -3, 2, 1, 6, -6, 0, 1, -6, -9, 3, -4, -6, 7, -11, -4, -13, -2, -8, 6, 6, 3, 0, -8, 3, -10, -9, -11, 1, -10, -10, 3, -10, 4, -7, -1, -7, 6, -8, -6, -7, 2, 0, -9, 4, -1, -2, -2, -5, -6, -2, -8, -11, -14, -11, -5, -3, -4, -14, 1, 6, 2, -12, 2, -15, -5, -4, -2, 0, -5, -7, -9, 8, 1, 2, 1, -13, -1, -9, -1, -2, -2, -12, 3, -14, -10, -14, -15, 1, -6, 8, -12, 8, 2, -3, -4, -1, -7, -4, -1, 5, 1, 0, -5, -5, -3, -9, -6, -1, -14, -10, 4, -5, 0, 3, -1, -12, -12, -3, -7, 2, -6, -1, -11, 4, 2, -2, -11, -6, 1, -1, -2, 4, 8, -10, -1, -2, -2, -4, -8, 5, 0, 8, 0, 10, -10, 0, -14, -7, 4, 7, -7, 5, -10, 7, -7, -10, -8, -11, -2, -5, 6, -1, 4, 0, 9, 0, 1, -1, -10, -11, 3, -3, 2, 3, -7, 6}
, {21, -18, -21, -4, -41, -38, -9, -15, -9, -1, 4, -50, -9, -21, 25, -28, 25, -8, -22, 11, -9, 23, 27, -27, 2, 10, -12, -36, 16, 52, -11, 12, 50, -2, 3, -20, 10, 13, 13, -14, 1, 34, 8, -16, 4, -12, 64, 3, 1, 0, 16, 46, 4, 4, 21, -14, -17, -2, -17, 3, 43, 31, -9, -10, 46, 10, 15, -4, 60, 18, 12, 4, 4, 24, 12, -4, -1, 2, 15, 30, -16, 3, -4, 12, 9, -11, 8, -15, 25, -1, -1, 42, 27, 24, 5, -22, 3, 17, 27, -1, 6, -1, 11, 0, 3, 14, 30, 43, -10, -1, -27, 8, -10, 9, 15, -12, 2, -4, 3, 6, 19, 6, 3, 9, 2, -17, 22, 0, -41, 11, -3, 10, 15, 7, 6, 11, 19, 11, 21, 30, -4, -8, -36, 18, -6, -12, 15, -8, 1, 0, -52, 10, -5, 0, 19, 6, -34, -57, 3, 1, 5, 19, 16, 16, -1, 10, 7, -2, -23, 14, 19, -11, -13, -5, 32, 4, 3, 13, 12, 5, 6, 27, -9, -24, -7, -8, 18, 13, 5, 10, 8, 17, -10, 22, 5, 1, 15, 20, 24, 1, 22, -1, 18, 32, -6, -12, 7, 0, 1, -7, 8, 14, 1, 29, 1, -11, 8, 0, 17, 12, 0, 17, -22, 17, -10, 24, 1, -15, -4, 9, 15, -1, 4, -21, -6, -23, -8, 7, 9, -9, -42, -24, -5, -4, -4, 8, 8, -21, -8, -8, 9, 17, -11, 5, -4, -24, -9, -7, -2, 0, -8, 10, 7, -40, -18, -8, 4, -43, 1, -1, -22, -2, 12, 1, -15, -16, -7, 2, -7, 5, 28, -10, 6, 14, -5, -4, -28, -27, -17, -2, -7, -16, 17, 5, -9, 5, 6, -7, 9, -19, 3, -31, -33, 1, -3, -15, -1, -33, -2, -8, -40, -26, -23, 8, 12, -8, -18, -18, -32, -12, -2, 6, -16, 3, 0, -14, 19, -9, -15, 16, -6, -4, -17, -7, -16, 10, 13, -9, -2, -11, 4, 0, -25, -15, 12, 5, -13, 1, 6, 15, 2, 4, -4, -14, 7, -1, 17, -5, 17, 5, -9, 13, 11, -20, -16, 12, 0, 3, -15, -18, -28, 5, -1, -14, -38, -13, 8, 4, 9, 7, 6, 8, -15, 8, 8, -28, -33, -16, 13, 2, -27, -9, -14, -10, 11, -10, -2, -34, -30, 10, -22, -12, -45, -34, -6, -32, -33, -46, 6, 1, -15, -4, 2, -6, -23, -12, -13, -30, -15, -13, -7, -9, 24, -36, -30, -11, -7, -44, 3, -4, -28, -6, -37, -12, -35, -21, 0, -27, -20, -38, -11, 2, -30, 3, 9, -34, -26, -27, -5, -21, -14, -24, -3, -40, 6, -33, -21, 24, 5, -13, -10, 13, -19, 5, -53, -3, -15, -10, -3, -11, -50, -8, 2, -4, -8, -11, 0, -26, 9, -44, -4, -2, 2, 4, 5, 4, 15, -5, -2, 6, -3, -2, -15, 0, -3, -4, -6, 1, 7, -39, 8, -27, -20, -4, 9, 4, -11, 7, 7, 21, -16, -10, -2, 9, -27, 8, 12, 17, 2, -3, -10, 7, 12, 18, -5, -4, -13, 0, 8, -4, -12, -5, 1, -9, 16, -12, 5, -5, 5, 2, 23, -3, -17, -2, 21, 33, 22, 10, 9, -2, 4, -2, 13, -5, -42, 16, -8, -2, 12, -18, 7, 9, 18, -6, -2, -12, -2, -31, -11, -6, 12, 3, 14, -1, 17, 9, 7, -5, 3, -7, -18, -36, -23, 11, 8, 6, 3, 16, -7, -4, 10, -24, -11, -9, -5, -29, -2, -24, -6, -10, 10, 2, -29, -20, 8, 2, -9, 10, -4, -34, -26, -22, -20, -16, -45, -16, 0, -1, -3, -3, -10, -3, -20, -35, 6, -7, -30, -21, -4, -23, -3, -15, -9, 0, -11, -55, -13, 5, -10, 9, -1, 12, -3, -8, -24, -21, -13, -15, -7, 0, -3, 15, -15, -19, 5, -11, -2, 14, 4, 38, -7, -24, 8, -27, -1, -7, -13, -23, -15, 3, 11, -39, 7, -29, -13, 10, 7, -5, -10, 17, -17, -13, -28, 29, -10, 14, 0, 3, 15, 16, 17, 2, -5, -95, 14, 35, 3, -4, -32, -10, -28, 28, 18, 9, -17, 19, 15, 32, -26, 23, -28, -2, -12, -62, -34, 9, -1, 25, -16, -8, 18, 40, 24, -15, -3, -70, 21, 7, 35, -1, -3, 5, -8, -10, 30, 25, -18, -34, 4, 18, -38, -15, -16, 11, 44, 4, 17, -9, -17, 4, 16, -18, -8, 8, -9, 23, 6, -31, 9, -8, -10, 6, 18, -26, -2, -2, 10, -1, -11, -44, 7, 23, -20, -55, -33, 33, 48, 18, 34, 18, 1, 18, 29, -24, 32, 1, 6, 3, 10, -29, 32, 18, -27, 4, -11, -15, -2, 28, -1, 43}
, {-8, 6, -8, 1, -46, -35, -14, 0, -13, -2, -16, -11, -3, -28, 13, -41, 13, -5, -12, 22, -10, 5, 17, -20, -12, 16, -21, -37, 18, 24, 2, 6, 27, -26, -16, -1, 0, -8, -35, -14, 11, -25, -17, -14, 3, 1, 40, -7, 9, -12, 2, 24, -9, -2, 13, 3, -21, 16, -16, 9, 34, 33, -9, 2, 32, -25, -3, -10, 11, -15, -9, 6, -8, 16, 0, -12, -3, 14, -6, 11, -3, 5, -10, -15, 8, -25, -11, -2, 0, 5, -12, -13, 26, 13, -8, 4, -10, 1, 4, -14, -8, 6, 0, 3, -4, 13, 9, 18, -10, 20, -30, 21, 4, 22, 20, -8, -1, -4, -5, -17, 2, 16, 5, -5, 3, -13, 6, 3, -7, -20, 3, -12, -16, -6, -10, 26, 22, 9, 28, 22, 1, -4, 13, 4, -22, -7, 3, 30, -3, 8, -6, 1, -16, 3, 12, -4, -6, -7, 12, 4, -2, 13, 14, 13, 15, 31, 5, -12, -3, -5, 3, 7, -5, 24, -22, 11, -3, 2, 6, 9, -9, 4, 1, 50, -3, 4, -6, 22, -26, -8, 12, 12, -19, 11, 13, -23, 9, 15, 9, 0, 4, -11, -1, -17, -12, 5, -7, 2, 2, 3, -1, -30, -6, 23, 6, -11, -6, 8, 2, 7, -7, -36, 10, -4, -16, 10, -9, -6, 5, 12, 24, -15, -4, 0, -4, -23, -11, 16, -15, 4, -15, 6, 18, -41, 9, 10, 15, 21, -9, 4, 0, 14, 10, -22, 22, -7, -15, -14, -5, 1, -8, 0, -1, -28, -29, 9, -9, -26, -10, 15, -30, 3, 17, 3, 0, -47, 2, -20, 21, -25, 21, 10, 13, 22, -17, -3, -18, 0, -2, 4, 5, -15, 14, 15, 5, 24, 17, 25, 40, 41, -15, 7, -13, 20, -18, 37, 25, -7, -3, -11, 17, 16, 8, 12, 15, 3, -17, -9, 17, 9, -16, -26, 9, 3, -6, 5, 15, 15, 18, -14, -12, 11, -2, 16, -14, 1, 15, 0, -3, 8, -7, -4, -26, 35, -1, 6, -2, -8, -9, 3, 11, 37, 9, -38, -1, -1, -11, -8, -29, -2, -9, -2, -7, 2, -2, -2, -9, 11, -2, -29, -41, 6, 4, -13, -42, 7, 11, 0, -8, -8, 5, 9, -2, 10, 10, -33, -35, -25, -18, -36, -21, 5, -2, 23, -6, 4, 0, -27, -19, 3, -22, -8, -41, -1, -11, -4, -14, -29, 13, 6, -16, -42, 10, -4, -22, -18, -14, 12, 3, 0, -40, -12, 15, -6, -17, -17, 4, -25, -4, -4, 0, -8, -21, -10, -18, 4, -6, 0, 26, -15, 15, 1, -2, -29, 30, -2, -15, -25, 7, 28, -8, -6, -14, 1, 22, -4, -13, 11, -2, 13, -8, 17, -23, 6, 3, 18, 33, -28, -8, -15, 39, 37, 13, 6, 8, 14, 12, 18, 11, -22, -13, -8, 3, 11, -2, -4, -27, 10, 24, 1, -16, 28, 2, 9, 33, 3, 28, 6, 10, 32, -2, 6, -1, 19, 4, 12, 9, -9, 7, -7, 9, 51, -4, -11, 1, 7, -5, -5, -19, 21, -1, -17, -22, -1, 0, -8, 4, -36, -5, -9, -4, 4, -9, -17, -10, 22, -41, 5, 3, -25, -13, -21, 13, -9, 0, -15, 12, 9, -6, -4, -18, 4, 3, -2, -14, -16, 6, -11, -17, -5, -2, -10, -24, -3, 2, -9, -6, -16, 1, 4, 0, -8, 9, -4, -18, 11, -10, 0, 1, -9, 14, -2, 24, -1, 8, 20, 21, 0, 6, 6, -1, 22, 2, -2, 9, -12, 2, -10, 1, 17, 20, 17, -12, 10, -16, 3, 8, -4, 12, -11, -17, 32, 13, 14, 21, 15, 17, -12, 41, 16, -12, 23, 8, 14, 16, -4, -13, 35, -8, 19, 3, 47, 12, 7, -7, 25, -16, 5, 15, -1, 3, 12, 8, 7, 5, 23, -13, 34, 42, -7, -9, 66, -7, 2, 20, 25, 28, 2, 25, 42, -7, 13, -8, 33, -7, 13, 24, 16, -7, 10, 26, 33, -15, 0, 0, 17, -14, -7, 2, -24, -18, 5, -15, -7, 3, -9, 5, -10, 21, -8, 11, -5, -2, 8, -13, -10, -24, -3, 15, -1, 6, 3, -18, -1, -2, 2, 9, -11, -19, -15, 8, -38, -29, 2, 0, -31, -1, -7, -23, 5, -14, -1, -13, -19, 1, -23, -2, -45, 16, -1, -1, -7, 25, -5, -19, -23, 1, -26, 14, -26, -5, -13, 14, -6, -2, -7, 10, 0, 5, 10, -9, 1, -33, -45, 13, 8, 2, -15, 18, -46, 0, 3, -5, -7, -3, -21, -18, -29, 3, -15, -21, 8, 12, -30, -28, 25, 19, 2, 6, 23, 6, 14, 34, -7, 7, 15, -7, 54, -11, 25, 27, 40, 6, 4, -8, 25, 6, 20, 34, 2}
, {-4, 10, 6, 11, 9, 14, -12, 10, 32, 9, -4, -3, 3, 13, -10, -12, 20, -3, 7, -3, 7, -10, 6, 2, 9, -7, 1, -13, -28, 24, 0, 7, 18, -22, 5, -5, 8, -21, -8, 12, 10, 27, 5, -11, 6, -2, -4, 8, 31, -24, 1, 27, 8, 8, 9, -21, 23, -14, -1, -15, 7, 41, -15, 21, 18, -22, -28, -15, 7, -12, -31, -22, -42, -22, 2, -14, -16, -24, 15, -17, -15, -18, -9, 25, -10, -10, 14, -10, 0, -15, -40, 3, 18, 28, 2, -45, 3, 16, 36, -25, 6, 8, 5, -17, -16, -30, -3, -2, -6, 37, 11, 7, 2, -6, -6, -8, 6, -29, 20, 27, 22, -1, -8, 15, 7, 1, 20, -3, -31, -18, -21, 32, 11, 2, -7, -15, -10, -1, -5, -9, -4, -1, -17, 22, 15, 1, -26, -45, -3, -31, -8, -10, 42, -13, -15, 22, -19, -15, -7, -20, -7, 4, 20, 46, -4, 3, 7, -4, -4, -5, 3, -20, -9, 11, 17, -5, -9, 2, -8, 14, 0, 13, 6, -10, -9, -10, -5, 18, 15, 5, -4, 25, 15, -12, 10, 27, 14, 11, 3, 17, -3, -17, 10, 14, -9, 26, 16, 30, -2, 2, 22, 6, 6, 13, 24, -17, 9, -2, 1, 6, 4, -5, -8, 17, 7, -12, 2, -11, -4, -3, -3, -7, -28, -27, -15, 28, -3, 15, 8, 1, -6, -7, -6, 0, -5, -2, 27, 6, 9, -3, -13, 16, 9, 1, 7, -45, -2, -5, 2, -7, -9, -12, 7, 3, 9, -26, -18, 16, -4, 8, -28, -3, 24, 0, 1, 14, -10, -16, -5, 0, 5, -5, -4, 1, -12, -16, -3, 18, -25, -12, -13, 11, -34, -26, -8, -4, -17, -27, -35, -6, -2, -12, -11, -40, -4, -14, -7, -15, -4, -13, 10, 4, -48, -6, -30, -18, -7, -11, -9, -26, -4, 2, -7, 18, -12, -14, -1, 5, -1, -11, -8, 1, -11, 6, -6, 2, -4, -13, 4, 0, -8, -5, -6, 7, 2, -3, -4, 2, 1, -12, -3, 8, -6, 10, 11, -8, 3, 6, 2, -14, 11, 1, 5, -2, 4, 31, 4, -11, -17, 8, 0, 20, -7, -8, 1, 9, -4, -8, 12, -11, 2, -5, 5, -12, -12, 14, 20, -7, -2, 22, 6, 26, 19, 1, 5, 17, 8, 10, 10, 6, -2, 18, 26, 17, 6, -14, 8, 13, -7, -16, 20, -2, 5, -8, 7, 5, -36, 0, 5, 11, 7, -1, -12, 5, -6, -3, -12, -7, -3, 13, -12, 10, -1, -25, -10, 12, -7, -18, -11, -7, 9, -17, 0, 13, -20, -31, -8, 9, -19, -26, 10, -7, -18, -16, -20, -6, 2, 12, -20, -10, 4, -17, 6, -8, -13, -20, 1, 5, -2, -10, -31, -21, -20, -8, -16, -20, 0, -32, -5, 0, -1, -19, 3, -9, -3, 0, -3, -20, -12, -9, -9, -4, -12, -4, -23, -10, -22, -14, -13, -21, 6, -22, -12, -35, -2, -14, -16, -4, -7, -7, -10, -24, -12, 6, 7, -25, -17, 1, -17, -1, 7, 15, 3, 1, 8, -7, -10, -13, -29, -9, -19, 0, -1, -15, -15, 13, 6, -8, 6, -32, -17, -9, 5, -29, -5, -5, -14, -10, 22, 15, -5, 18, 3, -9, 3, 15, -13, -7, -29, 8, 5, -12, 24, 30, 0, 18, -1, -1, -7, 2, 8, 5, 1, 3, 9, 13, 4, 17, 19, 2, -34, -17, -20, -6, -3, 1, -2, 3, -6, -8, 12, -4, -4, -11, 5, 5, 1, 32, 6, 28, -5, -3, 12, 4, 6, -5, -2, -1, -5, -6, -20, -3, -9, -10, -26, 12, 20, 8, -2, -3, -1, 1, 8, -24, 22, 0, 1, 10, 8, 8, 12, 4, -1, -4, 12, -23, 19, 13, -18, -7, -17, 13, 10, 6, -24, -13, -35, -4, -31, -28, -51, 22, 0, -4, -8, -26, -30, 22, 0, 4, -10, -19, -20, -23, -14, -9, -23, -16, -22, -22, 23, -28, -6, -22, -7, -11, 23, -7, 20, -37, -33, -8, -2, -16, -4, -38, -26, 13, 11, 7, 6, -12, 5, -10, -9, -5, -26, -6, -5, -7, -38, -10, -17, -43, -8, 5, 22, -8, -12, 13, -7, -12, -29, -24, -29, -14, 3, 15, -15, -18, -11, 2, 8, -11, -6, -24, -13, -31, 7, -12, 20, 0, 29, -2, 7, -20, 6, -11, -27, -26, 2, 7, 23, -18, -10, 24, 31, -27, 10, -14, -19, 4, -35, -18, -1, 17, 1, 16, -3, -35, 23, -2, 7, 11, 11, -26, -28, -48, -6, -1, -2, 16, 2, -11, 25, 8, 25, 20, 43, -8, -3, 23, 7, 5, -7, -24, 5, 18, 4, 26, 14, 12, 18, 1, 15, 21, 3, -9, -29, 4}
, {6, 15, 32, -1, -16, 5, 13, -14, 33, 18, 9, 0, 16, 9, 15, 7, 15, 38, 32, 21, 6, 19, 8, 17, 37, -5, 20, -2, 5, 8, 8, -4, 27, 3, 3, 10, -9, -22, -15, -7, -25, 0, 4, 24, 7, -7, 8, -6, 6, -13, -35, 25, -1, -5, 9, -13, 18, -3, 14, -20, -7, 31, -10, -4, 16, 0, -1, -9, 9, -10, 10, 19, 17, -6, 5, 30, 4, -6, -6, -3, -12, -2, -2, 15, 7, -15, -8, 32, -1, -10, 3, -7, 7, 3, -9, -18, -13, -11, -2, 3, -27, -4, -8, -12, -12, -2, -22, -13, 10, -22, 0, -38, 6, -8, -12, 11, -1, -12, 2, 7, -41, 6, 7, 5, -1, 3, -5, 16, -6, -5, -12, -26, -1, 3, 5, -2, 11, 6, -18, 11, 5, -8, -16, -17, -10, 20, -6, 0, 7, -6, -16, 1, -26, -8, 11, 13, 0, -19, 9, -30, -18, 5, -5, -17, 25, 13, -5, -5, 30, 2, 11, 9, 12, 15, -13, 15, -19, 23, 17, -9, 5, -9, 4, -18, 15, -5, -17, 2, -2, -13, -2, 14, -4, -25, 4, 22, -2, -11, -5, 3, -11, -6, 2, 23, -1, 14, -3, -2, 11, 7, -29, 26, -7, -20, 12, 25, 5, -1, -20, -5, -34, -25, 9, 12, -15, -39, -21, 16, -8, 6, -26, 29, 18, -10, 12, 27, 6, 6, 12, 3, -5, -30, -12, -2, 3, -33, -24, 27, 34, -3, -11, 5, -35, -24, 0, -9, -7, -5, -16, -15, 8, -21, 17, 4, 5, -16, -2, -18, 13, -29, 23, -3, 9, -8, -25, 13, -3, -22, -22, -14, -32, -14, -29, -14, 1, 19, 2, 11, -9, -18, -3, -3, -2, -19, 5, 6, -7, -12, -13, -1, -5, -14, -5, -22, -1, -4, 0, -12, -8, -25, -8, -16, -17, -7, -26, -21, 0, 31, -14, -14, -10, 34, 5, -17, 10, 0, 13, 6, -12, 16, 20, -5, 0, 2, -4, 6, -15, 7, 17, 5, -10, 13, 7, -11, -2, 6, 20, 11, -6, -13, -9, -6, -24, 8, 10, -14, -4, -15, -11, 9, -6, -7, -5, 0, 5, -11, -7, -30, 2, -26, -31, 13, 2, -16, -38, 16, -15, -11, -13, -11, -21, -51, -12, 23, -30, -38, -24, -15, -25, 18, -14, 25, 28, -13, -5, 29, 11, -6, -12, -17, -9, -2, 8, 5, -2, -18, -32, 10, -9, -10, -6, -30, -34, -45, 4, -16, -10, -32, 3, -15, -14, -19, -15, 4, 1, 17, 4, -15, 13, -14, 9, -31, -7, -12, -8, 40, 9, -9, -14, -16, -1, 4, -25, -27, -10, -46, 10, 10, 13, -19, 2, -5, -5, -17, 5, 11, -11, -2, -5, 9, -3, -3, 13, -11, 3, 2, -2, 15, 4, -13, 4, -9, -13, 3, -7, -5, 1, -1, 12, 8, 14, -15, 3, -17, 19, -5, 12, -6, 10, 3, -10, 5, -9, -6, 17, 20, -2, 15, 6, 3, 7, -8, 2, -9, 3, -7, -4, 17, 14, 28, -8, -11, 23, -30, -52, -8, 0, -15, -28, -5, -19, 0, -3, 1, 1, -30, -13, -23, -8, -49, -44, -4, -5, -16, -46, 2, -1, 3, -19, -8, -13, 15, -8, 16, 9, -36, 0, -14, 2, 0, -15, 33, 17, -3, -18, 18, 13, 8, -16, -19, 0, 24, 19, 6, -3, -18, -1, 39, -44, -2, 12, -32, 2, -7, 25, -10, -12, -5, 32, -12, -16, 2, -4, 12, 14, 1, -6, -17, -6, -12, 8, -20, 5, 1, -15, 4, 1, -12, -2, -5, -8, -1, -12, -24, -11, 4, 3, 16, -8, -8, -11, 3, -3, -8, -5, -4, -9, 0, -10, 0, 1, 25, -18, -6, 8, -21, -17, -23, -6, -9, -9, -19, 0, 5, -16, -9, -31, -7, -4, -8, -4, 16, 1, -13, -10, -28, -3, -4, 7, 2, -1, -6, 6, 2, 7, 15, -1, 9, 1, 6, -10, 30, 10, -5, 15, -6, 16, -10, 22, 8, -16, -6, -13, 7, -28, -21, -13, -4, 12, -21, -20, 15, 1, 1, 11, -44, 14, -21, 2, -15, -16, 32, -7, 2, -1, 12, 0, -10, 6, -26, 9, -23, -10, 16, 2, 16, -1, -7, -8, 1, 6, 27, 24, -8, 7, 26, 11, 16, -5, -9, -19, -1, 12, 22, 0, 12, -13, 13, -12, 0, 10, 1, -11, -13, -5, -10, -18, 6, 5, -16, -6, 1, 10, 12, 6, 0, -1, 9, -9, -10, -1, -17, 6, 12, -15, 26, 2, 9, -14, 6, -6, -3, 5, -30, 4, -18, 8, 18, -2, -17, -11, 29, -6, -28, 6, -37, -35, -8, -14, -25, -4, -11, 0, -17, 3, -19, -30, -28, -8, 11, -11, -36, -5, -7, -15, -5, -1, 11, -26, -12}
, {-9, -23, -30, 10, -44, -23, -34, -45, -42, -33, -51, -43, -11, -48, -20, -66, -8, -21, -45, -31, -7, -43, -3, -43, -20, -6, -28, -31, 4, 5, -19, -40, -13, -3, 16, -41, -26, -7, -2, -10, -47, -15, -31, 2, 9, -2, -6, -15, -19, 11, 7, -20, -10, -35, 32, 5, 16, -4, -14, -33, -16, -8, 4, -32, -28, -15, 9, -25, -36, -9, -5, -5, -2, 4, 9, 10, -1, 19, 0, 19, 8, 15, 3, -2, -9, -17, 12, 25, 40, 1, -17, -3, -13, -10, 9, -9, 4, 2, -21, 18, -3, 0, 9, 17, 8, -27, 1, -29, -12, 17, 0, -8, -7, -43, -23, 6, -1, -1, -7, 0, 37, -12, -7, 5, 10, 17, -24, 16, -5, -20, 1, -10, -18, -27, -5, -1, 11, -14, -16, -41, -10, -18, 2, -22, -10, -39, -22, -15, 3, -17, -2, -17, -26, 7, -19, -26, 13, 11, -29, 13, -10, -39, -9, 11, -13, -24, -24, -37, -8, -4, -10, -24, -5, -17, 20, -4, -22, 7, -2, -22, 7, -23, 30, -18, 3, 2, -39, -20, -24, 4, 15, -28, -16, -10, 8, -1, -13, -4, 5, 3, 13, 0, -6, -4, 8, 14, 1, -5, 21, 6, -4, -7, -8, -8, 27, 21, 0, -11, -4, -10, -37, 7, 1, 28, 5, 3, 11, 11, -16, -13, -12, 10, 20, 14, -8, 19, 2, 18, 13, -15, 17, 7, 1, 14, -9, 23, 13, 12, -11, -11, -8, -2, 0, 8, -7, -4, -1, 27, -9, 27, -4, 28, 10, 27, 5, 11, -7, 11, -1, -20, 9, -23, 10, -2, -2, 8, 5, -12, -1, 2, -3, -7, 5, -16, 20, 13, 20, 22, 19, -35, 0, 4, -3, -6, -16, 8, 17, 18, -5, 24, -3, 16, 3, 6, 13, 5, 7, -6, 6, 10, 8, -10, 6, -12, -18, 5, 3, 21, -3, 3, 1, -10, -14, -8, -8, -9, 18, -13, 1, 17, -1, -7, -15, -15, -13, -4, 0, 3, 14, -19, 4, -4, -9, -19, -7, -6, -8, -2, -5, -4, 4, 0, 19, 4, 5, 31, 13, -8, 24, -9, 12, 20, 4, -4, -16, -9, -19, 0, 11, -15, 14, 5, 0, 7, -5, -14, -8, -4, 15, 1, -6, 5, -19, 17, -3, 27, 1, -27, 29, 1, 27, 4, 7, 17, 8, 17, -13, 3, 2, 21, 1, -11, -1, 29, -10, 6, 4, -7, -10, -6, 17, 20, 12, -15, -1, 4, -13, -2, -1, -4, 38, -8, 7, -7, 15, -17, 13, -25, -12, -5, 42, 25, 8, 7, 7, -4, 7, -4, 5, 5, 4, 13, -15, 20, -2, -3, 9, -7, -11, -17, 9, 8, 7, -13, -15, -1, 15, -26, 3, 28, 10, 41, 15, 21, -3, 12, 5, 1, -2, -12, 3, 23, 13, -2, -8, 20, 15, 6, 18, -8, 15, 10, 33, -5, 4, -23, 10, 12, 5, -10, -20, 17, -12, 27, 26, -3, 16, 6, 13, 18, 7, -10, -5, 15, 8, 0, -4, -13, 6, 3, 2, 19, 13, -24, 6, 23, -4, -3, 6, -8, -4, 28, 21, -15, 0, -15, 9, 13, 7, -16, 1, -15, 4, -13, -8, -32, 22, 2, -6, 9, -1, 5, -27, -2, 5, 4, -1, -4, 22, -23, -14, -14, -15, -4, 8, -13, 6, 3, 4, -1, -2, -14, -11, 3, 9, -9, -12, -6, 0, 0, 12, 2, -2, 7, 5, -8, 14, -4, 13, -19, 12, 5, 20, 11, 4, 9, 27, 15, -11, 27, 7, 4, -12, -4, 30, -7, -10, -12, 2, -3, -7, -9, 25, 2, 3, 14, -6, 7, -21, 23, 14, 31, 2, 9, -13, -27, -19, 17, -1, 4, -11, 3, -26, -14, -13, 10, 18, -15, -7, -19, -6, -16, 4, -14, 12, 13, 8, -10, -13, -3, 2, 2, -7, -14, 6, 24, 9, 17, 17, 7, 11, -18, -5, -8, -2, -11, 1, -12, 21, -29, 1, 3, -13, -1, 3, -8, -3, -1, 15, -5, -12, -2, -17, -10, 7, 1, -10, -27, 3, 34, 33, 12, 17, 9, 2, 17, -1, -6, 20, -11, -11, -11, -4, 17, 7, 9, -20, 4, 3, -19, -14, 24, 2, 22, 0, 9, 8, 7, -7, -8, 21, 30, 10, -37, -28, 9, -13, 2, 22, -11, 18, -3, -10, 10, 9, -3, 14, -3, -23, 3, -8, -4, -17, 14, 22, 35, -12, 0, 42, -2, 29, 25, 30, -17, -12, -1, 1, -26, -12, 14, -18, 30, 2, 20, 21, -6, 3, -14, 37, -17, 35, -8, 1, 26, 10, -2, 1, -28, -21, -21, 10, 36, -18, 10, -7, -12, -18, -6, -10, -35, -3, 11, -42, -12, 8, -7, 7, -39, -1, -27, -12, -43, 4, -12, -10, -31, -12, -20, -30, 11}
, {5, 1, 3, -9, 5, 2, -13, 5, 2, -6, -10, 8, -3, -1, 6, -11, 1, 7, 6, -9, 0, -1, 6, 7, -4, -3, -5, -11, 1, -8, -8, -6, -7, -3, -11, 3, 4, -2, -12, 7, -1, -11, -6, 6, 5, -8, -3, -10, 7, -13, 2, 3, -1, -2, -9, 7, 4, 6, -2, -14, 2, -11, 3, -9, -12, -6, -18, -2, -3, -3, -4, 1, -9, -9, 5, 7, -4, -3, -6, 2, -2, 3, 9, 2, -1, -4, 1, -4, -2, -9, -13, -2, -11, -9, 3, -4, -2, -9, -10, 0, -15, 6, -4, -9, 2, 2, -3, -3, 5, 2, -11, -6, -1, -7, 0, 5, -9, -12, 0, 3, -4, 7, -6, -2, -10, -3, -3, 7, -3, -3, 5, 1, 2, 14, 5, 3, -6, 4, -9, -2, -5, 0, -13, -6, -9, -4, -6, -2, 9, -8, -13, -7, -6, 3, 0, -1, -5, -6, -11, 8, -10, 5, -1, -2, 2, -12, 4, -8, -8, 6, -14, -1, -5, 6, -2, 1, -2, -5, -9, -1, 9, -2, -11, 4, -11, -6, 8, 5, -6, -13, 3, 9, 4, 3, 5, -12, -5, -5, 5, -11, -14, -10, -11, 4, 7, 1, -9, -10, 2, -4, -6, -10, -4, 3, 1, -10, -7, -8, -8, -1, 2, 3, -6, 4, 1, 0, -5, -4, -8, -7, 7, -5, -9, -3, -8, 3, 8, -5, -13, -15, -10, -10, -6, -14, -10, -15, -12, -10, -3, -5, 0, -12, -7, -10, -18, -10, -4, -8, -11, 3, 5, -7, 6, -8, -6, -7, -3, 0, 8, -10, -4, -9, -8, -10, 0, -10, 9, -3, -3, -9, -1, -10, -17, -11, -4, -4, -2, 2, -6, 2, -5, -6, -11, -5, -12, 7, 0, -12, -3, -12, 4, -1, -16, -7, 6, -11, -10, -12, 1, -8, -15, 7, 2, -10, 1, 2, -6, 3, -4, 2, 7, -8, 0, 1, 6, 5, -8, 6, -9, -6, -16, -9, 1, 5, -4, -11, 2, 2, -11, -10, 6, -4, -7, 1, -14, -8, -6, -3, -4, -11, 8, -3, -6, -2, -1, -8, -13, 9, -10, -12, -9, -15, -8, -10, 4, 2, -7, -5, -14, -1, -7, 1, 2, -17, -1, 5, -6, 3, -1, -15, 6, 1, 2, 0, -6, -2, -12, -2, -14, -13, -13, 5, -4, -8, -6, 3, -10, -1, -7, 4, -12, 8, -4, -9, 4, -16, 2, 9, -9, 3, -5, -4, -4, 3, 0, -4, -6, -6, -4, -6, -9, 1, -6, 4, 1, -6, -15, -13, 2, -11, -19, -2, -2, 9, -12, 3, 3, -14, 3, 0, -5, 1, -3, 2, -9, -12, -1, -2, -3, -2, -7, 1, -8, 6, -6, -5, -8, -11, 2, 1, -3, -3, 2, -10, -5, 2, -6, -7, -11, -2, -1, -8, -3, -2, -1, -14, -10, 2, -10, -1, 2, 3, -8, -4, -5, -7, 3, -13, 3, -1, -6, -9, 0, -5, 0, -4, 3, 2, -4, -1, 7, 0, 0, -10, -3, -1, 0, -14, 2, 0, -7, 2, -8, -1, -10, -12, -5, 5, -9, 2, -18, -5, -6, -13, -4, 2, 2, -10, 2, -1, -5, -8, 2, -10, -13, 5, -8, 1, -1, -9, -10, -5, -3, -6, -16, 6, -16, -4, -2, -5, -10, -1, -11, 1, 1, 8, -2, -1, 4, -9, -11, 7, -5, -1, 8, -10, -13, 13, -3, -7, -12, 0, -3, -11, 5, 1, -4, 2, -12, -11, 2, -10, -12, -14, -8, -13, -15, -5, -7, -5, 3, 3, -12, -14, -7, 0, 5, -15, -12, -1, -5, 4, -3, -4, -7, -2, 0, 3, -10, 2, 2, 1, -1, 0, 6, 0, -1, -9, -3, -5, -6, -12, -7, 3, -14, 8, -3, -10, -4, -4, -14, -4, -8, 6, -1, 2, -12, 2, 1, -12, 1, -7, -10, 6, -7, 1, -6, -12, -9, -2, -12, -14, 2, -7, 4, -13, 14, -8, -4, 1, 4, -9, -8, -7, -12, 5, -5, 5, 2, -2, -12, 2, 4, -13, -8, -4, -17, 1, 0, -11, -4, -13, 3, -2, -7, -6, -8, -15, -1, -5, -5, -10, -11, -11, 5, -13, -14, -3, -11, -13, -13, -1, -10, -3, -5, 4, -7, 3, 0, 4, -14, -3, 4, -12, 2, 13, -7, 0, -14, -11, -4, 4, 1, -11, 1, -15, -16, 19, 2, -10, 2, -12, -3, -1, 7, -8, -14, -12, -6, 5, -1, -15, -4, -5, -8, 4, -4, -1, -10, -11, -10, -8, -1, 0, -16, 15, -4, -6, -11, -4, 1, 1, -9, -14, -12, -5, -12, 1, 4, 10, -7, -6, -6, -13, -9, 0, -11, 0, 0, 6, 6, -2, -10, -6, -6, 1, -6, -11, 1, -13, 0, 1, -1, -11, 6, 0, -1, -9, 1, -6}
, {11, 20, -3, -7, 14, -10, -9, 13, 35, 7, 19, 23, 11, 23, 21, 28, -21, 23, 9, -14, 0, 7, 0, 20, -1, -8, 28, -15, -22, -6, -1, 8, -40, -6, 26, 5, 10, -9, 11, 32, 22, 23, 12, -7, -3, 16, -22, 1, 22, 1, -4, -9, -9, 18, -2, 28, -6, -2, 28, 3, -19, -15, 18, 36, -14, -20, -13, 24, -13, -33, 7, 5, 7, 24, 7, 5, -3, -2, 3, -22, 20, -17, -6, 12, 5, -7, 20, -18, -11, -5, -16, -12, -23, 8, -3, 29, 1, 18, 7, -2, 17, 4, 14, 23, 17, 38, 27, 8, 5, 11, 5, 3, -18, 17, 25, -8, 0, -13, 3, 7, -6, -5, 22, -16, -9, 13, 18, -5, -18, 20, 13, -3, 0, 20, 4, 19, 13, 11, -7, 23, 5, 10, -13, 8, 14, 20, 18, -1, -8, 17, -20, 19, 18, 7, 11, 18, -2, -37, 25, 15, 6, -11, -2, -11, 7, -20, -1, -12, -11, 13, 13, -15, 4, -34, -2, 9, -9, -2, 8, 2, -6, 0, -11, -30, 3, 10, -5, 0, 6, 14, -15, -16, 19, -13, 4, -15, 0, -16, -2, 22, 20, 8, 14, 15, 3, -10, 30, 12, -13, -11, -12, 15, 7, -2, -7, -6, 8, 10, 19, 0, 11, -13, -4, 2, -19, -9, 3, -20, 1, -2, -2, 8, 3, 17, 10, 32, -1, 1, 2, 4, -8, 14, -9, 8, 3, -5, 3, -11, 6, 1, -16, -12, -17, -33, 7, -19, -11, 22, 11, 19, -3, 8, 17, -10, -16, -2, 0, -18, 9, 6, -19, -2, 12, -6, -10, -14, 8, -7, 12, -1, 22, 7, -13, -7, 0, -16, -14, 2, 1, 11, -6, 17, -10, 3, -6, -15, -6, -2, -16, -11, 13, -8, -39, -3, -1, -21, -6, -23, -1, -10, -22, -20, 15, 6, -17, 0, -15, 3, -20, -15, 6, 13, 10, 1, -4, 0, 23, 11, 0, 12, 18, 3, 19, 7, 21, -1, 4, 10, 19, 13, 5, 22, 27, 2, 5, 3, 3, 6, 11, 12, 8, 27, -5, -1, 5, -6, 3, -8, 2, 18, 24, 2, -7, 21, 6, 6, 22, -10, 5, -4, 2, 29, -2, -13, 1, 14, 8, -8, 17, 7, 14, 10, -4, 38, 0, -16, -3, -8, -3, 29, -15, 11, -11, 3, -12, -17, 9, 17, -3, 15, 6, 0, -1, -4, -7, -13, 0, 4, 5, -7, 1, 23, -13, 2, 9, 4, -12, -14, -8, 20, 9, -10, -33, -17, -21, -37, -17, -6, 3, -10, -31, -18, -2, -15, -20, -20, 7, -15, -25, -10, -6, -1, -7, 3, -27, -15, -15, 9, -8, -15, -10, -19, -30, -15, -10, -11, -12, 2, 2, -2, 2, -21, -9, -6, -6, -3, -7, 8, -7, -7, 4, -8, -3, 1, -7, -11, 3, 10, -7, -9, 17, 13, 20, -2, 9, 26, 12, -16, -10, 14, 18, -10, 17, 11, -7, 34, -17, -8, 4, -18, 3, 17, -21, -11, 10, 9, 18, 39, 4, 4, -2, -23, -1, 16, 10, -14, 8, -5, -25, -4, -20, -2, 2, 35, 7, 0, -8, 30, -8, -2, 14, 6, -11, 19, -10, 17, -6, 8, 20, -2, -4, 9, 4, -9, 1, -17, -6, -11, 3, 17, -6, -8, -15, -18, -21, -13, 10, 12, 15, -4, -5, -6, 3, -14, 8, -1, -14, -1, -7, -3, -8, 14, 4, 8, 3, 7, -10, -1, -2, 9, -18, -9, 6, -19, -8, 2, -23, -13, 11, 0, -16, -23, 20, 22, 12, -8, 7, -7, 10, -27, -9, 7, -13, 3, 20, -9, -3, -4, -14, 28, 21, -18, -5, -13, 22, -16, -6, 2, -22, 12, -2, 1, 4, -14, -17, 4, 24, 4, 6, -3, 8, -17, -1, 8, 13, -8, 20, -7, 8, -28, 2, 7, -6, -2, -22, -34, 19, -12, -6, -4, 2, -5, 18, -12, -7, 3, 16, 28, 2, -10, 8, -10, 4, 9, 10, 3, -4, -15, -3, 16, 6, -7, 15, -17, -13, 5, 3, 3, -2, -2, -14, -25, -19, 0, -9, 18, -7, 17, -22, 9, -26, 18, -8, -17, 12, 16, 20, 9, -18, 27, 6, 16, 17, -18, -14, -2, -24, 15, 2, -13, -16, 31, 8, 5, -7, 22, 6, -1, 27, -2, 5, 29, 1, -26, -10, -21, 23, 38, -14, -11, -30, -4, -40, 9, 20, -3, -3, 11, 0, 22, -9, 6, -15, 14, 4, -15, -24, 0, 5, 18, 19, -20, 20, 34, 5, -16, 7, -10, 8, 38, 6, -13, -25, -7, -8, 13, 14, 29, 3, 17, -6, -28, -31, 22, 4, -33, -28, -1, -13, -18, 5, 1, -13, -7, 3, 12, -19, -23, 4, -24, -12, 1, -9, 5, -4, -14, -5, -22, 4, 2}
, {12, -5, 7, -1, 6, 3, -4, 24, 28, 43, 18, 30, -6, 14, -34, 24, 2, -2, 27, -28, 7, 7, -23, 18, -22, -2, 22, -16, -15, -4, 4, 5, 15, 11, 18, 11, 19, 9, 8, 37, 13, 11, 11, 50, 2, 0, -30, 24, 15, -12, 18, -2, 6, 7, -22, 16, 21, -5, 28, -10, -17, -13, 5, 30, -20, -20, 7, 13, -8, 18, -4, 12, 9, -17, 9, 2, 3, 0, 8, 4, 18, 10, -9, 17, 6, 5, 13, -8, 11, -13, -7, -5, -36, 5, 1, 12, -31, 4, -1, 16, -4, 2, 19, -10, -13, 2, -4, -6, 12, 18, 2, -21, 5, -7, 9, 11, -1, -29, 19, 31, -7, 1, 4, 10, -1, -5, 14, -14, 10, 5, 16, 17, -25, 24, -2, -3, -22, -13, -19, -14, 3, 1, -5, 21, 7, 36, 11, 0, 0, -22, 12, 32, 29, -8, -20, 17, -5, -7, 8, -3, -16, 3, -2, 13, -12, -10, 8, -6, 9, 8, 10, -25, 4, -20, 24, 5, -24, -25, -2, -10, 2, -5, 5, -43, -6, 0, 21, 13, 37, 0, -29, -24, 27, -13, 10, 8, 11, -6, 4, -2, 2, 1, 7, 6, -6, 1, 20, 12, -29, -15, -10, -3, -4, 1, 15, -12, 24, -15, 3, -5, 6, -12, -12, -46, -10, -2, 12, -10, -11, 2, -5, 3, 4, 5, 0, 19, -10, -2, 8, 5, -19, 25, 5, 29, -4, -12, 8, -19, 27, -13, 3, 6, -25, -38, 10, -10, 2, -6, 3, 15, -19, -25, 6, -4, -10, 24, -6, 6, 6, -1, -7, -4, -1, -18, -3, 3, -2, -16, -6, 11, 5, 2, -14, -17, 11, -12, -12, 6, -35, 8, -9, 6, -21, 2, -6, 14, -2, -11, -52, -21, -2, -10, -18, -20, 3, -6, 13, -20, 4, -7, 1, -7, -18, -9, -6, -6, -14, -20, 0, 14, 13, 1, 1, 1, 4, -8, -9, -3, -25, -11, -4, -24, -7, -12, -35, 8, -28, 5, -21, -15, -4, -6, 3, -30, -2, -12, -12, 7, 14, 18, -13, -5, -7, 18, 6, -19, 33, 27, -9, 1, -21, 14, -7, -21, -10, -6, -44, -6, -50, 9, -2, -11, 0, -58, -45, 11, 4, -14, -12, 17, -15, -5, -4, -23, -19, -9, 0, 14, 23, 23, -46, 42, 9, -4, -3, -10, -1, -3, -4, -11, -5, 0, -2, 10, -10, -32, -13, -3, -12, -18, 3, -3, -22, -22, -4, 28, -12, 11, 5, -4, 4, 4, -30, 5, -3, -11, -22, 15, 8, 8, -27, -15, -1, 26, 2, 7, 3, -6, 1, 26, -15, -8, 13, -25, -21, -23, 17, 15, -19, -24, -6, 10, 3, -4, 0, 18, -7, -13, -16, 8, -6, -18, -13, -4, 13, 1, -21, -2, -5, -4, 3, -1, -8, -11, -8, -1, -13, -19, 4, 26, 12, -32, -15, 8, -9, -16, -34, -9, -31, -29, -19, -32, 5, -3, -49, -30, -4, -26, -7, -34, -2, 14, -18, -27, -3, -13, 3, -6, -10, -9, -6, -19, 15, -32, -30, 0, -49, -31, -52, -15, -34, -8, -21, -5, -2, -23, -4, -42, 0, -22, -21, -11, -6, -21, -27, -11, -17, -7, -19, -59, -10, 6, -30, -16, -19, -24, -14, -18, -20, -12, -19, 15, 4, 6, -9, 14, -1, 2, -24, -17, -1, 25, 17, -5, -10, -19, -10, 3, -15, 2, -7, -41, -12, -8, 5, -19, 0, -2, 1, 4, -10, 17, -30, -11, -2, -8, -10, 2, 1, 2, -2, -15, 0, 5, 3, 9, -10, -13, -4, 26, -13, -12, 3, -5, 13, -3, 9, 10, -11, 14, -3, 2, 0, 0, -14, -10, -5, 5, -34, -32, -1, -8, 10, -3, 6, 5, -2, -2, 1, -29, -5, -2, -3, -5, 3, 5, 17, 3, -3, 14, -11, -1, -3, 8, -14, -15, 2, -19, -41, -28, -28, -47, -5, -11, -38, -33, 28, -18, -7, 4, 7, 16, -10, -23, 5, -11, 19, -7, 8, 23, -16, -37, 23, -13, -26, -14, -7, -32, 35, -1, -5, -12, 3, 8, 8, -1, 7, 2, -4, 6, -15, 21, -11, -3, 12, 20, 8, -12, 5, -16, -3, 19, 11, -25, -15, 5, -22, 0, -14, -17, -1, 48, 37, 11, 1, 12, 8, 27, 38, -34, -11, 14, -12, -12, -7, -16, 3, 23, -32, 1, 1, -8, -1, -11, 22, 19, -1, 12, -25, 8, -7, 5, 12, 21, 10, 24, -18, 1, -1, -14, 13, -8, 21, 25, 7, 19, 1, 22, 3, 27, 3, -1, 17, -11, 12, -8, 5, 17, -23, 19, 23, 6, -25, 0, 35, -6, 5, 0, 29, -23, -10, 7, 0, 1, 13, -11, 4, -5, -4, -4, 0, -5, -34, 9, 30, -18, 14, -25, -23, 16}
, {-2, 6, 7, 0, -13, -22, -4, -17, -14, -1, -7, -31, -14, -1, 25, -10, 7, 15, 14, 0, 8, 21, 23, -9, 12, 19, -3, -7, 8, 7, 0, -7, -14, -1, 10, 23, 2, -7, -10, -33, -13, 2, -18, -54, -14, -8, 12, -15, -5, -8, -2, -30, -2, -33, 11, -31, -7, -3, -40, 2, -13, 3, -14, -17, -2, 22, 14, 10, 25, 5, 1, 19, 8, 51, 11, 41, 3, -15, -39, 2, -10, -11, -6, -55, -5, -33, -12, -3, -7, 15, 18, -6, -12, -15, -1, 12, 6, -9, -21, 6, 13, 1, 6, 14, 25, 23, -1, 36, 9, -10, -2, -17, 7, 3, 16, -2, 0, 14, -16, -32, -34, 9, 0, -6, -26, -6, 5, 16, -20, -17, -20, 2, -25, -31, -2, 22, 9, 17, 10, -5, -3, -14, 3, -17, -14, 13, 6, 20, -9, -4, -16, 15, -7, 1, -2, -13, -12, -20, 13, -2, 6, 8, 2, -5, -11, -4, -7, -37, -29, 3, -2, -23, -4, -9, -21, 16, 5, 1, 8, -15, 9, -13, -17, -44, 16, 16, -19, -15, -11, 2, -10, -4, -35, -17, 0, -2, 1, -4, 5, 8, -7, -21, -3, -18, -5, 0, 9, -3, 2, 9, -3, -8, -1, -6, -15, -1, 1, 10, -4, -4, 3, -5, -1, 2, -16, 0, 7, 39, -15, 6, -6, 12, 12, -13, -16, 16, -14, 16, -12, -16, 14, 8, -5, -35, 7, -18, 9, 21, -18, 13, -4, -7, -1, -5, 36, 13, -7, -22, 1, -22, -15, -15, -8, -14, -8, -17, 13, 6, -8, 2, 9, 5, 5, 3, 11, -13, -2, -14, 23, 10, 19, 10, -20, 3, -7, 20, 9, -10, 12, -6, -21, 4, -1, -17, -10, -2, 9, 1, -10, 1, 5, -5, -7, 2, 11, -8, -6, -20, 3, -24, 0, 14, -2, 11, -20, -17, -14, 19, -5, -10, -2, 35, -18, -4, -14, -12, 1, -10, 14, -12, 4, -29, -10, -6, 8, -20, -6, 5, 9, 1, 3, 20, 2, -16, -5, 19, 11, -4, -10, 6, -2, 0, -22, -3, -6, -8, -30, -38, 14, 25, 26, -4, 9, 2, 8, 14, 24, 8, 29, 5, 22, -6, -1, 17, 16, -15, 4, 2, 22, -11, -8, 8, 16, 13, 5, -25, -3, 9, -6, -30, -17, 3, 4, -28, -19, 0, 2, 14, 2, -4, 39, -1, 3, 18, -4, 0, 9, 19, 18, 4, 3, 2, -2, 11, -1, 15, -7, -15, -7, -2, -23, -15, 27, 16, -10, 16, 10, 23, 2, 12, -9, 4, -16, -7, 12, 6, 5, -7, 3, -14, 2, 9, 13, -26, -1, 1, -22, 4, -24, 33, 26, 21, -25, 19, 2, -3, 0, 18, -26, -9, 6, -11, -13, -23, -12, 40, 29, 5, -9, 5, 9, -9, 2, 19, 20, 0, -14, -42, 15, -11, 7, 2, -7, 4, 21, 10, 15, -25, -35, 12, 13, -5, 0, -35, -15, 23, -2, -27, -12, 3, 10, 13, -12, -39, 5, 12, -6, 29, -2, 25, 2, -23, 7, 26, 2, -19, 33, -5, 25, -5, -5, 21, 10, 23, 2, 8, 3, 35, 3, 0, 19, 42, 4, 34, -14, 7, -25, 2, 24, 27, -9, 11, -2, -14, -1, 6, 4, 3, 0, 3, 15, -5, -12, 4, 4, 4, -2, 23, 25, -3, -15, 8, 2, -15, -2, 17, 25, 6, 8, 18, -5, 30, 0, 3, 5, -13, 1, 4, 5, -2, 9, -3, 12, 19, 6, -6, -6, 14, 2, -5, 10, -4, 8, 0, -3, 1, -5, 8, 19, 2, 26, 15, 16, 3, -8, 11, -8, 4, 14, -20, -3, 11, -14, -4, -12, 19, -1, -12, 20, -8, -7, 9, -1, 5, 36, 27, 4, -14, 1, 6, 7, 12, 0, 11, -14, -4, 7, 32, 11, 18, -4, -19, -5, -12, 6, -6, 23, -11, -46, -3, -9, -25, -2, 4, -38, 10, -45, -17, -38, -8, 5, -7, -5, -26, -17, 16, -35, 28, 5, -32, -28, -37, 13, 13, 12, 6, 27, 29, -8, -16, -13, -7, -10, -37, -13, -8, -2, 4, -35, -7, -8, -18, 8, -16, 5, -38, 19, 11, -14, 23, 1, 4, -35, -18, 13, 8, 10, 4, 4, 6, -32, -35, -11, -15, -5, -5, 1, -8, -18, 18, -6, 32, 26, 13, 0, -2, 13, -4, 17, -2, 13, -2, 9, -3, 7, -19, -2, 30, 5, 7, -3, 7, -7, 7, 18, -16, 7, 24, -7, 20, -3, 10, -1, -2, -15, 7, -9, 21, 0, 8, 5, 13, 17, -10, 7, -5, 32, -6, 15, 14, 2, -5, 13, 4, 10, 7, 9, -1, -6, 21, -1, 17, 18, 31, 11, 10, -10, 15, 5, 4, 10, 32, 26, 9, -22, 24, -4, 3, 42, -9}
, {14, 4, 2, -14, 14, -2, 10, -4, -4, 22, 18, -15, -15, 13, -2, 5, -11, 14, 5, -13, -5, 10, 18, 8, -18, -9, 13, 0, 7, 0, 9, 5, 24, 9, 33, -3, 28, 3, 24, 24, 6, 18, 29, 7, -12, 24, -12, 58, -8, -8, 15, -17, -8, 3, 13, 9, 35, -3, 11, -10, 14, 5, 6, 28, 27, 6, -2, -2, -17, 3, 7, -4, 1, 20, 8, -9, -17, -10, 17, 28, -12, -4, -3, -3, 2, -1, 6, -5, 34, -15, -1, 20, 1, 19, 2, -12, 22, 19, 2, -5, 39, 10, 19, 12, 4, 17, 13, 12, -10, 13, 3, 12, 6, 12, 17, 1, -3, 13, -3, -27, 6, -13, 1, 10, 3, 8, -16, -3, -29, -14, 8, 12, 8, 5, -11, 21, 8, 9, -7, 10, -5, -9, -1, 13, -3, -3, -11, -6, -4, 9, -21, 8, 21, -3, 4, 4, -17, -25, 11, 7, -3, -9, -9, -1, -16, -21, -10, -23, -16, 1, -6, -10, 0, -22, 9, 8, -6, 4, -17, -1, -7, -14, -4, -39, 16, 6, -1, -12, 20, 0, -17, -43, 10, -18, 1, -16, 0, -3, -12, 10, 5, 1, 11, 16, -1, -2, 17, -4, 5, -1, -1, 15, -9, -3, -11, 29, -8, 2, -8, -15, -10, -11, -3, 17, -24, -4, 16, -10, -1, 7, -1, -5, -5, -5, 7, 16, -10, 10, -1, 8, 20, -12, -6, -3, -11, -10, -27, -15, -4, -13, -1, 4, 4, -28, -13, 1, -6, -7, 2, 27, 7, 8, 14, 2, 3, -2, 15, -13, 0, -12, -14, -1, 2, -36, -9, -1, 4, -17, -18, -47, 3, -3, -7, 3, -11, 7, -38, 0, -3, -7, -27, 10, 16, -12, -17, -9, 7, -8, -2, -8, -1, -18, 3, 0, -2, -34, -19, 12, -2, 2, -29, -14, 12, -8, 2, 1, -30, 10, -29, 6, -1, 30, -12, 3, -8, -8, 15, 4, 21, 10, 0, -10, -3, -6, 1, -15, 0, 29, 20, -20, -8, 13, 7, 6, -1, 5, 5, -15, -4, 12, 14, 7, -21, 0, 14, -1, 3, -7, -8, 20, 27, 4, -5, 15, -2, -3, -26, 2, 8, 16, 35, -2, 5, 17, -9, -9, -1, -7, 6, -10, -1, -9, -7, 21, 5, -24, -14, 15, -3, -27, -25, 24, 16, 10, -8, 25, 1, -18, -1, 5, 10, -26, -19, 4, 5, 1, -39, -19, -1, -5, -7, -16, -17, -13, -10, 7, -3, -39, -17, -10, 0, -21, -21, 4, 7, -14, 6, 12, -4, -11, 25, -11, -38, -15, -11, 5, 5, -18, 15, -31, -13, 3, -24, -28, 2, 5, 3, 3, -4, -5, -13, -2, -35, -17, 1, 11, 0, 30, -10, 23, -14, -1, -8, -24, -22, 35, 0, 12, -4, -16, 7, 4, 8, 9, -5, -31, -4, -20, 14, 7, 11, 0, 8, -11, 30, 10, 6, 30, 0, 19, 7, 25, -10, 36, -10, 26, 1, -10, -11, 16, 0, -12, -16, 6, 1, -6, -12, 24, -8, 15, -6, -3, 3, -16, -32, -4, 11, -19, -6, -16, -20, 0, 17, 6, -14, -4, -17, 16, -10, -16, 0, 12, 6, 9, -7, -27, 6, -9, -7, -7, -3, 0, -25, -10, 6, -1, -19, 4, -8, -14, -16, 2, -18, -3, -43, 15, -12, -29, 16, -17, -7, -6, -7, 20, 2, 15, -18, -7, -19, -4, 15, -16, 22, 1, -11, -12, 3, 12, -37, -3, 0, -10, -11, 1, 3, -2, -7, 15, 2, -1, 17, -5, -19, -14, 0, 1, 9, 11, -1, 0, 7, 4, 19, -12, 14, 6, -18, -12, -22, 21, 8, 1, -17, 6, -5, -8, -6, 19, -16, 10, -4, 10, -8, -10, 10, 2, 12, -40, -8, 8, 6, 14, -18, 4, 22, -14, 16, -4, 1, 2, -14, -30, -21, -13, 10, -11, 20, 11, -5, 2, -2, 1, -13, -7, -12, 9, -4, 9, 12, -2, -4, -6, 29, -23, -4, -3, -11, -1, -6, -4, 4, -20, 14, -31, -6, -2, -10, -20, -30, 40, 17, -22, -13, 11, -12, 15, 21, -17, 8, 15, -6, -2, -9, -38, 21, 49, 8, 3, -21, 4, 0, 23, 45, 23, -7, 15, -2, 15, -2, -6, -16, 44, 5, -28, -55, 13, -8, 12, 12, -21, 39, 34, 17, -4, 7, -12, 16, 22, -24, -11, -14, -1, -24, 9, 40, 41, -16, 26, -6, 18, 1, 13, 19, -23, -5, -4, -3, -28, -16, -21, -7, -9, -10, 9, 3, -8, -4, 27, -13, -7, -9, -7, 11, 3, 31, -13, -4, 2, -8, -17, 18, 7, 8, 4, 21, -16, 7, 9, 36, -22, -16, 12, 5, 6, 9, -19, -8, -18, 3, -10, -10, -20, -4, 5, 17, 13, 5, -18, -37, 7}
, {29, 8, 17, -17, 24, -16, 19, -21, -16, 26, 12, -21, -3, -7, -28, 9, -7, 4, 7, -10, 4, 31, 15, -30, 37, -4, 11, 28, 34, 23, 1, -18, 53, 16, 5, -10, 29, 34, -3, -12, 5, 15, 22, 4, -9, 23, 2, 47, -19, -19, 12, 1, 2, 2, -9, -6, 66, 2, 37, 7, 42, 24, 7, -30, -10, -19, -5, -5, 21, 4, 1, 5, 7, 27, 24, 7, 6, -4, -39, 12, -27, 3, 0, -22, -7, -30, -46, -9, 11, 7, 18, -9, -22, -28, 5, -4, -29, -12, 0, 29, 7, -4, -18, 31, 40, 21, 6, 17, -15, 28, -13, -18, 17, 20, -12, 0, -6, -22, -26, 9, -23, 6, 7, 4, -43, -38, 16, 28, -11, -23, -14, 24, -25, -1, -10, 44, 28, 28, 8, 8, 7, -8, 7, 1, 19, -7, 1, 17, 3, 16, -17, 26, 13, 0, -7, -13, -13, -14, 20, 22, 0, 21, 5, 20, 1, 0, 9, -28, -25, -10, 5, 12, -3, 15, 17, 7, -4, 11, -5, 13, -4, -3, 13, 37, -5, -4, -11, 7, -1, 9, 17, 6, 9, 3, -2, 3, -4, -7, 10, -34, -19, -21, 1, -4, -12, 0, -22, -1, -10, -40, -4, -1, 3, 8, -21, 5, 15, 0, -9, 8, -15, -17, -30, -49, -12, 0, -5, -18, -7, 0, 9, -40, -14, 35, 4, 12, -3, 7, 3, -7, -9, 36, 14, 8, -1, 8, 11, 16, -26, 4, 9, -1, -12, -2, 35, 12, -39, 19, 17, 19, 7, 20, -7, 22, 19, 31, 15, 18, -8, 7, -10, 4, 7, 33, 33, 1, -7, 12, -4, -4, -15, -2, 37, 10, -22, -37, 25, 0, -29, 10, 18, -12, 8, 22, 10, 16, 2, 15, -2, -2, -10, -6, 12, 1, 5, -6, 12, 8, 3, 15, 5, -21, -2, -4, 16, 15, -3, -7, 4, 1, -15, -14, -3, -7, -9, -4, -11, 21, -2, -19, -25, 7, -13, 4, -10, -21, 9, -10, -2, -7, -3, -11, 11, 37, -4, -4, 9, -4, -1, 18, 17, 26, 7, 6, 12, -19, 3, -15, -16, -9, -13, -5, -10, 4, -6, 0, -9, -13, 4, 12, -10, 8, -10, -24, 3, 11, 1, 2, -30, -19, 0, 3, 2, -9, 13, 21, 4, -9, 1, -2, -20, -15, -17, 2, -7, 7, -14, 28, -11, 5, -4, 7, 9, 4, -4, 6, 7, 4, 11, -9, 7, -5, 9, -10, -5, -24, 7, 8, 12, -13, -12, 0, 4, -23, -21, 8, -7, -32, 3, -1, -26, -1, -12, 15, 10, -4, -4, -2, 1, -16, 3, -9, 17, -14, -1, -3, 9, -30, -9, -7, 11, 26, -15, -5, 5, -36, -39, 11, -3, -16, 0, -6, -4, 5, -15, 12, -5, -22, 2, 0, 10, -26, 20, 1, -1, -10, -7, -2, -5, -26, 8, 1, 7, 1, 0, -13, 2, 21, 7, -6, 3, 37, 4, 24, 17, 20, 23, 25, 18, 48, -4, -9, 19, 20, 8, 4, 7, 4, -15, 24, 18, 16, -19, -9, 0, 3, -11, 21, 7, -6, 0, -7, 1, -19, -12, -9, 13, -18, 13, 2, 14, 27, -8, 7, 10, 8, -6, -7, 1, 11, 7, 2, 21, 3, -21, 11, 14, 18, 2, 13, 9, -10, 6, -47, -13, -2, 4, -6, -2, -13, 5, -1, -22, 3, -11, 11, -5, 15, -8, 2, 10, 7, 4, -1, 0, -7, 4, -3, -2, 7, -3, -7, 0, -20, -13, -41, -8, -49, -5, 4, -3, 5, -7, -19, -5, -17, -1, 0, -6, 15, 11, 11, 14, 15, 8, -5, -14, -3, 12, -19, -36, 30, -17, -43, -6, -2, -9, -36, -25, -30, -10, -1, -8, -10, -3, 4, -20, 9, 1, -2, 11, 2, -8, -1, -2, -12, 12, 19, -12, -21, -2, 15, 9, 7, 32, 28, 10, 45, 31, 6, -3, 59, -8, 40, 4, 16, 26, -3, 22, 37, 5, 1, 26, 12, -11, -9, 9, 33, -18, 15, 29, 31, 10, 21, 7, 18, 4, 16, -6, 2, 4, 3, -20, 10, 17, 3, 10, -1, 21, -7, 5, 2, 3, -1, -4, 9, 4, -5, 9, 13, -16, 9, -13, 17, 38, -30, -33, 2, -7, -16, -38, -19, -16, 2, -17, -2, 1, 4, -7, -3, 0, -8, -21, -7, 0, -39, -34, -24, 11, 13, -7, 5, 42, -3, -16, -5, 7, -10, -7, -11, -6, -1, 6, -5, -6, 5, 16, -9, 10, -21, -11, 12, -21, -63, -11, -2, -8, -9, 9, -46, 0, 0, -7, 5, 1, 0, -39, -24, 10, 0, -2, 2, 9, -27, 50, 14, 25, -23, -1, 22, -1, 0, 22, -3, -10, 10, -9, 14, 7, 8, 28, 25, 25, 7, -7, 17, 16, 20, 13, -14}
, {-10, -27, 3, -16, -23, -1, -32, 4, -17, -32, -38, -1, -5, -15, -22, -31, -6, -13, -28, -10, -1, -26, -32, -4, 10, 1, -24, -20, -24, -16, -6, -16, 14, -16, -16, -3, 7, -13, -6, -8, 20, -13, -12, -29, 12, -39, -3, -15, 3, 5, 4, -9, -3, 5, -24, -13, -9, -10, -5, -15, -4, 2, -21, -8, 9, -18, 22, 6, -17, -12, -5, 7, 2, 33, -22, 2, -3, 11, -21, 16, 34, -50, -3, -11, 1, -9, 1, -28, -14, 8, -21, -21, -14, -2, -10, 6, -15, 16, 2, -22, 22, 8, 43, 2, -18, -6, 18, 6, -9, 27, -15, 16, 20, 10, 4, -18, -2, -34, 12, 26, 42, 6, -10, 1, -7, -6, -4, -1, -7, 5, 8, 7, 3, 11, 5, 0, 13, -12, -40, -1, -3, 1, 10, -26, -10, 16, -14, 4, -1, -3, 4, 6, -16, -3, -29, 25, -8, -13, 22, -11, -10, -32, -24, 5, -11, -12, -21, 5, -7, -15, -17, 1, 6, -35, -12, -36, 14, -7, -8, -5, -9, -25, -32, 3, -33, -2, -16, -29, -11, 14, -4, -21, 1, -19, 5, -14, -1, 7, 17, -2, -25, -7, -3, 2, 4, -1, -1, -10, -13, -20, 14, -3, -5, 0, -16, -1, 9, -8, -27, 3, 26, 3, 3, -9, -14, -12, -7, 16, -14, 11, 8, -15, 8, -18, 6, -11, 2, 8, 30, -15, -9, 11, -19, 16, 7, 15, 25, -15, 17, 5, -8, 17, -28, -14, -3, -33, -14, 4, 1, 1, -3, 10, 20, -10, -11, -1, 9, -8, 4, 12, 15, -3, -16, 1, -5, 5, -1, 5, 17, 16, 6, -5, -19, 7, 0, -5, -8, -13, -7, 12, 0, 8, -3, 8, -15, 5, 0, -24, -18, -1, -8, 2, 5, 3, -3, 25, 15, 0, 1, 3, 8, 8, 4, 6, 7, -13, -10, -5, 17, -31, -23, 12, -11, -23, 16, 10, 31, -1, -8, -15, 3, -9, 8, -13, 8, -10, -3, -5, 6, -18, -2, 13, 16, -15, -12, -2, 21, 9, 10, -12, -11, -28, -11, -3, 3, -8, -4, 3, -16, -3, -13, -9, -2, -23, 8, -16, 7, -12, 5, -34, -34, 19, -6, -6, 3, -9, -18, -9, 4, 13, 3, 5, -42, -3, 1, 2, 9, 0, 11, 12, -4, -10, -1, -15, -14, 16, 3, 19, -6, 3, -5, -11, -4, -4, -3, -3, -15, 6, 10, -7, -8, -4, -2, -3, 15, -4, -15, 9, 12, -13, -11, 8, 15, -16, -13, -14, 6, 2, 9, 11, 1, -13, -2, -6, -4, -14, -3, -30, 5, 8, -5, 3, -23, -1, 15, -6, -15, 2, -20, 19, -9, 1, 10, -1, 24, -25, -10, 23, 3, -15, -7, 16, -16, 0, 2, 11, -4, -9, -11, -11, 9, 1, 17, 8, 0, -2, -15, -19, 1, -18, 18, -22, 3, -17, 3, -1, -12, -25, -12, 7, -7, -16, -2, 13, -14, 10, -8, -34, -5, -4, -1, -38, -7, -12, 19, 6, -36, 20, -4, -4, -16, -30, 7, 1, -19, -5, -19, -19, -27, -7, 7, -17, -6, -3, 11, -5, 28, -3, -19, -2, 4, -3, 3, -23, 0, 2, 18, 0, -21, -22, -1, 2, 6, -18, -11, 3, -6, -5, -12, -2, -13, 2, -13, -15, -6, 12, 7, 8, -13, -2, 14, -22, 7, 2, 2, -33, 4, 18, -17, -3, -21, -13, 14, -3, 4, 11, -37, 11, 5, -16, 2, 9, 11, -3, -8, 23, 21, -8, 8, 17, -1, 0, 0, -33, -2, -11, 8, -11, -27, -5, 16, 2, -17, 12, -8, -16, -18, 7, -11, -4, -6, 15, 0, -15, -12, -22, -22, 16, -11, -25, -10, -22, -23, 2, -29, -27, 6, -26, 6, -8, -5, -8, -4, 9, -29, -7, -8, -3, -3, -36, 1, 19, 16, 10, -25, -32, 0, -8, -6, 3, 7, -24, 5, -24, 4, -40, -2, 20, 16, -1, 0, -3, -3, -17, -20, -7, -2, -40, -26, -7, 0, -14, -29, 11, -4, 7, -9, 12, -15, -5, -4, 9, 10, 13, 9, 20, 12, 22, -9, 19, 19, -6, -8, 20, 17, -4, -13, -14, 16, 10, 13, -3, 19, -10, 1, -7, -24, 28, 12, -18, -9, 3, -6, -17, -7, 4, -2, -16, -9, 0, 20, -34, -17, -22, -10, 11, -13, 7, 16, 12, 4, 3, 1, 11, -16, 18, 15, -22, 25, -6, 3, -1, 17, -27, -26, 2, -12, -17, -10, 8, 0, 1, 9, -23, 30, -22, 3, 16, 34, -25, -5, 6, -19, -9, 15, -5, -22, 3, -20, -22, -6, -5, -42, -25, -14, -11, -27, 5, -50, -5, -9, -15, -10, -18, -20, -15, -21, -24, -4, -12, -2, 0, 11, 6, -15, -9, 8, -9, -6, -48}
, {-14, -13, -13, 2, -11, 3, 15, -4, 3, -47, -9, 3, 11, -21, 15, -23, -6, -8, -19, 14, -3, -5, 16, -9, 8, 1, -29, 13, 17, -8, 2, -16, -26, 5, -1, 3, -24, 21, 39, -11, -3, 1, 9, -32, -11, -9, 28, -15, -29, 10, -5, 14, -9, 12, 22, -5, -36, 0, -31, 21, 24, -10, 1, -33, -9, 26, 6, -8, -6, 7, 8, -16, -8, -2, -14, -18, 7, 21, 5, 21, -16, -9, 2, -2, 8, -3, -21, 25, 27, 2, 4, 7, 19, -17, -1, -15, 30, -18, -9, -9, -7, -20, 0, -1, 2, -4, -12, 20, 7, -22, -13, 5, -11, -3, -11, 11, -7, 20, -9, 5, 57, -1, -23, -11, 8, 11, -6, -20, 23, -12, -21, -23, 15, -11, 12, 4, -1, 21, 10, -1, -4, -9, 2, -22, -12, -12, 8, 23, -8, 32, 13, -25, -25, 14, 20, -26, 17, 27, -11, 12, 8, 5, 19, 5, 14, 21, 14, 21, 24, 0, 7, 16, -2, 5, -7, -1, 8, 6, 7, -13, 7, 2, 6, -12, 7, 7, 0, -2, -9, 3, 5, 7, 12, -5, 9, 19, -1, -6, -17, 6, 8, 10, -3, 5, -5, -14, -30, -15, -4, 10, 13, 6, -11, 2, -34, -41, 5, 4, 1, 1, 6, 34, 6, -3, 7, 3, -1, -4, 5, 0, -8, -14, 1, -12, -2, -20, 0, -30, -19, -10, 6, -4, 20, -20, 7, 19, -27, -17, -28, 4, 7, -14, 15, 21, -8, 30, -7, 7, -1, -11, 6, 17, -9, -5, -8, -30, -29, 11, -2, -9, 6, -28, -6, 18, -7, -28, -3, 26, 7, 9, -29, 1, 9, 4, 10, 19, 15, -19, -12, 21, 5, -9, 20, 4, 1, -1, 2, 6, 15, -9, -8, 7, -18, 5, 8, 16, 8, 6, 6, 11, -8, 3, 18, 9, -4, 1, -7, -15, 14, 11, 17, -2, 1, 12, 12, 14, -15, -12, -5, 10, 3, -15, -2, -14, 9, 11, 14, 16, -8, -2, -8, -17, 15, -22, 12, 3, -11, 11, -14, 2, -4, -28, -1, -5, 6, 0, 7, 0, 14, -8, -15, -14, -7, 2, 4, -12, 4, 17, -11, 9, -5, -14, -1, 10, 22, 16, 13, 0, -11, 12, -8, 16, 20, -29, 9, -25, 5, 21, 27, -28, 7, -17, -20, 1, -2, 7, 8, -2, 3, 8, -7, -14, -9, -10, -7, -3, 6, -2, 8, 8, 3, 29, -3, 21, 6, -24, 35, -1, 8, 24, -12, -9, 16, 2, 10, 10, -11, 14, 13, 1, 2, 4, 14, -6, -15, 0, -2, 1, -1, 12, -6, 8, -2, 26, -13, 33, 3, 2, -20, -25, -8, 4, 46, -13, -3, -3, 6, -29, 15, -42, 5, -3, 8, 9, -1, -43, -17, -14, -4, 19, -26, 4, -12, 5, -10, 10, 3, 17, -22, -23, -3, 19, 5, -4, -5, 30, 16, 0, 6, 6, -5, -16, 11, -19, 2, -19, -21, 11, 11, -22, -8, 30, 22, -8, -13, 3, 20, -10, -6, -9, 1, -27, -1, 7, 4, 21, -20, -4, 34, 12, 1, -16, 0, 11, 9, 6, 31, 6, 23, 14, -4, 1, 7, 30, 25, 14, 2, 4, 15, 6, 14, -1, 5, 40, 25, 16, 6, 1, -13, -13, 18, -13, 12, 5, 25, -10, 5, 8, 34, 8, 9, -4, -6, 28, 3, 19, 7, -2, 17, -3, 11, -10, -7, 0, -15, -1, 11, -21, -10, -6, -4, -11, -5, 1, 16, -8, 5, -9, 2, 6, 16, -5, -4, -23, -30, 6, -6, -2, -18, -23, 1, -2, 11, -1, -11, 1, -19, 12, 9, -25, 13, -17, 6, -4, -5, -4, -6, 10, -10, -8, -8, -14, 16, 0, -1, 5, 1, 7, -7, 3, 13, -24, 10, 10, -8, -27, 13, 6, 4, 6, -16, -6, -12, -26, 9, 7, 0, -27, -32, 8, 4, -28, 10, -21, -23, 13, -55, -24, -10, -24, 1, -2, -22, -20, 22, 12, -20, -1, -10, -52, -2, -21, -12, -12, 9, -14, 18, 12, 17, -1, 10, 43, 35, -5, 8, 5, -23, 12, -15, -17, 13, -6, -6, 10, -25, -15, 9, 12, 11, -1, -18, -28, -23, -13, 3, -8, 9, 10, 7, 12, -19, -9, 2, 12, 0, 5, 2, -5, 5, 25, -9, -12, 8, 1, 4, 3, -14, -19, 19, 10, 0, 16, -24, 3, -14, 3, 6, -13, -7, 21, -4, -9, -8, 12, 0, 12, 7, -2, -6, 0, -4, 4, 2, 9, 13, -7, -10, -5, -12, 9, 4, 10, 12, -6, -15, 2, 20, 9, -3, 0, 19, -4, 9, 14, 3, 5, -1, 24, -20, -7, 4, 13, -1, 10, 9, 24, 32, -15, -9, -12, 4, 6, 5, 12, 1, 7, 2, -15, 18, -3}
, {-20, 3, 1, 4, 0, 6, -5, 5, 22, -19, -9, 11, 2, 36, 30, -14, 4, 33, 31, 26, 4, -8, 15, 28, -1, -7, 11, -1, -36, -3, 26, 15, -25, 6, -2, 9, -2, 37, -8, -5, -23, -30, -27, -9, -5, 4, 11, -15, -7, 19, 9, 7, -2, -10, 8, 3, 3, 0, 9, -8, -34, -32, 10, 12, -25, 19, 23, -8, 4, 24, 1, -7, -23, -15, -18, -13, 5, 17, -32, 11, -15, 0, 6, 7, -5, -18, -27, 9, -14, -3, 7, 16, -32, -29, 24, -23, -9, 3, -6, -6, 7, 17, -17, -12, 4, 2, 2, -45, 8, -15, -21, 17, -12, -41, -5, -22, -11, -2, -31, -8, 26, -5, -11, 19, -41, -22, -36, -16, -26, -22, -1, 7, 1, -24, -13, -5, 6, -6, 6, -2, 6, -15, -8, 25, -6, -5, -4, -1, 0, -9, 11, -28, 22, -3, -21, 7, -7, -3, -13, -26, -23, 17, 2, 4, 0, 14, -5, 16, 17, -21, 15, 20, 6, 16, -1, 8, 20, 27, 19, 4, -2, 22, -3, 27, -5, -5, 33, -11, -5, 12, 12, 34, -7, 6, 3, -6, 8, 22, -20, -2, -4, -41, -50, -10, -4, 1, 8, -8, 8, 15, 4, -5, -6, -16, 10, 13, -18, 3, -4, 10, -17, -3, 25, 8, -25, -7, 15, 8, -32, 0, -27, -22, -9, -14, -38, -43, 6, -16, -15, -16, 21, -35, -16, -30, -11, -24, -19, -4, 0, -4, -32, -25, -21, 16, -13, -4, 9, -22, -22, -10, -1, -13, 11, -35, 9, 21, 14, -22, 6, -13, 1, 10, 3, 3, 3, -7, -5, -16, 2, -37, 0, -9, -16, -33, -22, 2, -9, 7, 17, 11, 6, 1, 3, -18, 22, 0, 7, 13, -3, 19, 8, 9, 19, 20, -10, 28, 8, 5, -3, 2, 40, 9, 13, 1, -3, 1, -4, -10, 22, 0, -5, 9, 24, -3, 6, 18, 2, 23, 24, -6, 11, 25, 16, 44, 11, 11, 16, 18, 34, 11, 1, 17, 22, 17, 15, -5, 25, 9, 9, 8, 15, 28, -21, -4, 13, 1, -20, 10, -8, 20, 19, -29, -19, -1, 7, 17, -12, 16, 20, -3, 2, 8, -12, 5, -10, 14, 23, 1, 7, -2, 2, -10, 5, 34, -31, 9, -8, -6, -43, 0, 20, -25, -27, -16, -4, -34, 2, -24, -8, -30, 9, 12, 4, -8, 1, -2, 13, -22, 13, 7, 0, -13, 4, 1, 8, -5, -6, 18, -3, -4, -8, -5, 13, -38, -35, 10, -2, -24, 9, -10, -5, -2, -14, 0, -5, 12, 8, 12, 15, -48, -17, 6, 15, -6, 14, -6, 2, -12, -4, 15, 11, -23, 9, 20, 5, 5, 11, -4, 14, 31, 6, 2, -8, 11, -13, 20, 38, 5, 4, 5, -1, 19, 0, 2, 24, 15, -19, -16, -1, 12, -2, 18, 26, 23, 7, 17, -15, 7, 24, 10, 18, 1, 20, 29, -10, 32, 43, 4, 21, -22, 6, 7, -10, -2, 18, -6, 39, 14, -19, -8, -5, 35, -16, -15, 6, 3, 8, -4, -15, 16, 19, -13, -6, -2, -3, 11, 0, 2, 2, -13, -16, -5, 1, -23, 3, 6, -1, 11, 5, -17, -39, -5, 2, 27, 2, 3, 14, 7, -6, 12, 2, -12, -20, 9, -11, -51, -3, -4, -23, 2, 8, -3, -6, -16, 8, -44, -1, 14, 8, -2, -21, 19, -46, 0, 0, 7, 13, -16, -21, -4, 4, 3, -10, -8, -19, 18, 18, -12, 5, -22, -28, 17, -21, 5, -8, -25, -9, -29, -6, 3, 24, 2, -34, 13, -28, 10, 5, -19, 18, 15, 4, -19, 9, 4, 17, -1, -6, 3, 6, 9, 2, 13, -11, 6, 15, 17, 24, -18, -4, 8, 10, -16, 16, -1, 11, 7, 7, 16, 1, 6, 14, 14, 27, 3, 16, 19, 16, 7, 15, -4, 22, 19, 6, 9, 1, 28, 21, -5, 18, 7, -1, 18, 2, -15, -8, -5, 8, 31, 17, 30, -13, 37, -12, 12, 9, 6, 21, -1, 9, 16, 14, 7, -3, -2, 2, -4, -3, 1, 26, 6, 2, 18, 8, 14, 4, -4, -16, -7, 38, -13, -7, -4, 7, -5, -13, -8, -29, 8, -13, -10, 12, 17, 4, -49, -12, -30, 8, -25, 10, -18, 18, -5, -29, 25, -1, 3, -7, -8, -24, -9, 9, -2, 15, 28, -3, -2, 1, -5, -7, -22, -13, -10, 8, -21, -12, -7, -8, -12, -5, -22, 16, -24, -7, 29, -8, 15, -8, -7, 38, 14, -20, 1, -10, -19, -25, 8, 23, -30, 1, -16, 0, 1, -13, 7, -6, -12, 5, -15, -4, 21, -4, 42, 14, -3, 3, 32, -10, 7, 8, -14, 29, 11, 13, -14, 7, 4, -9, 7, 7, 1}
, {-5, 1, 3, -1, -3, -5, 0, -6, -8, -16, -13, 2, -9, -10, 0, -13, -17, -22, -8, 6, 4, -6, 9, 9, -7, 5, 6, -20, -3, -12, -5, -4, 13, 6, 0, -16, 10, -2, 5, -6, -1, 3, 13, -8, -5, -18, 7, 5, -1, -15, 15, 5, 7, 16, -4, 1, 3, 4, 7, -17, 2, 6, 5, -6, 11, -3, 15, 3, -17, -3, -4, 1, -2, -8, -9, -22, 8, 3, -7, -17, 27, -16, -4, -1, -10, 6, -10, -10, -3, -2, -16, 9, 7, 21, -3, 13, -13, -8, -22, -25, -29, -7, -9, 4, -6, 11, -9, -7, -9, -13, -18, -9, -20, -9, -31, 5, 2, -25, -18, -9, 1, -5, -14, -30, -19, -29, -5, -13, -8, -5, -6, -21, -15, -2, -16, -5, -10, -6, 6, -1, 6, -10, -20, -17, -21, -10, -2, 4, 6, 1, -9, -6, -33, 1, -3, -17, -11, -28, -7, -9, -6, -6, 0, -3, -11, 2, -22, -7, 8, -5, -11, 9, 6, 7, 11, -2, -3, -16, -1, -7, 2, -17, 0, 4, -5, 4, 1, -12, 2, -6, -5, -5, -18, 1, 2, -13, -4, 2, 27, -5, -22, 7, 10, 1, 1, 10, -12, 15, 0, -6, 20, -6, -4, -1, -1, -22, 6, -2, -2, -14, -3, -12, -14, 10, -7, 0, -7, 6, -23, 2, -26, 2, -2, -13, 7, -10, 7, -2, -5, 10, -23, -21, -13, -3, 1, 9, -14, -2, 14, 0, -13, 20, -3, -25, -5, 4, -5, 5, -10, -14, -17, 4, -13, -5, -9, -7, -24, 6, 8, -19, -31, -19, -24, 5, -14, 2, 0, -15, -12, -11, -33, -4, -10, -22, -7, -36, -5, -16, -7, -4, -12, -27, -19, -6, -7, 0, -6, 2, -17, -10, -9, -13, -10, -4, -34, -22, -12, 1, -4, -4, -18, -7, -2, 7, 5, -9, -17, -34, -10, -9, -5, -2, -8, -9, -5, -5, -7, -1, 3, 5, -1, -8, -1, 0, 0, 4, -14, 2, -7, -9, 8, 3, 12, 2, -9, 3, 7, -11, -14, 6, 8, -3, -17, 2, -16, -7, -7, -7, -12, 2, -1, -15, -15, 3, -3, -9, -14, -1, 3, -2, -29, 9, -5, -8, -3, -10, 2, 7, -6, 6, -17, -17, 0, 4, -1, -1, 1, 1, -19, -15, -16, 2, 0, -4, 11, -8, -3, -10, -17, 3, -9, -6, 7, -5, -4, -9, -11, -4, -12, 8, -10, -20, -12, -5, -4, -8, 26, -7, 17, -12, -20, -10, -28, -7, 9, 11, -4, 10, -8, -25, 17, -18, 11, -19, 6, -1, 4, 4, 7, -3, 19, 0, -8, -18, 17, 20, -6, 37, -9, 5, -19, -23, 1, -20, 8, -11, -8, -13, -4, -3, -10, -12, -21, -11, 14, 1, -5, 8, -4, -15, 7, 7, -11, 5, -10, -23, -9, 24, -3, -21, 2, -4, -3, -2, 1, -11, -6, -9, -7, 5, 1, 6, -8, 4, -1, 13, 3, 6, -10, -12, -1, -8, 14, 7, 4, -7, -22, -4, -8, -8, 8, -8, -7, -4, -6, -12, -5, 9, 2, -6, -4, -15, 11, 3, 8, -10, -2, -9, -8, 5, 9, 7, -9, 17, 14, 7, -10, 5, -17, -8, -24, -10, -5, -4, -19, -12, 9, -18, -12, -8, 10, -4, 4, -10, 5, -9, -3, 1, -2, -19, -6, -18, -9, 1, 1, -4, -5, 2, -7, -2, 0, -15, -3, -3, -8, 6, -17, 7, 4, -9, 6, 19, 11, 11, -4, 11, 21, -10, 1, 6, -7, 13, 9, -10, -6, 5, -5, 30, 11, -4, 12, 3, -19, 11, -12, -6, -14, 13, -1, 19, -13, 20, -7, 7, -5, -5, -14, 16, -11, -16, -10, -4, -12, -15, -29, -13, -9, -9, -3, 11, -13, -8, -7, 3, 1, 15, -8, -16, -12, -19, -7, 18, 16, 1, -13, -19, 2, -6, 7, 3, -7, -8, -1, -3, -6, -12, -6, 5, -5, 5, -9, -6, -3, 2, 11, -8, -17, -9, -10, -17, 6, -12, 1, -21, -20, 5, -7, 1, -11, 4, 4, 2, -15, -17, -3, -1, 5, 4, -5, -26, 8, -10, -5, -1, 5, -14, 17, 9, -16, -7, -2, -9, -27, -4, -24, -13, -11, -6, 3, 10, 6, 14, -1, 1, 12, -9, -7, -14, -24, 9, -18, -25, 3, -1, -1, -14, -7, -14, 14, 8, -7, -14, 5, -18, -23, -20, 5, -13, 56, 18, 6, 16, -17, -1, 16, 20, 0, -1, 8, 5, 2, -17, 20, -5, 32, -8, 8, -10, 30, 6, 14, -8, -14, 19, 41, 8, -12, 39, -11, -12, -5, -17, -15, -15, 3, -8, 4, -11, -3, -8, -5, 12, -12, -6, 3, -13, 7, -1, 1, -10, 27, -12, 8, -7, -10, -15, -1, 16, 1, -5}
, {-14, 18, 23, 6, -20, 8, 21, 10, 21, -5, 10, 11, -7, 1, -4, 2, 6, 12, 9, -15, 5, 0, -5, 34, -21, -1, -8, -30, -3, -10, 21, -10, -28, -4, -3, 15, 43, 7, 8, -9, 8, 0, -4, -7, 2, 2, -10, 35, 20, -3, 10, -7, 2, 14, 11, 26, -7, -11, 14, 36, -1, -10, 6, 15, 9, -18, -12, -25, 23, -18, -1, -23, -24, -33, 16, -19, -1, -12, 4, 18, -5, 5, -12, 14, -8, 4, 7, -6, 13, 6, -11, 27, 19, 14, 9, -45, -3, -2, 17, -16, -6, 13, -17, -38, -38, -50, -9, -39, 6, -6, 13, 7, -8, 3, 11, -1, -8, -6, 20, 9, 24, -2, -14, -8, 10, 20, -5, -33, 11, 12, 22, 0, 5, 20, -3, -10, -5, -1, 4, -16, -4, 31, -4, 1, 1, 1, 17, -21, 1, -18, 26, -1, 2, -9, 3, 5, 13, 38, 5, -11, -2, -9, -2, 23, 9, 8, -3, -10, -10, -9, -3, -21, 2, -6, 8, 5, -12, 0, -14, -1, -11, 10, 5, -31, -8, -4, 15, 19, -6, 7, -8, -11, 2, 9, 10, -18, 6, 10, -11, -20, -28, -13, -3, -23, 3, 7, 0, 19, -20, -18, -2, -6, 8, 4, 5, -25, 11, -10, 12, -2, -8, 9, 2, -18, 6, 7, 7, -33, 6, -10, -6, -33, -26, -13, 6, 16, 0, -6, -30, 0, -40, -2, -8, 4, -1, -2, 18, -39, 12, 0, -11, -11, -3, -6, -22, -41, -20, -1, 15, 6, -11, 0, 24, -7, -20, 19, -7, -7, 3, 35, -1, 6, -6, -8, 14, -8, -6, 3, 11, 38, 10, -15, 12, -4, -9, -14, 11, -7, -21, -9, 14, 23, 14, 6, 21, -10, -7, 16, 22, 14, -6, 11, 1, -11, 6, 20, 16, -14, -11, -8, 21, -19, -12, 2, 1, 5, -2, 2, 6, 9, -6, -31, 3, 2, 0, -24, 2, 10, 1, -21, -35, -7, -3, -5, -4, -16, -21, 2, -21, -13, -2, -27, -11, -5, -5, -17, -28, -17, -3, -15, -9, -15, 10, -29, 7, 0, -25, 2, -2, -18, -31, -26, -14, -33, -12, -4, -12, -4, 11, -17, -35, -11, -11, -11, 7, -18, -2, -11, -9, -19, -6, -3, -16, -2, -17, 7, 3, -10, -3, 16, 15, -30, -13, -8, -11, -21, 3, 4, 14, -22, -22, -5, -6, -10, 9, 3, 16, 14, -11, -9, 0, -2, 13, 8, -10, -18, -1, 0, -1, 6, 5, 1, -13, -16, 4, -5, -10, -18, 3, 13, -13, -4, 11, -10, -12, -28, -1, -10, -11, 10, -9, -4, -20, 8, -7, -19, -28, 3, -26, -11, 16, -14, 22, -15, 13, -2, 11, 13, 7, 12, -11, 17, -10, 23, -19, 10, 13, -12, -2, 7, -8, 24, -4, -4, 2, 12, 20, -11, -6, 2, -10, -23, 9, -1, -3, 4, -6, 2, 19, -24, -13, -2, 5, -3, -7, -25, 22, 6, -6, -17, -6, -19, -5, -3, -9, 0, -19, -27, 8, -6, 1, 18, -2, -3, 21, 1, 11, 0, 3, -6, 4, 7, -23, -13, 4, 5, 12, -17, 12, -24, -9, 8, 1, 9, -11, -17, -16, -6, -5, 7, 14, 0, -14, 9, 22, 7, -12, -2, 24, -20, 12, -3, -20, -8, 1, -15, -7, -15, -23, 12, 12, -20, -3, 11, 7, -12, -24, -20, 2, -14, -14, -8, -8, 7, -3, -16, 13, 1, 8, -14, -5, 0, 3, 11, -7, 15, -4, 4, 3, -4, 21, -10, -5, -8, 5, 0, -2, -3, -5, -10, -24, -2, -11, -5, -12, 1, -4, -12, 18, 11, -5, 9, 11, 13, -35, 3, 16, 14, 14, -8, -4, 10, 27, 3, -4, 5, 17, -34, -10, -22, -5, 1, 10, 0, 24, 8, 6, -1, 6, -5, -11, 2, 6, 3, -6, -13, -9, -4, 12, 4, 1, 15, -9, 9, 5, -5, -9, 11, -1, -9, -10, 1, 0, -34, 4, -8, 17, 7, 11, -2, 0, -8, -11, 23, 0, 4, 0, 9, 7, -19, -3, -18, 4, 5, -8, 5, -13, 11, 16, -6, 13, 5, -7, 11, 16, -5, -16, 0, 8, 0, 7, -1, -1, 6, -10, 31, 19, 3, -7, 3, 26, -21, -15, -6, -12, -32, -6, -13, -4, -23, -20, 12, -11, -20, -6, 11, 6, -7, -6, 4, 23, 11, 9, -3, 2, -42, -10, -11, 2, -19, -4, -8, 5, -5, 2, 8, 15, -2, -3, 4, 10, -2, -25, 21, -1, 15, -9, -18, 6, -20, 0, -11, -1, -3, -8, -17, -1, -26, -12, -11, -7, 12, 4, -30, -34, 13, 51, 24, 24, 20, 7, 11, 33, 3, 7, 1, 21, 18, 9, -6, -8, 0, -10, 6, 5, -14, 2, 9, -21, 28}
, {-23, 27, 9, 3, 15, 16, 10, 7, 28, -18, -2, 14, 0, 20, 43, -7, -5, 20, -1, -14, 7, -3, 9, 15, -16, -2, 19, -17, -36, -34, 2, -6, -71, 2, 12, 24, -10, -2, 26, 18, 3, 5, -3, -5, 9, 22, -23, -17, 29, 18, 15, -29, 2, 6, 30, 31, -26, -14, -14, 18, -29, -17, 14, 36, -11, 10, -2, -2, -34, -17, 4, -23, -9, -11, -12, -6, -4, 14, 29, -5, 21, -2, -7, 23, -10, 16, 25, -5, 6, 4, -13, -3, -3, 0, -18, 14, 51, 30, 14, -7, 20, 18, 42, 8, -1, 13, 15, -24, 14, 9, 3, 34, -15, -8, 17, 7, 4, -5, 9, -8, 48, -2, 4, 0, -1, 55, -2, -13, 8, 18, -4, 19, 18, 8, -2, -8, -28, 15, 15, -1, 12, -2, -30, 2, 5, -5, 15, -17, 7, -5, -14, -12, 10, 7, 11, 11, -20, 8, -2, 0, 4, -14, 2, -26, 8, 1, -2, -24, -11, 10, 6, -38, 5, -13, -5, 7, -50, -8, -2, 5, -9, 0, -5, -34, 17, 6, -8, 9, -2, -4, -5, -29, 11, 7, 14, -2, 2, 2, 1, 18, 37, 10, 14, -22, 8, 24, 17, 1, 4, -33, -3, -3, -10, 15, 17, -12, -6, 3, 18, -4, 9, -16, -15, -9, 4, -16, -9, -12, 3, -3, -4, -10, 0, -10, 1, 28, 1, -11, 14, -10, -11, 10, -17, 19, -12, -2, -15, -41, 5, 10, -6, -3, 3, -11, -23, -37, -16, -10, 9, 0, 10, 16, 26, -8, -10, -13, -3, -18, 15, 0, 13, -4, 6, -47, -9, 8, 6, 8, 14, 4, 25, -1, -16, 11, 20, -16, -5, 6, 19, -8, -19, 17, -16, -11, -14, -26, 5, -5, 6, -35, 17, 11, 5, -1, 7, -32, -3, -20, -5, 8, 25, -44, -10, 9, -20, 3, 14, 8, -29, -7, -6, -10, 10, 3, -10, -18, 9, -19, -8, 3, 0, -9, 6, 11, 1, 1, -4, 10, -2, -15, -4, -2, -10, -8, 0, -4, -28, -4, 12, 16, -3, 21, 2, -9, -1, 11, 8, -6, -11, 3, 2, 1, 8, 5, -7, -23, 22, -1, -24, -33, -5, 37, 1, -12, -4, -2, 16, -7, 8, 15, 13, 6, -15, 12, -1, -13, -30, -2, 11, 9, -42, 17, 7, 5, -11, 26, 15, -1, -9, 4, -46, -24, -9, -9, -2, -21, -13, 0, -7, -7, 17, 2, -2, -10, -6, -29, 13, -30, -29, 0, -3, -13, -32, -8, -14, -9, -8, -38, 8, -27, -2, -10, -7, -33, -51, -19, -8, -5, -45, -7, 6, 0, -7, -18, -5, -2, -21, 5, 5, -32, -25, -24, -11, -48, -11, -10, -19, 2, 4, -42, 11, -20, -23, 9, -27, -21, -10, 1, -11, -5, -40, -9, 1, -1, -19, -30, -7, -18, -17, -23, -5, -3, -9, 6, -5, 14, 12, -18, -4, 4, -6, -25, 11, 0, -4, -5, 9, -20, -1, -34, -1, -5, -36, -9, 3, -6, 4, 12, 22, 0, -8, -8, 17, -8, 3, 2, 6, -12, -9, -12, -11, -1, 6, 49, 2, -4, -24, 8, 9, -12, -4, 7, 7, 13, -4, -20, 7, 8, 13, -11, 11, 8, -10, -8, 21, 5, -11, -10, -5, -11, -28, 6, -1, -28, -24, -4, -4, -3, 15, -27, -3, -8, 17, 2, 7, -14, -5, -1, -15, -6, 4, 2, -1, 10, 4, 6, 6, -13, -24, 6, -15, -18, -25, -18, 20, 17, 2, 8, 16, -29, -13, -33, 4, -16, -22, -11, -9, -17, -2, -30, 1, -5, -4, 0, -5, 0, -12, -5, -8, 2, 23, -29, -25, -9, -39, -4, 4, 22, -39, -7, -4, -8, 13, -18, -6, -1, 18, -43, -7, -27, 1, -29, -20, -14, -2, -27, 13, 0, -14, 3, 7, -17, -12, -11, 8, -35, 2, -30, 3, -8, 0, -15, 11, 3, 5, 15, -2, 2, 2, -6, 3, 0, 3, 3, -2, -9, -30, 1, 8, -4, 10, -19, 10, -16, 9, 13, 4, -13, -1, 24, 10, -9, -4, -2, 8, 22, 1, 30, -10, 6, -7, 30, -8, -2, 22, 21, 7, -2, -3, 0, 5, 3, 0, 15, -31, 1, -19, 36, 8, -13, 4, 31, 29, -14, 6, 4, -2, 1, 20, 4, 26, -12, -5, -14, -8, 13, 13, 24, -11, -4, -3, 18, -58, 2, 11, 21, 6, -22, -9, 10, -6, -28, -11, 5, 15, 8, -21, 21, 0, -10, 5, 3, 24, 52, 12, 9, -2, 0, -13, 48, -12, -10, -20, -14, -32, 13, 8, 20, -19, 6, 7, -10, -44, 30, -28, -16, -2, 9, -13, -10, 5, 16, -15, -1, -7, 25, 28, -13, 4, -32, -17, -13, -21, -9, 20, -32, -27, -20, 9, 17}
, {8, 2, 9, 4, 5, 4, 1, 41, 55, 12, 16, 48, -6, -2, 11, 23, 19, 8, 30, -19, -6, 24, -15, 21, 8, 4, 34, -22, -21, -11, 3, 56, -12, -11, 2, 22, 8, 12, -4, 13, 29, 18, 14, 12, -3, -8, -12, 0, 20, -4, 2, -17, 5, 5, -11, -15, 11, -7, 7, -6, 11, 10, -8, 20, 9, -8, 11, -1, 10, 2, -5, 32, 16, 25, -18, 24, 8, -5, -3, -9, -7, 0, -6, 10, 5, 10, 1, -15, -47, 2, 24, 6, 12, 5, 8, 6, -18, -12, 19, 4, -1, 10, -7, 9, 10, 33, 19, 7, 0, -9, 10, -14, 18, 8, 29, -11, -9, -8, 9, -9, -25, 9, 10, -6, -2, -6, 25, 25, 1, 4, 2, 9, -9, -8, -2, 2, -14, 10, -4, 42, -2, 8, -7, -7, -5, 21, 29, -10, 1, -26, -4, 29, 5, -1, -7, -8, 13, -5, 6, -6, 0, 2, 1, 8, 11, 11, 3, 28, 17, -2, 16, 21, 0, 3, 16, 21, 18, 9, 16, 19, 1, 13, -17, 26, -5, 9, 3, -5, 16, 5, 0, 42, -1, 19, 14, 1, 7, 5, 3, -13, 4, -2, 9, 1, 3, 22, 50, 25, -10, -1, 2, 26, 2, 7, 3, 12, 21, -3, 17, 16, 12, -27, -7, -24, -3, 6, -1, -23, 9, -10, -1, 6, -3, -7, -2, -2, -2, -6, -9, 4, -3, -18, -20, 19, 4, -4, 14, 8, 33, 5, -11, 11, -12, -3, 9, -12, -7, -7, 15, 22, -7, -18, 0, 2, 4, 17, 11, 15, 4, 31, 2, 9, -3, 2, -4, 7, -8, -9, 4, 3, 21, -5, -33, -11, -31, -20, 5, -5, 4, -4, 6, 2, -21, -25, -6, 12, -7, 33, 0, 17, -1, 16, 9, 6, 12, 5, 11, 20, 6, -2, 22, 25, -8, -3, 9, -7, -12, -14, 8, 16, 1, -8, 4, 4, 12, -5, 4, -10, 3, -3, 10, 4, 1, 15, -1, 18, -6, -30, -1, 0, 6, 9, -16, 10, 2, -7, 22, 3, 6, 4, -18, 8, -1, 3, 5, -8, -6, -5, -19, 18, 8, -13, 11, 10, -3, 0, 17, 0, 19, -2, 2, 5, -5, -7, -21, 11, 10, -4, -13, -5, -10, -4, 4, 28, 9, -13, 16, -14, -19, -8, -2, 25, 8, -15, -3, 7, -3, -6, -11, 2, 12, 18, 19, -2, 0, -12, 13, 2, 7, 1, -21, -21, -3, 7, 16, 10, -11, 22, 28, 5, -11, 24, 39, 14, -10, 4, 4, 23, -2, 23, -26, -11, -10, 8, 29, -18, 10, 3, 13, 1, -15, -5, 26, 20, -4, -20, 4, -4, -14, 49, 32, -20, 9, 23, 37, 5, -9, 25, -12, 4, 4, -8, -20, -8, -1, 50, 22, -3, 6, 18, 33, 5, -5, 0, 18, 16, 0, -18, 27, 3, 0, -12, -29, -3, -12, -37, -16, -21, -26, 3, 5, 16, -5, -35, -10, 11, 0, -7, -29, 12, -7, -12, 17, -19, 13, 3, -37, 1, -6, 11, 0, -4, 2, 25, -1, -50, 17, 2, 7, -9, -5, -9, -21, -1, -1, 4, -4, -5, -22, -5, 4, 31, 5, 22, -5, 14, -13, 3, 3, 7, -3, 15, 7, -42, -4, -23, 21, 0, 6, 22, 0, 28, -18, 9, 5, -18, 9, 15, -33, 32, -2, 8, 28, -40, 0, 0, 7, 19, -2, 3, -4, 17, 5, -17, 8, -5, -11, -5, -2, 12, -6, 3, -13, -9, -12, -8, -1, -20, 4, -20, -7, -4, 2, 31, -2, -29, -5, -12, 3, 9, 12, 7, -18, 21, -9, -5, 24, 4, 13, 23, 2, -24, 5, 21, 20, 4, -19, -36, -11, 2, 5, 13, -29, 19, -18, 6, -1, -44, 4, -28, -16, 12, -5, -10, -33, -2, -10, -15, 1, 3, 9, 7, -5, 9, 10, -9, -1, 4, 16, 3, -4, 17, 8, 19, 14, -12, 15, -1, -14, 4, 8, -7, 10, 40, -9, 7, 12, 23, 24, 24, 14, 0, 21, 10, 3, 1, 26, 16, 8, -18, -15, -21, -4, 5, 11, -12, 8, 3, 1, -1, -17, -5, -8, -14, -13, -10, -3, -11, -12, 13, 13, 11, 6, 7, 24, -17, -22, 10, 2, -9, 16, -5, 4, 16, 22, -2, 7, -2, -4, 1, -22, -9, -6, -36, 4, -21, -14, 0, 8, -14, -11, -10, 4, 0, 2, -17, 5, -10, -24, 14, -31, -25, -26, 28, 18, 4, -8, 47, -3, 7, 5, -19, 9, 12, -21, -8, 1, -4, 5, 25, 7, -2, -5, -20, -6, 5, 16, 31, -22, -22, -8, 4, -24, 7, -24, -8, -12, -53, -51, -24, 7, 2, -9, -24, 12, -37, -39, -11, -3, -1, -26, -4, -10, -9, -59, 2, -16, 0, -9, 2}
, {-19, 0, 26, 24, 15, 62, 20, 12, 8, -27, -11, 21, 18, 7, 3, 32, -2, -11, -6, -5, 6, -15, 8, 26, 12, -6, 8, 13, 3, -23, 2, -9, -38, 2, -9, 15, -15, 21, 9, -25, -5, -24, 5, 2, 4, -2, -7, -32, 7, 10, 16, -7, 9, 18, 29, 10, -37, 5, -3, 38, 21, -3, 4, -6, -13, 5, 21, -33, 3, 15, 0, -4, -3, -28, -10, 17, 12, 29, 3, 12, 28, -11, 16, 19, -4, 30, 6, 34, -6, -9, 32, 8, 12, -13, -14, 7, 9, -34, -17, -7, -17, -21, -15, -10, -21, -55, -21, -38, 3, -35, 10, 2, 15, -13, -32, 34, -12, 16, -5, -14, 15, 3, -21, 9, 24, 22, -2, 5, 15, 1, -21, -19, -14, -27, -2, -17, -20, 0, 6, -19, -8, 21, 22, -29, -2, -7, -19, 0, -8, -7, 20, -11, -32, -4, -4, -10, 10, 29, -20, -2, 13, -18, -8, -1, 14, 8, 6, 14, 27, -2, 8, 8, 6, 27, -26, 4, 12, -8, 1, -1, 0, 3, 22, 8, 25, -8, 4, 11, -7, -5, 0, 4, 19, 23, 8, 8, 6, -2, 19, -6, -13, 27, 20, -8, 3, 10, -12, 17, 9, -2, 3, 0, -3, 3, 4, -23, 4, 2, -1, 7, 23, 3, 8, 9, 15, -8, 4, 6, 17, 15, -3, -3, -9, -1, 8, -25, 2, -23, -4, 18, 0, -25, -1, 11, 5, 15, -10, -49, 9, -1, -1, -9, 11, 16, -23, 10, -5, -7, -3, -10, -1, -5, -1, -1, -1, -14, -18, 7, 7, 7, 3, 5, -8, 32, 28, -16, 2, 23, -16, 13, -10, 9, 10, -17, 10, 7, 14, -7, -2, -2, 4, 11, 6, 14, -7, -7, -19, 6, 22, 5, -4, -12, -18, -5, -4, 25, 2, 14, -1, 28, 0, 8, 1, -7, 17, 1, 21, -2, 21, -3, -1, -21, 7, -6, 24, 5, 12, -37, -18, -3, 5, -25, 1, 0, -33, 15, 7, -24, -27, -13, -10, -16, -39, -28, 0, -11, 1, 29, -4, -23, -19, -6, 29, -24, -18, 9, 13, -10, -16, -19, -13, 12, -15, -15, 2, -22, -20, 4, 8, -38, -28, -9, 6, -23, -33, -5, 15, 6, -11, 18, -14, 11, -28, -34, 20, 8, 1, 16, 16, -2, 13, -12, -3, 4, 12, 0, 2, 3, 11, -4, -18, -7, 4, -18, -9, 0, 33, 6, -1, -8, -15, -3, 3, 6, 4, -10, 1, 21, 17, 22, 5, 18, 10, 1, 12, -7, -20, -1, 9, 28, 2, -2, 33, 7, -6, -24, -3, -10, 11, 32, -1, 4, 7, 39, -21, 17, 17, -4, -8, 13, -13, -9, 41, 19, 31, -18, -12, -18, 0, -40, 5, 2, -4, 17, 7, -38, 6, -11, 7, 21, 9, -8, -6, -7, -3, 41, 4, 40, -29, -6, -2, -11, -14, 10, -45, -33, -27, -14, -5, -9, -21, -13, -1, -21, -1, -20, 25, 6, -16, 7, 3, -12, 16, 18, -5, 7, -18, -42, -11, 6, -2, 29, -6, 7, 6, 10, -31, 0, 1, -11, 2, -22, -10, 14, 6, -9, 24, -18, -2, 14, 1, 29, 4, 9, 35, 4, -19, -1, -10, -20, -3, -4, 10, 2, 22, 8, 11, 2, 4, -7, 17, 5, 11, 22, 9, -13, -3, 6, 11, 31, 3, 9, -4, 16, 3, 16, 7, 15, 9, 4, -3, -15, 0, 3, -5, -19, -8, -2, -17, 1, -5, -10, -4, 13, 5, -1, 25, -16, 3, 13, -1, 13, -1, -15, -6, 11, -7, -12, -11, -16, -10, -8, -3, 8, -18, -7, 1, -2, -3, -7, -14, 0, 3, -18, 4, -6, -3, 3, 6, -4, 5, -29, 12, 1, -10, -29, 6, 22, -2, -6, -6, -21, 14, 2, 7, 3, -4, 7, -27, 2, -24, 16, 1, -6, 1, -5, -11, 16, 22, -12, -4, -1, 2, 9, 25, -12, 24, -10, 21, 4, 0, 9, 8, 26, -5, 1, 18, -15, 12, 3, 1, 39, -19, 15, -2, -7, -27, 8, 17, -6, 11, 16, 24, 19, 13, 20, 2, -4, 0, 7, 18, 11, 4, 30, 12, -4, 3, -3, 9, -21, 14, -19, 6, 6, 4, 7, 19, -23, -6, 0, 16, -10, 1, 24, 11, 0, 7, 2, 10, 6, -14, -21, -21, -12, -8, 20, -1, -15, 8, 5, 5, 10, 6, 7, -16, -18, 13, -25, -3, 4, 30, -16, -6, 7, -24, 10, -14, 11, -2, -15, -6, 6, -7, -2, 6, -9, -5, -14, -20, -9, -7, -9, -8, 16, -12, 18, -19, -8, 8, 7, -17, 7, 4, -7, -35, 0, 13, 12, -22, 2, -16, -6, 6, 1, -9, 33, 25, 3, -8, -2, -9, -31, -19, 5, -10, -18, 14, 14, -2, -24}
, {-4, 0, -5, -12, -9, -5, -30, -12, -14, -23, -28, 5, 8, 7, -11, -4, -15, -9, -4, 3, 5, -7, -11, -8, -1, -8, -1, -3, -6, -24, -12, -6, -7, -3, 4, -9, -18, -20, -3, 2, -24, 24, -6, -4, -14, 8, -22, 20, -9, -15, -15, -2, -8, -2, -24, -5, 17, -2, -3, -9, 21, -10, 1, -14, 9, 11, -15, -10, -1, -24, -8, -3, 11, -11, 8, 7, -3, -24, -8, 11, -4, 22, -11, -7, -9, 16, 0, 28, 36, -6, 5, -16, 21, 8, 4, -16, 33, 3, 0, -27, -20, -20, -5, 17, 10, 0, -17, 2, -5, -12, -6, -19, -4, 4, -9, 14, -8, 10, -10, 5, -28, -1, 25, -29, 15, 18, 16, 24, 33, -20, -41, 7, -23, -16, -23, -4, -8, -4, -1, -16, 0, -24, -2, -30, 17, -12, -15, 1, 4, 11, 2, -17, 6, -8, -3, -45, -7, 40, -16, 18, -17, -9, -2, -6, -4, -12, -10, -1, -1, -16, -21, -6, -8, -14, -19, -24, -24, -9, -4, 2, 6, -15, -18, 3, -25, 0, -9, -23, -10, -20, -12, -20, -4, -16, 2, -4, -12, -13, 2, -16, -8, 3, -6, -11, -6, -13, -30, -21, -5, -12, -14, -14, 6, -21, -6, -14, -10, -6, -9, -2, -11, 7, -7, -21, 15, 12, 22, -19, -13, 21, 2, -26, -24, 0, 3, 3, -1, -4, -17, -7, -15, 24, -2, -15, 0, -28, 7, -20, 23, 7, -18, 9, -4, 19, 6, 2, -8, -26, 1, -18, 10, 7, 14, 4, -1, 25, -3, 20, 11, 2, -8, 20, -13, 0, -10, 0, -6, 16, 5, -8, 0, 1, 12, 32, 1, 8, -5, -3, -18, 4, 11, -14, 15, 11, 51, -24, -24, 10, 13, -26, 6, 9, 27, 12, -16, -3, 8, 12, 8, 39, 20, -35, 27, -12, 5, 23, 20, -21, -11, -21, -14, -11, -16, 3, -21, -19, -24, -12, -13, -21, -26, 3, -2, -7, 5, -19, 11, 3, -31, -6, -4, -16, 8, -7, 9, -6, 11, -15, -9, 0, -13, 8, -18, 29, 19, -6, 10, 17, 1, -19, -25, -2, -12, -25, 3, 6, -12, 5, -3, 25, 6, -11, 2, -10, -4, 10, -17, -3, -1, -6, -30, -3, 21, -16, -12, 14, 9, 17, 15, 0, -4, -22, -7, -10, 6, -26, 8, 1, -17, 17, -6, -2, 31, -27, -11, 22, -3, -21, 13, -1, -9, 22, -32, -13, -7, -15, 7, -11, 4, -12, -10, -14, 5, -14, -23, 1, 1, -2, 2, -13, 0, -10, -13, 10, -4, -8, 2, 12, 0, -4, 6, 10, 4, -5, 12, -1, 15, 3, 11, 1, -7, 8, -14, -17, -22, -17, -12, 21, 7, -19, 8, 11, -7, 20, -5, -3, -8, -26, 7, -6, 9, -6, 25, 4, 1, -13, 10, 22, -4, -12, 0, 19, 10, 4, 1, -7, -8, 6, 0, 3, -6, -6, -1, 24, 15, -15, 13, 20, 11, 27, 7, 18, 10, 22, -1, -9, 14, -4, 4, 15, -7, -5, 5, 23, 20, -2, -16, 22, 13, -8, -20, -41, -12, -41, -13, 11, -6, -6, 10, 4, 14, 3, 0, 11, 16, 1, 3, -6, 25, 4, 28, 14, 12, 5, -5, 3, 4, 5, -7, 2, 4, -29, -15, -43, -13, -40, 2, -1, 17, -11, -7, -13, -11, -18, -3, 1, 6, 0, 11, 7, -23, 1, 6, -10, -8, -27, -2, -19, 5, -11, 7, 1, -7, -17, -2, -6, -3, -9, 11, 3, -1, -1, -24, 3, -6, -11, -10, -14, -2, 26, -8, -2, 5, 8, 5, -12, 4, -15, -18, -10, -12, -3, -10, -9, 10, -17, -25, -16, -26, -11, -5, -13, -23, -17, -1, -26, -16, -15, 5, 1, -18, 17, -13, 4, -12, -4, 21, -18, -18, -12, -3, -6, 12, -31, 29, 17, 19, 8, 6, 4, 1, -4, 1, 21, 10, 22, 15, -4, 0, -8, 4, 6, 11, 16, 5, -8, 1, 14, -9, -5, 9, -6, 13, 1, -11, 28, 18, 2, 8, -3, -6, 1, -6, -24, -2, 2, -6, 13, 11, -28, -17, -24, -7, -5, -12, 1, 12, 8, -14, 14, -33, -10, -16, -7, 13, -27, -31, -24, -8, -8, -25, -18, -8, -20, -7, -7, -3, -4, -9, -2, -8, -2, -16, -23, -4, -17, -33, 0, 12, 3, -19, -9, -13, -10, -13, -13, -21, -20, -13, -11, -21, -31, -36, -7, -23, 0, -22, -29, 1, -24, -32, -19, 7, -26, -33, -14, -6, -3, -35, -6, -7, -5, -2, -31, -1, -20, -9, -5, -20, -22, 2, -19, -23, -13, -36, -11, 0, -15, -24, -17, -6, -21, -14, -26, -28, -1, -21, -11, -2, -8, -21, -14, -14, 1, -7, -29, 4, -33, -5, -5}
, {7, -2, -10, 7, -9, -4, 3, -10, 6, 3, -11, -5, -7, -4, -7, 7, 1, -9, -8, 7, -8, 5, 2, 2, 6, 0, 5, -8, 2, -11, 7, 3, 2, 1, -1, -11, 6, -10, -6, 1, -11, 4, -9, 6, 0, -13, -7, -12, -2, -9, -12, -8, -1, -13, -8, 0, -10, 2, 6, -1, -3, 7, -2, 8, -7, -7, -2, -1, 4, 2, 1, 3, -2, -11, -14, -2, 7, 0, -9, 3, -9, 4, -1, -8, -8, -2, -3, 5, -13, 5, 4, -4, -4, 5, -11, 0, 1, -7, -4, -14, -8, -2, -6, -10, -10, 9, -13, -8, -3, -11, -7, -10, 1, 6, -10, -8, -2, -13, -6, -8, 4, -8, -3, 1, -11, 6, -13, -9, 0, 2, 2, -13, -4, -3, -2, 5, -1, 2, -8, 0, -6, -10, 3, -10, -6, 11, 0, -7, 3, 4, -11, -3, 0, 0, 3, 8, -8, -6, 6, -12, -7, 0, -10, 1, 3, 10, -4, -5, 6, -8, 0, -4, 2, -9, 6, -9, 8, 12, 4, -9, -11, -1, 1, -10, -6, -8, -6, -12, -7, -3, -8, 14, -11, -3, 1, 0, 7, 6, 6, 8, -11, -3, 1, -5, 7, -11, 2, 4, -4, -4, 4, 8, 4, -7, 1, -4, -10, 5, -1, -11, -10, 7, -4, 5, 1, -6, -14, 2, -13, -13, -2, -11, -3, 2, -2, 6, 3, -6, 1, -2, -8, 0, -11, -2, 8, -13, -8, -4, -3, -7, -3, 0, -9, 0, 4, -11, -3, -12, -8, -8, 0, -5, -1, -13, -11, -9, -12, 0, -11, -13, 3, -7, -7, -6, 0, -4, 0, 2, 3, -15, -5, -2, -7, -11, -1, -12, 7, -4, -8, -11, 0, 0, 0, -12, 4, -12, -6, 8, 9, -6, 7, 3, 2, -1, -8, -11, -6, -5, -6, 2, 8, -5, -6, -3, -2, -12, -7, -1, 8, -14, 7, 0, 15, 9, -4, 9, 5, -5, 5, -11, 8, -5, -6, 6, 3, -9, -3, 5, 7, 5, 1, -7, 17, -1, 1, -6, -7, -9, 15, 12, 5, 4, 3, -1, 2, -1, -11, -13, -7, -5, 1, 7, -8, 0, 2, -13, -2, -5, -2, -6, -4, 5, 8, -7, -9, -7, -7, -4, -8, 5, -7, -9, 1, -9, -5, -5, -3, 5, 2, -5, -9, -9, -2, -11, -5, -7, 9, 5, -3, 1, 3, 2, -2, 7, -5, -18, 3, 4, -4, -7, -9, -1, 4, -2, 2, -12, 7, 2, -9, 4, -9, -1, -7, -7, 2, -11, 5, 2, 9, -1, 6, 2, -10, -4, 1, -8, -9, -2, -12, -7, -7, 9, -8, 4, 2, -4, 7, 0, -7, -4, -10, 6, -6, 5, -1, 2, -6, -6, 3, 8, 8, -9, -11, -1, -4, 1, 6, 1, -4, 0, -2, -9, -9, -5, 1, -6, -2, -5, 5, 4, -3, -10, 3, 1, 5, 4, 1, -7, 9, -1, 2, 5, -2, 4, -6, 11, 4, -9, 4, -4, -12, -8, 4, -5, 3, -3, -2, 8, 8, -5, 7, 3, 8, -7, -9, -4, -11, -5, 7, 4, 0, 1, 9, 5, 4, -3, -9, -5, -10, -4, -3, 2, -10, -2, -1, 0, -7, 1, 5, -2, -12, 7, 4, -1, -6, -7, 4, -11, -1, -3, -2, -8, 3, 6, -7, -2, -7, 3, -1, -2, 8, 5, 5, 8, 4, 2, -6, 4, -2, 5, -4, -6, -4, -10, 6, 9, -9, -12, 2, 4, 6, -9, 3, -2, 4, 8, 3, 10, 3, -12, -9, -6, -11, -5, -9, 0, 4, -11, -10, 2, -3, -10, 3, -3, 8, -12, 4, 3, 6, 7, -3, 4, -11, -7, 8, -3, -10, 8, -10, -3, 5, 5, -1, 6, -2, -6, 0, 1, -8, 1, 7, -2, -9, 1, 6, -6, 6, 3, -3, 2, 1, -9, -8, 4, 2, 4, -11, 0, -10, 2, 5, 2, 5, -2, -3, -5, 7, -10, -9, -5, -5, 3, 5, -8, 6, 0, -12, -13, 0, 6, 1, -7, 1, 7, -5, 3, -11, -1, -4, 5, 4, 6, -9, 10, 5, -5, -3, 2, -13, -11, -5, -10, -7, 3, 3, -12, 2, 3, -5, 1, -2, 7, -1, -11, -6, 4, -8, 1, 4, 4, -1, -10, -7, 2, -12, -3, 6, -10, -11, 7, 4, -1, 5, -8, -8, -10, 7, -8, -12, 3, -10, 2, -3, -1, -9, 6, -5, 3, 3, 0, 3, 1, 3, -11, 4, -4, -14, 0, -5, 0, 3, 0, -4, -6, 5, -4, -6, -13, -12, 6, 8, -8, -12, -9, -1, -10, 1, -3, 7, -6, -7, -1, -7, -6, 0, 0, -3, 1, 5, -1, -1, -9, -8, -6, -3, -6, -1, 7, -10, 2, 3, -3, -2, 3, 7, -4, -4, -5, 4, -2}
, {13, 7, 35, -2, 9, 1, 6, 2, -5, 17, 10, -25, -7, 26, 8, 9, 7, 2, 3, -14, -3, -15, 9, 12, 23, 11, 13, 11, 3, 9, 1, 13, 13, 8, 26, 15, 12, 8, -12, 0, 8, 17, 8, -6, 2, 16, 3, 38, 1, -2, 11, 6, 7, 7, -13, -17, 17, -5, -3, -4, 0, -1, -3, -3, 14, -18, 3, 25, -5, -11, -28, 15, 8, 49, 18, 10, 10, -33, -32, 14, -12, -15, -15, -27, -9, -3, -16, -9, 25, 3, -2, -1, 8, -4, -17, 6, 11, -21, -11, -5, 10, -9, 11, 5, 2, 3, 2, 24, 3, -14, -10, -15, 23, -7, -8, 3, 2, 35, -6, -13, -14, -7, -2, -15, 8, -2, -4, 12, 10, -22, 6, -2, -7, -9, -7, 5, 3, 1, 5, -17, -1, -6, 19, -10, 3, -8, -2, 14, -4, 16, 7, -2, 6, 1, 9, 7, 15, 22, -10, -5, -13, 10, -10, 21, -15, -8, -1, -2, -3, 6, 6, -5, -6, -13, 20, -5, 16, 15, -9, 0, 1, -14, -5, -23, -7, -9, -12, -10, 4, 1, -14, 10, -16, 1, -11, 16, -12, -1, -15, 8, -3, -19, -8, 10, 5, -2, 9, 0, 14, 0, 0, 19, 5, -10, -15, 11, -8, -7, -13, -15, -4, 1, 9, 15, -18, 6, 23, 7, 7, 3, 13, 0, -2, -4, 1, -10, 7, 19, 14, 0, 14, 18, 17, -9, -3, -13, 2, 29, -16, 7, 11, 2, 9, -12, 16, 18, 16, -12, 6, 8, 5, -7, 23, 5, 23, 4, 15, -6, 3, 11, 4, 9, 13, -27, -1, 1, -10, 5, -10, -14, 1, -12, -13, 12, 15, 18, -11, -4, 9, -4, -3, 0, 14, 1, 16, -2, 6, 12, 4, 4, -8, 17, 8, 21, -6, -17, -13, 6, 7, 11, 2, 13, 21, -10, -5, 18, 3, 17, -1, -17, 0, 10, -7, 0, 1, -4, 1, 3, 9, 20, 0, -2, -6, -16, 3, 2, -28, 16, 5, -10, 3, 14, 16, -21, -11, 0, 7, -2, -3, -9, 13, -14, -31, 28, 20, 15, 7, 25, 4, -1, 1, 5, 16, -19, -11, 15, -3, 4, -1, 22, 33, -11, -8, 19, -6, -19, -10, -4, 15, 13, -13, -13, -10, 2, -13, -11, 6, 15, 14, 5, -15, 4, 11, 2, 14, -27, -3, -1, -20, 6, 16, 5, -5, -13, -5, 9, -19, -3, 7, -8, -25, 0, -14, -17, 22, 19, 1, -8, 20, -24, -18, -2, 14, 1, 6, 16, -1, 41, 6, 24, 7, -5, -32, 17, 33, 1, -8, 5, 13, -7, -5, -12, 7, -6, -2, -8, 25, -7, -3, 26, 31, 24, -26, 21, -7, -10, -10, 20, -19, 10, -2, 1, -15, -17, -6, 8, 9, 0, 9, -16, -7, -8, 9, 10, 10, -4, 9, -39, 6, -20, -7, -10, -11, -8, 11, -15, 10, -1, -10, 8, -5, -1, 3, -7, -11, 3, -22, -9, -1, -19, -9, -7, 4, -9, 5, 1, -8, 4, 11, 8, -15, -27, -7, -12, -11, -6, 4, -6, 8, -9, -5, -1, 7, -29, -4, -9, -11, 10, -6, 6, 18, -18, 0, 2, -1, -30, 7, -2, -14, 15, 6, -4, -18, -8, -4, -18, 6, 9, -29, -12, -3, 13, 4, -2, -18, 5, -5, 10, -3, -7, -13, 7, 10, -41, -4, -21, 20, -9, -6, 6, -8, -14, -3, -12, -7, 2, 6, -5, 2, 2, -19, 5, -21, -2, -13, -9, -13, -18, 5, 15, -5, -11, 7, -11, 16, -23, -1, 9, 14, 20, 27, 3, 14, 6, 0, 4, 9, 11, 15, 5, -3, 1, -40, -9, -20, -2, -13, -15, -41, -28, -11, -5, -12, -18, 11, 6, -8, -37, 3, -19, 6, 12, 2, 1, -5, -26, 12, 21, -5, 0, -10, -23, -4, -37, 5, 13, -6, 5, -29, 5, -6, 0, -5, -2, -27, 3, -39, -11, -14, -18, -11, -6, -22, -28, -3, -3, 1, 3, -3, -40, -16, -18, -28, -18, 2, 5, 5, -9, 6, -7, -9, 21, 15, -4, -6, 5, -5, 14, -19, -6, 9, -42, -11, -6, -19, -18, 1, 0, -7, -9, -15, -9, -25, 7, -11, -1, 10, -17, -16, -4, -11, -15, -17, -7, 5, -15, -9, 5, -18, 16, -1, 17, 10, -6, -6, -17, 6, -23, 17, -3, 14, -26, 33, -28, -15, 3, -15, -5, -3, -2, -15, 16, 11, -5, 7, -8, 13, -13, -2, 8, -7, 2, 11, -12, -12, 1, 4, 15, 21, -19, -15, -2, 1, -4, 13, -9, 2, 1, -15, -9, 32, 3, 7, 1, 25, -11, 5, 11, 21, 6, 11, 22, 3, 1, 10, -13, 3, -2, -4, -7, 27, -17, 30, 5, -7, 16, 12, 3, 0, 6}
, {-19, -14, -2, 0, -31, -33, -6, -28, -14, -11, -7, -31, 13, -32, 11, -22, -15, 4, -2, -3, -6, -9, -4, 0, -6, -2, -8, -6, 0, 3, -12, -20, 6, -16, -24, -11, -13, 12, -1, 0, -3, -15, 0, -25, -5, -18, 37, -15, -37, -7, 5, 0, -8, -10, 19, -14, 6, -8, -44, -6, 13, -5, 3, -11, 9, -4, 28, 8, -35, 6, -7, -17, -3, 13, -6, -25, 4, 19, 3, 1, -18, 8, -6, -19, 7, -28, 4, 16, 25, -10, -32, -24, 10, 2, 12, 11, 22, 14, -6, -7, -11, 4, 12, -2, -20, 4, 4, 11, 12, -15, -31, 8, -11, 4, -1, -24, -10, -11, -13, 1, 43, 7, -1, 1, 2, 7, -20, -18, 1, 12, 10, -26, 2, 13, 33, -21, -6, 10, 19, -13, 11, -37, -3, 6, -4, -7, 20, -2, -9, 11, -18, -17, 13, 4, -2, -17, -6, 0, 0, -25, -12, 2, -3, -13, 13, 5, -5, -15, -12, 1, -10, -26, 5, -22, -15, -11, -4, 4, 0, -30, 7, -2, -2, -19, -8, 6, 0, -4, -1, -18, -14, -12, -9, 0, -18, -6, -3, -2, -6, 6, 1, 8, -2, -7, 1, 2, -26, -19, -1, -14, 19, -11, 8, 4, -21, -4, -20, 0, 5, -17, 14, 5, 3, -12, 1, -13, 23, 13, -7, -1, 10, 7, 24, -3, -2, 5, -4, 7, 25, -2, 12, 12, 22, -32, 4, 2, -12, 5, -8, -11, 9, -13, -10, 20, 8, 28, 16, -2, 1, 13, -15, -3, -13, -5, -11, -28, -30, -10, 0, 6, 2, -12, 9, 11, -17, -4, 8, -22, 15, -1, -25, 2, -4, 0, 12, 25, 8, 1, 2, 7, -7, -25, 14, 22, 20, -9, -17, -8, 2, -37, 5, 13, -23, 15, -15, -3, -10, -10, 3, -22, 5, 3, 31, 2, 0, 17, -24, 0, 7, -20, 3, -15, 6, 6, 11, -3, -21, -22, -9, -10, -6, -11, -1, -9, -12, 8, 16, 10, -8, 12, -6, -16, 7, -34, 13, 2, -9, -7, 1, 5, -1, -33, 16, -2, 1, -16, 16, -21, 15, -17, -2, 2, 2, 7, -4, 2, 1, 14, -15, 22, 18, 7, -2, 4, 32, 11, 6, -4, -2, 13, 25, 14, -3, -27, 0, -24, 7, -14, 22, -19, -1, -16, -3, 4, 11, 14, 6, 14, 15, 19, -10, -9, -9, 12, -4, 10, 9, 4, 1, -4, 12, 12, 11, -8, -2, -6, -1, -5, -21, -2, -5, -25, -18, -15, 13, -26, -6, 2, 1, -16, 29, -16, 7, -18, -10, 28, 8, 14, -5, -8, -12, 9, -3, 15, 16, 28, -5, -6, -11, -24, -31, -14, -5, -28, -14, 7, 23, -29, -9, -22, -3, -14, 26, -12, -4, -31, -24, 7, -11, 17, 2, -7, -11, 1, -24, -8, 12, 22, 8, 2, 8, 11, 9, -4, -8, 12, 13, -5, 14, 5, 7, -9, 8, 9, 8, -13, -14, 8, 12, -9, 4, 11, 14, -13, -24, 9, 8, -4, -1, -15, 6, -9, -14, 3, 16, 20, -3, 15, 13, 8, 12, 9, 13, -6, 7, 16, 9, 7, 11, 4, 14, -32, -1, -1, 17, 19, -2, 3, 9, 3, -34, -19, 3, 42, 1, -1, 17, -26, 4, 3, -15, -11, 14, 10, 19, 15, -3, 18, 18, -9, -22, 8, 0, 2, 0, -5, 4, -18, 19, -7, -1, 16, -11, -17, -10, 5, -4, -7, -1, 6, 2, 2, 2, 12, 4, 1, -6, 3, 6, -2, -1, 3, 8, -17, -9, -2, -2, -15, -5, -14, 7, 5, -19, -1, -1, 5, -15, -4, -14, -6, -1, -10, 18, 11, -15, 10, 5, 13, 2, 7, 10, -9, -16, -3, -17, -1, -9, -5, 6, -1, 1, 2, 8, -2, 6, -1, -1, -8, -6, -3, 10, -8, 3, -36, -15, -16, 7, 0, -12, 5, 13, -23, -4, -24, -35, 2, -9, -37, -3, -24, 3, 2, -24, -14, 16, 5, 6, -8, -6, -39, -35, -11, 2, -21, -12, -19, 4, -28, -12, 13, 12, 16, 18, -23, -14, -8, 0, 2, -2, -6, -2, -5, 3, 12, -3, -29, 0, -6, 4, -22, -8, -5, -22, -2, 1, -5, 10, 3, 11, -3, -6, 14, 15, -1, 6, 17, -5, 16, 4, 21, 1, 13, 23, 3, -3, 0, 5, 7, 11, 4, 2, -5, -2, 1, 11, 11, -28, 18, 9, 34, -15, 19, -3, 0, -4, -17, -8, -31, 4, 6, -24, -6, 12, 5, 10, -14, 5, 11, 11, 32, 4, -6, 10, -11, 29, 10, 13, 1, 7, 14, 25, -10, -4, 18, 24, -9, -7, -7, 9, -24, -10, 3, -21, 4, 14, 6, 24, -21, 5, -3, 18, -3, -3, 6, 24, -1, 19, 10, 0, 0}
, {-5, -10, 6, 1, 2, 6, 5, 4, -3, -12, -13, -4, -3, -2, -8, -9, 3, -10, -10, -1, 5, -5, -3, -8, 4, 6, -7, -4, -1, -9, -1, -2, 1, 3, -10, -9, 2, 4, -5, 3, -5, 0, -10, -3, 4, 1, 3, -9, -8, -5, 6, -3, -2, -3, 1, -10, -9, 1, -6, 8, 2, -11, 3, -5, -4, 4, -7, -13, 5, 1, -1, 7, -10, -11, -7, 9, 6, 0, 5, 2, -14, -9, 2, -2, -9, 5, -7, -4, -4, 7, -3, -8, 2, -6, -8, 4, 7, -3, -1, 2, -10, -3, -10, 1, 3, 0, -9, 8, 5, 5, -7, -2, -11, 5, -2, 0, 7, -8, -12, 2, -8, -4, -2, -5, -13, -2, 0, 2, 3, -2, 1, -9, -6, 1, -6, -7, 0, -9, -3, -7, 6, -4, 0, -2, -9, 7, 8, -8, 0, -2, 4, -10, 2, -10, -11, 0, -5, -4, -10, -1, 5, 8, -11, -8, 6, -12, 4, 5, 4, -6, 2, 5, -4, 3, -2, -11, -3, 0, -11, 4, -10, -1, -2, 6, -11, -4, 5, 0, 4, -9, -1, 9, 3, 0, -9, 5, 2, 7, 0, -9, 6, -12, -1, 7, -10, 5, 5, 4, -12, -6, 4, -6, 3, 4, 7, -12, 8, -6, -8, -1, 1, -10, -8, -2, -3, -13, 6, -5, 5, 2, 1, -3, -5, 3, -1, 11, 7, -10, -4, -7, -4, -3, -9, 6, -9, -12, -9, -2, 4, 8, -8, -7, -4, 2, -5, -10, -7, 4, -5, -8, -1, 6, -4, -12, 5, 5, -10, 5, 6, -2, 3, -4, -11, 2, -12, -4, -9, 0, -8, -9, 4, -5, -4, -2, -11, -4, 7, -8, -11, 13, 1, -1, -5, -6, -9, 0, -11, 8, -10, -1, -3, -7, 4, -8, -2, -10, -8, 8, -5, -12, -11, -4, 7, -3, 2, -3, 5, 0, 6, 4, -9, -11, -3, 9, -2, 1, 1, -8, 3, 7, 5, -1, 8, -7, 7, -5, 1, -1, -1, -3, 5, 2, 9, -5, 4, -3, -12, -5, -6, 9, 3, -10, -10, 3, -2, -5, -3, -10, 5, 1, 6, -9, 4, 9, 2, -8, 2, -3, -11, -3, 5, 7, -3, 0, -1, 4, -4, -7, -4, 3, 8, -4, -6, 4, 4, 1, -7, -3, -10, -7, -4, -1, -9, -6, -11, 12, -8, 2, 3, 0, 2, -9, -1, -11, 0, 3, -2, -9, -4, -4, 0, -6, -12, -5, 4, 6, -12, 11, 0, 8, -1, -1, -13, 7, -10, -3, -7, -7, 0, 5, -2, 2, 0, 0, -12, 6, -4, -3, 0, -4, -8, 3, -6, 5, 2, 3, -12, 1, 5, -5, -9, -5, -9, -5, -1, 2, 3, 4, 0, -2, 8, -13, -7, 7, -3, 4, -3, -11, -5, -11, 0, 5, -9, -9, 2, -5, -3, 5, -10, -4, -9, -12, 7, -8, -3, -11, -10, 0, -12, -10, 3, -9, 9, -12, -6, 5, -8, 3, -5, 1, -5, -3, -2, -1, -4, -7, 1, -1, 0, -11, 5, 5, -1, -7, -10, 0, 3, 6, 8, -7, 3, -1, 5, 2, 1, 0, 4, -10, 5, 7, -11, 3, 5, 1, -13, -4, 3, -2, 1, -6, -9, -2, -6, -8, -4, 9, 4, 1, -11, -2, -10, -3, 3, -11, 0, 18, -9, 2, -5, 2, -4, -7, 5, -5, -7, -8, -9, 3, -3, -6, -2, 4, -7, -12, 8, -5, -9, 5, 3, 7, 5, -7, -5, -10, -10, -9, 1, -2, 5, 7, -2, 0, -2, 2, -12, -9, -7, -10, -8, 5, -5, 10, -2, -8, -2, -2, -13, -9, -2, 3, -11, 1, 7, -9, -10, 0, -11, -9, 1, -7, -3, 5, -6, -10, -1, -10, 7, -12, -8, -14, 3, 8, -5, 10, -9, -5, -7, 9, -10, 5, -2, -4, -6, 6, -6, -1, -14, -10, -9, -13, -4, 0, -1, -9, 4, -1, -10, 9, -3, -2, -11, 1, 1, -6, 6, -8, 0, -10, -11, 7, -8, -9, -5, 7, 7, -5, -1, -4, -4, 1, -1, 1, -13, 15, 4, 6, -6, -2, -5, -5, 3, 7, 3, -3, 1, -2, -12, -6, 4, -11, -7, -13, -6, 8, -9, -8, -11, 6, -9, -9, -9, 6, -4, 2, -2, 7, 7, 5, -13, -5, -1, -11, -8, 5, 3, -9, -6, 7, 8, -4, -8, -4, -2, -1, -8, -7, 0, -13, -1, 4, -10, -7, -6, -11, 1, -7, -11, 0, 4, -9, 5, 5, 1, 6, -12, 2, 9, -2, 6, 7, -11, -10, -10, -13, 1, -1, 1, -5, -7, -11, -1, 2, -4, 5, -5, -3, -11, 5, 8, -10, -8, 4, 0, -2, 6, 3, 1, -4, 9, -4, -6, -7, -9, -9, 2, -2, -5, 2, -5, -10}
, {-9, -10, -2, -2, 5, -2, 13, 3, 8, 18, 8, 26, -8, 6, -6, 9, -16, -1, 4, -16, -3, -13, -14, 3, 14, -2, 1, -17, -17, -5, -2, 6, -20, 2, 19, 5, 34, 5, -5, 32, 30, 21, 11, 20, 10, 19, -16, 40, 17, 4, 18, -7, -8, -9, -24, 12, 2, 2, 25, 9, -25, -7, 2, 21, 18, -20, -29, 13, -10, -17, -39, 16, 1, 22, 4, -12, -6, -22, -4, 19, 20, 3, -14, 4, -4, -8, -7, -37, 15, -6, -4, -16, -23, 13, -9, 18, 23, -2, -11, -22, 11, 2, 0, -13, 0, 15, 23, 1, -7, 8, 16, -14, -20, 11, 17, 11, 4, -2, 7, 3, -7, -5, 16, -12, 1, 23, 14, -10, -12, -6, -5, -1, -8, -8, -22, 1, 3, 1, -4, 22, -5, -1, 11, -5, 8, -11, 8, -16, 0, -4, 6, 11, -7, 2, 17, -3, 7, 2, 20, 3, -11, -10, -10, -5, -1, -11, -17, -5, 1, 7, 10, 8, 3, -16, 14, -8, 7, -3, 5, 22, -8, -8, -5, -29, -4, 3, -1, -12, -5, 5, 10, -14, -5, -7, 2, 2, 12, -1, -2, -5, 12, 2, 9, 7, 10, 11, 22, 18, 7, -1, 11, 22, -7, 13, 28, 13, 8, 1, 2, -3, -9, -3, 3, -6, -19, -8, 10, -13, -1, -1, -6, -12, 10, -10, -7, 28, -5, -1, 28, -3, -7, 0, -19, 34, 7, 9, 16, 11, 3, 5, -11, 14, 4, -24, -2, -2, -6, 12, 16, 3, 18, 8, 20, 2, 12, 4, -4, 14, -8, 9, 3, 12, -17, -9, 11, 21, 8, -7, 6, 1, 14, 8, 4, 3, -23, -6, -4, -19, 3, -21, -10, 11, -13, 0, 4, -1, -12, 20, 1, -14, 7, 15, -4, -5, 10, -6, 7, 3, 6, 15, -13, -12, 6, 1, 4, 3, 0, -3, 2, 6, -14, 7, -5, 3, 4, -11, 14, 23, 3, 6, 3, 17, 5, 4, 0, -9, -42, 4, 4, -3, -2, -3, 6, 9, -9, -6, 16, -9, 0, -8, 2, -3, 1, 2, 10, -7, 8, 17, 20, 3, -6, 1, 13, -8, 0, -2, 1, -8, 21, 18, 5, -9, 0, -8, 0, -22, 6, -5, -12, -9, -8, 8, -8, 28, 0, 19, -5, -4, 0, 28, 3, 19, 3, -10, -2, 18, -9, 20, -5, -3, 0, 12, 21, -4, 7, -7, 0, 20, 1, 3, 8, 23, -6, -2, 12, 26, -4, -5, -10, 4, 18, 26, -38, -3, -6, -20, -18, -5, -2, 3, -25, -14, -24, -1, 1, -38, -11, -19, -18, -10, 0, 9, -3, -6, -4, -35, 8, 12, -2, 15, -1, 6, -28, 6, 6, -24, -27, 18, -21, 15, 2, -15, -39, -17, -14, 29, 9, -18, -4, -4, 6, -13, -10, 1, 11, -2, -13, -44, 17, -10, 0, -8, -10, -9, -3, -16, 1, 19, 17, 12, 3, 10, -8, 3, 23, 22, 13, -9, 0, -5, -11, -5, -7, 4, 18, 11, -14, -9, -10, 24, -4, -1, 3, 10, 7, -3, 13, 5, -6, -7, -4, 9, -4, 1, -4, 22, -20, 5, -5, 2, 5, 9, 3, 1, -19, 11, -12, -5, 12, 10, 5, -1, 9, -28, -8, -5, 3, 9, 2, 20, 2, 6, -26, -20, -8, -22, -3, -12, -10, 9, 1, 11, 16, -17, -6, 9, -7, -8, 1, -1, 6, -12, 21, -10, -3, -11, 4, 27, -12, 15, -28, -2, -16, -32, -25, -22, -23, -5, 3, -6, 8, -9, -4, -13, 9, 8, 9, 12, 15, 6, 6, 2, 17, -20, 4, 0, -7, -13, -4, -2, 12, -5, -10, -7, -21, -18, 1, -3, -27, -30, -6, 11, 11, 3, -10, 12, -11, -31, -7, -24, 4, 1, -2, 5, -10, -13, 19, 5, -10, -8, 10, 8, -2, 4, 12, 5, 7, 16, -6, 6, -2, 22, 6, 11, -8, -7, 23, 23, 5, -13, -10, -21, 10, 1, 2, -6, 12, 19, 19, 2, 15, 4, -10, -11, 9, 20, 12, 7, -1, 17, 17, -27, -18, 15, 7, 16, 4, 4, -9, 7, 5, -5, -3, -23, 14, 23, 5, -7, -19, 14, -6, -3, 19, 9, -29, 16, -1, 12, -12, 13, 11, 17, -1, -13, -25, -16, 0, 11, 7, 1, -9, 26, -21, -3, 3, -17, 8, 16, 3, 0, -19, 17, -37, -24, 23, 31, -15, -27, -26, 8, 1, -25, -6, -22, -10, 13, 14, -7, 8, -17, -14, 16, -18, -11, -23, 2, -11, -15, -35, -39, 12, 6, -43, -2, -20, -27, -34, -15, -15, -28, 1, 6, -14, -25, -48, 13, 20, 7, 0, 9, -5, 2, 2, 5, -3, -12, 1, 5, -9, -14, -14, 3, -17, -7, 6, -27, -25, -21, -20, 5}
, {-25, 15, 4, -2, -8, -4, 15, -11, 35, 7, -11, -10, 7, -2, 25, -19, 8, 9, 6, -6, -4, 20, 21, -5, 21, 5, -14, 8, 1, 19, 13, -3, 15, 9, 0, 3, 2, 7, -9, -6, 12, -20, -9, -4, 6, 4, 0, -14, 15, -6, 15, -11, 8, 10, 0, -4, -3, 1, 17, 12, 12, 6, 0, 20, -17, 27, 1, -15, 9, -3, -4, -7, -20, -23, -18, 16, 5, 0, -1, -24, 1, 6, 2, -19, -4, -12, -5, 12, -21, 1, 16, 1, 13, -23, 11, -25, -12, -7, -12, 15, -11, -19, 3, -14, 1, 13, -19, -11, 7, -9, -19, -14, -5, -7, -7, -12, -8, -17, -7, 5, 5, 5, -15, -8, -2, -22, 0, -10, -9, -10, -14, 3, -5, -3, -8, -17, -19, -21, -2, -11, -5, -22, -10, -10, -38, -1, -5, 11, 1, -5, -23, -1, 11, -3, -25, -7, -20, -10, -9, -23, -11, 10, 7, 23, 6, 5, 30, -6, 2, 14, 7, -20, -1, 6, -15, 9, 7, 34, 18, -15, 7, 12, 13, -9, 6, 1, 1, 17, 15, -8, -4, 9, 10, 4, 17, 32, -3, -1, -8, 4, 20, -12, 16, -21, 3, 25, 30, 12, 10, 4, 6, 4, -11, 12, 9, 12, 5, 3, 3, 19, 20, 8, 3, 40, 8, -8, 1, 6, -19, -5, -20, -5, -6, -19, -17, 1, -7, -11, -4, 0, 6, 5, -18, 4, -1, -4, -10, -11, 4, 0, -6, -17, -8, -2, -8, -19, -9, -23, -17, 0, -31, -10, -26, 1, -10, -21, -22, -7, 2, -16, -14, -16, -15, -39, -38, 3, -5, -13, -37, -15, -14, 4, -20, -13, -19, -1, -2, -1, -9, -8, 0, -16, -9, -12, -17, 1, -8, -13, -25, -12, -5, -29, -5, -10, -4, -29, -21, 3, -11, -5, -8, -4, -6, -7, -10, -34, -7, 2, -2, -24, -3, -1, 17, -14, -5, -5, 19, 0, -4, 20, 4, -19, -7, 18, 18, 14, -6, -3, -12, 7, -9, -32, 13, 21, 3, 3, -9, 8, 20, -5, -7, -21, 11, 1, 17, 12, 4, -2, -9, -10, -1, -7, 14, 0, -3, 8, 26, 14, 7, -14, -4, -4, 4, 0, 17, 8, 7, 2, -18, -8, -1, 14, 5, 5, -14, -23, -28, -12, -12, 13, 3, 5, 20, -3, -3, 4, -5, 1, 13, 0, 15, 33, 26, 4, 0, -1, 17, -12, 15, -8, 3, -9, 7, 24, -14, -1, 6, -28, 21, 10, -8, -8, -9, 8, -9, -29, -25, -15, -5, -5, 10, -9, 10, -14, -9, -3, -3, -3, 9, -6, -27, 9, 12, -11, 23, 23, -13, 33, -4, 0, 7, -10, -11, -24, 3, -2, -1, -4, -17, 2, 1, -34, -17, -7, -27, -2, -10, -2, 5, -4, -27, -2, -16, -9, -3, -12, -22, -33, -2, -17, -13, -17, 13, -30, -12, -26, 24, -10, -26, 15, -10, -13, -11, -16, -24, 15, -12, -18, -19, -8, 9, -5, -18, -6, -15, -2, -21, -2, 3, -19, -4, -21, -15, -3, -9, 7, 11, 15, 20, -16, 10, 18, 3, -13, 0, 11, -28, 10, -28, -12, 6, -18, 5, 13, -37, 4, -2, -5, 8, 15, -14, -6, -17, -13, -18, -10, 2, 5, 15, 32, 29, -19, -9, 37, 17, -2, 0, 21, -10, 11, -25, -10, 21, -19, -9, 10, -1, -14, -4, 8, -14, 4, -11, -12, -26, -1, 0, -4, -15, 12, 13, 22, -27, 13, 2, -24, 0, -1, -3, 10, -23, 19, -9, -13, -3, 5, 1, 6, -11, 4, 2, -8, -8, 8, 0, -20, -6, -20, 11, -4, -40, -10, 8, -6, 1, 12, 1, -29, -14, 8, 9, 12, -1, -10, 2, -6, -19, 16, -1, -17, -6, 8, 6, 1, -4, -23, -7, 1, -1, -11, -7, -18, -11, -12, -5, -8, -13, 1, -7, -15, -25, -13, 3, -6, -31, -21, -1, -27, -29, -18, -10, -12, -16, -14, -12, 7, -14, -12, 0, -11, -2, -25, 8, -7, -18, -1, -10, -13, -18, -30, -35, 10, -10, -31, 9, -22, -35, 4, -7, -25, -21, -11, -1, -15, -24, -6, 3, 2, -27, -22, -7, -11, -26, -8, -1, -10, 1, -13, -32, -15, -3, -27, -6, 0, -30, -9, 5, -22, 5, -23, 15, 7, -6, -14, 4, -9, 0, -8, -7, -14, -5, -38, 18, 0, -17, -14, -26, 4, 16, -23, -33, -1, -10, -31, -32, -34, -14, -14, -2, -20, -14, -38, 4, -9, -18, 8, -5, -15, 4, -20, -40, 3, -23, -6, 1, -2, -21, -45, -4, 2, -13, -55, 0, -13, 18, 11, 5, -24, 2, 36, 7, -18, 12, -26, 1, 13, -13, 15, -3, 14, -7, 17, -8, -5, 6, -2, -18, 2, 10, -43}
, {5, -10, -7, -5, -7, -8, -5, -3, 6, 0, -18, 8, 8, -16, -4, -13, 2, 1, -6, -6, 3, -13, -10, 2, 12, -10, -3, -5, -7, -2, -1, -7, -6, 5, -14, -10, -3, -3, -8, 6, -16, -25, -23, -11, -10, -28, -11, -19, -1, -3, -10, -7, 8, -12, -13, 8, -22, 7, -13, -15, -4, -3, 2, -3, -12, -17, 4, 6, -9, -8, -7, 2, -5, 7, 11, -10, 8, -9, -2, -14, -10, -10, -9, -8, 9, 2, -13, -2, -13, 4, 11, 13, 7, -9, -9, 4, -20, -8, -7, -8, 15, 7, -12, 15, 12, 15, 4, 11, -4, -2, -13, -6, -14, 10, 4, -5, 6, -6, -10, -2, -14, 3, 6, 6, 2, -7, -23, 3, -16, -10, 4, 9, -13, 5, -3, -10, -2, -4, 2, -5, 1, -12, -4, -12, -18, -5, -7, -17, 3, -15, -3, -6, 2, -7, -1, 4, 1, -8, -7, -9, 1, -10, -3, -2, -15, -4, 4, -2, -11, -5, -5, -7, 3, 3, -12, -2, -12, -7, -7, -4, -11, -7, -10, 7, -6, 7, 7, -11, -6, -13, -13, -4, -1, -14, -3, -6, -10, -5, -5, -17, -15, -13, 1, -3, 8, -12, -17, -13, -6, 1, -8, -4, -10, -2, -6, 4, -7, 5, -2, -6, -4, -8, -8, 2, 11, -3, -9, -2, -15, -21, 6, -13, -1, 2, -6, -12, 9, -3, 3, -8, -8, -16, -15, -1, 9, -10, -5, 1, 0, -3, -7, -6, -11, -5, -12, -13, -5, -10, 4, 2, -5, 6, 1, 13, 5, 9, 6, 3, -5, -3, -17, 6, 4, 15, 12, -1, 1, -8, -3, 2, 18, 7, 9, -3, -9, -3, 7, 12, -2, -13, -17, -10, -4, -13, -6, -18, -13, -7, -9, -6, 2, -14, -5, -5, -12, -12, -11, -14, 4, 1, -6, 3, -11, 5, -10, -1, 4, -8, -17, -5, -9, -14, 0, -10, -10, 4, -16, -4, 6, 1, -8, -5, -5, -6, -4, -11, -10, -8, -12, -1, -6, -4, 3, -9, -6, -2, -11, -11, -8, 5, -10, -9, 6, -4, 9, 6, 1, -9, 10, -13, 3, -5, -9, 5, 8, 0, 1, -4, 4, 2, -13, 0, -1, -3, -2, -8, 1, 2, -13, -12, -8, -9, -11, 5, -6, -9, 9, 4, -2, -10, -1, -1, -1, -8, 1, 13, 2, -15, 5, -14, -11, 7, -2, -12, -4, -8, 3, 8, -14, -5, -5, -7, -11, -14, -12, 3, -16, -2, -5, 13, -1, -1, -4, -12, 9, 11, -12, 9, 9, -2, -4, -1, 3, -9, -2, 4, -3, -16, -4, 10, -3, -7, -4, -1, -9, -18, -6, 2, -11, 1, -4, 4, -4, -12, 3, -7, -1, 5, -7, 7, -3, -5, -6, -2, 1, -3, -7, 9, -5, -20, -2, -5, 0, -3, -16, -14, -3, -13, -10, -7, -2, -5, -9, 5, -2, -1, -12, -5, 4, -2, 2, 8, 2, -4, -5, -5, 1, 9, -15, -2, -4, -10, -4, -3, 11, -6, 4, 2, -7, -7, 5, 10, 13, -14, -5, -9, -3, -6, 7, -12, 1, -14, -10, -5, -1, -5, -5, -11, -12, -3, -5, -4, 7, -13, -6, -10, -8, 6, -15, 2, -1, -21, -6, -2, -3, -7, 0, 0, -3, -2, -10, 7, -1, 4, 2, 3, 2, 2, -12, -4, -10, 4, 0, -7, 3, -5, -15, 9, 2, -6, -6, 6, -3, -6, -9, 1, -7, 5, 4, 4, 1, -1, -8, -14, 4, -5, -12, 7, -8, -13, -2, -3, -3, -14, 2, -13, 7, -6, -6, 1, 9, 0, -10, 12, -1, 5, -3, -12, 2, -4, 3, -13, 4, -16, -6, 0, -5, -4, -12, -5, 0, -1, -6, -12, -18, -3, -4, 5, -3, -9, -9, -1, -17, -3, 5, -18, -11, -16, -11, 2, -3, -5, -1, -12, -15, 0, -5, -10, -13, -4, -11, 4, -8, -7, -7, -2, -9, -5, -8, -8, 0, -15, -10, 2, -10, -11, -9, -12, -17, -8, 1, -3, 2, -15, -17, -8, -18, -16, 2, -16, -10, -15, -8, 2, 5, -10, -15, -2, -24, -3, -14, -7, -4, 5, -5, -6, 2, -10, 5, 2, -16, -11, -4, -11, -9, -11, -3, -11, -5, -6, -17, 7, 3, 12, -2, 9, -3, -2, -2, -10, -11, 3, -8, -5, -9, -7, -7, 6, -10, -9, 6, -15, -11, -29, -4, -18, -7, -3, -14, -3, -14, -9, 4, -11, -6, -9, -1, -6, 5, -8, 4, -12, 8, -4, -4, 4, 7, -18, -2, -7, 3, -4, -2, -10, -2, -6, -11, 2, -4, 2, -4, -17, -12, -4, -5, -9, -4, -12, 2, 2, 3, 7, -13, -1, -7, 3, 7, -9, 2, -15, 6, -10, 4, -4, -5, -3, -12, -4, 3, -9}
, {-10, -11, 21, -1, -14, -15, 6, -10, 14, 4, 9, 20, -2, -15, -13, 16, 8, 7, 5, 7, 2, -16, -5, -7, 19, -3, -25, -24, 3, 6, -6, 1, -19, -16, 2, 1, 13, -5, -2, 0, 3, -14, -2, 10, -3, 0, 20, -1, 4, -25, -17, 20, -2, 18, -3, 6, 15, -15, -3, 18, 29, 3, 0, -11, -10, -39, 4, -19, 7, -8, -25, 22, 1, -20, -9, -15, 9, 6, 11, -7, 22, -2, -18, 27, -7, 2, 1, -11, -18, 15, -2, 0, 22, 11, 1, -1, -5, -29, 11, 5, -43, -21, -17, -1, -6, -6, 2, -25, -1, -5, 16, 22, 14, -12, 6, 18, 4, -23, 12, 3, 49, 11, -26, -4, 28, 7, 5, 19, 26, -1, -17, 7, -20, -14, 4, -9, -7, -4, -8, -12, 13, -4, 5, -20, -22, 15, 8, 7, 8, -21, 25, 2, -13, -6, -1, -4, 10, 41, 1, -7, -10, 18, 21, 6, 19, 18, 2, 25, 5, -6, 17, 22, 14, 23, -10, -8, 13, 20, 15, 7, 4, 11, 3, 17, -21, -7, 8, -4, -24, -12, 15, 46, 0, 9, 20, -2, 5, 15, 13, -16, -13, 7, -3, -27, 0, -11, -31, 26, -2, 7, -4, -16, -9, 5, -5, -30, 12, 1, -4, 11, -32, 4, -2, -5, 21, 7, -9, -12, 21, 4, 12, -12, -8, -9, 4, 3, 1, -10, -38, 9, -23, -33, -7, 1, 1, 26, 9, -33, 5, 5, -8, 5, -8, -12, -21, -39, -21, -14, 7, 0, 9, 0, 16, 11, 5, 13, 6, 7, 3, 21, -6, 11, -6, 14, 15, -9, -8, 7, 1, 18, -2, 1, 2, 2, -21, -13, 10, -5, -15, 1, 10, 27, 12, 10, 1, 23, -5, -1, 16, 14, -2, 11, 10, -16, 21, 27, 15, 17, 1, 5, 13, 8, -28, 2, -5, -7, -6, -7, 4, 8, 4, -4, -4, -5, 14, 12, -3, -2, -10, 4, -17, 13, -1, 3, -27, 9, 12, -36, -31, -23, -2, -35, -50, 16, -9, 2, -9, 15, -7, -2, -12, -4, 17, -26, -17, 3, -26, 5, -19, -39, -47, -15, -13, -22, 7, 1, -29, 1, -1, -39, -64, -37, -5, -4, -20, -17, 13, 3, -20, 0, -10, 7, -22, -6, -1, 2, 2, -24, -9, 5, 25, -21, -10, -8, 0, -8, 7, -8, 3, -13, -16, 5, -11, -22, -11, -5, 20, 21, 4, -4, -21, -9, 6, 16, 12, -27, -2, 22, 23, 1, 21, 19, 23, 5, 12, 14, 4, 10, 9, 39, -15, 9, 10, 1, 16, -21, -7, 5, 6, 17, 14, -7, -2, 27, -4, -23, 5, 0, -12, 15, -9, 8, 29, 12, 24, 11, -5, 11, 0, 6, -7, 10, 3, 9, 9, -27, -11, -3, 9, -3, 3, 25, 1, -7, 2, 19, 9, -4, -5, 11, -15, -8, -6, 9, -33, -24, -32, -13, 6, -38, -19, -10, -4, -26, -6, -45, 20, -10, -18, -6, 0, -11, 9, -38, -9, 6, -11, -50, 9, -10, -14, -1, 1, -6, -11, -3, -24, -31, -4, -2, -15, 4, -17, -26, 3, -12, 19, -44, -3, -3, -11, 13, -8, 3, -1, -21, -30, 6, -11, -13, 25, 0, -4, -6, 4, -9, 5, -34, -6, -18, 44, 12, -6, 9, 23, 14, 0, -18, 1, 31, 2, 1, 3, 17, -10, -2, -1, -1, -3, 11, -6, -13, 11, 7, 13, -15, 18, 3, 6, 6, -7, 11, 3, 0, 2, 5, 7, -2, 1, -4, 13, 16, -1, 10, 3, -11, -1, -9, -1, 7, 8, 4, -2, -1, -15, 4, 22, 12, 8, -8, 3, 9, 12, 2, 9, 14, 18, -2, 18, -10, -5, -2, 2, 10, 12, -1, -4, -6, -11, -4, 1, 7, 23, 1, -2, -3, -3, 19, -7, 1, -20, 22, 11, 16, -1, 5, 0, -23, -3, -2, -3, 0, 2, -8, 2, -11, -31, -3, -6, 10, 3, 12, -24, -14, -8, -13, -4, 5, 2, -7, -17, 13, 18, 24, -10, 4, 6, 13, 37, -43, -38, 35, 17, 4, 20, -14, -31, -2, -2, -32, 1, 8, -1, 25, -30, -40, 16, -2, 10, 4, 24, 12, -34, -20, 24, 5, 16, -13, 6, -5, 18, -45, -22, 15, 28, -17, 12, -14, -10, -4, -34, -26, -8, -4, -7, -1, -14, -42, 21, -1, 23, 1, 31, 0, -34, -63, -16, -22, 12, -23, -5, -20, 1, 23, 19, 14, 10, 7, -4, 12, 14, 7, -20, -4, -5, 10, -1, -17, 10, -18, 10, 4, -5, -14, -24, -38, 3, -1, -23, 18, -1, 34, 17, 0, -12, 25, 10, -7, 0, 4, 9, 12, 16, -11, 25, 37, 10, 25, -5, 18, 16, 10, 7, -10, -10, 18, -11, -4, 15, 9}
, {-12, -4, -6, 4, -9, 4, -2, 7, -3, -7, 6, -10, -8, -1, 6, -7, 0, -1, -7, 5, 4, 5, 0, -6, 5, -5, -5, -8, -14, -11, 0, 2, -7, 0, 6, 5, 1, -11, 4, 4, -6, -3, -10, 1, 0, -4, -6, -13, 1, -10, 2, -3, -3, -9, -10, 5, -4, 8, 7, -13, -8, -2, -2, -3, -2, 7, -5, 2, 2, 3, -4, -8, -9, 6, -5, -7, 1, -10, -7, -13, -9, -9, -3, -9, -2, -4, 2, 4, 5, -1, 1, 4, -13, 6, 7, -2, -1, 7, -11, 2, -9, 7, -6, -9, -12, 5, 0, -8, 0, -8, -12, -12, -1, -10, -5, -11, -9, 5, 2, -9, -2, -10, 6, 6, 0, 6, -4, 0, 1, -2, -12, -8, -6, 1, 4, -1, 6, -10, 3, 3, -5, 3, -9, -12, 3, 1, -6, 1, 3, -12, 8, -8, -2, -9, 7, -8, -14, 8, -6, 5, -10, -7, -6, 6, 5, -8, -11, -5, 1, -4, 5, 8, 5, 8, -9, 7, -2, -6, 2, 2, 5, 0, -3, -11, -4, 1, -6, 3, -13, 0, -3, 1, -12, 1, -6, -8, -7, -1, -14, -10, 5, -5, -8, 1, -2, 5, 6, -11, 4, -3, -8, 3, 9, -12, 5, -4, -12, -7, 5, -3, 2, -10, -7, -7, 6, 7, -3, -9, -10, 9, -11, -9, 2, -6, -8, -7, 4, -13, 2, -13, 6, -10, -9, 0, 5, 1, -13, 5, -4, -5, -12, -9, -8, -7, -10, 3, -9, -12, -9, 6, 2, 8, 5, 2, 4, -9, 7, -3, -5, 1, 0, -6, 2, -10, -4, 6, 3, -12, -1, 6, 5, -3, 8, -5, -6, 1, -1, 3, -1, -9, -3, -7, 3, -12, 2, -12, -2, -5, 2, -4, 3, 0, 3, 6, -12, 7, 3, -3, -6, 5, 5, -12, -4, 2, 1, -7, -9, -9, -12, 2, 2, -10, 7, -3, 8, 8, -9, -8, -4, -4, -11, -2, -7, -5, -8, -9, 5, -8, -3, -7, -2, -2, -7, 0, 7, -1, 0, -5, 5, -9, -11, 0, -1, 5, -6, -1, 2, -10, -8, -12, -10, -10, -6, -2, 7, 2, 8, -9, -6, -5, 8, -10, 0, 6, -4, 0, -12, -9, 7, 2, 5, -13, -6, 0, 3, -1, -13, 3, 7, 6, 5, -1, -4, 6, -11, -5, 9, 7, -4, -9, -5, -8, -2, 0, -10, -12, -3, -3, -7, -2, 6, -8, 2, 3, -4, 8, 1, 6, 2, 6, -4, -4, 0, -12, -5, 4, -7, -1, 1, -8, 2, 7, -11, 8, -6, -3, -10, 2, 4, -1, -5, 8, 7, -6, -4, -15, 5, -10, 8, 1, 2, -3, 0, -8, 4, -10, 7, -7, 0, -3, -4, -1, -7, -4, -11, 5, -2, -7, -8, 2, 2, 0, 6, -4, 2, -1, -1, -6, -6, 3, -11, 8, -8, 7, -6, -6, -3, -2, -4, -8, -4, 8, 3, 3, 2, -12, -11, 8, -1, -2, 8, 0, 5, -6, -2, 1, 2, -2, -4, -6, 0, 5, 4, 7, 8, 4, -9, -7, 0, -9, -1, 5, 5, 10, 6, 3, -6, -5, -13, 8, -10, -2, 7, 6, -2, 7, -11, 3, -3, -5, 0, 4, -3, -9, -14, 5, -1, 0, 1, 1, -4, -3, -3, -8, 5, 3, 9, 7, 4, 4, 1, 7, -9, -8, 3, -3, -1, -8, -4, -11, 3, 1, 2, -9, 6, 3, -12, -8, -4, -4, 6, 0, -11, -9, -4, -2, -6, 4, 7, 4, 3, -11, -2, -5, -7, -6, 7, 3, -12, 1, -3, -3, -8, 5, -5, -13, 6, -8, 7, 1, -6, -8, 1, -2, 2, -10, 2, -4, -4, -2, -3, 0, -5, -5, -8, 7, -9, 8, 1, -9, 5, 0, -4, -4, 7, -2, -7, 3, -4, 7, -11, -11, 6, -1, 0, -1, -4, -9, 0, -1, 1, -8, 7, -8, -12, 2, 9, -5, 6, -9, 6, 2, -3, 0, -3, 3, -10, 7, 3, -11, -4, -5, -14, -5, 5, 5, -13, -5, 1, 2, -2, -13, -2, 9, -7, 6, -11, -1, -1, -9, -5, -11, 2, -13, -6, 4, 3, 8, 5, -3, -9, -1, -2, -2, -6, -8, -3, 4, -1, -11, 0, 5, -4, 0, -3, -1, 7, 1, 3, -6, -2, -1, 0, -7, 8, -3, -10, -1, 5, -10, -12, 6, -7, -12, -4, 4, -10, -9, 6, -8, 4, 2, -9, -2, -10, -4, 1, -4, 4, -6, -10, -4, 6, -8, -4, 6, -8, 7, -2, -5, -10, -8, 3, 4, -5, -1, 2, 1, -12, -10, 8, -12, 1, -10, 0, -2, -6, -5, 0, 2, -6, -5, -10, -9, 3, -8, -10, 3, -11, -2, -12, -9, 5, 2, -12, -7, -4, 4, -1, 4}
, {-34, 27, -11, -13, 4, 25, 19, 14, 2, -13, 7, 24, 9, 1, 29, -20, -24, 18, 13, 44, -8, 17, 8, 1, -14, 13, 13, 11, -1, -5, 12, 9, -28, 38, 5, -9, 22, 27, 10, -8, -45, -1, 15, -8, 14, -13, 2, 3, -8, -9, 1, 18, -6, 20, -3, 17, -34, 2, 17, 18, 0, -21, 12, -4, -31, 21, 24, 14, 29, 12, 18, 11, 3, 20, 40, -11, 10, 8, 38, 11, -12, 11, 17, -2, -8, 19, 10, 15, -17, -7, 24, 9, 10, -12, 12, 11, -14, 19, 23, 37, -4, 11, -27, -16, -22, 17, 6, -5, 12, 21, -20, 26, 8, 3, 13, -2, -1, -8, 1, -12, 23, -4, -6, -3, -8, -17, -9, 0, -19, 19, -3, -31, 11, 14, 24, -3, 8, -9, 2, 19, 0, 18, -8, 8, -25, 4, 17, -1, 7, -1, -6, -9, 1, 2, -4, -3, -11, -31, 2, -22, 39, -1, -10, -26, 8, -9, -14, 7, 7, -15, 7, 23, 13, 10, 12, 5, 23, 2, 8, 5, 5, -4, 5, 41, 21, 9, 7, -19, 18, 28, 4, 26, 24, -34, -15, 11, -1, -2, -28, -2, -9, -18, -20, 6, 6, -12, -12, -13, 10, -25, -37, 6, 3, -37, -17, 20, 8, 1, -10, 6, -15, 11, -3, 24, 0, 4, -2, -20, 5, -13, 23, 2, 15, 49, 10, 41, 10, 11, -21, 10, -17, 13, 20, 8, -3, -5, 7, -1, 20, 5, -19, -5, -15, -1, 1, -1, 2, 6, -16, 50, -8, -28, -26, 9, -8, -10, 2, 2, 12, 17, 20, 8, 15, 8, -24, 19, -6, 24, 2, 34, 33, 8, 4, -12, 0, 13, 8, 6, 20, -1, 4, -37, 9, -5, -4, 14, 9, 18, 7, 12, 9, 1, 10, -8, 17, 11, 11, -11, -5, -25, 23, 3, -7, -1, 8, -18, 19, 37, -2, 14, -12, 24, 11, 22, -8, 27, 2, 6, 21, -12, 2, 16, 9, 9, 1, -1, 19, 12, 25, 12, 4, 32, 18, 16, -31, 13, 35, -3, -1, 8, 16, 16, -12, -13, -2, -12, 1, -9, 6, 17, 13, -32, -14, 7, -9, -20, 22, -36, -1, -19, -9, 14, 8, 14, 3, -2, -42, 1, 4, -19, 8, -13, 0, 4, -25, 26, 23, -23, 4, 27, 23, -23, -32, -4, -4, -23, -6, 8, 4, -21, 7, 7, 6, 12, 1, 6, 22, 9, -24, 5, 17, 12, 19, -9, 13, -20, 7, 28, 2, -7, 19, 13, 21, -11, -6, 7, 15, -25, 0, -28, 2, 4, -21, -3, 0, 23, 5, 14, 2, 6, -2, 16, 10, 21, 1, -21, -1, -29, 12, 2, 10, -18, 27, 20, 35, 13, 10, -13, 16, 10, 8, 20, 22, 12, 8, -5, 6, 22, 9, 42, 12, 3, 2, 3, 20, 16, 29, 10, -17, 8, 7, 12, 15, 29, 9, 31, -2, -4, -2, 0, -3, -6, 1, 11, -10, 33, 35, -14, 11, -2, -4, -1, -6, 16, 17, -1, 3, 24, -9, 17, 0, 27, -1, 0, 9, -20, 24, 11, 0, -20, -16, 8, 10, -25, -2, -4, -35, -6, -12, 7, -3, -13, -8, 10, -11, 1, -15, 17, -7, 0, -22, -14, 15, -17, 6, 14, -6, -7, 18, 21, 2, -13, -17, -30, 6, -14, 14, -13, -22, 4, -5, -16, -31, 3, -5, 13, -25, -13, 3, 9, -27, 21, -10, 7, -14, -20, 23, 18, -2, -15, 29, 8, 23, -13, -20, 25, 2, -7, 5, -5, -11, 6, 2, 8, 3, 1, -8, -3, 1, -15, -9, -1, -10, 24, 6, 18, -12, -20, 13, 3, 0, -9, 42, -5, -5, 8, 9, 31, 47, 40, -9, -7, 6, 30, -10, 3, 9, 6, -6, 8, -16, 6, 13, 10, 24, 10, 4, -10, -8, 11, 20, 11, 2, 27, -11, 19, 0, 9, 10, -14, 4, 10, 6, 17, 19, -23, 18, 16, 21, 27, -2, -10, 8, 2, -10, -2, -5, -9, -16, 25, 24, 30, 17, -16, -19, 2, -4, 12, -21, -23, -24, -39, -25, -20, 7, -31, -9, -9, 4, -24, -37, -1, 0, -22, -11, -4, 3, 12, -19, -17, -20, 14, 7, -4, 10, -14, -20, -25, -8, -26, -2, 5, -6, -4, -7, 20, 14, -30, -4, -21, -17, -1, -7, 0, 9, -17, -12, 5, -22, 6, -43, -14, -17, 7, 12, -12, 15, -8, -16, -4, 3, -8, -10, -21, -4, -16, -11, 12, -5, -11, 12, -14, 15, -15, -17, 16, 7, -22, -8, -17, 7, 5, -21, -10, -11, 15, -2, 2, 17, -5, -11, -28, 1, -8, -27, 9, -1, 1, 3, 27, 9, 2, -12, 2, -12, 10, -3, 20, 0, -12, -3, 13, -13, 5, 10, -24, -1, -12, 11, 16}
, {9, -9, -11, -5, 4, 5, 4, -5, 2, -6, -15, 16, 0, -2, 0, 4, 9, -13, -3, 19, 0, 5, -16, -22, 16, 3, -2, 29, 7, 12, -20, 6, 12, -32, -24, -7, -5, -28, 11, -12, 8, 21, 7, -7, -12, -30, 18, -17, 0, -14, 9, 11, 5, 16, -16, -36, -1, 5, -23, -6, 16, 7, -14, -2, 2, 18, 17, 14, -25, -22, 12, -10, 2, -2, 5, -14, 7, -1, 16, -31, -23, 9, 6, 7, 10, 19, 10, 1, -18, 14, -9, -18, -7, 4, 7, -6, -9, 21, 18, -3, -23, 12, 18, -1, -19, 17, -5, 38, -4, 11, -25, 15, -6, 39, 20, -4, -4, 7, -3, 41, -5, -3, 25, 3, 1, -7, 31, -20, 14, 10, 7, 5, 19, 15, 23, -23, -13, 16, -15, 6, 6, 6, -7, 37, 25, 25, 25, -6, 9, 18, 8, 13, 19, 3, -8, 13, 9, -10, 15, 8, 14, 8, -14, 23, 5, -16, -4, 28, -3, -25, -7, 2, 6, 0, 2, -2, 10, -8, -3, -2, -1, 5, 2, 13, -15, 14, 2, -11, 5, -6, -7, 10, 25, -9, -21, -4, -18, -20, 8, -11, 1, 8, 4, 15, -4, -2, 9, -15, -35, -24, -14, 16, 8, -15, -19, -9, -10, 3, -18, -8, 7, 2, -3, -24, -16, 10, 9, 0, 15, 15, 14, 6, 11, 5, 12, -21, -2, -10, 32, -5, 6, 13, 23, -16, -6, -1, -11, 1, 7, -3, 4, 0, -25, 0, 10, 20, -2, -6, 5, 13, 2, 25, 0, -9, -7, -11, -4, -41, 14, 29, -4, -4, 11, 19, -7, -34, -8, 8, 11, -3, -13, 1, -12, 30, -4, 14, 6, -6, -14, 39, 18, -8, -4, 6, 15, -28, -25, -21, 0, -55, -2, -5, -20, 10, -4, 4, -8, 3, -8, 12, -7, 9, 22, 1, 13, 20, -1, -21, -12, -5, 5, -9, 16, 9, -15, 6, -13, 11, -1, -6, -19, -5, 12, 29, 17, 2, 18, -3, 3, 30, 1, -10, 9, -6, 8, 2, -9, -4, 2, 15, 2, -16, 0, 4, 7, -20, -22, -33, -14, 9, -3, -2, -27, 19, -7, -6, 9, -12, -30, 7, 16, 14, 2, -3, 23, 27, -10, 11, -9, -5, 20, 0, 20, -32, -2, -12, 3, 14, 9, -22, -4, -8, 12, 21, 21, 0, -13, 16, 27, 17, 2, -5, 22, 4, -6, 14, 25, 4, 10, 7, 18, 1, 10, 0, 19, -4, 26, -1, -1, 41, -18, -5, -18, -1, -11, -14, 1, -3, 10, -13, 11, -4, 27, -1, -20, 25, 9, 11, -8, 21, 3, 12, 2, 6, 17, 46, -6, -3, 27, -30, -20, -8, 2, -25, -20, 10, 6, -17, 11, -39, -7, -23, 27, -6, 7, -28, -40, 3, -5, 21, -12, -8, 6, -5, -24, -6, 17, 52, -27, 9, 9, 8, 8, 0, -2, 22, 18, -12, -27, 12, 12, -4, 13, 0, -3, -15, -31, 7, 4, -2, 0, 38, 4, -1, -24, 5, 15, 3, -15, -26, 22, -21, -25, 16, 13, 5, -15, 9, 15, 17, 6, -9, 0, -6, -17, 15, 1, 0, 3, 25, -2, -25, -3, 13, 18, 22, -2, 3, 19, -15, -22, -21, 12, 23, -13, 17, 7, -9, -1, 2, 9, 8, 30, 31, 34, 32, 2, 25, 24, 2, -5, 5, 8, -8, -4, 1, 12, 5, 16, 1, 14, 13, 1, -2, 6, 19, -10, -16, -3, 9, 5, -13, -21, -1, 0, 1, -14, 0, -2, -11, -4, -9, -2, 5, -22, 33, -1, 10, 6, 2, 1, 3, -15, -8, 31, 0, -17, 2, -7, -3, 20, -21, 9, 5, 32, 2, -4, 3, -5, 21, 6, -2, -9, -9, -12, -3, -17, 37, 4, 1, -11, -20, -35, 1, 5, -28, 1, -5, -14, 1, 3, -20, 9, -10, -11, -20, -9, 6, -10, -9, 7, -11, 1, -2, -23, 21, -4, -18, -22, -29, -9, -5, -14, 1, 7, 11, -1, 2, 3, -14, 0, -12, 17, -8, -13, -14, 19, -8, 1, 5, -2, 3, 7, -20, -17, -4, -23, 3, -23, 1, -2, -22, 0, -12, -20, -13, 8, 5, 6, 5, -1, -10, -14, -13, 4, -20, -11, 9, 12, 4, -28, -1, -8, 29, 11, 35, 3, 19, -15, 3, -26, 11, 15, -28, 7, -30, -4, 20, 6, 10, -14, 8, -27, -9, 9, -8, -11, 20, -22, 21, -14, 19, -11, 23, 3, -7, -32, -12, -1, 6, -9, -17, 24, 7, -5, -24, 4, -11, -25, 50, -2, -7, 10, 2, -8, -5, 2, 26, 12, 17, -8, -30, -1, 18, -1, -14, -42, -22, -33, -11, 2, -26, -50, -3, -10, 1, -20, -29, 11, -11, -33, 6, -14, 0, 26, 28, 6, -14, -15, 3}
, {7, 29, 11, 20, 24, 27, 15, 22, 40, -2, 23, 58, -1, 23, -2, 17, 18, 3, 4, -7, 8, 9, -8, 28, 8, 3, 19, -5, -25, -11, -10, 2, -18, -14, 6, 5, 13, -4, 1, 9, 16, -19, -7, 21, 1, 6, -25, 7, 32, 13, 10, -7, 7, 25, -12, -13, 21, -15, 14, -5, 5, -1, -12, 12, 6, -26, -7, 31, -26, -22, 9, -8, -9, -25, -4, -15, -4, 7, -3, -4, 16, -29, -12, 13, -8, 3, 12, -20, 7, 1, -1, -13, -6, 6, -5, 9, -11, 22, 9, -20, -32, 0, 10, -23, -6, 3, -4, 4, 11, 11, 3, 3, -26, 17, 20, -2, -4, -15, 23, 13, -1, -5, 9, -10, 18, 12, 22, -14, 17, 31, 19, 5, 8, 12, 25, -13, 3, 0, -3, 6, -8, 19, -7, 3, 3, 18, 4, -8, 4, -7, 11, 11, -1, 5, 3, 20, 3, -12, -4, -2, 7, -3, 1, -1, 6, -7, 9, 33, 12, 3, 2, -1, -3, -3, 8, 9, -3, -22, -13, 7, -9, -1, 10, -15, 3, -3, 4, -1, 13, -9, -16, 3, 28, 0, 0, 17, 6, 4, 3, -10, 1, 3, 9, 6, 4, 13, 21, 19, 4, -16, 5, 19, 7, 5, 14, -24, 23, -3, 18, 8, 0, 8, -9, -5, -1, -14, -27, -5, -1, -13, -5, -4, -4, 7, 6, -5, -5, -14, 44, 8, -4, -9, -26, 15, -1, 17, 17, 1, 18, 4, -13, 9, -5, 11, -9, -25, -2, 15, 21, 2, -4, -10, 5, 2, -3, 29, 16, -6, 9, 9, 3, 3, -7, 23, 12, -13, 3, -4, 19, 16, -4, 9, 5, 3, 8, -3, 7, 4, -12, 32, 18, 1, -11, 6, 11, 3, -3, 19, -4, 11, -8, 12, -25, -8, -1, 32, 19, -12, -7, 4, -2, 7, -5, 9, 23, 11, 6, -8, 20, 7, 3, 1, 9, 11, -11, -15, 8, 11, -3, -6, 3, -3, 13, 13, 9, 23, 1, -18, -12, -7, 4, -6, -12, -8, 12, -4, 11, -5, 4, 8, -9, -2, 24, -11, -18, 0, -12, -8, -19, -18, -9, -15, -17, 8, -5, -27, 4, -2, -15, -6, -7, 24, -8, -11, 11, 0, -5, 14, -19, -16, -1, 14, 8, -10, 0, 10, 1, -31, -22, 30, 32, 2, -5, -2, 10, -6, 2, -1, 15, -6, -25, 10, 9, -8, -10, 0, 18, -10, -20, 5, 20, -7, 35, 10, -5, -24, -2, -1, -14, 17, 13, 18, 4, 8, 6, 3, 0, -7, -8, 8, 6, 14, 8, 13, -13, 13, 11, 20, -10, 24, 12, -13, -4, 33, -5, 0, -1, 1, 18, -24, -9, -18, 23, -10, -20, 22, 9, -14, 23, -6, 1, 16, 6, 24, 5, -33, -28, -1, -1, 6, -5, 14, 7, 4, -14, -1, 10, 29, -8, 16, 16, 17, -5, 7, -4, -10, 15, -6, -11, 0, 1, -13, 2, -20, 1, -15, 7, -10, 5, -10, -1, 22, -4, 12, 2, 5, 2, -8, -5, 8, -3, 21, 1, 2, 15, 4, -11, -7, -27, 1, 5, -11, -9, -36, 4, 1, 7, -12, 1, -13, -13, -53, 9, -11, -3, 9, -22, -3, -7, -17, -11, -10, 8, 24, -8, 10, -7, -6, 14, 34, -10, 2, -4, 32, 21, -27, 3, 19, -23, 32, -3, -4, 13, -33, -8, -17, -33, -6, 2, 1, -3, 21, -12, -9, -8, -7, 18, 8, 0, -2, -11, -24, -32, 14, 6, 19, 24, 14, -6, -27, -7, -1, 13, 6, 11, -10, 4, 2, -2, -10, -4, -8, -4, -6, -3, 9, 11, 1, -5, 11, -2, -19, 42, 13, 9, 8, 22, 13, 25, 27, -1, -13, 8, 4, -13, 1, 20, 21, 10, -3, -13, -26, -27, -5, 21, -35, -9, 3, -13, 15, 23, 8, 16, 2, -6, -27, 17, -17, -12, -7, -5, -18, 0, -23, 12, -2, 32, 4, 1, -13, 10, 16, 3, -8, 17, -7, 1, -4, 26, 8, -2, 26, 7, -15, -21, -13, 7, -1, -14, -47, -23, -36, -11, -5, -12, -9, -12, 4, -14, -14, -26, 10, -2, 7, -6, -34, 29, 5, -7, 9, 7, -18, -3, -16, -10, -13, -22, -30, -20, -15, 28, -34, -6, 39, 19, -18, -5, -36, 6, -12, -45, -33, -11, -13, -3, -19, -9, -7, -9, -15, -31, -6, -38, 4, -25, -41, -1, -6, -13, -20, 2, -17, -13, 12, 1, 13, -21, 24, -10, 20, -4, 6, -2, 18, -5, -8, 1, -23, -4, 22, 2, 5, -12, 4, -21, 8, 24, 22, -5, 20, -7, 5, -15, 25, -21, -36, -52, -1, -15, -44, -17, -3, -20, -9, -5, -1, 0, -17, -4, -3, -14, -34, -17, -5, -7, 16, -2, -24, -17, -18}
, {-19, 22, 13, -9, 21, 10, -12, 29, 8, -18, 4, 37, 4, 37, -13, 22, 1, -8, 24, 4, 4, 16, -28, 31, -26, 10, 33, -5, -21, -6, 16, 33, -11, -24, -1, 3, 8, -1, -22, 9, -6, 11, 1, -12, -16, 0, -10, -6, -10, -14, -11, -8, 0, -3, -20, 3, -15, 11, 4, -2, -5, -23, -14, -15, -9, -16, 9, 28, 13, 5, 5, 15, 21, -1, 2, -2, 0, -6, 9, -12, -4, 6, 16, -14, 8, -19, -16, 2, -10, 11, -4, -19, 17, -11, -1, 10, -6, -17, 15, 12, -16, 9, -37, -2, 3, -9, -31, -4, -5, 16, -1, -34, 1, -10, 19, -26, 6, -28, 5, 13, -7, 13, -28, 3, 9, 1, -5, 7, -2, -11, -37, -26, -23, -11, -19, -13, -17, 8, -22, 6, 7, -13, -35, -11, -25, -10, -7, -5, 2, -27, -26, -3, 1, -2, -37, -28, -18, -3, 4, -9, 3, -9, 3, 10, 18, 0, -17, 23, 4, -12, 1, 31, -8, -10, -8, 10, 17, -17, -14, -2, -6, 11, -13, -7, 0, -2, 16, -15, -7, 10, -15, 4, 17, 1, -20, -18, -30, -30, -26, 5, 8, -5, -17, -4, -12, -19, 5, -32, -5, 1, -11, 12, 8, -6, -5, -1, -2, 4, -13, -24, 13, 23, 0, -17, -7, 10, -19, -13, -25, 3, 0, 11, 8, 2, -4, 22, 1, -5, -10, -1, 7, -1, 8, 5, 9, -11, 5, 15, -8, -5, -12, -24, -5, -4, 16, 10, -16, -12, 2, 5, -22, 7, 21, -2, -7, -6, 4, -6, -14, -16, 22, 1, 22, -15, 0, -3, 2, -5, 15, -16, 3, 15, -13, -23, 8, 6, 4, 13, -12, -19, -23, -11, -34, -9, 1, -12, -15, -30, -24, -6, 3, -21, 4, -31, 6, 14, -6, 23, 1, -3, 6, -4, -17, 3, -18, -8, 0, 1, 0, -24, 17, 20, 3, -18, 12, 18, 7, -14, -15, -5, 8, -23, -4, -10, 8, 9, -3, -10, 2, 24, -6, 25, 7, -11, 2, -4, 1, 17, 1, 3, -27, -10, -15, 16, 1, -9, 2, 3, 11, 12, -3, -7, 4, 7, 0, -7, 4, 0, -6, 3, 7, 29, 5, 26, 17, 22, -18, 0, 4, -3, 6, -8, 6, -4, -8, 2, 35, 1, -4, -6, -4, 10, -8, -25, -7, -5, -10, 3, 8, 0, 2, -17, 4, 12, 0, 21, -15, -1, -21, -2, -8, 2, 6, -8, 3, 8, 7, 10, 10, 4, 1, 9, 19, -7, 5, 22, 11, -14, 0, 3, -10, 12, 9, -8, -5, -8, -5, 9, -2, -11, 14, 7, -4, 13, -10, 1, -9, -9, -1, 24, 15, 7, 9, 15, 27, -12, 3, -1, 15, -18, -10, -4, 31, -24, -4, 13, 30, 19, -10, 27, 10, -6, -26, 9, 26, 15, 27, 1, -38, -12, 18, -3, 5, -20, 6, 14, 1, -24, -26, -9, 6, -14, 6, 1, -21, -3, -22, -26, -13, -15, -9, 17, -10, -2, 1, 14, -13, 20, -7, -8, -27, -11, 21, 15, -5, -5, 6, 7, 10, -13, -17, 16, 16, 0, -4, 11, -5, 18, -17, 4, 0, -10, -9, 5, -16, -15, 16, -3, 14, 8, 5, -10, -4, -34, 9, -9, -34, -19, 21, -8, 4, 0, -12, 0, 9, 7, -12, -21, -6, 13, -15, -19, -3, -8, -7, 14, -31, -29, 22, 16, -9, 16, 4, -27, -27, -14, -20, -4, -21, -11, 7, -3, 5, 0, -6, 8, -9, -18, -2, -22, -6, 5, -16, -24, -22, 1, 0, 9, -31, 1, -8, 1, -35, 3, -8, -9, -29, -12, 20, 6, -11, 4, 20, 2, 22, -14, -16, 33, 31, -13, 0, 10, -5, 26, -17, 5, 3, -9, -6, 11, -8, -18, 24, -3, 1, 15, -1, 2, -12, -13, -15, -18, 2, -18, -17, -4, -9, -12, -37, -22, -24, -15, 3, -11, -25, -2, -23, -24, -20, -6, 5, -34, -18, 6, -6, 13, -34, 0, -16, -33, -5, -10, 16, -21, -47, -29, -24, -12, -20, -11, -29, -13, -11, 1, -7, -21, -26, -8, -27, -22, -39, 6, 5, -12, -13, -18, 3, -4, -33, -13, 0, -1, -7, -28, 9, 5, -21, -36, -19, -15, -13, -16, 9, 12, -12, 4, -13, -24, -4, -7, -37, -6, 1, 14, 7, -6, -32, 5, -14, 4, -7, -25, -9, -1, -1, -40, -2, -10, -34, -19, -10, -15, -24, -1, 10, -7, -10, 3, 1, -29, 1, -34, -9, -10, -34, 6, 4, 3, -19, -17, -8, 2, -9, -24, -23, -15, -9, -5, 4, -10, -35, -22, 1, -11, 1, -2, 0, 19, 11, -20, -12, -8, -5, -4, -23, -16, -14, -5, -5, -9, -12, 8, -4, 12, -21, -10, -6, -9, 5, -14}
, {-12, 12, 4, -4, -6, 8, 4, 2, 14, -3, 12, -7, 3, -2, 16, -12, 11, -8, -8, 0, -2, 12, 6, 9, 10, -9, 6, -18, 29, 5, 9, 32, -16, 25, -2, -8, -10, 6, 23, -9, 13, 12, -6, -6, 19, 6, 4, -15, 3, 32, 20, -17, 4, 11, 19, -9, -28, 4, 10, -2, 43, 7, 10, 2, 2, 35, -4, -2, 4, 15, -10, 26, 12, 7, -9, 17, -4, 12, 8, -13, -21, 13, 26, 5, 4, 14, -14, 27, -17, -6, 36, 17, 10, -12, 2, 15, 23, -27, -21, 13, -1, -11, -1, 4, 11, 30, 3, -3, -2, -39, 15, -17, 19, 4, -2, 16, 1, 27, -16, -5, -17, 1, -4, -10, -3, 2, -11, 23, 6, 0, -8, -11, -16, -12, -12, 12, 25, 16, 7, 20, 2, -11, 27, -47, -11, -11, 17, 25, -6, 18, -2, -3, -43, -6, 21, -23, 19, 13, 4, 13, 21, -4, -1, 1, -1, -12, 6, -6, -4, 16, 18, -8, 12, -3, -10, 28, 4, 13, 0, 7, -2, 6, 9, 11, 18, 3, -12, 8, 0, -6, 4, 5, 10, 14, 3, 4, 18, 6, 7, 7, 13, 11, 20, 21, 3, 12, 24, 6, 8, 12, 13, 23, 2, 14, -15, 22, 8, -3, 10, 7, 9, 4, -3, 16, 7, 7, 19, 19, 18, 16, 1, 29, 14, -4, -3, -25, 7, -10, 11, -5, 5, -12, -7, 23, -5, 0, 3, 13, 10, -7, 23, 1, 19, 26, 14, 32, 8, 8, 12, -4, 13, -10, -11, -7, 10, -1, -5, 21, -1, 5, 9, 6, 9, 16, 15, 14, -7, 4, 1, -5, 1, -3, -12, -5, -7, 15, 5, -4, 1, 3, 9, -3, 7, 20, 12, -6, -4, 45, 12, -6, 2, 16, -1, 5, -5, -8, 14, 18, 1, 0, -10, 13, -3, 3, 10, 12, 17, -4, 0, 7, -13, 27, -3, -5, 12, -14, 11, -20, 6, 25, 8, -8, -3, -3, -2, -2, -19, 21, -6, 15, -6, 19, -14, -14, 0, 3, 12, 11, -7, -8, -12, 3, -3, 28, -3, -5, 3, -13, -11, 20, 20, 7, 11, 17, -8, 3, 26, 1, 5, 21, 9, 0, -8, 5, -19, 3, 17, 3, 2, 3, -1, 3, -9, 12, 26, 4, -16, 6, 40, -10, 1, 12, 8, -2, -6, -6, 10, -20, 2, 4, 13, 5, -3, 11, 1, 8, 10, -6, 23, -6, 0, 9, -10, 1, 0, 16, 5, 31, 22, 12, -1, 21, 47, 10, 7, 0, -18, 27, 5, 22, -8, -5, -7, 11, 22, -10, 2, -3, -6, 18, -4, -10, -4, 19, 3, 20, 19, -5, -18, 21, 1, 13, -4, 9, 13, -10, -22, 33, -9, -10, -3, 12, -11, 12, -4, 16, 9, 23, -7, 20, -4, -1, 18, 6, 6, 13, -13, 2, 18, -15, 15, -4, -9, -9, -7, -6, 16, -9, -26, 1, -13, -3, 9, -11, -11, 17, -2, 4, 4, 6, 1, -7, -3, 0, -6, 7, -11, -1, -1, 13, 1, -16, 11, 15, 0, -5, 6, 13, 19, -6, -13, 1, 6, 3, 11, -1, -7, 17, -18, 3, 10, 2, -8, 25, -12, -3, -11, 4, 10, 5, -17, -9, 9, -9, -10, 16, 16, -16, 5, 4, 3, -4, -10, 2, 5, -18, 12, 13, -10, -9, -12, 7, 15, 4, -2, 10, -3, 3, -3, -6, 7, -1, -14, -6, 13, -11, -19, 14, -17, -2, -3, 9, -8, -11, -25, -22, -8, -53, -8, 5, -11, -10, 1, -18, 1, -12, 9, 9, -16, -20, 19, -2, -12, 16, -17, -20, -11, -16, 6, 3, -18, 3, -19, -9, 10, -8, -15, -13, 8, -2, -1, -16, 0, 6, -8, -3, -21, -14, 0, 8, -10, 9, -2, -1, -1, 4, -11, -5, -17, -18, -13, -24, -17, -6, 0, 9, -18, -21, -9, -13, 0, 14, 4, 14, 2, -5, -28, -5, -22, 0, -9, -21, -7, 30, -8, -13, -3, -4, 13, -11, -1, -20, -9, -17, -1, -5, -6, -4, -5, -6, -5, -13, 1, -18, -4, -5, -10, -9, -6, -12, -20, -5, 6, -22, 5, 6, -13, 6, -9, -6, -18, -14, -8, 13, -5, 0, 4, -10, 3, 6, -7, -10, -13, -20, -26, -6, 0, 7, -2, 10, 4, 5, -8, -8, 2, -20, -4, -8, -5, 2, -18, -1, -10, -14, 5, -9, -11, -18, -6, 20, -11, -42, -6, -21, -13, -4, -13, -9, -4, -10, -30, 3, -12, -11, -30, -19, -5, -18, -32, -21, -2, -7, -14, 9, 11, -1, -24, -12, -5, -26, -18, -21, 12, -17, -12, 6, 15, 13, -6, 11, -6, -19, -21, -2, -17, -1, -8, -9, 8, -20, -11, -27, 1, 7, -6, -17, -9, 0, 15, -27}
, {-6, 17, -22, 3, 1, 35, 5, -4, -20, 2, -25, -37, 13, -16, -3, -33, -18, 3, 0, -4, -1, -15, 10, -21, -14, -13, -27, 22, 0, -14, 9, -25, 1, -7, 8, -12, -2, 11, 1, 5, -23, -11, -4, 18, 7, 0, 4, -12, -16, 7, 15, 21, -3, -16, 26, 4, 0, -4, -28, 4, -2, -5, 9, -5, -15, -10, 14, -10, -36, 13, 2, -7, -7, -30, 2, -4, -18, 29, 18, 11, -6, 9, -3, 8, -5, 6, 18, 5, 39, -5, -29, 8, -8, 5, 12, -9, 4, 11, 5, 1, -2, 3, -13, -11, -11, -30, -24, -28, -5, 2, -2, 12, -13, -8, -18, -11, 6, -10, -18, -12, 48, -13, -11, 7, 8, 24, -31, -14, 5, -8, 5, -31, 11, 16, -1, -14, -7, 5, -2, -17, -8, 6, -13, 26, -10, -12, 2, -30, -3, 0, 6, -11, 9, 0, 5, 2, 14, -6, -15, -7, 16, 1, 2, 2, -6, -1, 0, 28, 19, -11, -13, -11, 6, -6, -16, -4, -15, 7, 3, 2, -7, -1, 6, -13, 10, -2, -1, 6, 8, 16, 4, -23, -11, -12, 13, 7, -1, -1, -1, 2, 3, 0, -4, -2, -7, 0, -17, -24, -7, -9, 11, -22, -6, -3, 5, -9, -12, 3, -6, -9, 2, -1, -5, 9, 5, -7, 0, -8, 1, -1, -20, 3, -6, 3, -11, 5, 2, 0, 13, 8, 25, -6, -2, -2, 9, -5, -21, -13, 0, 1, -3, -9, -12, 0, -14, 19, 16, 5, 11, 16, 0, 17, 9, -9, -5, -14, -4, -12, -9, -17, 18, -7, -9, 2, 2, -5, 10, 0, 8, -28, -15, -1, 8, -4, 8, 14, -5, 0, 11, 5, -5, 8, -4, 11, 4, -18, -4, -11, -15, -28, -11, -2, 16, -3, 4, 2, 2, -25, -4, 13, 7, -25, 1, -12, 5, 2, 4, 4, 1, -3, 11, -14, 2, 13, -9, -7, -16, 15, 2, -11, -3, 14, 1, 25, 3, -7, 17, 9, 2, 1, 4, -8, -1, 20, 12, 3, -3, -18, 1, 0, 19, 9, -3, 1, -2, -4, 1, 2, 9, -10, -30, 15, -7, -4, -17, 1, -19, 3, -22, 17, -5, 19, 6, -28, 23, 11, 3, -1, -1, 5, 18, -6, 2, -27, 2, 21, 7, -9, 17, 5, 18, -30, -5, 7, -2, 1, -11, -17, -12, -4, -27, -23, 1, 16, -5, 2, -18, -13, -4, -4, 5, 23, 10, -26, -15, -34, 16, -18, -6, 13, 19, -38, -22, -12, 17, -10, 10, -26, -3, -3, 46, 6, 10, -40, -25, 4, -1, 9, -2, -10, 12, 2, -26, 10, 17, 15, -7, 0, 25, -39, -29, -8, 5, -41, -20, 10, 15, -13, 16, 20, -4, 10, 34, 30, -6, -31, -32, 20, 0, -17, 17, 6, 8, -2, -13, 6, 0, 48, 7, -12, -7, 7, 14, 8, 5, 3, -1, -2, 11, 3, 5, -1, 8, 17, -16, -2, 20, 14, 29, -1, -7, -4, 2, 28, -10, -15, 23, 2, 15, -17, 3, 12, -35, -16, 22, 19, -25, 5, -20, 10, 13, -12, -20, -9, 0, 25, -9, -17, 7, 1, 8, -6, -6, 2, 15, 28, 7, -2, -3, 0, 7, -10, 2, 14, 20, 23, -6, 17, -10, -7, -11, -16, -18, 2, -12, -14, 3, -23, -1, 5, 9, -6, -13, 33, 8, -3, 2, -16, 17, 5, -8, -9, 9, 19, -12, -4, 10, -2, -15, -11, 17, 2, 14, 3, 14, 19, 19, 45, 0, 8, 21, 5, -8, 2, 13, 17, 1, 8, -8, -12, -5, -1, 25, -11, -2, -1, 9, -19, -16, 20, 17, 2, 18, 12, 16, 6, 13, 5, 19, 48, -6, -5, 17, -12, -13, -9, 2, 37, -9, 12, -7, 9, -17, 2, 27, 0, 12, -12, 6, -2, -4, -25, -10, 0, -4, 2, 13, 11, 15, -1, 3, 5, 6, 3, 14, 14, 22, -10, 11, -4, 3, -10, 4, 3, 17, -4, -10, -2, 0, 12, -14, 14, -4, -21, 16, -4, -14, -11, -1, 18, 16, 13, 14, 15, 1, 27, -2, 18, -4, 11, -13, 6, -8, -16, 13, 14, 12, -11, -17, 10, -32, -9, 23, -11, -19, -3, 1, 4, 16, -9, 4, 32, 16, -1, -20, 16, -1, 6, 35, 6, 8, 2, -5, 30, 5, 6, 0, 29, -2, -12, -11, 12, -54, 16, 18, 19, 7, 9, -3, 16, 16, 7, 16, -21, 2, -12, -10, -13, -14, 1, -3, 7, 9, 21, 26, 13, 8, -9, -2, 28, 5, -4, 11, 3, 13, 10, 5, 2, 0, 33, 20, 0, 6, 15, 24, -11, -6, 19, 25, -14, -4, 19, -16, 8, 12, 14, 21, -9, 1, -3, -17, -16, -17, -5, 44, 1, 25, -19, -18, 24}
, {4, 13, -9, 8, 0, 4, -1, -22, -14, -21, -7, 3, 4, 18, 19, -4, -2, 2, -11, 27, 8, 12, 8, -21, 5, -9, 5, 26, -8, 9, -5, -17, -42, 6, 7, -11, 4, 2, 40, -15, -26, 25, 9, -13, 8, -2, -3, -8, -21, 8, 0, 6, 8, 22, 19, 21, 2, -3, 1, 25, -16, -30, 26, -19, 4, 12, 13, 8, -17, -3, 21, -25, 7, -28, 9, -4, 8, 7, 39, -1, 19, -9, 14, 40, 5, 17, 29, 1, 22, 4, -18, 5, -8, -7, 7, 10, 11, 13, 9, -8, 1, 12, 32, 25, 3, 19, -5, 12, 4, 19, 5, 26, -23, 9, 7, 31, 1, -2, 6, 28, 23, -5, 8, 1, -20, 5, 23, -18, 14, -1, 16, 15, 24, 18, -2, 14, 0, 38, -2, -9, 0, 16, 6, 30, 15, 24, -6, 27, -7, 8, -14, 15, 29, -1, 10, 20, -2, -38, -2, 34, 2, 8, -1, -29, 16, -2, -2, -12, -20, -5, 2, -25, 5, -10, 10, 8, -19, -1, 0, -3, 3, 10, 14, -18, 5, -10, -6, 6, 14, 10, -18, -16, -8, -5, 13, -17, -6, -5, 14, -34, -14, -4, 7, -33, 0, 1, -1, 1, -10, -2, -10, -40, -4, -6, 1, 5, 6, -4, -14, 9, 4, 1, -5, -34, 1, 1, -1, 13, 11, -4, 0, -11, 7, 20, 18, 10, -2, 7, 3, 17, 28, 23, 17, -29, -8, -10, 2, -3, 1, -8, -10, -1, -18, -11, -7, 17, -11, 5, 1, 18, 19, 23, 9, 9, 23, -29, 7, -28, 15, 0, -18, 5, -1, -36, -1, -17, -9, 11, -21, -22, -23, 0, 8, 16, -20, -11, -24, -3, -13, 41, 19, 16, 0, 0, 10, 7, 4, -29, 11, -9, -5, 9, -16, 16, 2, -32, 32, -35, 0, 14, -4, -12, 4, 0, 14, -6, -11, -26, -35, 5, 6, -7, 11, -12, -7, -12, -28, -31, -24, 18, 10, -9, 15, 4, -7, 11, -3, -37, -3, -29, -10, -29, 14, -29, 11, -3, -28, 1, 3, 20, -13, -29, -11, -21, 3, -1, -8, -15, 4, -35, -44, 11, -10, -19, 3, -18, 9, -14, -6, -31, -11, -3, -1, -31, 20, -21, -15, -5, -19, 11, 3, 7, -14, -14, -1, -26, 19, 6, 12, -4, -8, -39, -27, 19, 8, -9, 17, 11, -42, 9, -9, -22, 4, 4, -6, -29, -17, -5, 1, -1, -2, 15, -2, -6, -15, -28, 13, -53, -27, 17, 1, -40, -34, -12, -17, 0, 8, -17, 10, -17, -7, -7, -25, -64, -38, 14, 8, -8, -37, -16, 6, 7, -14, -8, 2, -6, -24, -17, -14, -38, -7, -31, 10, -44, -29, -13, -30, 11, 42, -38, 16, -17, -38, 17, -48, -48, 6, 9, -7, -13, -35, 15, 8, -7, -26, -47, -5, -35, -6, -32, -21, -29, 20, -1, -11, 3, -3, -24, -45, -20, -3, -28, 9, 19, -21, -36, 6, -37, 5, -12, 3, 1, -22, -19, -24, 3, -2, -10, 8, -33, -6, -22, 1, -28, 33, -14, 15, 22, 15, 6, 2, 3, 16, 19, 11, 38, 9, 19, -11, -8, 7, -6, 2, 3, -6, 3, 21, -7, -3, 16, 13, 2, 0, 2, 16, -46, 2, -4, -21, 0, -32, -30, 1, -14, 9, -12, 14, 6, 8, -4, 9, 1, -6, -31, -9, -16, -8, -13, 27, 4, -34, 24, 7, 16, -13, 16, 11, -25, 33, -4, 5, -33, -6, -3, -4, 0, -39, -13, -1, 6, -13, -32, -1, 15, 21, 16, 5, -22, 13, -39, 5, -4, -29, -21, 17, 4, -1, -13, -2, 15, 24, -16, -15, -21, -22, -14, -16, 29, -32, -17, 16, -7, -17, -27, 0, 22, 18, -16, 6, -18, -2, -13, -30, -14, -7, -52, 23, -2, 13, 2, -21, 6, 3, -15, -25, -20, -3, -27, -53, -6, -10, -2, 6, 5, -62, -14, -31, -48, -6, 15, 7, 3, -38, -35, -34, 3, -15, -13, -5, -46, -38, -25, -20, -44, -36, -13, 19, -41, 7, -7, -20, 6, 6, -11, -2, -16, -7, 18, -39, -17, -38, 1, -9, 12, -28, -34, 4, -9, -8, -8, -14, -20, -29, -38, -6, -32, -29, 40, -4, -24, -6, 23, 28, -2, 6, 25, 18, 22, 38, 2, 8, -10, 13, 11, 5, 3, -2, 33, 5, 5, -6, -3, -33, 5, 11, -6, 7, 12, 12, 38, -14, 6, -41, 11, -2, -19, -40, 12, 0, 24, 7, 5, 39, 39, 37, 11, 9, 10, 14, 70, 10, -4, 24, 17, 11, 40, 28, 23, 3, 31, 8, -4, -51, 32, 2, -6, -15, -19, -27, -28, -5, 4, -21, -15, 6, 3, -5, -1, -5, -26, 13, 19, -37, 1, 28, -3, 24, -15, 14, 2}
, {19, 12, 15, -2, 24, 12, 14, 1, -10, 8, 4, -2, -5, 16, -15, 17, -5, 7, 9, -27, 3, 8, -2, 21, -18, -5, 10, 6, 4, -11, 9, -12, -22, -4, 16, 19, 0, -14, -18, 13, 44, 9, 8, 16, 4, 40, -15, 26, -6, 2, 19, -21, -6, -5, -17, 20, 22, 5, 18, -8, -5, -15, 11, 17, 20, -1, -4, 8, -16, -19, -20, 10, -7, 34, -9, -3, 2, -10, -36, -1, -11, -19, 0, -22, 8, -16, -23, -19, -9, -7, 14, -13, -12, 2, -9, 21, 12, -23, -7, -6, 37, -3, 1, 11, 35, 21, 23, 12, 6, -5, -17, 6, 11, -3, 13, 11, 8, 10, -25, -32, -22, 10, 25, 15, -15, -24, -1, 14, -36, 1, -8, -1, 1, 5, -5, 9, 14, 0, 10, -2, 4, -13, 7, -9, -3, -1, 6, 4, 1, 16, -11, -2, 3, 2, 8, -2, -1, -19, 20, 0, 8, -29, -13, -29, -9, -9, 3, -40, -10, -3, -1, 0, -13, -5, -28, -4, 5, -4, 1, 1, -11, -6, -8, 23, 13, -4, -5, 2, -8, -10, 0, -17, -16, 1, -12, 5, 2, 2, -4, -4, 12, 7, 3, -18, 6, 4, -3, 9, -13, 8, 18, -7, 8, 7, -14, -17, -3, -7, 7, 11, 0, -36, -17, 4, -11, 5, 12, 6, 9, 9, 18, 11, -1, 12, -11, -24, 1, 9, 6, -8, 9, 1, 4, -18, -3, 3, 2, -1, -25, -3, 0, 15, 36, -13, 7, 11, 15, -8, 3, 12, 3, -8, 3, -24, 0, 9, 1, 7, 1, -26, 9, -4, 2, -25, 9, -18, 8, -4, -8, -40, 3, -4, 2, 12, 9, 19, -20, -22, 18, -23, 8, -19, 9, 8, 5, -6, -2, 7, 10, -13, -1, -1, 1, 9, -4, -19, -18, 6, 0, -10, 18, -23, 9, 4, -22, 4, 16, 27, -14, 1, -16, 4, 3, -13, -21, -5, 2, -4, -3, -13, -26, -16, -9, 7, -5, -23, 10, 2, 7, -20, -6, 9, 4, 3, -11, 2, 10, -14, -1, 12, 8, 8, 5, 0, 10, 0, 4, -13, 10, -3, 0, -2, 21, -20, 4, 6, -18, 5, 9, 14, 25, -11, -4, 7, -11, -32, 6, 1, -4, -6, 6, 13, -11, 10, 5, -10, -2, -2, 0, 1, -22, 9, -1, 3, -9, -13, 5, 1, -15, 9, 3, -8, -10, -34, 7, 3, -21, 16, 12, 14, -11, -5, -5, -3, -26, 6, -4, -9, -9, -11, -4, -1, 16, 3, 0, 7, 4, 9, -3, 10, 4, 10, -22, 14, 22, 12, -2, -9, 15, -26, 16, 5, 0, -19, 25, 10, 6, -21, 22, 16, 40, 14, -17, 1, 8, 9, 12, 22, 11, 3, -4, 19, -2, -2, -1, 28, 15, 8, 3, -12, 41, -15, 25, 10, 11, 5, -3, 20, 25, 10, -3, -15, -11, -4, -5, 10, -17, -5, 4, -11, -11, 3, 2, -1, 2, 7, 23, -24, -15, 27, -3, -12, -7, -3, 5, 11, -14, 25, -1, 11, 5, -16, 3, -23, -18, -18, 5, -20, -14, -11, -14, -2, -7, -35, 7, -14, 11, 8, 0, -8, -4, 7, -7, -10, -10, -36, -9, 12, -12, -27, -3, 19, -10, -29, -3, -15, -3, -3, -18, -3, -21, 1, 4, 3, -8, -14, -11, -3, 13, -8, -23, -16, 6, -27, 8, -19, 5, -21, -9, 12, -23, -27, -13, 7, -35, -8, -18, -9, 10, 0, 4, -4, 8, 3, 5, -17, 8, 20, -2, 17, 9, -6, 2, 2, 9, -2, -7, -18, 13, 30, 11, -2, -3, 9, 5, 2, 10, -5, -8, -8, -14, 3, -41, 4, 8, 6, -2, -14, 25, 10, 3, 27, 2, 21, 0, 2, 10, -7, -5, -9, 11, 26, 14, -5, -21, 8, 1, 5, 9, 12, 1, -3, -1, -4, 0, -7, 19, 8, 4, -19, -1, 21, 4, 15, 2, 13, 11, -24, 0, 26, -3, -13, 11, 5, -6, 8, 4, 12, -17, -11, 9, -4, -11, 6, 7, -10, -8, 22, 5, -3, -2, -6, -22, -17, -6, -7, 5, -21, -2, -13, 19, -8, 5, -4, 13, 3, 4, 2, -3, 8, 15, -10, -11, 13, -4, -17, 24, -11, -13, -14, -20, -26, -9, 16, 2, -48, -2, 13, -17, 14, -6, -11, 4, -11, -5, -4, 1, -29, -14, 5, -10, -21, 18, -24, -9, -14, -5, 15, -4, -5, 0, -1, 17, -22, -20, -3, 9, 17, 7, 15, -10, 3, -23, -32, -15, -8, -4, 17, -12, -25, -10, -7, 12, 6, -3, -22, 4, -22, 1, 9, -16, 2, 8, -10, 17, 2, 0, 7, -7, 11, 6, -5, 4, 12, -11, 3, -19, 19, -7, 28, -11, 12, 26, -3, -18, 20, 4, 3, 21, -19}
, {32, -3, 15, 12, 21, -2, 6, 35, 42, 15, 32, 64, -6, 56, -35, 52, 26, 6, 16, 34, 4, 3, -9, 26, 49, -5, 34, 33, 10, 16, 18, 45, 19, -5, 9, -1, 11, -14, 10, 11, 21, 18, 13, 17, -2, 12, 0, 43, 12, 1, 12, 5, 2, 11, -13, -13, 57, 4, 19, -7, -1, 10, -17, 11, 8, -2, -7, 21, -6, 7, 9, -1, -7, 3, -1, -8, 4, -21, -11, -1, -5, -17, 4, 10, 7, 22, 3, -24, -16, -7, 13, 11, 4, 4, -13, 7, -30, 8, 5, -18, 22, 24, 15, -12, -1, 31, 1, 20, 14, 20, -3, 3, 5, 11, 10, -14, -4, 30, -3, -5, -11, -12, 8, 17, -6, -14, 8, -15, 1, -21, 12, -1, -9, 3, -4, -25, 1, 12, 1, 22, 4, -14, -10, 17, -14, 0, -24, -11, -2, 3, -15, -22, 23, -7, -2, 21, 1, -9, -15, -30, -14, -15, -13, -1, -11, -33, -28, 51, 51, 9, -7, 46, -6, -20, 23, -8, 16, -2, -8, 5, -6, -22, 2, -18, -17, 8, 9, -32, -15, -14, -9, 31, -11, -12, -17, -3, -30, -17, -26, 12, 15, -15, -15, 8, -3, 24, 16, -9, 22, 15, 11, 21, 2, -17, -5, 6, -1, -8, -17, -29, -9, -2, 3, 22, -4, -23, 4, 27, -5, -5, -5, 1, -4, -4, -1, -33, 11, 4, -6, 1, 15, -23, -23, -1, 4, -18, -27, 17, 11, -10, -2, -2, 23, 0, -2, 24, 10, -17, -19, 3, -2, -6, 9, -11, 9, -15, -2, -39, 1, -26, -5, -3, -3, -39, -8, -24, 0, -18, -25, -36, -6, 4, -28, -7, -5, 20, -34, 2, -11, -10, -27, -13, -10, -29, -16, -14, -10, -22, -18, -31, 9, -6, -17, -7, -20, -19, -25, -15, -9, -14, 15, 0, 5, 10, -7, -15, -15, 9, -9, -28, 16, 36, 6, 9, 26, 30, 0, 8, 12, 9, 23, 2, -11, -20, 2, 14, 2, 18, 10, -19, -4, 20, 6, -2, -5, 6, 38, -1, -18, -2, -5, -3, -25, 13, 27, 2, 15, 46, 15, 11, 21, -15, 12, -2, 9, 23, -34, 0, 1, 3, 29, 1, 6, 4, -9, -24, -30, 6, 17, 34, -17, -7, 9, 13, -14, -28, 10, 23, 3, 8, -15, -2, 4, 9, 2, -24, 11, -12, 2, 10, 7, -21, -9, -19, 6, 14, -27, -7, -16, -1, -19, 5, -16, -14, -8, -2, -22, -52, -12, 6, -2, -23, -6, 4, -4, -2, 11, 4, 8, -5, 13, 3, -14, -28, 3, 9, 4, -1, 15, -47, -2, -7, -24, -13, 19, -3, -3, 19, 2, 10, -1, -14, 12, -10, 16, -1, -5, -8, -8, 15, 12, -6, 1, -26, -5, -28, -15, -16, 8, -22, 17, -13, 10, -8, -3, -7, -20, -11, 9, 5, -14, -15, -2, -6, 18, -1, -5, -7, -13, 3, 1, -6, 5, 14, -18, 14, -15, -44, -30, -3, 6, -11, -15, -24, -4, 5, 0, 35, 24, -1, -26, -33, 29, -13, -34, -2, -1, -41, -19, -9, 10, -5, 6, 3, 13, -10, 11, 8, 4, -34, -24, 10, -3, 12, -12, -24, 7, 2, -11, -10, 14, 26, -7, 2, 7, -41, -15, 7, -14, -20, -3, 24, 21, 25, -5, -4, 10, -11, 26, -22, 10, 15, 8, -17, 7, 8, 1, 17, 2, -7, 4, -17, 30, 5, 16, 18, -2, -20, 18, 15, -9, -26, 4, 7, -1, 0, -12, 13, 14, -5, 24, -6, 12, -2, 13, 0, -8, 18, 12, -11, 18, -11, 0, -14, 28, 19, -3, 15, -1, 14, 13, 3, -23, -2, 0, 15, -17, 9, -31, 2, -6, 4, -6, -23, 3, 19, 10, -6, 6, -5, 24, 4, -9, -9, -11, -24, 12, 19, 7, -2, -7, -16, -13, -5, 2, -13, 12, -14, -36, 11, 0, -36, -7, -15, -39, -1, -35, -17, -9, 6, -10, -8, -4, -17, -6, 9, -24, -17, -13, -26, -17, -32, 16, -8, -1, -8, 4, 25, -11, -8, 0, -6, -9, -12, 3, 0, 0, -11, -23, 0, -4, 23, 7, 9, 4, -21, 14, -4, 4, 17, 7, -11, 1, -7, 5, 14, 2, -29, 22, 14, 0, 14, 7, 24, 12, 10, -3, 3, 12, 4, 12, -14, 6, 16, -1, 19, 1, 2, -7, 6, 2, -15, 9, 16, -18, 8, -5, 26, -2, -7, 24, 9, 27, -9, 10, -2, 8, -10, 10, 1, -6, 13, 24, 8, -1, 14, -2, 49, 12, -1, -20, 6, 12, 8, 3, 11, -6, -9, -16, 32, 22, -4, 10, 41, 39, -21, -39, -46, -32, -44, 10, 3, -9, 12, -22, -41, -23, 7, 9, 33, 3, 35, 21, 4, 13, 36, -18, -34, -5, -34}
, {14, -12, -14, -2, 11, 15, 6, 0, -13, 12, 4, 2, 0, 4, -5, 3, -3, -1, 7, -7, -10, -10, 4, 21, -18, 3, -3, 9, 18, 1, 10, 4, 14, 26, 23, -13, 5, -4, 13, 12, -8, -7, 16, 11, -4, 11, -9, 20, -20, 0, 18, 7, 3, -5, 10, 11, 11, -7, -2, -17, -4, -8, 0, 4, 7, 10, 11, -12, -37, 1, 9, -11, -11, -9, 11, -10, -8, 2, 22, -1, -9, 16, 8, 1, 5, 3, 7, -1, 30, -11, -9, -16, -15, -6, 3, 1, -9, 11, -12, -2, 28, 10, -1, -8, -11, -15, 5, -14, 6, 8, -14, 5, -3, -19, -5, -5, -2, 17, -9, -19, -5, -5, 5, 9, -18, 5, -17, 9, -15, -11, 20, -3, -5, 6, -4, 1, -13, 0, -4, -9, -10, -5, -4, 19, -7, -22, -1, -16, 9, 6, -13, 6, 23, 2, 4, -13, 8, -10, 3, -14, 8, -17, -19, -1, -10, -15, -13, 10, 14, 6, -7, 0, 3, -20, 18, -3, -25, -16, -9, 5, 7, 2, -5, 2, 13, -10, 10, -10, 20, -11, 0, -35, -13, -17, -18, -4, -10, -11, -1, 17, 23, 5, -8, 31, 4, -1, 3, -14, 0, 5, -5, 0, 4, -5, 5, 37, -1, -10, -1, -23, 3, -12, -8, 29, -10, -6, 17, 15, -9, 0, 5, 25, 6, 5, -11, -7, -13, 11, 26, -4, 16, 5, -3, 13, 1, 5, -14, 13, -4, -8, -11, -7, -2, 16, 12, 29, 9, 7, 6, 18, 2, 8, 2, 7, -4, 7, 10, -17, 2, -14, -3, -1, 4, -16, 7, 4, 9, -6, -24, -33, 0, 2, -4, 15, 13, -1, -24, 8, 6, -15, -12, -9, 9, -3, 9, -23, -18, -15, -6, -26, -9, 7, -2, 13, -17, -22, -25, -34, -6, 9, -26, -17, 12, -5, -1, 16, 6, -1, -12, -16, -6, 10, -8, 10, 5, -9, 5, -3, 1, 17, -3, -2, -8, 5, 0, -7, -13, 39, 23, -15, 2, 19, 4, -17, 3, 3, 24, -7, 11, -1, 6, -16, -3, 18, 10, 9, 12, 12, 2, 2, 25, 8, 15, 30, -17, -7, -5, -1, 0, 23, 40, 28, -5, 13, 1, -10, -18, -7, 24, 6, -1, -14, 0, -9, 6, 21, 7, 6, 28, 3, -5, 4, 28, 17, 12, 16, -18, 6, -1, 11, 9, -14, 1, 29, 0, 10, -27, -21, -11, -1, 14, 13, -13, -29, -19, 10, -4, -27, -13, 2, 18, -9, -5, -20, 2, -18, 1, -28, -13, -15, 40, 7, -10, -13, -16, 26, 1, -3, 0, -9, 0, 17, -31, 4, 14, 22, 0, 6, 4, -27, -12, 5, -31, -27, -41, -4, -6, 16, -3, 8, -14, 2, 12, -6, -12, -19, -23, 17, -3, -11, 9, -21, 21, -2, -23, -15, 0, 6, -6, -13, 4, -12, -3, 4, 0, -7, 11, 8, 8, 14, -6, 7, -11, 52, 0, 25, 3, 7, 6, 0, 0, -31, 2, 50, 6, -11, -1, 6, -15, 7, 7, 2, -1, -11, -2, 30, -8, 15, 2, 4, -12, 17, 0, 11, -6, 19, -15, 25, -1, 7, 11, -8, 3, -24, 5, -8, 13, -5, -18, -2, -5, -12, -2, 10, 2, 3, -12, 26, 6, -8, -9, -5, -8, -8, -27, -4, -11, 2, 5, -10, -15, -8, -11, 16, -2, 6, 6, -31, 4, -3, 9, -9, 11, 1, -24, -13, 6, -11, -25, 1, -3, -8, -3, 0, 4, 8, 0, 5, -3, 3, 19, -9, -15, -14, 14, 16, -9, 18, 6, -12, 2, -2, 12, -23, 7, -5, -11, 1, -23, 8, 10, -6, -38, -20, -6, -18, 4, 22, -17, 13, -6, 1, 3, -19, 9, 0, -1, -9, 5, -6, 6, 3, -21, 6, 11, -19, 8, -9, -9, -12, -13, -9, -3, -1, -9, -11, 5, 14, 21, -8, 16, 0, -10, 10, 4, -5, 32, 16, 11, 19, -6, 4, 27, 20, -16, -8, -4, -26, -18, 27, 13, 15, 17, -34, -1, -3, -24, -22, -33, 46, 24, -34, -3, 7, -4, 34, 9, -5, 31, 20, -14, 8, -9, -27, 12, 34, -3, 7, -15, -4, -29, 25, 34, 25, -11, 13, 7, 11, 12, -8, -1, 50, 6, -8, -35, 16, 1, 22, 20, -5, 26, 22, 18, 13, -9, -26, 18, 32, -17, -4, -33, 17, -26, 20, 24, 42, 2, 18, 4, 29, 28, 18, 7, -26, -8, 5, 1, -29, -10, -18, -14, 1, 3, 12, 28, 3, -1, 23, -13, 8, 13, 3, 6, 21, 34, -4, -17, -15, -7, -2, 32, 5, -10, 23, 26, -17, 5, 23, 24, -25, -13, 13, -13, 2, -2, -12, 22, -24, -8, 10, -16, -35, -23, 5, 40, -11, 26, 0, -25, 5}
, {-15, -22, -10, -18, -12, -6, 1, 3, 0, -12, -19, -11, 8, 10, -15, -27, 2, -4, -11, 0, 7, -13, 0, 6, 6, 0, -4, 2, -5, -26, 8, 0, -6, 7, -4, -9, -10, 0, 17, -4, -12, 45, 17, -14, 10, 10, 4, 15, -7, -3, 7, 7, 1, 8, 1, 16, 10, 8, -2, 15, 14, -19, -1, -12, -14, 12, 1, 4, 12, 6, -8, -7, -16, -22, 16, -12, 3, -7, 16, 26, -18, -8, -12, -1, 1, 10, 3, -1, 34, -5, -12, -7, 28, -38, 2, -13, -19, -17, -17, 7, 4, -20, -11, 6, -12, -45, -34, -8, -5, -20, -19, -12, 0, -23, -12, -2, 5, -16, -29, -4, -4, 1, -5, -8, -10, -27, -18, -8, -9, -10, 13, -29, -26, -28, -18, 0, -7, -4, -12, 4, 8, -13, -6, -19, -29, -6, 5, 2, 1, -15, -12, 4, -10, 4, -13, -29, -6, -40, -15, -20, -27, -6, 16, -18, -7, -4, -28, 7, -5, -8, -22, 5, 10, -19, -5, -18, 11, 0, -7, -3, -6, 10, -1, 1, -7, -8, 10, -18, -1, -11, -6, -5, -3, 0, -7, -28, -11, -1, -9, -5, -14, 3, -6, 1, 5, -5, -2, -9, -9, -16, -10, -16, 7, 1, -18, 5, -6, 3, -1, -15, 14, -6, -14, -6, 0, -3, -21, 0, -4, -7, -36, -11, -3, 6, -23, 11, 5, -6, -7, 6, 4, -25, -25, 5, 6, -16, -36, -5, -1, 2, -5, -8, 1, -6, -2, -15, -11, -12, -1, -7, -28, -29, -22, -10, -17, -34, -12, -16, -5, -26, -7, -27, -13, -14, -18, 5, 7, -18, -20, -8, -21, -7, -4, -30, -19, -14, -1, -21, -15, -10, 23, -28, 1, 9, 0, -4, -1, -23, -7, -1, -4, 6, -4, -16, -9, -12, 5, 5, 9, -9, 5, -12, -17, 4, 1, -11, 6, -15, 1, -9, 16, 3, 17, -23, 4, 3, -12, -12, 0, -5, 11, 7, -7, 0, -24, -8, -5, -4, 3, 1, -11, -9, -11, 1, 8, 3, 2, 7, -8, 6, -6, -11, -3, -8, 15, -3, -1, 3, -27, -2, 1, 7, -15, -7, 7, -1, 1, -7, 2, -9, 2, -7, 2, -9, 6, 3, -2, 4, 3, -15, 2, -4, 5, -5, 4, -8, 17, -11, -8, 14, 0, -14, -4, -28, -18, 10, -3, -4, 9, -12, -11, -15, 11, -1, 6, -17, 6, -3, -25, -4, -14, -7, 5, -16, 3, -3, -26, -10, 8, -13, -15, -5, -8, -6, -27, -4, -16, 4, -8, -4, -26, -21, -21, -15, -5, -15, -9, -2, -22, 1, -13, 9, 0, -2, -9, -21, 4, -30, -1, -1, 16, -9, 6, 6, 3, -15, -14, -9, 9, -3, 0, -6, -10, 3, -11, -12, 11, -8, 8, 7, -9, -5, 13, -10, -2, 6, 1, -15, 3, -9, 8, 3, 29, -13, -20, 1, -15, -5, -3, -15, -11, -10, -4, 14, -13, -13, -13, -10, 1, 1, 5, -6, -1, 2, -4, 1, 5, -3, -5, -10, 3, -18, -10, -8, 20, -11, -8, 4, 3, -7, -12, -16, -13, 10, -4, 16, 3, 0, 4, -3, 18, -6, -11, 6, -7, -9, -12, 0, 1, 0, -11, -25, -1, -19, -29, -1, 7, -15, -18, -2, -19, -7, -26, -20, -14, -3, -6, 9, -22, -9, -16, -7, -19, 3, -6, -9, -2, -9, 16, 6, -4, -14, -7, -22, 3, -5, -24, -6, 15, -15, -33, -24, -15, -14, -4, -9, -6, 3, 5, 0, -6, -12, -1, -35, 6, -8, 4, -20, -5, -10, -7, -10, -8, -16, 9, -7, 4, -18, -18, 0, 3, -17, -11, -10, -15, -9, -3, -6, 2, -7, 6, 13, -21, 0, -9, -15, -3, -18, -5, -20, 1, -2, -14, 7, -9, -24, 10, -6, -1, -22, -6, 2, 17, -6, -13, 0, 4, 2, -17, -11, -13, -2, 3, -1, -24, -14, -13, -2, -3, 1, 1, -2, 1, -12, 12, 4, -9, -19, -1, -20, 4, -5, -2, -8, 9, -11, 1, 1, -22, 6, 25, -1, 3, -5, 9, -12, 3, -4, 9, 5, 16, 1, -1, 37, 0, -4, 7, -1, 9, 4, 15, 22, -23, -2, 43, 3, 22, -16, 0, -22, -25, 13, 14, 32, 9, -16, 6, -16, 37, -27, 43, 6, 13, -2, -11, 8, 33, -9, 32, 8, -3, 1, 51, 42, -7, 50, 1, -4, -10, 19, -11, -7, -1, 5, -20, -5, -25, -4, 1, 6, -11, 16, 37, -3, -4, 1, -3, 5, 12, 7, -5, 3, 14, -14, 22, 33, -10, 0, -9, -6, -9, 11, -22, -11, -25, -12, -8, -15, -25, -11, -3, -19, 2, -29, -13, -8, -13, -1, -1, -21, -18, -1, 9, 5, -2, -6, -4, -6, 0, 1}
, {3, -5, 0, -8, -36, -4, -3, -1, -12, -1, -17, 9, 4, -21, -13, -28, 1, -22, 5, 7, -3, -13, -18, -26, -7, -3, -17, 0, -1, 17, -10, -11, 35, 1, -7, -35, 5, -3, 10, -17, -21, -10, 6, 13, 6, -17, 2, 22, 2, -10, 7, 23, -7, 3, 17, -15, 12, -1, -18, -14, -7, 20, -8, -7, 0, -11, 13, -7, -15, -7, 12, -17, -12, -34, 17, -19, -19, 1, 9, 6, -2, 9, 12, 2, -2, 6, 15, 6, 39, 0, -15, 7, 9, 11, -1, -24, -16, 12, 7, -10, -16, 17, 10, -18, -36, -18, -11, -14, -12, 16, -16, 0, -9, -15, -19, -17, -5, -11, -7, 9, 23, -12, -14, 3, 5, 0, -11, -22, 0, 2, 11, -6, -7, 4, 11, -12, -12, -9, -24, -18, -2, 3, -20, 33, 14, 1, -2, -20, 8, -7, 16, -3, 28, 3, -11, 24, -6, -2, -5, -4, -4, 3, -6, 8, -8, -2, -10, 2, -1, 2, -6, 0, 3, -12, 14, -17, -19, -7, 0, 5, 2, -8, 16, -37, -18, -3, 15, 5, -3, -5, 2, -27, 5, -7, 9, 1, 2, -4, -2, -1, -2, -14, -2, -2, 7, 1, -19, -1, -5, -16, -11, -19, -4, -8, 18, -15, -2, -11, -12, -2, -14, 16, -6, -4, -3, -2, 9, -2, -8, -9, -9, -14, 6, 1, -7, 3, 0, -2, 8, 1, 2, -11, -4, -4, 7, -17, -7, -4, 14, 3, 1, 1, -16, -2, -24, -16, -18, 9, 11, 4, 17, 3, 1, 5, -8, 8, 10, -31, -1, 1, 1, -11, -5, 7, -15, 0, 4, 6, -3, -17, -17, -7, -2, 9, 6, -3, -22, 4, -18, 6, 4, -13, 17, 4, 24, -12, -3, -11, 9, -4, 4, -11, 7, -13, -7, 8, 16, -7, 9, 4, 4, -13, -5, -13, 16, 10, -4, 0, 5, -8, -2, -31, -20, 2, 1, -11, -14, 8, -2, 9, -10, 7, 4, 2, 7, -5, -11, 8, -7, -7, 3, -14, 6, -6, 8, -12, -15, 4, 8, 7, -2, -9, -8, -16, 7, 5, -8, 1, 0, -6, 8, 5, -8, -1, 4, 6, 6, -3, -18, 7, 4, -1, 1, -9, 34, 4, 3, -15, -5, -9, -2, -9, 17, -21, -17, -6, 5, 9, -3, 16, 14, -13, 10, 11, 3, 9, -12, 14, 2, 9, -8, 0, 3, 1, -11, -16, 7, 9, -2, -6, 8, 8, 10, -20, 1, -8, 2, -24, -4, 19, 9, -22, -21, -14, 2, -12, 7, -27, 10, -9, 11, 2, 16, -8, -14, 2, 4, 12, -7, -2, -10, 4, -14, 12, 16, -8, -12, 11, 19, -31, -14, -19, -2, -23, -17, 9, 13, 6, 22, 4, 6, 4, 17, 22, -20, -29, -23, -18, 3, -17, -8, -1, 4, 5, -5, -6, 21, 12, -3, 3, -4, 8, 23, 7, -4, 0, 18, 18, 25, -7, 12, 12, 10, 22, 16, -19, -3, 17, 30, -20, -10, 9, 19, 34, -18, 6, 18, -44, 12, -6, 4, 18, -26, -2, 15, 24, -25, 7, -20, 20, 12, 4, 5, 8, -9, 17, 1, 0, 15, -11, -13, -36, 9, -1, -8, -5, 24, -6, 6, 2, 10, -17, -16, 40, 4, -3, -26, 1, -18, -19, -13, 3, -1, -13, -6, 26, 4, -22, -3, -4, -6, -7, 3, 10, -2, 13, -6, -18, -4, -7, 15, -12, 8, 1, -1, 1, 8, -5, -1, -19, -3, -18, 4, 0, 5, 17, 3, 20, 1, 12, -2, 7, -3, -9, -10, -4, 2, 18, -10, -22, -19, 5, 11, -14, 2, 7, -21, 7, -21, 10, -5, -12, 3, 2, -23, -9, 20, 22, -30, 2, 0, -7, 2, -7, -1, 2, 4, -12, -10, 2, -14, -12, -10, -8, 21, -2, 9, 1, 18, -7, 20, -3, 14, -1, 17, 4, 13, 3, 1, 13, -1, 4, -4, -14, -1, 21, -1, 0, 12, -23, 4, 3, -9, -32, 7, -6, 12, 5, 26, -8, -10, -11, -2, -25, -6, -4, 12, -17, 4, 9, 15, 18, 16, 11, -10, 9, -16, 6, 5, -10, -13, -1, -11, 8, -12, 15, 7, 1, 5, -5, -5, -5, -4, -13, -6, 5, -18, 24, 5, -10, 0, 47, 17, -11, -20, 27, 3, 11, 16, -6, 18, -15, -10, 3, -1, 7, -9, 15, 1, -5, 3, 6, -13, 11, -4, 13, -2, -2, 2, 7, 13, 21, 18, 6, 7, -2, -14, -28, 6, -7, 0, 16, 4, 23, 26, 9, 0, 13, 10, 10, 2, -6, 1, 7, 11, 7, 5, 3, -13, 16, 31, 0, 5, 27, 38, -14, 3, 11, 25, -33, 2, 20, -9, -9, 8, -5, 14, -11, 0, 7, -5, -21, -17, 5, 16, -2, 26, -2, -24, -5}
, {2, -9, 3, 0, 1, -5, 2, -4, 9, -2, -1, 6, -2, 0, -8, 1, 8, -4, -7, 8, -2, -1, -8, 1, -3, 1, 6, 4, -6, 0, -7, -5, -8, -7, -12, -4, -6, 7, -8, -4, -4, -9, -10, -2, -1, 3, 8, -9, -10, 8, -2, -7, -11, 8, -10, 6, -4, 6, -9, 6, 8, 3, -11, 3, -2, -2, 7, -4, -7, 0, 11, 7, -5, -6, -6, -1, 9, -5, -5, -6, -8, -4, -4, -9, 4, 4, -16, 7, -4, 8, 6, -10, 6, -9, -3, 1, -5, 8, -10, 0, 2, -3, -14, -10, -12, 31, 15, 8, -3, 8, -14, 29, 6, -4, 22, 5, 7, 5, -20, -7, 13, 6, 0, -5, 19, -11, -8, -6, 1, 1, 0, -6, -9, -6, -1, -1, 8, 6, -7, 8, 6, 4, -3, 6, -15, -4, -5, 5, 0, -1, -2, 0, 22, 13, -4, -1, 4, -18, -4, -10, 4, -6, -5, -10, -11, -11, -5, -11, 8, -6, 2, -7, 0, 3, 7, -11, -9, 3, -7, -2, 2, 4, 4, -8, -8, 11, -3, -4, -3, -8, -6, -10, -8, 3, -13, -1, 0, 1, 0, 7, 2, 1, -3, 7, -7, -10, -5, 3, -1, -10, -5, -7, 6, 1, -11, -9, -3, 12, -8, -12, -2, 3, 2, -5, 1, -6, -6, -8, 4, 0, -7, -6, -1, 5, -15, 9, 7, -9, -9, 2, 4, -8, 2, -3, -8, -13, -17, -5, -14, 14, -2, -13, -11, 6, -2, -7, 5, 9, -5, -3, -12, -2, -6, 5, -2, -5, -2, 7, -2, -10, -10, -5, -14, 4, -4, -9, -9, -11, -1, 2, -10, 1, -2, -2, 0, -11, -6, -14, -1, -4, 3, 0, -8, -10, 1, 5, 3, 6, 7, 2, -7, 8, 5, -15, 2, -1, 11, 5, 8, 3, -13, -1, -1, 16, 4, -9, -6, -9, 2, 3, 2, -8, -14, -8, -5, 4, -7, 6, 4, 6, -8, 9, -1, 3, -7, 7, 1, -8, -4, -6, 1, 7, -9, -4, 3, 15, -3, -4, 0, -6, 5, -2, -10, -6, -5, 1, -12, -6, -3, -4, -3, -10, 6, 1, 8, -9, 1, -5, 6, -2, -7, -5, -10, 5, -3, 1, -4, 5, -7, 9, 1, 1, -7, 9, 3, -4, -1, 0, 2, 1, 2, 1, 9, -7, -12, 3, 0, -8, -7, -12, 5, -5, -4, -5, 6, -4, -6, -8, -1, 14, -2, 3, -15, -3, 7, -4, 5, -5, 2, -10, -1, -4, -10, -8, 2, -9, -9, -5, -5, -10, 1, -3, -11, -7, 9, -5, 8, 6, 0, 8, -9, 12, 9, -11, 9, -2, -2, 4, 3, -8, -13, -6, 3, 9, -6, 0, -7, -11, 4, 9, -8, 1, -2, 1, -3, 0, -3, 5, -3, -7, -10, 2, -1, 5, 6, -5, -8, 1, -10, -1, -12, 9, -8, -7, 2, -3, -10, 7, -4, -3, -8, -4, 4, -3, -6, -1, -6, 4, 1, -10, 4, -4, -13, 0, -11, -2, 4, 2, -5, 4, 5, 3, -6, -2, -4, 9, -9, -8, -1, 1, -13, 3, -6, 0, 8, 2, -10, -3, -13, -14, -10, 8, -9, -8, 1, 3, -11, 4, -4, 7, -1, -3, 9, 2, -10, 3, -3, -3, 8, -8, 1, -6, 8, 7, -14, -6, 7, -3, -8, -5, -5, -3, 1, -5, 4, -4, -7, -4, 0, 4, 7, 6, 6, -13, -9, -1, -7, -6, -3, 0, -11, -13, -9, -4, -1, -5, -7, -6, 8, -5, -11, -2, 7, -12, -3, -5, -10, -13, -8, -10, -3, 11, -12, 2, 3, -10, 3, -6, 2, -9, -11, 2, 5, -12, -13, -3, -6, 1, -11, 4, -3, -2, 1, -13, -10, -7, -5, 5, 7, -7, -4, 5, 3, 3, -9, 5, 2, -4, 7, -7, -3, -11, -2, 8, 4, 4, -11, -2, -9, -4, -6, -9, 5, -1, -5, 6, -4, -5, -3, -3, 6, -2, 4, -8, 0, 9, -5, 1, -7, 7, 0, -4, -6, 4, -5, 5, -13, -8, 0, -4, 2, 6, -5, -8, 6, -11, -1, -9, 5, -6, -4, -10, 2, -6, -10, -2, 2, 14, -1, -11, 7, -3, 4, 6, -4, -8, -17, -11, -2, -6, -8, -8, -11, 7, -3, 5, 5, -7, -5, -4, -3, -6, 10, -8, -7, 4, -14, 6, -3, 8, -11, -4, 0, -6, 1, -2, 1, -1, -6, 8, -3, -6, -9, -2, -12, -18, -8, 4, 3, -2, -5, -12, -13, 6, -13, -9, -6, -9, -21, -7, -1, 6, 4, -8, -22, -11, -7, -17, -1, -5, 0, 0, 2, 6, 5, 6, -4, 1, 4, 0, -10, 7, 1, -5, -7, -13, -4, 6, 6, -11, -10, -8, -2, 2, 4, -9, -15, -13, -4, -2}
, {6, -14, -14, -1, -19, -27, -10, 5, 37, 18, 2, 1, -4, -29, 2, 0, 22, 8, 0, 16, 0, 35, 2, -22, 6, -4, -4, -27, 3, 12, -23, 27, 14, 4, -23, -3, 8, 7, 11, -3, 10, -1, -6, 0, 6, 1, 10, -9, 2, -4, 3, 10, -2, 21, -18, -28, -2, 4, 21, -8, 22, 13, 0, -12, 13, -11, 2, 5, 38, 22, 16, 14, 7, 2, 1, 11, 3, 6, 9, -11, -17, 6, -10, -7, -10, -5, 11, -3, -19, 17, 5, 17, -6, 4, -7, -17, -8, -2, 7, 19, -1, -21, 11, 1, 1, 39, 2, 21, 12, 7, -4, -21, -12, -4, 9, -15, 0, -29, 16, 11, -42, 6, -2, 15, -5, -23, 22, -9, 0, -2, 19, 18, 4, -6, 3, 5, 11, 4, 5, 26, 1, 10, 4, 14, 7, 14, 25, 23, -6, -8, -7, 4, 21, -5, -5, -1, 4, -5, -3, 4, 0, 35, 16, 4, 7, 3, 4, 33, -3, 13, 3, 30, -5, 16, 14, 10, 22, 8, 19, 7, 5, 16, -19, 35, -7, 8, 19, 22, 19, 17, 2, 41, 26, 9, -6, 10, 16, 3, -11, -7, 2, -11, -7, -10, 0, 4, 23, 15, -11, -7, 0, 28, -9, -2, 11, 8, 24, 3, -2, -7, -17, 3, 20, -17, 16, 1, -11, 3, -9, -12, -1, -6, -12, -2, -20, -18, 8, -1, -13, 9, -11, -5, -16, 7, -10, -8, -2, 5, 21, 13, -26, -6, -29, -4, 16, -10, -23, -24, 5, -9, -11, -23, -11, 6, 0, 25, 15, 11, 1, 15, -11, 9, -20, -11, -19, 5, 6, -19, 17, 5, 16, -8, -30, -7, -33, -31, 11, -8, -18, 4, 8, 16, -8, -9, 2, 15, 22, 28, -5, 35, 11, 13, 15, 13, -8, 7, 21, 28, 7, 1, -11, 31, 11, 0, 9, 6, -33, -26, 20, 12, 5, -13, -5, -6, 7, 9, -8, 30, 15, -4, 8, 38, 7, 15, 17, 7, -14, -1, -8, 17, -7, -10, 20, 44, 11, -3, -6, 5, 4, 7, 1, 27, -21, 18, 17, 1, 4, 6, -13, 19, 8, -14, -1, 9, 15, 16, 41, -12, 15, 21, 8, -15, -2, -9, 6, 29, -9, 3, -5, -3, -5, -3, 23, 7, -14, -2, -7, 11, -14, 7, -13, 21, -8, -28, -21, -5, 7, -12, -4, -34, 25, 19, 12, -13, 9, -16, 17, -2, -15, 2, -10, -36, -2, -9, 11, 22, 8, 28, 15, -12, -24, 19, -4, 25, -13, -8, -13, 17, 8, -6, -33, -26, -5, 12, 8, -12, -2, 6, 1, -6, -17, -3, 31, -16, -18, -4, -4, -6, 4, 34, 3, -5, 9, 6, 18, 14, -6, 22, -7, -17, 5, -18, -16, -22, 11, 34, 29, -9, 6, 3, 13, -13, -6, 10, 33, -11, -8, -22, 1, 16, -5, 4, -9, 10, 7, 0, 11, -12, -18, -1, 8, 14, 5, 5, -14, 9, -16, 22, 10, -2, -8, 8, 8, 1, -13, 1, 4, -2, 10, -7, 13, -14, 3, 29, 23, -16, 17, 14, 16, 6, 23, -4, -1, 15, -3, 30, 3, 17, -4, 9, 12, 14, 5, 19, -20, 14, -1, -1, 29, -2, -7, -6, 12, -14, -13, -22, -17, 9, 0, 17, -10, 10, -8, -9, -3, 5, 3, 17, -12, 12, 28, -12, 7, -22, -8, 10, -4, 22, -3, 4, 5, 8, -7, -5, 13, 34, 2, -8, -11, 13, 16, 14, -20, 0, -13, -14, -11, -12, 14, -2, 2, -1, 10, 30, -2, -22, -2, -14, -3, 14, 24, 0, -23, 27, -1, 4, 23, -2, -1, -18, 7, -10, 26, 20, 4, 10, -8, -23, 5, -26, -3, 21, -7, 28, -6, 3, -8, -27, -4, -18, 11, -6, -4, 7, -12, 6, -9, -7, 9, 10, -2, 13, -4, 0, 6, -4, -3, -7, -19, 10, -15, 11, 8, 3, -17, -8, -19, -2, -41, -15, 9, -18, 0, 26, -6, -5, 1, 33, -7, -4, 6, -27, 10, 14, -2, 11, 42, 9, -3, -22, -14, 17, 12, -18, 5, -19, -9, 10, -13, -17, 11, -8, -6, 6, -8, -12, -7, -2, 4, 13, -10, 0, -26, -34, 10, 1, -9, 9, 5, -7, -12, -17, 9, 4, 26, 6, 4, -6, -18, 0, -24, 16, 12, -5, -2, -3, -1, -3, -3, 6, 13, -1, 20, -3, 2, -16, -16, -1, -22, 21, -29, -15, -18, 37, 30, -4, 10, 8, 12, -1, 23, -25, 18, 8, -10, 10, 1, 7, 21, 24, -10, -2, 4, -21, -3, -8, 13, 33, -29, -23, -10, -1, 1, 8, -13, 16, 16, -25, -5, -4, 12, 4, 8, 4, 12, -10, -23, -18, -11, -22, 15, -9, -10, 10, -28, -6, -20, 9, 5, 26}
, {-19, -4, 24, -5, -8, -7, 7, 3, -5, -9, -11, -8, -10, 12, -8, -2, 0, 2, -3, -2, -1, -4, -14, 1, 14, -1, 4, 7, 2, -9, -10, 2, -8, -13, 12, -20, -15, 1, 6, 3, -7, 34, 17, -7, -6, 23, -3, 45, -3, -12, 10, -18, -5, -16, -7, -3, 40, -2, -19, 19, 0, -13, -17, -17, -11, -20, -12, 11, -15, -5, 2, 11, -31, -26, 4, -8, 0, -16, -26, 31, -27, -13, -15, -22, -4, -18, -10, -12, 41, 13, -10, -11, -19, -43, -20, -7, 14, -9, 2, -16, -12, -13, -8, 8, -7, -1, 8, -6, -4, -3, -4, 6, -23, -7, -9, -1, 8, -25, -21, -19, 15, -4, -15, 0, 0, -1, 8, -2, -7, 4, -11, 1, 9, 0, -13, -4, 9, 17, -5, 10, 6, -18, -8, -12, -15, 7, -4, -26, -3, -4, -10, -10, 28, 15, -29, 5, -8, -11, -1, -15, -3, -8, -5, -9, -15, -11, -23, 7, -19, -6, -17, -5, 3, -11, -12, -20, 1, -23, 13, -1, -9, -17, -10, -3, -3, -1, 9, -9, -10, -14, 7, -1, -18, -8, 9, -16, -9, -21, -11, -18, -15, -5, -17, -6, 4, -18, -7, -3, -5, -10, -9, -9, 0, -10, -22, -1, -11, 2, -3, -7, -7, -17, -9, 6, 15, -13, -13, -1, -41, -24, -17, -10, 5, -13, -24, 1, -1, -17, -20, -37, 1, -26, -11, -6, -11, -25, -19, -8, -13, 12, -11, -32, -10, 6, -22, 8, 13, 16, -3, -32, -18, -15, -16, 8, -7, -2, -3, 13, 0, -11, -3, -2, 9, 12, -12, 0, -2, -3, 10, -9, 5, -4, 5, -31, 18, 24, 18, 2, -11, 5, 4, -16, 12, 1, 12, -22, -8, 0, 0, -9, 4, 7, 6, -13, 12, 19, 7, -11, 6, 1, 22, 6, -12, 1, 5, 11, 17, -3, 12, 10, -24, 5, -4, 4, -15, 12, -12, -13, 0, -14, -1, 0, -7, 7, -5, -13, -15, 3, 16, -3, 2, 9, 7, -1, 11, -3, 0, -14, -9, -22, 7, -8, -3, -8, -1, 5, -11, -14, -8, 4, 4, 7, 6, 4, 2, -29, 1, -4, 11, 2, 5, 4, 8, 7, 9, -9, -5, 13, -2, 0, 0, -2, -21, 5, -6, 18, 3, -21, 0, 14, 10, 2, -2, -13, -8, 9, -8, 3, 3, -3, 17, 26, 6, -10, -1, -7, 3, -3, -17, 10, 2, -12, 16, 6, 7, -2, 8, 18, 5, -17, 15, 9, 26, 2, 8, -4, -2, 2, -1, 10, 0, -3, -45, 22, 6, -5, 1, 15, 17, -11, -4, 14, 33, 13, 4, 2, 6, -8, 6, -14, -16, 20, -1, 8, -33, -7, -5, 4, 16, -12, -5, 8, 11, 21, 16, -21, -21, -7, -2, -9, -11, -16, 25, -2, -5, 17, 16, -2, -2, 10, 5, 11, 6, 0, -14, -1, 1, -9, -7, 7, -6, -9, 1, -1, 7, -5, 13, 12, 6, 2, 4, 4, 19, 2, -3, -3, 31, -7, 4, 9, -3, -2, -5, 13, 11, 0, 9, 19, 10, -23, -7, 9, -3, -3, -10, -3, -9, -6, 6, 18, 0, -18, -2, 5, 25, -12, 8, 2, 14, 11, 13, 6, -9, -14, -9, 16, 0, 8, 14, 15, 25, 10, -3, -1, -12, 8, -9, 14, -8, 10, -1, -12, 7, -8, 7, -14, 18, 10, -5, 5, 8, 17, 4, -1, -14, -11, -8, 0, -13, 7, 6, -3, -10, 17, 12, -13, 4, -1, -1, -15, -14, -8, -13, 19, -2, -7, 0, -4, -24, -3, 8, 6, 10, 15, 7, -7, 2, -4, -12, -8, 0, 7, -13, -17, -10, 1, 4, -17, -22, -5, 5, -10, -19, -13, -4, -25, 0, -13, -10, -1, -24, 7, 1, 5, -8, -15, -7, -21, -17, 6, 14, 22, 13, 12, -5, 14, 16, -5, -3, -10, 3, -15, -1, 26, 10, 21, 20, -13, 3, -2, 6, 11, 11, 0, 15, 8, -14, 13, 8, -1, 2, 7, -6, 16, -7, 4, 2, 6, 14, -6, -26, 3, 4, 16, -5, 13, -14, 17, -10, -13, -15, -13, -10, -15, -18, -7, 37, 15, -7, 15, -23, -8, 20, -15, -18, -7, -26, 11, -10, -13, -18, 1, -10, -8, -12, -9, -3, 1, -30, -6, -2, -10, -11, -11, 2, -5, -37, -5, 13, -3, -13, -1, -10, -17, -2, 1, -15, -19, -14, -11, -10, -20, -12, -14, 6, -21, -22, -2, -10, -24, 5, -24, 4, -5, -4, 4, -4, -12, -16, -5, -24, 14, -5, -23, -17, -20, 2, 11, -19, -6, 3, -21, -5, 2, -10, -4, 3, 7, 3, -14, -6, -1, -13, -12, -3, -15, -4, -12, 2, -18, -9, 5, -12, 15, -3, -4, -3, -19, -12, -10}
, {6, -2, 9, -30, -6, -14, -6, -9, -8, -14, -11, -8, 10, -20, -3, -6, -1, -12, 7, -11, 8, -8, -2, -7, 3, 8, -3, -6, 1, 2, 2, 15, -1, -4, 6, -20, -13, -10, -18, 0, -18, 18, -16, 0, -9, 33, -9, 25, -5, 3, -5, -9, 5, -9, -19, -12, 25, -6, -7, -11, -7, -14, -2, -2, -24, -12, -1, -14, -9, -18, -26, -18, -6, -12, -5, 10, -4, -9, -17, 5, -18, -9, -18, -18, 7, -18, -16, 10, -6, 6, -5, -20, -20, -12, 1, -17, -16, -19, 6, -19, 12, -13, -21, -9, -5, -6, -9, -15, 5, -6, 2, -6, -7, -12, -14, -4, -9, -12, -10, -15, -7, -6, -27, 1, -22, -13, -13, -6, -14, -3, -9, -9, 19, -13, 0, 9, -9, -9, 18, 14, -1, -24, -6, 5, -12, -5, 1, -7, 0, -23, -3, 6, 13, 7, -9, -7, -8, -12, 14, -15, -18, -16, 5, -9, -1, 2, -16, 7, 3, -14, -4, 5, -2, 6, 5, -8, 7, -12, 4, 3, 2, -13, -4, 11, -14, -2, 2, 1, -6, -17, 1, -14, -2, 1, -5, -14, -10, -20, -23, -10, -14, -21, -32, -2, -9, -19, -7, -20, -16, -9, -17, 0, -4, -5, -10, -6, -11, 4, -4, -9, -10, 1, -2, -8, 0, -18, -13, -14, -16, -12, -30, -15, -8, -17, -30, 1, -7, -6, -13, -14, 8, -13, 9, 0, 8, -7, -18, 0, -14, -1, -5, -11, -19, -3, -10, 7, -21, -8, 4, -13, -23, -12, -5, -20, 7, -1, -14, 8, -10, -4, 16, -16, -6, -9, -7, -5, 6, 9, 10, 2, -13, -10, -3, -5, 10, -11, 1, -4, 9, 7, 20, -20, -8, -12, 13, 19, 12, 12, -6, -10, -5, 17, 31, -8, 5, 15, 2, 7, -9, 4, 28, 21, -6, -1, -20, -6, 23, 3, 9, 0, 7, -1, 11, 3, 0, 14, -7, -2, -12, -11, 8, 3, 4, -4, -18, -11, -18, -17, 13, -3, 3, -10, -7, 10, -11, -8, -9, -8, -16, -5, 4, 9, 0, -11, 3, -19, 15, 6, 0, 6, -5, -15, -12, 9, 2, 4, -8, 9, 7, -3, 4, -1, 8, -15, -6, -10, -7, 8, -20, 1, -10, -20, -5, -3, -15, -2, 26, 21, 6, -2, 11, -10, -2, -17, -9, -12, -9, 13, -7, 5, 2, -7, 10, 1, 9, -3, 8, -9, -7, 9, -23, 4, -11, 0, 6, 1, 10, -1, 5, -12, 6, -6, 25, -15, 3, 12, 1, 7, 3, 9, -12, -5, -9, 22, -9, 3, -3, 2, 5, 1, -7, -7, 7, -2, 10, 9, 0, -4, 9, 15, -2, 2, 7, 6, -2, -1, 13, -7, -1, 26, 1, 11, 7, 3, 2, 20, 18, 13, 7, -4, 3, -6, 11, 0, 23, 5, -18, -9, 10, 7, 4, -6, 3, 5, -9, -9, -11, -15, -3, 13, -7, 8, -4, -9, -10, 2, 16, -9, -21, 3, -3, 5, 3, -19, -9, 8, 8, 3, 4, -1, -5, -8, 1, 0, 8, -3, -17, -6, -10, -11, 6, -6, 8, 8, 0, -4, 15, -4, 1, 2, -2, 3, 4, -1, 11, 2, 1, -6, 7, -22, 13, 0, 1, 15, -1, -10, 19, 12, 1, 13, 8, -10, -24, -12, -8, -1, 2, 10, 0, -3, -1, 0, 11, 0, -8, -4, 6, -12, -8, 8, 1, -1, -6, -3, 3, 11, -11, -3, 8, -24, 1, 2, 12, -1, -2, 12, 5, -14, 9, 1, -21, 6, -28, 6, 3, 1, 2, 3, -19, 1, -5, -1, -16, 0, -19, -21, 8, -8, -23, 10, 10, -3, -8, -2, 8, -17, -16, -15, 0, 15, 7, 6, -18, 10, -9, 2, -11, -14, 3, -6, -17, 4, 2, 8, -7, 10, -27, -23, 1, -5, -9, 1, 26, -15, 8, 18, 13, -11, -2, -14, -12, 7, 6, 19, -2, 6, 9, 5, 0, 1, -5, 12, 13, 4, 16, 3, 9, 22, 0, -17, -14, 19, -12, 3, 1, -12, 5, -2, 14, 1, -3, 6, 22, 9, -4, -2, 11, 18, 0, 8, 0, 5, -10, -3, 12, -5, 13, -5, -12, 26, -24, 4, -7, 1, 12, -8, 23, 0, 1, 6, -18, 7, 10, 30, 9, 4, -8, 13, 12, -8, 9, 0, 25, 3, 8, 0, 5, 6, 10, 11, -8, 3, 12, 36, -7, 29, -3, -10, -25, -7, -14, -5, -8, 8, -12, -23, -16, 4, 9, -15, -18, -7, 28, -2, -17, -2, 1, -10, -12, -11, 6, -8, -11, -7, 4, 40, -5, 2, -4, -4, -4, 0, 5, 2, -23, 6, 10, -14, -1, 3, -1, -8, -19, -15, 4, -8, -14, 6, 4, -12, -22, 4, -24, -6, 1, -22, -26, 8, 1, 1}
, {-10, 7, 16, 0, 22, 13, -5, 18, 8, 2, 7, -14, -7, 13, -3, 25, 11, 18, 16, -25, 8, 1, 3, 42, 6, 7, 25, -4, 6, -26, 11, 9, -13, -1, 20, 29, -9, 3, -16, 13, 2, -8, -1, -5, 7, 0, -40, -3, 14, 9, 5, -49, 0, -2, -23, 2, -6, 10, 21, -13, 2, -18, 11, 15, -13, -7, 13, -12, -13, 15, -6, 16, 30, 57, -1, 31, -6, 4, -20, 7, -6, 3, 13, -11, 2, 3, -22, -2, -36, -5, 31, -25, -28, -3, -2, 15, -15, -20, -17, 20, 22, 8, -8, 33, 27, 19, 17, -11, 9, -11, -6, -11, 17, -13, 23, -3, -4, 12, -17, -11, -12, 7, 13, -2, -20, -24, -19, 23, -13, -10, -14, 5, -3, -30, -16, 12, 14, 0, 13, -8, 4, -11, 11, -25, -1, -3, -4, -1, 3, 6, -6, 4, -14, 12, -1, -3, 3, -14, 3, 7, 11, -28, -14, -4, 6, -14, 7, -12, 10, 2, 10, -11, -9, 4, 5, 11, -4, -1, 2, -11, -2, -10, -1, 40, 24, 2, -7, 10, 8, 7, -1, -9, 0, -10, -5, 3, 7, 5, 1, 11, -3, 1, -1, 15, -1, -4, 16, 14, 0, -3, -7, 29, -6, 4, -13, 31, 4, -5, -3, -5, 12, -16, -4, 22, 10, -6, 14, 2, -3, 10, 9, 10, 30, 35, 10, -24, 7, 14, 12, 0, 0, 11, -15, 16, 6, 6, 9, 8, 2, -2, 3, 15, 8, -13, -2, 3, 11, 4, -10, 27, -19, -10, -8, -15, -20, 2, -6, 20, 4, -14, 4, -6, 10, -5, -9, 22, 5, 5, -6, -22, 8, 8, -12, -12, -3, 9, 1, -16, 18, -16, -17, -9, -10, -14, -2, -1, -4, 36, 11, 19, -6, 5, -2, 15, -6, 10, 12, 16, -1, 9, 6, 0, -2, -2, -26, -9, 19, 19, 10, -2, -9, 11, 2, -18, -2, 5, 16, -9, 14, 5, 18, -3, 3, -2, 1, -2, -18, 3, 7, 9, 1, 26, 4, 15, -1, -3, 23, -11, 8, -9, -3, 7, -7, 9, -13, 3, 1, -3, -8, 17, 27, 4, 5, 1, -1, -3, 13, -6, 18, -14, -3, 13, -11, -9, -21, -5, -1, 7, 20, -4, 2, -6, -21, 23, 3, 1, -21, -2, 11, -7, -29, 18, 13, -21, -5, -1, -3, -13, -21, -7, -4, -9, 2, -9, 2, -27, -3, -13, 14, 3, -6, -12, -2, -8, -7, 20, -6, -4, -3, -4, -20, 17, 54, -19, -25, 3, -11, 5, -4, 6, -24, -8, -23, 13, 35, -32, -4, -37, 15, -11, -5, 0, -11, 0, -8, 1, 12, -35, -9, 38, 20, 11, -8, 28, 14, -16, -4, 21, -16, 19, 7, 19, -34, -17, -2, 55, 22, 10, 9, -11, 20, -6, 11, 1, 4, 4, -15, 0, 29, -25, -4, -5, -20, -3, -7, -26, 5, -13, -27, -3, -3, 15, -5, 1, -7, 30, 7, -10, -8, 28, 4, -12, -16, 3, 12, -2, -9, 7, -4, 12, 5, -25, 7, 9, -9, -9, 19, -4, -10, -1, -35, -3, 5, -9, 13, -7, -24, 27, -11, 1, 5, 19, 0, 5, -21, -6, -6, 11, 10, 3, -21, 17, -6, -21, -17, -9, 14, -13, 9, 19, -23, 3, -8, -33, -17, -17, -7, 5, -15, 8, 11, 7, 18, -26, 5, 6, -1, 29, 7, 6, -6, -7, 9, 5, 3, -12, -12, -7, -12, 13, 0, 7, -3, -31, -7, -39, -21, -27, 2, 13, -1, 2, 13, 1, -13, 26, -2, -10, 20, -1, 22, -3, -14, 18, 6, -1, -2, -27, 16, 5, 4, -4, -25, -5, 15, -15, -17, -33, 17, 1, 6, 13, 1, 10, -2, 0, 2, 1, 2, -17, 6, 26, 2, 4, -6, -10, 12, 16, -9, 3, 2, -3, -2, 8, 14, 14, 1, 7, 30, -18, 8, -2, 11, 0, 31, -14, 29, -7, -2, -2, 9, -5, 30, 20, 5, -3, -3, 12, 9, 49, 16, 3, 24, -6, 17, -4, 7, 2, -7, 8, -8, -37, -21, -5, -3, 3, 14, -8, 13, 15, -21, 12, 9, -5, 35, -2, -1, -6, -13, 15, 37, 30, 16, 13, 22, 2, 8, -32, -11, 4, 8, -41, -20, 9, 0, -4, -3, 2, 23, 16, 3, 7, 6, -4, -7, 7, 27, -2, -11, -7, -26, -3, 20, -1, 1, -4, 17, 4, -10, 10, 8, -20, 18, -19, 4, -11, 8, 21, -2, 5, 5, 9, -3, -21, -20, -4, 7, 12, -1, 8, -2, 6, -10, 10, 7, -5, 4, -1, 10, 2, 9, -21, 4, -9, -8, 14, 8, 6, 16, 41, 7, 6, 4, 16, -1, -8, 9, 19, 0, 3, 7, -20, 18, 8, -7, 4, -3, 5, 8, -18}
, {5, -2, -25, 23, -1, -1, 0, -1, -11, 1, -2, 16, -14, -26, 6, 9, 0, 4, 3, 10, 7, 17, -7, -21, 9, 4, -26, 10, 12, 4, -18, -9, 30, -6, -7, 1, -8, -28, -23, -2, -3, 12, 5, 20, 11, -1, 13, 22, 1, 8, -14, 20, -6, -9, -8, 13, 31, 3, -19, 5, 18, 17, -2, -4, 28, -8, -26, -16, -1, -1, -4, -10, -8, 19, 6, 11, -16, -29, -26, 25, 0, 13, -2, 2, 0, -32, -13, 9, 44, 0, -16, -7, -22, 10, 7, -2, -1, 24, 10, 7, -6, 17, -9, 29, 13, 12, -5, 23, 4, -4, 14, -21, 23, 3, 14, -5, -5, -8, -6, 36, -5, -9, 13, 14, 7, 5, 19, 13, -24, 7, -5, 13, -9, 24, -2, 7, 27, -9, -9, -1, -4, -1, -6, -4, 14, -19, 23, -4, -1, 19, 10, 15, -34, -5, 18, 16, -24, 0, 3, 18, -2, 23, -2, 9, 5, 10, 8, 0, 4, -14, -5, -16, -14, -9, -2, -3, 12, 1, -5, 13, 0, 20, 5, 26, -36, -15, 1, 5, -8, 24, -7, -1, 4, 11, -25, -23, 2, 18, 5, -22, -6, -25, -5, 2, -3, -19, 5, -16, -18, -8, 2, 23, -11, 14, 0, -6, -40, -7, 8, 9, 11, -37, -2, -10, 0, 17, 32, 31, -12, 5, 21, -10, -14, 15, 12, -1, -12, -3, 10, -24, -25, 20, -6, -7, 2, 3, 13, -16, -26, 1, 3, 3, 2, -27, 15, -9, -18, 20, 14, 12, 2, 23, 19, 24, 24, 56, 37, 20, -4, 11, -5, 25, 22, 23, 19, 16, -4, 7, 3, -6, 16, -7, 30, 13, 1, 7, 19, 9, 7, -6, 5, 16, 11, -5, -29, 18, 11, -19, 5, 19, -10, -16, 24, 0, 22, -18, -7, 10, -8, 7, 4, 15, 7, 1, 2, 6, -8, 9, -1, 24, 3, -1, 15, 27, 6, 35, -3, 8, 6, 16, -1, -11, -7, -1, 42, 5, 5, 9, 13, 6, 8, 11, 39, 12, 5, -6, 22, 15, 18, 27, 7, 38, 27, -6, 5, -6, 9, -12, 7, -18, -32, 0, -12, 18, -4, 26, -13, -5, -8, -32, -6, 0, 3, -15, -13, 13, -2, -10, -20, -7, 14, 9, -14, 4, 10, -20, -4, 13, -21, -22, -1, -3, -1, 15, 6, -8, -15, 8, -19, 22, -32, -35, -18, -8, -8, -10, -35, -12, 14, -5, -7, -10, -4, 11, -10, -9, 10, -10, 15, 7, 14, 0, 10, -9, -4, 39, 23, -3, -3, 19, 5, 5, -3, -16, 8, 5, -4, 2, 0, -14, 21, -2, -9, 17, 25, 13, -11, -15, 11, 1, 2, 22, 0, -9, 33, 10, -8, 19, 14, -11, -8, -15, 12, 4, 7, 2, 8, 3, 7, 9, 34, 8, 0, -5, 13, 16, 28, 9, -5, -9, 0, 11, 8, 5, 24, 33, 3, 18, -26, 2, 1, 0, 3, 16, -15, 40, -33, -1, 14, -1, 0, 3, -3, 7, 21, -2, -1, 35, 1, -25, 15, 40, -17, 3, -32, -4, -30, -31, -35, -32, -20, -21, -21, -27, 2, -33, -18, -30, -15, -24, -36, 4, -6, -2, -39, -4, -9, -12, 1, 3, -14, -21, -17, -27, -27, -11, -11, -23, -2, -21, -7, -10, -3, -33, -35, -17, -13, -51, -26, -52, -22, 1, -21, 22, -10, -6, -20, -19, -18, -6, 2, -20, -18, -21, -15, -17, -20, -1, -25, -27, -2, -15, -8, -15, -25, 14, 4, 5, 1, -2, -18, 1, -33, -18, -19, 16, 8, -23, -23, 3, -1, 1, -3, 1, -7, -44, 3, -39, -19, 10, -15, 4, 1, -11, -2, -13, -29, 26, 1, -20, 4, -11, -24, 3, -39, -33, -41, -3, -7, -16, -33, -1, -1, -5, -10, 7, -19, -42, -10, -8, -13, -4, -2, 16, -28, -14, -38, 19, -1, -30, -44, -7, -5, -5, -14, -32, 21, -30, -21, -12, -8, -34, -25, -18, -3, -5, -11, -15, -6, 14, -8, 15, 26, -21, -27, -14, -23, -11, -19, -25, -8, -3, -19, -19, 5, -25, 24, -18, 25, 25, 0, -10, 6, -28, 3, -25, 25, 2, -25, -17, 31, 45, -3, -18, 18, -12, -7, -15, -19, -13, -37, -18, -24, -49, -41, -15, -7, 0, -34, -20, 22, -27, 4, -13, 7, -14, 19, -15, -4, -3, -12, -5, 27, 20, -13, 10, -25, -32, -34, -35, -27, -45, -38, -1, -28, -26, -56, -14, -5, -56, -49, -44, -51, -53, -40, 1, 1, -6, -54, -17, -22, -11, -17, -31, -28, -51, -36, -49, -17, -22, -31, -32, -22, -46, -31, -5, -34, -34, -33, -23, -2, -48, -28, -42, -20, -28, -59, -15, -11, -47, -39, -12, -29, -8, -41, -34, -21, -47, -21, -11}
, {-6, 14, 15, -6, -10, -7, -3, -2, 6, 42, 3, 16, -9, 16, -17, 19, -15, 2, 21, 0, -9, -1, -7, -8, -10, 4, 19, 1, -27, -11, -3, -10, 27, 5, -8, -7, 35, -14, -14, 27, 5, 11, 29, 15, -8, -9, -26, 57, 29, -28, -10, 8, 8, -2, -12, -1, 55, 1, 14, -15, -40, 1, -8, 20, 2, -30, -1, 24, 11, -13, 27, -2, 8, -29, 5, 1, -8, -1, 4, -14, 38, -6, 4, 11, -4, 5, 28, -11, 6, 14, -29, 5, 7, 37, 12, 7, 4, 24, 15, -7, 23, 4, 40, -16, 0, 6, -1, -26, 0, 32, 20, 8, -27, 9, -11, 14, 5, 0, 20, -7, -10, -2, 6, 18, 11, 17, 5, -11, -6, 9, 11, 15, 9, 32, -7, 9, 3, -5, 3, -26, 2, 14, 8, 32, 7, 23, 1, -24, 2, -6, 10, 21, 47, -10, -3, 37, -5, -11, 16, 8, -4, 4, -10, 1, -12, -13, -24, -21, -12, 10, -11, -23, -5, -25, 15, -6, -21, 5, -13, -7, -5, -25, 1, -35, -29, 9, -5, -8, -10, -11, -16, -21, 20, -12, -4, -16, -9, -21, 6, -9, -8, 6, -3, 3, 0, -17, 16, 14, -2, 7, -2, -10, 7, -5, 32, 6, -5, -5, -20, 2, -11, -11, -11, -18, 15, 2, -9, -2, -4, -6, 17, 7, -3, -4, 10, 37, -4, 31, 8, 14, -14, -30, 5, -1, -6, 8, -9, 6, 32, 0, -7, 21, -7, -26, -6, -34, 4, -12, -13, -3, 9, -6, 14, -4, 1, 14, 2, -15, 8, -12, -2, 0, 6, -28, -26, 21, -1, -1, -10, -16, -16, -1, -3, 14, -1, -6, -23, 8, -10, 12, -16, 0, 11, 0, 9, 14, 5, -26, -14, -22, -4, -11, 16, -1, -4, -38, -18, 1, 1, 4, -6, -17, -4, -8, 2, -3, -33, -11, -23, 13, 3, -7, -4, -11, -24, -30, -26, 18, 13, -1, -23, -1, 5, 3, 0, -16, -26, 16, 1, -16, 5, -4, 9, -1, -11, -4, -20, -23, 6, -6, 16, -15, -4, 5, 10, 7, -13, 15, -2, 12, 13, -4, 3, -7, 2, 21, -44, -5, -13, 13, 16, 3, -6, -7, -4, 2, -8, -6, 1, -4, 5, -23, 8, 15, -4, -32, -9, -5, -23, -13, -40, 18, 11, 2, 7, 6, -8, -3, -2, -1, -20, -22, -4, -2, 2, -10, -43, 0, 4, -3, 9, -20, -10, -13, -10, -3, -4, -35, -14, -25, -10, -9, -40, 2, 1, 4, 8, 0, -3, -23, 20, -5, -6, -1, -20, 10, -4, -14, -4, 2, -2, -9, -13, -32, 2, -25, -6, 17, 1, -26, -2, -14, -17, -16, -36, 5, 4, 8, 20, -2, -8, -14, 2, 2, -13, 1, -6, -7, -11, -18, -21, 6, 3, -4, -19, -35, -16, -26, 15, 23, -20, -2, -2, 10, 5, 9, -8, 22, 20, 2, 2, 13, 1, 17, -17, 8, 1, -4, 2, -22, -7, -9, -25, 5, -20, 1, 17, -5, -6, -4, -2, -4, -2, -27, -31, 7, -11, -16, -50, 17, -9, 12, -14, 5, 6, -18, 6, -3, 14, -28, -20, -3, -9, -25, -11, -17, -7, -10, -28, -21, -3, 8, -25, 7, -6, -20, -26, -18, -26, -21, -13, -5, -9, -4, -20, -2, -10, -19, -15, -31, -16, 2, 6, -6, 9, -11, -18, -17, -39, -5, -5, -48, 7, 0, 4, 10, -14, -11, 14, -11, -29, -14, -37, 3, 13, 4, -1, 28, 3, -13, -4, -28, -4, 1, 21, -2, -3, 11, -3, 23, -26, 6, -1, -22, 15, -6, -5, 9, 2, 4, 3, 14, -13, -14, -32, -7, 3, 21, -17, -14, -11, 2, -16, -9, 16, 14, 17, -23, -4, -1, 0, -16, -17, 0, -3, -19, -10, 1, 19, 7, -15, -9, -6, 5, -4, -17, -2, -13, -12, 1, 7, 4, -13, -24, -10, 20, -32, 0, 3, 5, -5, 14, -11, -43, -8, -6, 16, 2, 22, -20, -19, -27, 12, -3, 1, -9, 4, -20, 19, 0, 3, 4, -14, -14, 11, -12, -14, 2, -31, -8, -9, 15, 6, 8, -9, -8, 0, -4, 10, 2, 0, 0, -17, 0, -9, 2, -3, 24, 4, -27, -17, 12, 12, 5, -16, -4, 15, 4, 2, -18, 1, 3, 2, -7, -1, -6, -1, 12, -19, 9, 14, 9, -8, 13, 3, 19, -11, 22, -13, -8, 10, 7, 6, 7, 0, 12, 9, -7, 1, -7, -15, 13, 10, 5, 16, 12, -6, 35, 8, 6, -12, 3, 33, 9, 25, 14, -3, 2, 1, 23, 6, 2, 4, 15, 67, -16, -9, -35, -13, -22, 3, 15, 13, -16, -9, -20, -23, 6, 5, 25, 11, 10, -10, 0, 9, 14, 18, 8, 0, -14}
, {-7, -4, -19, 8, 3, -13, 6, 2, -22, 2, 5, 8, -11, 5, -21, -9, -24, -8, -21, -10, 0, -12, -12, -14, -2, 7, -6, -8, -5, -18, -3, 3, -2, -1, -6, 12, -27, 3, 6, 5, -4, -13, -26, -12, 0, 15, -16, -12, -8, -20, -27, -8, 0, -19, -18, 2, -13, 9, -17, -4, -15, -17, 5, -8, -15, -26, -7, 5, 21, 16, 29, -8, 30, -5, 6, 6, 1, -20, 23, 12, -38, -7, 14, -2, 4, -25, -9, 3, 0, 7, 0, -1, -22, -39, 11, -16, 3, -37, 28, 23, -13, 4, -14, -5, -6, 4, -18, -6, 7, -6, 39, 5, 15, -6, 21, -21, 5, -9, 10, -23, 25, 1, -34, 13, 18, 24, -20, 15, -4, -17, -26, -15, -11, -21, -30, -2, -6, -5, -3, 18, -7, -7, -8, 2, 12, -6, 7, 5, 3, -30, -1, -7, 12, 8, 3, -30, -23, 31, -7, -1, -8, 9, -5, 6, -11, -26, -6, 8, -13, -8, -8, -12, -7, -8, 11, -7, -17, -5, -22, 8, 4, -7, -17, -13, 1, 15, 4, -3, -16, -16, -2, -6, -15, -1, -40, -25, -17, -25, -29, -7, -4, -25, -17, 4, 6, -45, -16, -23, -21, -2, -29, 2, 0, -14, -43, -2, -20, 17, -5, -9, -19, -17, 2, 2, -22, -2, 4, -27, -23, -20, 9, -1, -1, -4, -13, 0, 4, -9, -13, -7, -29, 7, -6, 9, -5, -12, -10, -12, -29, 10, -9, -23, -23, -19, -12, -4, -30, -6, 21, -18, 0, 4, 52, -5, 14, 30, 9, -24, -8, 21, 16, 15, -6, 3, 19, 10, 9, 18, 46, -27, 27, 2, -2, 7, -4, -48, -19, 4, 8, -9, 0, 25, -13, -19, 22, 6, 1, -15, -5, 5, 5, -12, 11, -17, 3, -9, -7, -1, -8, 23, 7, 5, -4, 11, -12, 27, 27, -11, -8, -25, -2, -1, -14, 5, 4, -6, 5, 4, 0, -22, -9, 0, -7, -12, -7, -22, -3, 7, -9, -1, -5, -11, -12, 4, -11, 7, -1, -15, 1, -13, -1, -11, -10, -4, -16, -11, 0, -9, -6, -4, 3, -9, -11, -10, 3, 3, 2, -10, -15, -9, -9, -5, 8, -8, 4, 9, -15, 4, 1, 4, -10, -5, 8, 6, -9, -17, 2, 5, -18, -16, -35, -7, -12, -19, -36, -8, -1, -18, 0, -24, 0, -13, -29, -3, 8, -7, -21, -2, -17, 7, -13, -27, -18, 6, -23, -1, 17, -34, -6, -25, 0, -32, 5, -12, -25, 15, -8, -18, 9, -14, -17, 3, -9, -7, -3, 5, -1, -41, 4, -18, 8, -2, -25, -29, 3, -5, -22, -28, 0, 2, 0, -21, -1, 13, 8, -5, -9, 0, 4, -8, 9, 2, -14, 10, -25, -6, 7, -4, -3, -20, 2, 5, 16, -2, -11, -6, 3, -13, -6, 1, -11, 3, -3, 1, 6, -5, 13, 6, -7, 2, 3, -1, -9, 4, 22, -1, -11, -6, -1, -5, 8, -4, -9, 10, 15, 1, -11, 3, -5, 10, -5, -2, -3, -7, -15, -17, -4, -2, -14, 0, -17, -16, -13, -12, -10, -12, -38, -15, -18, 2, -18, 7, -2, -4, -17, 6, -21, 1, -9, 5, -18, -30, -10, 18, -3, -25, -2, -13, 14, -7, -12, -13, -16, -10, -12, 1, 3, -13, -3, -11, -26, -14, -8, -3, -2, -14, -22, -22, 8, 2, -14, -30, 7, -18, -17, -38, 2, -14, -5, -20, -24, -18, -19, -19, -20, -20, -18, -8, -8, -12, -20, -23, 15, 3, -37, -6, -3, -4, -3, -19, -10, 1, -23, -8, -1, -2, -1, -11, -1, 1, 3, -30, 7, -2, 19, 6, -1, 17, 8, 3, -1, -4, 4, -5, -30, -12, -7, 7, 6, -7, -13, 11, 3, 1, -12, -9, -8, -30, -1, -7, -4, 7, -18, -7, 7, 12, 16, -3, -7, 10, 17, 8, -9, -4, 3, 8, -20, -10, 10, 7, -2, 6, 15, 6, 9, 3, -9, -9, 4, -24, -14, -1, -13, -11, -2, 5, -15, -13, -16, -11, -34, -12, -24, -14, -3, 12, 29, -1, 12, -22, -6, 9, 8, -16, 6, -16, 15, 1, -3, 1, 40, 9, -17, -17, 6, -30, 31, 1, -14, -16, 22, -27, 1, 11, 11, -22, 3, -11, 18, -11, 33, 9, 32, -5, -3, -17, 45, -18, -1, 3, -24, -34, 28, 27, -1, 17, 2, -11, 30, 7, -18, -1, -9, -5, -14, -21, 2, 14, 0, 25, 3, -8, 37, -12, 22, 3, 0, -21, 29, -17, 17, 7, -19, 8, 28, 41, -6, 3, 18, 13, -4, -20, -8, 10, -4, -20, -1, 3, -10, 6, -6, 0, -25, -6, 18, 7, 1, -9, 10, 2, -1, 3, 16, 4, -6, -19, -24, 28, 15, -12}
, {9, -28, -2, -5, -23, -16, -2, -13, -12, 4, -14, -11, 2, -15, -2, -9, 2, -5, -11, 23, -1, -7, -12, -45, 17, 8, -41, 9, 20, 27, -5, -14, 50, -32, -12, -7, -13, -41, -24, -18, -6, -3, -11, -13, -8, -7, 25, -1, -1, -10, 11, 13, 9, -17, 10, -30, 4, 2, -22, -14, 16, 35, -9, 2, 2, 0, 26, 13, 10, 13, 21, -13, -23, -18, -15, -15, 12, 21, 16, 5, -6, 21, 1, -15, -3, -5, 8, 5, 10, 7, -12, 3, 1, 8, 6, -3, -20, 13, 11, -5, 6, 15, 4, 1, -13, 7, 7, -4, 16, 4, -31, 2, -14, 7, -3, -23, -4, 14, 1, 0, 5, 6, 2, 24, -9, -18, 8, -37, -16, 3, -2, -7, -1, 15, 1, 11, -10, 4, 1, 5, 13, -9, -25, 24, -10, -1, 1, -10, 9, 7, -21, 18, 12, -6, 24, 6, -10, -43, -1, 8, -9, 14, 14, -1, -5, 9, -7, 8, 5, 0, -11, 2, 1, 22, -21, 0, 8, -20, 0, 8, 2, 1, 13, 28, -14, -7, -8, 2, -24, -6, 3, -3, -28, -9, 0, -22, 2, -1, 7, 0, -12, 10, -12, -2, 14, 16, -19, 5, -15, 6, 15, -26, 7, 14, 1, 2, 2, 8, 3, 19, -5, -8, -3, -16, 2, -10, 15, 13, 2, 4, 18, 7, 3, -16, -1, 5, 3, 15, 0, 14, 11, 4, 17, -27, 7, -4, 2, 0, -2, 3, 11, 18, 12, 13, -13, 22, 9, -1, -16, 11, 4, 11, -7, -9, -18, -17, -3, -28, 7, -2, -10, -2, 9, -17, -14, -23, -1, -6, -5, -41, -8, 4, 20, 10, 13, 26, -23, -15, 25, 9, -10, -21, 23, -8, 15, -18, -18, -43, -8, -36, 9, -23, 8, 4, -11, -23, -33, -15, 7, 6, -14, -10, -6, 9, -14, -7, -1, 12, -34, -2, 10, 9, 18, 19, -15, 20, -3, 14, 4, -13, 5, 1, 2, 7, -4, 2, 19, 13, 21, 8, 8, 7, 6, 20, 18, 7, 3, -14, -2, -2, 1, 22, 1, 14, 15, 7, -13, 5, 5, 2, -8, 1, -3, 1, -4, -3, -7, 4, 23, -5, 4, -6, -4, 20, 17, 3, 15, 3, -1, -5, 2, 15, 3, 4, 8, -16, 9, 7, 0, -23, 4, -5, -1, 6, 2, -3, 8, 17, -1, 6, 3, -2, -14, 6, 1, 19, 0, 4, 5, -8, 0, -15, -1, 8, -18, -12, 12, -30, -8, 4, 2, -44, -13, -9, 5, 0, 3, -22, 12, -15, 11, -5, 7, -13, -21, 17, -3, 1, 4, -22, 3, -6, -14, -28, 30, 37, -19, -8, 41, -29, -15, 1, -15, -22, -6, 7, 7, -18, -2, -7, 1, -5, 20, -5, 8, -24, -23, 2, 2, -2, -3, -24, -19, -8, -13, -23, 9, 53, -10, 10, -5, 2, 9, 23, -2, 14, -3, -4, -4, -20, -15, 6, 16, 15, -23, 11, 28, 9, 14, 3, -8, 3, 4, 20, -4, -5, 26, 14, 5, -5, -1, 31, -9, -19, -2, 5, 11, 4, 12, -3, -3, 18, 3, -8, 5, -6, 0, -12, 2, 5, -7, -3, -1, -8, 14, -14, 16, -7, -27, 15, 0, -12, 6, -2, -9, 1, -10, -2, 0, 6, 12, -4, 11, 9, -17, 4, 9, -5, 15, -11, -5, 12, -21, 25, -4, 9, 3, -15, 9, 8, -4, 3, -8, -7, 3, -11, 4, 3, -6, 4, 11, 10, 27, 19, 8, 16, 16, 34, 11, -4, 2, 10, -19, 14, 14, 9, -1, 14, 3, 7, -2, -5, 7, -9, -4, -1, 29, -1, -11, -5, 2, -9, -2, 8, 38, -9, -4, 3, 14, 26, 9, 8, 10, -4, -14, -23, -3, 45, 8, 7, -5, 12, -12, 2, 4, 25, -12, -14, -5, -6, -5, 0, 10, 18, -7, 11, -10, 17, 13, -33, -4, 14, 13, 21, 11, 11, 39, 2, 14, 35, -6, 15, 18, 15, 6, -10, -4, 3, 5, 24, 16, 28, 3, 14, 3, -9, -43, -14, -2, -6, -12, -3, -30, 7, 11, -14, -7, -30, 2, 13, -11, -8, 5, 7, 4, 17, 6, 8, 0, -13, -12, -3, 19, -28, -14, -2, -11, -6, -3, 14, 4, -6, -14, -5, -8, -15, 11, 6, -11, 14, 0, 1, -9, -4, -2, 0, 3, 4, 1, 1, 0, 6, -9, -3, 4, -18, -2, 12, 9, 14, 19, 22, 17, -8, -5, 5, 2, 6, 20, 6, -11, -2, 0, -22, -7, -28, -1, -15, -14, 22, 11, 1, -9, 13, -33, 2, 15, 1, 1, -3, -14, 18, 12, -11, 4, -6, -20, -16, 6, -1, 1, 2, -4, -3, 4, -19, -22, 1, -3, 6, -21, 21, 10, 6, -2, 24, -24, 0, 1, -18}
, {-10, -12, -17, -3, -18, -26, 1, -21, 2, 6, -1, -35, -10, -13, 5, -26, -34, 28, -11, -18, -5, -13, 9, 9, -10, -12, -21, -40, -19, -18, 8, -42, -35, 25, -7, 1, 16, 16, 26, 8, -12, -1, 11, -1, 5, 15, -16, -3, 2, -7, -9, -28, -4, 5, 10, 14, -17, -17, 1, 13, -30, -10, -8, 8, -10, -2, -10, 0, -29, -10, -14, -15, -6, 22, -3, 7, -19, -1, 23, 12, 5, 5, 4, -10, -9, 12, 19, 3, 3, 2, -11, -22, -8, 21, 1, 6, 31, 13, 7, -7, 16, 5, 18, 18, -7, 3, 7, 10, -6, 4, 10, -4, -6, 15, 15, 6, -2, -8, 3, 10, 14, 0, 3, -20, 0, 17, 3, 0, -19, -16, 6, -1, -1, -9, -13, 8, 18, -17, -5, -19, 10, -4, 28, -21, -4, -3, 8, 3, 3, 4, -5, -12, -2, 1, -8, -25, 2, 5, -2, 11, -13, 0, -1, -12, 1, 6, -19, -37, -9, 1, 7, -19, 0, -5, 14, 3, -12, -1, 0, 3, -12, 16, -10, -1, 11, -14, -6, -3, -4, 5, -5, -41, 11, 0, -8, -5, -11, -12, 14, 16, 11, -10, 12, -1, 4, -20, 23, -13, 3, -9, -14, 22, -12, 12, 6, 27, -8, -1, -7, -16, -9, 13, -2, 9, 7, 7, 1, -3, -6, 0, 9, 10, 6, 8, 3, 14, -13, 7, 8, 11, 3, 15, 5, 24, -5, 4, 9, 17, 3, -3, -13, 13, -26, -1, 13, -12, -5, 10, 9, 26, 16, 14, 6, -4, 0, 6, -1, 1, -9, 7, 3, -3, 10, -8, -10, 16, -5, 2, 11, 22, 8, -12, -12, 2, -8, -14, -14, 11, -16, -33, 6, 15, -10, -1, -8, 9, 17, 19, 18, 2, 12, 15, -9, 1, 2, 10, 10, -15, -10, 4, -31, -11, -4, -8, 8, 2, -23, -14, 11, -5, 6, 8, -26, -12, 3, -14, 20, 4, 21, -4, 15, -10, 0, -17, 17, -6, -12, 10, 9, 0, 2, 3, -7, -4, 2, -8, -10, 4, 10, 4, -1, 13, -3, -1, -7, 8, 7, -9, 19, 15, 16, 20, 15, -1, -6, -9, 5, 4, 5, -1, 21, 5, -3, 10, -1, -5, 0, 6, 10, -15, -1, -4, 0, 15, 18, 5, -12, -5, -1, 4, 8, 23, 19, -1, -13, 13, -3, -3, -9, 5, 14, -8, -9, 22, 5, 6, -10, -4, 2, -6, 9, 2, 11, 6, -16, 22, 13, -6, -8, 1, -1, 6, 5, -15, -11, -1, -7, -15, -7, -4, -10, 3, -22, 4, -2, -15, -6, -4, -12, -6, 3, 1, 1, 26, -14, 0, -12, -20, -14, 0, 2, 8, -35, -4, -36, -1, 15, 13, -3, 25, 0, 7, -16, -4, -18, 19, 2, 2, -12, 0, -8, 5, 7, -10, 2, 2, 1, -28, 20, -3, 7, -7, 1, -12, 5, 3, 9, 9, -3, 15, 18, -7, -8, -2, 9, 6, -12, 8, 4, -16, 2, 6, -16, -9, 2, -11, -14, 3, 11, 17, 12, -11, 18, 16, 10, -9, 8, 5, 4, -5, -18, 21, 21, 4, -2, 5, 3, 33, 8, -7, 7, -7, -1, 20, -21, 5, -12, -13, 12, 6, 17, -4, 5, 5, -19, 2, 7, 7, 28, -6, -22, -16, -14, -14, 0, -10, 3, -12, -14, -11, -2, -18, -4, -5, -6, 13, -19, -9, 7, -11, 11, 3, 12, 3, -29, 0, 4, -11, -27, -2, -20, -18, -18, -33, -5, 0, -16, -12, -4, 1, -5, -16, -5, -19, 1, -7, -1, -3, -6, -33, -13, -5, 3, -12, 3, -12, -17, -8, -8, 6, -4, -2, -7, 1, -23, 12, 8, 13, -16, 9, -7, 0, 4, -19, -10, 9, 13, -33, 8, -26, -14, 16, -9, -14, 14, -12, -4, -4, 14, 3, -10, -7, -16, -10, 13, -4, 9, -25, -14, 6, 4, -13, -5, -7, -10, 12, -28, -2, -9, -16, 3, -11, 0, -20, 19, -12, -24, -6, -3, 0, -1, -35, 27, -34, -7, -5, 13, -4, -15, 20, 21, -19, -11, -5, -5, 14, 8, 25, 21, 13, -23, -14, -12, -38, 11, 20, 4, -11, -20, 7, -11, 30, 31, 13, -25, -2, -1, -1, -12, 14, 18, 31, -9, -25, -30, -7, -6, -2, -7, -6, 27, 21, -1, -18, 3, -10, 13, 25, -9, -1, -18, 12, -41, 8, 16, 38, -10, -14, -5, 19, 7, -3, 10, -12, 8, 4, 17, -7, -4, -24, -11, 2, -2, 2, 5, 10, 2, 14, -25, -10, 3, -6, -3, 15, 10, -14, -21, -29, -4, -1, 24, 2, -7, -4, 4, 12, 20, 10, 11, -6, -19, 15, 14, 0, 15, 9, 18, -2, -9, -2, -1, -11, -26, -14, 7, -30, 13, 9, 5, 28}
, {20, -16, -7, 22, -2, 4, 8, 8, 20, -3, -3, 30, -3, 8, -13, -6, 15, -11, 3, 13, -7, 18, -18, -6, -4, 5, 2, 5, 5, 11, -11, 54, 26, -17, -3, -20, 7, -18, -8, -7, 22, 38, 10, 3, 7, -12, 27, 15, 4, -5, 3, 24, 6, 2, 8, 2, 0, -6, 6, 2, 25, 6, 7, 11, 21, -7, 0, -5, 30, -2, 11, 13, 4, 15, 17, -60, -4, -14, 6, -3, -3, 6, -6, 18, 3, 1, -2, -42, -5, 5, -2, 28, 14, 5, -9, 15, -2, 0, 17, 21, 15, 12, -22, -22, -12, 9, 11, 12, 6, 1, 13, 2, -2, 17, 31, -9, 7, -5, 5, -24, -5, -3, 13, 1, -4, 3, 13, -4, -1, 7, 9, 6, 5, 24, 3, 10, -3, 1, -10, 32, -1, 24, -31, 13, -8, -3, 26, -16, -1, -8, -7, 27, 17, -5, 22, -7, 2, 2, 0, -17, -3, 7, 12, 13, -2, -2, 5, 24, 23, 2, 8, 35, 7, 2, 5, -9, 12, -3, 19, 48, 0, 13, -9, -13, -17, -8, 18, 13, 1, -12, -3, 30, -21, 23, 9, -4, 7, 7, 9, -14, -19, -5, 11, 0, -3, -7, 7, 27, -17, 7, 9, 21, 7, 28, -11, -5, 3, 3, 23, 4, -12, -15, -5, -16, -13, 1, 4, -10, 19, 4, 1, -3, -17, -8, 2, -20, -1, 7, -30, -4, -3, -8, -1, -4, 0, 12, 13, -11, 12, -7, -1, 7, -2, 0, 4, -21, -10, -9, 28, -7, -12, -5, 8, -22, -4, 14, 16, -9, -7, 38, -14, 16, -16, -5, 3, -13, -9, -6, 11, -4, 16, 6, -10, -2, -24, -2, -2, -26, -20, -4, -3, 8, -7, 12, 12, -6, -20, -4, 16, -22, -7, 10, -1, -2, 22, -17, -13, -10, -8, 0, -5, -2, 8, -8, 24, 3, 7, 6, -10, 4, -1, -1, -16, 11, -3, 5, -15, 9, 9, -9, 8, 15, 7, 8, -20, 4, -3, -12, -19, 12, -2, -1, -30, 5, 1, 6, 14, -3, -11, -3, -5, 7, -14, -32, -2, -12, -21, -3, -17, 1, -8, -6, -12, -2, -5, -6, 15, -16, 11, 6, -14, -17, 4, 1, -7, 10, 5, -14, -13, -28, 8, -15, 11, 9, -4, -8, 11, 1, -5, -4, 8, 2, -8, -10, -20, -23, -10, -7, -9, -25, 2, 9, 10, -10, 6, -4, 9, 9, 0, -5, -27, -4, 1, 2, 10, 7, 0, 5, 4, -6, 17, 10, -9, 16, 5, 29, 0, 6, 12, 16, 2, 6, -11, -1, 15, -1, 1, 7, -4, -12, -2, -8, 11, 5, -9, -20, 0, -4, 16, 14, 22, 3, 1, 3, 5, 14, -15, 25, 0, 6, 13, -18, 5, 9, 16, -12, -7, -10, -11, 7, 3, -14, -7, -15, 14, 7, -5, 5, -9, 5, -18, -5, -19, 17, -25, -10, -23, -14, -10, 0, -9, 12, -8, -20, 2, -12, -1, -20, -6, 5, -4, -10, -4, 15, -11, 1, -16, -19, -10, -5, -1, -10, -13, 22, 1, -22, 14, 6, 17, -3, 16, 8, 6, 5, 8, 11, -10, -15, -21, 12, 5, 28, 8, 16, -15, 1, -6, -5, 7, -5, 7, -12, 15, -34, 14, -2, -1, 17, -10, 8, 17, 17, 7, -9, -6, -1, 5, 4, -2, 10, 13, 0, 12, 10, 4, 11, 11, -21, -6, 5, -10, 3, 14, 11, -3, -5, 3, 25, 21, -10, -3, 14, 29, -4, -15, -2, 5, 13, 1, -6, 2, 6, 6, 26, 5, -7, 3, 5, 2, 6, -5, 4, -9, -9, -11, 6, 19, -1, -4, 12, -17, -7, 16, 14, 19, -16, -2, -4, 3, -2, -4, 17, -1, 21, -14, -17, 9, 0, 1, 20, -14, 3, 2, 3, -3, 43, -9, -16, -6, -20, 13, -18, -16, 20, -14, -11, -22, 8, 28, -12, -13, 26, 7, 11, 7, -14, 46, 18, -9, 4, -2, -30, 11, 34, -15, -12, -6, 1, -5, 27, 31, 26, 5, -3, -3, 31, 11, 20, -9, -1, -3, -9, -25, 1, -5, -6, -19, 7, 2, 14, -1, 7, -3, -36, -5, 22, -9, 0, -20, 1, -29, 21, 3, -6, -19, -2, 15, 19, -1, 25, -6, 9, 14, -14, -20, -19, -2, 10, -12, -18, -19, 29, 7, -10, -2, -32, 5, -10, 6, 5, 4, 1, -14, -5, 17, -7, -26, -29, -10, 7, -24, -16, -19, 6, 3, -22, 5, -12, 12, 15, -10, -5, -1, 2, -21, 6, 2, -53, -1, -13, 4, -10, -39, -19, -7, -22, 13, 19, -2, -39, -40, 9, -9, -25, -53, -5, 7, -26, -15, -13, -6, -10, -11, -10, 2, -8, -21, -10, 5, -57, -19, 15, -8, 7, -37, -12, -21, -9, -16, 15}
, {0, -7, 7, -17, 31, 4, 10, -22, -14, 13, -2, -4, -6, -17, -6, 37, -22, 5, 10, -14, 1, -2, 10, -2, 16, -3, -19, 29, -8, -8, 11, -46, 10, 38, 9, 26, 37, 8, -7, 9, 8, 20, 28, 19, 17, 29, -9, 32, -24, 45, 34, -36, -5, -6, 10, 27, 33, 6, 19, 24, -9, -28, 37, -32, -21, 12, 14, 26, 2, 25, -9, 21, -1, -40, -20, 3, 0, 18, -19, 6, 24, -8, -24, -10, -5, 15, -9, 24, 37, 9, 12, 14, -7, 9, 5, 29, -39, -16, -33, 3, 7, -29, 16, -33, -3, 4, -19, -18, 2, -23, -13, -35, -12, -6, -35, -22, 0, -24, 13, -4, -41, -7, -25, -7, -14, -21, -12, -29, 8, -8, -12, -6, -9, -6, -8, -19, 8, 5, -4, -16, 6, -10, -21, 3, 3, -11, -15, -7, -7, -11, -22, -9, 26, -5, -3, 1, 5, -12, -13, -2, -16, 26, 16, -7, -1, -13, 17, -8, 21, 1, 2, -4, -7, 19, -10, -2, -10, 2, 9, 2, 9, 12, 20, 54, 7, -4, 15, -1, 13, -19, 2, -5, 11, 20, -6, 12, -9, 8, 2, 5, -3, -6, 0, -18, 11, 19, -32, -7, -9, 30, -7, -30, -8, -12, 23, 29, 19, -2, 11, 4, 21, -12, 28, 8, -10, -17, -12, 16, -18, -6, -24, 6, 16, -8, -39, -17, -11, -21, -20, -11, 34, -10, -32, -15, -11, -29, -33, 8, -21, -5, -11, -27, -10, 1, -22, 25, -4, -5, -19, -12, -27, -42, -24, -6, -15, -17, -34, -3, 2, -25, -4, -20, -16, -11, -19, -6, 4, -27, 6, -12, -20, 6, -11, -28, -19, -10, 6, -23, -6, -8, -11, -31, -1, -13, -8, -2, -1, -13, -5, -3, 3, 13, -15, -26, -12, 1, 0, -8, -10, -8, 0, -13, -20, -3, -15, -9, -11, -3, -13, 4, 2, -19, 0, -17, 0, 4, -29, -13, -11, -5, 1, -15, 3, -12, -13, 0, 22, -6, -18, -16, 8, -16, -7, 3, 9, 6, 5, 2, -3, 2, -12, 2, 15, -26, 3, -12, 1, 8, 18, -15, 1, 17, 17, -13, 7, 10, 2, 5, -6, 13, 7, -12, -2, -11, 11, -20, 2, 6, 1, 11, 7, 0, -2, 15, -35, -28, -10, 24, -6, -6, -14, -6, -7, -40, -16, 5, 11, 25, -5, -19, -9, -14, -8, 8, -4, -25, -7, 5, 1, 1, -2, 23, -25, -29, -13, 0, -21, -9, -17, -23, -26, -16, -12, -15, -32, -13, -25, -8, 0, -7, -29, -33, -4, -22, -18, 3, 8, -10, -10, -3, -27, -8, -12, -11, -19, -12, 5, 5, 0, -3, 3, 3, 10, -9, -12, 3, -15, -13, 3, -8, 11, 9, 18, 7, 1, 12, 19, -6, -4, -8, 4, -10, 4, 9, -2, 5, 13, 2, -8, -5, 2, -16, 3, 5, -7, 5, -11, -19, -14, -24, -33, -17, 7, 8, 1, -13, 7, -1, -7, -14, -7, -13, 10, 1, -1, -8, -10, -9, -4, -4, -10, -17, -21, 3, 14, 4, -15, -10, 1, -20, -9, -9, -4, -5, 2, 12, 2, 5, -17, -18, 8, -2, 2, 14, 1, -5, -21, 3, -7, 11, -9, -3, -5, -11, -16, -28, 7, -17, -1, -19, -37, -6, 4, -27, -9, 6, -5, 6, 5, -14, -14, -8, -11, 14, 2, 26, -10, -7, 1, 0, 25, -2, -23, -3, -35, -1, -6, -7, 0, -3, -16, -20, 0, -2, -14, -36, -17, 13, 0, -7, 10, -34, -4, -1, 0, 44, -4, -5, 10, 16, -6, -1, 11, -10, 0, -21, 18, -5, -17, 11, -23, 5, 3, -11, 1, 19, -5, -36, 4, -38, -4, -4, -5, -1, -5, 2, -26, 25, 9, 17, 0, 20, -8, 10, 14, -11, -13, -13, -23, 3, -27, -24, 3, -14, -45, -22, -28, -8, -6, -18, -16, -19, 3, 16, 19, -24, -24, 0, -34, 19, 5, -42, -23, 13, -12, -6, -15, -37, -31, -18, -5, -21, -12, -31, -17, -10, 2, -21, -19, 1, 13, -5, 23, -38, -7, -21, 18, -10, -9, -16, 5, -4, -4, -23, -14, -6, -11, 8, -21, -27, -36, -6, -41, 15, 16, -19, 5, -4, -15, -36, -18, 33, 40, 29, 2, 10, 1, -1, 12, -22, 25, -2, 12, -13, -6, -32, 10, -2, -22, 7, -13, -11, 31, 18, -14, 31, 28, -2, -7, 12, 7, 11, -21, 17, -13, 7, -32, 21, 13, -4, -20, 0, 49, -5, -15, 13, 3, 2, 5, 30, 3, -3, 17, 7, 14, 14, -2, 22, 9, 5, 0, 10, 25, 0, 30, -10, -17, -6, 4, -3, -3, -30, -23, -17, -13, -18, -1, -14, -6, 21, 5, -30, -9, -11, 0, 4, 14, 19, -15, -25}
, {7, -28, -12, 1, 20, 17, 0, -2, 3, -4, 7, 17, -16, 11, 1, 22, 15, 3, 0, 19, -5, -13, -11, -6, 36, 3, -17, 37, 14, 3, 9, -1, 41, 3, 3, 6, 10, -7, -5, 6, 5, 9, 19, 18, 9, 14, 18, 19, -13, 5, 8, 12, 4, 18, -1, -10, 15, 6, -6, 0, 21, 15, 7, 0, -5, 3, 11, 8, 32, 10, 5, -5, 10, -21, -13, -26, -9, 0, -9, 4, -1, 28, 12, 7, 5, 11, -9, -10, -4, 1, 5, 12, -2, -15, 18, 13, -18, 0, 8, 12, 9, 9, 5, -8, -11, -16, -24, 3, -12, -17, 6, -13, 11, 16, 2, 10, -5, 47, -9, 19, -19, -1, 16, 9, -7, -11, 15, -10, 8, 7, 4, 21, 1, 13, -9, -24, -30, -16, -6, -5, -1, 27, -23, -9, 2, 8, 8, -11, 3, 16, 21, 3, -10, 8, 22, 29, 16, 12, -5, -19, -16, 12, -4, 5, -3, -11, 9, 27, 26, 1, -10, 32, -9, 16, -12, -2, 23, 4, 10, 17, -7, 12, 25, 24, -30, 3, 8, -16, -35, -15, 7, 29, -5, 4, -8, 19, 8, 15, -9, 1, 3, 7, 2, -7, 11, 8, -18, -1, -1, 13, 1, 4, 1, -7, -8, -3, 10, -2, -9, 7, -1, -16, 0, 9, -7, -1, 8, 20, 9, 17, -15, 10, 3, -13, 15, -33, 1, -9, -11, -2, -1, -9, -7, 6, -1, -1, -18, -3, 3, 3, 6, -9, 9, 19, 7, 27, 21, -22, 13, -30, 14, 3, 7, -8, 9, -6, -15, -19, 2, 23, 0, 24, 1, -12, -1, -17, 5, 15, -28, -13, -15, -7, 8, 24, 9, 15, -10, -3, 19, -9, -20, 18, 13, 7, 18, -37, -19, -38, -5, -38, 0, -3, 20, 4, -9, -35, -25, 6, -9, 21, 2, -27, 11, -9, -4, 10, 13, 13, -34, 1, -7, 22, 18, 10, 9, 1, -13, 5, -8, 2, 5, 0, -13, 21, -15, 15, 20, 7, 10, -6, 7, -2, -11, 6, 9, -1, 28, 1, -7, 0, 15, 13, -9, 6, 0, 11, 4, 22, 10, -31, -17, -1, -2, -18, -2, 2, -30, -6, -5, 9, 4, 2, 0, 4, -11, -9, -8, -9, -8, 11, -21, -11, 1, -7, 0, -5, 13, 22, 8, 14, -11, -12, -14, 2, 8, -14, 5, 3, -18, -2, -5, 13, 2, -21, 0, 0, -9, 4, -16, -11, -7, 12, -15, -10, 8, -10, 17, 1, 13, 7, -11, -1, -17, 12, 7, 23, 19, 13, -13, 5, 26, -5, 26, 8, 18, 6, 5, 5, 8, -3, 9, 6, -7, 6, 12, 19, 16, 3, 37, -7, -9, 12, 13, -22, -1, 7, 2, -9, -2, 9, 1, -11, 10, -16, 24, -10, -3, 1, -3, -5, 6, -24, -13, 11, -15, -6, -3, 33, -12, 20, -11, -1, -12, 4, -3, 9, -18, 15, -3, 1, -4, 0, -11, -1, 2, -6, -10, -7, -15, 10, 2, -13, 11, 5, -9, 11, 13, -4, 0, -13, -6, -2, -16, -7, -7, -8, -29, 5, -5, 7, 12, -14, -28, -30, -4, -12, 9, -31, -6, 1, -12, 5, 7, -10, 10, -8, -23, -1, -26, -28, 7, -3, 0, -6, -8, 6, 16, 6, -9, 14, 31, 13, 10, 20, -10, -9, -8, -2, 3, 15, -17, 16, 1, 2, -3, 8, 26, 3, 9, 2, -5, -20, 0, -6, 8, -20, -14, 12, 2, 8, -12, 8, 6, -9, -5, -7, 19, 22, -7, 8, -4, 10, -10, 24, -3, 0, 0, 3, 9, 8, 6, 0, 11, -4, -11, -12, 11, -8, -2, 7, -5, 5, -22, -7, 22, 11, 3, 2, -1, 16, -7, 0, 4, -4, -7, -11, -5, 40, 4, 10, 4, 10, -28, 9, -1, -8, -8, -2, -3, -11, -16, -10, 5, -9, -5, 0, -15, 15, 14, -17, -22, -6, -11, 6, -7, -8, 4, -8, -5, 14, -5, 7, 3, 12, -2, -4, 13, -11, -16, 0, 15, 22, -1, 8, -6, -20, -5, 7, 1, -9, 0, 33, -6, -14, 6, 2, -5, -15, -17, -8, 18, -19, 6, -7, -21, 8, 7, 10, -1, -21, -14, -10, -9, -17, -20, 2, -9, -4, 1, 12, -28, 6, -16, 7, -11, -16, -10, 0, -27, -9, 4, 1, 0, 4, 5, -18, -2, -14, 11, 10, 8, -3, -7, -11, -4, -8, -3, 14, -9, 1, 14, -8, -13, -3, -15, -7, -24, -19, 2, -4, -26, -16, -9, -27, -10, -24, -7, -26, -22, -11, -22, 1, -22, -12, -20, -9, -25, -4, 20, 18, -33, -24, 17, 0, -2, 0, -31, -14, -14, 13, 4, -17, -27, -6, -12, 6, -15, 10, -2, 12, -19, 11, 7, 5, -11, 12, -16, -5, 11, -39}
, {-1, -9, -1, -12, 3, 3, 0, -1, -7, 6, -10, 4, -2, 3, -8, 1, -10, -7, 3, 2, 7, -6, -6, -8, -14, 8, 5, 4, -4, 6, 7, -4, 0, -10, 3, -6, -12, -9, -9, -5, -11, -7, -4, -11, -2, -9, -11, -10, -9, 1, -7, -7, -12, 5, -6, -4, -5, 8, -2, 4, -6, -4, -10, 4, 7, -4, -8, -7, 2, 3, -4, -1, -10, 1, -12, 7, 8, 0, 7, -1, -1, 0, 5, -2, 1, 6, 2, 6, -7, -2, 0, -2, -15, -6, -9, -11, -10, -10, 2, -4, -9, 3, -2, 1, 3, -12, 6, 5, 2, -12, -3, -11, -5, 1, -3, -12, -2, 5, 1, -8, -1, 7, -9, -9, -6, -2, 2, -1, 7, -6, -8, -4, 4, -4, 1, 2, -4, -3, -4, 4, -11, -6, -9, -1, -9, -3, -8, -6, 2, 1, -8, -10, -6, 3, -9, -11, -9, -3, 5, -1, 8, -9, -4, 2, -2, 5, -3, 8, 8, -11, -3, -2, 4, -8, 5, 3, 5, -5, 4, -6, -7, 6, -2, 0, 3, 5, 0, 5, -2, -4, 2, 5, -6, -10, 8, 3, -11, -11, -12, 7, -10, -8, -7, 2, 6, -4, -7, 5, -12, 8, -6, -2, -1, -8, 3, -10, -10, 8, -11, 8, 2, -9, 7, 5, 1, 4, 3, -4, 7, 1, -6, 7, -10, -4, 6, -5, -6, -4, 1, -5, 7, -10, 5, 1, -1, 7, 4, -8, -3, -5, -13, -12, -15, -12, 3, 3, -9, 1, 8, -10, 2, -4, 3, -3, -10, -8, 3, -2, -7, 2, 5, -8, -13, -10, 0, -11, -5, -2, -10, -6, 1, 6, -5, 5, -12, -5, 5, 5, 4, 6, -6, -4, -9, -9, -12, 0, 2, -9, -7, 4, 8, 5, -7, -8, 5, -3, 5, -7, -2, -9, -7, -2, -8, -6, 8, -5, -12, 0, 8, 5, -12, 9, 3, -6, 8, 2, -12, -5, -10, -3, -5, 1, -2, -4, -7, -5, -1, -2, 1, -1, -10, -8, -5, -6, 0, 4, -6, -11, 5, -2, -10, 1, 5, 5, 0, -2, 1, 9, 8, -9, -5, -2, -11, 6, 0, 5, -11, -3, 3, 6, -10, 4, 4, 0, 8, -2, -8, -11, 4, 1, -16, 4, 1, 2, -1, 7, -11, 4, -7, -5, 3, 7, 6, -2, -13, 2, 6, 4, -8, -11, 0, -13, -5, -8, -11, 2, 2, 5, 3, -10, -13, 0, -17, -2, 6, -9, -6, -5, 1, -12, -9, 2, -15, -11, -5, -12, 2, 5, 9, -6, 4, -3, 4, 1, 0, 3, -10, 1, -5, -5, -16, -9, 2, -9, -9, -7, 1, -3, 5, 5, -2, 1, -7, 5, -5, -9, -8, -1, -1, 7, -9, -12, -8, -12, -2, -6, -9, -4, -7, -3, 5, 3, 0, -10, -2, -1, -2, -9, -1, 2, 7, -1, 2, -9, 2, -11, -6, 9, -3, 1, -8, 0, 5, -8, -2, 2, 4, -9, -8, -2, 6, -3, -5, -5, -15, -5, 4, -4, -12, 5, -3, 6, -3, 0, -7, -6, -5, 1, 7, -3, 0, -11, -9, -4, -1, -2, -2, -4, -6, -12, -10, 10, -11, 9, 0, 8, 4, -6, 0, 6, -12, 1, 8, 3, -9, 4, -12, -5, -13, -1, 2, -2, 1, 3, 2, 8, -6, 1, -9, -3, -4, -1, -7, 8, 5, -11, -5, -1, 0, 7, -2, -8, -3, -4, -2, -4, -12, 9, 4, -1, -10, -7, -11, 0, 1, 1, 2, 1, 1, 0, 6, 0, -1, 1, -2, 5, -9, -11, 2, 2, 0, 5, -8, 5, -13, -6, -1, 1, -10, 2, -5, -1, -11, 8, 4, 5, 9, -5, 9, 6, 5, 4, 7, -5, -4, 1, -7, -6, 0, -8, -3, 1, -10, -3, -10, -4, 4, -8, -4, -4, -7, 3, -2, 0, -8, 9, -7, -3, -11, 7, 7, 5, -2, 6, 4, -13, -10, 1, -2, 1, 8, 8, -8, -2, -6, -1, -5, 2, -9, -6, 6, 7, 5, -3, -15, -10, -8, -4, -2, -10, 1, 3, -4, 0, 2, -1, -14, -8, 8, -1, -9, 0, -1, -1, -11, -10, -11, -5, -1, 5, -11, 5, -7, -8, 6, 3, 0, -10, -7, 7, -2, -10, 2, 7, 0, -7, -9, -14, 4, -2, -3, -11, -17, 7, -9, -2, -9, -3, -14, 6, -15, -6, -3, -13, -12, 4, -11, -12, 7, -10, -5, 4, -4, -3, 4, -1, -1, -2, -4, 7, 1, 6, -7, -13, 1, -1, 6, 1, -7, -12, -2, -4, -1, -3, -7, -5, -7, -6, 5, -8, 6, -5, -8, 3, -1, -1, -6, 4, -4, 5, -5, -8, 6, 2, -1, 4, -15, -1, -11, -3, -5, 0, -1, 8, -8, 1, -4, -4, 4, 6}
, {-15, -50, -16, 9, -11, -28, -8, -28, -3, -5, -8, -18, -4, -41, 23, -27, 5, 11, -16, -4, -7, -25, 19, -39, -12, -6, -40, -15, 6, 15, -12, -6, -27, -2, 39, 3, -22, 5, 1, -8, -14, -1, -45, -5, 14, 26, 6, -21, 35, 10, 0, -10, -6, -8, 18, 0, -18, -21, 1, 7, -7, 19, -11, -3, -8, -13, -15, -11, 0, -20, 2, 4, 11, 22, 17, 5, 5, -9, 1, 8, -2, -3, 14, -2, -1, -24, 13, 11, -5, -10, -3, -27, 4, 3, -12, -28, 0, -28, 18, -7, -7, 7, -22, 19, 4, -35, -20, 9, 4, 26, -11, -7, -5, -14, 15, -5, 1, -41, 13, -24, 2, -16, -3, 17, 0, 6, 2, 36, -12, -16, -3, -15, 19, -22, -16, -5, -7, -4, -8, -32, 1, -16, -17, -27, -12, -11, -22, -29, -7, -24, -27, -6, -33, -22, -38, 10, 9, -19, -20, -20, -18, -1, 19, -7, 1, 11, 26, -15, -26, 8, 1, -23, 0, 29, 20, 10, -6, 22, 9, -8, -3, 0, 13, 3, 22, -18, -32, 13, 5, 11, 17, -24, -16, 17, 19, 32, 12, -1, -12, 4, 6, 0, 6, 12, 15, 11, 33, -7, 31, 20, -10, 7, -9, 3, 34, 21, 13, -15, 17, -2, 6, -9, 11, 22, -13, -11, -1, 5, -24, -1, 12, 1, 14, -13, -2, 40, 4, 16, -2, 0, 15, 23, 15, 10, 0, -5, 26, 20, -10, -14, 8, -12, -11, -3, 20, -13, -29, 2, 2, 22, -5, 13, -15, 0, -5, -27, 0, -11, 4, 8, 2, -4, 0, -11, 4, -5, 8, -15, -1, -7, 27, -23, -5, -5, -3, -27, -4, 15, -12, -23, 6, -6, -47, -6, -7, -12, -10, -23, -42, -21, 7, -15, -21, -26, -3, -21, -28, -4, -4, -1, 6, -25, -44, -16, -5, -33, -10, -26, -5, -21, -2, 18, 26, 13, -3, 5, 24, -3, 5, 13, 31, 18, -1, 17, 0, 19, -6, -1, 19, -4, 8, 21, 12, 9, 0, -9, 20, 6, 12, -16, 8, 1, -5, 11, 9, 11, 20, 12, 11, -3, -3, -2, 20, -2, 9, -12, 15, 9, 30, -9, 19, 20, 4, 12, -3, 17, 3, -19, 16, 0, 2, 2, 0, 25, -7, 4, 7, 3, -3, 25, -8, 8, 3, -4, -8, 3, 10, 20, 2, 1, 9, 9, 13, 21, 8, 8, 5, -5, -18, -1, 5, -19, -13, -1, 5, 16, -2, -6, 8, 15, 17, 12, -9, -10, -23, -19, -8, -17, -6, 1, -16, 2, 8, -25, -8, -18, -8, -17, -16, -25, 11, -1, 1, -8, -9, -20, -31, 10, -42, -9, -1, -12, -37, -14, -22, -15, -13, -13, -25, 6, 1, -17, -43, -6, -26, -22, -26, -19, -9, -18, -48, -15, -4, -22, -16, -8, -32, -57, -14, -7, -6, -6, 2, -6, 1, -18, 3, -42, -26, 25, 17, -26, 3, -1, -17, 36, -12, -14, -12, -32, -11, 1, -1, -47, 23, -1, -3, 21, 4, 8, -9, -34, 35, 1, 20, -27, 32, 16, -13, 2, 19, -2, 10, 6, -2, 14, 36, 15, -5, 7, 28, 12, 6, 6, 19, 14, 16, -8, 9, 18, 14, 31, 24, -15, 13, 17, 19, 23, 23, 28, 13, -8, -14, 0, -1, 0, 4, 37, 7, 21, 2, -3, 19, -18, -10, -19, -1, 5, 4, -9, 6, 28, 7, 3, 9, -1, -1, 0, -3, 17, -38, -20, -23, -24, -45, -52, -26, -21, -3, -22, -51, -4, -6, -1, -2, -41, 0, 14, -28, -19, 5, -14, -8, -9, -7, -28, 2, 1, -7, -29, 3, -25, -12, -19, -17, -29, -32, -12, -26, -22, 4, -46, -5, -37, -20, -28, -17, -31, 3, -4, -16, -2, -19, -20, -17, -10, 18, -43, -26, -25, -8, 11, 21, 14, -6, -17, 29, -18, -24, 19, 1, -21, 8, 11, 4, 13, 19, 29, 13, -27, -5, 0, 26, 12, -6, -17, -21, 1, 7, 19, 7, 2, -12, 30, 9, 14, 12, 24, 10, -4, 8, -17, 1, 29, 14, 28, -5, 12, -1, 13, 14, 28, 3, -2, 20, 11, 37, -12, 2, 18, 5, -12, 8, 13, -25, 2, -11, 30, 8, 3, -4, 10, 33, -11, -8, -1, 6, 5, 37, 6, 4, 24, -1, 15, 4, 5, -10, 24, 5, -12, 3, 15, -7, -6, 22, 5, -14, -27, -5, 19, -14, -3, -24, -18, -26, -49, -44, -13, 11, -15, -17, -27, -3, -53, -44, -14, -2, -13, -27, -21, -22, -9, -26, -9, -15, -7, -40, -6, -9, -15, -25, -45, -12, -44, -17, -12, -25, 8, -9, 1, -2, -14, -39, -16, -76, -20, -14, -9, -7, -12, -34, -23, -14, -11, -9, -35, -28, -41, -14, -46}
, {-6, -9, -9, -7, 0, 6, 2, -12, -10, -8, -1, -14, -3, -1, 6, -1, -8, 2, -7, 6, 0, -3, -5, -4, -1, 0, -7, 4, 5, -6, -6, -10, -11, 9, -8, -2, 1, -8, -11, -2, 2, -11, -9, 5, -1, 0, -5, 0, -2, 1, -4, 6, -8, -5, 1, 5, -18, 4, -11, -10, -16, 4, -9, -14, -6, -2, -5, -17, 1, -3, -9, 7, -7, -8, -1, 1, 5, -9, -10, -5, 8, 6, 2, -9, 0, -9, -2, -6, -12, -4, 4, -3, -9, -7, -11, 4, -11, -5, -9, 8, -14, 2, -2, -1, -11, -5, -12, -9, 8, -4, -12, -8, -7, -4, -3, -1, -3, -12, -3, 0, -7, -5, -17, -9, -5, 2, 0, 1, 1, -8, -11, -9, -4, -19, -12, -5, -6, -2, -5, -15, -1, 7, -6, -7, -11, 7, 1, -18, -8, -9, -12, 0, 5, -5, -11, -12, 6, -7, -4, -8, -10, -14, -4, -6, -6, 4, -9, -5, -10, 8, -4, -6, 6, -11, 0, -3, 3, 5, -8, -17, 1, -3, 1, -7, 7, -11, -10, 2, -9, 7, 8, -9, 3, -6, 2, 6, 6, -3, -9, -10, -15, -5, 0, -8, 1, 5, -3, -3, -3, -3, -4, -5, -2, -9, -11, -10, -8, 7, -7, -3, 1, -16, -7, -3, -9, 2, -2, 11, -7, 3, -6, -9, -15, -10, -1, -12, -10, -5, -18, 4, -12, -6, -17, -3, 2, -13, -12, 1, -11, 8, 7, -5, 0, -22, -3, 5, -5, -8, 5, 10, 6, -2, -9, -16, -8, -2, -15, 2, 5, -7, 6, -11, -15, -6, -12, 8, 8, 6, 3, 2, -10, -11, -18, 6, 7, -6, 3, -2, -7, -8, 3, -12, 10, -8, -2, -8, -16, 7, 9, -4, -1, -2, 9, -4, -8, -10, 1, -5, -8, 8, 5, -9, 3, 1, -17, 11, -9, -9, -3, -19, 4, -7, -9, -5, 0, 8, 4, 4, -8, -2, 1, 2, -6, -11, -5, 3, 3, -15, -6, 6, 6, -6, -2, 10, -5, -5, 1, 12, 0, -9, -13, 2, -1, -3, -22, -12, 8, 4, 1, -15, 7, 3, -8, 2, -9, -8, -8, -10, -6, -2, -6, 0, 6, -13, 1, -18, -3, -8, -9, 0, -9, 10, 1, 1, -15, 4, 3, 6, 3, -2, -6, -14, -14, -2, -14, -6, 6, -17, -19, 1, 7, 3, -7, -20, 8, -19, 7, 3, -10, 7, -6, -4, -18, 7, -12, -11, 6, -14, -10, -2, -16, 4, -1, -4, -3, -1, -8, -3, 8, -3, 6, 1, -4, -2, -1, -14, 3, -19, 11, -11, -8, 4, -7, 2, 7, -5, -14, -18, 12, -7, -1, -9, -11, -9, -1, -14, -8, 3, -2, -8, 5, 7, -7, 7, -15, 5, -12, -2, -3, 6, 12, -13, 7, 1, 4, -4, -2, -5, 4, 3, 5, 1, 1, -5, -10, 3, -6, -8, 4, -12, -17, -15, -3, -4, 3, 6, 4, 0, -11, 2, 0, -5, 0, -3, 8, 8, -11, -2, 1, 1, 0, -4, -9, 5, 3, -9, -12, -10, 4, 1, -3, -12, -6, -8, 0, -7, -11, 0, 2, -9, 3, 6, -4, 3, 1, 6, -4, 1, 11, -7, 6, -3, 8, 5, -5, -6, -15, -5, 1, -6, -14, -3, -11, -9, -8, -6, -7, -11, -8, 2, 9, -7, -3, 1, 0, 5, 7, 8, 4, -2, 6, 6, -7, -1, -5, 8, 2, -13, -23, -1, -13, -4, -1, -6, -5, -8, -18, 0, -1, 9, -9, -8, -5, -8, -1, -8, 8, -5, -3, -8, 5, 2, -3, 4, 2, 3, 1, -4, 0, 3, 11, -19, 3, -5, -5, -11, 1, -5, -16, -6, -3, 8, -9, -15, -12, 5, 1, -9, -1, -5, 8, -3, -12, 2, -2, 5, -1, -8, 7, -5, 6, 11, -3, -9, -8, 7, 2, -7, -17, -18, -5, -13, -3, -2, -2, -5, -12, 1, -1, -16, 8, 7, -2, 12, -12, -4, -3, -1, -11, 0, 5, -1, 11, -18, 1, 12, 4, 4, 8, -21, 0, -9, 0, -18, 4, -6, -10, 0, -20, -8, -5, -22, -3, -7, -13, -12, 3, -4, -6, 4, -12, -7, -9, -3, -8, -3, -10, -12, -9, 4, -5, 7, -14, 6, -8, -14, -4, -11, 6, -8, -11, -1, -2, 1, -11, -4, -13, -7, -10, -7, -4, -1, -6, -5, -18, -12, -1, 11, -9, -9, -9, 0, 2, 4, -7, -4, -7, -6, -7, 10, 8, 9, -14, -18, -21, 3, 8, -9, 2, -12, -5, 2, -10, -5, 4, -16, -13, -13, -19, 1, -7, -9, 7, -5, 12, -7, -11, -10, -4, 1, 7, 9, 8, -7, -7, -10, 3, 3, -8, 1, -2, -1, 10, -1, -17, 0, 7, -6, -6, -5}
, {-36, 12, 3, -8, -20, 24, 18, -7, 17, -11, 11, 4, 5, 3, 34, -9, 6, 26, 7, 44, -5, 12, 14, -31, 0, 14, 3, -13, 3, -6, 10, -15, -23, 25, -1, -1, 7, 24, 13, -6, -23, -11, -2, -14, 12, -13, 26, 0, -4, 1, 15, 39, 7, 19, 11, 14, -32, -1, 9, 6, -8, -25, 9, -4, -14, -5, -12, -13, -1, -7, -9, -41, -17, -28, 9, -30, 1, -39, 11, -44, 3, -37, -32, 0, 7, 25, -13, -22, -29, -1, 1, -6, 21, 9, -31, -23, 8, 3, 19, -29, -5, -4, 16, -37, -29, 6, 9, -45, -2, 7, 8, 17, 6, -6, -11, -25, 8, 3, 18, -23, 3, -1, 3, 4, 34, 10, -30, -13, -2, 18, 19, -9, 21, 12, 14, -32, -22, 9, 14, 18, 12, 29, -3, 17, -20, 32, 25, -50, -5, -9, 17, -2, 13, -5, 17, 31, 0, -4, 9, 0, 5, 8, 4, -6, 11, 0, 1, 5, 2, 3, 20, 17, 11, 1, -14, 4, -16, -11, -10, 12, -4, -13, -28, 13, 24, 7, -7, -1, -4, 2, -8, 4, 0, -46, -24, -7, 12, -4, -27, -10, -5, 8, -11, -19, 0, -15, -37, -3, -1, -14, -25, -11, -1, -23, 0, -12, 8, 6, -16, -8, 4, 3, -22, -18, 15, -20, -11, -15, 7, -17, 4, -13, -10, -3, 12, -24, 11, -26, -8, -4, -31, -18, -23, -28, 8, -16, -13, -19, 13, -4, -33, -1, -9, 3, -13, -13, 10, -31, 0, -28, 6, -4, -19, -34, -39, 9, 7, -15, 4, 26, -26, 28, -29, -18, 19, 13, -8, 11, 0, 5, 17, 7, -22, 10, -25, -17, 0, -39, -7, 16, -2, -7, -7, -15, -4, 0, -3, 2, 8, 20, 10, 19, 25, 4, -31, 2, 5, -7, 0, -17, -27, 46, 4, -3, -6, -1, -19, -21, 5, -19, 13, -14, -17, 1, 11, 8, -28, -2, -15, -6, -14, -1, -4, 5, -26, -12, -2, -26, -17, 2, -4, 4, -31, 10, 7, 3, -7, -10, 12, 8, 5, -12, 10, -3, -21, -1, -34, -29, -10, 3, -10, -9, -11, -22, 9, -25, -1, -29, 29, -8, 6, -14, 5, 15, 11, 0, -17, 0, -3, -24, 7, 12, -3, 30, -3, -1, 5, -10, -41, -21, -10, 2, -21, -31, -36, 14, 7, -21, 4, -28, 26, -29, -15, 11, -10, 1, -4, -9, -34, -2, 18, -34, -1, 4, -10, 17, 2, 6, -8, -21, -41, -19, 9, -21, -27, -24, -27, -42, 9, -13, -12, -22, -15, -17, -25, -4, 4, -16, -17, 20, -7, 5, -14, -12, 1, -12, -9, -22, -10, -6, -15, -25, -18, -3, 19, -4, -24, -10, 4, -10, 3, -1, -15, -17, -9, -5, 16, 4, 3, -3, -12, 9, -13, 9, -1, -2, 16, -15, 8, -33, 1, 0, 17, -19, -7, 5, 22, -5, -12, -19, -9, 1, 9, -5, -25, -12, -3, -28, 14, 7, -5, 4, -15, -14, 5, 7, -11, -13, 16, -3, -27, 40, -14, 11, 7, 1, 21, 21, 37, -28, -51, 4, -7, -20, 13, 21, -43, 0, -32, -33, 0, 3, -5, -25, -58, -8, 1, 14, -26, 8, -12, -27, -13, -40, -4, -37, -23, -10, 5, 24, -17, -33, -15, -7, -13, -19, -1, 4, -31, 12, -10, -10, -35, -22, 4, -54, -13, -15, 16, -3, -44, 16, -17, -5, -6, -37, 24, 6, 11, -1, 12, -2, -29, -10, 7, -11, -17, 2, 6, 4, -16, -3, 20, 2, 13, -23, 5, -15, 8, 2, 0, 8, 0, -12, 7, 23, -11, 31, -4, -18, -24, -2, -8, 7, -36, -10, -3, 26, -3, 1, 10, -8, 0, -7, -9, -6, -28, 1, -5, 0, -3, 9, -14, 7, 28, -13, 11, 8, -4, -15, 19, -5, 14, -5, -14, -15, -27, 13, 17, -13, -15, 2, 6, -10, -11, -17, 23, -31, -14, -10, 1, 14, -30, -20, 10, 8, 22, -9, 24, 27, -16, 27, 2, -23, -38, -19, -10, -63, -9, -36, -13, -19, -19, -26, 2, -59, 19, -45, 9, -20, -13, -13, -4, 16, 17, -31, 17, 3, -8, -18, 29, 6, -36, -9, -25, 18, -6, -42, 13, 7, 27, -33, 4, -1, 22, -11, 11, -4, -6, -17, -23, 1, -3, 4, 1, 21, 4, 21, -29, 1, 3, -7, 1, -1, -4, -36, -3, -3, 34, 6, 25, -11, -4, 13, 21, 4, 2, 22, -1, 44, 9, 8, 0, 7, 26, 0, -11, 8, 16, 33, 6, 14, 10, 28, 0, 21, 4, 69, -3, -2, -11, 15, 12, -1, -25, -4, -7, -4, -11, -16, 9, -5, 0, -6, -5, 28, 12, 4, -1, 6, 18, 16, -23, 8, -19, -2, 1, 14, 8, 3}
, {-9, -5, -4, -3, 0, 7, 2, 3, 9, 5, -2, -9, 1, 0, 9, 2, -6, 8, 3, 0, 3, 5, 7, -8, -6, -3, -10, -8, 2, -5, -7, 7, 7, 1, 3, 4, -9, -1, -11, -6, 0, -8, 3, 7, 3, 0, -7, -11, -7, -2, 4, 2, -10, 3, 8, -6, -2, -6, -5, -4, -3, -8, -6, -7, 6, 5, 2, -6, -1, -4, 3, -3, 8, -2, -9, -9, -5, -9, -2, -1, 8, 6, -5, -3, -1, 6, -11, -11, -11, 1, 8, -5, 0, -6, -2, 0, 5, 6, 0, -9, -4, -8, -1, 8, -9, 4, -17, -7, 9, -5, -6, -14, -4, 5, 1, -8, -9, -8, -3, -3, 3, 6, -1, -7, -8, 4, -2, -6, -2, 1, -5, 2, -7, -9, 6, 8, 6, -7, -8, -7, -5, -11, -4, -9, 2, -11, 1, -3, 4, 3, -3, -9, -9, -9, 2, 0, -2, 8, 6, 10, 8, 3, 4, 1, -7, -4, -11, -6, -2, -9, -4, 1, -6, 8, -4, -5, -5, -11, -11, 7, -2, -9, 6, 5, -3, -6, -10, -7, -1, 4, -2, 3, -11, 1, -9, 8, -5, 5, 1, -9, -10, 9, 4, -1, 4, -5, -7, -9, 4, -10, -1, 1, -4, -7, -7, 5, 4, -8, -8, 5, -12, 0, -11, 7, -6, 0, 4, -5, -9, -7, -9, -8, -3, -6, -5, 1, -4, 5, -10, 0, -11, 4, -10, 1, -6, 7, -1, 3, -11, -9, -6, -11, -4, -2, 0, 1, -3, 8, -6, -8, -10, -9, -4, -11, 7, 5, 2, -1, 8, -5, 5, -10, 3, -4, 0, -6, 6, -1, 0, 7, -4, -4, -8, -11, 3, -12, 2, 2, -7, 7, 6, -6, -9, 8, -1, -2, -10, 4, 3, 5, 7, 5, -2, -11, -5, 3, 0, -6, -4, -11, -9, 2, -8, -14, -6, 7, -12, -7, -5, 6, -5, -9, 1, -11, 2, -7, 2, 1, -9, -5, -10, -5, -8, 6, -7, -11, -5, 3, 1, 8, -3, 5, 1, -7, 5, -3, -8, 2, -12, 3, 2, -6, 8, 4, 4, -3, -4, 3, -5, -8, -12, 2, -9, -8, 4, 2, -2, 0, 7, -4, 5, -10, -3, -1, 1, -8, -4, 0, 9, 1, -2, -3, 8, -8, -5, -9, 8, 5, -10, 6, 6, 2, -9, -3, -6, 0, 1, -2, 2, 2, -7, -5, -2, -4, 7, -1, -3, -5, -10, 2, 5, -6, -13, -1, 5, 9, -13, 6, -6, -6, 6, -9, 6, -8, 5, -8, -6, -4, -9, 5, -10, 8, -2, 8, -7, 1, -1, -6, -5, 4, 2, -7, -6, 5, -2, -4, 6, -4, 4, -9, 3, -3, -1, 6, -7, -4, -8, -10, -9, 8, 8, 3, -8, -2, -4, -12, 8, 3, -9, 4, -12, 7, -2, 4, 2, -13, -7, 6, -8, -3, -10, -4, 4, 1, -11, -9, 9, -6, -12, 2, 5, 1, -6, -11, -2, -5, 8, 8, -6, 6, -10, -2, -6, 8, -1, 5, 4, -6, -13, 6, -6, -8, -5, -4, -1, 2, -8, 4, -2, 5, 1, -5, -2, -4, -10, 1, -8, 1, -11, -6, 1, 3, -6, -5, -7, -7, 0, -15, 4, -6, -3, -9, 0, 2, -4, -9, 4, 7, 7, 6, -11, 5, -1, -10, -3, -8, -10, -9, -14, 7, 3, 1, -1, 0, 1, -10, -3, 7, 4, -11, 2, 8, -15, -9, -9, 5, -12, 0, 5, 7, -2, -2, 6, 5, -6, 4, -4, -7, 8, -15, -4, 4, 4, 7, -14, 1, 8, -8, -4, 8, -8, -6, 4, -10, -5, -3, 9, -8, -4, 7, 8, 4, 2, -6, -3, 0, -7, -8, 1, 5, 10, 4, -2, 3, -3, -5, 8, -10, 9, -2, 4, 1, -10, 5, -8, -8, -5, -3, 9, -7, 4, -2, -8, -8, -11, -1, -7, 2, -7, -9, 0, 9, 10, 0, 1, -4, -11, -7, 0, 7, -6, 8, 1, -9, -8, -5, -11, -11, -9, 5, -3, -2, -9, 5, -4, -3, -5, -2, -3, -7, 7, -3, 2, 7, 0, 5, -1, 1, 6, -11, -1, 2, -8, 0, -9, -8, 7, 2, 9, -2, -2, -3, -12, 6, 0, 0, 1, -4, -7, 0, -3, -6, 1, 3, -7, 0, 1, -6, 1, -8, 6, -3, -6, 4, 5, -14, -9, -2, -3, -7, -3, -10, -9, -2, -3, 1, -5, 0, -11, -9, -8, -12, -1, 1, -8, -8, 5, 6, -4, 1, -11, -7, -9, -1, -5, 6, -9, -6, -13, -7, -3, -11, 0, 1, 4, -10, 5, -4, 0, -3, -11, 6, -2, -7, 2, 5, -11, 8, 2, 6, -1, 1, 4, -8, -4, -4, -4, 7, 2, -9, -10, -2, -1, 0, -1, -5, 5, -1, -7, -3}
, {-15, 3, 11, -8, -9, -1, -11, 0, 29, 1, -4, 8, -2, -2, 6, -8, -29, 29, 3, 2, 2, -17, 10, 4, 7, -2, -15, 3, -28, -19, 6, -10, -24, 17, -7, 43, -17, 25, 2, 3, 14, 2, 3, -1, 15, 25, -8, -2, -19, 28, 23, -31, 2, 1, 9, -8, 9, 5, 11, 3, -24, -28, -1, -23, -24, 12, 28, 27, -4, 44, -6, 23, -3, -46, -45, 3, 7, 23, -14, 7, -1, 0, -3, -20, -5, 5, -4, 25, 5, -2, 0, -3, -14, -6, 9, 2, -24, -5, -9, 23, -11, -22, 3, -38, -33, -20, -20, -48, 7, -7, -28, -15, -5, -22, -12, -28, -6, -20, -13, -21, -12, 11, -31, 20, -1, -27, -22, -12, -6, -20, -15, -23, 8, -41, -33, -18, -5, 4, -9, -31, -3, -22, -26, -14, -9, -13, -5, -18, 9, -17, -8, -27, 6, 3, -31, -4, 0, -28, -31, -29, -21, 21, 3, 23, 15, 10, -2, 5, 8, 0, 0, -13, 12, 36, -5, 12, -11, 14, 13, -7, -2, 0, -12, 25, 12, -4, 8, 2, -6, -13, -10, -6, 11, 1, 10, 52, -5, -6, -12, 16, 13, 3, 9, 16, 8, -2, -21, 0, 6, 8, 0, -26, 6, -15, 2, -3, 23, 1, 4, 10, -1, -4, 20, 7, 7, 6, -11, 14, 6, 7, -33, 21, 15, -20, -9, -7, -5, -18, -6, -15, 11, -3, -10, -11, 4, -17, -25, 0, -18, -10, 7, -16, -6, 17, -12, 5, 18, -12, -21, -5, -54, -29, -31, -7, -14, -11, -34, -22, -1, -50, 25, -32, 10, 3, -2, 14, -1, -18, 31, -28, -20, -8, -47, -31, 6, 26, -9, -1, 7, -3, 1, -46, -20, -16, -2, -13, -18, -12, -13, -3, 11, 7, -16, -13, -7, 16, 2, 13, 8, -13, 10, 10, -15, 0, 6, -20, -8, 10, 2, -26, 1, -13, -1, 0, 22, 19, -19, -17, -32, -12, -1, -10, -6, -12, -21, 3, 10, -14, -14, 9, -3, -2, -5, -25, 17, -12, -22, 31, 4, 14, -17, -16, 7, 23, 1, -19, 19, 2, -16, 5, 0, 22, 1, 10, 9, 14, 13, 18, -5, 20, 16, 2, 11, 0, 14, 34, -11, 5, 4, 23, -7, 7, 7, -33, 2, 0, 16, 11, 15, -11, 3, 22, 5, -8, 2, 4, 3, 13, 13, 7, 19, 2, -11, 8, 7, -23, 2, 10, -3, -6, 7, 18, 5, 6, -1, 10, -7, 5, 13, 9, 10, 17, 24, -4, 3, 5, 5, 18, 14, 13, 6, 13, 14, 26, -5, 5, 5, -11, 13, -1, -2, -12, -14, 25, -46, -3, -4, 9, 6, -8, -25, 8, 6, -3, -17, -15, -8, -31, -11, -34, 0, 4, 6, -9, 5, -32, -13, 0, -10, 8, -3, -17, -12, 2, -16, 23, -1, 28, -9, -10, -18, -11, -6, 16, 4, -12, 2, -25, -10, 4, -2, -5, -4, -21, 8, -10, -11, 27, 26, -1, -6, -6, 29, 1, 11, -4, -3, -38, -16, -7, 19, -13, -8, 23, 36, 9, -28, 6, 0, 18, 18, -13, -2, 26, 1, 18, 20, 7, 3, 35, 19, 0, 3, -1, 35, 9, 6, -3, 14, -2, -20, -9, 32, 28, -33, 2, -11, 1, 4, 5, -2, -2, 8, 8, 19, 23, 0, 22, -7, -6, -6, 1, -4, 41, -1, 16, -21, 7, 10, -8, 23, -6, -25, -20, 3, 13, -18, -3, -13, 21, -7, 10, -13, 7, 16, -6, -9, -27, 8, -13, 11, 8, 8, -4, -2, 19, -5, 12, 1, -18, 10, 7, -20, 15, 2, -9, -20, 2, 10, -17, 18, -11, -8, -7, 12, 1, -1, -8, -23, 2, -2, 4, 6, 3, 1, 8, 2, 27, -7, 14, 9, -5, 8, -8, -20, -19, -4, 30, -3, 5, 6, 10, 15, -10, 6, 6, 7, -10, 12, 10, 13, -31, 5, 6, 3, 0, -16, 5, 20, -5, 1, 1, 1, -5, 13, -4, -12, 6, -24, -29, -14, -16, -17, 6, -4, 15, 15, -2, 2, 18, 32, -16, 5, 16, 4, 10, 9, -2, 20, 0, 10, -1, 2, 17, 4, 7, 3, -3, 21, -8, 0, -26, 1, 36, 23, -17, -12, -7, -7, -3, -2, -17, -6, 25, 3, -2, 10, -17, -10, -7, 6, -41, -22, 5, -8, -2, -20, 11, -24, -8, -22, -5, 33, 5, -14, 22, 19, 15, -4, 0, 39, 5, -3, 6, 1, 31, 12, 0, 15, -10, 1, 10, 14, 3, 2, -12, -6, 16, 3, -11, 13, 4, 13, -18, -16, 14, -4, 7, 9, 32, 18, -11, 11, 12, 15, 9, 9, -12, 2, 20, 6, 29, 10, 5, -3, 27, 17, -2, 5, 21, 10, 7, 9, 6, -2, 14, 4, 19, 41, -4}
, {13, -29, -34, 1, -27, -31, -4, -34, -6, 8, -6, -25, -10, -30, 17, -34, 19, -4, -24, 36, 9, 11, -6, -52, 17, 3, -41, 0, 21, 47, -15, -3, 44, -7, -13, -26, 0, -25, 14, -15, -11, 17, 7, -14, -2, -18, 64, -9, -5, -8, -6, 45, -1, 1, 25, -23, 8, 8, -28, -3, 39, 31, -23, -13, 17, 13, 3, 6, 48, -18, 22, -14, -28, -5, 18, -36, -15, 9, 27, 2, -5, 1, 14, 7, -8, 0, 27, -5, 15, 9, -23, 26, 15, 10, 4, -15, 8, 23, 20, 4, 3, 5, 1, 15, 2, 13, 6, 33, 1, 15, -25, 12, -15, 22, 14, -12, 5, -15, -1, 5, 11, 2, 16, 1, -30, -24, 12, -29, -13, 17, 9, 9, 8, 7, 5, 17, 3, 25, -9, 24, 4, 8, -44, 23, 6, 1, 6, 2, 6, -5, -37, -3, 10, 6, 19, 6, -39, -44, 1, -2, 2, 27, 21, 14, 6, 13, -2, 3, -8, -5, -6, 4, 6, 11, -7, 0, -3, 10, 8, 21, 7, 16, 2, 3, -25, 1, 10, -7, 5, -19, 14, 12, -19, 9, -5, -7, 4, 19, 7, -7, 10, -6, -3, -16, 1, 4, 16, 8, -11, 5, 13, -16, 8, 2, 24, -2, -1, -1, 8, 3, -6, -8, -2, -9, -1, 10, -12, 10, -4, 0, 17, 1, -14, -16, 0, -11, 2, 10, 5, 3, 4, -16, 0, -18, -8, -4, 0, 9, 19, -4, 2, -9, 3, -4, 17, 11, 0, 7, -2, -4, 2, 10, -2, -11, -3, -26, 0, -17, -9, -6, -23, 7, -12, 5, -10, -28, 7, -14, -2, -17, -4, 2, 30, -6, -17, -11, -7, -7, -6, 7, -2, -5, 6, 5, -11, -22, -11, -10, -9, -42, 6, -17, -13, -6, 10, -40, -6, -11, 2, 12, -20, -25, -5, -11, -14, -10, -13, -9, -20, 0, 6, 2, 14, 13, -33, -1, -18, 17, 11, 16, 1, 17, -3, 26, 12, 9, 32, -10, 10, 7, 4, -7, 9, 27, 15, -9, 1, -11, -2, 5, -1, 35, -9, 9, 7, 1, -6, 3, 12, 8, 1, 7, 2, -25, -8, 31, 3, 15, 3, 22, 15, 1, 5, 15, 22, 10, 4, -6, 5, -14, -1, -11, 18, 7, -12, -15, -1, -4, -22, -5, -3, -19, -23, 7, 17, -17, 5, 13, 1, -6, 7, -19, -15, 11, 4, -7, -15, -6, 14, -1, 3, -5, -6, -15, -15, 4, 3, -19, -33, -3, 2, -45, -14, -8, -5, 6, 8, -22, 5, -34, -3, 8, 6, -38, -17, 7, -2, 3, -28, -3, 1, -1, -11, -21, 16, 13, -10, 5, 29, -22, 4, -21, -22, -30, 22, -6, -8, -4, -1, -20, 5, -13, 22, -7, 22, -6, 7, -4, -6, 17, 13, -20, -1, 10, 2, -30, 9, 31, -17, -3, 5, -9, 19, 17, 15, 6, 4, 3, 19, -7, 13, -1, -11, 13, 2, 9, 7, -2, 4, -7, -9, 7, -2, 0, 2, -6, 13, -5, -5, -11, 9, 23, -9, 12, 13, -2, 2, 4, 11, -4, 26, 5, 26, 8, -16, 13, 2, 11, 8, 14, -2, -2, -7, -6, 6, -1, 7, 1, -7, 2, -10, -4, 14, -2, 4, 6, 9, 9, -13, 8, -4, 8, 15, -13, -2, 18, 7, -16, 17, -21, 4, 14, -14, -5, 3, -16, 5, -17, -4, 11, -10, -5, -3, 6, 15, 12, -16, 4, 26, -19, 17, -2, 41, -14, -9, 2, 12, 17, 8, -8, -9, 3, -12, 4, 8, -12, -2, -4, -6, -31, 9, 0, -32, -7, -6, 9, 4, -11, -19, -22, -13, -2, 14, 5, 8, 13, 0, -6, 22, 9, 1, 8, -6, 19, -16, 7, 4, 32, -4, 33, 0, 9, 2, -6, -7, 27, -8, -11, 20, -13, -5, 7, -4, 7, -11, -15, -2, 10, -17, -14, -26, 1, -15, 13, -2, 1, 2, 1, -9, 30, 7, -22, -4, 8, -9, 4, 0, -8, -21, -16, -4, 20, -14, -6, -9, 16, -1, -6, -20, -14, -18, 2, -27, -9, 3, 7, -23, 2, -5, 5, 5, -26, -3, -43, -38, 20, 13, -5, -22, 16, -46, -16, 14, -30, -31, 9, 15, 20, -10, 15, -23, -6, -3, -28, -24, -2, 5, 4, -22, 3, -2, 27, 21, -12, 7, -46, 7, 9, 25, 15, -7, -8, -10, -7, 21, -5, -19, -14, 12, 8, -2, 7, -8, -10, 11, -11, 7, -16, 4, 15, 4, -1, 6, 0, 7, -5, 8, -41, 18, 3, 6, 7, 10, -7, 1, 5, 17, -2, 0, 12, 2, -13, -14, 3, -19, 12, -14, -22, -24, -17, -2, 11, -10, -14, 9, -6, -19, -11, -8, -8, 6, 48, -16, 12, -11, -5, 1, 20, 15, 12}
, {-12, 6, 10, 10, 2, 3, 1, 14, 6, -14, -12, 11, -10, 19, -2, -10, 13, -1, 4, 13, 6, 3, -8, 15, 23, 7, 8, 8, -6, 4, 2, 16, -10, -16, -5, -8, -12, -5, -10, -24, -31, -32, -16, -13, -7, -17, -21, -16, -2, -3, 0, -14, 3, -6, -16, -10, -7, -1, -21, -6, -7, 5, -8, -7, -10, -6, 24, -13, -9, -23, -4, -17, -22, -15, -13, -15, -2, -11, -27, -13, -1, -18, 3, 3, 0, 1, 0, -2, -14, 1, -21, -19, 25, -18, 9, -3, 11, -13, 2, -13, 5, -31, -35, -28, 9, 41, 20, -8, -5, 7, 5, 48, -2, 3, 6, -27, 5, 14, -13, -6, 24, 6, -16, -13, 7, -8, -23, -16, 11, 5, -2, -2, 17, 12, 17, 13, 16, 19, 22, 12, -10, 3, 3, 35, 13, -8, 0, -8, 8, -24, -13, 13, 35, 6, 2, -27, 19, -7, 2, 23, 12, -6, 11, -12, -3, -4, 8, 18, 14, 1, 3, 12, 6, 16, -1, 3, 2, -11, 11, 4, -9, 4, -15, -9, 28, -5, 3, 5, 15, 1, 0, 14, -5, -17, 3, -14, -14, -14, 3, -10, -25, -11, -21, -11, 3, 1, -4, -14, 10, 10, -8, -16, -3, -6, 2, 10, -8, 7, -15, -1, 8, 0, -8, 1, -13, -3, -2, -6, 1, -1, -4, -12, -10, -21, 3, 0, -8, -14, -12, -13, 21, -4, -16, 2, 7, 6, -13, -18, -4, 6, 14, -11, -3, -9, -11, -7, 7, -11, 11, -19, 1, 0, 13, -5, -1, -7, -9, -7, -10, -5, -14, 0, -15, 7, -10, -3, -1, 21, -7, -9, 1, 8, 3, -18, -17, -14, -26, -3, 3, -2, 13, 16, 0, 9, 3, 1, -17, 16, 12, 7, -1, 12, -5, 4, -1, 10, 10, 9, -3, 19, -11, 10, 9, -3, -7, 19, 9, -17, 12, -29, 7, 1, 20, 6, 17, -12, -10, -9, -1, 4, 20, -5, -5, 1, -5, 2, 10, -2, 7, 5, 6, 14, -22, -6, 11, -2, 2, -5, 10, -20, -13, -8, -18, -7, 19, 12, 11, 6, -2, -9, 3, -2, -6, -8, 9, 12, -12, 8, -2, -2, 9, -2, -8, 17, -14, 2, 15, 6, 2, 12, 5, -20, -7, 0, -10, -8, -16, -18, 2, 9, -30, -10, 10, -17, -11, -5, -5, -9, 14, 4, 9, 15, -2, 2, 9, -10, -7, -4, 0, 15, -5, 8, -25, 15, -4, -24, 15, -21, -9, -18, -8, -10, -15, -12, -18, -11, -17, -3, -2, -22, -5, -12, 4, -1, -29, 3, -8, -17, -2, -10, -7, 11, -26, -14, 1, 1, -30, 6, -2, 14, 6, -35, -4, -17, -6, -2, 0, 10, -6, -3, -2, 5, 3, 7, -5, 6, 2, 7, 4, -15, -4, 3, 15, 0, -5, -23, -19, -4, 8, -15, -22, -19, -11, -12, -2, -17, -11, -8, -2, 2, -10, -10, -3, -4, -19, 2, -10, -23, -10, -1, -1, -6, -13, -4, 3, 12, -4, -14, -18, -1, -2, -18, 0, -20, -5, 5, -20, -13, -23, -10, 13, -9, -13, 15, -7, 5, 26, -11, 5, 4, -5, -4, 0, -25, -12, 10, 30, 6, -6, -17, -10, 11, 6, -18, -15, 1, -1, -14, -25, 2, -8, -17, -11, -4, -27, 2, 3, 22, -10, -9, 16, 1, 14, 5, 4, 4, 8, 4, -19, -1, 12, 2, -9, 8, 7, 21, 4, -18, 7, 1, -1, 9, -1, -25, -13, 5, -18, -9, 1, 12, 9, -11, -2, -9, -6, 0, -6, 1, 8, -5, 14, 8, -1, 2, 25, -14, -13, 18, -15, -4, -7, -5, 8, -10, -1, -8, -6, 3, -2, -10, 1, 5, 6, -7, 18, 2, 17, -1, -1, -4, -1, 1, -16, 5, 6, 1, 26, 0, 5, -4, -4, -5, 12, -16, -23, -32, -19, -12, -2, -7, -13, -8, -7, 2, 2, -21, 13, -8, -2, 2, -8, 3, -3, 0, -5, 14, 2, -16, -5, -3, -9, -13, -10, 17, -10, -3, 12, -4, 24, 17, -8, 13, 1, 23, -3, -21, 6, 11, -10, 12, 9, -8, -4, 4, 18, 10, -25, 6, 4, 3, 7, -26, 13, 6, -9, 6, 8, 7, 4, 33, 11, -4, 10, -11, -12, -12, 2, 21, 2, -26, -4, -16, 19, 10, -6, 14, -12, -6, -7, 11, 12, 13, 6, -6, -1, -7, -7, 14, -9, -18, -9, 16, -5, -4, -8, 5, 20, -4, -3, 18, -28, 26, -35, -13, -10, -2, 8, -6, -18, 14, 26, -3, -7, -5, -29, -26, -10, -27, -14, 11, -4, -7, 6, 5, 0, -2, -12, -5, 2, 9, -3, 7, -6, 7, 2, -24, -5, -8, -9, 7, -10, -2, 20, 7, -2, -2, -8, -20, -13, -12}
, {10, 6, 22, 0, 26, 8, 18, 16, 20, 5, 25, 34, 8, 62, 13, 42, 3, 28, 19, -10, 2, 12, 12, 22, 26, 7, 46, 15, -12, -22, 10, 45, -50, -22, 11, 21, -6, 0, 10, 28, 40, 33, -6, -7, 0, 17, -6, 6, 30, -2, -23, -22, 8, 9, 1, 13, 10, -15, 13, 6, 0, -8, 23, 23, 4, 0, -12, 4, -11, -27, 4, -19, -6, 27, -8, -13, 7, -16, 15, -15, 24, -21, -13, 7, 7, -5, 8, -28, 2, -7, 0, -7, 9, 16, -15, 34, 23, -5, 0, -21, 24, 11, 3, 4, 18, 24, 26, 7, 6, 5, 15, 19, -24, 13, 25, 15, -5, 17, 20, -15, 17, -11, 31, 2, -3, 16, 3, -23, -13, 42, 26, 4, 4, 13, 22, -1, -9, 5, 4, 1, 4, 9, -5, 4, 0, 47, 23, -12, 3, 1, 19, 30, 9, 6, 7, 26, -2, -14, 26, 6, 10, -5, 5, -18, -1, -2, -5, -1, -15, -1, 14, -1, 2, -30, -7, 2, 7, -5, -4, -9, 4, -9, 0, -46, 1, -9, -7, -7, -6, 7, -12, 11, -7, -4, 19, -6, -2, -9, -1, -1, 4, 8, 12, -13, 11, 9, 5, 1, -11, -2, 2, 5, -9, 10, 17, -6, -1, 1, 11, -4, -1, -14, -9, -5, 6, 1, -10, -4, -7, -18, 6, -11, -21, -23, -10, 19, 6, 8, -6, -7, 5, -8, -2, 8, 2, 5, -9, -3, -7, 0, -24, 0, 6, -6, 12, -25, 11, 15, 16, -9, 8, 11, 15, -15, -22, -2, -3, -14, -1, 9, 6, 5, 1, -5, 10, -6, -4, -11, 10, 14, 3, 9, -23, -4, -6, 18, -5, -17, -2, 12, -3, 6, -19, 12, 9, -5, -10, -12, -2, -20, 2, -6, -15, 2, 16, -3, -6, -33, -5, -2, 15, -12, 16, 10, -20, 11, -13, 2, 0, 11, -1, 14, 17, 3, -1, -3, -5, -11, -6, 3, 1, -36, 5, -1, 2, -3, 19, -11, 9, -12, 8, 11, -15, -12, -2, -3, 25, -10, 14, 15, -9, -5, 7, -10, -4, -2, 8, -4, 1, -9, 4, -11, 16, -3, 2, 5, -14, 5, 8, -2, 2, -19, -5, 3, -13, -23, 12, 10, 17, -10, 4, 1, -3, 26, -1, -15, 7, -2, -12, 9, 0, 3, 12, -6, 12, -16, -1, 16, 13, 10, 7, -16, 17, -4, -7, 15, 7, -10, 13, 2, -3, 3, 2, 6, -7, -8, 0, -25, -4, 5, -12, -12, -4, 17, 11, 12, 13, 15, -6, -7, 17, 4, 1, -5, -3, -5, -7, -13, -2, 7, 0, 2, 0, -3, 13, -7, -6, 3, -15, 3, 25, -6, -31, 2, 8, 20, -11, 10, -3, -15, 3, -8, -17, -3, 2, 10, 7, -21, -1, -12, 7, 8, -10, 8, -9, -5, -12, -21, 8, 9, 11, -10, -15, 4, 5, 7, -6, -40, -4, 1, 6, -25, -9, -30, -3, 5, -10, -33, -38, 6, 5, 3, -20, -56, 15, 7, -29, 31, 9, 14, -20, -19, 44, -20, 1, -41, 21, -19, 14, -29, -10, 13, 4, 2, -7, -7, 13, 0, -18, -28, 4, 27, -5, 24, -15, -2, -25, -8, 11, -1, 24, 23, -27, -29, 24, -3, 2, -18, 4, 15, 19, -1, -6, 21, 13, -18, -2, 8, 12, 5, -20, -9, 21, -32, -3, -4, -9, -6, 5, 7, -3, 5, 8, 4, 8, -12, 1, 15, 8, 4, -9, -16, 2, 6, 9, 9, -6, 19, 14, -15, -6, -18, 7, 17, -6, 0, -10, -1, -8, 7, 3, -4, 3, -15, 2, -3, 4, 9, 15, -10, 6, 3, -6, -8, -9, 26, 8, 15, -9, 5, -6, 8, -3, -8, 23, 28, 2, -22, -7, -10, 2, 6, -3, -1, 5, -10, 24, -10, -6, 5, 18, -11, -1, 9, -3, -11, 15, -36, -19, -2, -14, -25, 7, -9, -23, -3, -23, 6, -12, 10, -4, -2, 4, -17, -3, -7, -9, 15, 13, -13, -10, -28, 21, 20, -4, 16, 21, 10, 2, -3, -22, -15, -13, -14, -7, -24, -1, 4, -39, -8, 1, 21, 4, -10, -14, -14, 33, 10, -16, 18, 28, 6, -8, -9, -11, -1, 2, 24, 4, 8, 15, -9, 11, 23, 12, 2, 1, -2, -8, 10, -22, 10, 38, -35, -4, 0, 18, 3, -8, -1, 11, -15, -42, 1, 0, -21, -22, -5, -5, 14, 7, -6, -22, 5, 12, 8, -3, 33, 11, 16, 3, -16, 9, 15, 7, -8, -6, -1, -24, 10, -10, -7, 11, 4, -4, -1, 19, 8, -17, 23, -26, -2, -15, 20, -3, -6, -26, -30, -50, -11, -2, -1, -19, -12, -6, 7, -17, -11, 9, -9, -15, 31, 7, -3, -2, 2, -2, -31, 22, -15}
, {-7, -2, -8, 5, 7, 3, 1, 2, 1, -11, 5, -1, 0, 1, 6, -4, -13, -5, 7, -2, 6, 6, -3, -1, -9, 1, -12, -4, 8, -10, -8, -5, 0, -4, -12, 4, 0, 0, -6, 2, 4, 6, 2, -7, -9, -14, -10, 5, 0, -4, -3, 7, 1, -3, -1, -8, -9, 7, 0, -11, 5, -1, 2, 2, -4, -2, 3, -1, -6, -9, 1, -4, -1, 8, -7, 4, 2, 1, 1, 1, 1, -9, 5, -8, -8, 2, -1, 2, -10, -1, 7, -12, -3, -2, -4, 5, -5, 6, 1, 2, -9, -6, -9, -2, 3, 0, -9, 1, -9, -4, 5, -9, -6, -10, 3, 3, 9, 7, 6, -9, 7, -5, 0, -1, -6, -7, -5, -2, -8, -8, -11, 1, -8, -5, -11, -9, -4, -7, -1, -5, -7, -7, -4, 6, -8, 6, 4, -11, -4, 0, -10, 6, 2, 5, -3, -9, 1, 8, -1, 7, 1, 2, 3, 4, 1, 7, -10, -4, -1, -2, 4, 2, 7, 0, 1, -8, -2, -5, -8, -2, 7, 3, -12, -2, 5, -3, 3, -4, -4, -4, -3, -8, -11, -7, -3, 3, -11, -4, -12, 6, -4, -8, -9, -3, -9, -14, 3, -10, -10, -2, 2, 3, -11, 0, 7, 3, 5, 5, -11, -11, 2, 9, -3, -9, -1, -1, 2, -7, 5, -13, -8, -2, 4, 5, -6, 5, -2, -8, 5, -4, 4, -3, -8, -10, 5, 4, 3, -7, 4, 1, -8, -7, -7, 0, 0, 1, -9, 9, -2, -8, -10, 6, -1, -11, 4, -3, -8, -4, -6, 0, -9, -3, -5, 7, -3, -12, 4, -5, -11, -6, -11, -1, -5, 2, -10, -14, -10, 0, 5, 3, 2, -10, -1, -7, 2, -7, -7, -5, 5, -4, 0, -15, -11, -2, -13, -7, -7, -2, -5, 7, -3, -7, -10, 7, -2, -4, -4, -9, 9, 4, 5, -9, 4, -9, -4, -14, 6, 0, 5, -2, -7, 4, -10, -1, -3, 4, -9, -10, -1, 4, -9, -2, 1, 9, -10, 5, -8, -3, 0, 7, 8, 8, 7, -3, -5, -11, -4, -6, -9, 2, -8, -7, 0, 1, -5, -9, -5, -10, -11, -10, -4, 4, -11, -3, 0, 7, 4, 7, -10, 5, -1, -10, 9, -5, 3, -4, 5, -5, 8, 5, 4, -9, 9, -3, -12, -10, 6, 1, -5, -4, 2, -4, 2, -8, -7, -1, 3, 7, -8, 6, 4, 6, -1, -8, 8, -2, 4, -4, -15, -7, -6, -7, -14, 3, -1, 3, 0, -2, -1, 4, -1, -7, -9, -6, -8, 8, -5, -9, -8, 1, -2, 9, 7, 1, -6, -10, 0, 8, -2, -3, -9, 2, -7, -10, -4, -9, -8, 0, -6, 7, 4, -7, -11, -11, -2, 7, 5, 7, 5, 0, 8, 10, 5, -8, -8, -1, 4, 0, 5, -9, 8, 2, 1, 3, -8, -3, -4, 0, -4, -10, -5, -1, -1, -8, -7, 0, 2, -10, 7, 4, -12, -10, 4, 4, 2, 5, 10, -8, 5, -8, 6, -4, 9, -7, -7, -13, 0, 7, -4, 0, 0, -8, -9, -6, 2, -12, 5, -8, 2, -6, -10, 6, 2, -7, -7, 6, 6, -6, 5, -8, -3, -6, -9, -9, 6, 8, 2, -1, 0, -8, 1, -9, -11, 9, -10, -8, 6, -5, -11, 0, -3, -12, -8, -4, 5, -9, -5, -3, -5, -4, -7, 3, -5, 3, -7, 6, -10, -7, -7, -6, -5, 5, -3, 3, 7, 1, -13, -2, -2, 4, 3, -7, -1, -4, -10, 1, -6, -10, 5, -4, -1, 8, -7, 4, -6, 0, -2, -5, -11, 4, 2, -2, 1, -6, -9, -1, -9, -4, -2, -3, -11, -5, -3, -10, 4, 7, -8, -7, -1, -1, 3, 9, 3, 7, -12, -9, 3, -8, -11, -6, 6, 6, -8, 6, -7, 3, 4, 1, -4, -1, 7, -2, -2, -7, -4, 5, -2, -4, 5, 8, 1, -7, 6, -4, -12, -11, 3, -8, -4, 1, -1, 3, 8, -5, 7, -6, -6, -3, -7, 3, 3, 2, -8, 3, -3, -3, -8, -8, -10, -2, 2, -10, -4, -4, -3, 4, 3, -7, -7, 3, 1, 5, -10, -4, 5, -10, -2, 6, 7, -8, 1, 3, 3, 1, -13, -8, -7, -2, -2, 0, -6, -13, -5, 3, -1, -2, 2, -1, -6, -7, 2, 4, -8, -7, 0, 7, -1, -7, -6, 0, -3, 1, 4, 0, 1, 2, -2, -4, -10, -13, -2, -1, -14, -10, -12, -4, 0, -7, 1, -9, 8, -11, -13, 0, 7, 7, 6, -14, -6, -4, -13, -1, 1, -4, 8, -13, -1, 0, -12, 7, 2, -11, -10, 1, 3, -7, -10, -2, 7, -9, 0, 7, -4, -6, -8, 6, -13, 6, -2, 3}
, {-6, 23, 11, 8, -14, 34, -1, -5, -4, -6, -3, 9, 8, 2, 8, -36, -14, -7, 12, -14, 3, -4, -10, 39, -19, -3, 20, -33, -11, -11, 11, -21, -34, -2, 5, -4, -7, -3, 7, 1, -4, -30, -8, -10, -4, 1, -2, 11, 45, 7, -1, -30, 7, -3, 33, 15, 14, -15, -23, -16, -12, 14, -2, 31, -15, 12, -4, -18, -36, -10, -7, -25, -34, -31, 9, -31, -16, 8, 22, 8, -1, 14, 1, 14, 0, -1, 40, -7, 23, -14, -19, 13, -4, 33, 7, -14, 16, 1, -9, -8, -2, 18, -9, -36, -27, -19, -7, -13, 3, 25, -1, 2, -23, 2, -8, 12, -2, -28, 3, 13, 33, -6, -10, 5, 8, 17, -9, -17, 16, -10, 5, 1, -21, -14, -14, -13, 2, -17, -1, -7, -12, -6, 6, 10, 19, -8, 1, -15, -9, -19, 10, 9, 9, -7, -23, -3, 6, 21, -2, 3, 13, -25, -18, 13, -15, -1, 1, -15, -4, 3, 5, -23, 6, -16, -8, 3, -32, -6, -17, -13, -9, -3, 13, -59, 15, -17, -6, 5, -14, -1, -15, 2, 0, 7, 14, 1, 16, -2, 4, -12, 19, 4, 21, 4, 8, 23, 9, 6, 7, 0, 6, -14, -7, 3, 17, -1, 7, -4, 8, 12, -3, -17, -15, -11, 0, -21, -9, 6, -1, -6, -26, -11, -9, -23, -10, 5, -8, 3, 26, 3, 1, -7, -29, 23, 6, 8, 8, -1, 10, -9, -24, -3, -17, 4, -9, -28, -14, 21, 2, -5, -7, 3, -7, 15, -6, -1, -2, 8, -7, -6, -17, -18, -8, 9, 21, 2, 10, 13, 7, 23, -19, -13, 14, -18, 7, -7, 4, 8, -14, -5, 20, 0, 10, 18, 1, -4, 0, 9, 20, 7, 0, 23, -3, -3, 15, 16, 9, -17, -10, 15, 8, 5, -6, -8, 9, 15, 14, -6, 21, 11, -12, -44, -2, 1, -12, -14, 8, -30, -10, -17, -30, -4, 5, -19, -9, -12, 4, -3, -19, -32, 4, -10, -15, -43, -5, -1, -37, 5, -7, -11, -10, -4, 30, -23, -12, 12, 6, -30, -10, -17, 2, 3, 12, 16, -11, -28, -2, 2, -25, -20, -17, -7, 1, -2, 8, -10, 2, -8, 7, 4, 15, 1, -21, -17, -2, 11, -3, -12, 6, 7, 33, 0, 7, 2, -5, 23, -4, -9, 12, 6, -30, -12, 0, 23, -12, 21, 8, 12, -10, -19, 21, 13, -7, -12, -8, -7, -11, -3, -5, 14, 15, -2, -25, -11, 15, -2, 2, -22, 3, -2, 25, 10, 6, -5, -6, 4, 7, 3, 13, 8, -5, 2, -11, 37, 10, -2, -14, 24, 4, -18, -7, -9, 12, -17, -37, 8, 7, -1, 18, 27, -13, 22, 10, 23, -13, 0, -25, 0, 5, 1, -16, 39, 22, -17, -8, 3, 21, 9, 4, 6, 3, 4, 20, -8, -24, -20, -2, 3, 9, -1, -12, 10, 7, 14, 23, -27, 18, -10, 2, 10, -4, -10, -6, 31, 3, -5, -26, -30, -3, 8, -7, 51, 6, -20, 18, 8, 5, -17, 3, -4, -2, 13, 4, -12, -6, 4, -4, -3, 12, -5, -10, -16, -1, -10, -13, 6, 8, 0, -19, 2, 0, 18, -15, 8, 15, 14, -12, 9, 34, -10, -4, -6, -1, -1, 17, -8, 7, -24, -17, 21, -9, -23, 9, 19, 3, 0, -7, -24, -13, 0, 2, -7, 19, 2, -30, -21, 2, 6, 15, -20, -6, -1, 8, 15, 18, 35, 35, 24, -9, 5, 17, 14, -19, -17, 11, -11, -8, 3, -7, -11, -21, -2, 25, -26, 11, -1, 1, 1, 2, 11, 20, 8, 9, -2, -17, 21, 31, 12, 4, 21, -1, 1, 7, 1, 7, 6, 0, -22, -11, 3, -11, 0, -2, -10, 14, 12, 7, -9, -1, 19, -1, 8, 0, 6, -9, 3, -4, -11, 9, 12, 18, 4, 3, -24, -1, -20, 10, 4, 21, -11, 4, 1, -3, -14, -9, -16, 10, -6, 24, -13, -1, 22, -26, -10, -1, 3, -24, 1, 1, 19, 31, -4, 6, 4, 9, 3, -4, -4, -14, 14, 2, -8, 0, -15, 4, 2, -16, -13, -2, -3, -28, -9, 22, 29, -13, -4, 9, 25, 11, 5, 21, 36, 9, 5, -23, 13, 1, -6, 11, -1, -1, -4, -8, -21, -3, -23, -4, 5, -8, -1, -23, 11, -52, -15, 14, 32, 6, -18, 5, -8, 8, 11, -3, -20, -14, 19, -1, 7, -7, -3, -26, 14, -24, 29, 22, -8, 1, -23, -26, -10, 28, -15, -5, 13, -31, -14, -1, -28, -20, 0, 11, 25, -4, 21, -7, 2, -3, 31, 11, -15, 0, 1, -7, 9, -3, 7, 25, -16, -9, 4, -21, -29, -6, -20, 12, -7, 1, -35, -13, 22}
, {10, -10, -4, 6, -25, -20, -5, -48, -25, -19, -15, -25, -9, -32, 24, -38, 9, -10, -23, 35, -2, 12, 10, -56, 19, 2, -40, 17, 18, 33, 4, -24, 15, 13, -3, -19, -17, -27, 2, -27, -13, -9, -28, -8, -3, -15, 35, -26, -19, -13, -2, 46, 5, 3, 22, -11, -35, 14, -7, 2, 6, 19, -13, -36, 18, 17, -21, -29, 18, -13, 11, -13, -15, 1, 21, -32, 8, 8, 4, 9, 10, -14, 5, 16, -1, 11, -9, 15, 0, 3, -1, 10, 14, -16, -16, -21, 13, 7, 15, 12, 7, -7, -15, 13, 16, 18, 9, 33, -8, 5, -27, 30, 9, 22, 20, 8, 2, 19, 7, 4, 45, 9, 15, 8, 6, -1, 19, -1, 7, 6, 7, 2, 17, -1, 9, 0, -4, 14, 16, -16, 0, 17, 6, 19, -6, 3, 9, 7, -3, 33, 10, -4, 4, -3, 25, 3, 10, 16, 0, 11, 2, 18, 19, 6, 12, 6, 20, -1, -10, -2, -2, -9, 2, 25, -46, -2, 1, 24, 6, 10, -10, -1, 22, 36, -10, 10, -16, 5, -24, -4, 27, 20, -20, -1, 3, -10, -5, 13, 8, -10, 1, -15, -24, -36, 4, -14, 1, -17, 4, 5, -27, -11, 4, -1, -3, -1, -22, -4, 7, 14, -22, 3, -5, 13, -5, -2, -14, 8, -7, -3, 30, -33, -31, -4, 1, -3, 6, -21, 9, 1, -11, 1, 7, -8, -10, 6, 11, -16, -12, 10, -9, -1, -3, -17, 14, -10, -22, 6, 15, -9, 10, 17, 26, -3, 7, -9, 19, -29, -10, 20, 2, 27, -7, 8, 6, -15, 7, 8, 6, 7, 16, 12, 22, 26, 7, 18, 0, -13, 21, -5, -13, 7, 33, 1, 10, -9, -7, -25, 6, -18, 1, 2, 8, 12, 5, -33, -17, -2, -5, 18, -5, -35, 15, 10, -1, 17, 11, 26, -13, 6, -9, -12, 8, -5, -12, 4, -25, 6, -16, 1, -9, 0, -9, 10, -24, -9, 1, -25, -15, -10, 0, -12, -34, 20, -13, 11, 0, 1, -2, -3, -5, 7, 9, -47, 5, -4, -11, 5, -6, -15, -18, 4, -13, -21, -1, 5, -30, -6, -9, -14, -20, -26, 5, -13, 5, -4, 9, 10, -13, 8, -6, 0, 10, -13, -22, -20, 0, 30, -14, -10, -22, -38, -21, -2, -1, -38, 11, -1, -20, -12, 13, -13, -26, -31, 0, -26, -24, -13, 5, -7, -34, -5, -9, 3, -19, -6, 15, 3, 8, -7, -1, -22, 13, 0, 9, 41, 23, -15, -2, -7, -8, 3, -6, -5, 1, -1, 10, -12, 0, -17, 9, 0, -12, -7, 6, 14, -8, -3, 7, -13, -23, 14, -8, -19, 3, -11, 7, -18, 4, -19, -10, -12, 21, 8, 0, -13, 7, -13, 3, 3, -5, -7, -19, 7, 11, -8, 7, -2, -9, -1, -17, -1, 2, -1, -13, 3, -11, 0, -2, -17, 3, 9, 2, -4, 2, -27, 10, -1, 0, 11, 1, 0, 9, 2, -32, 5, 16, -9, 7, -17, -6, 10, -15, -4, -1, -9, -33, -21, -8, 2, 27, 1, 2, -7, 5, -9, 22, -22, -1, 7, -19, 4, 8, -5, 20, -6, -15, 0, 3, -44, 5, -6, 9, 2, 14, 15, 19, -2, -28, -11, 27, 2, 12, -25, -14, -9, 0, 11, 23, -34, 0, 6, 2, 4, 2, -12, 16, -10, -17, 13, -18, -19, 14, 16, 10, 16, 9, -6, -8, -3, -5, -1, 28, -1, -15, 12, 8, 18, -5, 23, 2, 17, -15, 0, -2, -21, -2, -6, -13, -12, -4, 1, -10, 13, -22, -5, -3, 6, -6, -1, -15, 5, 18, -5, -2, 13, 12, 14, 6, 4, 12, -5, -13, 11, -7, 7, -9, 27, -7, 4, -3, 10, 6, 14, 5, 6, -3, 5, 12, -12, 3, 9, 2, -1, 6, 19, 2, 13, -7, -16, -7, -9, 0, 21, -12, -4, -37, -5, -3, 26, 7, -7, -15, 31, 2, -4, -4, 6, -11, -46, 16, -4, -26, 17, 9, -11, 15, 21, 6, -24, -15, 42, 8, -15, -5, 5, -20, -1, -30, -5, 19, -1, -2, -19, -16, 13, 8, -6, 10, 2, -17, -29, -7, -36, -19, -3, -1, 21, 17, 17, -22, -25, -10, -13, -10, -22, -2, -1, -28, 21, -7, -12, 2, 3, 5, -10, -5, -19, 13, 12, 12, -4, -14, -16, -7, -12, -7, -12, 12, -11, -1, 11, -14, -8, 6, -2, -16, 4, -2, -7, 11, -27, -3, -1, 14, -7, -2, -7, 13, -29, -5, 15, 6, -7, 7, 16, 8, -10, 11, 15, 23, -12, -4, 19, 9, 8, -21, -6, -3, -6, 13, -3, -6, -4, 9, 32, 15, -7, -5, 3, 34, 25, 12, 4, 21, -6, 30, 10, 7, 5}
, {-14, 19, 18, -6, -1, 16, 12, -18, -17, -25, -26, -32, -5, -6, 4, -10, 5, -6, -13, -32, 3, -23, 3, 22, -9, -2, -9, -7, 3, 3, -3, -24, -29, -3, 13, 13, 1, 11, -15, -17, 11, -7, 5, -11, 5, 20, -15, 9, 24, -3, 3, -20, 5, 13, -7, 26, 11, -15, 10, 21, 6, -11, 4, 6, -11, -8, 1, -10, 1, -6, -15, 23, 7, 5, 5, 13, 1, 18, -27, 24, 34, -8, 10, -2, -1, 15, -5, 12, -11, -1, 7, 12, 2, -6, -1, 12, -16, -24, -3, 7, -21, -12, 19, 2, 0, -17, 2, -23, -5, -11, 22, -6, 3, -15, 5, 18, -4, 6, 24, 1, -2, -3, -18, 16, 37, 13, -7, 16, 13, 3, -23, 14, -17, -10, -3, 8, 2, -13, 3, -25, 4, 18, 18, -27, -24, 5, -21, 14, 4, -8, 32, -21, -22, 7, 2, -2, 15, 23, 1, 0, 1, -18, 0, -13, 0, 24, 17, -27, -1, -9, -2, -32, 3, 21, -26, 12, -19, -2, -7, -24, 3, -6, 13, 7, 15, 4, -28, 16, -17, -8, 9, 10, -11, 7, 1, 5, -4, -1, 8, -6, -31, 4, 10, -22, -6, 13, -2, 10, 16, 14, 4, -19, -2, -1, 16, -14, 11, -8, 6, 6, 4, -20, 6, 4, 5, 6, 7, 5, 22, 16, 3, 10, -1, 5, 9, 0, 7, -9, -6, 16, -24, -2, -5, 5, 8, 20, 15, -28, 20, -2, 4, 20, 23, -23, -2, -11, -3, 13, -16, -14, 4, -10, 13, 22, -7, 8, 2, 15, 2, -12, -5, 23, 11, 14, 17, -10, -7, -2, -6, 15, 20, -3, 17, -8, 7, -7, 11, 12, 6, -7, 6, 2, 2, -5, -10, 29, 7, 15, 11, 28, 8, -8, 3, 2, 19, 18, 5, 7, -1, -13, 34, 3, -3, -8, -7, -13, -6, 17, 19, 12, -17, -19, -4, -20, -21, -4, -3, -16, -20, -19, -27, -16, 9, -9, -40, -15, 1, -37, -38, -4, -3, -38, -37, -7, -4, -4, -15, -14, -16, -20, -12, -12, -2, -6, -11, 5, -2, 3, -10, -22, -52, -12, -2, -39, -1, -13, -44, -8, -7, -29, -38, -4, -6, -13, -38, -5, 7, -5, -14, 1, -26, -4, -26, -10, -1, -2, 6, -21, -9, -1, 3, 2, -1, 5, 6, 2, 14, -2, -10, -5, -28, 4, -13, -36, -3, 12, -8, 1, 4, -6, -6, 9, -7, 14, -5, -8, -12, 7, 20, -13, -4, 13, 18, 5, 4, -3, 9, 7, 1, 30, -6, -2, 6, 13, 23, -1, 6, -6, 11, 11, 7, -6, -11, 7, 1, -13, 28, -8, -4, 10, 11, -1, 14, 1, 34, -4, 6, -17, -3, -5, -9, 30, 6, 1, 1, 1, 23, 8, -10, 7, 8, 16, -23, -6, 7, 23, -3, 17, 1, 15, -12, -17, -12, 6, -40, -10, -29, -11, -3, -42, -40, 0, 15, -45, -10, -49, 10, 1, -19, 5, 3, -31, -25, -7, 3, 10, -34, -32, 7, -3, -16, 20, -4, -33, -29, 0, -17, -49, -42, -13, -7, -14, -38, -43, 12, -37, 15, -46, 7, -11, -14, 16, 4, -28, 5, -12, -33, 6, -17, -15, 7, -1, -6, -22, 23, -24, 5, -18, 10, -16, -3, 8, 0, -3, 5, -12, 3, -13, -8, 21, -1, 15, 7, -17, -7, -12, -2, 13, -9, 4, -11, -13, -11, 14, -6, 2, -7, 2, 8, 1, 4, 10, 11, -6, 8, 8, 7, 2, 0, 10, -12, 15, -5, -4, 2, -6, -2, -22, -4, 24, -2, -3, 5, 25, -16, -5, 10, 6, 0, -6, -10, 9, 13, -4, -18, -2, 15, -8, 31, -6, 3, -8, -5, 13, -1, -27, 6, 11, 4, -3, -3, 21, 25, -7, -3, 18, -16, -5, -26, 8, -12, 29, 13, 11, -5, 13, -24, 3, 17, -3, -9, 17, 10, -11, -3, -17, -8, -4, 15, -7, 2, 11, -5, 9, -22, 6, 5, -17, -8, -22, 18, 25, -22, 44, 16, 2, -29, 2, 10, -17, -15, 12, 25, -1, 4, -5, 4, -10, -6, 9, 33, -15, 6, 40, 9, -33, -1, 5, 39, -22, 34, -12, 0, -2, 20, -3, 3, -14, -20, -1, 21, -42, -9, 15, 21, -24, 7, -13, 7, -6, -24, -17, -8, -5, 2, 35, -9, -38, 13, 1, 23, -13, 45, -6, -31, -31, 18, -13, 3, -33, 0, -5, 8, 4, -12, 11, 4, 21, 2, 4, -6, -4, -20, -29, -14, -10, 9, -8, -13, -34, -14, 7, -15, 6, -37, -8, -19, -3, 16, -12, -21, 26, 15, -30, -14, 21, 17, 9, -14, 25, 16, -20, 18, -11, -3, 21, 3, 1, 3, 4, -4, -16, 18, -6, -19, -5, 1, 30, 19, -19}
, {6, -9, 5, -8, -18, -19, -24, 4, -15, -29, -20, 1, 8, -18, -23, -26, -22, -13, -19, -13, 7, -48, -43, -7, -5, -6, -26, -17, -12, -23, 2, -5, -30, -45, -24, -18, -35, -40, -40, -6, 3, -22, -39, -7, 5, -5, -8, -20, -19, 4, -38, 4, 7, -41, -18, -8, -5, 7, -62, -28, -25, 8, -9, -23, -1, -32, -20, -3, -13, -60, -13, -38, -34, 25, 15, 23, 3, -17, -23, 21, -11, -52, -48, -4, -3, -6, -23, 20, -12, -2, -11, -16, -3, -6, -21, 1, 24, -6, 2, -5, -1, 6, -27, 4, 15, -22, -1, 0, 1, 33, 11, -6, 6, -9, -20, -6, 2, -42, -11, 28, 27, -3, -26, -20, -18, 20, 3, 8, 12, -9, -3, 15, -19, -16, 1, -42, -29, -10, -9, 16, 7, -8, -20, 2, 21, 11, -18, -26, 2, -23, 2, 10, 32, 2, -40, 1, -15, 5, 20, -16, -16, -26, -7, 6, -25, -11, 1, -8, -6, -10, -8, -9, 8, -41, 15, -43, 30, -22, -12, 7, 3, -2, -7, -17, -18, -7, -23, -14, -21, 13, -7, 3, -11, 0, -6, -8, 6, -5, -3, -19, -18, -22, -48, -11, -2, -14, -5, -33, 1, -17, 16, -8, 4, -20, -6, -19, -39, 0, -8, -7, -23, 6, -19, -11, 14, -28, -21, -2, -13, -33, 1, -22, -19, -6, -15, -6, 15, -27, 18, -13, 24, 8, -25, 20, 2, -32, 12, -32, -14, 7, -32, 4, -11, 29, -17, -7, -9, 23, 18, 0, 4, -13, -21, 3, 6, 14, 12, 14, -1, -14, 12, 6, -9, 11, 14, 11, -3, 1, 8, 0, 12, 9, -8, -11, 0, -16, -1, -8, -37, 14, 7, 18, -14, -8, 28, -18, -36, -3, -4, -26, 10, 12, 17, 3, 2, 6, -3, -18, -3, -14, 9, -5, -5, 0, -21, 9, -14, -33, 5, -17, -3, 4, 30, -11, 8, 12, 34, 0, -21, -10, 6, 7, -1, -6, 1, 3, -4, -28, 2, 16, -1, 21, -1, -3, -5, -8, 20, 18, 18, 5, 8, -33, 9, -12, -6, 17, -7, -8, 2, -4, -3, -1, 29, 8, 4, 5, 43, -9, -13, -21, -5, 40, -8, 12, 25, 13, 14, 8, -1, -21, 7, 7, -19, -9, -4, 21, 18, -17, -15, 48, 32, 2, 5, -12, -9, 10, 14, 32, 35, 9, 27, 43, 28, 26, 8, 38, 37, 28, -9, 3, 23, 1, 60, 14, 18, 9, 33, 1, 18, 18, 16, 16, -5, -22, -20, -32, 10, -7, 2, 13, -10, 12, -7, 8, 3, -19, 0, 48, 21, -8, 22, 3, 41, 16, 44, -15, 11, 2, 5, -16, -16, 7, -39, -25, -17, -2, -8, -39, -15, -7, 14, -4, -4, -13, 2, 16, -31, -14, -6, 2, -14, -12, 4, 6, -21, -7, 4, 12, -5, 0, -13, -34, -20, -22, 30, -15, -8, -31, -22, 23, 7, -8, -4, -30, 1, 30, -25, -16, -50, -31, 5, -32, -15, -33, 31, 2, -31, 21, -3, 7, -17, -28, -4, 9, 29, -35, 15, 15, -17, 7, 18, 25, 14, 8, 11, 22, -11, 33, -46, 20, 29, -22, -4, -55, 5, 2, 26, -4, -25, 26, 2, -9, 28, -48, 14, 10, 17, 9, 40, 31, 12, 4, 11, 8, 8, 18, 6, 21, 6, 45, 3, -2, 29, -24, 4, -5, -3, 3, 8, -8, 5, 25, 3, -22, 4, 19, -2, 11, -15, 10, -3, 4, -19, -30, -31, -21, 7, -43, 8, -2, -41, 5, -2, -12, -16, -54, -11, 17, -32, -21, 15, 11, -28, 11, -2, 5, -26, 3, -28, -13, -4, -25, 4, 14, 2, -2, 9, -2, 1, -19, 0, -17, -20, -1, -41, -10, -23, 16, 5, 6, -33, 22, -24, 10, -7, -4, -8, -17, -3, -7, 19, 24, 24, 16, -3, -22, -1, -25, -10, 8, 5, 22, 8, 10, 1, 5, 4, 29, 28, -21, 3, 10, 14, -34, 3, -8, -8, -2, -16, 27, 1, -20, -10, 10, -2, 43, -5, 8, -12, 8, 7, -10, 1, 1, 17, 11, -13, 6, -12, 15, 2, 2, -7, 11, -15, 3, 19, 3, 23, -1, -2, -17, 18, 2, 1, 14, -9, 6, -5, -12, 13, 13, 19, -4, 11, 13, 4, -10, -11, -6, 12, 10, -1, -18, -7, 1, -37, 33, 5, -3, 1, -4, -2, 1, 2, 19, 23, -29, -46, 17, -16, -35, -22, -17, -33, -29, -24, -12, 8, -12, 7, -21, 1, -60, -43, -13, 7, -14, -33, -35, -16, -2, -35, -4, -21, 27, -35, -2, 14, -20, -6, -50, -14, -14, -8, -12, -20, 0, -26, -9, 1, -25, -18, -23, -27, -18, -22, -9, -1, -27, -11, -11, -8, -1, -19, -15, 4, -12, -14, -20}
, {0, 20, 19, -1, 36, 17, 12, 37, 25, 13, 22, 25, -4, 32, -15, 29, -3, -3, 14, -35, -6, 8, -16, 59, -7, 3, 30, 12, -10, -32, 16, 38, -31, -5, 30, 31, 25, 12, 5, 14, 34, 22, 28, 25, 7, 11, -49, 41, 26, -6, 4, -43, 0, 17, -19, 7, 10, 2, 38, 22, 4, -13, 5, 26, 8, -22, 3, 4, 13, 5, -13, 22, 13, 25, 6, -2, -3, -20, -2, 3, 9, -2, -9, 0, -8, 8, 12, -45, 10, 7, 17, 19, -8, 16, -21, 34, -4, -7, 3, 3, 9, 2, -12, -16, 2, 5, -3, -5, -1, -13, 24, -17, -4, 7, 16, 17, 2, -1, 16, -12, -30, 6, 14, 0, 15, 11, 4, -8, -10, 12, 12, 0, -7, 5, -2, 4, -10, -16, -11, 11, 4, 8, 8, -24, 6, 21, 11, -21, 9, -31, 13, 38, -5, -1, 20, 2, 19, 20, 17, -14, 5, -7, -1, -6, -12, -8, -7, 22, 10, 2, 16, 0, 2, -13, 11, -2, -5, -15, 7, 16, -11, 4, -8, -51, -4, -9, 5, -6, -1, -8, 6, 4, 0, -4, 23, 1, -4, -3, -11, -2, 4, 13, 11, 11, 2, 3, 22, 37, 24, 15, 0, 10, -3, 14, 9, 9, 10, 8, 3, 6, 2, -10, 1, 1, -1, 6, 6, -15, 5, -14, -7, 12, 11, -18, 8, 13, 2, -8, 9, 0, -10, -4, -9, 41, 1, 4, 7, 3, 12, 8, -10, -2, 13, 3, 20, -7, 18, 5, -4, -10, -15, -17, -1, -3, -10, -1, -2, 0, 2, 25, -6, 11, 1, 2, 16, 2, -6, -6, -3, 20, 10, 1, -8, -3, -9, -8, 0, 10, -13, 2, 10, 24, -21, 2, 2, -2, -8, 9, -6, -13, 6, 4, -6, 7, 18, 1, 9, -8, -11, -5, 12, 15, -3, 7, 15, 10, 3, -22, -4, 9, -4, 12, -4, 10, 21, 11, 5, -5, -12, 15, 8, -12, 8, -1, -12, 3, -31, 13, -2, -16, 7, 7, 3, 7, -14, 0, 11, 9, -11, -8, -12, 3, -13, 11, -2, -13, 16, 35, 9, 7, -19, 5, 4, -14, 11, 0, -18, -6, -11, 16, 2, -14, -7, 2, -34, -28, -6, -3, -4, -1, -5, -19, 0, 7, -9, 11, 11, -17, -3, 19, 2, 26, 8, -4, 1, 2, -6, -2, -13, -6, -31, 10, 7, -3, 3, 13, -5, 7, -2, 1, 9, 18, -18, -25, 22, 15, -28, 15, 25, -15, 7, 24, -4, 12, 6, 5, 2, 14, 1, 10, -18, 1, -6, 18, 26, -10, -6, 11, -3, 0, -3, -4, 12, 0, -17, -52, 11, 14, -15, 8, 7, 14, 9, 18, -21, 5, -10, 11, -4, -6, 12, -23, -29, -6, 5, 3, -5, -32, 8, 5, -18, 0, -5, 5, 5, 2, -6, -36, 1, 6, -5, -19, -27, -2, -2, -17, -6, -25, -9, 8, -16, -11, 8, -30, -21, -15, -9, -31, -38, -7, -9, -25, -15, -8, -2, 3, -38, -8, -3, -1, -22, -8, 12, -20, -25, -23, 8, -18, -17, -12, -16, -12, -26, -20, 11, -12, -7, 3, -5, -5, -18, 18, -4, -20, -16, -4, -5, -4, -16, -5, 5, 23, -3, -28, 5, 1, -3, -6, 3, 12, -3, 11, -17, 12, -13, -14, -11, -7, -25, 13, -6, 11, 14, -23, -9, -16, 8, 5, -12, 3, 10, -15, 8, 7, 2, -13, 5, -1, -11, 4, -29, 7, -35, -22, -16, -9, -3, -16, 10, 1, -2, -8, -1, 2, 7, -25, -3, -5, -7, 7, 0, 7, -3, -15, -7, -5, 8, 14, 7, -13, -22, 4, -33, -3, -45, -7, 0, -12, -40, -9, 0, -16, 0, -24, -9, 1, -12, -55, 5, -20, -19, -7, -17, -9, -17, -12, -8, -13, 0, -6, 4, -7, 1, 14, -3, 1, -7, -23, -14, 3, -7, 25, -1, 9, 4, 1, 20, 13, -10, -28, -8, -19, 7, 5, -6, -1, -2, 17, 20, 23, 4, -12, 6, 8, 12, 15, -1, 3, -8, 21, 6, -30, -15, 18, 7, 12, 5, 4, -7, 20, 18, 3, -3, -32, 20, 17, -31, 6, -11, -17, -8, -9, 16, 23, -19, -2, 12, -6, -20, 10, 14, 8, 6, 23, -8, -15, 1, 13, 4, -11, -21, 10, -3, -4, -7, 0, 1, -2, -13, -3, -8, -6, -11, -19, 7, 5, -25, -19, -6, -18, -13, -18, -21, 10, 14, -1, 22, 14, -9, -12, -10, -6, -23, -11, -16, -2, 5, -3, -19, -18, -22, -11, -23, -29, -16, -42, -14, 17, -40, -42, -33, 3, -12, -31, -30, 6, 12, 16, 13, 16, -4, -11, 25, -19, -10, -24, -24, -3, 9, -21, -15, -5, -27, 7, -11, -12, -32, -27, -20, 3}
, {11, 3, -3, -4, 14, -5, 12, 21, 22, 5, 30, 21, 2, 23, 23, 15, -6, 19, 24, 18, 6, -1, 6, -1, 22, 8, 13, -3, -1, 2, 7, 48, 0, 1, -4, 16, -11, 1, -15, 14, 22, -28, -32, 7, 4, 12, 24, -9, 20, -17, -29, 15, 7, 7, 7, 3, 15, 4, -19, 9, 5, 15, 3, 0, 2, 9, -20, 31, -14, -14, 11, -22, -35, -14, -6, -35, 8, -24, 1, -34, 9, -24, 4, -15, -4, -5, 11, -20, -11, 8, -4, -4, 27, 7, -7, 10, 26, 11, 17, -17, -1, -4, 17, -9, 1, 29, 13, 36, 11, 2, -11, 12, -22, 6, 17, -22, -6, -2, 15, -7, 12, 7, 4, -6, 13, 10, 0, -21, -5, 18, 25, -16, 13, 15, 1, -1, 7, 38, -16, 12, 1, 9, -3, 29, 26, 15, 19, 10, -1, 10, -1, 17, 39, -10, 18, 1, -2, 1, 28, 8, 20, 15, -2, 2, 1, -3, -10, 9, -13, 22, 12, -14, -7, 0, 6, 15, -8, 6, 1, 0, -7, 22, -11, 21, -7, 1, 1, 6, 7, -4, -2, 29, -7, -12, 7, 10, 1, 5, -2, 4, 20, -11, 17, 29, 1, 26, 11, -2, -18, -5, 3, 17, 4, 2, 3, 15, -10, -6, 23, -11, 15, -16, -8, -8, 1, -25, -20, 3, -29, -14, 0, 13, 8, -20, -29, 18, 5, 8, 21, -18, 8, -16, -4, 13, -8, -8, -6, 22, -3, 9, -33, -17, 3, 26, 25, 9, 0, -2, 34, -30, -9, -1, 8, 1, 12, 9, 9, 13, 5, 9, 13, 1, -6, 19, 23, 6, 2, -3, 39, 20, 9, -4, -4, -3, 12, 20, 22, -16, 1, 38, 13, 11, 3, 19, 26, -17, -7, 8, 14, -9, -2, 10, -11, 11, 2, 7, 7, -18, 3, 10, -13, 1, 14, 5, 31, 14, 5, -7, 9, 4, -9, -13, 0, 18, -8, -12, 7, -36, -34, 8, 1, 0, -10, -6, 7, 0, -14, -23, 8, 33, -1, -1, -3, -35, 16, 5, 14, -9, 25, 14, -4, -21, 9, 0, -16, -3, 2, -30, -4, 17, 27, 20, 12, 32, 0, 1, 28, 11, -29, 19, 32, -2, 11, 2, 22, 25, 7, 9, 16, -25, 32, 5, 20, -13, -19, -12, -1, -10, 6, 12, 20, 26, 4, -10, -4, 10, 0, 1, 17, 5, 23, 4, 12, 15, 5, -6, 14, 0, 9, 10, 21, 13, 16, -8, 23, 9, -2, 11, -4, 13, 14, 17, 17, 9, -11, -4, 10, 0, -5, 0, 1, 14, 2, 9, -3, 17, 9, 21, -9, 28, 15, -7, 16, 32, 2, 9, 6, 4, -1, -2, 1, 6, -19, 8, -3, -3, -2, 18, -8, -7, -14, 4, 9, 2, -4, -20, -32, -6, 4, 13, -11, -16, 28, 0, -28, 17, 14, -7, -6, 1, -5, -16, -4, 11, -12, -3, -3, -26, -37, 10, 2, 6, -5, -7, -5, -5, -11, 3, -17, 7, -8, -7, 3, 7, 7, -3, -19, -5, -11, -8, -2, -20, -8, 6, 17, -13, 5, 13, 3, 7, 14, 20, 12, 16, -16, 27, -19, 26, -20, 27, 24, -20, -8, -6, -6, 21, 2, 2, 4, -8, -33, -35, 28, -10, -42, 11, 20, 14, -4, 27, 10, -11, 7, 26, 41, 9, 1, 13, 7, 24, -19, -18, 15, -2, -4, 6, 0, -1, 13, 7, -3, 8, -36, -33, -5, 6, -5, -5, 14, 18, -15, -4, -28, 18, 13, -15, 2, -14, -7, -2, -1, -8, 12, -12, -6, 0, -5, 5, -15, 12, -6, 4, -24, -7, -3, -6, -17, 23, -7, -30, -1, -6, -5, -10, -18, 31, 3, -11, -3, 29, -4, -15, -20, -21, 7, -12, -12, -10, 6, -3, -14, -1, -6, 7, -5, -20, -6, 0, 2, 3, -6, -23, -8, -8, -1, -29, 13, -9, -14, -3, -20, 5, 3, -17, -23, 1, -16, -9, -4, -37, 7, -19, -9, -13, -17, -9, 6, 8, -14, -32, -27, -29, -20, -20, -12, 18, 19, 4, -14, -11, -18, 6, -14, -12, -20, -10, -49, 14, -26, -10, -8, -34, -6, -35, -28, -12, -3, -9, -17, 25, -33, -33, -24, -3, -24, -16, 6, 11, 0, 9, -9, -13, 0, 51, 17, -16, -8, -2, -51, 2, -23, -11, 25, -33, -2, -27, -31, -29, -2, -5, -16, -19, -9, -32, -20, -18, -33, -4, -18, 14, 7, 8, -6, 14, 10, 7, 21, -9, -9, 4, -27, 9, 0, -10, -10, -18, -5, 2, -34, -22, 10, 5, 4, 6, 5, -21, -10, 2, -4, -6, -1, -13, -9, 0, -19, -18, -14, -27, -15, 13, -5, -2, -48, -21, -7, -10, -8, -9, -8, 10, -33, 10, -16, -13, 5, -12, 1, -8, 3, 21}
, {-2, 12, 23, 6, 38, 45, 26, 29, 20, -12, -1, 36, 5, 55, 11, 48, 7, -3, 16, 11, 9, -7, 16, 48, 1, 4, 24, 42, 13, -21, 19, 18, -30, -2, 11, 11, -2, -3, -4, -2, 18, -8, 10, -9, 5, 20, -4, -2, 15, 1, -22, -9, -7, 17, 8, 25, -4, -2, 6, 25, 12, 4, 23, 11, -14, 19, -16, -30, 11, -15, 13, -22, 7, -3, 2, -31, -7, -10, 10, -1, 1, 4, 8, 21, -4, 26, -21, -19, 3, 10, 13, 2, -4, 6, -15, 11, 17, -28, -19, -14, 15, -1, -10, -6, 0, -11, -4, -23, 6, -10, 19, 7, 0, -2, -4, 33, -5, 29, -17, -29, 7, -9, 7, -2, 13, 9, -4, 0, -4, 21, 11, -10, -7, 1, 0, -10, -4, -23, -6, 4, -8, 14, -6, -15, -21, 1, 2, -6, -10, 15, 20, 11, -23, 0, 26, 2, 5, 15, 0, -8, 0, -2, 7, -18, 14, 10, 8, 25, 18, 13, 6, 36, 3, 11, -14, 11, 18, 11, 1, 35, 0, 5, 2, 17, 12, -6, 12, -3, -8, -9, -3, 24, -9, -5, 12, -9, 4, 5, -21, -2, 3, 8, 2, -19, -7, 6, -6, 3, -6, 8, -9, 5, 6, 2, 2, -6, -13, -8, 8, -7, 0, 1, -1, 7, 10, 7, -4, -4, 8, -2, 0, 14, 1, -4, -2, -4, -7, -16, -17, 4, 13, 7, -7, 21, -3, 18, -10, -23, -11, -1, -16, 8, 10, 17, -9, 7, 23, 2, 16, -6, 6, -2, 6, -12, -11, -4, 10, -2, 2, 19, -7, -2, 1, -7, 20, -26, -5, -4, -6, -7, 6, -4, -12, 3, 4, 20, 4, -17, 15, -13, -1, 8, 4, 13, 6, -3, -18, -10, 18, -3, 0, -2, 2, 6, 11, 1, -14, 0, 1, 12, 9, -2, 16, -2, 4, 2, 15, 26, -18, 14, 0, 26, 8, 25, 24, -2, -11, -17, -7, 3, 4, 4, 7, 2, -21, 10, 7, -5, -9, -15, -10, 2, -26, 7, -11, 6, 30, -2, -19, -8, 1, 32, -4, -26, -15, 9, -10, 2, 3, -2, 0, 0, 7, -9, -8, -25, -16, 3, 27, -11, -17, -13, 6, -8, -44, -34, 2, 0, 6, -9, -9, 5, -35, 24, 8, 12, -10, -13, -9, -7, 2, -11, 5, 6, -7, 3, -11, -7, -1, -9, -13, -1, 9, -15, -7, 30, 3, 0, -7, -8, -13, 13, -12, 13, 2, -8, 1, -14, -3, 21, -2, -14, -14, -5, 13, 15, 3, -10, -6, 2, 24, 8, 9, -19, 13, -9, 2, -9, 9, -4, -7, 0, -10, 10, 11, 16, 2, 3, 10, 15, 9, 7, 10, 11, 3, 5, 3, 13, 15, 36, 10, 0, 0, 6, -6, 1, 23, 13, 4, -8, 10, 16, -7, 6, 10, 7, -13, 11, 13, -2, 19, 2, -11, -1, -16, -18, -26, -14, 11, 1, -9, -6, 6, -15, -1, 4, 40, -40, -27, 35, -4, 0, -15, -26, 8, 2, -6, -2, -7, 2, -28, 24, 17, -8, -15, -21, 15, -22, 12, -19, -7, 10, -1, -23, 7, -21, 20, -5, -5, -21, -1, 37, -10, 24, -5, -31, -23, 3, -14, -12, 17, 15, -16, -15, 34, -4, 22, -1, 12, -2, 34, 6, -11, 5, 6, -22, -8, -19, 17, 16, -1, -6, -6, -3, 7, 1, 5, -18, 4, 7, -3, -5, 11, 19, -6, -8, 11, 29, -2, -10, -26, 2, 5, 0, 5, -5, 3, 36, -1, -12, 13, -16, -10, 22, 0, -1, 6, -5, 9, 5, 7, -3, 16, -24, -17, -9, 38, -5, 0, 16, 6, -1, 2, 0, 5, 9, 2, -5, 8, 23, 8, -12, 21, -4, 9, 3, -3, -3, 0, -7, -2, 19, 11, 6, 5, 13, -1, -20, -15, 7, 1, -9, -6, 9, -4, 5, -11, 16, 4, -20, 0, -2, 4, 7, 6, -5, 34, -4, -13, 35, 6, -14, 14, 30, 4, -13, -4, -8, 18, 15, 27, 35, 22, 20, 22, -17, -21, 18, -11, -5, -7, 14, -6, 2, -2, 9, -4, -4, 3, 3, 3, 20, 3, -8, 3, -5, 14, 4, -21, -10, -4, 16, 16, 10, -14, 2, 31, 10, 0, 24, 4, -23, -8, 7, -9, -32, 2, 2, 16, 20, -7, 0, 7, -1, -5, -2, 17, -19, 21, 10, 4, -5, -23, 12, -1, -5, -5, 6, -16, 0, 20, -11, -2, 3, -21, -20, -8, 19, -5, -3, -15, -3, -24, -5, -11, -9, 3, -29, -20, -4, -7, 2, -16, -4, -32, -11, 8, -14, 4, 0, -39, 7, -9, -9, -36, -13, -4, -18, -33, 5, -4, -15, -7, -5, -6, -10, 7, 5, 6, -13, -29, 15, 1, -8, -13, -1, -27, -27, -4, -21}
, {-23, 21, 44, -3, 41, 40, 10, 17, 13, -11, 15, 20, 6, 25, -7, 22, 12, 2, 4, -28, -11, -13, -11, 54, 14, -12, 21, 2, 4, -9, 17, 10, -22, 13, 25, 39, -14, 7, -7, -15, 13, -9, -7, 4, -4, 15, -41, 9, 18, 21, 19, -31, 5, 4, -13, 26, -12, 0, 17, 0, 14, 4, 11, -2, -17, -9, 13, -3, -5, 3, -20, 14, 25, 1, -5, 20, -4, 1, -3, 25, 18, -9, 9, 3, -6, 16, -23, 5, -11, 2, 21, -17, 7, -9, -12, 15, -6, -31, -17, -4, -27, -13, -5, -11, -24, -28, -21, -37, -12, -26, 23, -25, 26, -4, -3, 25, -11, 15, 8, -22, -11, 0, -14, -13, 22, 2, -19, 24, 31, -7, -15, -16, -23, -23, -21, -17, -6, -20, 8, -19, -8, 19, 32, -30, -20, -13, -8, -10, 7, -5, 23, 6, -21, 3, -9, -8, 17, 49, -7, -6, 12, -22, -21, -8, 14, 3, 17, -4, 10, 4, 16, -13, 5, 14, 0, 4, 3, 4, 5, 0, -11, -11, 19, -28, 28, 4, 5, 16, 12, 10, -3, 13, 8, 0, 11, 24, 0, 8, 1, -3, -7, 14, 9, 4, -7, 8, -2, 1, 8, 1, 11, 16, 2, -6, -11, -9, 5, -6, 7, 9, 25, -14, 22, 5, 8, -3, 13, -15, 23, 3, -7, 21, 23, 16, 19, -24, 7, -10, 21, 15, -2, -2, -1, 10, -11, 6, -1, -8, 5, -12, 9, 11, 22, -5, -13, -4, -6, -1, -9, 6, -8, -5, 15, -5, -16, -11, -8, 13, 9, 19, -3, -1, -9, 6, 19, -12, -7, 6, -8, -8, -12, 1, -4, 14, 18, -10, 13, 4, -14, -10, 2, 5, -7, 9, -3, 6, -19, 20, 14, 16, 7, 9, 0, -7, -9, 19, 5, 25, -1, 16, 18, 23, 2, -3, 8, 3, 18, -9, 33, -5, -6, -19, 1, 10, 32, -13, 28, -25, -15, 7, 12, -1, 6, -8, -4, -5, -11, 6, -5, -29, -10, -15, -3, -35, -8, 5, -16, 14, -4, -22, -9, -22, 10, -3, -19, 11, 15, -4, -15, -6, -18, 3, -8, 6, 9, -17, 0, -11, 18, -31, -16, -9, -3, -20, -34, -9, 0, 10, -5, 12, -10, 3, -33, -14, 22, 3, 7, 4, 15, -21, 5, 18, 17, 8, -4, 21, 12, -17, -4, 9, -20, 7, 0, -16, -10, -9, 18, -1, -1, -9, -14, -17, -16, 7, 5, -1, -6, 23, 12, 15, -8, 16, 22, -16, -12, -13, -31, -9, 7, 31, 4, -7, 0, 0, 11, -30, 6, -13, 11, 2, 14, -8, -22, 25, -11, 3, 9, -16, -14, 32, 19, -4, 27, 15, -2, -14, -6, -2, 0, -1, 1, 27, -34, 9, -16, 10, 5, -23, 9, 7, -11, 24, 0, 7, 0, 35, -11, -20, 8, -33, -8, -8, 0, -3, -9, -32, 7, 3, -14, -3, -28, 1, 6, -3, 12, -13, 11, 20, -8, 16, -9, -27, 13, 16, 2, -3, -26, -16, -10, -2, -10, 5, 7, 15, -5, 4, -7, 7, 11, -19, -17, -15, -11, -22, 9, -12, -1, -2, 8, 6, 7, -3, -7, 6, 21, -2, -5, -3, 1, 2, 8, 21, -3, -6, 8, 1, 21, 3, 4, 9, -3, 8, -1, -6, -8, -12, -4, 1, -1, 12, -7, -7, 7, 3, 8, 10, -1, 12, 12, 10, 14, -15, 16, 6, 2, -11, 16, -14, -15, 11, -15, -8, -34, -15, -12, -19, 4, -34, -2, -14, 5, -17, 1, -21, -8, 3, 4, -7, 1, -18, 7, -7, 9, 0, -3, 14, -17, -17, 27, 7, -11, -29, -19, -13, -8, -3, -5, -12, -25, -12, 2, -17, 10, -21, 0, -17, 7, -24, 1, -14, 17, -5, -12, -9, -3, -24, 18, 11, -30, 6, -21, -3, 12, 2, 13, 15, 0, -17, 15, -4, 9, -3, -3, 7, 8, -15, -17, 2, 9, -30, -10, 10, -13, 7, -9, -5, 14, 0, 6, -13, 1, -6, -8, 1, 25, 4, -17, 10, 24, -1, 17, 9, 21, 35, 3, 10, 12, 15, 6, 4, 21, -25, -9, 19, 24, -12, 13, -2, 10, -3, 20, -2, 8, 15, 19, -8, 5, -24, -22, -2, 30, -8, -7, 28, 10, -13, 1, -5, 7, 11, -22, -20, -16, -17, 9, -5, 7, -23, 23, 3, -5, 10, 10, -8, -4, -3, 23, -3, -4, -3, 7, -17, 1, -7, -5, 17, 3, 5, 0, -5, -9, -5, -19, -7, 1, 1, 1, -5, 2, -7, -23, 6, -16, 7, -5, -5, -20, 3, 5, -11, -11, 12, 8, -22, -12, 5, 22, 31, 3, 46, 2, -18, 15, 10, 6, 11, 35, 15, 6, 0, 4, -40, 11, -11, 3, 9, -1, 8, -3, 3}
, {6, 2, 7, -4, -9, -7, 2, -2, -4, 8, 7, -12, 7, -8, 6, -2, 5, 13, 6, -7, -4, 9, 2, 3, -10, -12, -3, 4, 4, 6, -2, -10, -4, -13, -10, -10, -1, -7, 0, 7, 6, 4, 7, -6, -10, -12, -5, 2, 4, 8, -7, 5, -4, -6, -14, -9, 0, -9, -5, -3, 3, -3, -8, -8, -11, 0, -12, 3, -10, -8, -2, -8, -9, -5, -7, -11, -8, 1, -5, 6, -15, 3, -4, 4, -10, -12, -15, -8, -11, -3, -9, -7, -15, -20, -9, 9, -11, -6, 9, -13, 5, 0, 20, -1, 3, -1, -4, -5, -3, 2, 8, 2, -12, 6, 9, 2, 0, -6, 6, 1, -5, 6, 5, -8, 11, -18, 1, -15, 0, 8, 15, 17, -6, -8, -6, -11, 5, 4, -7, 5, 5, 9, 1, 3, 1, 2, 6, 7, 0, 4, 5, -2, -16, -9, -8, 0, -4, -13, -2, -11, 3, 10, -15, -15, -10, -10, -8, -7, 1, -12, 5, 12, -7, -3, -6, 1, -7, 0, -5, -7, -8, -5, -5, 7, -4, -10, -5, 2, 4, -5, -8, -4, 0, -3, -1, 3, 2, 0, -3, 3, -9, 5, -9, 0, 6, -5, -11, -9, 10, -7, 2, -4, -5, -8, -3, -11, -9, -9, -10, -7, -5, 0, -9, -3, 2, -1, 5, -5, -12, -3, -8, 2, 3, -16, -1, -6, 6, -12, 3, -17, 8, -5, -10, -2, -5, -3, -13, 4, -11, -11, 9, -7, 9, 0, 7, 0, 4, -12, -6, -6, -9, -15, -9, 4, 6, 10, 12, 1, -7, -3, -15, 8, -12, -5, -8, 7, -12, -19, -2, 6, 9, -4, 4, -13, -8, 0, 5, 4, 2, -10, -5, -2, 6, -12, -7, -3, 0, -2, 2, -3, -9, 2, -3, -4, -14, -5, -18, 5, -11, -16, -5, 2, 3, -3, -7, -6, -7, 8, -12, 6, -10, 1, -6, -1, -6, -4, -17, 1, -9, -13, 5, 2, -5, -1, -2, -8, 9, -8, -14, -7, 7, 10, -7, 7, -3, -9, -3, 2, -3, -2, -2, -6, 4, -2, -6, -2, -14, -7, -5, -4, -6, 0, -5, 5, -9, 4, -10, -7, -13, -8, -17, -9, -12, -11, -2, -10, -9, -2, 3, -10, -12, 3, -13, -9, 5, -1, -14, 2, -6, -4, -16, -7, -2, -8, -7, -2, 8, -6, -2, -5, -6, -12, -1, 5, -3, 4, -3, 0, 4, -10, 6, -1, 4, -15, -10, 1, -12, 8, -5, -15, 1, -12, -3, -4, -12, -1, -14, 0, 5, 3, -3, 5, 0, -3, -17, -6, 0, 6, -17, -9, 2, -3, 4, -14, -12, 6, -8, -3, 2, -2, -10, -2, 3, -6, -9, -3, -11, -3, -8, 1, -2, 0, 1, 4, -7, -5, -13, -8, -1, 6, -10, -7, -5, -11, -2, -5, -9, -1, -4, -2, -7, -12, -2, -11, -10, 5, 1, -14, -3, -4, -4, 4, 8, 4, -4, 0, -3, -2, 2, 10, -11, 9, -11, -3, 2, 2, -2, -12, 3, 7, -5, 6, 5, 3, -1, 0, -13, -12, -12, -5, -9, -8, 3, 2, -6, -6, 4, -1, -8, 5, -14, -5, 6, -3, -6, 6, -6, 0, 5, 0, -12, 4, 4, 3, -6, 6, -4, -10, -2, -3, -5, -2, -8, -7, -9, -10, 0, 4, 3, -4, 6, -5, 5, 7, 4, 0, -2, 0, -2, 3, -2, -2, 4, -1, -10, 3, 8, -10, -2, -10, 2, -2, 1, -12, -3, -4, -12, 6, -9, 0, 9, -4, -12, -7, -3, -1, -2, -11, 0, -8, -1, -1, 4, 2, -7, -5, 4, -10, -10, 10, 1, 3, 4, -4, -13, -1, -4, -4, 6, -8, 4, 2, 4, 8, -17, -5, -2, 0, -2, -9, -1, 1, 2, 9, -10, -6, 1, -10, -13, -3, 8, 2, -8, -5, -2, -3, -13, 4, -2, 7, -7, 5, 4, -7, 5, 4, 11, 4, 2, 2, -10, 4, 6, -4, -10, 1, 3, -14, 3, 5, 5, 9, 7, 9, -15, -16, -11, -11, -12, -1, 11, -13, -13, 6, -2, -15, 2, -1, -8, -1, -15, 7, 0, -16, -11, -7, -14, -9, -10, -7, -6, -11, 4, -1, -1, 3, -17, -3, -14, -3, -12, 8, 9, -10, -1, -3, 6, 6, 4, -7, -3, -8, -15, -8, -2, 0, -17, 6, -9, -8, -13, -8, -7, -23, -2, -1, 3, -2, -12, -5, 7, 8, -10, 9, 9, -4, -5, 10, -6, 4, -4, -14, 0, 11, -5, 1, -4, -7, 3, -5, -16, 1, -12, -16, -3, -8, 1, 2, 3, -15, -9, 2, -2, 1, -6, 2, -7, -6, -9, -14, 10, -14, -2, -8, -4, -5, -7, 4, -10, 0, -22, 1, -4, -3, -10, -8, -9, -17, 4, -9}
, {-20, -6, -4, 20, -10, -10, -15, 4, 0, -5, -11, 2, -6, -3, -8, -7, 3, -3, -5, 9, -1, 8, -6, -1, -1, -6, -2, -5, -16, -12, 4, -4, -2, 2, -9, -5, -2, 4, -9, 9, -2, -12, -21, 2, 1, 2, -2, -7, 2, -6, -14, 12, -7, -5, -3, 9, 7, 2, -17, -23, -12, -2, -20, 0, 3, -1, -1, -7, -14, -4, -7, -11, -5, -9, -29, 0, 4, -3, -18, -22, 9, -3, -9, 3, -8, -17, -5, -10, 0, 5, -25, -11, -20, 7, -2, 3, -3, -6, 6, -14, 18, 1, -9, 11, -8, -11, 7, 5, 5, -5, -13, 10, -10, 1, -8, -9, -10, 0, -18, 2, -17, 0, -4, 0, -8, 6, -12, -15, -16, 23, -5, 6, 0, 8, 1, -5, -5, 22, 16, 15, 7, 9, -16, 17, -19, -3, -13, 11, -6, 8, -12, 1, 21, -7, 11, 17, -27, -32, 4, -17, 23, 1, -3, 21, -7, -16, -10, 5, 1, -20, -10, 38, 7, 11, 4, -16, 47, 14, 10, 8, -2, -22, -2, 30, -7, -10, -16, -8, -5, 27, 18, 40, -18, -7, 13, 5, -8, 10, 3, -1, -10, -9, -10, -9, 7, -11, 1, -17, 12, 1, -15, 11, 5, -3, -9, 16, -21, 4, -6, -11, -11, 0, 7, 6, -9, -9, -11, -8, 3, 0, 6, -8, -7, -20, -21, 1, 6, -1, -12, -18, 5, -17, -12, 2, -1, 0, -13, -16, -11, 1, -2, -9, -9, -17, -14, 2, 12, 5, -17, -5, -3, 4, -3, -5, 2, -5, -17, 1, -4, -16, -3, 5, 3, -11, -12, 9, -7, 6, -1, 7, -2, -8, -3, -16, 8, 4, -1, -7, 8, -7, 10, -12, -7, 5, -6, -5, -18, -5, -10, -23, 6, -16, -4, -11, 10, -13, -25, 7, -9, 5, -19, 4, 0, 3, -3, -1, -4, 17, -20, 1, 7, 24, 37, 39, -19, 45, -5, 39, 40, -19, 3, 28, 6, 31, 19, -16, 32, 9, 14, 23, -6, 20, 9, 43, -17, 0, 50, -16, 17, 6, 4, 44, 10, 6, 5, 16, 3, -4, -10, 2, 17, -5, -6, 0, -9, -19, -8, -4, 23, -8, -16, 8, 3, 7, 10, 18, -6, 4, -3, -10, 0, 6, -2, 13, 2, 2, -2, 7, -1, -3, -9, -7, -9, 3, -5, -2, -1, -11, -10, -10, 14, 14, -1, -4, 9, 8, 12, 9, 1, 9, 0, -3, -7, 10, 4, 12, 5, 3, 2, 0, 18, 8, -8, 11, 16, -9, -3, -1, -5, -14, -7, -13, 20, -6, -7, 12, -2, 1, -18, 1, -16, 3, -3, 4, -3, 3, 7, 4, 10, -1, 1, 5, 4, -6, 2, -5, 8, -9, 2, 7, -5, -7, -5, -2, 17, -11, -18, 24, -1, 17, -1, -1, 2, 2, 1, -8, -13, 15, -19, -5, -6, 10, 0, 62, 19, 15, -13, 50, 31, -20, 7, 42, -5, 21, 13, 33, 33, -5, 4, 35, 7, 8, -5, 36, 53, 7, 51, 1, -25, 16, 37, 45, -5, 3, -3, 9, -4, 6, -3, 14, 9, 11, 10, 12, -3, 6, 16, 3, -16, 19, -3, 24, 2, 21, 13, 12, 0, 11, 6, -5, 0, -8, 18, 3, -22, -10, 6, 15, -6, 22, -4, 13, 8, -2, -12, -20, -7, 4, 8, 0, -9, -13, -10, -7, -3, -11, -9, 15, 2, 1, -11, 2, -14, -20, -2, 15, -11, -24, -18, 14, 3, 6, -3, -7, 9, 10, 4, -5, 10, -11, -1, 9, -12, -21, -12, -1, -3, -12, -31, -5, -4, 8, -20, -1, -12, -13, -16, -16, 4, -25, -19, -10, -4, -23, 5, 6, 15, 21, 19, 22, 7, -15, 8, 8, -14, -26, -19, 32, -1, 5, -11, 9, -4, -4, 11, -3, -1, 3, -11, -3, -5, 9, -3, 39, -10, 1, -9, 48, 35, -13, -6, 32, 1, 14, 34, 0, 39, 16, 18, 45, 1, 8, 11, 35, 13, 3, 22, -13, -11, 1, 26, 45, 8, -10, -5, 18, -8, 3, -11, 22, 8, -10, -11, 10, 1, 9, 0, 11, 7, -12, -3, 10, -4, -10, -7, 13, -7, -9, 8, -5, -27, -15, 12, 23, 6, -30, -17, 12, -19, -18, -24, 7, 7, 1, -8, -7, -3, -20, 8, 2, 12, -19, -6, 6, -9, -22, -22, -6, -13, -2, -15, -14, -6, 3, -5, 18, 10, -19, -20, -3, -9, -31, -25, -8, -19, 5, 2, -13, 6, 3, -18, -6, 17, -1, -12, 5, -3, -32, 0, -10, -7, 7, -18, -28, 0, 5, -8, 12, 2, -2, -8, -14, -1, -26, -7, 13, 20, 33, 10, 16, 4, 1, -4, 14, -5, 17, 4, 2, -4, -1, 2, 30, -12, -6, 17, -25, -5, -1, 16, -12}
}
;

#undef INPUT_SAMPLES
#undef FC_UNITS
/**
  ******************************************************************************
  * @file    fc.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 january 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _DENSE_3_H_
#define _DENSE_3_H_

#ifndef SINGLE_FILE
#include "number.h"
#include <stdint.h>
#endif

#define INPUT_SAMPLES 128
#define FC_UNITS 10

typedef int16_t dense_3_output_type[FC_UNITS];

#if 0
void dense_3(
  const number_t input[INPUT_SAMPLES], 			      // IN
	const number_t kernel[FC_UNITS][INPUT_SAMPLES],  // IN

	const number_t bias[FC_UNITS],			              // IN

	number_t output[FC_UNITS]); 			                // OUT
#endif

#undef INPUT_SAMPLES
#undef FC_UNITS

#endif//_DENSE_3_H_
/**
  ******************************************************************************
  * @file    fc.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 1.0.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "dense_3.h"
#include "number.h"
#endif

#ifdef WITH_CMSIS_NN
#include "arm_nnfunctions.h"
#elif defined(WITH_NMSIS_NN)
#include "riscv_nnfunctions.h"
#endif

#define INPUT_SAMPLES 128
#define FC_UNITS 10
#define ACTIVATION_RELU

// For fixed point quantization
#define WEIGHTS_SCALE_FACTOR 7
#define BIASES_SCALE_FACTOR 7
#define TMP_SCALE_FACTOR 7
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void dense_3(
  const NUMBER_T input[INPUT_SAMPLES], 			      // IN
	const NUMBER_T kernel[FC_UNITS][INPUT_SAMPLES],  // IN

	const NUMBER_T bias[FC_UNITS],			              // IN

	NUMBER_T output[FC_UNITS]) {			                // OUT

#if !defined(WITH_CMSIS_NN) && !defined(WITH_NMSIS_NN)
  unsigned short k, z; 
  LONG_NUMBER_T output_acc;

  for (k = 0; k < FC_UNITS; k++) { 
    output_acc = 0;
    for (z = 0; z < INPUT_SAMPLES; z++) 
      output_acc = output_acc + ((LONG_NUMBER_T)kernel[k][z] * (LONG_NUMBER_T)input[z]);

    output_acc = scale(NUMBER_T, output_acc, WEIGHTS_SCALE_FACTOR - TMP_SCALE_FACTOR, OUTPUT_ROUND_MODE);

    output_acc += scale(NUMBER_T, (LONG_NUMBER_T)bias[k], BIASES_SCALE_FACTOR - TMP_SCALE_FACTOR - INPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);


    // Activation function
#ifdef ACTIVATION_LINEAR
    // Linear (MEANS NONE)
    output[k] = scale_and_clamp_to(NUMBER_T, output_acc, INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
#elif defined(ACTIVATION_RELU) || defined(ACTIVATION_RELU6)
    // ReLU
    if (output_acc < 0) {
      output[k] = 0;
    } else {
#if defined(ACTIVATION_RELU6)
      if (output_acc > scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE)) {
        output_acc = scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE);
      }
#endif
      output[k] = scale_and_clamp_to(NUMBER_T, output_acc, INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
    }
#else
#error "Unsupported activation function"
#endif
  }
#else

#if BIASES_SCALE_FACTOR > WEIGHTS_SCALE_FACTOR
#error "CMSIS-NN does not support BIASES_SCALE_FACTOR larger than WEIGHTS_SCALE_FACTOR"
#endif

  static q15_t bufferA[INPUT_SAMPLES];
#ifdef WITH_CMSIS_NN
  arm_fully_connected_q15(
#elif defined(WITH_NMSIS_NN)
  riscv_fully_connected_q15(
#endif
                             (q15_t*)input,
                             (q15_t*)kernel,
                             INPUT_SAMPLES,
                             FC_UNITS,
                             INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - BIASES_SCALE_FACTOR,
                             INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - OUTPUT_SCALE_FACTOR,
                             (q15_t*)bias,
                             (q15_t*)output,
                             (q15_t*)bufferA);
#ifdef ACTIVATION_RELU
#ifdef WITH_CMSIS_NN
  arm_relu_q15((q15_t*)output, FC_UNITS);
#elif defined(WITH_NMSIS_NN)
  riscv_relu_q15((q15_t*)output, FC_UNITS);
#endif
#elif !defined(ACTIVATION_LINEAR)
#error "Unsupported activation with CMSIS-NN"
#endif


#endif
}

#undef INPUT_SAMPLES
#undef FC_UNITS
#undef ACTIVATION_RELU
#undef WEIGHTS_SCALE_FACTOR
#undef BIASES_SCALE_FACTOR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
#undef NUMBER_T
#undef LONG_NUMBER_T
/**
  ******************************************************************************
  * @file    weights/fc.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 1.0.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#include <stdint.h>

#define INPUT_SAMPLES 128
#define FC_UNITS 10


const int16_t dense_3_bias[FC_UNITS] = {10, 23, 4, 7, 9, 10, -10, 13, 6, 8}
;

const int16_t dense_3_kernel[FC_UNITS][INPUT_SAMPLES] = {{-38, -6, -24, 15, 0, 25, 17, 12, -54, 33, 25, 29, 13, -4, -41, 16, -23, 22, 11, -26, 10, -12, -3, 27, 19, -29, 1, 32, 13, -11, -3, -23, 19, -29, 28, -21, 9, -7, -14, 25, 32, -47, -11, 30, -7, 12, -17, 15, -9, -41, 5, 16, -27, 0, 40, 39, 51, 11, 25, -18, -22, 14, -17, 28, 35, -14, -16, 17, -31, 18, -26, -2, -37, 43, 20, 21, -3, 3, 20, 8, 9, 16, -23, 1, -38, -74, -36, -32, -6, -30, 4, -17, 9, -74, 22, -25, 4, -61, 11, -25, -143, 6, -31, 3, 9, 21, -11, -14, -3, -43, 15, -4, -19, -11, 10, -1, -25, 24, 19, 11, -11, -11, 9, 5, -24, -4, 17, -19}
, {-21, 12, 20, -21, 18, -15, -41, 16, 44, -71, 21, 26, -31, -3, -11, 9, 8, 17, 16, 54, 24, 30, 26, -21, -5, 24, 20, -4, -63, 31, -7, -23, -27, -40, -17, 9, 15, -21, 25, -3, -55, 23, -25, -54, -49, -14, -26, -10, 37, -55, -63, 7, -44, 2, -34, -45, 30, 21, -22, 17, 37, 39, -29, -28, -8, 30, -35, 34, -22, 1, 28, 30, -4, -9, -22, 5, -23, 17, 8, 29, 16, 20, 15, -61, 17, 41, -8, -39, -58, 32, 9, 9, -37, 57, 5, -23, 6, 8, 0, 4, -13, 32, -89, -41, -16, 4, -20, 0, -12, 27, 4, 0, 5, 60, -28, 14, 27, 18, 2, -44, 14, 43, 15, 20, -22, 7, 18, -15}
, {31, -15, 31, 25, -55, 27, -21, -52, -83, 6, 7, -12, 18, 3, -43, -11, 4, -58, 38, -53, 26, 29, 9, -44, -24, 8, 9, -38, -36, -18, -47, -8, 12, 34, 47, 10, 22, 2, 24, 30, 12, 24, 19, -18, -8, 10, 24, -4, 31, -31, 17, 16, -6, 15, -5, 26, -53, -10, 13, 0, -70, -37, 9, -38, 25, 29, 9, -29, -51, -7, -37, 14, 21, -14, -24, 20, 14, -12, -40, 7, 27, 28, -9, -24, -23, -51, -5, 20, -10, -23, 16, -39, -24, -5, -19, 14, 3, -17, 13, 17, 22, -40, -50, 4, -8, -24, 35, 8, -9, -54, 10, -49, -10, 36, -21, 2, -22, -13, -53, 24, 32, -60, -19, -43, -21, 27, -14, -43}
, {18, 6, -42, 18, 6, 22, 2, -1, -49, -22, -10, 15, -15, -57, 43, -26, 35, -2, -29, -17, 17, 13, 37, -13, 9, 33, 23, -50, 3, -48, -18, 13, -22, -16, -15, 7, 35, -6, -40, -31, 3, 26, 27, -51, 7, -27, -3, -10, -26, 65, 4, 27, -41, 15, -12, 32, 5, -22, 29, 0, -25, -55, -64, -39, -43, 20, 14, -12, 49, 16, 30, -18, 35, -41, 20, 0, 4, -23, -27, -22, -10, 40, -28, 20, -11, 15, 10, -28, 10, 18, -20, -43, -18, -58, 12, -21, -40, -52, -16, -51, 23, -2, 26, 9, -29, -4, -59, 9, 19, 2, -4, 48, -14, -47, -9, -42, 7, -3, 42, 39, 35, -86, -10, -35, 19, 20, -34, -41}
, {-50, 14, -12, 3, -13, 21, -32, 22, -58, -57, 3, 30, -44, 17, 26, -7, -22, 28, 28, -19, -38, -24, 14, -4, -12, 5, -19, 4, -8, -18, -16, -12, 7, -36, -34, 37, 20, 33, -33, 44, -26, -54, 32, 46, 0, 6, 7, -2, 13, -6, 13, -32, 15, 3, 37, 13, -6, -47, -30, -2, 22, -40, 34, -10, -34, -22, 14, -1, -50, -12, 24, 7, -46, -24, -25, 5, 18, 14, -29, -50, -10, 17, -22, 24, 29, -2, 43, -7, -19, 51, -4, -16, -26, -61, 0, 48, 11, -13, 20, -46, -49, 0, 8, 4, 3, 12, -33, -19, -24, 13, -14, 24, -2, 41, 33, 57, 28, 6, -21, 43, -51, 14, -35, 30, -26, -41, -35, -27}
, {35, 21, -6, 25, 26, -28, 15, -35, 39, -13, 9, -35, 13, -39, 24, -13, -16, 42, 30, -60, 45, -35, 22, -21, -19, 20, 3, -46, 18, 21, 35, 0, 7, -35, -56, 32, -21, 29, -41, -39, 10, -5, 20, 13, -5, -37, -42, -8, 30, -28, -34, -51, -39, -28, -48, -37, -18, -12, 23, -9, -6, -9, -5, 38, 61, -69, 3, -35, -41, -44, -58, -50, -14, 27, -15, 28, 26, 14, -14, -27, -18, -45, -4, -29, 16, -28, -38, -26, 14, 34, 24, 45, 35, 73, 18, -9, -73, 28, 48, -17, 50, 24, 14, 27, -33, -36, 39, 29, 7, -21, -25, -74, 4, 2, -5, -44, 10, 4, 6, 27, -9, -21, -21, -9, 10, -4, -21, -27}
, {-1, -41, 42, -42, 28, 32, 23, 39, 12, -31, -29, -4, 36, 42, -50, 22, -8, -2, 40, 44, -63, -29, -2, -49, -23, 21, -30, 1, 1, 2, -3, -51, -14, 12, -20, 14, -28, -22, 14, -12, -9, -21, -8, -19, 8, -2, 3, -1, -22, -21, 21, -33, -20, -23, -12, 31, -18, -31, -22, -27, 18, -13, -15, -6, 28, -37, -45, 29, -39, -43, -15, 29, 15, -42, 1, -69, -17, -26, -12, -37, -34, -33, -26, 51, 40, 8, -43, 14, 18, -10, 18, -12, 21, -20, -25, -25, 8, -9, 3, 16, 36, -38, -45, 43, -33, 30, -39, 29, 5, 20, 26, -37, -27, 17, 26, -5, -9, -20, -7, 22, -31, -37, -13, 21, 13, -31, -2, 55}
, {-6, 11, -36, -41, -39, 25, -11, 11, 47, 1, 21, -23, -54, 26, -34, -2, -30, -74, -19, -54, -4, -17, 19, 25, -3, 31, 10, 15, -21, 14, -20, 20, -12, 33, 10, -95, -31, 10, -13, -39, 19, -39, 1, -76, 10, -26, -41, 9, 30, 24, -36, -5, -38, 21, -83, -52, -32, 4, -69, -30, -3, 17, -4, -70, -19, -12, 11, -39, -47, -2, -35, 35, 32, 23, -3, 19, -4, 27, 7, -46, -12, 32, -18, -26, 37, 19, 33, 22, -11, -37, -33, 58, -13, -54, -5, 11, 6, -7, -25, -4, 15, -47, 54, -9, -41, 28, -39, 38, 6, -2, -13, 2, -16, 4, -46, 2, 22, -10, -10, 17, 12, -14, 29, 23, 26, 34, -20, -21}
, {31, 21, -25, 15, -15, 17, 23, -20, 68, -7, -43, 17, -17, 0, -7, 11, -15, 12, 3, -14, -1, -18, -47, 9, 7, 21, 14, -39, 10, -14, 19, -8, 3, -39, 20, 26, -13, -33, 25, 18, -4, 28, -3, 24, -22, -1, 8, 6, -20, -1, 21, 24, 14, -13, 3, -29, -40, -61, 30, -23, 5, 11, -18, 5, -38, 4, 9, -31, 37, -9, 28, -17, 17, -43, -7, 10, 20, -11, -24, -39, -14, -40, 5, 12, 31, 30, -34, 14, 14, 37, -21, -34, 23, 0, 14, -34, -17, -37, -27, 9, 20, -14, 27, -1, 19, -28, 29, -24, 20, -13, 17, 42, -21, 25, -48, -39, -19, 24, 37, -46, -5, -43, -18, 31, -26, 18, -12, 6}
, {19, -6, -5, 12, -27, 2, 18, 0, -61, -12, 23, -19, -25, -18, 19, -28, -41, -39, -16, -28, -7, -15, 27, 27, 8, 1, -31, -25, 24, -29, -32, -2, -13, -32, 3, 12, -13, -36, -23, -42, -17, 31, -10, 42, 11, -34, 23, -19, -7, -48, 0, -11, -9, -6, 6, 12, -5, 13, 31, -29, 26, 2, 34, 36, 4, -32, -19, -38, -5, 8, 30, 17, -14, -47, 9, 16, -22, -14, 29, 14, -16, -10, 11, 7, -47, -11, -25, 23, -18, -19, 34, -8, 11, -45, -29, -1, -3, -28, -23, 30, -51, -6, 20, -46, 27, 11, -62, -19, -16, 58, -11, -20, -9, -61, -40, -28, 27, 5, 22, -34, 11, -7, 34, 23, 13, 14, -22, 8}
}
;

#undef INPUT_SAMPLES
#undef FC_UNITS
/**
  ******************************************************************************
  * @file    model.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 1.0.0
  * @date    08 july 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */


#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MODEL_H__
#define __MODEL_H__

#ifndef SINGLE_FILE
#include "number.h"

 // InputLayer is excluded
#include "conv2d_2.h" // InputLayer is excluded
#include "max_pooling2d_2.h" // InputLayer is excluded
#include "conv2d_3.h" // InputLayer is excluded
#include "max_pooling2d_3.h" // InputLayer is excluded
#include "flatten_1.h" // InputLayer is excluded
#include "dense_2.h" // InputLayer is excluded
#include "dense_3.h"
#endif


#define MODEL_INPUT_DIM_0 28
#define MODEL_INPUT_DIM_1 28
#define MODEL_INPUT_DIM_2 1
#define MODEL_INPUT_DIMS 28 * 28 * 1

#define MODEL_OUTPUT_SAMPLES 10

#define MODEL_INPUT_SCALE_FACTOR 7 // scale factor of InputLayer
#define MODEL_INPUT_ROUND_MODE ROUND_MODE_FLOOR
#define MODEL_INPUT_NUMBER_T int16_t
#define MODEL_INPUT_LONG_NUMBER_T int32_t

// node 0 is InputLayer so use its output shape as input shape of the model
// typedef  input_t[28][28][1];
typedef int16_t input_t[28][28][1];
typedef dense_3_output_type output_t;


void cnn(
  const input_t input,
  output_t output);

void reset(void);

#endif//__MODEL_H__


#ifdef __cplusplus
} // extern "C"
#endif
/**
  ******************************************************************************
  * @file    model.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 1.0.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SINGLE_FILE
#include "number.h"
#include "model.h"
// #include <chrono>

 // InputLayer is excluded
#include "conv2d_2.c"
#include "weights/conv2d_2.c" // InputLayer is excluded
#include "max_pooling2d_2.c" // InputLayer is excluded
#include "conv2d_3.c"
#include "weights/conv2d_3.c" // InputLayer is excluded
#include "max_pooling2d_3.c" // InputLayer is excluded
#include "flatten_1.c" // InputLayer is excluded
#include "dense_2.c"
#include "weights/dense_2.c" // InputLayer is excluded
#include "dense_3.c"
#include "weights/dense_3.c"
#endif


void cnn(
  const input_t input,
  dense_3_output_type dense_3_output) {
  
  // Output array allocation
  static union {
    conv2d_2_output_type conv2d_2_output;
    conv2d_3_output_type conv2d_3_output;
    dense_2_output_type dense_2_output;
  } activations1;

  static union {
    max_pooling2d_2_output_type max_pooling2d_2_output;
    max_pooling2d_3_output_type max_pooling2d_3_output;
    flatten_1_output_type flatten_1_output;
  } activations2;


// Model layers call chain 
  
  
  conv2d_2( // First layer uses input passed as model parameter
    input,
    conv2d_2_kernel,
    conv2d_2_bias,
    activations1.conv2d_2_output
    );
  
  
  max_pooling2d_2(
    activations1.conv2d_2_output,
    activations2.max_pooling2d_2_output
    );
  
  
  conv2d_3(
    activations2.max_pooling2d_2_output,
    conv2d_3_kernel,
    conv2d_3_bias,
    activations1.conv2d_3_output
    );
  
  
  max_pooling2d_3(
    activations1.conv2d_3_output,
    activations2.max_pooling2d_3_output
    );
  
  
  flatten_1(
    activations2.max_pooling2d_3_output,
    activations2.flatten_1_output
    );
  
  
  dense_2(
    activations2.flatten_1_output,
    dense_2_kernel,
    dense_2_bias,
    activations1.dense_2_output
    );
  
  
  dense_3(
    activations1.dense_2_output,
    dense_3_kernel,
    dense_3_bias,// Last layer uses output passed as model parameter
    dense_3_output
    );
}

#ifdef __cplusplus
} // extern "C"
#endif

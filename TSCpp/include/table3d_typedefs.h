/** 
 * @addtogroup table_3d 
 *  @{
 */


/** \file
 * @brief Typedefs for primitive 3D table elements
 * 
 * These used are for consistency across functions that work on 3D table data. 
 * For example:<br>
 * <c>table3d_value_t foo(table3d_axis_t input);</c><br>
 * instead of:<br>
 * <c>uint8_t foo(int16_t input);</c>
 */

#pragma once

#include <stdint.h>

/** @brief Encodes the \b length of the axes */
using table3d_dim_t = uint8_t;

/** @brief The type of each table value */
using table3d_value_t = uint8_t;

/** @brief The type of each axis value */
using table3d_axis_t = int16_t;

/** @brief Core 3d table generation macro
 * 
 * We have a fixed number of table types: they are defined by this macro.
 * GENERATOR is expected to be another macros that takes at least 3 arguments:
 *    axis length, x-axis domain, y-axis domain
 */
#define TABLE3D_GENERATOR(GENERATOR, ...) \
    GENERATOR(6, Rpm, Load, ##__VA_ARGS__) \
    GENERATOR(4, Rpm, Load, ##__VA_ARGS__) \
    GENERATOR(8, Rpm, Load, ##__VA_ARGS__) \
    GENERATOR(8, Rpm, Tps, ##__VA_ARGS__) \
    GENERATOR(16, Rpm, Load, ##__VA_ARGS__)

#define TABLE3D_GENERATOR_TABLE(GENERATOR, action, pTable, ...) \
    GENERATOR(6, Rpm, Load, action, pTable, ##__VA_ARGS__) \
    GENERATOR(4, Rpm, Load, action, pTable, ##__VA_ARGS__) \
    GENERATOR(8, Rpm, Load, action, pTable, ##__VA_ARGS__) \
    GENERATOR(8, Rpm, Tps, action, pTable, ##__VA_ARGS__) \
    GENERATOR(16, Rpm, Load, action, pTable, ##__VA_ARGS__)

#define TABLE3D_GENERATOR_GET(GENERATOR, action, data, start, ...) \
    GENERATOR(6, Rpm, Load, action, data, start, ##__VA_ARGS__) \
    GENERATOR(4, Rpm, Load, action, data, start, ##__VA_ARGS__) \
    GENERATOR(8, Rpm, Load, action, data, start, ##__VA_ARGS__) \
    GENERATOR(8, Rpm, Tps, action, data, start, ##__VA_ARGS__) \
    GENERATOR(16, Rpm, Load, action, data, start, ##__VA_ARGS__)
#define TABLE3D_GENERATOR_SET(GENERATOR, action, data, start, value, ...) \
    GENERATOR(6, Rpm, Load, action, data, start, value, ##__VA_ARGS__) \
    GENERATOR(4, Rpm, Load, action, data, start, value, ##__VA_ARGS__) \
    GENERATOR(8, Rpm, Load, action, data, start, value, ##__VA_ARGS__) \
    GENERATOR(8, Rpm, Tps, action, data, start, value, ##__VA_ARGS__) \
    GENERATOR(16, Rpm, Load, action, data, start, value, ##__VA_ARGS__)


// Each 3d table is given a distinct type based on size & axis domains
// This encapsulates the generation of the type name
#define TABLE3D_TYPENAME_BASE(size, xDom, yDom) table3d ## size ## xDom ## yDom

#define CAT_HELPER(a, b) a ## b
#define CONCAT(A, B) CAT_HELPER(A, B)

/*
#define CTA_GET_ROW_ITERATOR(size, xDomain, yDomain, pTable) \
      return ((TABLE3D_TYPENAME_BASE(size, xDomain, yDomain)*)pTable)->values.begin();

#define CTA_GET_X_ITERATOR(size, xDomain, yDomain, pTable) \
      return ((TABLE3D_TYPENAME_BASE(size, xDomain, yDomain)*)pTable)->axisX.begin();

#define CTA_GET_X_RITERATOR(size, xDomain, yDomain, pTable) \
      return ((TABLE3D_TYPENAME_BASE(size, xDomain, yDomain)*)pTable)->axisX.rbegin();

#define CTA_GET_Y_ITERATOR(size, xDomain, yDomain, pTable) \
      return ((TABLE3D_TYPENAME_BASE(size, xDomain, yDomain)*)pTable)->axisY.begin();

#define CTA_GET_Y_RITERATOR(size, xDomain, yDomain, pTable) \
      return ((TABLE3D_TYPENAME_BASE(size, xDomain, yDomain)*)pTable)->axisY.rbegin();
*/
/** @} */
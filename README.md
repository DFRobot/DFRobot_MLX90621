
# DFROBOT_MLX90621 Library for Arduino
-  The MLX90621 is a fully calibrated 16x4 pixels IR array

## Table of Contents

* [Summary](#summary)
* [Methods](#methods)
* [History](#history)
* [Credits](#credits)
<snippet>
<content>

## Summary
### The library provide some application rely on MLX90621:
####   Mearsure the ambient temperature.
####   Mearsure the temperature as a 4X16 matrix, ectract the Max and Min temperature.

## Methods

```C++
/*
 * @brief Initialize MLX90621
 *
 */
void  initMLX90621(void);

/*
 * @brief Measure temperature
 *
 */
void  measureTemperature(void);

/*
 * @brief Get the temperature in matrix
 *
 * @param row      The row    in temperature matrix from 1 to 4
 *        column   The column in temperature matrix from 1 to 16
 *
 * @return
 *        The value in temperature matrix
 */
float getTemperature(uint8_t row,uint8_t column);

/*
 * @brief Get the ambient temperature
 *
 * @return
 *        The ambient temperature
 */
float getAmbientTemperature(void);

/*
 * @brief Get the Minimum temperature in matrix
 *
 * @return
 *        The minimum temperature in matrix
 */
float getMinTemperature(void);

/*
 * @brief Get the Maximum temperature in matrix
 *
 * @return
 *        The Maximum temperature in matrix
 */
float getMaxTemperature(void);

## History

- data 2018-8-10
- version V1.0

## Credits
- author [Zhangjiawei  <jiawei.zhang@dfrobot.com>]

/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: all_power_cal.c
 *
 * MATLAB Coder version            : 23.2
 * C/C++ source code generated on  : 31-Dec-2023 01:00:23
 */

/* Include Files */
#include "all_power_cal.h"

/* Function Definitions */
/*
 * ALL_POWER_CAL
 *     P = ALL_POWER_CAL(T,K1,K2,K3,W)
 *
 * Arguments    : double T
 *                double k1
 *                double k2
 *                double k3
 *                double w
 * Return Type  : double
 */
double all_power_cal(double T, double k1, double k2, double k3, double w)
{
  /*     This function was generated by the Symbolic Math Toolbox version 23.2.
   */
  /*     2023-12-31 00:57:20 */
  return ((k3 + T * w * 0.10471204188481675) + T * T * k1) + k2 * (w * w);
}

/*
 * File trailer for all_power_cal.c
 *
 * [EOF]
 */
/*
 * types.h
 *
 *  Created on: 26/04/2016
 *      Author: omn
 */

#ifndef TYPES_H_
#define TYPES_H_

#include "stdint.h"

/* We use 24 bit current samples and 16 bit voltage samples */
typedef int16_t voltage_sample_t;
typedef int32_t current_sample_t;

/* We use 32 bit registers for the voltage DC estimators
          48 bit registers for the current DC estimators */
typedef int32_t voltage_dc_estimate_t[1];
typedef int16_t current_dc_estimate_t[3];

/* We use 48 bit dot product accumulators for voltage*voltage
          64 bit dot product accumulators for voltage*current
          64 bit dot product accumulators for current*current */
typedef int16_t voltage_accum_t[3];
typedef int64_t current_accum_t[1];
typedef int64_t power_accum_t[1];

/* We store frequencies in 0.01Hz steps. A 16 bit unsigned number supports the range
   0.01Hz to 655.35Hz, which is clearly more than we will ever need for the
   fundamental. It might not encompass all the harmonic frequencies we could be
   required to display in the future. */
typedef uint16_t frequency_t;
#define FREQUENCY_FRACTIONAL_DIGITS 2


/* These are the flags defined for each phase's status variable. */
enum
{
    PHASE_STATUS_NEW_LOG = 0x0001,

    /*! This flag in a channel's status variable indicates the voltage signal is currently in
        the positive half of its cycle. */
    PHASE_STATUS_V_POS = 0x0002,

    /*! This flag in a channel's status variable indicates the current signal is currently in
        the positive half of its cycle. */
    PHASE_STATUS_I_POS = 0x0004,
    PHASE_STATUS_ENERGY_LOGABLE = 0x0008,

    /*! This flag in a channel's status variable indicates the voltage signal was in overload
        during the last logged interval. Overload is determined by an excessive number of
        samples hitting the end-stops of the ADC's range. */
    PHASE_STATUS_V_OVERRANGE = 0x0010,

    /*! This flag in a channel's status variable indicates the phase current signal was in overload
        during the last logged interval. Overload is determined by an excessive number of
        samples hitting the end-stops of the ADC's range. */
    PHASE_STATUS_I_OVERRANGE = 0x0020,

    /*! This flag in a channel's status variable indicates the phase current signal was reversed
        during the last logged interval. */
    PHASE_STATUS_I_REVERSED = 0x0040,

    /*! This flag in a channel's status variable indicates the phase current signal was in overload
        during the last logged interval. Overload is determined by an excessive number of
        samples hitting the end-stops of the ADC's range. This is only used if the meter supports
        monitoring of both the live and neutral leads for anti-tamper management. */
    PHASE_STATUS_I_NEUTRAL_OVERRANGE = 0x0080,

    /*! This flag in a channel's status variable indicates the neutral current signal was
        reversed during the last logged interval. This is only used if the meter supports
        monitoring of both the live and neutral leads for anti-tamper management. */
    PHASE_STATUS_I_NEUTRAL_REVERSED = 0x0100,

    /*! This flag in a channel's status variable indicates the neutral current is the one
        currently being used. This means it has been judged by the anti-tamper logic to be
        the measurement which can best be trusted. This is only used if the meter supports
        monitoring of both the live and neutral leads for anti-tamper management. */
    PHASE_STATUS_CURRENT_FROM_NEUTRAL = 0x0200,

    /*! This flag in a channel's status variable indicates the neutral current signal is
        currently in the positive half of its cycle. This is only used if the meter supports
        monitoring of both the live and neutral leads for anti-tamper management. */
    PHASE_STATUS_I_NEUTRAL_POS = 0x0800,

    /*! This flag in a channel's status variable indicates the power has been declared to be
        reversed, after the anti-tamper logic has processed the raw indications. Live neutral
        or both leads may be reversed when this bit is set. */
    PHASE_STATUS_REVERSED = 0x1000,

    /*! This flag in a channel's status variable indicates the power (current in limp mode)
        has been declared to be unbalanced, after the anti-tamper logic has processed the
        raw indications. */
    PHASE_STATUS_UNBALANCED = 0x2000,

    PHASE_STATUS_DC_MODE = 0x8000
};

#endif /* TYPES_H_ */

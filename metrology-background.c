/*
 * metrology-background.c
 *
 *  Created on: 26/04/2016
 *      Author: omn
 */
#include "metrology-background.h"
#include "./driverlib/driverlib.h"
#include <string.h>
#include "communication.h"
#include "types.h"
#include "toolkit/fastarithmetic.h"
//#include "toolkit/emeter-toolkit.h"

#include <stdlib.h>

#define MAINS_NOMINAL_FREQUENCY		50

#if (OSR_FILTER == 256)	//Samplerate 4Khz
#define SAMPLE_RATE                 4000		//8000//7812
#define SAMPLES_PER_10_SECONDS      (40000UL)	//(80000UL)//(78125UL)
#else	//Samplerate 8kHz
#define SAMPLE_RATE                 8000		//8000//7812
#define SAMPLES_PER_10_SECONDS      (80000UL)	//(80000UL)//(78125UL)
#endif


struct mains_period_s
{
	/*! \brief This is a count of the number of samples to date in the current mains cycle. */
	uint16_t cycle_samples;
	/*! \brief This is the current estimate of the length of a mains cycle, in units of 1/256th
               of a sample time. */
	uint32_t period;
};

struct current_sensor_dot_prod_set_s
{
	/*! The active power dot product (i.e. V.I) currently in progress. */
	power_accum_t P_active;
	/*! The reactive power dot product (i.e. V90.I). */
	power_accum_t P_reactive;
	/*! The current dot product (i.e. I.I). */
	current_accum_t I_sq;
	/*! The number of ADC samples gathered in the dot products. */
	int16_t sample_count;
};

struct phase_correction_sd_s
{
    /*! \brief The step into the history data used for phase correction. */
    int16_t step;
    int16_t sd_preloaded_offset;
};


struct phase_dot_prod_set_s
{

	/*! \brief Accumulated squares of the phase corrected voltage (V.V). These are used to
               calculate the RMS voltage, and the power factor. We need separate accumulation
               for each current sensor, as we accumulate the phase corrected voltage which matches
               the current sensor's phase shift. The actual phase correction is not important for this.
               However, if we use just one accumulated voltage we would need to use a scaling factor before
               it can be applied. This might reduce precision, so we do things this way. */
	voltage_accum_t V_sq;

	/*! \brief This is the number of samples accumulated to date in the block of processed
               signals being gathered. */
	int16_t sample_count;
};

/*! This structure contains the working parameters associated with a
    single current sensor (e.g a shunt resistor or a current transformer). */
struct current_sensor_parms_s
{
    /* Channel's DC estimates, accumulated powers, and the logged
       accumulated power from the last measurement cycle. */
    /*! \brief Current channel DC estimates. We need separate estimates for each gain
               stage, as the amps may have significant temperature related DC offsets. */
    current_dc_estimate_t I_dc_estimate;
    int32_t I_dc_estimate_logged;
    int32_t I_dc_estimate_temp;
    /*! We use 2 sets of dot products, and ping-pong between them. At any moment one is
        in the hands of the background activity, and the other is in the hands of the
        foreground activity. */
    struct current_sensor_dot_prod_set_s dot_prod[2];

    struct phase_correction_sd_s in_phase_correction;

    /*! \brief History of the current samples, for use when making large phase corrections */
    current_sample_t I_history[2];

    /*! \brief The number of times the ADC sample has hit one of the end-stops,
               during the current accumulation period. */
    int8_t I_endstops;
};

struct phase_metrology_s
{
    /*! \brief Voltage channel DC estimates. We need separate estimates for normal
               and limp mode, as in limp mode we use a different ADC reference. */
    voltage_dc_estimate_t V_dc_estimate;
    int32_t V_dc_estimate_logged;
    int32_t V_dc_estimate_temp;
    /*! \brief Voltage history, used for phase correction and/or reactive power estimation
               by the quadrature method. */
    int16_t V_history[4];

    /*! \brief This is a pointer to the current latest value in the voltage sample history
               buffer. */
    int8_t V_history_index;

    uint16_t cycle_count;
    /*! We use 2 sets of dot products, and ping-pong between them. At any moment one is
        in the hands of the background activity, and the other is in the hands of the
        foreground activity. */
    struct phase_dot_prod_set_s dot_prod[2];
    /*! This is the data for the current sensors. */
    struct current_sensor_parms_s current;

    struct mains_period_s voltage_period;

    /*! \brief The number of samples since the last one which did not appear to be a mains spike */
    int since_last;
    /*! \brief The previous voltage channel sample. */
    int16_t last_V_sample;

    /*! \brief This value counts down each time an ADC sample from the voltage
               channel hits the end of the ADC range. It is used to detect an
               overrange signal in a robust way, ignoring a small number of samples
               hits the ends of the ADC range, which might just be a few noise spikes. */
    int8_t V_endstops;
    /*! The index of the dot product set currently for foreground processing. */
    int8_t dp_set;
} metrology;


/* These are the buffers for one set of samples */
static voltage_sample_t adc_v_buffer;
static current_sample_t adc_i_buffer;
int16_t samples_per_second = SAMPLES_PER_10_SECONDS/10;

#define V_HISTORY_MASK	0x3

int datardy = 0;


//static int maxsamplecount = 0;
//static int minsamplecount = 59999;

/*! The maximum per sample change in the voltage signal, before we declare a voltage spike. */
#define MAX_PER_SAMPLE_VOLTAGE_SLEW     4000

/*! \brief Status bits */
uint16_t /*volatile*/ status;


//static int64_t test;
void metrology_start(){
	//printf("metrology_start \n");

	//Start sampling
	SD24_startConverterConversion(SD24_BASE, SD24_CONVERTER_1);
}

static int16_t evaluate_mains_frequency(void){
	int32_t x = metrology.voltage_period.period;
	x = (int32_t) SAMPLES_PER_10_SECONDS*256L*10L/(x >> 16);
	return x;
}

/* We store voltages in 1mV steps. A 32 bit unsigned number supports the range
   1mV to 2147483.647V, which seems OK for all forseeable uses. */
typedef int32_t rms_voltage_t;

static rms_voltage_t evaluate_rms_voltage(int dp){

    /* The accumulated voltage is 16bitsx16bits*(~4096). So its a 43/44 bit number.
       After dividing by (~4096) its a 31/32 bit number.
       After we take the square root of the 32 bit number its a 16.16 bit number. */
	int32_t tmp = div48(metrology.dot_prod[dp].V_sq, metrology.current.dot_prod[dp].sample_count);

    /* The ac_offset removes the effect of the AWGN from the ADC front end. AWGN is orthogonal to everything but a true copy
       of itself. This means means we need to subtract the ac_offset in a "Pythagoras" manner", while still squared. */

	rms_voltage_t x = isqrt32(tmp);
	x >>= 16;	//Remove the least significant bits. XXXX.XX mv
	x = umul16(x, 49575);
	x >>= 11;

	//Vmv = VADC * 49575/2^11
	return x;
}

/* We store currents in 1uA steps. A 32 bit unsigned number supports the range
   1uA to 2147.483647A, which seems far more than will ever be needed. */
typedef uint32_t rms_current_t;

//Current range: 0 - 22.5A => 0 - 22500000uA
//A 32bit unsigned, each LSB equas 1uA -> range 0 - 4294967296uA = 0 - 4294.967296A
static rms_current_t evaluate_rms_current(int dp){
    uint64_t tmp;

    tmp = *metrology.current.dot_prod[dp].I_sq/metrology.current.dot_prod[dp].sample_count;

    //Returns Q32.32 - we only need Q22.xx to hold support a range from ADC 0 - 2600000 eq 0 - 22.5A
    //In praksis this means that we have a maximum Q22.32 bit value
    tmp = isqrt64(tmp);

    //Lets reduce the fractions to allow for a 32bit fix point integer for further calculations
    //21bit reduction in the fractions means we have a Q22.10 value.
    tmp >>= 22;

    //ADC -> uA = (ADC * 1171875) >> 16.
    tmp = mul64u_32_32(tmp, 1171875) >> 26;

    return (rms_current_t)tmp;
}

static uint32_t evaluate_active_power(int dp){
	uint64_t tmp;
    //x[ch] = div_ac_power(phase->metrology.current[ch].dot_prod[dp].P_active, phase->metrology.current[ch].dot_prod[dp].sample_count);
	//x[ch] >>= 9;
    //x[ch] = mul48_32_16(x[ch], phase_cal->current[ch].P_scale_factor);

	//tmp max 36bit	including sign
	tmp = *metrology.current.dot_prod[dp].P_active/(metrology.current.dot_prod[dp].sample_count+10);
	tmp >>= 9;
	tmp = mul64u_32_32(tmp, 1869969) >> 23;

	return tmp;
}

void metrology_print_status(){
	uint16_t p_status;
	volatile frequency_t frequency;

	uint32_t P_active = 0;
	uint32_t p_apparent = 0;
	uint32_t V_rms = 0;
	uint32_t I_rms = 0;

	p_status = status;
	int dp;

	if (p_status & PHASE_STATUS_NEW_LOG)
    {
		status &= ~PHASE_STATUS_NEW_LOG;
    	GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

        /* The background activity has informed us that it is time to
           perform a block processing operation. */
        //calculate_phase_readings();

		dp = metrology.dp_set;

        frequency = evaluate_mains_frequency();
        V_rms = evaluate_rms_voltage(dp);
        I_rms = evaluate_rms_current(dp);
        P_active = evaluate_active_power(dp);
        p_apparent = mul64u_32_32(V_rms, I_rms) / 1000000;	//uA x mV / 1000000 = mW



//        printf("Samples= %i\n", metrology.dot_prod[dp].sample_count);
        printf("Freq = %u\n", frequency);
        printf("VRMS = %n\n", V_rms);
        printf("IRMS= %n\n", I_rms);
        printf("P_active   = %n\n", P_active);
        printf("P_apparent = %n\n", p_apparent);

        memset(&metrology.dot_prod[dp], 0, sizeof(metrology.dot_prod[0]));
        memset(&metrology.current.dot_prod[dp], 0, sizeof(metrology.current.dot_prod[0]));
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }
	else{
		tx();	//Send data through the uart
	}
}

static void __inline__ log_parameters(void){

	/* Tell the foreground there are things to process, and swap the dot product sets. */
	metrology.dp_set ^= 1;
	status |= PHASE_STATUS_NEW_LOG;
}

static __inline__ int32_t abs32(int32_t x)
{
    return (x < 0)  ?  -x  :  x;
}

voltage_sample_t v_hist[165];
voltage_sample_t* v_hist_wrt_ptr = &v_hist[0];
voltage_sample_t* v_hist_start = &v_hist[0];
voltage_sample_t* v_hist_end = &v_hist[164];

static __inline__ int per_sample_dsp(void)
{
	int kick = 0;
	voltage_sample_t V_sample = adc_v_buffer;
	voltage_sample_t V_corrected;
	current_sample_t I_sample;
	current_sample_t I_corrected = adc_i_buffer;
    int x, y, z, k;
    int dp;
	struct phase_dot_prod_set_s* phase_dot_products;
	struct current_sensor_dot_prod_set_s *sensor_dot_products;
	int8_t loggers = 0;

	//Use the set of data that the frontend is NOT using
	dp = metrology.dp_set ^ 1;
	phase_dot_products = &metrology.dot_prod[dp];
	sensor_dot_products = &metrology.current.dot_prod[dp];

	V_corrected = V_sample;
	I_sample = I_corrected;

	*v_hist_wrt_ptr++ = V_corrected;
	if(v_hist_wrt_ptr > v_hist_end) v_hist_wrt_ptr = v_hist_start;

	sqac48_16_fast(phase_dot_products->V_sq, V_sample);	//Accumulate and square product			8us
	sqac64_24_fast(sensor_dot_products->I_sq, I_sample);//Accumulate and square product			12us
	//mac64_16_24_fast(sensor_dot_products->P_active, V_sample, I_sample);//						11us
	mac64_16_24(sensor_dot_products->P_active, V_sample, I_sample);//

	phase_dot_products->sample_count++;
	sensor_dot_products->sample_count++;

	if(phase_dot_products->sample_count >= 30){
		_no_operation();
	}
	//1423377786
	/* We need to save the history of the voltage signal if we are performing phase correction, and/or
	           measuring the quadrature shifted power (to obtain an accurate measure of one form of the reactive power). */
	//metrology.V_history[(int) metrology.V_history_index] = V_sample;

	/* Perform bulk delay (i.e. integer sample times) of the voltage signal. */
    //V_corrected = metrology.V_history[(metrology.V_history_index - metrology.current.in_phase_correction.step) & V_HISTORY_MASK];

    //metrology.V_history_index = (metrology.V_history_index + 1) & V_HISTORY_MASK;
    metrology.voltage_period.cycle_samples += 256;
	if (abs32(V_corrected - metrology.last_V_sample) <= metrology.since_last*MAX_PER_SAMPLE_VOLTAGE_SLEW)
	{
		/* This doesn't look like a spike - do mains cycle detection, and
           estimate the precise mains period */
		if(V_corrected < 0){
			/* We just crossed from positive to negative */
			/* Log the sign of the signal */
			status &= ~PHASE_STATUS_V_POS;
		}
		else{
			/* We just crossed from negative to positive */
			if (!(status & PHASE_STATUS_V_POS))
			{	//2925
				/* The mains should be <40Hz or >70Hz to fail this test! */
				if (256*SAMPLES_PER_10_SECONDS/700 <= metrology.voltage_period.cycle_samples
						&&
						metrology.voltage_period.cycle_samples <= 256*SAMPLES_PER_10_SECONDS/400)
				{
					/* A mains frequency measurement procedure based on interpolating zero crossings,
                       to get a fast update rate for step changes in the mains frequency */
					/* Interpolate the zero crossing by successive approx. Its faster than just dividing. */
					z = V_corrected - metrology.last_V_sample;
					//z = V_corrected - last_V_sample;
					x = 0;
					y = 0;
					for (k = 0;  k < 8;  k++)
					{
						y <<= 1;
						z >>= 1;
						x += z;
						if (x > V_corrected)
						//if (x > V_corrected)
							x -= z;
						else
							y |= 1;
					}
					/* Now we need to allow for skipped samples, due to spike detection */
					z = y;
					while (metrology.since_last > 1)
					{
						z += y;
						metrology.since_last--;
					}
					/* z is now the fraction of a sample interval between the zero
            		   crossing and the current sample, in units of 1/256 of a sample */
				    /* A lightly damped single pole filter should now be enough to remove noise and get a
				       stable value for the frequency */
					metrology.voltage_period.period += ((int32_t) (metrology.voltage_period.cycle_samples - z) << 12) - (metrology.voltage_period.period >> 4);
					/* Start the next cycle with the residual fraction of a sample */
					metrology.voltage_period.cycle_samples = z;
				}
				else
				{
					metrology.voltage_period.cycle_samples = 0;
				}

                if (phase_dot_products->sample_count >= samples_per_second){
                	loggers |= 1;
                }
			}
            /* Log the sign of the signal */
            status |= PHASE_STATUS_V_POS;
		}
        metrology.since_last = 0;
        metrology.last_V_sample = V_corrected;
	}
	metrology.since_last++;

    if (phase_dot_products->sample_count >= samples_per_second + 2*SAMPLE_RATE/MAINS_NOMINAL_FREQUENCY)
    {
        /* We don't seem to be detecting the end of a mains cycle, so force
           the end of processing block condition. */
        loggers |= 1;
    }

    if (loggers)
    {
        /* There are one or more phases to be logged */
        log_parameters();
        kick = 1;
    }
    return kick;
}

static __inline__ int16_t ADC16_0(void)
{
	return SD24MEM0;
}

typedef union
{
    int16_t by16[2];
    int32_t by32;
} sh_lo_t;

static __inline__ int32_t ADC32_1(void)
{
    sh_lo_t val;

    SD24CCTL1 &= ~SD24LSBACC;
    val.by16[1] = (int16_t) SD24MEM1 >> 8;
    SD24CCTL1 |= SD24LSBACC;
#if (OSR_FILTER == 256)
    val.by16[0] = (int16_t) SD24MEM1;
#elif (OSR_FILTER == 128)
    val.by16[0] = (int16_t) SD24MEM1 << 3;
#else
#error UNSUPPORTED OSR VALUE
#endif
    return val.by32;
}

void __attribute__ ((interrupt(SD24_VECTOR))) SD24_ISR(void){
	switch(__even_in_range(SD24IV,SD24IV_SD24MEM3))
	{
	case SD24IV_NONE: break;
	case SD24IV_SD24OVIFG: break;
	case SD24IV_SD24MEM0: break;
	case SD24IV_SD24MEM1:

		adc_v_buffer = ADC16_0();
		adc_i_buffer = ADC32_1();

		if(per_sample_dsp()){
			//New samples ready for to be calculated
			_BIC_SR_IRQ(LPM0_bits);
		}
		break;
	case SD24IV_SD24MEM2: break;
	case SD24IV_SD24MEM3: break;
	default: break;
	}
}


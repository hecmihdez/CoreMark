/*
 * temperature.c
 *
 *  Created on: 21 may. 2025
 *      Author: hecto
 */


#include "fsl_lpadc.h"
#include "fsl_debug_console.h"
#include "temperature.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void ADC_Configuration(void);
static float DEMO_MeasureTemperature(ADC_Type *base, uint32_t commandId, uint32_t index);

/*******************************************************************************
 * Variables
 ******************************************************************************/
lpadc_conv_command_config_t g_LpadcCommandConfigStruct; /* Structure to configure conversion command. */
volatile bool g_LpadcConversionCompletedFlag = false;
float g_CurrentTemperature                   = 0.0f;
#if (defined(DEMO_LPADC_USE_HIGH_RESOLUTION) && (DEMO_LPADC_USE_HIGH_RESOLUTION))
const uint32_t g_LpadcFullRange = 65536U;
#else
const uint32_t g_LpadcFullRange = 4096U;
#endif /* DEMO_LPADC_USE_HIGH_RESOLUTION */

/*******************************************************************************
 * Code
 ******************************************************************************/
void DEMO_LPADC_IRQ_HANDLER_FUNC(void)
{
    g_CurrentTemperature           = DEMO_MeasureTemperature(DEMO_LPADC_BASE, DEMO_LPADC_USER_CMDID, 0U);
    g_LpadcConversionCompletedFlag = true;
    SDK_ISR_EXIT_BARRIER;
}



/*Static functions*/
static void ADC_Configuration(void)
{
    lpadc_config_t lpadcConfigStruct;
    lpadc_conv_trigger_config_t lpadcTriggerConfigStruct;

    /* Init ADC peripheral. */
    LPADC_GetDefaultConfig(&lpadcConfigStruct);
    lpadcConfigStruct.enableAnalogPreliminary = true;
#if defined(FSL_FEATURE_LPADC_HAS_CFG_PWRSEL) && (FSL_FEATURE_LPADC_HAS_CFG_PWRSEL == 1U)
    lpadcConfigStruct.powerLevelMode = kLPADC_PowerLevelAlt4;
#endif /* defined(FSL_FEATURE_LPADC_HAS_CFG_PWRSEL) && (FSL_FEATURE_LPADC_HAS_CFG_PWRSEL == 1U) */
#if defined(DEMO_LPADC_VREF_SOURCE)
    lpadcConfigStruct.referenceVoltageSource = DEMO_LPADC_VREF_SOURCE;
#endif /* DEMO_LPADC_VREF_SOURCE */
#if defined(FSL_FEATURE_LPADC_HAS_CTRL_CAL_AVGS) && FSL_FEATURE_LPADC_HAS_CTRL_CAL_AVGS
    lpadcConfigStruct.conversionAverageMode = kLPADC_ConversionAverage128;
#endif /* FSL_FEATURE_LPADC_HAS_CTRL_CAL_AVGS */
#if defined(FSL_FEATURE_LPADC_TEMP_SENS_BUFFER_SIZE)
#if (defined(FSL_FEATURE_LPADC_FIFO_COUNT) && (FSL_FEATURE_LPADC_FIFO_COUNT == 2))
    lpadcConfigStruct.FIFO0Watermark = FSL_FEATURE_LPADC_TEMP_SENS_BUFFER_SIZE - 1U;
    lpadcConfigStruct.FIFO1Watermark = 0U;
#else
    lpadcConfigStruct.FIFOWatermark = FSL_FEATURE_LPADC_TEMP_SENS_BUFFER_SIZE - 1U;
#endif /* (defined(FSL_FEATURE_LPADC_FIFO_COUNT) && (FSL_FEATURE_LPADC_FIFO_COUNT == 2)) */
#endif /* FSL_FEATURE_LPADC_TEMP_SENS_BUFFER_SIZE */
    LPADC_Init(DEMO_LPADC_BASE, &lpadcConfigStruct);
#if (defined(FSL_FEATURE_LPADC_FIFO_COUNT) && (FSL_FEATURE_LPADC_FIFO_COUNT == 2U))
    LPADC_DoResetFIFO0(DEMO_LPADC_BASE);
#else
    LPADC_DoResetFIFO(DEMO_LPADC_BASE);
#endif

    /* Request LPADC calibration. */
#if defined(FSL_FEATURE_LPADC_HAS_CTRL_CALOFSMODE) && FSL_FEATURE_LPADC_HAS_CTRL_CALOFSMODE
    LPADC_SetOffsetCalibrationMode(DEMO_LPADC_BASE, DEMO_LPADC_OFFSET_CALIBRATION_MODE);
#endif /* FSL_FEATURE_LPADC_HAS_CTRL_CALOFSMODE */

#if defined(FSL_FEATURE_LPADC_HAS_CTRL_CALOFS) && FSL_FEATURE_LPADC_HAS_CTRL_CALOFS
#if defined(DEMO_LPADC_DO_OFFSET_CALIBRATION) && DEMO_LPADC_DO_OFFSET_CALIBRATION
    LPADC_DoOffsetCalibration(DEMO_LPADC_BASE); /* Request offset calibration, automatic update OFSTRIM register. */
#else                                           /* Update OFSTRIM register manually. */

#if defined(FSL_FEATURE_LPADC_HAS_OFSTRIM) && FSL_FEATURE_LPADC_HAS_OFSTRIM
#if defined(FSL_FEATURE_LPADC_OFSTRIM_COUNT) && (FSL_FEATURE_LPADC_OFSTRIM_COUNT == 2U)
    LPADC_SetOffsetValue(DEMO_LPADC_BASE, DEMO_LPADC_OFFSET_VALUE_A, DEMO_LPADC_OFFSET_VALUE_B);
#elif defined(FSL_FEATURE_LPADC_OFSTRIM_COUNT) && (FSL_FEATURE_LPADC_OFSTRIM_COUNT == 1U)
    LPADC_SetOffsetValue(DEMO_LPADC_BASE, DEMO_LPADC_OFFSET_VALUE);
#endif /* FSL_FEATURE_LPADC_OFSTRIM_COUNT */

#else  /* For other OFSTRIM register type. */
    if (DEMO_LPADC_OFFSET_CALIBRATION_MODE == kLPADC_OffsetCalibration12bitMode)
    {
        LPADC_SetOffset12BitValue(DEMO_LPADC_BASE, DEMO_LPADC_OFFSET_VALUE_A, DEMO_LPADC_OFFSET_VALUE_B);
    }
    else
    {
        LPADC_SetOffset16BitValue(DEMO_LPADC_BASE, DEMO_LPADC_OFFSET_VALUE_A, DEMO_LPADC_OFFSET_VALUE_B);
    }
#endif /* FSL_FEATURE_LPADC_HAS_OFSTRIM */

#endif /* DEMO_LPADC_DO_OFFSET_CALIBRATION */
#endif /* FSL_FEATURE_LPADC_HAS_CTRL_CALOFS */

#if defined(FSL_FEATURE_LPADC_HAS_CTRL_CAL_REQ) && FSL_FEATURE_LPADC_HAS_CTRL_CAL_REQ
    /* Request auto calibration (including gain error calibration and linearity error calibration). */
    LPADC_DoAutoCalibration(DEMO_LPADC_BASE);
#endif /* FSL_FEATURE_LPADC_HAS_CTRL_CAL_REQ */

#if (defined(FSL_FEATURE_LPADC_HAS_CFG_CALOFS) && FSL_FEATURE_LPADC_HAS_CFG_CALOFS)
    /* Do auto calibration. */
    LPADC_DoAutoCalibration(DEMO_LPADC_BASE);
#endif /* FSL_FEATURE_LPADC_HAS_CFG_CALOFS */

    /* Set conversion CMD configuration. */
    LPADC_GetDefaultConvCommandConfig(&g_LpadcCommandConfigStruct);
    g_LpadcCommandConfigStruct.channelNumber       = DEMO_LPADC_TEMP_SENS_CHANNEL;
    g_LpadcCommandConfigStruct.sampleChannelMode   = DEMO_LPADC_SAMPLE_CHANNEL_MODE;
    g_LpadcCommandConfigStruct.sampleTimeMode      = kLPADC_SampleTimeADCK131;
    g_LpadcCommandConfigStruct.hardwareAverageMode = DEMO_LPADC_HARDWARE_AVERAGE;
#if defined(FSL_FEATURE_LPADC_TEMP_SENS_BUFFER_SIZE)
    g_LpadcCommandConfigStruct.loopCount = FSL_FEATURE_LPADC_TEMP_SENS_BUFFER_SIZE - 1U;
#endif /* FSL_FEATURE_LPADC_TEMP_SENS_BUFFER_SIZE */
#if defined(FSL_FEATURE_LPADC_HAS_CMDL_MODE) && FSL_FEATURE_LPADC_HAS_CMDL_MODE
    g_LpadcCommandConfigStruct.conversionResolutionMode = kLPADC_ConversionResolutionHigh;
#endif /* FSL_FEATURE_LPADC_HAS_CMDL_MODE */
    LPADC_SetConvCommandConfig(DEMO_LPADC_BASE, DEMO_LPADC_USER_CMDID, &g_LpadcCommandConfigStruct);

    /* Set trigger configuration. */
    LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfigStruct);
    lpadcTriggerConfigStruct.targetCommandId = DEMO_LPADC_USER_CMDID;
    LPADC_SetConvTriggerConfig(DEMO_LPADC_BASE, 0U, &lpadcTriggerConfigStruct); /* Configurate the trigger0. */

    /* Enable the watermark interrupt. */
#if (defined(FSL_FEATURE_LPADC_FIFO_COUNT) && (FSL_FEATURE_LPADC_FIFO_COUNT == 2U))
    LPADC_EnableInterrupts(DEMO_LPADC_BASE, kLPADC_FIFO0WatermarkInterruptEnable);
#else
    LPADC_EnableInterrupts(DEMO_LPADC_BASE, kLPADC_FIFOWatermarkInterruptEnable);
#endif /* FSL_FEATURE_LPADC_FIFO_COUNT */
    EnableIRQ(DEMO_LPADC_IRQn);

    /* Eliminate the first two inaccurate results. */
    LPADC_DoSoftwareTrigger(DEMO_LPADC_BASE, 1U); /* 1U is trigger0 mask. */
    while (false == g_LpadcConversionCompletedFlag)
    {
    }
}

/*
 * Porting Guild.
 *
 * To port LPADC Temperature measurement driver example to other platforms, please recode
 * DEMO_MeasureTemperature function based on the actual approximate transfer function of
 * the temperature sensor.
 *
 * If the actual calculation equation is similar to the following equation, please take
 * the DEMO_MeasureTemperature function in this file as a reference.
 *          Temp = A * [alpha * (Vbe8 - Vbe1) / (Vbe8 + alpha * (Vbe8 - Vbe1))] - B.
 * In the above equation:
 *          A    --  Temperature sensor slope.
 *          B    --  Temperature sensor offset.
 */
/*${function:start}*/
/*!
 * brief Measure the temperature.
 */
float DEMO_MeasureTemperature(ADC_Type *base, uint32_t commandId, uint32_t index)
{
    lpadc_conv_result_t convResultStruct;
    uint16_t Vbe1            = 0U;
    uint16_t Vbe8            = 0U;
    uint32_t convResultShift = 0U;
    float parameterSlope     = DEMO_LPADC_TEMP_PARAMETER_A;
    float parameterOffset    = DEMO_LPADC_TEMP_PARAMETER_B;
    float parameterAlpha     = DEMO_LPADC_TEMP_PARAMETER_ALPHA;
    float temperature        = -273.15f; /* Absolute zero degree as the incorrect return value. */

#if defined(FSL_FEATURE_LPADC_TEMP_SENS_BUFFER_SIZE) && (FSL_FEATURE_LPADC_TEMP_SENS_BUFFER_SIZE == 4U)
    /* For best temperature measure performance, the recommended LOOP Count should be 4, but the first two results is
     * useless. */
    /* Drop the useless result. */
    (void)LPADC_GetConvResult(base, &convResultStruct, (uint8_t)index);
    (void)LPADC_GetConvResult(base, &convResultStruct, (uint8_t)index);
#endif /* FSL_FEATURE_LPADC_TEMP_SENS_BUFFER_SIZE */

    /* Read the 2 temperature sensor result. */
    if (true == LPADC_GetConvResult(base, &convResultStruct))
    {
        Vbe1 = convResultStruct.convValue >> convResultShift;
        if (true == LPADC_GetConvResult(base, &convResultStruct))
        {
            Vbe8 = convResultStruct.convValue >> convResultShift;
            /* Final temperature = A*[alpha*(Vbe8-Vbe1)/(Vbe8 + alpha*(Vbe8-Vbe1))] - B. */
            temperature = parameterSlope * (parameterAlpha * ((float)Vbe8 - (float)Vbe1) /
                                            ((float)Vbe8 + parameterAlpha * ((float)Vbe8 - (float)Vbe1))) -
                          parameterOffset;
        }
    }

    return temperature;
}

/*Exported Functions*/

void temperature_init(void)
{
    ADC_Configuration();

//    PRINTF("ADC Full Range: %d\r\n", g_LpadcFullRange);
#if defined(FSL_FEATURE_LPADC_HAS_CMDL_CSCALE) && FSL_FEATURE_LPADC_HAS_CMDL_CSCALE
    if (kLPADC_SampleFullScale == g_LpadcCommandConfigStruct.sampleScaleMode)
    {
        PRINTF("Full channel scale (Factor of 1).\r\n");
    }
    else if (kLPADC_SamplePartScale == g_LpadcCommandConfigStruct.sampleScaleMode)
    {
        PRINTF("Divided input voltage signal. (Factor of 30/64).\r\n");
    }
#endif
}

int32_t temperature_getValue(void)
{
	int32_t scaledTemp = 0U;

    g_LpadcConversionCompletedFlag = false;
    LPADC_DoSoftwareTrigger(DEMO_LPADC_BASE, 1U); /* 1U is trigger0 mask. */
    while (false == g_LpadcConversionCompletedFlag)
    {
    }
    PRINTF("Current temperature: %6.3f\r\n", (double)g_CurrentTemperature);

    scaledTemp = (int32_t)(g_CurrentTemperature * 1000);

    return scaledTemp;
}

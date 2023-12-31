/**
 * @file voile_gpio_rp2040.c
 * @author JimmyWang
 * @brief 
 */ 

#include "voile_iopin_rp2040.h"

voile_const_ioPin_Operate_t voile_const_ioPin_Operate_rp2040 = {
    .Init = (voile_status_t (*)(const void *, voile_io_mode_t, ...))voile_ioPin_Operate_Init,
    .Write = (voile_status_t (*)(const void *, bool))voile_ioPin_Operate_Write,
    .Read = (voile_status_t (*)(const void *, bool *))voile_ioPin_Operate_Read,
    .Toggle = (voile_status_t (*)(const void *))voile_ioPin_Operate_Taggle
};

voile_const_ioPin_Get_t voile_const_ioPin_Get_rp2040 = {
    .Read = (bool (*)(const void *))voile_ioPin_Get_Read
};


uint32_t IO_RP2040_IsOpenDrainMask = 0;

voile_status_t voile_ioPin_Operate_Init(voile_const_internal_ioPin_rp2040_t *ioPin_p, voile_io_mode_t mode, bool value) {
    uint32_t mask = 1ul << ioPin_p->pin;
    if(ioPin_p->pin > 32){
        return hardwareUnsupportedError;
    }
    switch(mode){
        case IOmodeInput:
            voile_rp2040_SIO->GPIO_OE_CLR = mask;
            voile_rp2040_PADS_BANK0->GPIO[ioPin_p->pin].selectBit.PUE = 0;
            voile_rp2040_PADS_BANK0->GPIO[ioPin_p->pin].selectBit.PDE = 0;
            IO_RP2040_IsOpenDrainMask &= ~mask;
            break;
        case IOmodeInputWithPullUpResistor:
            voile_rp2040_SIO->GPIO_OE_CLR = mask;
            voile_rp2040_PADS_BANK0->GPIO[ioPin_p->pin].selectBit.PUE = 1;
            voile_rp2040_PADS_BANK0->GPIO[ioPin_p->pin].selectBit.PDE = 0;
            IO_RP2040_IsOpenDrainMask &= ~mask;
            break;
        case IOmodeInputWithPullDownResistor:
            voile_rp2040_SIO->GPIO_OE_CLR = mask;
            voile_rp2040_PADS_BANK0->GPIO[ioPin_p->pin].selectBit.PUE = 0;
            voile_rp2040_PADS_BANK0->GPIO[ioPin_p->pin].selectBit.PDE = 1;
            IO_RP2040_IsOpenDrainMask &= ~mask;
            break;
        case IOmodeQuasiBidirectional:
            voile_rp2040_SIO->GPIO_OUT_CLR = mask;
            voile_rp2040_PADS_BANK0->GPIO[ioPin_p->pin].selectBit.PUE = 1;
            voile_rp2040_PADS_BANK0->GPIO[ioPin_p->pin].selectBit.PDE = 0;
            IO_RP2040_IsOpenDrainMask |= mask;
            voile_ioPin_Operate_Write(ioPin_p, value);
            break;
        case IOmodePushPull:
            voile_rp2040_SIO->GPIO_OE_SET = mask;
            voile_rp2040_PADS_BANK0->GPIO[ioPin_p->pin].selectBit.PUE = 0;
            voile_rp2040_PADS_BANK0->GPIO[ioPin_p->pin].selectBit.PDE = 0;
            IO_RP2040_IsOpenDrainMask &= ~mask;
            voile_ioPin_Operate_Write(ioPin_p, value);
            break;
        case IOmodeOpenDrain:
            voile_rp2040_SIO->GPIO_OUT_CLR = mask;
            voile_rp2040_PADS_BANK0->GPIO[ioPin_p->pin].selectBit.PUE = 0;
            voile_rp2040_PADS_BANK0->GPIO[ioPin_p->pin].selectBit.PDE = 0;
            IO_RP2040_IsOpenDrainMask |= mask;
            voile_ioPin_Operate_Write(ioPin_p, value);
            break;
        default:
            return hardwareUnsupportedError;
    }
    voile_rp2040_PADS_BANK0->GPIO[ioPin_p->pin].selectBit.IE = 1;
    voile_rp2040_PADS_BANK0->GPIO[ioPin_p->pin].selectBit.OD = 0;
    voile_rp2040_IO_BANK0->GPIO[ioPin_p->pin].CTRL.selectBit.FUNCSEL = 5;
    return success;
}


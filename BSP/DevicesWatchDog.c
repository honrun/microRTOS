#include "gd32f30x.h"
#include "DevicesWatchDog.h"


void vWatchdogInit(void)
{
    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);

    /* wait till IRC40K is ready */
    while(SUCCESS != rcu_osci_stab_wait(RCU_IRC40K));

    /* enable write access to FWDGT_PSC and FWDGT_RLD registers.
        FWDGT counter clock: 32KHz(IRC32K) / 64 = 0.625 KHz */
    fwdgt_config(625, FWDGT_PSC_DIV256);

    fwdgt_enable();
}

void vWatchdogReload(void)
{
    /* reload the counter of FWDGT */
    fwdgt_counter_reload();
}

#include "cy_pdl.h"
#include "cybsp.h"
#include "cycfg.h"
#include "cycfg_capsense.h"

#define CAPSENSE_INTR_PRIORITY    (3u)
#define CY_ASSERT_FAILED          (0u)

static void initialize_capsense(void);
static void capsense_isr(void);

int main(void) {
	cy_rslt_t result;

	/* Initialize the device and board peripherals */
	result = cybsp_init();
	if (result != CY_RSLT_SUCCESS) {
		CY_ASSERT(0);
	}
	/* Enable global interrupts */
	__enable_irq();

	initialize_capsense();

	Cy_CapSense_ScanAllWidgets(&cy_capsense_context);
	for (int i = 0; i < 10; i++) {
		Cy_GPIO_Write(P2_5_PORT, P2_5_NUM, 1);
		Cy_SysLib_Delay(50);
		Cy_GPIO_Write(P2_5_PORT, P2_5_NUM, 0);
		Cy_SysLib_Delay(50);
	}
	for (;;) {
		if (CY_CAPSENSE_NOT_BUSY == Cy_CapSense_IsBusy(&cy_capsense_context)) {
			/* Process all widgets */
			Cy_CapSense_ProcessAllWidgets(&cy_capsense_context);

			/* Turning ON/OFF based on button status */
			if (0
					!= Cy_CapSense_IsWidgetActive(CY_CAPSENSE_BUTTON0_WDGT_ID,
							&cy_capsense_context)) {
				Cy_GPIO_Write(P2_5_PORT, P2_5_NUM, 1);
			} else {
				Cy_GPIO_Write(P2_5_PORT, P2_5_NUM, 0);
			}

			/* Start the next scan */
			Cy_CapSense_ScanAllWidgets(&cy_capsense_context);
		}

	}
}

static void initialize_capsense(void) {
	cy_capsense_status_t status = CY_CAPSENSE_STATUS_SUCCESS;
	const cy_stc_sysint_t capsense_interrupt_config = {
			.intrSrc = CYBSP_CSD_IRQ, .intrPriority = CAPSENSE_INTR_PRIORITY, };

	status = Cy_CapSense_Init(&cy_capsense_context);

	if (CY_CAPSENSE_STATUS_SUCCESS == status) {
		Cy_SysInt_Init(&capsense_interrupt_config, capsense_isr);
		NVIC_ClearPendingIRQ(capsense_interrupt_config.intrSrc);
		NVIC_EnableIRQ(capsense_interrupt_config.intrSrc);
		status = Cy_CapSense_Enable(&cy_capsense_context);
	}
}
static void capsense_isr(void) {
	Cy_CapSense_InterruptHandler(CYBSP_CSD_HW, &cy_capsense_context);
}
/* [] END OF FILE */

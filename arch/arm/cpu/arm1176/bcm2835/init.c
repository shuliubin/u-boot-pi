/*
 * TNETV107X: Architecture initialization
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <config.h>
#include <common.h>

DECLARE_GLOBAL_DATA_PTR;

#define BCM2835_UART_BASE 0x20215000
#define BCM2835_GPIO_BASE 0x20200000

#define BCM2835_ARM_MBOX0_BASE 0x2000b880
#define BCM2835_ARM_MBOX1_BASE 0x2000b8a0
#define MBOX_CHAN_POWER   0 /* for use by the power management interface */
#define MBOX_MSG(chan, data28)      (((data28) & ~0xf) | ((chan) & 0xf))

int arch_cpu_init(void)
{
	volatile int *aux_enables = (int*)(BCM2835_UART_BASE + 0x4);
	volatile int *gpio_fsel1 = (int*)(BCM2835_GPIO_BASE + 0x04);
	int v;

	// Configure GPIO pins 14 and 15 to serve as ALT5 func (TXD1 RXD1)
	v = *gpio_fsel1;
	v &= ~((7 << (4*3)));
	v |= ((2 << (4*3)));
	v &= ~((7 << (5*3)));
	v |= ((2 << (5*3)));
	*gpio_fsel1 = v;

	// enable UART
	v = *aux_enables;
	v |= 1;
	*aux_enables = v;

	/*
	 * Request power for USB
	 */
	volatile uint32_t *mbox0_read = (volatile uint32_t *)BCM2835_ARM_MBOX0_BASE;
	volatile uint32_t *mbox0_write = (volatile uint32_t *)BCM2835_ARM_MBOX1_BASE;
	volatile uint32_t *mbox0_status = (volatile uint32_t *)(BCM2835_ARM_MBOX0_BASE + 0x18);
	volatile uint32_t *mbox0_config = (volatile uint32_t *)(BCM2835_ARM_MBOX0_BASE + 0x1C);

	while (*mbox0_status & 0x80000000) {
	}

	*mbox0_write = MBOX_MSG(0, (8 << 4)); 

	/* Show only 128Mb of RAM, to not touch GPU data */
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}
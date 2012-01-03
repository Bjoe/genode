/*
 * \brief  PL180 multi-media-card interface definitions for the RealView platform
 * \author Christian Helmuth
 * \author Stefan Kalkowski
 * \date   2011-05-19
 */

/*
 * Copyright (C) 2011-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__PLATFORM__PBXA9__PL180_DEFS_H_
#define _INCLUDE__PLATFORM__PBXA9__PL180_DEFS_H_

#include <platform/vea9x4/bus.h>

enum {
	PL180_PHYS = SMB_CS7 + 0x5000, PL180_SIZE = 0x1000,

	/**
	 * Interrupt lines
	 */
	PL180_IRQ0 = 9,
	PL180_IRQ1 = 10,
};

#endif /* _INCLUDE__PLATFORM__PBXA9__PL180_DEFS_H_ */

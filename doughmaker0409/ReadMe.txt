DESCRIPTION:
This example toggles one on-board LED using a 200ms Systick interrupt base
and send UART messages to the PC.

REQUIREMENTS:
- J-Link Drivers installed on PC
- J-Link v4.59 or above
- XMC1100 Boot Kit (XMC1100 CPU Card)

SETUP:
- Connect XMC1100 CPU Card to USB

HOW TO CREATE THE PROJECT:
1) Create a new Empty Main Project
2) Initialize P0.5 connected to an on-board LED
3) Initialize P1.2 and P1.3 for UART communication
4) Initialize clock
5) Initialize USIC channel 1
6) Initialize Systick
7) Create SysTick interrupt handler to toggle LED and send UART messages

OBSERVATIONS:
1. The top on-board LED toggles
2. The following two alternating messages are sent to the PC:
		"Visit www.infineon.com/XMC"
		"Visit www.infineonforums.com"
# PSoC-FreeRTOS
Implementation of FreeRTOS as a PSoC Creator Component

This project is an implementation of the FreeRTOS (http://www.freertos.org) RTOS
as a component that can be dragged on to a PSoC Creator schematic and configured
from the configuration dialog windws. Additionally, a group of "Driver" APIs are
being developed to provide a thread-safe (task-safe) way to access the unique
programmable perepherals within any PSoC implementation.

I have also included the E2ForLife CLI shell, which was designed to work with
FreeRTOS, and replace the FreeRTOS+CLI extension to FreeRTOS. The CLI shell is
an easy to use command line interface useful as a test/debug/maintenance interface
or as the primary interface to your application.


FreeRTOS is developed, maintainted, and owned solely by Real Time Engineers ltd.
and is not associated with E2ForLife. Please see the FreeRTOS licensing information
http://www.freertos.org/a00114.html for more informations regarding using FreeRTOS
in your own applications.

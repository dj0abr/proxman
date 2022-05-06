/* kmclib Functions and Declarations */

#ifndef _KMCLIB_
#define _KMCLIB_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <inttypes.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <linux/spi/spidev.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/fb.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>

#ifndef BUILDLIBRARY
// file in /usr/include/kmclib
#include <display.h>
#include <bargraph.h>
#include <base64.h>
#include <encoder.h>
#include <gpio.h>
#include <gps.h>
#include <i2c_rpi.h>
#include <identifySerUSB.h>
#include <kmclib.h>
#include <kmfifo.h>
#include <km_helper.h>
#include <kmtimer.h>
#include <max11615.h>
#include <mcp23017.h>
#include <motor.h>
#include <serial.h>
#include <serial_helper.h>
#include <sha1.h>
#include <stepper.h>
#include <sunposition.h>
#include <tjpgd.h>
#include <touch_button.h>
#include <udp.h>
#include <websocketserver.h>
#include <ws.h>
#include <xpt2046_rpi.h>
#endif

// =========================================================================================
// touch, extern menu
// =========================================================================================

int getActMenu();   // used by the touch button logic to assign buttons to menus

#endif //_KMCLIB_

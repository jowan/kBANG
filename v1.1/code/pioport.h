#ifndef pioport_h
#define pioport_h

#include <gb\gb.h>

#define __REG    volatile UINT8 *

#define GBWR_REG        (*(__REG)(0xA000))    /* GB OUT Port register, write access  */
#define GBWR2_REG        (*(__REG)(0xA800))    /* GB IN Port register, read access  */
#define GBCMD_REG       (*(__REG)(0xB000))    /* GB Command out  */
#endif //  pioport_h 

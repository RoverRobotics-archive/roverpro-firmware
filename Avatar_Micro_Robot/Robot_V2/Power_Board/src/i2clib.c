#include "i2clib.h"
#include "stdhdr.h"

/** State of an I2C bus. */
typedef enum i2c_state_t {
    I2C_STARTING,            ///< I2C is Starting
    I2C_IDLE_ACK,            ///< I2C has issued a start condition and is idle.
    I2C_IDLE_NACK,           ///< A NACK was either sent or received, and I2C should stop
    I2C_TRANSMITTING,        ///< I2C is currently writing out data
    I2C_RESTARTING,          ///< I2C is issuing a Start for a repeated start command
    I2C_RECEIVING,           ///< I2C is receiving
    I2C_RECEIVE_BUFFER_FULL, ///< Receive Buffer Full - I2C has received a byte
    I2C_ACKING,              ///< ACK ENabled - I2C is transmitting an ack bit
    I2C_STOPPING,            ///< I2C is stopping
    I2C_BUS_COLLISION,       ///< Attempted an action while the bus was busy. We should disable and
                             ///< re-enable the I2C module and issue a start.
    I2C_DISABLED,            ///< The I2C module is not running
} i2c_state_t;

#ifdef __PIC24FJ256GB106__

/** I2C Control Register, based on the PIC24 MCU documentation */
typedef struct i2c_con_t {
    unsigned SEN : 1;   ///< Start in progress
    unsigned RSEN : 1;  ///< ReStart in progress
    unsigned PEN : 1;   ///< stoP in progress
    unsigned RCEN : 1;  ///< ReCeive in progress
    unsigned ACKEN : 1; ///< transmitting ACK or NACK
    unsigned ACKDT : 1; ///< whether the transmitting ACK is an ACK or NACK
    unsigned STREN : 1;
    unsigned GCEN : 1;
    unsigned SMEN : 1;
    unsigned DISSLW : 1;
    unsigned A10M : 1;
    unsigned IPMIEN : 1;
    unsigned SCLREL : 1;
    unsigned I2CSIDL : 1;
    unsigned : 1;
    unsigned I2CEN : 1; ///> whether the I2C module is enabled
} i2c_con_t;

/** I2C status register, based on the PIC24 MCU documentation */
typedef struct i2c_stat_t {
    unsigned TBF : 1; ///< Transmit Buffer Full
    unsigned RBF : 1; ///< Receive Buffer Full
    unsigned R_W : 1;
    unsigned S : 1;
    unsigned P : 1;
    unsigned D_A : 1;
    unsigned I2COV : 1;
    unsigned IWCOL : 1;
    unsigned ADD10 : 1;
    unsigned GCSTAT : 1;
    unsigned BCL : 1;
    unsigned : 3;
    unsigned TRSTAT : 1; ///< TRansmit STATus. If true, I2C module is still writing transmit data to
                         ///< buffer.
    unsigned ACKSTAT : 1; ///< ACKnowledge STATus. If NACK, a NACK has been either sent or received.
} i2c_stat_t;

/** I2C bus definition, based on the PIC24 MCU documentation
 * http://ww1.microchip.com/downloads/en/DeviceDoc/70000195f.pdf
 */
typedef struct i2c_busdef_t {
    volatile i2c_con_t *CON;    ///< Memory location of Control Register
    volatile i2c_stat_t *STAT;  ///< Memory location of Status Register
    volatile unsigned int *TRN; ///< Memory location of Transmit Data Register
    volatile unsigned int *RCV; ///< Memory location of Receive Data Register
    volatile unsigned int *BRG; ///< Memory location of Baud Rate Generator Reload Register
} i2c_busdef_t;

i2c_state_t i2c_state(i2c_bus_t bus) {
    i2c_state_t state;
    i2c_con_t con = *bus->CON;
    i2c_stat_t stat = *bus->STAT;

    if (con.SEN)
        state = I2C_STARTING;
    else if (con.RSEN)
        state = I2C_RESTARTING;
    else if (con.PEN)
        state = I2C_STOPPING;
    else if (con.RCEN)
        state = I2C_RECEIVING;
    else if (con.ACKEN)
        state = I2C_ACKING;
    else if (!con.I2CEN)
        state = I2C_DISABLED;

    else if (stat.BCL)
        state = I2C_BUS_COLLISION;
    else if (stat.TRSTAT)
        state = I2C_TRANSMITTING;
    else if (stat.RBF)
        state = I2C_RECEIVE_BUFFER_FULL;
    else if (stat.ACKSTAT == NACK)
        state = I2C_IDLE_NACK;
    else
        state = I2C_IDLE_ACK;

    return state;
}

i2c_result_t i2c_enable(i2c_bus_t bus) {
    bus->CON->I2CEN = 1;
    return I2C_OKAY;
}
i2c_result_t i2c_start(i2c_bus_t bus) {
    i2c_state_t state;
    switch (state = i2c_state(bus)) {
    default:
        return I2C_ILLEGAL;
    case I2C_STOPPING:
        return I2C_NOTYET;
    case I2C_IDLE_ACK: // TODO: do we need both of
    case I2C_IDLE_NACK:
        break;
    }
    bus->CON->SEN = 1;
    return I2C_OKAY;
}

i2c_result_t i2c_stop(i2c_bus_t bus) {
    i2c_state_t state;
    switch (state = i2c_state(bus)) {
    default:
        return I2C_ILLEGAL;
    case I2C_ACKING:
    case I2C_TRANSMITTING:
        return I2C_NOTYET;

    case I2C_IDLE_NACK:
    case I2C_IDLE_ACK:
        break;
    }
    bus->CON->PEN = 1;
    return I2C_OKAY;
}

i2c_result_t i2c_restart(i2c_bus_t bus) {
    i2c_state_t state;
    switch (state = i2c_state(bus)) {
    default:
        return I2C_ILLEGAL;
    case I2C_TRANSMITTING:
        return I2C_NOTYET;
    case I2C_IDLE_ACK:
        break;
    }
    bus->CON->RSEN = 1;
    return I2C_OKAY;
}

i2c_result_t i2c_request_byte(i2c_bus_t bus) {
    i2c_state_t state;
    switch (state = i2c_state(bus)) {
    default:
        return I2C_ILLEGAL;
    case I2C_ACKING:       // still acknowledging the previous byte
    case I2C_TRANSMITTING: // still sending data
        return I2C_NOTYET;
    case I2C_IDLE_ACK:
        break;
    }
    bus->CON->RCEN = 1;
    return I2C_OKAY;
}

i2c_result_t i2c_address(i2c_bus_t bus, unsigned char addr, i2c_readwrite_t rw) {
    i2c_state_t state;
    switch (state = i2c_state(bus)) {
    default:
        return I2C_ILLEGAL;
    case I2C_STARTING:   // Writing address initially
    case I2C_RESTARTING: // Writing address after a repeated start condition
        return I2C_NOTYET;
    case I2C_IDLE_ACK: // Idle and is not addressing a
    case I2C_IDLE_NACK:
        break;
    }
    *(bus->TRN) = (addr << 1) + rw;
    if (bus->STAT->IWCOL) {
        bus->STAT->IWCOL = 0;
        return I2C_ILLEGAL;
    }
    return I2C_OKAY;
}

i2c_result_t i2c_transmit_byte(i2c_bus_t bus, unsigned char data) {
    i2c_state_t state;
    switch (state = i2c_state(bus)) {
    default:
        return I2C_ILLEGAL;
    case I2C_TRANSMITTING:
        return I2C_NOTYET;
    case I2C_IDLE_ACK:
        break;
    }
    *(bus->TRN) = data;
    if (bus->STAT->IWCOL) {
        bus->STAT->IWCOL = 0;
        return I2C_NOTYET;
    }
    return I2C_OKAY;
}

i2c_ack_t i2c_get_ack(i2c_bus_t bus) { return bus->STAT->ACKSTAT ? NACK : ACK; }

i2c_result_t i2c_check_ack(i2c_bus_t bus, i2c_ack_t *ack) {
    i2c_state_t state;
    switch (state = i2c_state(bus)) {
    default:
        return I2C_ILLEGAL;
    case I2C_TRANSMITTING:
        return I2C_NOTYET;
    case I2C_IDLE_ACK:
        *ack = ACK;
        break;
    case I2C_IDLE_NACK:
        *ack = NACK;
        break;
    }
    return I2C_OKAY;
}

i2c_result_t i2c_receive_and_ack(i2c_bus_t bus, i2c_ack_t acknack, unsigned char *data) {
    switch (i2c_state(bus)) {
    default:
        return I2C_ILLEGAL;
    case I2C_RECEIVING:
        return I2C_NOTYET;
    case I2C_RECEIVE_BUFFER_FULL:
        break;
    }

    *data = *bus->RCV;
    bus->CON->ACKDT = acknack;
    bus->CON->ACKEN = 1;
    return I2C_OKAY;
}
i2c_result_t i2c_receive_byte(i2c_bus_t bus, unsigned char *data) {
    switch (i2c_state(bus)) {
    default:
        return I2C_ILLEGAL;
    case I2C_RECEIVING:
        return I2C_NOTYET;
    case I2C_RECEIVE_BUFFER_FULL:
        break;
    }

    if (!(bus->STAT->RBF))
        return I2C_NOTYET;

    *data = *bus->RCV;
    return I2C_OKAY;
}
i2c_result_t i2c_ack(i2c_bus_t bus, i2c_ack_t ack) {
    switch (i2c_state(bus)) {
    default:
        return I2C_ILLEGAL;
    case I2C_IDLE_ACK:
        break;
    }

    bus->CON->ACKDT = ack;
    bus->CON->ACKEN = 1;
    return I2C_OKAY;
}

#include "p24FJ256GB106.h"

static const i2c_busdef_t I2C_BUS1_DEF = {(i2c_con_t *)&I2C1CON, (i2c_stat_t *)&I2C1STAT, &I2C1TRN,
                                          &I2C1RCV, &I2C1BRG};
static const i2c_busdef_t I2C_BUS2_DEF = {(i2c_con_t *)&I2C2CON, (i2c_stat_t *)&I2C2STAT, &I2C2TRN,
                                          &I2C2RCV, &I2C2BRG};
static const i2c_busdef_t I2C_BUS3_DEF = {(i2c_con_t *)&I2C3CON, (i2c_stat_t *)&I2C3STAT, &I2C3TRN,
                                          &I2C3RCV, &I2C3BRG};

const i2c_bus_t I2C_BUS1 = &I2C_BUS1_DEF;
const i2c_bus_t I2C_BUS2 = &I2C_BUS2_DEF;
const i2c_bus_t I2C_BUS3 = &I2C_BUS3_DEF;
#endif

// Begin device-independent I2C Stuff.
int min_(int x, int y) { return x < y ? x : y; }

i2c_result_t i2c_do(i2c_bus_t bus, i2c_operationdef_t *op, i2c_progress_t *progress) {
    i2c_result_t result;
    i2c_ack_t ack;

#define STEP(cmd)                                                                                  \
    progress->resume_at = (__LINE__);                                                              \
    case (__LINE__):                                                                               \
        result = (cmd);                                                                            \
        if (result == I2C_OKAY) { /*continue on to next step*/                                     \
        } else if (result == I2C_NOTYET) {                                                         \
            return result;                                                                         \
        } else {                                                                                   \
            BREAKPOINT();                                                                          \
        }

#define STEP_ACK(cmd)                                                                              \
    progress->resume_at = (__LINE__);                                                              \
    case (__LINE__):                                                                               \
        result = (cmd);                                                                            \
        BREAKPOINT_IF(result != I2C_OKAY && result != I2C_NOTYET);                                 \
        if (i2c_get_ack(bus) == NACK) {                                                            \
            goto CASE_I2C_STOP;                                                                    \
        } else if (result == I2C_OKAY) { /*continue on to next step*/                              \
        } else if (result == I2C_NOTYET) {                                                         \
            return result;                                                                         \
        }

    switch (progress->resume_at) { // todo: we have to abort early if NACK
    default:
        BREAKPOINT();
    case 0:
        STEP(i2c_start(bus))
        if (op->size_writebuf || !op->no_command_byte) {
            STEP_ACK(i2c_address(bus, op->address, I2C_WRITE))

            if (!op->no_command_byte) {
                STEP_ACK(i2c_transmit_byte(bus, op->command_byte))
            }
            if (op->write_starts_with_len) {
                progress->nbytes_write_len = min_(op->size_writebuf, op->writebuf[0] + 1);
            } else {
                progress->nbytes_write_len = op->size_writebuf;
            }
            for (progress->nbytes_written = 0;
                 progress->nbytes_written < progress->nbytes_write_len;
                 progress->nbytes_written++) {
                STEP_ACK(i2c_transmit_byte(bus, op->writebuf[progress->nbytes_written]))
            }
        }

        if ((op->size_writebuf || !op->no_command_byte) && op->size_readbuf > 0) {
            STEP(i2c_restart(bus))
        }

        if (op->size_readbuf > 0) {
            STEP_ACK(i2c_address(bus, op->address, I2C_READ))

            progress->nbytes_read = 0;
            // read the first data byte
            STEP(i2c_request_byte(bus))
            STEP(i2c_receive_byte(bus, op->readbuf))
            // the first time through, if the first byte we read is a length
            if (op->read_starts_with_len) {
                // then set the number of bytes to read based on that length or the read buffer
                // size, whichever is smaller remember, the readbuf byte count doesn't include
                // itself, hence the +1
                progress->nbytes_read_len = min_(op->size_readbuf, op->readbuf[0] + 1);
            } else {
                progress->nbytes_read_len = op->size_readbuf;
            }

            for (progress->nbytes_read = 1; // we have already read one byte.
                 progress->nbytes_read < progress->nbytes_read_len; progress->nbytes_read++) {
                STEP(i2c_ack(bus, ACK));    // ACK since there's another byte to read
                STEP(i2c_request_byte(bus)) // let recipient know we're ready for another byte
                STEP(i2c_receive_byte(
                    bus, op->readbuf + progress->nbytes_read)) // get that byte into the read buffer
            }
            STEP(i2c_ack(bus, NACK)) // NACK after reading all data bytes
        }

    CASE_I2C_STOP:
        STEP(i2c_stop(bus))
#undef STEP
    }
}
/*
void i2c_await(i2c_bus_t bus, i2c_operation_t *op);
void i2c_await(i2c_bus_t bus, i2c_operation_t *op) {
    i2c_progress_t progress = {0};
    while (i2c_do(bus, op, &progress) == I2C_NOTYET) {
        // just block execution until this is done...
    };
}

*/
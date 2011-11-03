#ifndef _TWI_MASTER_DRIVER_H
#define _TWI_MASTER_DRIVER_H 1

#define TWI_BAUD(F_SYS, F_TWI) ((F_SYS / (2 * F_TWI)) - 5)


#define TWIM_STATUS_READY              0
#define TWIM_STATUS_BUSY               1


typedef enum TWIM_RESULT_enum {
	TWIM_RESULT_UNKNOWN          = (0x00<<0),
	TWIM_RESULT_OK               = (0x01<<0),
	TWIM_RESULT_BUFFER_OVERFLOW  = (0x02<<0),
	TWIM_RESULT_ARBITRATION_LOST = (0x03<<0),
	TWIM_RESULT_BUS_ERROR        = (0x04<<0),
	TWIM_RESULT_NACK_RECEIVED    = (0x05<<0),
	TWIM_RESULT_FAIL             = (0x06<<0),
} TWIM_RESULT_t;


#define TWIM_WRITE_BUFFER_SIZE         8
#define TWIM_READ_BUFFER_SIZE          (32 * ((16 / 8) * 3))


typedef struct TWI_Master {
	TWI_t *iface;               /* Pointer to what interface to use */
	register8_t addr;           /* Slave address */
	register8_t write_data[TWIM_WRITE_BUFFER_SIZE]; /* Data to write */
	register8_t read_data[TWIM_READ_BUFFER_SIZE];   /* Read data */
	register8_t bytes_to_write;   /* Number of bytes to write */
	register8_t bytes_to_read;    /* Number of bytes to read */
	register8_t bytes_written;   /* Number of bytes written */
	register8_t bytes_read;      /* Number of bytes read */
	register8_t status;         /* Status of transaction */
	register8_t result;         /* Result of transaction */
}TWI_Master_t;



void twi_master_init(TWI_Master_t *twi,
                    TWI_t *module,
                    TWI_MASTER_INTLVL_t intLevel,
                    unsigned char baudRateRegisterSetting);

TWI_MASTER_BUSSTATE_t twi_master_state(TWI_Master_t *twi);

char twi_master_ready(TWI_Master_t *twi);

char twi_master_write(TWI_Master_t *twi,
                     unsigned char address,
                     unsigned char *write_data,
                     unsigned char bytes_to_write);

char twi_master_read(TWI_Master_t *twi,
                    unsigned char address,
                    unsigned char bytes_to_read);

char twi_master_writeread(TWI_Master_t *twi,
                         unsigned char address,
                         unsigned char *write_data,
                         unsigned char bytes_to_write,
                         unsigned char bytes_to_read);


void twi_master_irq_handler(TWI_Master_t *twi);

void twi_master_arbitration_lost_bus_error_handler(TWI_Master_t *twi);
void twi_master_write_handler(TWI_Master_t *twi);
void twi_master_read_handler(TWI_Master_t *twi);
void twi_master_transaction_finished(TWI_Master_t *twi, unsigned char result);


/*! TWI master interrupt service routine.
 *
 *  Interrupt service routine for the TWI master. Copy the needed vectors
 *  into your code.
 *
    ISR(TWIC_TWIM_vect)
    {
      TWI_MasterInterruptHandler(&twiMaster);
    }

 *
 */

#endif

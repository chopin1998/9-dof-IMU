#include "common.h"
#include "twi_master_driver.h"
#include <stdbool.h>

void twi_master_init(TWI_Master_t *twi,
                     TWI_t *module,
                     TWI_MASTER_INTLVL_t intLevel,
                     unsigned char baudRateRegisterSetting)
{
	twi->iface = module;
	twi->iface->MASTER.CTRLA = intLevel |
        TWI_MASTER_RIEN_bm |
        TWI_MASTER_WIEN_bm |
        TWI_MASTER_ENABLE_bm;
	twi->iface->MASTER.BAUD = baudRateRegisterSetting;
	twi->iface->MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
}


TWI_MASTER_BUSSTATE_t twi_master_state(TWI_Master_t *twi)
{
	TWI_MASTER_BUSSTATE_t twi_status;
	twi_status = (TWI_MASTER_BUSSTATE_t) (twi->iface->MASTER.STATUS &
	                                      TWI_MASTER_BUSSTATE_gm);
	return twi_status;
}


char twi_master_ready(TWI_Master_t *twi)
{
	char twi_status = (twi->status & TWIM_STATUS_READY);
	return twi_status;
}


/*
 *  the function is TWI Master wrapper for a write-only transaction
 */
char twi_master_write(TWI_Master_t *twi,
                      unsigned char addr,
                      unsigned char *write_data,
                      unsigned char bytes_to_write)
{
	char twi_status = twi_master_writeread(twi, addr, write_data, bytes_to_write, 0);
	return twi_status;
}


/*
 *  the function is a TWI Maste wrapper for read-only transaction
 */
char twi_master_read(TWI_Master_t *twi,
                    unsigned char addr,
                    unsigned char bytes_to_read)
{
	char twi_status = twi_master_writeread(twi, addr, 0, 0, bytes_to_read);
	return twi_status;
}


/*
 *  The function is a TWI Master write and/or read transaction
 */
char twi_master_writeread(TWI_Master_t *twi,
                         unsigned char addr,
                         unsigned char *write_data,
                         unsigned char bytes_to_write,
                         unsigned char bytes_to_read)
{
	/*Parameter sanity check. */
	if (bytes_to_write > TWIM_WRITE_BUFFER_SIZE)
    {
		return false;
	}
	if (bytes_to_read > TWIM_READ_BUFFER_SIZE)
    {
		return false;
	}

	/*Initiate transaction if bus is ready. */
	if (twi->status == TWIM_STATUS_READY)
    {
		twi->status = TWIM_STATUS_BUSY;
		twi->result = TWIM_RESULT_UNKNOWN;

		twi->addr = addr<<1;

		/* Fill write data buffer. */
		for (unsigned char bufferIndex=0; bufferIndex < bytes_to_write; bufferIndex++)
        {
			twi->write_data[bufferIndex] = write_data[bufferIndex];
		}

		twi->bytes_to_write = bytes_to_write;
		twi->bytes_to_read = bytes_to_read;
		twi->bytes_written = 0;
		twi->bytes_read = 0;

		/* If write command, send the START condition + Addr + 'R/_W = 0' */
		if (twi->bytes_to_write > 0)
        {
			unsigned char writeAddr = twi->addr & ~0x01;
			twi->iface->MASTER.ADDR = writeAddr;
		}
		/* If read command, send the START condition + Addr + 'R/_W = 1' */
		else if (twi->bytes_to_read > 0)
        {
			unsigned char readAddr = twi->addr | 0x01;
			twi->iface->MASTER.ADDR = readAddr;
		}
		return true;
	}
    else
    {
		return false;
	}
}


/*
 * TWI master interrupt service routine
 */
void twi_master_irq_handler(TWI_Master_t *twi)
{
	unsigned char currentStatus = twi->iface->MASTER.STATUS;

	/* If arbitration lost or bus error. */
	if ((currentStatus & TWI_MASTER_ARBLOST_bm) ||
	    (currentStatus & TWI_MASTER_BUSERR_bm)) {

		twi_master_arbitration_lost_bus_error_handler(twi);
	}

	/* If master write interrupt. */
	else if (currentStatus & TWI_MASTER_WIF_bm) {
		twi_master_write_handler(twi);
	}

	/* If master read interrupt. */
	else if (currentStatus & TWI_MASTER_RIF_bm) {
		twi_master_read_handler(twi);
	}

	/* If unexpected state. */
	else {
		twi_master_transaction_finished(twi, TWIM_RESULT_FAIL);
	}
}


/*
 * TWI master arbitration lost and bus error interrupt handler
 */
void twi_master_arbitration_lost_bus_error_handler(TWI_Master_t *twi)
{
	unsigned char currentStatus = twi->iface->MASTER.STATUS;

	/* If bus error. */
	if (currentStatus & TWI_MASTER_BUSERR_bm) {
		twi->result = TWIM_RESULT_BUS_ERROR;
	}
	/* If arbitration lost. */
	else {
		twi->result = TWIM_RESULT_ARBITRATION_LOST;
	}

	/* Clear interrupt flag. */
	twi->iface->MASTER.STATUS = currentStatus | TWI_MASTER_ARBLOST_bm;

	twi->status = TWIM_STATUS_READY;
}


/*
 * TWI master write interrupt handler
 */
void twi_master_write_handler(TWI_Master_t *twi)
{
	/* Local variables used in if tests to avoid compiler warning. */
	unsigned char bytes_to_write  = twi->bytes_to_write;
	unsigned char bytes_to_read   = twi->bytes_to_read;

	/* If NOT acknowledged (NACK) by slave cancel the transaction. */
	if (twi->iface->MASTER.STATUS & TWI_MASTER_RXACK_bm) {
		twi->iface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		twi->result = TWIM_RESULT_NACK_RECEIVED;
		twi->status = TWIM_STATUS_READY;
	}

	/* If more bytes to write, send data. */
	else if (twi->bytes_written < bytes_to_write) {
		unsigned char data = twi->write_data[twi->bytes_written];
		twi->iface->MASTER.DATA = data;
		++twi->bytes_written;
	}

	/* If bytes to read, send repeated START condition + Addr +
	 * 'R/_W = 1'
	 */
	else if (twi->bytes_read < bytes_to_read) {
		unsigned char readAddr = twi->addr | 0x01;
		twi->iface->MASTER.ADDR = readAddr;
	}

	/* If transaction finished, send STOP condition and set RESULT OK. */
	else {
		twi->iface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		twi_master_transaction_finished(twi, TWIM_RESULT_OK);
	}
}


/*
 * TWI master read interrupt handler
 */
void twi_master_read_handler(TWI_Master_t *twi)
{
	/* Fetch data if bytes to be read. */
	if (twi->bytes_read < TWIM_READ_BUFFER_SIZE) {
		unsigned char data = twi->iface->MASTER.DATA;
		twi->read_data[twi->bytes_read] = data;
		twi->bytes_read++;
	}

	/* If buffer overflow, issue STOP and BUFFER_OVERFLOW condition. */
	else {
		twi->iface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		twi_master_transaction_finished(twi, TWIM_RESULT_BUFFER_OVERFLOW);
	}

	/* Local variable used in if test to avoid compiler warning. */
	unsigned char bytes_to_read = twi->bytes_to_read;

	/* If more bytes to read, issue ACK and start a byte read. */
	if (twi->bytes_read < bytes_to_read) {
		twi->iface->MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
	}

	/* If transaction finished, issue NACK and STOP condition. */
	else {
		twi->iface->MASTER.CTRLC = TWI_MASTER_ACKACT_bm |
		                               TWI_MASTER_CMD_STOP_gc;
		twi_master_transaction_finished(twi, TWIM_RESULT_OK);
	}
}


/*
 * TWI transaction finished handler
 */
void twi_master_transaction_finished(TWI_Master_t *twi, unsigned char result)
{
	twi->result = result;
	twi->status = TWIM_STATUS_READY;
}

/*
 * spi.h
 *
 *  Created on: 13 Aug 2023
 *      Author: brandon
 */

#ifndef SRC_SPI_H_
#define SRC_SPI_H_

#include <stdint.h>

void spi_init(void);
void spi_trx(uint8_t const *tx_buffer, uint8_t *rx_buffer, int length);


#endif /* SRC_SPI_H_ */

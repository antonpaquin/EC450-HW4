/*
 * spi.h
 *
 *  Created on: Apr 4, 2017
 *      Author: anton
 */

#ifndef SPI_H_
#define SPI_H_

//#define SPI_SLAVE
#define SPI_MASTER

void spi_setup();

char* spi_read(char);

void spi_put(char);
void spi_writeout();
char spi_writeFinished();

void spi_sync();

void spi_onTimingInterrupt();

#endif /* SPI_H_ */

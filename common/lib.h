/* 
 * File:   lib.h
 * Author: juan_ (for now?)
 *
 * Common, general-purpose functions used in various places.
 */

#ifndef LIB_H
#define LIB_H

#include <stddef.h>

/**
 * Reads from the specified file descriptor to <i>dest</i>, ensuring to read
 * exactly <i>bytes</i> bytes.
 * 
 * @param void* dest Where to store the read bytes.
 * @param int bytes The number of bytes to read.
 * @param int fd File descriptor from which to read.
 * @return int 1 If the read was completed successfully or 0 on error.
 */
int ensureRead(void *dest, size_t bytes, int fd);

/**
 * Writes to the specified file descriptor from <i>dest</i>, ensuring to write
 * exactly <i>bytes</i> bytes.
 * 
 * @param void* src Where to read the bytes to write.
 * @param int bytes The number of bytes to write.
 * @param int fd File descriptor to write to.
 * @return int 1 If the write was completed successfully or 0 on error.
 */
int ensureWrite(const void *src, size_t bytes, int fd);

/**
 * Wrapper function for <i>select()</i> function.
 * 
 * @param maxFD The highest FD number to check.
 * @param readFDs Array of file descriptors to check for reading.
 * @param writeFDs Array of file descriptors to check for writing.
 * @param errFDs Array of file descriptors to check for errors.
 * @param timeoutSec Max seconds until timeout, or -1 if infinite.
 * @param timeoutUSec Max microseconds until timeout, or -1 if infinite.
 * @return int Whatever select() returned.
 */
int select_wrapper(int maxFD, const int readFDs[], const int writeFDs[], const int errFDs[], int timeoutSec, int timeoutUSec);

/**
 * Counts the number of digits in the specified number.
 * 
 * @param number The number to count digits for.
 * @return int The number of digits in <i>number</i>.
 */
int countDigits(int number);

/**
 * Inputs an int from stdin, showing the specified message until a valid number
 * is read.
 * 
 * @return int The read int.
 * @see Librería de la cátedra de Programación Imperativa.
 */
int scanInt(const char* msg);

#endif /* LIB_H */
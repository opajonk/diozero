#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include "com_diozero_internal_provider_sysfs_NativeSpiDevice.h"

#if __WORDSIZE == 32
#define long_t uint32_t
#else
#define long_t uint64_t
#endif

#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

/*
 * Class:     com_diozero_internal_provider_sysfs_NativeSpiDevice
 * Method:    spiOpen
 * Signature: (Ljava/lang/String;BIBZ)I
 */
JNIEXPORT jint JNICALL Java_com_diozero_internal_provider_sysfs_NativeSpiDevice_spiOpen(
		JNIEnv* env, jclass clazz, jstring path, jbyte mode, jint speed, jbyte bitsPerWord, jboolean lsbFirst) {
	int len = (*env)->GetStringLength(env, path);
	char filename[len];
	(*env)->GetStringUTFRegion(env, path, 0, len, filename);

	int fd;
	if ((fd = open(filename, O_RDWR)) < 0) {
		printf("open failed: %s", strerror(errno));
		return -1;
	}

	if (Java_com_diozero_internal_provider_sysfs_NativeSpiDevice_spiConfig(env, clazz, fd, mode, speed, bitsPerWord, lsbFirst == JNI_TRUE ? 1 : 0) < 0) {
		printf("open failed: %s", strerror(errno));
		return -1;
	}

	return fd;
}

/*
 * Class:     com_diozero_internal_provider_sysfs_NativeSpiDevice
 * Method:    spiConfig
 * Signature: (IBIBZ)I
 */
JNIEXPORT jint JNICALL Java_com_diozero_internal_provider_sysfs_NativeSpiDevice_spiConfig(
		JNIEnv* env, jclass clazz, jint fileDescriptor, jbyte mode, jint speed, jbyte bitsPerWord, jboolean lsbFirst) {
	if (ioctl(fileDescriptor, SPI_IOC_WR_MODE, &mode) < 0) {
		printf("Cannot set SPI mode: %s", strerror(errno));
		return -1 ;
	}
	uint8_t actual_mode;
	if (ioctl(fileDescriptor, SPI_IOC_RD_MODE, &actual_mode) < 0) {
		printf("Cannot get SPI mode: %s", strerror(errno));
		return -1 ;
	}
	if (actual_mode != mode) {
		printf("Warning SPI mode (%d) does not equal that set (%d)\n", actual_mode, mode);
	}

	if (ioctl(fileDescriptor, SPI_IOC_WR_BITS_PER_WORD, &bitsPerWord) < 0) {
		printf("Cannot set SPI bits per word: %s", strerror(errno));
		return -1;
	}
	uint8_t actual_bits_per_word;
	if (ioctl(fileDescriptor, SPI_IOC_RD_BITS_PER_WORD, &actual_bits_per_word) < 0) {
		printf("Cannot get SPI bits per word: %s", strerror(errno));
		return -1;
	}
	if (actual_bits_per_word != bitsPerWord) {
		printf("Warning SPI bits per word (%d) does not equal that set (%d)\n", actual_bits_per_word, bitsPerWord);
	}

	if (ioctl(fileDescriptor, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
		printf("Cannot set SPI speed: %s", strerror(errno));
		return -1;
	}
	uint32_t actual_speed;
	if (ioctl(fileDescriptor, SPI_IOC_RD_MAX_SPEED_HZ, &actual_speed) < 0) {
		printf("Cannot get SPI speed: %s", strerror(errno));
		return -1;
	}
	if (actual_speed != speed) {
		printf("Warning SPI speed (%d) does not equal that set (%d)\n", actual_speed, speed);
	}

	uint8_t lsb = lsbFirst > 0 ? SPI_LSB_FIRST : 0;
	if (ioctl(fileDescriptor, SPI_IOC_WR_LSB_FIRST, &lsb) < 0) {
		printf("Cannot set lsb first: %s", strerror(errno));
		return -1;
	}
	uint8_t actual_lsb;
	if (ioctl(fileDescriptor, SPI_IOC_RD_LSB_FIRST, &actual_lsb) < 0) {
		printf("Cannot get lsb first: %s", strerror(errno));
		return -1;
	}
	if (actual_lsb != lsb) {
		printf("Warning SPI LSB_FIRST (%d) does not equal that set (%d)\n", actual_lsb, lsb);
	}

	return 0;
}

/*
 * Class:     com_diozero_internal_provider_sysfs_NativeSpiDevice
 * Method:    spiClose
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_diozero_internal_provider_sysfs_NativeSpiDevice_spiClose(
		JNIEnv* env, jclass clazz, jint fileDescriptor) {
	return close(fileDescriptor);
}

/*
 * Class:     com_diozero_internal_provider_sysfs_NativeSpiDevice
 * Method:    spiTransfer
 * Signature: (I[B[BIIIBZ)I
 */
JNIEXPORT jint JNICALL Java_com_diozero_internal_provider_sysfs_NativeSpiDevice_spiTransfer(
		JNIEnv* env, jclass clazz, jint fileDescriptor, jbyteArray txBuffer,
		jbyteArray rxBuffer, jint length, jint speedHz, jint delayUSecs, jbyte bitsPerWord, jboolean csChange) {
	jboolean is_copy;
	jbyte* tx_buf = NULL;
	if (txBuffer != NULL) {
		tx_buf = (*env)->GetByteArrayElements(env, txBuffer, &is_copy);
	}
	jbyte* rx_buf = NULL;
	if (rxBuffer != NULL) {
		rx_buf = (*env)->GetByteArrayElements(env, rxBuffer, &is_copy);
	}

	struct spi_ioc_transfer tr = {
		.tx_buf = (long_t) tx_buf
		, .rx_buf = (long_t) rx_buf

		, .len = (uint32_t) length
		, .speed_hz = (uint32_t) speedHz

		, .delay_usecs = (uint16_t) delayUSecs
		, .bits_per_word = (uint8_t) bitsPerWord
		, .cs_change = csChange == JNI_TRUE ? 1 : 0
		/*
		, tr.tx_nbits = (uint8_t) 0
		, tr.rx_nbits = (uint8_t) 0
		, tr.pad = (uint16_t) 0
		*/
	};

	int ret = ioctl(fileDescriptor, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 0) {
		printf("SPI message transfer failed: %s", strerror(errno));
	}

	if (tx_buf != NULL) {
		// mode = JNI_ABORT - No change hence free the buffer without copying back the possible changes
		(*env)->ReleaseByteArrayElements(env, txBuffer, tx_buf, JNI_ABORT);
	}
	if (rx_buf != NULL) {
		// Mode 		Actions
		// 0 			Copy back the content and free the elems buffer
		// JNI_COMMIT 	Copy back the content but do not free the elems buffer
		// JNI_ABORT 	Free the buffer without copying back the possible changes

		// mode = 0 - Copy back the content and free the buffer (rx)
		(*env)->ReleaseByteArrayElements(env, rxBuffer, rx_buf, 0);
	}

	return ret;
}

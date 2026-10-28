#ifndef _WTPROT_ENDIAN_H
#define _WTPROT_ENDIAN_H

#include "wtprot_platform.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if WTPROT_PLATFORM == WTPROT_PLATFORM_WINDOWS
    #include <winsock2.h>
#elif WTPROT_PLATFORM == WTPROT_PLATFORM_UNIX
    #include <byteswap.h>
#elif WTPROT_PLATFORM == WTPROT_PLATFORM_ESPIDF
    #include <endian.h>
#endif

static inline uint16_t wtp_bswap16(uint16_t value) {
#if WTPROT_PLATFORM == WTPROT_PLATFORM_WINDOWS
    return _byteswap_ushort(value);
#elif WTPROT_PLATFORM == WTPROT_PLATFORM_UNIX
    return bswap_16(value);
#elif WTPROT_PLATFORM == WTPROT_PLATFORM_ESPIDF
    return __bswap16(value);
#else
    #error Unsupported platform!
#endif
}

static inline uint32_t wtp_bswap32(uint32_t value) {
#if WTPROT_PLATFORM == WTPROT_PLATFORM_WINDOWS
    return _byteswap_ulong(value);
#elif WTPROT_PLATFORM == WTPROT_PLATFORM_UNIX
    return bswap_32(value);
#elif WTPROT_PLATFORM == WTPROT_PLATFORM_ESPIDF
    return __bswap32(value);
#else
    #error Unsupported platform!
#endif
}

static inline uint64_t wtp_bswap64(uint64_t value) {
#if WTPROT_PLATFORM == WTPROT_PLATFORM_WINDOWS
    return _byteswap_uint64(value);
#elif WTPROT_PLATFORM == WTPROT_PLATFORM_UNIX
    return bswap_64(value);
#elif WTPROT_PLATFORM == WTPROT_PLATFORM_ESPIDF
    return __bswap64(value);
#else
    #error Unsupported platform!
#endif
}

static inline uint16_t wtp_uint16_to_native(uint16_t value) {
    return wtp_bswap16(value);
}

static inline uint16_t wtp_uint16_to_wire(uint16_t value) {
    return wtp_bswap16(value);
}

static inline int16_t wtp_int16_to_native(int16_t value) {
    uint16_t tmp;
    memcpy(&tmp, &value, sizeof(tmp));
    tmp = wtp_bswap16(tmp);
    memcpy(&value, &tmp, sizeof(value));
    return value;
}

static inline int16_t wtp_int16_to_wire(int16_t value) {
    return wtp_int16_to_native(value);
}

static inline uint32_t wtp_uint32_to_native(uint32_t value) {
    return wtp_bswap32(value);
}

static inline uint32_t wtp_uint32_to_wire(uint32_t value) {
    return wtp_bswap32(value);
}

static inline int32_t wtp_int32_to_native(int32_t value) {
    uint32_t tmp;
    memcpy(&tmp, &value, sizeof(tmp));
    tmp = wtp_bswap32(tmp);
    memcpy(&value, &tmp, sizeof(value));
    return value;
}

static inline int32_t wtp_int32_to_wire(int32_t value) {
    return wtp_int32_to_native(value);
}

static inline uint64_t wtp_uint64_to_native(uint64_t value) {
    return wtp_bswap64(value);
}

static inline uint64_t wtp_uint64_to_wire(uint64_t value) {
    return wtp_bswap64(value);
}

static inline int64_t wtp_int64_to_native(int64_t value) {
    uint64_t tmp;
    memcpy(&tmp, &value, sizeof(tmp));
    tmp = wtp_bswap64(tmp);
    memcpy(&value, &tmp, sizeof(value));
    return value;
}

static inline int64_t wtp_int64_to_wire(int64_t value) {
    return wtp_int64_to_native(value);
}

static inline float wtp_float32_to_native(float value) {
    uint32_t tmp;
    memcpy(&tmp, &value, sizeof(tmp));
    tmp = wtp_bswap32(tmp);
    memcpy(&value, &tmp, sizeof(value));
    return value;
}

static inline float wtp_float32_to_wire(float value) {
    return wtp_float32_to_native(value);
}

static inline double wtp_float64_to_native(double value) {
    uint64_t tmp;
    memcpy(&tmp, &value, sizeof(tmp));
    tmp = wtp_bswap64(tmp);
    memcpy(&value, &tmp, sizeof(value));
    return value;
}

static inline double wtp_float64_to_wire(double value) {
    return wtp_float64_to_native(value);
}

#endif // _WTPROT_ENDIAN_H
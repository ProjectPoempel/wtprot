#ifndef _WTPROT_H
#define _WTPROT_H

#define WTP_DEFAULT_ALLOCATOR // Comment-out to use custom allocator

#ifdef WTP_DEFAULT_ALLOCATOR
    #include <stdlib.h>
    #include <string.h>

    #define WTP_MALLOC malloc
    #define WTP_CALLOC calloc
    #define WTP_REALLOC realloc
    #define WTP_FREE free
    #define WTP_MEMSET memset
    #define WTP_MEMCPY memcpy
#endif

#ifdef __cplusplus
#pragma once

extern "C" {
#endif

#include <stdint.h>

#define WTP_HEADER_SIZE 4 // bytes

// Versions
#define WTP_VERSION_INVALID (uint8_t)0
#define WTP_VERSION_1 (uint8_t)1

#define WTP_VERSION_CURRENT WTP_VERSION_1

// Value Bitmasks
#define WTP_VBIT_TEMPERATURE (uint32_t)1 << 0
#define WTP_VBIT_HUMIDITY (uint32_t)1 << 1
#define WTP_VBIT_AIR_PRESSURE (uint32_t)1 << 2
#define WTP_VBIT_PM10 (uint32_t)1 << 3
#define WTP_VBIT_PM25 (uint32_t)1 << 4
#define WTP_VBIT_CO2 (uint32_t)1 << 5

// Status Bitmasks
#define WTP_SBIT_FAULT (uint64_t)1 << 0
#define WTP_SBIT_SENSOR_ERROR (uint64_t)1 << 1
#define WTP_SBIT_LOW_BATTERY (uint64_t)1 << 2
#define WTP_SBIT_LOW_STORAGE (uint64_t)1 << 3
#define WTP_SBIT_REBOOTING (uint64_t)1 << 4
#define WTP_SBIT_MAINTENANCE (uint64_t)1 << 5
#define WTP_SBIT_UNKNOWN (uint64_t)1 << 6

// Disconnect Reason Bitmasks
#define WTP_DRBIT_SERVER_SHUTDOWN (uint16_t)1 << 0
#define WTP_DRBIT_SERVER_REBOOTING (uint16_t)1 << 1
#define WTP_DRBIT_SERVER_MAINTENANCE (uint16_t)1 << 2
#define WTP_DRBIT_ALREADY_CONNECTED (uint16_t)1 << 3
#define WTP_DRBIT_BANNED (uint16_t)1 << 4
#define WTP_DRBIT_UNKNOWN (uint16_t)1 << 5

// Client Disconnect Reason Bitmasks
#define WTP_CDRBIT_CLIENT_SHUTDOWN (uint16_t)1 << 0
#define WTP_CDRBIT_CLIENT_REBOOTING (uint16_t)1 << 1
#define WTP_CDRBIT_SLEEP (uint16_t)1 << 2
#define WTP_CDRBIT_MAINTENANCE (uint16_t)1 << 3
#define WTP_CDRBIT_UNKNOWN (uint16_t)1 << 4

// Capability Types
#define WTP_CT_SENSOR 0x01
#define WTP_CT_FIRMWARE_INFO 0x02
#define WTP_CT_BATTERY_INFO 0x03

#define WTP_SUCCESS 0x00
#define WTP_ERR_UNKOWN 0x01
#define WTP_ERR_BUFFER_UNDERFLOW 0x10

typedef uint32_t wtp_err_t;
typedef uint32_t wtp_station_id_t;

// Value IDs
typedef enum {
    WTP_VID_TEMPERATURE = 0x01,
    WTP_VID_HUMIDITY = 0x02,
    WTP_VID_AIR_PRESSURE = 0x03,
    WTP_VID_PM10 = 0x04,
    WTP_VID_PM25 = 0x05,
    WTP_VID_CO2 = 0x06
} wtp_value_id_t;

// Packet IDs
typedef enum {
    // Bi-Directional
    WTP_PACKET_HELLO = 0x01,

    // Server to Client
    WTP_PACKET_PING = 0x10,
    WTP_PACKET_DATA_ACK = 0x31,
    WTP_PACKET_DATA_FAIL = 0x32,
    WTP_PACKET_DISCONNECT = 0x40,
    WTP_PACKET_CONFIG_PUSH = 0x41,
    
    // Client to Server
    WTP_PACKET_CAPABILITIES = 0x02,
    WTP_PACKET_PONG = 0x11,
    WTP_PACKET_DATA = 0x30,
    WTP_PACKET_LIVE_DATA = 0x33,
    WTP_PACKET_STATUS = 0x20,
    WTP_PACKET_BATCH_DATA = 0x34,
    WTP_PACKET_CLIENT_DISCONNECT = 0x50
} wtp_packet_id_t;

typedef struct {
    uint8_t* buf;
    uint32_t bufSize;
    uint32_t offset;
} wtp_packet_t;

wtp_packet_t* wtp_packet_create(wtp_packet_id_t id);
wtp_packet_t* wtp_packet_create_from_buffer(const uint8_t* buf, uint32_t size);
wtp_packet_t* wtp_packet_copy(wtp_packet_t* packet);
void wtp_packet_free(wtp_packet_t* packet);
void wtp_packet_expand(wtp_packet_t* packet, uint32_t amount);
void wtp_packet_finalize(wtp_packet_t* packet);

void wtp_packet_write(wtp_packet_t* packet, const uint8_t* buf, uint32_t size);
void wtp_packet_write_string(wtp_packet_t* packet, const char* str, uint16_t size);
void wtp_packet_write_stringn(wtp_packet_t* packet, const char* str); // null-terminated
void wtp_packet_write_u8(wtp_packet_t* packet, uint8_t v);
void wtp_packet_write_s8(wtp_packet_t* packet, int8_t v);
void wtp_packet_write_u16(wtp_packet_t* packet, uint16_t v);
void wtp_packet_write_s16(wtp_packet_t* packet, int16_t v);
void wtp_packet_write_u32(wtp_packet_t* packet, uint32_t v);
void wtp_packet_write_s32(wtp_packet_t* packet, int32_t v);
void wtp_packet_write_u64(wtp_packet_t* packet, uint64_t v);
void wtp_packet_write_s64(wtp_packet_t* packet, int64_t v);
void wtp_packet_write_float(wtp_packet_t* packet, float v);

uint32_t wtp_packet_read(wtp_packet_t* packet, uint8_t* buf, uint32_t size);
char* wtp_packet_read_string(wtp_packet_t* packet, uint16_t size);
char* wtp_packet_read_stringn(wtp_packet_t* packet); // null-terminated
wtp_err_t wtp_packet_read_u8(wtp_packet_t* packet, uint8_t* value);
wtp_err_t wtp_packet_read_s8(wtp_packet_t* packet, int8_t* value);
wtp_err_t wtp_packet_read_u16(wtp_packet_t* packet, uint16_t* value);
wtp_err_t wtp_packet_read_s16(wtp_packet_t* packet, int16_t* value);
wtp_err_t wtp_packet_read_u32(wtp_packet_t* packet, uint32_t* value);
wtp_err_t wtp_packet_read_s32(wtp_packet_t* packet, int32_t* value);
wtp_err_t wtp_packet_read_u64(wtp_packet_t* packet, uint64_t* value);
wtp_err_t wtp_packet_read_s64(wtp_packet_t* packet, int64_t* value);
wtp_err_t wtp_packet_read_float(wtp_packet_t* packet, float* value);

uint16_t wtp_get_payload_length_from_header(const uint8_t* buf, uint16_t size);
const char* wtp_packet_id_to_string(wtp_packet_id_t id);
void wtp_free_string(char* str);

#ifdef __cplusplus
}

#include <string>

namespace wtp {

    inline std::string packetReadString(wtp_packet_t* packet, uint16_t size) {
        char* str = wtp_packet_read_string(packet, size);
        std::string s(str, size);
        WTP_FREE(str);
        return s;
    }

    inline std::string packetReadString(wtp_packet_t* packet) {
        char* str = wtp_packet_read_stringn(packet);
        std::string s(str);
        WTP_FREE(str);
        return s;
    }

}
#endif

#endif // _WTPROT_H
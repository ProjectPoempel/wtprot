#include "wt/wtprot.h"
#include "wt/wtprot_endian.h"

wtp_packet_t* wtp_packet_create(wtp_packet_id_t id) {
    wtp_packet_t* pkt = (wtp_packet_t*)WTP_CALLOC(1, sizeof(wtp_packet_t));
    WTP_MEMSET(pkt, 0, sizeof(wtp_packet_t));

    wtp_packet_expand(pkt, 4);

    pkt->buf[0] = WTP_VERSION_CURRENT;
    pkt->buf[1] = id;
    pkt->offset = 2;
    wtp_packet_write_u16(pkt, 0);

    return pkt;
}

wtp_packet_t* wtp_packet_create_from_buffer(const uint8_t* buf, uint32_t size) {
    wtp_packet_t* pkt = (wtp_packet_t*)WTP_CALLOC(1, sizeof(wtp_packet_t));
    WTP_MEMSET(pkt, 0, sizeof(wtp_packet_t));

    pkt->buf = (uint8_t*)WTP_MALLOC(size);
    pkt->bufSize = size;

    WTP_MEMCPY(pkt->buf, buf, size);

    return pkt;
}

wtp_packet_t* wtp_packet_copy(wtp_packet_t* packet) {
    if (!packet) return NULL;

    wtp_packet_t* pkt = (wtp_packet_t*)WTP_CALLOC(1, sizeof(wtp_packet_t));
    pkt->buf = (uint8_t*)WTP_MALLOC(packet->bufSize);
    pkt->bufSize = packet->bufSize;
    pkt->offset = packet->offset;
    WTP_MEMCPY(pkt->buf, packet->buf, packet->bufSize);

    return pkt;
}

void wtp_packet_finalize(wtp_packet_t* packet) {
    if (!packet) return;

    uint16_t payloadSize = packet->offset - 4;
    packet->offset = 2;
    wtp_packet_write_u16(packet, payloadSize);
    packet->offset = 4 + payloadSize;
}

void wtp_packet_free(wtp_packet_t* packet) {
    if (!packet) return;
    WTP_FREE(packet);
}

void wtp_packet_expand(wtp_packet_t* packet, uint32_t amount) {
    if (!packet) return;
    if (amount < 1) return;

    packet->buf = (uint8_t*)WTP_REALLOC(packet->buf, packet->bufSize + amount);
    packet->bufSize += amount;
}

#define WTP_PACKET_WRITE_TEMPLATE(type, pkt, v) \
    type _v = v; \
    wtp_packet_write(pkt, (uint8_t*)&_v, sizeof(type))

void wtp_packet_write(wtp_packet_t* packet, const uint8_t* buf, uint32_t size) {
    if (!packet) return;
    if (packet->offset + size > packet->bufSize) {
        wtp_packet_expand(packet, (packet->offset + size) - packet->bufSize);
    }

    memcpy(&packet->buf[packet->offset], buf, size);
    packet->offset += size;
}

void wtp_packet_write_string(wtp_packet_t* packet, const char* str, uint16_t size) {
    wtp_packet_write(packet, (uint8_t*)str, size);
}

void wtp_packet_write_stringn(wtp_packet_t* packet, const char* str) {
    size_t len = strlen(str);
    wtp_packet_write(packet, (uint8_t*)str, len);
}

void wtp_packet_write_u8(wtp_packet_t* packet, uint8_t v) {
    WTP_PACKET_WRITE_TEMPLATE(uint8_t, packet, v);
}

void wtp_packet_write_s8(wtp_packet_t* packet, int8_t v) {
    WTP_PACKET_WRITE_TEMPLATE(int8_t, packet, v);
}

void wtp_packet_write_u16(wtp_packet_t* packet, uint16_t v) {
    WTP_PACKET_WRITE_TEMPLATE(uint16_t, packet, wtp_uint16_to_wire(v));
}

void wtp_packet_write_s16(wtp_packet_t* packet, int16_t v) {
    WTP_PACKET_WRITE_TEMPLATE(int16_t, packet, wtp_int16_to_wire(v));
}

void wtp_packet_write_u32(wtp_packet_t* packet, uint32_t v) {
    WTP_PACKET_WRITE_TEMPLATE(uint32_t, packet, wtp_uint32_to_wire(v));
}

void wtp_packet_write_s32(wtp_packet_t* packet, int32_t v) {
    WTP_PACKET_WRITE_TEMPLATE(int32_t, packet, wtp_int32_to_wire(v));
}

void wtp_packet_write_u64(wtp_packet_t* packet, uint64_t v) {
    WTP_PACKET_WRITE_TEMPLATE(uint64_t, packet, wtp_uint64_to_wire(v));
}

void wtp_packet_write_s64(wtp_packet_t* packet, int64_t v) {
    WTP_PACKET_WRITE_TEMPLATE(int64_t, packet, wtp_int64_to_wire(v));
}

void wtp_packet_write_float(wtp_packet_t* packet, float v) {
    WTP_PACKET_WRITE_TEMPLATE(float, packet, wtp_float32_to_wire(v));
}

uint32_t wtp_packet_read(wtp_packet_t* packet, uint8_t* buf, uint32_t size) {
    if (!packet) return 0;
    if (packet->offset + size > packet->bufSize) return 0;

    memcpy(buf, &packet->buf[packet->offset], size);
    packet->offset += size;

    return size;
}

char* wtp_packet_read_string(wtp_packet_t* packet, uint16_t size) {
    char* ptr = (char*)WTP_MALLOC(size + 1);
    ptr[size] = '\0';

    wtp_packet_read(packet, (uint8_t*)ptr, size);

    return ptr;
}

char* wtp_packet_read_stringn(wtp_packet_t* packet) {
    uint16_t len = 0;
    uint16_t offset = packet->offset;

    while (offset < packet->bufSize) {
        if (packet->buf[offset] == '\0') break;
        offset++;
        len++;
    }
    if (offset >= packet->bufSize) return NULL;

    return wtp_packet_read_string(packet, len);
}

#define WTP_PACKET_READ_TEMPLATE(type, pkt, v) \
    uint32_t read; \
    if ((read = wtp_packet_read(pkt, (uint8_t*)v, sizeof(type))) <= 0) \
        return WTP_ERR_BUFFER_UNDERFLOW;

wtp_err_t wtp_packet_read_u8(wtp_packet_t* packet, uint8_t* value) {
    WTP_PACKET_READ_TEMPLATE(uint8_t, packet, value);
    return WTP_SUCCESS;
}

wtp_err_t wtp_packet_read_s8(wtp_packet_t* packet, int8_t* value) {
    WTP_PACKET_READ_TEMPLATE(int8_t, packet, value);
    return WTP_SUCCESS;
}

wtp_err_t wtp_packet_read_u16(wtp_packet_t* packet, uint16_t* value) {
    WTP_PACKET_READ_TEMPLATE(uint16_t, packet, value);
    *value = wtp_uint16_to_native(*value);
    return WTP_SUCCESS;
}

wtp_err_t wtp_packet_read_s16(wtp_packet_t* packet, int16_t* value) {
    WTP_PACKET_READ_TEMPLATE(int16_t, packet, value);
    *value = wtp_int16_to_native(*value);
    return WTP_SUCCESS;
}

wtp_err_t wtp_packet_read_u32(wtp_packet_t* packet, uint32_t* value) {
    WTP_PACKET_READ_TEMPLATE(uint32_t, packet, value);
    *value = wtp_uint32_to_native(*value);
    return WTP_SUCCESS;
}

wtp_err_t wtp_packet_read_s32(wtp_packet_t* packet, int32_t* value) {
    WTP_PACKET_READ_TEMPLATE(int32_t, packet, value);
    *value = wtp_int32_to_native(*value);
    return WTP_SUCCESS;
}

wtp_err_t wtp_packet_read_u64(wtp_packet_t* packet, uint64_t* value) {
    WTP_PACKET_READ_TEMPLATE(uint64_t, packet, value);
    *value = wtp_uint64_to_native(*value);
    return WTP_SUCCESS;
}

wtp_err_t wtp_packet_read_s64(wtp_packet_t* packet, int64_t* value) {
    WTP_PACKET_READ_TEMPLATE(int64_t, packet, value);
    *value = wtp_int64_to_native(*value);
    return WTP_SUCCESS;
}

wtp_err_t wtp_packet_read_float(wtp_packet_t* packet, float* value) {
    WTP_PACKET_READ_TEMPLATE(float, packet, value);
    *value = wtp_float32_to_native(*value);
    return WTP_SUCCESS;
}

uint16_t wtp_get_payload_length_from_header(const uint8_t* buf, uint16_t size) {
    if (size < WTP_HEADER_SIZE) return 0;

    uint16_t len;
    memcpy((void*)&len, &buf[WTP_HEADER_SIZE - sizeof(len)], sizeof(len));

    return wtp_uint16_to_native(len);
}

const char* wtp_packet_id_to_string(wtp_packet_id_t id) {
    switch (id) {
        case 0x01: return "WTP_PACKET_HELLO";
        case 0x10: return "WTP_PACKET_PING";
        case 0x31: return "WTP_PACKET_DATA_ACK";
        case 0x32: return "WTP_PACKET_DATA_FAIL";
        case 0x40: return "WTP_PACKET_DISCONNECT";
        case 0x41: return "WTP_PACKET_CONFIG_PUSH";
        case 0x02: return "WTP_PACKET_CAPABILITIES";
        case 0x11: return "WTP_PACKET_PONG";
        case 0x30: return "WTP_PACKET_DATA";
        case 0x33: return "WTP_PACKET_LIVE_DATA";
        case 0x20: return "WTP_PACKET_STATUS";
        case 0x34: return "WTP_PACKET_BATCH_DATA";
        case 0x50: return "WTP_PACKET_CLIENT_DISCONNECT";
        default: return "UNKNOWN";
    }
}

void wtp_free_string(char* str) {
    WTP_FREE(str);
}

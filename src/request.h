#include <netinet/ip.h>
#include <time.h>
#include "stdlib.h"
#include "constants.h"

#ifndef FF_REQUEST_H
#define FF_REQUEST_H

#define FF_REQUEST_MAX_OPTIONS 10

enum ff_request_state
{
    FF_REQUEST_STATE_RECEIVING = 0,
    FF_REQUEST_STATE_RECEIVED = 1,
    FF_REQUEST_STATE_RECEIVING_FAIL = 2,
    FF_REQUEST_STATE_DECRYPTING = 3,
    FF_REQUEST_STATE_DECRYPTED = 4,
    FF_REQUEST_STATE_DECRYPTING_FAILED = 5,
    FF_REQUEST_STATE_SENDING = 6,
    FF_REQUEST_STATE_SENT = 7,
    FF_REQUEST_STATE_SENDING_FAILED = 8,
};

enum ff_request_version
{
    // Support for proxying a RAW HTTP request
    FF_VERSION_RAW = -1,
    // Ensure that version numbers do not overlap 'A' (65) to 'Z' (90)
    // to prevent collisions with determining if the payload contains a raw HTTP request
    // identified by the request method.
    FF_VERSION_1 = 1
};

enum ff_request_option_type
{
    FF_REQUEST_OPTION_TYPE_EOL = 0,
    FF_REQUEST_OPTION_TYPE_ENCRYPTION_MODE = 1,
    FF_REQUEST_OPTION_TYPE_ENCRYPTION_IV = 2,
    FF_REQUEST_OPTION_TYPE_ENCRYPTION_TAG = 3,
    FF_REQUEST_OPTION_TYPE_HTTPS = 4
};

struct ff_request_option_node
{
    enum ff_request_option_type type;
    uint16_t length;
    uint8_t *value;
};

struct ff_request_payload_node
{
    uint16_t offset;
    uint16_t length;
    uint8_t *value;
    struct ff_request_payload_node *next;
};

struct ff_request
{
    enum ff_request_state state;
    enum ff_request_version version;
    struct sockaddr source;
    time_t received_at;
    uint64_t request_id;
    uint8_t options_length;
    struct ff_request_option_node **options;
    uint64_t payload_length;
    uint64_t received_length;
    struct ff_request_payload_node *payload;
};

struct __raw_ff_request_header
{
    uint16_t version;
    uint64_t request_id;
    uint32_t total_length;
    uint32_t chunk_offset;
    uint16_t chunk_length;
} __attribute__((packed));

struct __raw_ff_request_option_header
{
    uint8_t type;
    uint16_t length;
} __attribute__((packed));

struct ff_request_option_node *ff_request_option_node_alloc(void);

void ff_request_option_load_buff(struct ff_request_option_node *node, uint32_t buff_size, void *buff);

void ff_request_option_node_free(struct ff_request_option_node *);

struct ff_request_payload_node *ff_request_payload_node_alloc(void);

void ff_request_payload_load_buff(struct ff_request_payload_node *node, uint32_t buff_size, void *buff);

void ff_request_payload_node_free(struct ff_request_payload_node *);

struct ff_request *ff_request_alloc(void);

void ff_request_free(struct ff_request *);

#endif
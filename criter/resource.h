/*
 * resource.h - Resource block structure for 64-bit clean standard C17.
 *
 */

#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <stdint.h>

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
typedef struct {
    uint32_t length;     /* Length of the resource data */
    uint16_t major_id;   /* Major resource ID */
    uint16_t minor_id;   /* Minor resource ID */
    char cookie[8];      /* Cookie validation (e.g. "FREECOM ") */
} 
#ifdef __GNUC__
__attribute__((packed))
#endif
resource_header_t;
#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif /* RESOURCE_H_ */

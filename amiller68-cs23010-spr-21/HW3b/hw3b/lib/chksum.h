#ifndef _CHKSUM_H_
#define _CHKSUM_H_

#include "packetsource.h"
#include <stdbool.h>

long chksum_serial(PacketSource_t *packet_source, volatile Packet_t * (* packet_method)(PacketSource_t *, int), int N, int M, bool correct);

long chksum_parallel(PacketSource_t *packet_source, volatile Packet_t * (* packet_method)(PacketSource_t *, int), int N, int M, int D, char L, char S, bool correct);

#endif

#ifndef _CHKSUM_H_
#define _CHKSUM_H_

#include "packetsource.h"

long chksum_serial(PacketSource_t *packet_source, volatile Packet_t * (* packet_method)(PacketSource_t *, int), int N, int T);

long chksum_serial_queue(PacketSource_t *packet_source, volatile Packet_t * (* packet_method)(PacketSource_t *, int), int N, int T, int D);

long chksum_parallel(PacketSource_t *packet_source, volatile Packet_t * (* packet_method)(PacketSource_t *, int), int N, int T, int D);

#endif

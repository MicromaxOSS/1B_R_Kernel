/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#undef TRACE_SYSTEM
#define TRACE_SYSTEM mmdvfs_events

#if !defined(_TRACE_MMDVFS_EVENTS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_MMDVFS_EVENTS_H

#include <linux/tracepoint.h>

TRACE_EVENT(mmqos__update_port,
	TP_PROTO(u32 larb_id, u32 master_id, s32 bw, s32 config),
	TP_ARGS(larb_id, master_id, bw, config),
	TP_STRUCT__entry(
		__field(u32, larb_id)
		__field(u32, master_id)
		__field(s32, bw)
		__field(s32, config)
	),
	TP_fast_assign(
		__entry->larb_id = larb_id;
		__entry->master_id = master_id;
		__entry->bw = bw;
		__entry->config = config;
	),
	TP_printk("bw_master_%u_%u=%d, config_master_%u_%u=%d",
		(u32)__entry->larb_id,
		(u32)__entry->master_id,
		(s32)__entry->bw,
		(u32)__entry->larb_id,
		(u32)__entry->master_id,
		(s32)__entry->config)
);

TRACE_EVENT(mmqos__update_larb,
	TP_PROTO(u32 comm, u32 larb_id, s32 bw, s32 bwl, s32 soft_mode),
	TP_ARGS(comm, larb_id, bw, bwl, soft_mode),
	TP_STRUCT__entry(
		__field(u32, comm)
		__field(u32, larb_id)
		__field(s32, bw)
		__field(s32, bwl)
		__field(s32, soft_mode)
	),
	TP_fast_assign(
		__entry->comm = comm;
		__entry->larb_id = larb_id;
		__entry->bw = bw;
		__entry->bwl = bwl;
		__entry->soft_mode = soft_mode;
	),
	TP_printk("comm=%u bw_larb_%u=%d, bwl_larb_%u=%d, soft=%d",
		(u32)__entry->comm,
		(u32)__entry->larb_id,
		(s32)__entry->bw,
		(u32)__entry->larb_id,
		(s32)__entry->bwl,
		(s32)__entry->soft_mode)
);

TRACE_EVENT(mmqos__update_qosbw,
	TP_PROTO(u32 larb_id, u32 port_id, u32 req_id, s32 bw),
	TP_ARGS(larb_id, port_id, req_id, bw),
	TP_STRUCT__entry(
		__field(u32, larb_id)
		__field(u32, port_id)
		__field(u32, req_id)
		__field(s32, bw)
	),
	TP_fast_assign(
		__entry->larb_id = larb_id;
		__entry->port_id = port_id;
		__entry->req_id = req_id;
		__entry->bw = bw;
	),
	TP_printk("larb_id=%u port_id=%u req_id=%u bw=%d",
		(u32)__entry->larb_id,
		(u32)__entry->port_id,
		(u32)__entry->req_id,
		(s32)__entry->bw)
);

#endif /* _TRACE_MMDVFS_EVENTS_H */

#undef TRACE_INCLUDE_FILE
#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#define TRACE_INCLUDE_FILE mmdvfs_events

/* This part must be outside protection */
#include <trace/define_trace.h>


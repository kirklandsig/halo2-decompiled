// @flags /O2 /arch:SSE /Gr
/* ONLINE_RESULT_UPLOAD.CPP: counts the failed attempts of a result upload
   and gives up on the server list after three attempts per server (built
   with SSE, apart from its neighbours in unknown_0b3d30.cpp) */
#include "upload_storage.h"
#include "online_result_cache.h"

void online_result_registration_clear(void);
void function_6b640(long task_index);
extern bool g_510580;  /* the upload server list is valid */
extern long g_510584;  /* the online task that queries the server list, or NONE */

/* the number of upload servers, none while the list is not valid */
static __forceinline long online_result_server_count(void)
{
	long count = 0;
	if (g_510580) count = g_510588.count;
	return count;
}

/* counts a failed upload attempt; after three per server, drops the
   server list, its query and the pending upload */
// @retail 0xb4050
void online_result_upload_record_failure(void)
{
	g_upload_storage.attempt_count++;
	if (g_upload_storage.attempt_count >= 3 * online_result_server_count())
	{
		online_result_registration_clear();
		long task_index = g_510584;
		if (task_index != NONE)
		{
			function_6b640(task_index);
			g_510584 = NONE;
		}
		g_510580 = false;
		g_upload_storage.upload_pending = 0;
	}
}

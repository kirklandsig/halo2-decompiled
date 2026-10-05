// @flags /O1 /Gr
/* UNKNOWN_22376B.CPP: the demo disc's demos
   and the content downloader, launched as other images */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "files.h"

s_type_acf665 *function_136710(s_type_acf665 *file, bool replace, const char *name);
bool function_1368f0(s_type_acf665 *file);

void function_12bf40(void);
void function_6cb60(void);

/* whether the demos and the downloader are on the disc, and whether that
   still has to be checked */
bool g_47ffb5 = true;
bool g_47ffb4 = true;
bool g_55e782;
bool g_55e781;

/* shuts the game down before another image is launched */
// @retail 0x22376b
void function_22376b(long type)
{
	function_12bf40();
	if (type != 4)
	{
		function_6cb60();
	}
	D3DDevice_PersistDisplay();
}

/* whether the disc has the demos */
// @retail 0x223784
bool function_223784(void)
{
	if (g_47ffb5)
	{
		s_type_acf665 file;

		if (function_136710(&file, false, "d:\\XDemos\\XDemos.xbe") && function_1368f0(&file))
		{
			g_55e782 = true;
		}
		g_47ffb5 = false;
	}
	return g_55e782;
}

/* launches the demos */
// @retail 0x2237d4
void function_2237d4(void)
{
	if (function_223784())
	{
		LAUNCH_DATA launch_data = { 0 };

		function_22376b(0);
		XLaunchNewImage("d:\\XDemos\\XDemos.xbe", &launch_data);
		XLaunchNewImage(NULL, &launch_data);
		for (;;)
		{
		}
	}
}

/* whether the disc has the downloader */
// @retail 0x22382b
bool function_22382b(void)
{
	if (g_47ffb4)
	{
		s_type_acf665 file;

		if (function_136710(&file, false, "d:\\Downloader.xbe") && function_1368f0(&file))
		{
			g_55e781 = true;
		}
		g_47ffb4 = false;
	}
	return g_55e781;
}

// @retail 0x22387b
void function_22387b(void)
{
	if (function_22382b())
	{
		LD_DOWNLOADER launch_data = { 0 };
		XONLINE_LOGON_STATE logon_state;

		if (SUCCEEDED(XOnlineSaveLogonState(&logon_state)))
			launch_data.LogonState = logon_state;
		function_22376b(0);
		XLaunchNewImage("d:\\Downloader.xbe", (PLAUNCH_DATA)&launch_data);
		XLaunchNewImage(NULL, (PLAUNCH_DATA)&launch_data);
		for (;;)
		{
		}
	}
}

/* the dashboard launch data: the reason, then the context and parameters */
struct s_dashboard_launch_data
{
	LD_LAUNCH_DASHBOARD dashboard;
	byte unused[MAX_LAUNCH_DATA_SIZE - sizeof(LD_LAUNCH_DASHBOARD)];
};

/* launches the dashboard at one of its pages */
// @retail 0x2238f4
void function_2238f4(long page, dword context, dword parameter1, dword parameter2)
{
	dword reasons[6] = { XLD_LAUNCH_DASHBOARD_MAIN_MENU, XLD_LAUNCH_DASHBOARD_MEMORY, XLD_LAUNCH_DASHBOARD_NETWORK_CONFIGURATION,
		XLD_LAUNCH_DASHBOARD_NEW_ACCOUNT_SIGNUP, XLD_LAUNCH_DASHBOARD_ACCOUNT_MANAGEMENT, XLD_LAUNCH_DASHBOARD_ONLINE_MENU };
	s_dashboard_launch_data launch_data = { 0 };

	function_22376b(page);
	launch_data.dashboard.dwReason = reasons[page];
	launch_data.dashboard.dwContext = context;
	launch_data.dashboard.dwParameter1 = parameter1;
	launch_data.dashboard.dwParameter2 = parameter2;
	XLaunchNewImage(NULL, (PLAUNCH_DATA)&launch_data);
	for (;;)
	{
	}
}

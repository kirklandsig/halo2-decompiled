// unknown_0662e0.cpp: the defaults of the network configuration
#include "unknown_11c920.h"
#include "unknown_0662e0.h"
#include <string.h>

// @flags /O2 /arch:SSE /Gr

s_network_configuration g_network_configuration;

void function_66db0();
void function_67090();
void function_67200();
void function_67a40();
void network_configuration_set_simulation_defaults();

// @retail 0x66db0
void function_66db0()
{
	g_network_configuration.entries_d20[25].real0 = 0.75f;
	g_network_configuration.entries_d20[25].value4 = 10000;
	g_network_configuration.entries_d20[16].real0 = 0.75f;
	g_network_configuration.entries_d20[16].value4 = 5000;
	g_network_configuration.entries_d20[24].real0 = 0.75f;
	g_network_configuration.entries_d20[24].value4 = 5000;
	g_network_configuration.entries_d20[22].real0 = 0.6f;
	g_network_configuration.entries_d20[22].value4 = 5000;
	g_network_configuration.entries_d20[23].real0 = 0.6f;
	g_network_configuration.entries_d20[23].value4 = 5000;
	g_network_configuration.entries_d20[21].real0 = 0.75f;
	g_network_configuration.entries_d20[21].value4 = 5000;
	g_network_configuration.entries_d20[19].real0 = 0.6f;
	g_network_configuration.entries_d20[19].value4 = 5000;
	g_network_configuration.entries_d20[20].real0 = 0.35f;
	g_network_configuration.entries_d20[20].value4 = 5000;
	g_network_configuration.entries_d20[17].real0 = 0.7f;
	g_network_configuration.entries_d20[17].value4 = -1;
	g_network_configuration.entries_d20[18].real0 = 0.7f;
	g_network_configuration.entries_d20[18].value4 = -1;
	g_network_configuration.entries_d20[15].real8 = 100.0f;
	g_network_configuration.entries_d20[15].realc = 0.5f;
	g_network_configuration.entries_d20[15].real10 = 0.75f;
	g_network_configuration.entries_d20[15].value4 = 5000;
	g_network_configuration.entries_d20[11].real8 = 100.0f;
	g_network_configuration.entries_d20[11].realc = 0.5f;
	g_network_configuration.entries_d20[11].real10 = 0.75f;
	g_network_configuration.entries_d20[11].value4 = 5000;
	g_network_configuration.entries_d20[14].real8 = 100.0f;
	g_network_configuration.entries_d20[14].realc = 0.5f;
	g_network_configuration.entries_d20[14].real10 = 0.75f;
	g_network_configuration.entries_d20[14].value4 = 5000;
	g_network_configuration.entries_d20[12].real8 = 100.0f;
	g_network_configuration.entries_d20[12].realc = 0.35f;
	g_network_configuration.entries_d20[12].real10 = 0.7f;
	g_network_configuration.entries_d20[12].value4 = 1500;
	g_network_configuration.entries_d20[13].real8 = 100.0f;
	g_network_configuration.entries_d20[13].realc = 0.3f;
	g_network_configuration.entries_d20[13].real10 = 0.7f;
	g_network_configuration.entries_d20[13].value4 = 5000;
	g_network_configuration.entries_d20[10].real8 = 100.0f;
	g_network_configuration.entries_d20[10].realc = 0.2f;
	g_network_configuration.entries_d20[10].real10 = 0.7f;
	g_network_configuration.entries_d20[10].value4 = 1000;
	g_network_configuration.entries_d20[8].real8 = 100.0f;
	g_network_configuration.entries_d20[8].realc = 0.2f;
	g_network_configuration.entries_d20[8].real10 = 0.73f;
	g_network_configuration.entries_d20[8].value4 = 5000;
	g_network_configuration.entries_d20[3].real8 = 100.0f;
	g_network_configuration.entries_d20[3].realc = 0.2f;
	g_network_configuration.entries_d20[3].real10 = 0.73f;
	g_network_configuration.entries_d20[3].value4 = -1;
	g_network_configuration.entries_d20[0].real8 = 100.0f;
	g_network_configuration.entries_d20[0].realc = 0.25f;
	g_network_configuration.entries_d20[0].real10 = 0.73f;
	g_network_configuration.entries_d20[0].value4 = 3000;
	g_network_configuration.entries_d20[7].real8 = 100.0f;
	g_network_configuration.entries_d20[7].realc = 0.15f;
	g_network_configuration.entries_d20[7].real10 = 0.73f;
	g_network_configuration.entries_d20[7].value4 = 3000;
	g_network_configuration.entries_d20[9].real8 = 100.0f;
	g_network_configuration.entries_d20[9].realc = 0.15f;
	g_network_configuration.entries_d20[9].real10 = 0.73f;
	g_network_configuration.entries_d20[9].value4 = 5000;
	g_network_configuration.entries_d20[1].real8 = 100.0f;
	g_network_configuration.entries_d20[1].realc = 0.1f;
	g_network_configuration.entries_d20[1].real10 = 0.73f;
	g_network_configuration.entries_d20[1].value4 = 3000;
	g_network_configuration.entries_d20[2].real8 = 100.0f;
	g_network_configuration.entries_d20[2].realc = 0.1f;
	g_network_configuration.entries_d20[2].real10 = 0.73f;
	g_network_configuration.entries_d20[2].value4 = 5000;
	g_network_configuration.entries_d20[4].real8 = 100.0f;
	g_network_configuration.entries_d20[4].realc = 0.1f;
	g_network_configuration.entries_d20[4].real10 = 0.73f;
	g_network_configuration.entries_d20[4].value4 = 5000;
	g_network_configuration.entries_d20[5].real8 = 100.0f;
	g_network_configuration.entries_d20[5].realc = 0.1f;
	g_network_configuration.entries_d20[5].real10 = 0.73f;
	g_network_configuration.entries_d20[5].value4 = 5000;
	g_network_configuration.entries_d20[6].real8 = 100.0f;
	g_network_configuration.entries_d20[6].realc = 0.1f;
	g_network_configuration.entries_d20[6].real10 = 0.73f;
	g_network_configuration.entries_d20[6].value4 = 5000;
}

// @retail 0x67090
void function_67090()
{
	g_network_configuration.entries_f28[0].real0 = 0.7f;
	g_network_configuration.entries_f28[1].real0 = 0.7f;
	g_network_configuration.entries_f28[2].real0 = 0.7f;
	g_network_configuration.entries_f28[3].real0 = 0.7f;
	g_network_configuration.entries_f28[4].real0 = 0.7f;
	g_network_configuration.entries_f28[5].real0 = 0.7f;
	g_network_configuration.entries_f28[7].real0 = 0.7f;
	g_network_configuration.entries_f28[6].real8 = 0.5f;
	g_network_configuration.entries_f28[6].realc = 0.8f;
	g_network_configuration.entries_f28[6].real4 = 100.0f;
	g_network_configuration.entries_f28[9].real8 = 0.5f;
	g_network_configuration.entries_f28[9].realc = 0.8f;
	g_network_configuration.entries_f28[9].real4 = 100.0f;
	g_network_configuration.entries_f28[13].real8 = 0.4f;
	g_network_configuration.entries_f28[13].realc = 0.8f;
	g_network_configuration.entries_f28[13].real4 = 100.0f;
	g_network_configuration.entries_f28[12].real8 = 0.35f;
	g_network_configuration.entries_f28[12].realc = 0.8f;
	g_network_configuration.entries_f28[12].real4 = 100.0f;
	g_network_configuration.entries_f28[14].real8 = 0.1f;
	g_network_configuration.entries_f28[14].realc = 0.8f;
	g_network_configuration.entries_f28[14].real4 = 100.0f;
	g_network_configuration.entries_f28[10].real8 = 0.1f;
	g_network_configuration.entries_f28[10].realc = 0.7f;
	g_network_configuration.entries_f28[10].real4 = 100.0f;
	g_network_configuration.entries_f28[11].real8 = 0.1f;
	g_network_configuration.entries_f28[11].realc = 0.7f;
	g_network_configuration.entries_f28[11].real4 = 100.0f;
	g_network_configuration.entries_f28[15].real8 = 0.1f;
	g_network_configuration.entries_f28[15].realc = 0.65f;
	g_network_configuration.entries_f28[15].real4 = 100.0f;
	g_network_configuration.entries_f28[16].real8 = 0.1f;
	g_network_configuration.entries_f28[16].realc = 0.65f;
	g_network_configuration.entries_f28[16].real4 = 100.0f;
	g_network_configuration.entries_f28[8].real8 = 0.1f;
	g_network_configuration.entries_f28[8].realc = 0.65f;
	g_network_configuration.entries_f28[8].real4 = 100.0f;
}

// @retail 0x67200
void function_67200()
{
	g_network_configuration.entries_f28[0].real1c = 1.0f;
	g_network_configuration.entries_f28[0].real18 = 0.6f;
	g_network_configuration.entries_f28[0].value20 = 0;
	g_network_configuration.entries_f28[0].value24 = 1000;
	g_network_configuration.entries_f28[0].real2c = 0.2f;
	g_network_configuration.entries_f28[0].real28 = 0.05f;
	g_network_configuration.entries_f28[0].value30 = 1000;
	g_network_configuration.entries_f28[0].real38 = 0.67f;
	g_network_configuration.entries_f28[0].real34 = 0.2f;
	g_network_configuration.entries_f28[0].real3c = 0.8f;
	g_network_configuration.entries_f28[0].real14 = 100.0f;
	g_network_configuration.entries_f28[1].real1c = 1.0f;
	g_network_configuration.entries_f28[1].real18 = 0.6f;
	g_network_configuration.entries_f28[1].value20 = 0;
	g_network_configuration.entries_f28[1].value24 = 1000;
	g_network_configuration.entries_f28[1].real2c = 0.2f;
	g_network_configuration.entries_f28[1].real28 = 0.05f;
	g_network_configuration.entries_f28[1].value30 = 1000;
	g_network_configuration.entries_f28[1].real38 = 0.67f;
	g_network_configuration.entries_f28[1].real34 = 0.2f;
	g_network_configuration.entries_f28[1].real3c = 0.8f;
	g_network_configuration.entries_f28[1].real14 = 100.0f;
	g_network_configuration.entries_f28[2].real1c = 1.0f;
	g_network_configuration.entries_f28[2].real18 = 0.6f;
	g_network_configuration.entries_f28[2].value20 = 0;
	g_network_configuration.entries_f28[2].value24 = 1000;
	g_network_configuration.entries_f28[2].real2c = 0.2f;
	g_network_configuration.entries_f28[2].real28 = 0.05f;
	g_network_configuration.entries_f28[2].value30 = 1000;
	g_network_configuration.entries_f28[2].real38 = 0.67f;
	g_network_configuration.entries_f28[2].real34 = 0.2f;
	g_network_configuration.entries_f28[2].real3c = 0.8f;
	g_network_configuration.entries_f28[2].real14 = 100.0f;
	g_network_configuration.entries_f28[3].real1c = 1.0f;
	g_network_configuration.entries_f28[3].real18 = 0.6f;
	g_network_configuration.entries_f28[3].value20 = 0;
	g_network_configuration.entries_f28[3].value24 = 1000;
	g_network_configuration.entries_f28[3].real2c = 0.2f;
	g_network_configuration.entries_f28[3].real28 = 0.05f;
	g_network_configuration.entries_f28[3].value30 = 1000;
	g_network_configuration.entries_f28[3].real38 = 0.67f;
	g_network_configuration.entries_f28[3].real34 = 0.2f;
	g_network_configuration.entries_f28[3].real3c = 0.8f;
	g_network_configuration.entries_f28[3].real14 = 100.0f;
	g_network_configuration.entries_f28[4].real1c = 1.0f;
	g_network_configuration.entries_f28[4].real18 = 0.6f;
	g_network_configuration.entries_f28[4].value20 = 0;
	g_network_configuration.entries_f28[4].value24 = 1000;
	g_network_configuration.entries_f28[4].real2c = 0.2f;
	g_network_configuration.entries_f28[4].real28 = 0.05f;
	g_network_configuration.entries_f28[4].value30 = 1000;
	g_network_configuration.entries_f28[4].real38 = 0.67f;
	g_network_configuration.entries_f28[4].real34 = 0.2f;
	g_network_configuration.entries_f28[4].real3c = 0.8f;
	g_network_configuration.entries_f28[4].real14 = 100.0f;
	g_network_configuration.entries_f28[5].real1c = 1.0f;
	g_network_configuration.entries_f28[5].real18 = 0.6f;
	g_network_configuration.entries_f28[5].value20 = 0;
	g_network_configuration.entries_f28[5].value24 = 1000;
	g_network_configuration.entries_f28[5].real2c = 0.2f;
	g_network_configuration.entries_f28[5].real28 = 0.05f;
	g_network_configuration.entries_f28[5].value30 = 1000;
	g_network_configuration.entries_f28[5].real38 = 0.67f;
	g_network_configuration.entries_f28[5].real34 = 0.2f;
	g_network_configuration.entries_f28[5].real3c = 0.8f;
	g_network_configuration.entries_f28[5].real14 = 100.0f;
	g_network_configuration.entries_f28[7].real1c = 1.0f;
	g_network_configuration.entries_f28[7].real18 = 0.6f;
	g_network_configuration.entries_f28[7].value20 = 0;
	g_network_configuration.entries_f28[7].value24 = 1000;
	g_network_configuration.entries_f28[7].real2c = 0.2f;
	g_network_configuration.entries_f28[7].real28 = 0.1f;
	g_network_configuration.entries_f28[7].value30 = 1000;
	g_network_configuration.entries_f28[7].real38 = 0.67f;
	g_network_configuration.entries_f28[7].real34 = 0.35f;
	g_network_configuration.entries_f28[7].real3c = 0.8f;
	g_network_configuration.entries_f28[7].real14 = 100.0f;
	g_network_configuration.entries_f28[6].real1c = 1.0f;
	g_network_configuration.entries_f28[6].real18 = 0.6f;
	g_network_configuration.entries_f28[6].value20 = 0;
	g_network_configuration.entries_f28[6].value24 = 1000;
	g_network_configuration.entries_f28[6].real2c = 0.2f;
	g_network_configuration.entries_f28[6].real28 = 0.1f;
	g_network_configuration.entries_f28[6].value30 = 1000;
	g_network_configuration.entries_f28[6].real38 = 0.67f;
	g_network_configuration.entries_f28[6].real34 = 0.35f;
	g_network_configuration.entries_f28[6].real3c = 0.8f;
	g_network_configuration.entries_f28[6].real14 = 100.0f;
	g_network_configuration.entries_f28[9].real1c = 1.0f;
	g_network_configuration.entries_f28[9].real18 = 0.6f;
	g_network_configuration.entries_f28[9].value20 = 0;
	g_network_configuration.entries_f28[9].value24 = 800;
	g_network_configuration.entries_f28[9].real2c = 0.55f;
	g_network_configuration.entries_f28[9].real28 = 0.32f;
	g_network_configuration.entries_f28[9].value30 = 1000;
	g_network_configuration.entries_f28[9].real38 = 0.7f;
	g_network_configuration.entries_f28[9].real34 = 0.55f;
	g_network_configuration.entries_f28[9].real3c = 0.83f;
	g_network_configuration.entries_f28[9].real40 = 0.9f;
	g_network_configuration.entries_f28[9].real44 = 0.9f;
	g_network_configuration.entries_f28[9].real14 = 100.0f;
	g_network_configuration.entries_f28[12].real1c = 1.0f;
	g_network_configuration.entries_f28[12].real18 = 0.6f;
	g_network_configuration.entries_f28[12].value20 = 0;
	g_network_configuration.entries_f28[12].value24 = 800;
	g_network_configuration.entries_f28[12].real2c = 0.55f;
	g_network_configuration.entries_f28[12].real28 = 0.25f;
	g_network_configuration.entries_f28[12].value30 = 1000;
	g_network_configuration.entries_f28[12].real38 = 0.7f;
	g_network_configuration.entries_f28[12].real34 = 0.55f;
	g_network_configuration.entries_f28[12].real3c = 0.83f;
	g_network_configuration.entries_f28[12].real48 = 0.1f;
	g_network_configuration.entries_f28[12].real14 = 100.0f;
	g_network_configuration.entries_f28[13].real1c = 1.0f;
	g_network_configuration.entries_f28[13].real18 = 0.6f;
	g_network_configuration.entries_f28[13].value20 = 0;
	g_network_configuration.entries_f28[13].value24 = 500;
	g_network_configuration.entries_f28[13].real2c = 0.6f;
	g_network_configuration.entries_f28[13].real28 = 0.3f;
	g_network_configuration.entries_f28[13].value30 = 1000;
	g_network_configuration.entries_f28[13].real38 = 0.73f;
	g_network_configuration.entries_f28[13].real34 = 0.6f;
	g_network_configuration.entries_f28[13].real3c = 0.82f;
	g_network_configuration.entries_f28[13].real14 = 100.0f;
	g_network_configuration.entries_f28[16].real1c = 1.0f;
	g_network_configuration.entries_f28[16].real18 = 0.8f;
	g_network_configuration.entries_f28[16].value20 = 200;
	g_network_configuration.entries_f28[16].value24 = 1000;
	g_network_configuration.entries_f28[16].real2c = 0.23f;
	g_network_configuration.entries_f28[16].real28 = 0.13f;
	g_network_configuration.entries_f28[16].value30 = 1000;
	g_network_configuration.entries_f28[16].real38 = 0.67f;
	g_network_configuration.entries_f28[16].real34 = 0.23f;
	g_network_configuration.entries_f28[16].real3c = 0.8f;
	g_network_configuration.entries_f28[16].real14 = 100.0f;
	g_network_configuration.entries_f28[14].real1c = 1.0f;
	g_network_configuration.entries_f28[14].real18 = 0.8f;
	g_network_configuration.entries_f28[14].value20 = 200;
	g_network_configuration.entries_f28[14].value24 = 1000;
	g_network_configuration.entries_f28[14].real2c = 0.23f;
	g_network_configuration.entries_f28[14].real28 = 0.1f;
	g_network_configuration.entries_f28[14].value30 = 1000;
	g_network_configuration.entries_f28[14].real38 = 0.72f;
	g_network_configuration.entries_f28[14].real34 = 0.2f;
	g_network_configuration.entries_f28[14].real3c = 0.82f;
	g_network_configuration.entries_f28[14].real14 = 100.0f;
	g_network_configuration.entries_f28[10].real1c = 1.0f;
	g_network_configuration.entries_f28[10].real18 = 0.8f;
	g_network_configuration.entries_f28[10].value20 = 200;
	g_network_configuration.entries_f28[10].value24 = 1000;
	g_network_configuration.entries_f28[10].real2c = 0.23f;
	g_network_configuration.entries_f28[10].real28 = 0.1f;
	g_network_configuration.entries_f28[10].value30 = 1000;
	g_network_configuration.entries_f28[10].real38 = 0.67f;
	g_network_configuration.entries_f28[10].real34 = 0.3f;
	g_network_configuration.entries_f28[10].real3c = 0.82f;
	g_network_configuration.entries_f28[10].real14 = 100.0f;
	g_network_configuration.entries_f28[11].real1c = 1.0f;
	g_network_configuration.entries_f28[11].real18 = 0.8f;
	g_network_configuration.entries_f28[11].value20 = 200;
	g_network_configuration.entries_f28[11].value24 = 1000;
	g_network_configuration.entries_f28[11].real2c = 0.23f;
	g_network_configuration.entries_f28[11].real28 = 0.1f;
	g_network_configuration.entries_f28[11].value30 = 1000;
	g_network_configuration.entries_f28[11].real38 = 0.67f;
	g_network_configuration.entries_f28[11].real34 = 0.3f;
	g_network_configuration.entries_f28[11].real3c = 0.82f;
	g_network_configuration.entries_f28[11].real14 = 100.0f;
	g_network_configuration.entries_f28[15].real1c = 1.0f;
	g_network_configuration.entries_f28[15].real18 = 0.6f;
	g_network_configuration.entries_f28[15].value20 = 0;
	g_network_configuration.entries_f28[15].value24 = 1000;
	g_network_configuration.entries_f28[15].real2c = 0.2f;
	g_network_configuration.entries_f28[15].real28 = 0.05f;
	g_network_configuration.entries_f28[15].value30 = 1000;
	g_network_configuration.entries_f28[15].real38 = 0.63f;
	g_network_configuration.entries_f28[15].real34 = 0.2f;
	g_network_configuration.entries_f28[15].real3c = 0.8f;
	g_network_configuration.entries_f28[15].real14 = 100.0f;
	g_network_configuration.entries_f28[8].real1c = 1.0f;
	g_network_configuration.entries_f28[8].real18 = 0.6f;
	g_network_configuration.entries_f28[8].value20 = 0;
	g_network_configuration.entries_f28[8].value24 = 1000;
	g_network_configuration.entries_f28[8].real2c = 0.2f;
	g_network_configuration.entries_f28[8].real28 = 0.05f;
	g_network_configuration.entries_f28[8].value30 = 1000;
	g_network_configuration.entries_f28[8].real38 = 0.63f;
	g_network_configuration.entries_f28[8].real34 = 0.2f;
	g_network_configuration.entries_f28[8].real3c = 0.8f;
	g_network_configuration.entries_f28[8].real14 = 100.0f;
}

// @retail 0x678d0
void network_configuration_set_simulation_defaults()
{
	g_network_configuration.realca8 = 0.994f;
	g_network_configuration.realcac = 0.999f;
	g_network_configuration.realcb0 = 0.95f;
	g_network_configuration.realcb4 = 1.0f;
	g_network_configuration.realcb8 = 100.0f;
	g_network_configuration.realcbc = 1.0f;
	g_network_configuration.realcc0 = 0.6f;
	g_network_configuration.valuecc4 = 0;
	g_network_configuration.valuecc8 = 1000;
	g_network_configuration.realccc = 1.0f;
	g_network_configuration.realcd0 = 0.99f;
	g_network_configuration.realcd4 = 0.5f;
	g_network_configuration.realcd8 = 5.0f;
	g_network_configuration.valuecdc = 6;
	g_network_configuration.valuece0 = 60;
	g_network_configuration.valuece4 = 800;
	g_network_configuration.valuece8 = 20;
	g_network_configuration.valuecec = 2;
	g_network_configuration.valuecf0 = 30;
	g_network_configuration.valuecf4 = 6;
	g_network_configuration.valuecf8 = 0;
	g_network_configuration.valuecfc = 20;
	g_network_configuration.valued00 = 10000;
	g_network_configuration.reald04 = 2.0f;
	g_network_configuration.valued08 = 8;
	g_network_configuration.valued0c = 60000;
	g_network_configuration.valued10 = 3;
	g_network_configuration.valued14 = 2000;
	g_network_configuration.valued18 = 5;
	g_network_configuration.valued1c = 180000;
	function_66db0();
	function_67090();
	function_67200();
}

// @retail 0x67a40
void function_67a40()
{
	g_network_configuration.value14a0 = 1;
	g_network_configuration.value14a4 = 0;
	g_network_configuration.value14c4 = 3;
	g_network_configuration.value14c8 = 0;
	g_network_configuration.value14cc = 500;
	g_network_configuration.value14d0 = 500;
	g_network_configuration.value14e8 = 4;
	g_network_configuration.value14ec = 500;
	g_network_configuration.value14f0 = 500;
	g_network_configuration.value14f4 = 1000;
	g_network_configuration.value14f8 = 2000;
	g_network_configuration.value150c = 15000;
	g_network_configuration.value1510 = 1000;
	g_network_configuration.value1514 = 2000;
	g_network_configuration.value1518 = 500;
	g_network_configuration.value151c = 2000;
	g_network_configuration.value1520 = 4000;
	g_network_configuration.value1528 = 1000;
	g_network_configuration.value152c = 5000;
	g_network_configuration.value1524 = 10000;
	g_network_configuration.real1530 = 0.5f;
	g_network_configuration.real1534 = 1.0f;
	g_network_configuration.real1538 = 30.0f;
	g_network_configuration.value153c = 7;
	g_network_configuration.real1540 = 1.0f;
	g_network_configuration.real1544 = 0.6666667f;
	g_network_configuration.real1548 = 0.5f;
	g_network_configuration.real154c = 0.33333334f;
	g_network_configuration.real1550 = 0.25f;
	g_network_configuration.real1554 = 0.2f;
	g_network_configuration.real1558 = 0.1f;
	g_network_configuration.value1580 = 200;
	g_network_configuration.real1584 = 0.25f;
	g_network_configuration.real1588 = 0.5f;
	g_network_configuration.real158c = 0.75f;
	g_network_configuration.real1590 = 12.0f;
	g_network_configuration.value1594 = 2000;
	g_network_configuration.value1598 = 2000;
	g_network_configuration.value159c = 30;
	g_network_configuration.value15a0 = 4000;
	g_network_configuration.real15a4 = 0.5f;
	g_network_configuration.value15a8 = 64;
	g_network_configuration.value15ac = 96;
	g_network_configuration.real15b0 = 0.33333334f;
	g_network_configuration.value15b4 = 10240;
	g_network_configuration.value15b8 = 1000;
	g_network_configuration.real15bc = 0.5f;
	g_network_configuration.value15c0 = 3;
	g_network_configuration.value15c4 = 40;
	g_network_configuration.value15c8 = 5000;
	g_network_configuration.value15cc = 15;
	g_network_configuration.value15d0 = 100;
	g_network_configuration.value15d4 = 50;
	g_network_configuration.value15d8 = 50;
	g_network_configuration.value15dc = 15;
	g_network_configuration.value15e0 = 8;
	g_network_configuration.flag15e4 = 1;
	g_network_configuration.value15e8 = 2000;
	g_network_configuration.value15ec = 4096;
	g_network_configuration.value15f0 = 71680;
	g_network_configuration.value15f4 = 1000;
	g_network_configuration.value15f8 = 30720;
	g_network_configuration.value15fc = 122880;
	g_network_configuration.value1600 = 512000;
	g_network_configuration.value1604 = 8192;
	g_network_configuration.value1608 = 40;
	g_network_configuration.value160c = 320;
	g_network_configuration.value1610 = 3;
	g_network_configuration.value1614 = 32;
	g_network_configuration.real1618 = 0.1f;
	g_network_configuration.value161c = 4;
	g_network_configuration.real1620 = 0.8f;
	g_network_configuration.value1624 = 10;
	g_network_configuration.value1628 = 21;
	g_network_configuration.value162c = 3072;
	g_network_configuration.real1630 = 0.2f;
	g_network_configuration.value1634 = 5120;
	g_network_configuration.real1638 = 0.3f;
	g_network_configuration.value163c = 5000;
	g_network_configuration.value1640 = 1500;
	g_network_configuration.value1644 = 1500;
	g_network_configuration.value1654 = 3;
	g_network_configuration.value1658 = 6144;
	g_network_configuration.value165c = 20000;
	g_network_configuration.value1648 = 30720;
	g_network_configuration.real164c = 0.75f;
	g_network_configuration.value1650 = 60000;
	g_network_configuration.value1660 = 6;
	g_network_configuration.real1664 = 0.5f;
	g_network_configuration.real1668 = 20.0f;
	g_network_configuration.value166c = 0;
	g_network_configuration.real1670 = 0.000390625f;
	g_network_configuration.value1674 = 160;
	g_network_configuration.value1678 = 40;
	g_network_configuration.value167c = 120;
	g_network_configuration.value1680 = 3;
	g_network_configuration.value1684 = 10;
	g_network_configuration.value1688 = 10;
	g_network_configuration.value168c = 3;
	g_network_configuration.value1690 = 8000;
}

__forceinline long real_round_to_long(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return result;
}
__forceinline void network_configuration_levels_initialize(s_network_configuration_levels *levels, long base)
{
	levels->base = base;
	for (long i = 0; i <= 16; i++)
	{
		if (i <= 1)
		{
			levels->values[i] = 0;
		}
		else
		{
			levels->values[i] = levels->base + (i - 1) * real_round_to_long(12288.0f);
		}
	}
}

// @retail 0x66330
void network_configuration_set_defaults()
{
	long i;
	long j;

	memset(&g_network_configuration, 0, sizeof(g_network_configuration));

	g_network_configuration.real10 = 0.25f;
	g_network_configuration.value14 = 2;
	g_network_configuration.value18 = 20480;
	g_network_configuration.value1c = 51200;
	g_network_configuration.value20 = 300000;
	g_network_configuration.real24 = 0.5f;
	g_network_configuration.value28 = 5;
	g_network_configuration.value2c = 65536;
	g_network_configuration.value30 = 32768;
	g_network_configuration.real34 = 1.0172526e-05f;
	g_network_configuration.real38 = 0.0125f;
	g_network_configuration.real3c = 2.0f;

	for (i = 0; i <= 16; i++)
	{
		long value40;
		long value84;

		if (i <= 1)
		{
			value40 = 0;
			value84 = 0;
		}
		else
		{
			long step40;
			long step84;

			if (i < 4)
			{
				step40 = 8192;
				step84 = 40960;
			}
			else
			{
				real t = (i - 4) * 0.083333336f;
				step40 = real_round_to_long(t * 9216.0f + 8192.0f);
				step84 = real_round_to_long(t * 30720.0f + 40960.0f);
			}
			value40 = (i - 1) * step40 + 15360;
			value84 = (i - 1) * step84 + 61440;
		}
		g_network_configuration.value40[i] = value40;
		g_network_configuration.value84[i] = value84;
	}
	g_network_configuration.valuec8 = 8;
	g_network_configuration.valuecc = 131072;
	g_network_configuration.valued0 = 131072;
	g_network_configuration.valued4 = 10240;
	g_network_configuration.valued8 = 262144;
	g_network_configuration.valuedc = 262144;
	g_network_configuration.valuee0 = 131072;
	g_network_configuration.valuee4 = 131072;

	network_configuration_levels_initialize(&g_network_configuration.levels[1], 35840);
	network_configuration_levels_initialize(&g_network_configuration.levels[0], 25600);

	g_network_configuration.value178 = 10000;
	g_network_configuration.value17c = 10000;
	g_network_configuration.value180 = 3000;
	g_network_configuration.value184 = 100;
	g_network_configuration.value188 = 100;
	g_network_configuration.value18c = 65536;
	g_network_configuration.value190 = 10000;
	g_network_configuration.value194 = 60000;
	g_network_configuration.value198 = 240000;
	g_network_configuration.value19c = 60000;
	g_network_configuration.value1a0 = 6;
	g_network_configuration.value1a4 = 8;
	g_network_configuration.value1a8 = 10000;
	g_network_configuration.flag1ac = false;
	g_network_configuration.value1b0 = 5;
	g_network_configuration.value1b8 = 150;
	g_network_configuration.value1bc = 3;
	g_network_configuration.value1c8 = 50;
	g_network_configuration.value1d4 = -200;
	g_network_configuration.value1d8 = -200;
	g_network_configuration.value1dc = -200;
	g_network_configuration.value1e0 = -200;
	g_network_configuration.value1e4 = -200;
	g_network_configuration.value1e8 = 200;
	g_network_configuration.value1ec = 100;
	g_network_configuration.value1f0 = 4;
	g_network_configuration.value1f4 = 100;
	g_network_configuration.value1f8 = 25;
	g_network_configuration.value340 = 50;
	g_network_configuration.value344 = 180;
	g_network_configuration.value348 = 2.0;
	g_network_configuration.value350 = 100;
	g_network_configuration.value1c0 = 100;
	g_network_configuration.value1c4 = 80;
	g_network_configuration.value1cc = 0;
	g_network_configuration.value1d0 = -100;

	bool flags[9] = { false, true, false, false, false, false, true, true, false };
	memset(g_network_configuration.value1fc, 0, sizeof(g_network_configuration.value1fc));
	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 9; j++)
		{
			long value;

			if (flags[i])
			{
				value = i == j ? 100 : -100;
			}
			else if (i == j)
			{
				value = 0;
			}
			else
			{
				value = flags[j] ? -100 : -50;
			}
			g_network_configuration.value1fc[i][j] = value;
			g_network_configuration.value1fc[j][i] = value;
		}
	}

	g_network_configuration.value354 = 5000;
	g_network_configuration.value358 = 65536;
	g_network_configuration.value35c = 20000;
	g_network_configuration.value360 = 10000;
	g_network_configuration.value364 = 100;
	g_network_configuration.value368 = 5;
	g_network_configuration.value36c = 100;
	g_network_configuration.value370 = 50;
	g_network_configuration.value374 = 300;

	memset(g_network_configuration.value378, 0, sizeof(g_network_configuration.value378));
	for (i = 0; i < 120; i++)
	{
		g_network_configuration.value378[i] = 180;
	}
	g_network_configuration.value378[120] = 170;
	g_network_configuration.value378[121] = 160;
	g_network_configuration.value378[122] = 150;
	g_network_configuration.value378[123] = 140;
	g_network_configuration.value378[124] = 130;
	g_network_configuration.value378[125] = 120;
	g_network_configuration.value378[126] = 110;
	g_network_configuration.value378[127] = 100;
	g_network_configuration.value378[128] = 90;
	g_network_configuration.value378[129] = 80;
	g_network_configuration.value378[130] = 70;
	g_network_configuration.value378[131] = 60;
	g_network_configuration.value378[132] = 50;
	g_network_configuration.value378[133] = 40;
	g_network_configuration.value378[134] = 30;
	for (i = 135; i < 255; i++)
	{
		g_network_configuration.value378[i] = 20;
	}
	for (i = 0; i < 120; i++)
	{
		g_network_configuration.value576[i] = -20;
	}
	g_network_configuration.value576[120] = -30;
	g_network_configuration.value576[121] = -40;
	g_network_configuration.value576[122] = -50;
	g_network_configuration.value576[123] = -60;
	g_network_configuration.value576[124] = -70;
	g_network_configuration.value576[125] = -80;
	g_network_configuration.value576[126] = -90;
	g_network_configuration.value576[127] = -100;
	g_network_configuration.value576[128] = -110;
	g_network_configuration.value576[129] = -120;
	g_network_configuration.value576[130] = -130;
	g_network_configuration.value576[131] = -140;
	g_network_configuration.value576[132] = -150;
	g_network_configuration.value576[133] = -160;
	g_network_configuration.value576[134] = -170;
	for (i = 135; i < 255; i++)
	{
		g_network_configuration.value576[i] = -180;
	}

	g_network_configuration.value774[0] = 0;
	g_network_configuration.value774[1] = 100;
	g_network_configuration.value774[2] = 200;
	g_network_configuration.value774[3] = 400;
	g_network_configuration.value774[4] = 600;
	g_network_configuration.value774[5] = 900;
	g_network_configuration.value774[6] = 1200;
	g_network_configuration.value774[7] = 1600;
	{
		long value = 2000;
		for (i = 8; i < 40; i++)
		{
			g_network_configuration.value774[i] = value;
			value += 500;
		}
	}
	g_network_configuration.value774[40] = g_network_configuration.value774[39] + 600;
	g_network_configuration.value774[41] = g_network_configuration.value774[40] + 800;
	g_network_configuration.value774[42] = g_network_configuration.value774[41] + 1100;
	g_network_configuration.value774[43] = g_network_configuration.value774[42] + 1500;
	g_network_configuration.value774[44] = g_network_configuration.value774[43] + 2000;
	g_network_configuration.value774[45] = g_network_configuration.value774[44] + 2600;
	g_network_configuration.value774[46] = g_network_configuration.value774[45] + 3300;
	g_network_configuration.value774[47] = g_network_configuration.value774[46] + 4100;
	g_network_configuration.value774[48] = g_network_configuration.value774[47] + 5000;
	g_network_configuration.value774[49] = g_network_configuration.value774[48] + 6000;
	for (i = 50; i < 128; i++)
	{
		g_network_configuration.value774[i] = 0x3fffffff;
	}

	g_network_configuration.value974[0] = 0;
	g_network_configuration.value974[1] = 5;
	g_network_configuration.value974[2] = 10;
	g_network_configuration.value974[3] = 20;
	g_network_configuration.value974[4] = 35;
	g_network_configuration.value974[5] = 45;
	g_network_configuration.value974[6] = 55;
	g_network_configuration.value974[7] = 60;
	g_network_configuration.value974[8] = 65;
	g_network_configuration.value974[9] = 70;
	g_network_configuration.value974[10] = 75;
	g_network_configuration.value974[11] = 80;
	g_network_configuration.value974[12] = 85;
	g_network_configuration.value974[13] = 90;
	g_network_configuration.value974[14] = 95;
	memset(&g_network_configuration.value974[15], 100, 113);
	memset(&g_network_configuration.value9f4[0], 100, 41);
	g_network_configuration.value9f4[41] = 95;
	g_network_configuration.value9f4[42] = 90;
	g_network_configuration.value9f4[43] = 85;
	g_network_configuration.value9f4[44] = 80;
	g_network_configuration.value9f4[45] = 70;
	g_network_configuration.value9f4[46] = 65;
	g_network_configuration.value9f4[47] = 60;
	g_network_configuration.value9f4[48] = 55;
	g_network_configuration.value9f4[49] = 50;
	g_network_configuration.value9f4[50] = 50;
	memset(&g_network_configuration.value9f4[51], 0, 77);

	memset(g_network_configuration.valueb74, 0, sizeof(g_network_configuration.valueb74));
	memset(g_network_configuration.valuea74, 0, sizeof(g_network_configuration.valuea74));

	g_network_configuration.valuec74 = 5000;
	g_network_configuration.valuec78 = 200;
	g_network_configuration.valuec7c = -200;
	g_network_configuration.valuec80 = 2000;
	g_network_configuration.valuec84 = 2000;
	g_network_configuration.valuec88 = 2000;
	g_network_configuration.valuec8c = 2000;
	g_network_configuration.valuec90 = 10000;
	g_network_configuration.valuec94 = 15000;
	g_network_configuration.valuec98 = 100;
	g_network_configuration.valuec9c = 32768;
	g_network_configuration.valueca0 = 32768;
	g_network_configuration.valueca4 = 16384;

	network_configuration_set_simulation_defaults();

	g_network_configuration.value1434 = 20000;
	g_network_configuration.real1438 = 0.5f;
	g_network_configuration.value143c = 1000;
	g_network_configuration.real1440 = 0.84f;
	g_network_configuration.real1444 = 0.62f;
	g_network_configuration.real1448 = 0.25f;
	g_network_configuration.real144c = 0.32f;
	g_network_configuration.real1450 = 0.3f;

	function_67a40();

	g_network_configuration.value1454 = 1000;
	g_network_configuration.value1458 = 8000;
	g_network_configuration.value145c = 8000;
	g_network_configuration.value1460 = 200;
	g_network_configuration.value1464 = 1000;
	g_network_configuration.value1468 = 6000;
	g_network_configuration.value146c = 1500;
	g_network_configuration.value1470 = 500;
	g_network_configuration.value1474 = 3000;
	g_network_configuration.value1478 = 10000;
	g_network_configuration.value147c = 10000;
	g_network_configuration.value1480 = 12000;
	g_network_configuration.value1484 = 5000;
	g_network_configuration.value1488 = 45000;
	g_network_configuration.value148c = 4000;
	g_network_configuration.value1490 = 6000;
	g_network_configuration.value1494 = 1000;
	g_network_configuration.value1498 = 120000;
	g_network_configuration.value149c = 5000;
	g_network_configuration.value1694 = 500;
	g_network_configuration.value1698 = 3;
	g_network_configuration.value169c = 2000;
	g_network_configuration.value16a0 = 6000;
	g_network_configuration.value16a4 = 5000;
	g_network_configuration.value16a8 = 20;
	g_network_configuration.value16ac = 64;
	g_network_configuration.value16b0 = 3;
	g_network_configuration.value16b4 = 5000;
	g_network_configuration.value16b8 = 3;
	g_network_configuration.value16bc = 2;
	g_network_configuration.value16c0 = 4;
	g_network_configuration.value16c4 = 80;
	g_network_configuration.value16c8 = 150;
	g_network_configuration.value16cc = 9999;
	g_network_configuration.value16d0 = 150;
	g_network_configuration.value16d4 = 1000;
	g_network_configuration.value16d8 = 200;
	g_network_configuration.value16dc = 100;
	g_network_configuration.value16e0 = 700;
	g_network_configuration.value16e4 = 200;
	g_network_configuration.real16e8 = 3.0f;
	g_network_configuration.value16ec = 400;
	g_network_configuration.value16f0 = 10000;
	g_network_configuration.real16f4 = 0.75f;
	g_network_configuration.real16f8 = 10.0f;
	g_network_configuration.flag16fc = false;
	g_network_configuration.value1700 = 350;
	g_network_configuration.value1704 = 600000;
	g_network_configuration.flag1708 = false;
	g_network_configuration.value170c = 600000;
	g_network_configuration.value1710 = 3600000;
	g_network_configuration.real1714 = 2.9999f;
	g_network_configuration.value1718 = 2;
	g_network_configuration.real171c = 0.5f;
	g_network_configuration.value1720 = 1048576;
	g_network_configuration.value1724 = 65536;
	g_network_configuration.value1728 = 600;
	g_network_configuration.value172c = 600;
}

// @retail 0x662e0
PRIVATE bool __stdcall network_configuration_initialize_defaults(s_online_file *file)
{
	network_configuration_set_defaults();
	return true;
}

s_online_file_definition const g_467140 =
{
	0,
	L"network_configuration.dat",
	0x23,
	0,
	network_configuration_initialize_defaults,
	0,
	0
};

s_online_file g_477058 =
{
	&g_467140,
	0,
	{ 0, 0, 0 },
	1,
	0,
	NONE,
	NONE,
	NONE,
	0,
	NONE,
	&g_network_configuration,
	sizeof(g_network_configuration),
	0
};

// @retail 0x662f0
void network_configuration_initialize()
{
	if (g_477058.definition && g_477058.definition->initialize_defaults)
	{
		bool success = g_477058.definition->initialize_defaults(&g_477058);

		if (success)
			g_477058.flags |= (1 << 1);
		else
			g_477058.flags &= ~(1 << 1);
		if (success)
			g_477058.flags |= (1 << 3);
		else
			g_477058.flags &= ~(1 << 3);
	}
}

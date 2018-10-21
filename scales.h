//
// scales.h
// ========
//
// This file is a list of all currently-recognized scales' vendor and product
// IDs.
//
// For example, the USB product 1446:6173 becomes {0x1446, 0x6173}
//
#include <stdint.h>

//
// **NSCALES** should be kept updated with the length of the list.
//
#define NSCALES 9

//
// What is the number of the weighing result to show, as the first result may be incorrect (from the previous weighing)
//
#define WEIGH_COUNT 2


int initScale();
double  runScale();
void closeScale();


#ifndef DATETIME_H
#define DATETIME_H

#include "Timezone.h"

void datetimeInit();
time_t datetimeGet();
String datetimeGetTimeStr();
String datetimeGetDateStr();

#endif
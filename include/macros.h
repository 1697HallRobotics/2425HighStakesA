#ifndef _MACROS_H_
#define _MACROS_H_

#include "api.h"

#define MACRO_SLOT0    1ul
#define MACRO_SLOT1    2ul
#define MACRO_SLOT2    4ul
#define MACRO_SLOT3    8ul
#define MACRO_SLOT4    16ul
#define MACRO_SLOT5    32ul
#define MACRO_SLOT6    64ul
#define MACRO_SLOT7    128ul
#define MACRO_SLOT8    256ul
#define MACRO_SLOT9    512ul
#define MACRO_SLOT10   1024ul
#define MACRO_SLOT11   2048ul
#define MACRO_SLOT12   4096ul
#define MACRO_SLOT13   8192ul
#define MACRO_SLOT14   16384ul
#define MACRO_SLOT15   32767ul

uint16_t RUNNING_MACROS = 0;

#define FLAG_MACRO_ON(macro) RUNNING_MACROS |= macro
#define FLAG_MACRO_OFF(macro) RUNNING_MACROS &= ~macro
#define MACRO_RUNNING(macro) RUNNING_MACROS & macro

#define REF_MACRO(name) macro_##name
#define MACRO_IMPLEMENTATION(name) REF_MACRO(name)(void *param)
#define DEFINE_MACRO(name) void MACRO_IMPLEMENTATION(name)
#define TRIGGER_MACRO(name) pros::rtos::Task macroTask__##name(REF_MACRO(name))

#endif
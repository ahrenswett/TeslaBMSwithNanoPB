/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.3-dev */

#ifndef PB_TESLABMS_SRC_TESLA_BMS_PB_H_INCLUDED
#define PB_TESLABMS_SRC_TESLA_BMS_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _TeslaBMS_Pack {
    int32_t id;
    pb_callback_t packName;
    int32_t numberOfModules;
    float currentVoltage;
    float averagePacktemp;
    pb_callback_t modules;
} TeslaBMS_Pack;

typedef struct _TeslaBMS_Pack_Module {
    pb_callback_t id;
    float moduleVoltage;
    float moduleTemp;
    float lowestCellVolt;
    float highestCellVolt;
    pb_callback_t cells;
} TeslaBMS_Pack_Module;

typedef struct _TeslaBMS_Pack_Module_Cell {
    int32_t cellId;
    float cellVolt;
    pb_callback_t balanceState;
} TeslaBMS_Pack_Module_Cell;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define TeslaBMS_Pack_init_default               {0, {{NULL}, NULL}, 0, 0, 0, {{NULL}, NULL}}
#define TeslaBMS_Pack_Module_init_default        {{{NULL}, NULL}, 0, 0, 0, 0, {{NULL}, NULL}}
#define TeslaBMS_Pack_Module_Cell_init_default   {0, 0, {{NULL}, NULL}}
#define TeslaBMS_Pack_init_zero                  {0, {{NULL}, NULL}, 0, 0, 0, {{NULL}, NULL}}
#define TeslaBMS_Pack_Module_init_zero           {{{NULL}, NULL}, 0, 0, 0, 0, {{NULL}, NULL}}
#define TeslaBMS_Pack_Module_Cell_init_zero      {0, 0, {{NULL}, NULL}}

/* Field tags (for use in manual encoding/decoding) */
#define TeslaBMS_Pack_id_tag                     1
#define TeslaBMS_Pack_packName_tag               2
#define TeslaBMS_Pack_numberOfModules_tag        3
#define TeslaBMS_Pack_currentVoltage_tag         4
#define TeslaBMS_Pack_averagePacktemp_tag        5
#define TeslaBMS_Pack_modules_tag                6
#define TeslaBMS_Pack_Module_id_tag              1
#define TeslaBMS_Pack_Module_moduleVoltage_tag   2
#define TeslaBMS_Pack_Module_moduleTemp_tag      3
#define TeslaBMS_Pack_Module_lowestCellVolt_tag  4
#define TeslaBMS_Pack_Module_highestCellVolt_tag 5
#define TeslaBMS_Pack_Module_cells_tag           6
#define TeslaBMS_Pack_Module_Cell_cellId_tag     1
#define TeslaBMS_Pack_Module_Cell_cellVolt_tag   2
#define TeslaBMS_Pack_Module_Cell_balanceState_tag 3

/* Struct field encoding specification for nanopb */
#define TeslaBMS_Pack_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, INT32,    id,                1) \
X(a, CALLBACK, OPTIONAL, STRING,   packName,          2) \
X(a, STATIC,   REQUIRED, INT32,    numberOfModules,   3) \
X(a, STATIC,   REQUIRED, FLOAT,    currentVoltage,    4) \
X(a, STATIC,   REQUIRED, FLOAT,    averagePacktemp,   5) \
X(a, CALLBACK, REPEATED, MESSAGE,  modules,           6)
#define TeslaBMS_Pack_CALLBACK pb_default_field_callback
#define TeslaBMS_Pack_DEFAULT NULL
#define TeslaBMS_Pack_modules_MSGTYPE TeslaBMS_Pack_Module

#define TeslaBMS_Pack_Module_FIELDLIST(X, a) \
X(a, CALLBACK, REQUIRED, STRING,   id,                1) \
X(a, STATIC,   REQUIRED, FLOAT,    moduleVoltage,     2) \
X(a, STATIC,   REQUIRED, FLOAT,    moduleTemp,        3) \
X(a, STATIC,   REQUIRED, FLOAT,    lowestCellVolt,    4) \
X(a, STATIC,   REQUIRED, FLOAT,    highestCellVolt,   5) \
X(a, CALLBACK, REPEATED, MESSAGE,  cells,             6)
#define TeslaBMS_Pack_Module_CALLBACK pb_default_field_callback
#define TeslaBMS_Pack_Module_DEFAULT NULL
#define TeslaBMS_Pack_Module_cells_MSGTYPE TeslaBMS_Pack_Module_Cell

#define TeslaBMS_Pack_Module_Cell_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, INT32,    cellId,            1) \
X(a, STATIC,   REQUIRED, FLOAT,    cellVolt,          2) \
X(a, CALLBACK, REQUIRED, STRING,   balanceState,      3)
#define TeslaBMS_Pack_Module_Cell_CALLBACK pb_default_field_callback
#define TeslaBMS_Pack_Module_Cell_DEFAULT NULL

extern const pb_msgdesc_t TeslaBMS_Pack_msg;
extern const pb_msgdesc_t TeslaBMS_Pack_Module_msg;
extern const pb_msgdesc_t TeslaBMS_Pack_Module_Cell_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define TeslaBMS_Pack_fields &TeslaBMS_Pack_msg
#define TeslaBMS_Pack_Module_fields &TeslaBMS_Pack_Module_msg
#define TeslaBMS_Pack_Module_Cell_fields &TeslaBMS_Pack_Module_Cell_msg

/* Maximum encoded size of messages (where known) */
/* TeslaBMS_Pack_size depends on runtime parameters */
/* TeslaBMS_Pack_Module_size depends on runtime parameters */
/* TeslaBMS_Pack_Module_Cell_size depends on runtime parameters */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

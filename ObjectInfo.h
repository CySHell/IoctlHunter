#pragma once

#include "HandleInfo.h"


typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

//0x78 bytes (sizeof)
typedef struct _OBJECT_TYPE_INITIALIZER
{
    USHORT Length;                                                          //0x0
    union
    {
        USHORT ObjectTypeFlags;                                             //0x2
        struct
        {
            UCHAR CaseInsensitive : 1;                                        //0x2
            UCHAR UnnamedObjectsOnly : 1;                                     //0x2
            UCHAR UseDefaultObject : 1;                                       //0x2
            UCHAR SecurityRequired : 1;                                       //0x2
            UCHAR MaintainHandleCount : 1;                                    //0x2
            UCHAR MaintainTypeList : 1;                                       //0x2
            UCHAR SupportsObjectCallbacks : 1;                                //0x2
            UCHAR CacheAligned : 1;                                           //0x2
            UCHAR UseExtendedParameters : 1;                                  //0x3
            UCHAR Reserved : 7;                                               //0x3
        };
    };
    ULONG ObjectTypeCode;                                                   //0x4
    ULONG InvalidAttributes;                                                //0x8
    struct _GENERIC_MAPPING GenericMapping;                                 //0xc
    ULONG ValidAccessMask;                                                  //0x1c
    ULONG RetainAccess;                                                     //0x20
    enum _POOL_TYPE PoolType;                                               //0x24
    ULONG DefaultPagedPoolCharge;                                           //0x28
    ULONG DefaultNonPagedPoolCharge;                                        //0x2c
    VOID(*DumpProcedure)(VOID* arg1, struct _OBJECT_DUMP_CONTROL* arg2);   //0x30
    LONG(*OpenProcedure)(enum _OB_OPEN_REASON arg1, CHAR arg2, struct _EPROCESS* arg3, VOID* arg4, ULONG* arg5, ULONG arg6); //0x38
    VOID(*CloseProcedure)(struct _EPROCESS* arg1, VOID* arg2, ULONGLONG arg3, ULONGLONG arg4); //0x40
    VOID(*DeleteProcedure)(VOID* arg1);                                    //0x48
    union
    {
        LONG(*ParseProcedure)(VOID* arg1, VOID* arg2, struct _ACCESS_STATE* arg3, CHAR arg4, ULONG arg5, struct _UNICODE_STRING* arg6, struct _UNICODE_STRING* arg7, VOID* arg8, struct _SECURITY_QUALITY_OF_SERVICE* arg9, VOID** arg10); //0x50
        LONG(*ParseProcedureEx)(VOID* arg1, VOID* arg2, struct _ACCESS_STATE* arg3, CHAR arg4, ULONG arg5, struct _UNICODE_STRING* arg6, struct _UNICODE_STRING* arg7, VOID* arg8, struct _SECURITY_QUALITY_OF_SERVICE* arg9, struct _OB_EXTENDED_PARSE_PARAMETERS* arg10, VOID** arg11); //0x50
    };
    LONG(*SecurityProcedure)(VOID* arg1, enum _SECURITY_OPERATION_CODE arg2, ULONG* arg3, VOID* arg4, ULONG* arg5, VOID** arg6, enum _POOL_TYPE arg7, struct _GENERIC_MAPPING* arg8, CHAR arg9); //0x58
    LONG(*QueryNameProcedure)(VOID* arg1, UCHAR arg2, struct _OBJECT_NAME_INFORMATION* arg3, ULONG arg4, ULONG* arg5, CHAR arg6); //0x60
    UCHAR(*OkayToCloseProcedure)(struct _EPROCESS* arg1, VOID* arg2, VOID* arg3, CHAR arg4); //0x68
    ULONG WaitObjectFlagMask;                                               //0x70
    USHORT WaitObjectFlagOffset;                                            //0x74
    USHORT WaitObjectPointerOffset;                                         //0x76
} OBJECT_TYPE_INITIALIZER, *POBJECT_TYPE_INITIALIZER;

//0xd8 bytes (sizeof)
typedef struct _OBJECT_TYPE
{
    struct _LIST_ENTRY TypeList;                                            //0x0
    struct _UNICODE_STRING Name;                                            //0x10
    VOID* DefaultObject;                                                    //0x20
    UCHAR Index;                                                            //0x28
    ULONG TotalNumberOfObjects;                                             //0x2c
    ULONG TotalNumberOfHandles;                                             //0x30
    ULONG HighWaterNumberOfObjects;                                         //0x34
    ULONG HighWaterNumberOfHandles;                                         //0x38
    struct _OBJECT_TYPE_INITIALIZER TypeInfo;                               //0x40
    struct _EX_PUSH_LOCK TypeLock;                                          //0xb8
    ULONG Key;                                                              //0xc0
    struct _LIST_ENTRY CallbackList;                                        //0xc8
} OBJECT_TYPE, *POBJECT_TYPE;

typedef struct _OBJECT_HEADER
{
    LONGLONG PointerCount;                                                  //0x0
    union
    {
        LONGLONG HandleCount;                                               //0x8
        VOID* NextToFree;                                                   //0x8
    };
    struct _EX_PUSH_LOCK Lock;                                              //0x10
    UCHAR TypeIndex;                                                        //0x18
    union
    {
        UCHAR TraceFlags;                                                   //0x19
        struct
        {
            UCHAR DbgRefTrace : 1;                                            //0x19
            UCHAR DbgTracePermanent : 1;                                      //0x19
        };
    };
    UCHAR InfoMask;                                                         //0x1a
    union
    {
        UCHAR Flags;                                                        //0x1b
        struct
        {
            UCHAR NewObject : 1;                                              //0x1b
            UCHAR KernelObject : 1;                                           //0x1b
            UCHAR KernelOnlyAccess : 1;                                       //0x1b
            UCHAR ExclusiveObject : 1;                                        //0x1b
            UCHAR PermanentObject : 1;                                        //0x1b
            UCHAR DefaultSecurityQuota : 1;                                   //0x1b
            UCHAR SingleHandleEntry : 1;                                      //0x1b
            UCHAR DeletedInline : 1;                                          //0x1b
        };
    };
    ULONG Reserved;                                                         //0x1c
    union
    {
        struct _OBJECT_CREATE_INFORMATION* ObjectCreateInfo;                //0x20
        VOID* QuotaBlockCharged;                                            //0x20
    };
    VOID* SecurityDescriptor;                                               //0x28                                                   
}OBJECT_HEADER, * POBJECT_HEADER;


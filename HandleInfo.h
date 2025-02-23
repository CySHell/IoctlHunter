#pragma once

#include <DbgEng.h>

/////////////////////////////////////////////////////////////////////////////////
//	Internal windows structures

//0x8 bytes (sizeof)
struct _EX_PUSH_LOCK
{
    union
    {
        struct
        {
            ULONGLONG Locked : 1;                                             //0x0
            ULONGLONG Waiting : 1;                                            //0x0
            ULONGLONG Waking : 1;                                             //0x0
            ULONGLONG MultipleShared : 1;                                     //0x0
            ULONGLONG Shared : 60;                                            //0x0
        };
        ULONGLONG Value;                                                    //0x0
        VOID* Ptr;                                                          //0x0
    };
};


//0x8 bytes (sizeof)
struct _EXHANDLE
{
    union
    {
        struct
        {
            ULONG TagBits : 2;                                                //0x0
            ULONG Index : 30;                                                 //0x0
        };
        VOID* GenericHandleOverlay;                                         //0x0
        ULONGLONG Value;                                                    //0x0
    };
};

//0x8 bytes (sizeof)
struct _HANDLE_TABLE_ENTRY_INFO
{
    ULONG AuditMask;                                                        //0x0
    ULONG MaxRelativeAccessMask;                                            //0x4
};


//0x10 bytes (sizeof)
typedef struct _HANDLE_TABLE_ENTRY
{                                                
    ULONGLONG Unlocked : 1;                                                   //0x0
    ULONGLONG RefCnt : 16;                                                    //0x0
    ULONGLONG Attributes : 3;
    ULONGLONG ObjectPointerBits : 44;                                     //0x0
    ULONG GrantedAccessBits : 25;                                             //0x8
    ULONG NoRightsUpgrade : 1;                                                //0x8
    ULONG Spare1 : 6;                                                     //0x8
}HANDLE_TABLE_ENTRY, *PHANDLE_TABLE_ENTRY;


//0x40 bytes (sizeof)
struct _HANDLE_TABLE_FREE_LIST
{
    struct _EX_PUSH_LOCK FreeListLock;                                      //0x0
    HANDLE_TABLE_ENTRY* FirstFreeHandleEntry;                        //0x8
    HANDLE_TABLE_ENTRY* LastFreeHandleEntry;                         //0x10
    LONG HandleCount;                                                       //0x18
    ULONG HighWaterMark;                                                    //0x1c
};

//0x80 bytes (sizeof)
typedef struct _HANDLE_TABLE
{
    ULONG NextHandleNeedingPool;                                            //0x0
    LONG ExtraInfoPages;                                                    //0x4
    volatile ULONGLONG TableCode;                                           //0x8
    struct _EPROCESS* QuotaProcess;                                         //0x10
    struct _LIST_ENTRY HandleTableList;                                     //0x18
    ULONG UniqueProcessId;                                                  //0x28
    union
    {
        ULONG Flags;                                                        //0x2c
        struct
        {
            UCHAR StrictFIFO : 1;                                             //0x2c
            UCHAR EnableHandleExceptions : 1;                                 //0x2c
            UCHAR Rundown : 1;                                                //0x2c
            UCHAR Duplicated : 1;                                             //0x2c
            UCHAR RaiseUMExceptionOnInvalidHandleClose : 1;                   //0x2c
        };
    };
    struct _EX_PUSH_LOCK HandleContentionEvent;                             //0x30
    struct _EX_PUSH_LOCK HandleTableLock;                                   //0x38
    union
    {
        struct _HANDLE_TABLE_FREE_LIST FreeLists[1];                        //0x40
        struct
        {
            UCHAR ActualEntry[32];                                          //0x40
            struct _HANDLE_TRACE_DEBUG_INFO* DebugInfo;                     //0x60
        };
    };
}HANDLE_TABLE, *PHANDLE_TABLE;

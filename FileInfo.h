#pragma once

#include "ObjectInfo.h"


//0x18 bytes (sizeof)
struct _DISPATCHER_HEADER
{
    union
    {
        volatile LONG Lock;                                                 //0x0
        LONG LockNV;                                                        //0x0
        struct
        {
            UCHAR Type;                                                     //0x0
            UCHAR Signalling;                                               //0x1
            UCHAR Size;                                                     //0x2
            UCHAR Reserved1;                                                //0x3
        };
        struct
        {
            UCHAR TimerType;                                                //0x0
            union
            {
                UCHAR TimerControlFlags;                                    //0x1
                struct
                {
                    UCHAR Absolute : 1;                                       //0x1
                    UCHAR Wake : 1;                                           //0x1
                    UCHAR EncodedTolerableDelay : 6;                          //0x1
                };
            };
            UCHAR Hand;                                                     //0x2
            union
            {
                UCHAR TimerMiscFlags;                                       //0x3
                struct
                {
                    UCHAR Index : 6;                                          //0x3
                    UCHAR Inserted : 1;                                       //0x3
                    volatile UCHAR Expired : 1;                               //0x3
                };
            };
        };
        struct
        {
            UCHAR Timer2Type;                                               //0x0
            union
            {
                UCHAR Timer2Flags;                                          //0x1
                struct
                {
                    UCHAR Timer2Inserted : 1;                                 //0x1
                    UCHAR Timer2Expiring : 1;                                 //0x1
                    UCHAR Timer2CancelPending : 1;                            //0x1
                    UCHAR Timer2SetPending : 1;                               //0x1
                    UCHAR Timer2Running : 1;                                  //0x1
                    UCHAR Timer2Disabled : 1;                                 //0x1
                    UCHAR Timer2ReservedFlags : 2;                            //0x1
                };
            };
            UCHAR Timer2ComponentId;                                        //0x2
            UCHAR Timer2RelativeId;                                         //0x3
        };
        struct
        {
            UCHAR QueueType;                                                //0x0
            union
            {
                UCHAR QueueControlFlags;                                    //0x1
                struct
                {
                    UCHAR Abandoned : 1;                                      //0x1
                    UCHAR DisableIncrement : 1;                               //0x1
                    UCHAR QueueReservedControlFlags : 6;                      //0x1
                };
            };
            UCHAR QueueSize;                                                //0x2
            UCHAR QueueReserved;                                            //0x3
        };
        struct
        {
            UCHAR ThreadType;                                               //0x0
            UCHAR ThreadReserved;                                           //0x1
            union
            {
                UCHAR ThreadControlFlags;                                   //0x2
                struct
                {
                    UCHAR CycleProfiling : 1;                                 //0x2
                    UCHAR CounterProfiling : 1;                               //0x2
                    UCHAR GroupScheduling : 1;                                //0x2
                    UCHAR AffinitySet : 1;                                    //0x2
                    UCHAR Tagged : 1;                                         //0x2
                    UCHAR EnergyProfiling : 1;                                //0x2
                    UCHAR SchedulerAssist : 1;                                //0x2
                    UCHAR ThreadReservedControlFlags : 1;                     //0x2
                };
            };
            union
            {
                UCHAR DebugActive;                                          //0x3
                struct
                {
                    UCHAR ActiveDR7 : 1;                                      //0x3
                    UCHAR Instrumented : 1;                                   //0x3
                    UCHAR Minimal : 1;                                        //0x3
                    UCHAR Reserved4 : 2;                                      //0x3
                    UCHAR AltSyscall : 1;                                     //0x3
                    UCHAR Emulation : 1;                                      //0x3
                    UCHAR Reserved5 : 1;                                      //0x3
                };
            };
        };
        struct
        {
            UCHAR MutantType;                                               //0x0
            UCHAR MutantSize;                                               //0x1
            UCHAR DpcActive;                                                //0x2
            UCHAR MutantReserved;                                           //0x3
        };
    };
    LONG SignalState;                                                       //0x4
    struct _LIST_ENTRY WaitListHead;                                        //0x8
};

//0x18 bytes (sizeof)
struct _KEVENT
{
    struct _DISPATCHER_HEADER Header;                                       //0x0
};

struct _DEVICE_OBJECT;

//0xd8 bytes (sizeof)
typedef struct _FILE_OBJECT
{
    SHORT Type;                                                             //0x0
    SHORT Size;                                                             //0x2
    struct _DEVICE_OBJECT* DeviceObject;                                    //0x8
    struct _VPB* Vpb;                                                       //0x10
    VOID* FsContext;                                                        //0x18
    VOID* FsContext2;                                                       //0x20
    struct _SECTION_OBJECT_POINTERS* SectionObjectPointer;                  //0x28
    VOID* PrivateCacheMap;                                                  //0x30
    LONG FinalStatus;                                                       //0x38
    struct _FILE_OBJECT* RelatedFileObject;                                 //0x40
    UCHAR LockOperation;                                                    //0x48
    UCHAR DeletePending;                                                    //0x49
    UCHAR ReadAccess;                                                       //0x4a
    UCHAR WriteAccess;                                                      //0x4b
    UCHAR DeleteAccess;                                                     //0x4c
    UCHAR SharedRead;                                                       //0x4d
    UCHAR SharedWrite;                                                      //0x4e
    UCHAR SharedDelete;                                                     //0x4f
    ULONG Flags;                                                            //0x50
    struct _UNICODE_STRING FileName;                                        //0x58
    union _LARGE_INTEGER CurrentByteOffset;                                 //0x68
    ULONG Waiters;                                                          //0x70
    ULONG Busy;                                                             //0x74
    VOID* LastLock;                                                         //0x78
    struct _KEVENT Lock;                                                    //0x80
    struct _KEVENT Event;                                                   //0x98
    struct _IO_COMPLETION_CONTEXT* CompletionContext;                       //0xb0
    ULONGLONG IrpListLock;                                                  //0xb8
    struct _LIST_ENTRY IrpList;                                             //0xc0
    VOID* FileObjectExtension;                                              //0xd0
} FILE_OBJECT, *PFILE_OBJECT;
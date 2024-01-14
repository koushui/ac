#ifndef CALLBACKS_H
#define CALLBACKS_H
#include <ntifs.h>
#include <wdftypes.h>
#include <wdf.h>
#include "common.h"
#include "driver.h"

#define HANDLE_REPORT_PROCESS_NAME_MAX_LENGTH 64

typedef struct _OPEN_HANDLE_FAILURE_REPORT
{
        INT  report_code;
        INT  is_kernel_handle;
        LONG process_id;
        LONG thread_id;
        LONG access;
        CHAR process_name[HANDLE_REPORT_PROCESS_NAME_MAX_LENGTH];

} OPEN_HANDLE_FAILURE_REPORT, *POPEN_HANDLE_FAILURE_REPORT;

// handle access masks
// https://learn.microsoft.com/en-us/windows/win32/procthread/process-security-and-access-rights
#define PROCESS_CREATE_PROCESS            0x0080
#define PROCESS_TERMINATE                 0x0001
#define PROCESS_CREATE_THREAD             0x0002
//#define PROCESS_DUP_HANDLE                0x0040
#define PROCESS_QUERY_INFORMATION         0x0400
#define PROCESS_QUERY_LIMITED_INFORMATION 0x1000
#define PROCESS_SET_INFORMATION           0x0200
#define PROCESS_SET_QUOTA                 0x0100
#define PROCESS_SUSPEND_RESUME            0x0800
#define PROCESS_VM_OPERATION              0x0008
#define PROCESS_VM_READ                   0x0010
#define PROCESS_VM_WRITE                  0x0020

// https://www.sysnative.com/forums/threads/object-headers-handles-and-types.34987/
#define GET_OBJECT_HEADER_FROM_HANDLE(x) ((x << 4) | 0xffff000000000000)

static const uintptr_t EPROCESS_IMAGE_FILE_NAME_OFFSET = 0x5a8;
static const uintptr_t EPROCESS_HANDLE_TABLE_OFFSET    = 0x570;
static const uintptr_t EPROCESS_PLIST_ENTRY_OFFSET     = 0x448;

static UNICODE_STRING OBJECT_TYPE_PROCESS = RTL_CONSTANT_STRING(L"Process");
static UNICODE_STRING OBJECT_TYPE_THREAD  = RTL_CONSTANT_STRING(L"Thread");

#define DRIVER_PATH_LENGTH  0x100
#define SHA_256_HASH_LENGTH 32

typedef struct _DRIVER_LIST_ENTRY
{
        SINGLE_LIST_ENTRY list;
        PVOID             ImageBase;
        ULONG             ImageSize;
        BOOLEAN           hashed;
        CHAR              path[DRIVER_PATH_LENGTH];
        CHAR              text_hash[SHA_256_HASH_LENGTH];

} DRIVER_LIST_ENTRY, *PDRIVER_LIST_ENTRY;

NTSTATUS
InitialiseDriverList();

VOID NTAPI
ExUnlockHandleTableEntry(IN PHANDLE_TABLE HandleTable, IN PHANDLE_TABLE_ENTRY HandleTableEntry);

VOID
ObPostOpCallbackRoutine(_In_ PVOID                          RegistrationContext,
                        _In_ POB_POST_OPERATION_INFORMATION OperationInformation);

OB_PREOP_CALLBACK_STATUS
ObPreOpCallbackRoutine(_In_ PVOID                         RegistrationContext,
                       _In_ POB_PRE_OPERATION_INFORMATION OperationInformation);

NTSTATUS
EnumerateProcessHandles(_In_ PPROCESS_LIST_ENTRY ProcessListEntry, _In_opt_ PVOID Context);

NTSTATUS
InitialiseThreadList();

NTSTATUS
InitialiseProcessList();

VOID
ThreadCreateNotifyRoutine(_In_ HANDLE ProcessId, _In_ HANDLE ThreadId, _In_ BOOLEAN Create);

VOID
ProcessCreateNotifyRoutine(_In_ HANDLE ParentId, _In_ HANDLE ProcessId, _In_ BOOLEAN Create);

VOID
CleanupThreadListOnDriverUnload();

VOID
FindThreadListEntryByThreadAddress(_In_ PKTHREAD Thread, _Inout_ PTHREAD_LIST_ENTRY* Entry);

VOID
FindProcessListEntryByProcess(_In_ PKPROCESS Process, _Inout_ PPROCESS_LIST_ENTRY* Entry);

VOID
EnumerateThreadListWithCallbackRoutine(_In_ PVOID CallbackRoutine, _In_opt_ PVOID Context);

VOID
EnumerateProcessListWithCallbackRoutine(_In_ PVOID CallbackRoutine, _In_opt_ PVOID Context);

VOID
FindDriverEntryByBaseAddress(_In_ PVOID ImageBase, _Out_ PDRIVER_LIST_ENTRY* Entry);

VOID
CleanupProcessListOnDriverUnload();

VOID
CleanupDriverListOnDriverUnload();

VOID
ImageLoadNotifyRoutineCallback(_In_opt_ PUNICODE_STRING FullImageName,
                               _In_ HANDLE              ProcessId,
                               _In_ PIMAGE_INFO         ImageInfo);

NTSTATUS
InitialiseTimerObject(_Out_ PTIMER_OBJECT Timer);

VOID
CleanupDriverTimerObjects(_Out_ PTIMER_OBJECT Timer);

VOID
UnregisterProcessCreateNotifyRoutine();

VOID
UnregisterImageLoadNotifyRoutine();

VOID
UnregisterThreadCreateNotifyRoutine();

#endif

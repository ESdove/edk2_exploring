#include <Uefi.h>
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/DevicePathLib.h>
#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Guid/HobList.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/MemoryAllocationHob.h>
#include <Protocol/LoadedImage.h>
#include "../../MdeModulePkg/Core/Dxe/Hand/Handle.h"

extern UINTN               Argc;
extern CHAR16              **Argv;
extern EFI_SHELL_PROTOCOL  *mShellProtocol;
extern EFI_SHELL_PROTOCOL *
GetShellProtocol (
  VOID
  );

extern EFI_STATUS
GetArg (
  VOID
  );

EFI_LOADED_IMAGE_PROTOCOL  *mDxeCoreLoadedImage;
LIST_ENTRY                 *mDxeCoreHandleList;
LIST_ENTRY                 *mDxeCoreProtocolDatabase;

VOID
ToolInfo (
  VOID
  )
{
  Print (L"Dump ProtocolInterface Tool V1.0 \nAuthor:ElderChen\n");
  Print (L"For Tool Usage: --help Parameter\n");
}

VOID
PrintUsage (
  VOID
  )
{
  Print (L"-All      Dump All ProtocolInterfaces on Each Handle\n");
}

VOID
GetDxeCoreProtocolDatabase (
  IN IHANDLE  *IHandle
  )
{
  LIST_ENTRY      *ListEntry;
  IHANDLE         *Handle;
  PROTOCOL_ENTRY  *IProtocolEntry;
  PROTOCOL_ENTRY  *ProtocolEntry;

  IProtocolEntry = NULL;
  ListEntry      = &IHandle->AllHandles;
  for (ListEntry = ListEntry->ForwardLink;
       ListEntry != &IHandle->AllHandles;
       ListEntry = ListEntry->ForwardLink)
  {
    Handle = BASE_CR (ListEntry, IHANDLE, AllHandles);
    if (Handle->Signature == EFI_HANDLE_SIGNATURE) {
      LIST_ENTRY          *ProtocolListEntry;
      PROTOCOL_INTERFACE  *ProtocolInterface;

      ProtocolListEntry = &Handle->Protocols;
      for (ProtocolListEntry = ProtocolListEntry->ForwardLink;
           ProtocolListEntry != &Handle->Protocols;
           ProtocolListEntry = ProtocolListEntry->ForwardLink)
      {
        ProtocolInterface = BASE_CR (ProtocolListEntry, PROTOCOL_INTERFACE, Link);
        if (ProtocolInterface->Signature == PROTOCOL_INTERFACE_SIGNATURE) {
          IProtocolEntry = ProtocolInterface->Protocol;
        }

        if (IProtocolEntry != NULL) {
          break;
        }
      }

      if (IProtocolEntry != NULL) {
        break;
      }
    }
  }

  if (IProtocolEntry == NULL) {
    return;
  }

  ListEntry = &IProtocolEntry->AllEntries;
  for (ListEntry = ListEntry->ForwardLink;
       ListEntry != &IProtocolEntry->AllEntries;
       ListEntry = ListEntry->ForwardLink)
  {
    ProtocolEntry = BASE_CR (ListEntry, PROTOCOL_ENTRY, AllEntries);
    if (ProtocolEntry->Signature != PROTOCOL_ENTRY_SIGNATURE) {
      mDxeCoreProtocolDatabase = ListEntry;
      Print (L"mDxeCoreProtocolDatabase = 0X%X\n", mDxeCoreProtocolDatabase);
      break;
    }
  }

  return;
}

VOID
GetDxeCoreHandleList (
  IN IHANDLE  *IHandle
  )
{
  LIST_ENTRY  *ListEntry;
  IHANDLE     *Handle;

  ListEntry = &IHandle->AllHandles;
  for (ListEntry = ListEntry->ForwardLink;
       ListEntry != &IHandle->AllHandles;
       ListEntry = ListEntry->ForwardLink)
  {
    Handle = BASE_CR (ListEntry, IHANDLE, AllHandles);
    if (Handle->Signature != EFI_HANDLE_SIGNATURE) {
      mDxeCoreHandleList = ListEntry;
      Print (L"\nmDxeCoreHandList = 0x%X\n", mDxeCoreHandleList);
      break;
    }
  }

  return;
}

VOID
DumpProtocolInterfacesOnHandle (
  IN IHANDLE  *IHandle
  )
{
  LIST_ENTRY          *ListEntry;
  PROTOCOL_INTERFACE  *ProtocolInterface;

  // CHAR16               *PathStr;

  ListEntry = &IHandle->Protocols;
  for (ListEntry = ListEntry->ForwardLink;
       ListEntry != &IHandle->Protocols;
       ListEntry = ListEntry->ForwardLink)
  {
    ProtocolInterface = CR (ListEntry, PROTOCOL_INTERFACE, Link, PROTOCOL_INTERFACE_SIGNATURE);
    Print (L"    PROTOCOL_INTERFACE = 0X%X\n", ProtocolInterface);
    Print (L"    Signature = 0X%X\n", ProtocolInterface->Signature);
    Print (
           L"    Link ForwardLink = 0X%X BackLink = 0X%X\n",
           ProtocolInterface->Link.ForwardLink,
           ProtocolInterface->Link.BackLink
           );
    Print (L"    Handle = 0X%X\n", ProtocolInterface->Handle);
    Print (
           L"    ByProtocol ForwardLink = 0X%X BackLink = 0X%X\n",
           ProtocolInterface->ByProtocol.ForwardLink,
           ProtocolInterface->ByProtocol.BackLink
           );
    Print (L"    PROTOCOL_ENTRY = 0X%X\n", ProtocolInterface->Protocol);
    Print (L"    Interface = 0X%X\n", ProtocolInterface->Interface);
    Print (
           L"    OpenList ForwardLink = 0X%X BackLink = 0X%X\n",
           ProtocolInterface->OpenList.ForwardLink,
           ProtocolInterface->OpenList.BackLink
           );
    Print (L"    OpenListCount = 0X%X\n\n", ProtocolInterface->OpenListCount);
  }
}

VOID
DumpHandleList (
  VOID
  )
{
  LIST_ENTRY  *ListEntry;
  IHANDLE     *Handle;
  UINTN       Index;

  ListEntry = mDxeCoreHandleList;
  for (ListEntry = ListEntry->ForwardLink, Index = 0;
       ListEntry != mDxeCoreHandleList;
       ListEntry = ListEntry->ForwardLink, Index++)
  {
    // Print(L"###########################");
    // Print(L"#####Protocol Database#####");
    // Print(L"###########################");
    // Print(L"Protocol Database = 0X%X\n",ListEntry);
    Handle = CR (ListEntry, IHANDLE, AllHandles, EFI_HANDLE_SIGNATURE);
    Print (L"%-4dHandle - BA = 0X%X\n", Index + 1, Handle);
    Print (
           L"    Signature = 0X%X - String = %c%c%c%c\n",
           Handle->Signature,
           (char)(Handle->Signature),
           (char)(Handle->Signature >> 8),
           (char)(Handle->Signature >> 16),
           (char)(Handle->Signature >> 24)
           );
    Print (
           L"    AllHandles ForwardLink = 0X%X BackLink = 0X%X\n",
           Handle->AllHandles.ForwardLink,
           Handle->AllHandles.BackLink
           );
    Print (
           L"    Protocol   ForWardLink = 0X%X BackLink = 0X%X\n",
           Handle->Protocols.ForwardLink,
           Handle->Protocols.BackLink
           );
    Print (L"    LocateRequest = %d\n", Handle->LocateRequest);
    Print (L"    Key = %d\n", Handle->Key);
    DumpProtocolInterfacesOnHandle (Handle);
  }

  return;
}

EFI_STATUS
EFIAPI
DumpProtocolInterface (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  GetShellProtocol ();

  GetArg ();
  // CpuBreakpoint();

  if (Argc == 1) {
    ToolInfo ();
    return EFI_SUCCESS;
  } else if ((Argc == 2) && (StrCmp (Argv[1], L"--help") == 0)) {
    PrintUsage ();
  } else if ((Argc == 2) && (StrCmp (Argv[1], L"-All") == 0)) {
    GetDxeCoreHandleList ((IHANDLE *)ImageHandle);
    GetDxeCoreProtocolDatabase ((IHANDLE *)ImageHandle);
    DumpHandleList ();
  } else {
    Print (L"Invalid command line option(s)\n");
  }

  return EFI_SUCCESS;
}

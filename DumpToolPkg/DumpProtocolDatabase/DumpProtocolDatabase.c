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


extern UINTN Argc;
extern CHAR16 **Argv;
extern EFI_SHELL_PROTOCOL *mShellProtocol;
extern EFI_SHELL_PROTOCOL *GetShellProtocol(VOID);
extern EFI_STATUS GetArg(VOID);
EFI_LOADED_IMAGE_PROTOCOL  *mDxeCoreLoadedImage;
LIST_ENTRY      *mDxeCoreHandleList;
LIST_ENTRY      *mDxeCoreProtocolDatabase;


VOID ToolInfo(VOID)
{
  Print(L"Dump ProtocolDatabase Tool V1.0 \nAuthor:ElderChen\n");
  Print(L"For Tool Usage : --help Parameter\n");
}

VOID PrintUsage(VOID)
{
  Print(L"-All      Dump All Protocols from Database\n");
  Print(L"-Detail  Dump PROTOCOL _ENTRY instances including Protocol Database\n");
}


VOID
GetDxeCoreProtocolDatabase(
  IN IHANDLE    *IHandle
  )
{
  LIST_ENTRY      *ListEntry;
  IHANDLE         *Handle;
  PROTOCOL_ENTRY  *IProtocolEntry;
  PROTOCOL_ENTRY  *ProtocolEntry;

  IProtocolEntry = NULL;
  ListEntry = &IHandle->AllHandles;
  for (ListEntry = ListEntry->ForwardLink;
       ListEntry != &IHandle->AllHandles;
       ListEntry = ListEntry->ForwardLink)
   {
    Handle = BASE_CR(ListEntry, IHANDLE, AllHandles);
    if (Handle->Signature == EFI_HANDLE_SIGNATURE) 
    {
      LIST_ENTRY           *ProtocolListEntry;
      PROTOCOL_INTERFACE   *ProtocolInterface;

      ProtocolListEntry = &Handle->Protocols;
      for (ProtocolListEntry = ProtocolListEntry->ForwardLink;
           ProtocolListEntry != &Handle->Protocols;
           ProtocolListEntry = ProtocolListEntry->ForwardLink) 
      {
        ProtocolInterface = BASE_CR(ProtocolListEntry, PROTOCOL_INTERFACE, Link);
        if (ProtocolInterface->Signature == PROTOCOL_INTERFACE_SIGNATURE) 
        {
          IProtocolEntry = ProtocolInterface->Protocol;
        }
        if (IProtocolEntry != NULL) 
        {
          break;
        }
      }
      if (IProtocolEntry != NULL) 
      {
        break;
      }
    }
  }

  if (IProtocolEntry == NULL) 
  {
    return;
  }

  ListEntry = &IProtocolEntry->AllEntries;
  for (ListEntry = ListEntry->ForwardLink;
       ListEntry != &IProtocolEntry->AllEntries;
       ListEntry = ListEntry->ForwardLink)
    {
      ProtocolEntry = BASE_CR(ListEntry, PROTOCOL_ENTRY, AllEntries);
      if (ProtocolEntry->Signature != PROTOCOL_ENTRY_SIGNATURE) 
     {  
       mDxeCoreProtocolDatabase = ListEntry;
       Print(L"mDxeCoreProtocolDatabase = 0x%X\n\n",mDxeCoreProtocolDatabase);
       break;
     }
    }

  return;
}

VOID
GetDxeCoreHandleList(
  IN IHANDLE    *IHandle
  )
{
  LIST_ENTRY      *ListEntry;
  IHANDLE         *Handle;

  ListEntry = &IHandle->AllHandles;
  for (ListEntry = ListEntry->ForwardLink;
       ListEntry != &IHandle->AllHandles;
       ListEntry = ListEntry->ForwardLink) {
    Handle = BASE_CR(ListEntry, IHANDLE, AllHandles);
    if (Handle->Signature != EFI_HANDLE_SIGNATURE) {
    
      mDxeCoreHandleList = ListEntry;
      Print(L"mDxeCoreHandList = 0x%X\n",mDxeCoreHandleList);
      break;
    }
  }

  return;
}

VOID
DumpProtocolInterfacesOnHandle(
  IN IHANDLE    *IHandle
  )
{
  LIST_ENTRY           *ListEntry;
  PROTOCOL_INTERFACE   *ProtocolInterface;

  ListEntry = &IHandle->Protocols;
  for (ListEntry = ListEntry->ForwardLink;
       ListEntry != &IHandle->Protocols;
       ListEntry = ListEntry->ForwardLink)
     {
        ProtocolInterface = CR(ListEntry, PROTOCOL_INTERFACE, Link, PROTOCOL_INTERFACE_SIGNATURE);
        Print(L"    PROTOCOL_INTERFACE = 0x%X\n",ProtocolInterface);
        Print(L"    Signature - 0x%X - String - %c%c%c%c\n",
                    ProtocolInterface->Signature,
                    (char)ProtocolInterface->Signature,
                    (char)ProtocolInterface->Signature >> 8,
                    (char)ProtocolInterface->Signature >> 16,
                    (char)ProtocolInterface->Signature >> 24);
        Print(L"    Link ForwardLink = 0x%X BackLink = 0x%X\n",
                    ProtocolInterface->Link.ForwardLink,
                    ProtocolInterface->Link.BackLink);
        Print(L"    Handle = 0x%X\n",ProtocolInterface->Handle);
        Print(L"    ByProtocol ForwardLink = 0x%X BackLink = 0x%X\n",
                    ProtocolInterface->ByProtocol.ForwardLink,
                    ProtocolInterface->ByProtocol.BackLink);
        Print(L"    PROTOCOL_ENTRY = 0x%X\n",ProtocolInterface->Protocol);
        Print(L"    Interface = 0x%X\n",ProtocolInterface->Interface);
        Print(L"    OpenList ForwardLink = 0x%X BackLink = 0x%X\n",
                    ProtocolInterface->OpenList.ForwardLink,
                    ProtocolInterface->OpenList.BackLink);
        Print(L"    OpenListCount = 0x%X\n\n",ProtocolInterface->OpenListCount);        
      }
}

VOID
DumpHandleList(
  VOID
  )
{
  LIST_ENTRY      *ListEntry;
  IHANDLE         *Handle;
  UINTN           Index;

  ListEntry = mDxeCoreHandleList;
  for (ListEntry = ListEntry->ForwardLink,Index = 0;
       ListEntry != mDxeCoreHandleList;
       ListEntry = ListEntry->ForwardLink,Index++) 
    {
      Handle = CR(ListEntry, IHANDLE, AllHandles, EFI_HANDLE_SIGNATURE);
      Print(L"%-4dHandle - BA = 0x%X\n", Index + 1,Handle);
      Print(L"    Signature = 0x%X - String = %c%c%c%c\n",
                  Handle->Signature,
                  (char)Handle->Signature,
                  (char)Handle->Signature >> 8,
                  (char)Handle->Signature >> 16,
                  (char)Handle->Signature >> 24);
      Print(L"    AllHandles ForwardLink = 0x%X BackLink = 0x%X\n",
                  Handle->AllHandles.ForwardLink,
                  Handle->AllHandles.BackLink);
      Print(L"    Protocol   ForWardLink = 0x%X BackLink = 0x%X\n",
                  Handle->Protocols.ForwardLink,
                  Handle->Protocols.BackLink);
      Print(L"    LocateRequest = %d\n",Handle->LocateRequest);
      Print(L"    Key = %d\n",Handle->Key); 
      DumpProtocolInterfacesOnHandle(Handle);
  }

  return;
}


EFI_STATUS
EFIAPI
DumpProtocolDatabase
 (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  LIST_ENTRY            *ListEntry;
  PROTOCOL_ENTRY        *ProtocolEntry;
  UINTN                 Index;

  GetShellProtocol();
    
  GetArg();


  if (Argc == 1)
  {
    ToolInfo();
    return EFI_SUCCESS;
  } 

  else if(Argc == 2 && StrCmp(Argv[1],L"--help") == 0)
  {
    PrintUsage();
  }

  else if(Argc == 2 && StrCmp(Argv[1],L"-Detail") == 0)
  {
    GetDxeCoreHandleList((IHANDLE *)ImageHandle);
    GetDxeCoreProtocolDatabase((IHANDLE *)ImageHandle);
    //DumpHandleList();
    ListEntry = mDxeCoreProtocolDatabase;
    Print(L"+--------------------------------------------------------------+\n");
    Print(L"|          PROTOCOL_ENTRY sizeof(PROTOCOL_ENTRY) = %d          |\n",sizeof(PROTOCOL_ENTRY));
    Print(L"+--------------------------------------------------------------+\n\n");
    for(ListEntry = ListEntry->ForwardLink, Index = 0;
        ListEntry != mDxeCoreProtocolDatabase;
        ListEntry = ListEntry->ForwardLink, Index++)
    {    
      ProtocolEntry = BASE_CR(ListEntry,PROTOCOL_ENTRY,AllEntries);
      Print(L"%03d - PROTOCOL_ENTRY = 0x%X\n",Index + 1, ProtocolEntry);
      Print(L"  Signature  = 0x%X - String = %c%c%c%c\n",
            ProtocolEntry->Signature,
            (char)(ProtocolEntry->Signature),
            (char)(ProtocolEntry->Signature >> 8),
            (char)(ProtocolEntry->Signature >> 16),
            (char)(ProtocolEntry->Signature >> 24));
      Print(L"  Database   - ForwardLink = 0x%X BackLink = 0x%X\n",
            ProtocolEntry->AllEntries.ForwardLink,
            ProtocolEntry->AllEntries.BackLink);
      Print(L"  ProtocolID - %g\n",&(ProtocolEntry->ProtocolID));
      Print(L"  Interfaces - ForwardLink = 0x%X BackLink = 0x%X\n",
            ProtocolEntry->Protocols.ForwardLink,
            ProtocolEntry->Protocols.BackLink);
      Print(L"  Notify     - ForwardLink = 0x%X BackLink = 0x%X\n\n",
            ProtocolEntry->Notify.ForwardLink,
            ProtocolEntry->Notify.BackLink);
        
    }

  }
  else if(Argc == 2 && StrCmp(Argv[1],L"-All") == 0)
  {
    GetDxeCoreHandleList((IHANDLE *)ImageHandle);
    GetDxeCoreProtocolDatabase((IHANDLE *)ImageHandle);
    //DumpHandleList();
    ListEntry = mDxeCoreProtocolDatabase;
    Print(L"+---------------------------------------------------+\n");
    Print(L"|                 PROTOCOL DATABASE                 |\n");
    Print(L"+---------------------------------------------------+\n\n");
    
    for(ListEntry = ListEntry->ForwardLink, Index = 0;
        ListEntry != mDxeCoreProtocolDatabase;
        ListEntry = ListEntry->ForwardLink, Index++)
    {    
      ProtocolEntry = BASE_CR(ListEntry,PROTOCOL_ENTRY,AllEntries);
      Print(L"%03d - Protocol - %g\n",Index + 1, &(ProtocolEntry->ProtocolID));
    }
  }
  else
  {
    Print(L"Invalid command line option(s)\n");
    
  } 

  return EFI_SUCCESS;
}

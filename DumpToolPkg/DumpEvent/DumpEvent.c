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
#include <Protocol/Runtime.h>
#include "../../MdeModulePkg/Core/Dxe/Hand/Handle.h"
#include "../../MdeModulePkg/Core/Dxe/Event/Event.h"
#include <Guid/EventGroup.h>


extern UINTN Argc;
extern CHAR16 **Argv;
extern EFI_SHELL_PROTOCOL *mShellProtocol;
extern EFI_SHELL_PROTOCOL *GetShellProtocol(VOID);
extern EFI_STATUS GetArg(VOID);

LIST_ENTRY      *mDxeCoreEventSignalQueue;
LIST_ENTRY      *mDxeCoreEfiTimerList;


VOID ToolInfo(VOID)
{
  Print(L"Dump Event Tool V1.0 \nAuthor:ElderChen\n");
  Print(L"For Tool Usage : --help\n");
}

VOID PrintUsage(VOID)
{
  Print(L"-All      Dump All Event\n");
  Print(L"-Timer    Dump All Timer Event\n");
}

VOID DumpEventSignalQueue(VOID)
{
  LIST_ENTRY      *ListEntry;
  IEVENT          *Event;
  UINTN           Index;

  ListEntry = mDxeCoreEventSignalQueue;
  for (ListEntry = ListEntry->ForwardLink, Index = 0;
       ListEntry != mDxeCoreEventSignalQueue;
       ListEntry = ListEntry->ForwardLink, Index++) 
    {
      Event = CR(ListEntry, IEVENT, SignalLink, EVENT_SIGNATURE);
      Print(L"%03d - IEVENT - 0x%X\n", Index + 1, Event);
      Print(L"    Signature - 0x%X - String - %c%c%c%c\n",
                  Event->Signature,
                  (char)(Event->Signature),
                  (char)(Event->Signature>>8),
                  (char)(Event->Signature>>16),
                  (char)(Event->Signature>>24)
                );
      Print(L"    Type - 0x%X\n", Event->Type);
      Print(L"    SignalCount - %d\n", Event->SignalCount);
      Print(L"    SignalLink - ForwardLink 0x%X BackLink 0x%X\n",
                  Event->SignalLink.ForwardLink,
                  Event->SignalLink.BackLink);
      Print(L"    NotifyTpl - 0x%X\n", Event->NotifyTpl);
      Print(L"    NotifyFunction - 0x%X\n", Event->NotifyFunction);
      Print(L"    NotifyContext - 0x%X\n", Event->NotifyContext);
      Print(L"    EventGroup - %g\n",&(Event->EventGroup));
      Print(L"    NotifyLink - ForwardLink 0x%X BackLink 0x%X\n",
                  Event->NotifyLink.ForwardLink,
                  Event->NotifyLink.BackLink);
      Print(L"    ExFlag - %d\n",Event->ExFlag);
      Print(L"    RuntimeData - 0x%X\n",Event->RuntimeData);
      Print(L"    Timer - 0x%X\n",Event->Timer);
      if ((Event->Timer.TriggerTime != 0) || (Event->Timer.Period != 0)) 
      {
          Print(L"      (Link - ForwardLink 0x%X BackLink 0x%X)\n",
                    Event->Timer.Link.ForwardLink,
                    Event->Timer.Link.BackLink);
          Print(L"      (TiggerTime - %ld)\n", Event->Timer.TriggerTime);
          Print(L"      (Period - %ld)\n", Event->Timer.Period);
      }   
      Print(L"\n");
  }

  return;
}

VOID GetDxeCoreEventSignalQueue(IN IEVENT *IEvent)
{
  LIST_ENTRY      *ListEntry;
  IEVENT          *Event;

  ListEntry = &IEvent->SignalLink;
  for (ListEntry = ListEntry->ForwardLink;
       ListEntry != &IEvent->SignalLink;
       ListEntry = ListEntry->ForwardLink)
  {
    Event = BASE_CR(ListEntry, IEVENT, SignalLink);

    if (Event->Signature != EVENT_SIGNATURE) 
    {
      mDxeCoreEventSignalQueue = ListEntry;
      Print(L"mDxeCoreEventSignalQueue = 0x%X\n",mDxeCoreEventSignalQueue);
      break;
    } 
    
  }
  return;
}

VOID GetDxeCoreTimerList(
  IN IEVENT *IEvent
  )
{
  LIST_ENTRY      *ListEntry;
  IEVENT          *Event;

  ListEntry = &IEvent->Timer.Link;
  for (ListEntry = ListEntry->ForwardLink;
       ListEntry != &IEvent->Timer.Link;
       ListEntry = ListEntry->ForwardLink) {
    Event = BASE_CR(ListEntry, IEVENT, Timer.Link);
    if (Event->Signature != EVENT_SIGNATURE) 
    {
      mDxeCoreEfiTimerList = ListEntry;
      break;
    }
  }
  return;
}

VOID
DumpEventTimerList(
  VOID
  )
{
  LIST_ENTRY      *ListEntry;
  IEVENT          *Event;
  UINTN           Index;

  ListEntry = mDxeCoreEfiTimerList;
  Print(L"Timer Event\n");
  for (ListEntry = ListEntry->BackLink, Index = 0;
       ListEntry != mDxeCoreEfiTimerList;
       ListEntry = ListEntry->BackLink, Index++) {
    Event = CR(ListEntry, IEVENT, Timer.Link, EVENT_SIGNATURE);
    Print(L"%03d - IEVENT - 0x%X\n", Index + 1, Event);
    Print(L"    Signature - 0x%X - String - %c%c%c%c\n",
                Event->Signature,
                (char)(Event->Signature),
                (char)(Event->Signature >> 8),
                (char)(Event->Signature >> 16),
                (char)(Event->Signature >> 24)
              );
    Print(L"    Type - 0x%X\n", Event->Type);
    Print(L"    SignalCount - %d\n", Event->SignalCount);
    Print(L"    SignalLink - ForwardLink 0x%X BackLink 0x%X\n",
                Event->SignalLink.ForwardLink,
                Event->SignalLink.BackLink);
    Print(L"    NotifyTpl - 0x%X\n", Event->NotifyTpl);
    Print(L"    NotifyFunction - 0x%X\n", Event->NotifyFunction);
    Print(L"    NotifyContext - 0x%X\n", Event->NotifyContext);
    Print(L"    EventGroup - %g\n",&(Event->EventGroup));
    Print(L"    NotifyLink - ForwardLink 0x%X BackLink 0x%X\n",
                Event->NotifyLink.ForwardLink,
                Event->NotifyLink.BackLink);
    Print(L"    ExFlag - %d\n",Event->ExFlag);
    Print(L"    RuntimeData - 0x%X\n",Event->RuntimeData);
    Print(L"    Timer - 0x%X\n",Event->Timer);
    if ((Event->Timer.TriggerTime != 0) || (Event->Timer.Period != 0)) 
    {
      Print(L"      (Link - ForwardLink 0x%X BackLink 0x%X)\n",
                    Event->Timer.Link.ForwardLink,
                    Event->Timer.Link.BackLink);
      Print(L"      (TiggerTime - %ld)\n", Event->Timer.TriggerTime);
      Print(L"      (Period - %ld)\n", Event->Timer.Period);
    }   
    Print(L"\n");
  }

  return;
}


VOID EFIAPI NodeEventFunc(EFI_EVENT Event, VOID *Context)
{
   
}

EFI_STATUS EFIAPI DumpEvent
 (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
 )
{
   EFI_STATUS    Status;
   EFI_EVENT     NodeEvent;

  GetShellProtocol();
    
  GetArg();
   
  if (Argc == 1)
  {
    ToolInfo();
  } 

  else if(Argc == 2 && StrCmp(Argv[1],L"--help") == 0)
  {
    PrintUsage();
  }

  else if(Argc == 2 && StrCmp(Argv[1],L"-All") == 0)
  {

    Print(L"+--------------------------------------------------------------------+\n");
    Print(L"|                           EVENT DATABASE                           |\n");
    Print(L"+--------------------------------------------------------------------+\n");
    // Create Node Event to get gEventSignalQueue
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    NodeEventFunc,
                    NULL,
                    &gEfiEventReadyToBootGuid,
                    &NodeEvent);

  if (!EFI_ERROR (Status)) 
  {
    GetDxeCoreEventSignalQueue((IEVENT *)NodeEvent);
    gBS->CloseEvent(NodeEvent);
    if (mDxeCoreEventSignalQueue == NULL) 
    {
      Print(L"DxeCore gEventSignalQueue is not found!\n");
    }
    DumpEventSignalQueue();
  }
}
else if(Argc == 2 && StrCmp(Argv[1],L"-Timer") == 0)
{
    Status = gBS->CreateEvent (
                  EVT_TIMER,
                  TPL_NOTIFY,
                  NodeEventFunc,
                  NULL,
                  &NodeEvent
                  );
  if (!EFI_ERROR(Status)) {
    Status = gBS->SetTimer (
                    NodeEvent,
                    TimerPeriodic,
                    10 * 1000 * 1000
                    );
    if (!EFI_ERROR(Status)) {
      GetDxeCoreTimerList((IEVENT *)NodeEvent);
    }
    gBS->CloseEvent(NodeEvent);
    if (mDxeCoreEfiTimerList == NULL) 
    {
      Print(L"DxeCore mEfiTimerList is not found!\n");
    }
    DumpEventTimerList();
   }
    Print(L"\n");
 }

  else
  {
    Print(L"Invalid command line option(s)\n");
  } 

  return EFI_SUCCESS;
}



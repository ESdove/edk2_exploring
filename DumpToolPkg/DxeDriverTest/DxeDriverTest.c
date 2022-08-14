#include <PiPei.h>
#include <PiDxe.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/EventGroup.h>
#include <Protocol/ReportStatusCodeHandler.h>

void
MyGuidCallBackEvent (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  DEBUG ((DEBUG_INFO, "MyGuidCallBackEvent is called successfully\n"));
  gBS->CloseEvent (Event);
}

void
MyReadyToBootEvent (
  EFI_EVENT  Event,
  VOID       *Context
  )

{
  DEBUG ((DEBUG_INFO, "DxeDriverTestEntryPoint3\n"));
  DEBUG ((DEBUG_INFO, "%a %d  \n", __FUNCTION__, __LINE__));
  DEBUG ((DEBUG_INFO, "My ReadyToBootEvent Test\n"));
  gBS->CloseEvent (Event);
}

EFI_STATUS
EFIAPI
DxeDriverTestEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_EVENT  MyEvent;
  EFI_EVENT  MyEvent1;
  VOID       *Registration;
  int        HolyChen = 4;

  DEBUG ((DEBUG_INFO, "HolyChen =  %d  Stack HolyChen BA:0x%X\n", HolyChen, &HolyChen));

  gBS->CreateEventEx (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      MyReadyToBootEvent,
                      NULL,
                      &gEfiEventReadyToBootGuid,
                      &MyEvent
                      );
  DEBUG ((DEBUG_INFO, "Ready to Boot IEVENT BA: 0x%X\n", MyEvent));
  gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    MyGuidCallBackEvent,
                    NULL,
                    &MyEvent1
                    );
  DEBUG ((DEBUG_INFO, "Guid IEVENT BA: 0x%X\n", MyEvent1));
  DEBUG ((DEBUG_INFO, "Ready To RegisterProtocolNotify\n"));

  gBS->RegisterProtocolNotify (
                               &gEfiRscHandlerProtocolGuid,
                               MyEvent1,
                               &Registration
                               );

  DEBUG ((DEBUG_INFO, "DxeDriverTestEntryPoint is Ending\n"));
  return EFI_SUCCESS;
}

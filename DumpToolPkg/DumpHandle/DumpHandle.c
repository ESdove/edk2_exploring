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
#include "../../MdeModulePkg/Core/Dxe/Hand/Handle.h"

#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/LoadFile.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/HiiImageDecoder.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/PciIo.h>
#include <Protocol/MmPciRootBridgeIo.h>
#include <Protocol/SerialIo.h>
#include <Protocol/UsbIo.h>
#include <Protocol/I2cHost.h>
#include <Protocol/SuperIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/StorageSecurityCommand.h>
#include <Protocol/RestEx.h>
#include <Protocol/UnicodeCollation.h>


extern UINTN Argc;
extern CHAR16 **Argv;
extern EFI_SHELL_PROTOCOL *mShellProtocol;
extern EFI_SHELL_PROTOCOL *GetShellProtocol(VOID);
extern EFI_STATUS GetArg(VOID);
LIST_ENTRY                *mDxeCoreHandleList;

VOID ToolInfo(VOID)
{
  Print(L"Dump Handle Tool V1.1 \nAuthor:ElderChen\n");
  Print(L"Retrieve the handles where specificed protocol is installed\n");
  Print(L"--help Parameter for Tool Usage.\n");
}

VOID PrintUsage(VOID)
{  
  Print(L"-All -Detail  Dump All Handles for Details\n");
  Print(L"-All      Dump All Handles\n");
  Print(L"-gEfiLoadImageProtocolGuid\n");
  Print(L"-gEfiFirmwareVolume2ProtocolGuid\n");
  Print(L"-gEfiGraphicsOutputProtocolGuid\n");
  Print(L"-gEfiLoadFileProtocolGuid\n");
  Print(L"-gEfiFirmwareVolumeBlockProtocolGuid\n");
  Print(L"-gEfiDriverBindingProtocolGuid\n");
  Print(L"-gEfiHiiImageDecoderProtocolGuid\n");
  Print(L"-gEfiSimpleTextInProtocolGuid\n");
  Print(L"-gEfiSimplePointerProtocolGuid\n");
  Print(L"-gEfiPciIoProtocolGuid\n");
  Print(L"-gEfiPciRootBridgeIoProtocolGuid\n");
  Print(L"-gEfiSerialIoProtocolGuid\n");
  Print(L"-gEfiUsbIoProtocolGuid\n");
  Print(L"-gEfiI2cHostProtocolGuid\n");
  Print(L"-gEfiSioProtocolGuid\n");
  Print(L"-gEfiBlockIoProtocolGuid\n");
  Print(L"-gEfiDiskInfoProtocolGuid\n");
  Print(L"-gEfiDevicePathProtocolGuid\n");
  Print(L"-gEfiStorageSecurityCommandProtocolGuid\n");
  Print(L"-gEfiRestExServiceBindingProtocolGuid\n");
  Print(L"-gEfiUnicodeCollation2ProtocolGuid\n");
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
      Print(L"###################################################\n");
      Print(L"########## mDxeCoreHandList = 0X%X ##########\n",mDxeCoreHandleList);
      Print(L"###################################################\n\n");
      break;
    }
  }

  return;
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
       ListEntry = ListEntry->ForwardLink,Index++) {
    Handle = CR(ListEntry, IHANDLE, AllHandles, EFI_HANDLE_SIGNATURE);
    Print(L"%-4d Handle - BA = 0X%X\n", Index + 1,Handle);
    Print(L"     Signature = 0X%X\n",Handle->Signature);
    Print(L"     AllHandles ForwardLink = 0X%X BackLink = 0X%X\n",
                 Handle->AllHandles.ForwardLink,
                 Handle->AllHandles.BackLink);
    Print(L"     Protocol   ForWardLink = 0X%X BackLink = 0X%X\n",
                 Handle->Protocols.ForwardLink,
                 Handle->Protocols.BackLink);
    Print(L"     LocateRequest = %d\n",Handle->LocateRequest);
    Print(L"     Key = %d\n\n",Handle->Key); 
  }

  return;
}


EFI_STATUS
EFIAPI
DumpHandle
 (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS            Status;
  UINTN                 NoHandles;
  EFI_HANDLE            *HandleBuffer;
  UINTN                 Index;
  CHAR16                *PathStr;
  EFI_LOADED_IMAGE_PROTOCOL  *LoadedImage;

  GetShellProtocol();
    
  GetArg();

  if (Argc == 1)
  {
    ToolInfo();
    return EFI_SUCCESS;
  } 
  
  else if(Argc == 2 && ((StrCmp(Argv[1], L"--help") == 0)))
  {
      PrintUsage();
  } 
  else if(Argc == 3 && (StrCmp(Argv[1],L"-All") == 0) && (StrCmp(Argv[2],L"-Detail") == 0))
  {
     GetDxeCoreHandleList((IHANDLE *)ImageHandle);
     DumpHandleList();
  }
   else if((Argc ==2) && ((StrCmp(Argv[1], L"-All") == 0)))
  {
       Status = gBS->LocateHandleBuffer(
                AllHandles,
                NULL,
                NULL,
                &NoHandles,
                &HandleBuffer
                );
      Print(L"AllHandles NoHandles = %d sizeof(IHANDLE) = %d\n",NoHandles,sizeof(IHANDLE));
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);

       
        // {
        //   Print(L"    Signature = 0X%X\n",((IHANDLE *)(HandleBuffer[Index]))->Signature);
        //   Print(L"    AllHandles ForwardLink = 0X%X BackLink = 0X%X\n",
        //         (((IHANDLE *)(HandleBuffer[Index]))->AllHandles.ForwardLink),
        //         (((IHANDLE *)(HandleBuffer[Index]))->AllHandles.BackLink));
        //   Print(L"    Protocol ForWardLink =  0X%X  BackLink = 0X%X\n",
        //         (((IHANDLE *)(HandleBuffer[Index]))->Protocols.ForwardLink),
        //         (((IHANDLE *)(HandleBuffer[Index]))->Protocols.BackLink));
        //   Print(L"    LocateRequest = %d\n",
        //   ((IHANDLE *)(HandleBuffer[Index]))->LocateRequest);
        //   Print(L"    Key = %d\n",
        //   ((IHANDLE *)(HandleBuffer[Index]))->Key); 

        //  }
      }
      gBS->FreePool(HandleBuffer);
  } 
   
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiLoadImageProtocolGuid") == 0)))
  {
       Status = gBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiLoadedImageProtocolGuid,
                  NULL,
                  &NoHandles,
                  &HandleBuffer
                  );
  
      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
            Status = gBS->HandleProtocol(
                    HandleBuffer[Index],
                    &gEfiLoadedImageProtocolGuid,
                    (VOID **)&LoadedImage
                    );
            if (EFI_ERROR(Status)) {
              continue;
            }
        PathStr = ConvertDevicePathToText(LoadedImage->FilePath, TRUE, TRUE);
        Print(L"%-3d --- IHANDLE BA = 0X%X --- %s\n",Index+1,HandleBuffer[Index],PathStr);
      
      }
      gBS->FreePool(HandleBuffer);
  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiSimpleFileSystemProtocolGuid") == 0)))
  {
     
     Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiSimpleFileSystemProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      } 
      gBS->FreePool(HandleBuffer);
  }
  
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiFirmwareVolume2ProtocolGuid") == 0)))
   {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiFirmwareVolumeBlockProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      } 
      gBS->FreePool(HandleBuffer);
   }  
  
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiGraphicsOutputProtocolGuid") == 0)))
   {
       Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiGraphicsOutputProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
   }

   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiLoadFileProtocolGuid") == 0)))
   {
       Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiLoadFileProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
   } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiFirmwareVolumeBlockProtocolGuid") == 0)))
  {

      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiFirmwareVolumeBlockProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
  
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiDriverBindingProtocolGuid") == 0)))
  {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiDriverBindingProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
  
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiHiiImageDecoderProtocolGuid") == 0)))
  {
           Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiHiiImageDecoderProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiSimpleTextInProtocolGuid") == 0)))
  {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiSimpleTextInProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiSimplePointerProtocolGuid") == 0)))
  {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiSimplePointerProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiPciIoProtocolGuid") == 0)))
  {
     Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiPciIoProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiPciRootBridgeIoProtocolGuid") == 0)))
  {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiPciRootBridgeIoProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiSerialIoProtocolGuid") == 0)))
  {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiSerialIoProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiUsbIoProtocolGuid") == 0)))
  {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiUsbIoProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiI2cHostProtocolGuid") == 0)))
  {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiI2cHostProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiSioProtocolGuid") == 0)))
  {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiSioProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiBlockIoProtocolGuid") == 0)))
  {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiBlockIoProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiDiskInfoProtocolGuid") == 0)))
  {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiDiskInfoProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiDevicePathProtocolGuid") == 0)))
  {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiDevicePathProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);

  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiStorageSecurityCommandProtocolGuid") == 0)))
  {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiStorageSecurityCommandProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiRestExServiceBindingProtocolGuid") == 0)))
  {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiRestExServiceBindingProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
      gBS->FreePool(HandleBuffer);
  } 
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-gEfiUnicodeCollation2ProtocolGuid") == 0)))
  {
      Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiUnicodeCollation2ProtocolGuid,
                NULL,
                &NoHandles, 
                &HandleBuffer
                );

      Print(L"NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d IHANDLE - BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
       gBS->FreePool(HandleBuffer);
  } 

  else
  {
    Print(L"Invalid command line option(s)\n");
  } 
  
  return EFI_SUCCESS;
}

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
#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Guid/HobList.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/MemoryAllocationHob.h>

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

VOID ToolInfo(VOID)
{
  Print(L"Dump Handle Tool V1.1 \nAuthor:ElderChen\n");
  Print(L"Retrieve the handles where specificed protocol is installed\n");
  Print(L"--help Parameter for Tool Usage.\n");
}

VOID PrintUsage(VOID)
{
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

  GetShellProtocol();
    
  GetArg();

  if (Argc == 1)
  {
    ToolInfo();
    return EFI_SUCCESS;
  } 
  
  else if(Argc ==2 && ((StrCmp(Argv[1], L"--help") == 0)))
  {
      PrintUsage();
  } 
  
   else if(Argc ==2 && ((StrCmp(Argv[1], L"-All") == 0)))
  {
       Status = gBS->LocateHandleBuffer(
                AllHandles,
                NULL,
                NULL,
                &NoHandles,
                &HandleBuffer
                );
      Print(L"AllHandles NoHandles = %d\n",NoHandles);
      for(Index = 0; Index < NoHandles; Index++)
      {
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      } 
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
        Print(L"%-3d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      } 
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      } 
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
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
        Print(L"%-4d --- IHANDLE BA = 0X%X\n",Index+1,HandleBuffer[Index]);
      }
  } 

  else
  {
    Print(L"Invalid command line option(s)\n");
  } 

  return EFI_SUCCESS;
}

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
LIST_ENTRY *mDxeCoreHandleList;

VOID ToolInfo(VOID)
{
  Print(L"Dump Handle Tool V1.1 \nAuthor:ElderChen\n");
  Print(L"Retrieve the handles where specificed protocol is installed\n");
  Print(L"For Tool Usage: --help Parameters\n");
}

VOID PrintUsage(VOID)
{
  Print(L"-Detail          Dump All Handles for Details\n");
  Print(L"-All             Dump All Handles\n");
  Print(L"-LoadImage       gEfiLoadImageProtocolGuid\n");
  Print(L"-FS              gEfiSimpleFileSystemProtocolGuid\n");
  Print(L"-FV2             gEfiFirmwareVolume2ProtocolGuid\n");
  Print(L"-Gop             gEfiGraphicsOutputProtocolGuid\n");
  Print(L"-LoadFile        gEfiLoadFileProtocolGuid\n");
  Print(L"-FVB             gEfiFirmwareVolumeBlockProtocolGuid\n");
  Print(L"-DriverBinding   gEfiDriverBindingProtocolGuid\n");
  Print(L"-Hii             gEfiHiiImageDecoderProtocolGuid\n");
  Print(L"-SimpleTextIn    gEfiSimpleTextInProtocolGuid\n");
  Print(L"-SimplePointer   gEfiSimplePointerProtocolGuid\n");
  Print(L"-PciIo           gEfiPciIoProtocolGuid\n");
  Print(L"-PciRoot         gEfiPciRootBridgeIoProtocolGuid\n");
  Print(L"-SerialIo        gEfiSerialIoProtocolGuid\n");
  Print(L"-UsbIo           gEfiUsbIoProtocolGuid\n");
  Print(L"-I2c             gEfiI2cHostProtocolGuid\n");
  Print(L"-Sio             gEfiSioProtocolGuid\n");
  Print(L"-BlockIo         gEfiBlockIoProtocolGuid\n");
  Print(L"-DiskInfo        gEfiDiskInfoProtocolGuid\n");
  Print(L"-DevicePath      gEfiDevicePathProtocolGuid\n");
  Print(L"-StroageSec      gEfiStorageSecurityCommandProtocolGuid\n");
  Print(L"-ResetEx         gEfiRestExServiceBindingProtocolGuid\n");
  Print(L"-Unicode         gEfiUnicodeCollation2ProtocolGuid\n");
}

VOID GetDxeCoreHandleList(IN IHANDLE *IHandle)
{
  LIST_ENTRY *ListEntry;
  IHANDLE *Handle;

  ListEntry = &IHandle->AllHandles;
  for (ListEntry = ListEntry->ForwardLink;
       ListEntry != &IHandle->AllHandles;
       ListEntry = ListEntry->ForwardLink)
  {
    Handle = BASE_CR(ListEntry, IHANDLE, AllHandles);
    if (Handle->Signature != EFI_HANDLE_SIGNATURE)
    {

      mDxeCoreHandleList = ListEntry;
      Print(L"###################################################\n");
      Print(L"########## mDxeCoreHandList = 0X%X ##########\n", mDxeCoreHandleList);
      Print(L"###################################################\n\n");
      break;
    }
  }

  return;
}

VOID DumpHandleList(VOID)
{
  LIST_ENTRY *ListEntry;
  IHANDLE    *Handle;
  UINTN      Index;

  ListEntry = mDxeCoreHandleList;
  for (ListEntry = ListEntry->ForwardLink, Index = 0;
       ListEntry != mDxeCoreHandleList;
       ListEntry = ListEntry->ForwardLink, Index++)
  {
    Handle = CR(ListEntry, IHANDLE, AllHandles, EFI_HANDLE_SIGNATURE);
    Print(L"%-4d Handle - BA = 0X%X\n", Index + 1, Handle);
    Print(L"     Signature - 0X%X - String - %c%c%c%c\n", 
                  Handle->Signature,
                  (char)Handle->Signature,
                  (char)Handle->Signature >> 8,
                  (char)Handle->Signature >> 16,
                  (char)Handle->Signature >> 24);
    Print(L"     AllHandles ForwardLink = 0X%X BackLink = 0X%X\n",
          Handle->AllHandles.ForwardLink,
          Handle->AllHandles.BackLink);
    Print(L"     Protocol   ForWardLink = 0X%X BackLink = 0X%X\n",
          Handle->Protocols.ForwardLink,
          Handle->Protocols.BackLink);
    Print(L"     LocateRequest = %d\n", Handle->LocateRequest);
    Print(L"     Key = %d\n\n", Handle->Key);
  }

  return;
}

EFI_STATUS
EFIAPI
DumpHandle(IN EFI_HANDLE ImageHandle,
           IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status;
  UINTN NoHandles;
  EFI_HANDLE *HandleBuffer;
  UINTN Index;
  CHAR16 *PathStr;
  EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;

  GetShellProtocol();

  GetArg();

  if (Argc == 1)
  {
    ToolInfo();
    return EFI_SUCCESS;
  }

  else if (Argc == 2 && ((StrCmp(Argv[1], L"--help") == 0)))
  {
    PrintUsage();
  }
  else if (Argc == 3 && (StrCmp(Argv[2], L"-Detail") == 0))
  {
    GetDxeCoreHandleList((IHANDLE *)ImageHandle);
    DumpHandleList();
  }
  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-All") == 0)))
  {
    Status = gBS->LocateHandleBuffer( AllHandles,
                                      NULL,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
        
    Print(L"AllHandles NoHandles = %d sizeof(IHANDLE) = %d\n", NoHandles, sizeof(IHANDLE));
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);

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

  else if (Argc == 2 && ((StrCmp(Argv[1], L"-LoadImage") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiLoadedImageProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
     

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Status = gBS->HandleProtocol(
          HandleBuffer[Index],
          &gEfiLoadedImageProtocolGuid,
          (VOID **)&LoadedImage);
      if (EFI_ERROR(Status))
      {
        continue;
      }
      PathStr = ConvertDevicePathToText(LoadedImage->FilePath, TRUE, TRUE);
      Print(L"%-3d --- IHANDLE BA = 0X%X --- %s\n", Index + 1, HandleBuffer[Index], PathStr);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-FS") == 0)))
  {

    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiSimpleFileSystemProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
       

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }

  else if (Argc == 2 && ((StrCmp(Argv[1], L"-FV2") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiFirmwareVolumeBlockProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
     

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }

  else if (Argc == 2 && ((StrCmp(Argv[1], L"-Gop") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiGraphicsOutputProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
      

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }

  else if (Argc == 2 && ((StrCmp(Argv[1], L"-LoadFile") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiLoadFileProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
        

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-FVB") == 0)))
  {

    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiFirmwareVolumeBlockProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
    

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }

  else if (Argc == 2 && ((StrCmp(Argv[1], L"-DriverBinding") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiDriverBindingProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
  

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }

  else if (Argc == 2 && ((StrCmp(Argv[1], L"-Hii") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiHiiImageDecoderProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);


    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-SimpleTextIn") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiSimpleTextInProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
       

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-SimplePointer") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiSimplePointerProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
       

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-PciIo") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiPciIoProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
       

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-PciRoot") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiPciRootBridgeIoProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
       

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-SerialIo") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiSerialIoProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
       

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-UsbIo") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiUsbIoProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
        

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-I2c") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiI2cHostProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
        

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-Sio") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiSioProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
        

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-BlockIo") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiBlockIoProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
                                      

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-DiskInfo") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiDiskInfoProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
                                      

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-DevicePath") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiDevicePathProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
      

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-StroageSec") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiStorageSecurityCommandProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
                                      

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-ResetEx") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiRestExServiceBindingProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }
  else if (Argc == 2 && ((StrCmp(Argv[1], L"-Unicode") == 0)))
  {
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiUnicodeCollation2ProtocolGuid,
                                      NULL,
                                      &NoHandles,
                                      &HandleBuffer);
                                      

    Print(L"NoHandles = %d\n", NoHandles);
    for (Index = 0; Index < NoHandles; Index++)
    {
      Print(L"%-4d IHANDLE - BA = 0X%X\n", Index + 1, HandleBuffer[Index]);
    }
    gBS->FreePool(HandleBuffer);
  }

  else
  {
    Print(L"Invalid command line option(s)\n");
  }

  return EFI_SUCCESS;
}

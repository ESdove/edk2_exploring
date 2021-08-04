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

extern UINTN Argc;
extern CHAR16 **Argv;
extern EFI_SHELL_PROTOCOL *mShellProtocol;
extern EFI_SHELL_PROTOCOL *GetShellProtocol(VOID);
extern EFI_STATUS GetArg(VOID);

VOID ToolInfo(VOID)
{
  Print(L"Dump EfiLoadImages Tool V1.0 \nAuthor:ElderChen\n");
  Print(L"Dump EFI_LOAD_IMAGE_PROTOCOL Instances\n");
  Print(L"For Tool Usage: --help Parameter\n");
}

VOID PrintUsage(VOID)
{
  Print(L"-All      Dump All EfiLoadImages\n");
}

CHAR16 *mMemoryTypeShortName[] = {
    L"Reserved",
    L"LoaderCode",
    L"LoaderData",
    L"BS_Code",
    L"BS_Data",
    L"RT_Code",
    L"RT_Data",
    L"Available",
    L"Unusable",
    L"ACPI_Recl",
    L"ACPI_NVS",
    L"MMIO",
    L"MMIO_Port",
    L"PalCode",
    L"Persistent",
};

CHAR16 mUnknownStr[11];

CHAR16 *ShortNameOfMemoryType(IN UINT32 Type)
{
  if (Type < sizeof(mMemoryTypeShortName) / sizeof(mMemoryTypeShortName[0]))
  {
    return mMemoryTypeShortName[Type];
  }
  else
  {
    UnicodeSPrint(mUnknownStr, sizeof(mUnknownStr), L"%08x", Type);
    return mUnknownStr;
  }
}

EFI_STATUS EFIAPI
DumpEfiLoadImage(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status;
  UINTN NoHandles;
  EFI_HANDLE *HandleBuffer = NULL;
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

  else if (Argc == 2 && ((StrCmp(Argv[1], L"-All") == 0)))
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
      Print(L"%-3d IHANDLE BA = 0X%X --- %s\n", Index + 1, HandleBuffer[Index], PathStr);
      Print(L"      EFI_LOADED_IMAGE_PROTOCOL = 0X%X\n", (UINTN *)LoadedImage);
      Print(L"      ParentHandle = 0X%X\n", LoadedImage->ParentHandle);
      Print(L"      DeviceHandle = 0X%X\n", LoadedImage->DeviceHandle);
      Print(L"      LoadOptionSize = %d --- 0X%X\n", LoadedImage->LoadOptionsSize, LoadedImage->LoadOptionsSize);
      Print(L"      LoadOptions = 0X%X\n", LoadedImage->LoadOptions);
      Print(L"      ImageBase = 0X%X\n", LoadedImage->ImageBase);
      Print(L"      ImageSize = %d --- 0X%X\n", LoadedImage->ImageSize, LoadedImage->ImageSize);
      Print(L"      ImageCodeType = %s\n", ShortNameOfMemoryType(LoadedImage->ImageCodeType));
      Print(L"      ImageDataType = %s\n", ShortNameOfMemoryType(LoadedImage->ImageDataType));
      Print(L"      Unload = 0X%X\n\n", LoadedImage->Unload);

    }
  }
  else
  {
    Print(L"Invalid command line option(s)\n");
  }
  
  if(HandleBuffer != NULL)
  gBS->FreePool(HandleBuffer);
  
  return EFI_SUCCESS;
}

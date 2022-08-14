#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/GraphicsOutput.h>

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

EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInputEx;
EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput;

VOID
ToolInfo (
  VOID
  )
{
  Print (L"Test for Text and Gop Mode\nAuthor:ElderChen.\n");
  Print (L"--help Parameter for Tool Usage.\n");
}

VOID
PrintUsage (
  VOID
  )
{
  Print (L"-Text              Show Current Text Mode Info\n");
  Print (L"-Gop               Show Current Graphic Mode Info\n");
  Print (L"-AllText           Show All Text Mode Info\n");
  Print (L"-AllGop            Show All Graphic Mode Info\n");
}

EFI_STATUS
LocateSimpleTextInputEx (
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  *Handles;
  UINTN       HandleCount;
  UINTN       HandleIndex;

  Status = gBS->LocateHandleBuffer (
                                    ByProtocol,
                                    &gEfiSimpleTextInputExProtocolGuid,
                                    NULL,
                                    &HandleCount,
                                    &Handles
                                    );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->HandleProtocol (
                                  Handles[HandleIndex],
                                  &gEfiSimpleTextInputExProtocolGuid,
                                  (VOID **)&SimpleTextInputEx
                                  );

    ASSERT (!EFI_ERROR (Status));
  }

  gBS->FreePool (Handles);
  return EFI_SUCCESS;
}

EFI_STATUS
LocateGraphicsOutput (
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  *GraphicsOutputControllerHandles = NULL;
  UINTN       HandleIndex                      = 0;
  UINTN       HandleCount                      = 0;

  // get the handles which supports GraphicsOutputProtocol
  Status = gBS->LocateHandleBuffer (
                                    ByProtocol,
                                    &gEfiGraphicsOutputProtocolGuid,
                                    NULL,
                                    &HandleCount,
                                    &GraphicsOutputControllerHandles
                                    );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->HandleProtocol (
                                  GraphicsOutputControllerHandles[HandleIndex],
                                  &gEfiGraphicsOutputProtocolGuid,
                                  (VOID **)&GraphicsOutput
                                  );

    ASSERT (!EFI_ERROR (Status));
  }

  gBS->FreePool (GraphicsOutputControllerHandles);
  return EFI_SUCCESS;
}

VOID
ShowTextModeInfo (
  )
{
  Print (L"EFI_SIMPLE_TEXT_OUTPUT_MODE = %p\n", gST->ConOut->Mode);
  Print (L"  MaxMode=%d\n", gST->ConOut->Mode->MaxMode);
  Print (L"  Current Mode=%d\n", gST->ConOut->Mode->Mode);
  Print (L"  Attribute=%d\n", gST->ConOut->Mode->Attribute);
  Print (L"  CursorColumn=%d\n", gST->ConOut->Mode->CursorColumn);
  Print (L"  CursorRow=%d\n", gST->ConOut->Mode->CursorRow);
  Print (L"  CursorVisible=%d\n", gST->ConOut->Mode->CursorVisible);
}

EFI_STATUS
ShowSpecifyTextModeInfo (
  UINTN  ModeNumber
  )
{
  EFI_STATUS  Status;
  UINTN       Columns;
  UINTN       Rows;

  Status = gST->ConOut->QueryMode (gST->ConOut, ModeNumber, &Columns, &Rows);
  if (!EFI_ERROR (Status)) {
    Print (L"Text Mode %d\n", ModeNumber);
    Print (L"  Column=%d\n", Columns);
    Print (L"  Row=%d\n", Rows);
  }

  return Status;
}

VOID
ShowGraphicModeInfo (
  )
{
  Print (L"  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE %p\n", GraphicsOutput->Mode);
  Print (L"  MaxMode=%02x\n", GraphicsOutput->Mode->MaxMode);
  Print (L"  Current Mode=%02x\n", GraphicsOutput->Mode->Mode);
  Print (L"  SizeOfInfo=%x\n", GraphicsOutput->Mode->SizeOfInfo);
  Print (L"  Info->Version=%x\n", GraphicsOutput->Mode->Info->Version);
  Print (L"  Info->HorizontalResolution=%x\n", GraphicsOutput->Mode->Info->HorizontalResolution);
  Print (L"  Info->VerticalResolution=%x\n", GraphicsOutput->Mode->Info->VerticalResolution);
  Print (L"  Info->PixelsPerScanLine=%x\n", GraphicsOutput->Mode->Info->PixelsPerScanLine);
}

EFI_STATUS
ShowSpecifyGraphicModeInfo (
  UINT32  ModeNumber
  )
{
  EFI_STATUS                            Status;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *GopModeInfo;
  UINTN                                 SizeOfInfo;

  Status = GraphicsOutput->QueryMode (GraphicsOutput, ModeNumber, &SizeOfInfo, &GopModeInfo);
  if (!EFI_ERROR (Status)) {
    Print (L"EFI_GRAPHICS_OUTPUT_MODE_INFORMATION %p\n", GopModeInfo);
    Print (L"  Info->Version=%d\n", GopModeInfo->Version);
    Print (L"  Info->HorizontalResolution=%d\n", GopModeInfo->HorizontalResolution);
    Print (L"  Info->VerticalResolution=%d\n", GopModeInfo->VerticalResolution);
    Print (L"  Info->PixelsPerScanLine=%d\n", GopModeInfo->PixelsPerScanLine);
  }

  return Status;
}

EFI_STATUS
EFIAPI
Mode (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  GetShellProtocol ();

  GetArg ();

  if (Argc == 1) {
    ToolInfo ();
    return EFI_SUCCESS;
  } else if ((Argc == 2) && ((StrCmp (Argv[1], L"--help") == 0))) {
    PrintUsage ();
  } else if ((Argc == 2) && ((StrCmp (Argv[1], L"-Text") == 0))) {
    ShowTextModeInfo ();
    return EFI_SUCCESS;
  } else if ((Argc == 2) && ((StrCmp (Argv[1], L"-AllText") == 0))) {
    UINTN  ModeNumber;
    Status = LocateSimpleTextInputEx ();
    if (!EFI_ERROR (Status)) {
      for (ModeNumber = 0; ModeNumber < (UINTN)(gST->ConOut->Mode->MaxMode); ModeNumber++) {
        ShowSpecifyTextModeInfo (ModeNumber);
      }
    }

    return Status;
  } else if ((Argc == 2) && ((StrCmp (Argv[1], L"-Gop") == 0))) {
    Status = LocateGraphicsOutput ();
    if (!EFI_ERROR (Status)) {
      ShowGraphicModeInfo ();
    }

    return Status;
  } else if ((Argc == 2) && ((StrCmp (Argv[1], L"-AllGop") == 0))) {
    Status = LocateGraphicsOutput ();
    if (!EFI_ERROR (Status)) {
      UINT32  ModeNumber;
      for (ModeNumber = 0; ModeNumber < GraphicsOutput->Mode->MaxMode; ModeNumber++) {
        ShowSpecifyGraphicModeInfo (ModeNumber);
      }
    }

    return Status;
  } else {
    Print (L"Invalid command line option(s)\n");
  }

  return EFI_SUCCESS;
}

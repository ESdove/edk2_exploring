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

VOID
ToolInfo (
  VOID
  )
{
  Print (L"SimpleTextIn\nAuthor:ElderChen.\n");
  Print (L"For Tool Usage : --help Parameter\n");
}

VOID
PrintUsage (
  VOID
  )
{
  Print (L"-t               For Input Key Test\n");
}

EFI_STATUS
GetKeyFromEx (
  OUT UINT16                *ScanCode,
  OUT UINT16                *UnicodeChar,
  OUT UINT32                *KeyShiftState,
  OUT EFI_KEY_TOGGLE_STATE  *KeyToggleState
  )
{
  EFI_STATUS                         Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx;
  EFI_KEY_DATA                       KeyData;
  EFI_HANDLE                         *Handles;
  UINTN                              HandleCount;
  UINTN                              HandleIndex;
  UINTN                              Index;

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
                                  (VOID **)&SimpleTextInEx
                                  );

    ASSERT (!EFI_ERROR (Status));

    gBS->WaitForEvent (1, &(SimpleTextInEx->WaitForKeyEx), &Index);
    Status = SimpleTextInEx->ReadKeyStrokeEx (SimpleTextInEx, &KeyData);

    if (!EFI_ERROR (Status)) {
      *ScanCode       = KeyData.Key.ScanCode;
      *UnicodeChar    = KeyData.Key.UnicodeChar;
      *KeyShiftState  = KeyData.KeyState.KeyShiftState;
      *KeyToggleState = KeyData.KeyState.KeyToggleState;
      return EFI_SUCCESS;
    }
  }

  gBS->FreePool (Handles);
  return Status;
}

EFI_STATUS
EFIAPI
SimpleTextIn (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS            Status;
  UINT16                ScanCode    = 0;
  UINT16                UnicodeChar = 0;
  UINT32                KeyShiftState;
  EFI_KEY_TOGGLE_STATE  KeyToggleState;
  UINT32                count = 0;

  GetShellProtocol ();

  GetArg ();

  if (Argc == 1) {
    ToolInfo ();
    return EFI_SUCCESS;
  } else if ((Argc == 2) && ((StrCmp (Argv[1], L"--help") == 0))) {
    PrintUsage ();
  } else if ((Argc == 2) && ((StrCmp (Argv[1], L"-t") == 0))) {
    gST->ConOut->OutputString (gST->ConOut, L"Please Input Key(ESC to exit):\n\r");

    while (ScanCode != 0x17) {
      // ESC
      Status = GetKeyFromEx (&ScanCode, &UnicodeChar, &KeyShiftState, &KeyToggleState);
      if (EFI_ERROR (Status)) {
        Print (L"Call GetKeyFromEx() Error!\n");
        break;
      } else {
        Print (L"NO.%08d\n", count);
        ++count;
        Print (L"  ScanCode=%04x", ScanCode);
        Print (L"  UnicodeChar=%04x", UnicodeChar);
        Print (L"  KeyShiftState=%08x", KeyShiftState);
        Print (L"  KeyToggleState=%02x", KeyToggleState);
        Print (L"\n");
      }
    }
  } else {
    Print (L"Invalid command line option(s)\n");
  }

  return EFI_SUCCESS;
}

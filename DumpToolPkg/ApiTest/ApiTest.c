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
  Print (L"ApiTest\nAuthor:ElderChen.\n");
  Print (L"For Tool Usage : --help Parameter.\n");
}

VOID
PrintUsage (
  VOID
  )
{
  // Print(L"-dump            Dump All GPIO Infomation\n");
  Print (L"-[n]               n = A,B,C,D,E,F,G,H,I,J,K,R,S,GPD\n");
}

EFI_STATUS
EFIAPI
ApiTest (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_SYSTEM_TABLE  *mST = NULL;

  // EFI_STATUS Status;

  mST = SystemTable;
  UINTN          Index;
  EFI_INPUT_KEY  key;

  SystemTable->BootServices->WaitForEvent (1, &(SystemTable->ConIn->WaitForKey), &Index);
  SystemTable->ConIn->ReadKeyStroke (SystemTable->ConIn, &key);
  Print (L"=== <%c>  ===", key.UnicodeChar);

  GetShellProtocol ();

  GetArg ();

  if (Argc == 1) {
    ToolInfo ();
    return EFI_SUCCESS;
  } else if ((Argc == 2) && ((StrCmp (Argv[1], L"-A") == 0))) {
    PrintUsage ();
  } else {
    Print (L"Invalid command line option(s)\n");
  }

  return EFI_SUCCESS;
}

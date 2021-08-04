
#include <Uefi.h>
#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>

UINTN                Argc;
CHAR16               **Argv;
EFI_SHELL_PROTOCOL   *mShellProtocol = NULL;


EFI_STATUS GetArg (VOID)
{
  EFI_STATUS                    Status;
  EFI_SHELL_PARAMETERS_PROTOCOL *ShellParameters;
//UINTN                         i = 0;

  Status = gBS->HandleProtocol (gImageHandle,
                                &gEfiShellParametersProtocolGuid,
                                (VOID**)&ShellParameters);
                                
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  Argc = ShellParameters->Argc;
  Argv = ShellParameters->Argv;
   
// for (i = 0; i < Argc; i++)
// { 
//     Print(L"Argc = %d\n",Argc);
//     Print(L"Argv[i] = %s\n",Argv[i]);
// }

  return EFI_SUCCESS;
}



 EFI_SHELL_PROTOCOL *
 GetShellProtocol (VOID)
 {
   EFI_STATUS            Status;

   if (mShellProtocol == NULL) 
   {
     Status = gBS->LocateProtocol (&gEfiShellProtocolGuid,
                                  NULL,
                                  (VOID **) &mShellProtocol);
     if (EFI_ERROR (Status))
     {
      mShellProtocol = NULL;
     }
   }
   return mShellProtocol;
 }
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

#define PCH_PCR_BASE_ADDRESS 0xE0000000 ///< SBREG MMIO Base Address
#define PCH_PCR_ADDRESS(Pid, Offset) (PCH_PCR_BASE_ADDRESS | (UINT32)(((Offset)&0x0F0000) << 8) | ((UINT8)(Pid) << 16) | (UINT16)((Offset)&0xFFFF))

//From Adl S PCH Volume2 GPIO Community [n] Registers Summary
#define PID_GPIOCOM0 0x6E
/**
   GPP_I_0 - GPP_I_22  offset(0x700 - 0x860)
   GPP_R_0 - GPP_R_21  offset(0x890 - 0x9E0)
   GPP_J_0 - GPP_J_11  offset(0xA00 - 0xAB0)
**/

#define PID_GPIOCOM1 0x6D
/**
   GPP_B_0 - GPP_B_23  offset(0x700 - 0x870)
   GPP_G_0 - GPP_G-7   offset(0x880 - 0x8F0)
   GPP_H_0 - GPP_H_23  offset(0x900 - 0xA70)
 **/
#define PID_GPIOCOM2 0x6C
/**
   GPD_0 - GPD_12       offset(0x700 - 0x7C0)
**/
#define PID_GPIOCOM3 0x6B
/**
   GPP_A_0 - GPP_A_14   offset(0x790 - 0x870)
   GPP_C_0 - GPP_C_23   offset(0x890 - 0xA00)
**/
#define PID_GPIOCOM4 0x6A
/**
   GPP_S_0 - GPP_S_7    offset(0x700 - 0x770)
   GPP_E_0 - GPP_E_21   offset(0x780 - 0x8D0)
   GPP_K_0 - GPP_K_11   offset(0x8F0 - 0x9A0)
   GPP_F_0 - GPP_F_23   offset(0x9E0 - 0xB50)
**/
#define PID_GPIOCOM5 0x69
/**
   GPP_D_0 - GPP_D_23   offset(0x700 - 0x870)
**/

extern UINTN Argc;
extern CHAR16 **Argv;
extern EFI_SHELL_PROTOCOL *mShellProtocol;
extern EFI_SHELL_PROTOCOL *GetShellProtocol(VOID);
extern EFI_STATUS GetArg(VOID);

CHAR16 GpioCmdString[sizeof("mem #################")];
UINTN GpioAddress;
UINTN index;

VOID ToolInfo(VOID)
{
  Print(L"AdlS Gpio Tool V1.0\nAuthor:ElderChen.\n");
  Print(L"For Tool Usage: --help Parameter \n");
}

VOID PrintUsage(VOID)
{
  Print(L"-[n]               n = A,B,C,D,E,F,G,H,I,J,K,R,S,GPD\n");
}

EFI_STATUS EFIAPI
AdlSGpio(IN EFI_HANDLE ImageHandle,
         IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status;
  GetShellProtocol();
  GetArg();

  if (Argc == 1)
  {
    ToolInfo();
  }

  else if ((Argc == 2) && (StrCmp(Argv[1], L"--help")) == 0)
  {
    PrintUsage();
  }

  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-A") == 0) || (StrCmp(Argv[1], L"-a") == 0)))
  {
    GpioAddress = PCH_PCR_ADDRESS(PID_GPIOCOM3, 0x790);
    UnicodeSPrint(GpioCmdString, sizeof(GpioCmdString), L"mem 0X%08x 0XF0", GpioAddress);

    mShellProtocol->Execute(&ImageHandle,
                            GpioCmdString,
                            NULL,
                            &Status);

    Print(L"\nCommunity 3 GPP_A_0 - GPP_A_14\n");
    for (index = 0; index < 15; index++)
    {
      Print(L"  GPP_A%-2d:0X%08x\t", index, *(UINT32 *)(GpioAddress + (index * 0x10)));
    }
    Print(L"\n");
  }

  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-B") == 0) || (StrCmp(Argv[1], L"-b") == 0)))
  {
    GpioAddress = PCH_PCR_ADDRESS(PID_GPIOCOM1, 0x700);
    UnicodeSPrint(GpioCmdString, sizeof(GpioCmdString), L"mem 0X%08x 0X180", GpioAddress);

    mShellProtocol->Execute(&ImageHandle,
                            GpioCmdString,
                            NULL,
                            &Status);

    Print(L"\nCommunity 1 GPP_B_0 - GPP_B_23\n");

    for (index = 0; index < 24; index++)
    {
      Print(L"  GPP_B%-2d:0X%08x\t", index, *(UINT32 *)(GpioAddress + (index * 0x10)));
    }
    Print(L"\n");
  }

  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-C") == 0) || (StrCmp(Argv[1], L"-c") == 0)))
  {
    GpioAddress = PCH_PCR_ADDRESS(PID_GPIOCOM3, 0x890);
    UnicodeSPrint(GpioCmdString, sizeof(GpioCmdString), L"mem 0X%08x 0X180", GpioAddress);

    mShellProtocol->Execute(&ImageHandle,
                            GpioCmdString,
                            NULL,
                            &Status);

    Print(L"\nCommunity 3 GPP_C_0 - GPP_C_23\n");

    for (index = 0; index < 24; index++)
    {
      Print(L"  GPP_C%-2d:0X%08x\t", index, *(UINT32 *)(GpioAddress + (index * 0x10)));
    }
    Print(L"\n");
  }

  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-D") == 0) || (StrCmp(Argv[1], L"-d") == 0)))
  {
    GpioAddress = PCH_PCR_ADDRESS(PID_GPIOCOM5, 0x700);
    UnicodeSPrint(GpioCmdString, sizeof(GpioCmdString), L"mem 0X%08x 0X180", GpioAddress);

    mShellProtocol->Execute(&ImageHandle,
                            GpioCmdString,
                            NULL,
                            &Status);

    Print(L"\nCommunity 5 GPP_D_0 - GPP_D_23\n");

    for (index = 0; index < 24; index++)
    {
      Print(L"  GPP_D%-2d:0X%08x\t", index, *(UINT32 *)(GpioAddress + (index * 0x10)));
    }
    Print(L"\n");
  }

  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-E") == 0) || (StrCmp(Argv[1], L"-e") == 0)))
  {
    GpioAddress = PCH_PCR_ADDRESS(PID_GPIOCOM4, 0x780);
    UnicodeSPrint(GpioCmdString, sizeof(GpioCmdString), L"mem 0X%08x 0X160", GpioAddress);

    mShellProtocol->Execute(&ImageHandle,
                            GpioCmdString,
                            NULL,
                            &Status);

    Print(L"\nCommunity 4 GPP_E_0 - GPP_E_21\n");

    for (index = 0; index < 22; index++)
    {
      Print(L"  GPP_E%-2d:0X%08x\t", index, *(UINT32 *)(GpioAddress + (index * 0x10)));
    }
    Print(L"\n");
  }

  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-F") == 0) || (StrCmp(Argv[1], L"-f") == 0)))
  {
    GpioAddress = PCH_PCR_ADDRESS(PID_GPIOCOM4, 0x9E0);
    UnicodeSPrint(GpioCmdString, sizeof(GpioCmdString), L"mem 0X%08x 0X180", GpioAddress);

    mShellProtocol->Execute(&ImageHandle,
                            GpioCmdString,
                            NULL,
                            &Status);

    Print(L"\nCommunity 4 GPP_F_0 - GPP_F_23\n");

    for (index = 0; index < 24; index++)
    {
      Print(L"  GPP_F%-2d:0X%08x\t", index, *(UINT32 *)(GpioAddress + (index * 0x10)));
    }
    Print(L"\n");
  }

  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-G") == 0) || (StrCmp(Argv[1], L"-g") == 0)))
  {
    GpioAddress = PCH_PCR_ADDRESS(PID_GPIOCOM1, 0x880);
    UnicodeSPrint(GpioCmdString, sizeof(GpioCmdString), L"mem 0X%08x 0X80", GpioAddress);

    mShellProtocol->Execute(&ImageHandle,
                            GpioCmdString,
                            NULL,
                            &Status);

    Print(L"\nCommunity 1 GPP_G_0 - GPP_G_7\n");

    for (index = 0; index < 8; index++)
    {
      Print(L"  GPP_G%-2d:0X%08x\t", index, *(UINT32 *)(GpioAddress + (index * 0x10)));
    }
    Print(L"\n");
  }

  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-H") == 0) || (StrCmp(Argv[1], L"-h") == 0)))
  {
    GpioAddress = PCH_PCR_ADDRESS(PID_GPIOCOM1, 0x900);
    UnicodeSPrint(GpioCmdString, sizeof(GpioCmdString), L"mem 0X%08x 0X180", GpioAddress);

    mShellProtocol->Execute(&ImageHandle,
                            GpioCmdString,
                            NULL,
                            &Status);

    Print(L"\nCommunity 1 GPP_H_0 - GPP_H_23\n");

    for (index = 0; index < 24; index++)
    {
      Print(L"  GPP_H%-2d:0X%08x\t", index, *(UINT32 *)(GpioAddress + (index * 0x10)));
    }
    Print(L"\n");
  }

  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-I") == 0) || (StrCmp(Argv[1], L"-i") == 0)))
  {
    GpioAddress = PCH_PCR_ADDRESS(PID_GPIOCOM0, 0x700);
    UnicodeSPrint(GpioCmdString, sizeof(GpioCmdString), L"mem 0X%08x 0X170", GpioAddress);

    mShellProtocol->Execute(&ImageHandle,
                            GpioCmdString,
                            NULL,
                            &Status);

    Print(L"\nCommunity 0 GPP_I_0 - GPP_I_22\n");

    for (index = 0; index < 23; index++)
    {
      Print(L"  GPP_I%-2d:0X%08x\t", index, *(UINT32 *)(GpioAddress + (index * 0x10)));
    }
    Print(L"\n");
  }

  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-J") == 0) || (StrCmp(Argv[1], L"-j") == 0)))
  {
    GpioAddress = PCH_PCR_ADDRESS(PID_GPIOCOM0, 0xA00);
    UnicodeSPrint(GpioCmdString, sizeof(GpioCmdString), L"mem 0X%08x 0XC0", GpioAddress);

    mShellProtocol->Execute(&ImageHandle,
                            GpioCmdString,
                            NULL,
                            &Status);

    Print(L"\nCommunity 0 GPP_J_0 - GPP_J_11\n");

    for (index = 0; index < 12; index++)
    {
      Print(L"  GPP_J%-2d:0X%08x\t", index, *(UINT32 *)(GpioAddress + (index * 0x10)));
    }
    Print(L"\n");
  }

  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-K") == 0) || (StrCmp(Argv[1], L"-k") == 0)))
  {
    GpioAddress = PCH_PCR_ADDRESS(PID_GPIOCOM4, 0x8F0);
    UnicodeSPrint(GpioCmdString, sizeof(GpioCmdString), L"mem 0X%08x 0XC0", GpioAddress);

    mShellProtocol->Execute(&ImageHandle,
                            GpioCmdString,
                            NULL,
                            &Status);

    Print(L"\nCommunity 4 GPP_K_0 - GPP_K_11\n");

    for (index = 0; index < 12; index++)
    {
      Print(L"  GPP_K%-2d:0X%08x\t", index, *(UINT32 *)(GpioAddress + (index * 0x10)));
    }
    Print(L"\n");
  }

  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-R") == 0) || (StrCmp(Argv[1], L"-r") == 0)))
  {
    GpioAddress = PCH_PCR_ADDRESS(PID_GPIOCOM0, 0x890);
    UnicodeSPrint(GpioCmdString, sizeof(GpioCmdString), L"mem 0X%08x 0X160", GpioAddress);

    mShellProtocol->Execute(&ImageHandle,
                            GpioCmdString,
                            NULL,
                            &Status);

    Print(L"\nCommunity 0 GPP_R_0 - GPP_R_21\n");

    for (index = 0; index < 22; index++)
    {
      Print(L"  GPP_R%-2d:0X%08x\t", index, *(UINT32 *)(GpioAddress + (index * 0x10)));
    }
    Print(L"\n");
  }

  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-S") == 0) || (StrCmp(Argv[1], L"-s") == 0)))
  {
    GpioAddress = PCH_PCR_ADDRESS(PID_GPIOCOM4, 0x700);
    UnicodeSPrint(GpioCmdString, sizeof(GpioCmdString), L"mem 0X%08x 0X80", GpioAddress);

    mShellProtocol->Execute(&ImageHandle,
                            GpioCmdString,
                            NULL,
                            &Status);

    Print(L"\nCommunity 4 GPP_S_0 - GPP_S_7\n");

    for (index = 0; index < 8; index++)
    {
      Print(L"  GPP_S%-2d:0X%08x\t", index, *(UINT32 *)(GpioAddress + (index * 0x10)));
    }
    Print(L"\n");
  }

  else if ((Argc == 2) && ((StrCmp(Argv[1], L"-GPD") == 0) || (StrCmp(Argv[1], L"-gpd") == 0)))
  {
    GpioAddress = PCH_PCR_ADDRESS(PID_GPIOCOM1, 0x700);
    UnicodeSPrint(GpioCmdString, sizeof(GpioCmdString), L"mem 0X%08x 0XD0", GpioAddress);

    mShellProtocol->Execute(&ImageHandle,
                            GpioCmdString,
                            NULL,
                            &Status);

    Print(L"\nCommunity 2 GPD_0 - GPD_12\n");

    for (index = 0; index < 13; index++)
    {
      Print(L"  GPD_ %-2d:0X%08x\t", index, *(UINT32 *)(GpioAddress + (index * 0x10)));
    }
    Print(L"\n");
  }

  else
  {
    Print(L"Invalid command line option(s)\n");
  }

  return EFI_SUCCESS;
}

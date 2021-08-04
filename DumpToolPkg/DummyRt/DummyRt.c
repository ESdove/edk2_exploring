
#include <Uefi.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

UINTN Global;
UINTN GlobalInit = 3;
UINTN GlobalInit0 = 0;
CHAR8 *mGlobal = "Just For Test";
CHAR8 *mGloba2 = "Just for Testing";

VOID *mRtCode;
VOID *mRtData;



static int stack_dir;  
static void find_stack_direction (void)  {  
    static char   *addr = NULL;   /* address of first
                                     `dummy', once known */  
    char     dummy;          /* to get stack address */  
    Print(L"dummy = %p\n",&dummy);
    if (addr == NULL)  
    {                           /* initial entry */  
        addr = &dummy;  

        find_stack_direction ();  /* recurse once */  
    }  
    else                          /* second entry */  
        if (&dummy > addr)  
            stack_dir = 1;            /* stack grew upward */  
        else  
            stack_dir = -1;           /* stack grew downward */  
}  



void test2()
{
  int mStack2 = 3;
  Print(L"&mStack2 = %p\n",&mStack2);
  
}
    
 void test1()
 {
   int mStack1 = 4;
   Print(L"&mStack1 = %p\n",&mStack1);
   test2();
 }


/**
  Main entry for this driver/library.

  @param[in] ImageHandle  Image handle this driver.
  @param[in] SystemTable  Pointer to SystemTable.

**/
EFI_STATUS
EFIAPI
TestEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  Print (L"TestEntryPoint - 0x%p\n", TestEntryPoint);

  UINTN mStackData1 = 3;
  UINTN mStackData2 = 4;
  UINTN mStackData3[3] = {0,1,2};
  CHAR16 *mStackData4 = L"Just For Test";
  UINTN mStackData5 = 6;
  Print(L"=======Stack=======\n");
  Print(L"mStackData1 - 0x%x\n", &mStackData1);
  Print(L"mStackData1 - 0x%p\n", &mStackData1);
  Print(L"mStackData2 - 0x%p\n", &mStackData2);
  Print(L"mStackData3 - 0x%p - 0x%x - 0x%x\n", &mStackData3[0],&mStackData3[1],&mStackData3[2]);
  Print(L"mStackData4 - 0x%p\n", &mStackData4);
  Print(L"mStackData4 String - 0x%x\n", mStackData4);
  Print(L"mStackData5 - 0x%x\n", &mStackData5);
  Print(L"=======Stack=======\n");
  Print(L"=======Global=======\n");
  Print (L"Global -   0x%p\n", &Global);
  Print (L"GlobalInit -   0x%p\n", &GlobalInit);
  Print (L"GlobalInit0 -   0x%p\n", &GlobalInit0);
  Print (L"mGlobal1 -   0x%p\n", &mGlobal);
  Print (L"mGlobal1 String -   0x%x\n", mGlobal);
  Print (L"mGlobal2 -   0x%p\n", &mGloba2);
  Print (L"mGlobal2 String -   0x%x\n", mGloba2);
  Print (L"RT .code - 0x%p\n", &mRtCode);
  Print (L"RT .data - 0x%p\n", &mRtData);
  Print(L"=======Global=======\n");
  Print(L"=======Heap=======\n");
  gBS->AllocatePool (EfiRuntimeServicesCode, 1, &mRtCode);
  gBS->AllocatePool (EfiRuntimeServicesData, 1, &mRtData);
  Print (L"RT Code - 0x%p\n", mRtCode);
  Print (L"RT Data - 0x%p\n", mRtData);
  Print(L"=======Heap=======\n");
  find_stack_direction();  
    if(stack_dir==1)  
        Print(L"stack grew upward\n");  
    else  
        Print(L"stack grew downward\n");  
   
        test1();
  return EFI_SUCCESS;
}

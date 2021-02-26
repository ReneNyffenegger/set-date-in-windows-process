#include <windows.h>
#include <stdio.h>
#include "detours.h"
#include "setDate.h"

int main(int argc, char* argv[]) {

   if (argc < 2) {
      printf("specify executable\n");
      return 1;
   }

   STARTUPINFO         si; ZeroMemory(&si, sizeof(si));
   PROCESS_INFORMATION pi; ZeroMemory(&pi, sizeof(pi));

   si.cb = sizeof(si);
   si.dwFlags = STARTF_USESHOWWINDOW;
   si.wShowWindow = SW_SHOW;


   char* dllPath = malloc(MAX_PATH);
   char* cp = dllPath + GetModuleFileName(NULL, dllPath, MAX_PATH);
   while (*--cp != '\\');
   memcpy (cp+1, "setDate.dll\x00", 12);
   printf("%s<\n", dllPath);

   char* lpCommandLine = GetCommandLine();

   lpCommandLine += strlen(argv[0]) + 2; // Skip program name plus 2 apostrophes in which it is enclosed.
   printf(lpCommandLine);
   printf("\n");

   while (*++lpCommandLine == ' ');

   printf("starting to parse with >%s<\n",lpCommandLine);
   SYSTEMTIME fake_SYSTEMTIME;

   *(lpCommandLine + 4) = 0; fake_SYSTEMTIME.wYear         = atoi(lpCommandLine); lpCommandLine += 5;
   *(lpCommandLine + 2) = 0; fake_SYSTEMTIME.wMonth        = atoi(lpCommandLine); lpCommandLine += 3;
   *(lpCommandLine + 2) = 0; fake_SYSTEMTIME.wDay          = atoi(lpCommandLine); lpCommandLine += 3;
   *(lpCommandLine + 2) = 0; fake_SYSTEMTIME.wHour         = atoi(lpCommandLine); lpCommandLine += 3;
   *(lpCommandLine + 2) = 0; fake_SYSTEMTIME.wMinute       = atoi(lpCommandLine); lpCommandLine += 3;
   *(lpCommandLine + 2) = 0; fake_SYSTEMTIME.wSecond       = atoi(lpCommandLine); lpCommandLine += 3;
                             fake_SYSTEMTIME.wDayOfWeek    = 0;  // Hah. How am I supposed to know?
                             fake_SYSTEMTIME.wMilliseconds = 0;  // Granularity of one second is enough for everyone...

   while (*lpCommandLine == ' ') lpCommandLine++;

   if (! DetourCreateProcessWithDllEx (
       0                           , // LPCTSTR                         appName
       lpCommandLine               , // LPTSTR                          lpCommandLine,
       0                           , // LPSECURITY_ATTRIBUTES           lpProcessAttributes,
       0                           , // LPSECURITY_ATTRIBUTES           lpThreadAttributes,
       0                           , // BOOL                            bInheritHandles,
       CREATE_DEFAULT_ERROR_MODE |
       CREATE_SUSPENDED            , // DWORD                           dwCreationFlags,
       0                           , // LPVOID                          lpEnvironment,
       0                           , // LPCTSTR                         lpCurrentDirectory,
      &si                          , // LPSTARTUPINFOW                  lpStartupInfo,
      &pi                          , // LPPROCESS_INFORMATION           lpProcessInformation,
       dllPath                     , // LPCSTR                          lpDllName
       0                             // PDETOUR_CREATE_PROCESS_ROUTINEW pfCreateProcessW
   )) {

     printf("Failed.\n");
     exit(-1);

   }

   free(dllPath);

// https://github.com/microsoft/Detours/blob/master/samples/tracebld/tracebld.cpp
   if (!DetourCopyPayloadToProcess(
          pi.hProcess,
         &payload,
         &fake_SYSTEMTIME,
         sizeof(SYSTEMTIME))
      ) {

       printf("failed copy payload\n");
       return 1;
    }


    ResumeThread(pi.hThread);

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(&si);
    CloseHandle(&pi);

    return 0;
}

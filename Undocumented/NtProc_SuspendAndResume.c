#pragma warning(disable:4996)

#include <windows.h>
#include <tlHelp32.h>
#include <stdio.h>


void NtSuspend(DWORD);
void NtResume(DWORD);
BOOL GetProcessList();

typedef LONG(NTAPI *NtProcess)(IN HANDLE ProcessHandle);

int main() {
	
	unsigned int number[2]={1,1};
    DWORD dwPid = 1;	

    while (number[0]) {
		puts("1. Tasklist");
		puts("2. InputPid");
		puts("3. Exit");
		printf(">> ");  
        scanf("%u", &number[0]);

        switch (number[0]) {
        case 1:
            GetProcessList();
            break;
		case 2:
			printf("Enter the pid: ");
			scanf("%u",&dwPid);			
			while (number[1]) {				
                puts("1. Suspend");
                puts("2. Resume");
                puts("3. Back");
				printf(">> ");
                scanf("%u", &number[1]);
                switch (number[1]) {
                case 1:
                    NtSuspend(dwPid);
                    break;
                case 2:
                    NtResume(dwPid);
                    break;
                case 3:
                    number[1] = 0;
					break;
				default:
					puts("Enter the valid number");
                }
            }
			number[1]=1;
			break;
		case 3:
			number[0]=0;
			printf("terminate program");
			break;
		default:
			puts("Enter the valid number");            
        }
    }
	return 0;
}

void NtSuspend(DWORD dwPid) {        

    HANDLE hProcess;    
    NtProcess pfnNtSuspendProcess;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
    if (hProcess == NULL) {
        puts("OpenProcess is failed in the NtSuspend");
        return;
    }
    
    pfnNtSuspendProcess = (NtProcess)GetProcAddress(GetModuleHandle("ntdll"), "NtSuspendProcess");    
    if (pfnNtSuspendProcess == NULL) {
        puts("GetProcAddress is failed in the NtSuspend");
        return;
    }
    
    if (pfnNtSuspendProcess(hProcess) == 0) puts("successed suspending");
    else puts("failed suspending");
    
    CloseHandle(hProcess);    
}

void NtResume(DWORD dwPid) {

    HANDLE hProcess;
    NtProcess pfnNtResumeProcess;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);    
    if (hProcess == NULL) {
        puts("OpenProcess is failed in the NtResume");
        return;
    }
    
    pfnNtResumeProcess = (NtProcess)GetProcAddress(GetModuleHandle("ntdll"), "NtResumeProcess");
    if (pfnNtResumeProcess == NULL) {
        puts("GetProcAddress is failed in the NtResume");
        return;
    }
    
    if (pfnNtResumeProcess(hProcess) == 0) puts("successed resuming");
    else puts("failed resuming");
    CloseHandle(hProcess);
}


// referred to msdn
BOOL GetProcessList(){

  HANDLE hProcessSnap;  
  PROCESSENTRY32 pe32;  
  
  hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
  if( hProcessSnap == INVALID_HANDLE_VALUE )  {
    puts("CreateToolhelp32Snapshot (of processes)");
    return 0;
  }
  
  pe32.dwSize = sizeof( PROCESSENTRY32 );
  if( !Process32First( hProcessSnap, &pe32 ) )  {
    puts("Process32First"); 
    CloseHandle( hProcessSnap );          
    return( FALSE );
  }  
  
  printf("%-24s%10s%10s\n","Image Name","PID","PPID");
  do  {
    printf("%-24s%10u%10u\n",pe32.szExeFile,pe32.th32ProcessID,pe32.th32ParentProcessID);
  } while( Process32Next( hProcessSnap, &pe32 ) );

  CloseHandle( hProcessSnap );  
  return 1;
}

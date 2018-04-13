#ifndef _MAINDLG_CPP
#define _MAINDLG_CPP
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Windows.H>
#include <Stdio.H>
#include <Stdlib.H>

#include "../Source/Entry.H"

#include "../Dialogs/MainDlg.H"
#include "../Dialogs/NetLogoDlg.H"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace NSWFL::Windows;
using namespace NSWFL::Conversion;
using namespace NSWFL::Registry;
using namespace NSWFL::String;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------(Variable Declarations)
HWND hMainDialog = NULL;
HWND hCurrent = NULL;
HWND hHighest = NULL;
HWND hLowest = NULL;
HWND hCount = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void GetCycleCount(unsigned __int64 *i64Cycles)
{
    __asm {
		push edx
		rdtsc
		push edi
		mov edi, dword ptr i64Cycles
		mov dword ptr [edi], eax
		mov dword ptr [edi+4], edx
		pop edi
		pop edx
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned __int64 GetCycleCount(void)
{
    unsigned __int64 i64Cycles = 0;

    __asm {
        rdtsc
        lea ecx, [i64Cycles]
        mov [ecx], eax
        mov [ecx+4], edx
    }

    return i64Cycles;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *GetSymbol(double dValue, double *dDivisor)
{
	if(dValue >= 1000000000000)
	{
		*dDivisor = 1000000000000;
		return "THz"; //terahertz
	}
	else if(dValue >= 1000000000)
	{
		*dDivisor = 1000000000;
		return "GHz"; //gigahertz
	}
	else if(dValue >= 1000000)
	{
		*dDivisor = 1000000;
		return "MHz"; //megahertz
	}
	else if(dValue >= 1000)
	{
		*dDivisor = 1000;
		return "kHz"; //kilohertz
	}
	else{
		*dDivisor = 1;
		return "Hz"; // hertz
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI CPUTimer_Thread(LPVOID lpVoid)
{
	HANDLE hThread = GetCurrentThread();

	SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);

    SYSTEM_INFO SI;

    GetSystemInfo(&SI);

	unsigned __int64 iBefore = 0;
	unsigned __int64 iAfter = 0;

	char sMHZ[64];
	char sMHZFrmt[64];
	char sText[128];

	double dCurrent = -1;
	double dHighest = -1;
	double dLowest = -1;

	char *sSymbol = "";
	double dDivisor = 1;

	sprintf_s(sText, sizeof(sText), "%d Logical Cores", SI.dwNumberOfProcessors);
	if(SI.dwNumberOfProcessors > 1)
	{
		strcat_s(sText, sizeof(sText), "s");
	}
	Set_Text(hCount, sText);

	while(true)
	{
		for(DWORD dwCPU = 0; dwCPU < SI.dwNumberOfProcessors; dwCPU++)
		{
			SetThreadIdealProcessor(hThread, dwCPU);

			GetCycleCount(&iBefore);
			Sleep(1000);
			GetCycleCount(&iAfter);

			dCurrent = (double)(iAfter - iBefore);

			if(dCurrent > dHighest || dLowest == -1)
			{
				dHighest = dCurrent;
			}
			if(dCurrent < dLowest || dLowest == -1)
			{
				dLowest = dCurrent;
			}

			sSymbol = GetSymbol(dCurrent, &dDivisor);
			sprintf_s(sMHZ, sizeof(sMHZ), "%s %s\n",
				FormatDouble(sMHZFrmt, sizeof(sMHZFrmt), dCurrent / dDivisor, 2), sSymbol);
			Set_Text(hCurrent, sMHZ);

			sSymbol = GetSymbol(dHighest, &dDivisor);
			sprintf_s(sMHZ, sizeof(sMHZ), "%s %s\n",
				FormatDouble(sMHZFrmt, sizeof(sMHZFrmt), dHighest / dDivisor, 2), sSymbol);
			Set_Text(hHighest, sMHZ);

			sSymbol = GetSymbol(dLowest, &dDivisor);
			sprintf_s(sMHZ, sizeof(sMHZ), "%s %s\n",
				FormatDouble(sMHZFrmt, sizeof(sMHZFrmt), dLowest / dDivisor, 2), sSymbol);
			Set_Text(hLowest, sMHZ);

			Sleep(1);
		}

		Sleep(1);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK MainDialog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static DWORD dwBackgroundColor;
	static HWND hReportedName;
	static HWND hReportedSpeed;

    //--------------------------------------------------------------------------

    if(uMsg == WM_INITDIALOG) // Received A Initilize Dialog Message
    {
        hMainDialog = hWnd;
        dwBackgroundColor = GetSysColor(COLOR_3DFACE);

        HMENU hMenu = GetSystemMenu(hWnd, FALSE);
        AppendMenu(hMenu, MF_SEPARATOR, 0, 0);
        AppendMenu(hMenu, MF_STRING, MAINDIALOG_MENU_ABOUT, "About");

        SendMessage(hWnd, (UINT)WM_SETTEXT, (WPARAM)0, (LPARAM)gsTitleCaption);
        SendMessage(hWnd, WM_SETICON, TRUE, (LPARAM) LoadIcon(ghAppInstance, MAKEINTRESOURCE(IDI_MAIN)));

        hCurrent		= GetDlgItem(hWnd, IDC_CURRENT);
        hHighest		= GetDlgItem(hWnd, IDC_HIGHEST);
        hLowest			= GetDlgItem(hWnd, IDC_LOWEST);
        hReportedName   = GetDlgItem(hWnd, IDC_REPORTEDNAME);
        hReportedSpeed  = GetDlgItem(hWnd, IDC_REPORTEDSPEED);
        hCount			= GetDlgItem(hWnd, IDC_REPORTEDCOUNT);

		/*
		HDC hDC = GetDC(hCurrent);
		ReleaseDC(hCurrent, hDC);

        HFONT hFont = CreateFont( 16,
                    0,0,0,FW_NORMAL,0,0,0,
                    DEFAULT_CHARSET,
                    OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS,
                    DEFAULT_QUALITY,
                    DEFAULT_PITCH,
                    "Courier New"
        );

        SendMessage(hCurrent, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
        SendMessage(hHighest, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
        SendMessage(hLowest, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
		*/

		Set_Text(hCurrent, "Processing...");
		Set_Text(hHighest, "Processing...");
		Set_Text(hLowest, "Processing...");

		char sRegBuf[1024];
		DWORD dwRegBufSz = 0;
		
		memset(sRegBuf, 0, sizeof(sRegBuf));
		dwRegBufSz = sizeof(sRegBuf);
		if(Get_StringRegistryValue(HKEY_LOCAL_MACHINE,
			"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "ProcessorNameString", sRegBuf, dwRegBufSz))
		{
			Trim(sRegBuf);
			Set_Text(hReportedName, sRegBuf);
		}
		else{
			Set_Text(hReportedName, "(N/A)");
		}

		if(Get_DWORDRegistryValue(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "~MHz", dwRegBufSz))
		{
			dwRegBufSz *= 1000000;

			char sMHZ[64];
			char sMHZFrmt[64];
			double dDivisor = 1;
			char *sSymbol = GetSymbol(dwRegBufSz, &dDivisor);
			sprintf_s(sMHZ, sizeof(sMHZ), "%s %s\n",
				FormatDouble(sMHZFrmt, sizeof(sMHZFrmt), dwRegBufSz / dDivisor, 2), sSymbol);
			Set_Text(hReportedSpeed, sMHZ);
		}
		else{
			Set_Text(hReportedSpeed, "(N/A)");
		}

		CreateThread(NULL, 0, CPUTimer_Thread, NULL, 0, NULL);
		
		CenterWindow(hWnd);

        return TRUE; // Return TRUE to set the keyboard focus, Otherwise return FALSE.
    }

	//--------------------------------------------------------------------------

	else if((HANDLE)lParam == hCurrent)
	{
        return Set_Color(RGB(0,150,0), dwBackgroundColor, wParam);
	}
	else if((HANDLE)lParam == hHighest)
	{
        return Set_Color(RGB(0,0,150), dwBackgroundColor, wParam);
	}
	else if((HANDLE)lParam == hLowest)
	{
        return Set_Color(RGB(150,0,0), dwBackgroundColor, wParam);
	}

	//--------------------------------------------------------------------------

    else if(uMsg == WM_COMMAND)
    {
        if(wParam == IDC_CLOSE) //- Ok Button.
        {
            EndDialog(hWnd, 0);
            DestroyWindow(hWnd);

			hMainDialog = NULL;

            return TRUE;
        }

        return FALSE;
    }

    //--------------------------------------------------------------------------

	else if(uMsg == WM_PAINT)
    {
        HDC ThisHDC;
        PAINTSTRUCT ThisPS;

        ThisHDC = BeginPaint(hWnd, &ThisPS);

        //- Any painting should be done here.

        EndPaint(hWnd, &ThisPS);
        return TRUE;
    }

    //--------------------------------------------------------------------------

    else if(uMsg == WM_SYSCOMMAND) //- Received a system menu message.
    {
        if(LOWORD(wParam) == MAINDIALOG_MENU_ABOUT) //- About.
        {
            _AboutDialogInfo ADI;
    
            ADI.DisplayIcon  = LoadIcon(ghAppInstance, MAKEINTRESOURCE(IDI_MAIN));
            ADI.TitleCaption = gsTitleCaption;
            ADI.FileVersion  = gsFileVersion;
            ADI.BuildDate    = __DATE__;
            ADI.BuildTime    = __TIME__;
            ADI.CopyRight    = gsAppCopyRight;
            ADI.OwnerHandle  = hWnd;

            NetLogo(&ADI);
            return TRUE;
        }

        return FALSE;
    }

    //--------------------------------------------------------------------------

    else if(uMsg == WM_CLOSE) //- Received close message.
    {
        EndDialog(hWnd,0);
        DestroyWindow(hWnd);

		hMainDialog = NULL;

        return TRUE;
    }

    //--------------------------------------------------------------------------

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif


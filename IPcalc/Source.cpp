#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "resource1.h"
#include<CommCtrl.h>
#include <cstdio>
#include<iostream>


#define IDC_COLUMN_NETWORK_ADDRESS			2000
#define IDC_COLUMN_BROADCAST_ADDRESS		2001
#define IDC_COLUMN_NUMBER_OF_IP_ADDRESSED	2002
#define IDC_COLUMN_NUMBER_OF_HOSTS			2003


BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParan, LPARAM lParam);
BOOL CALLBACK DlgProcSubnets(HWND hwnd, UINT uMsg, WPARAM wParan, LPARAM lParam);
LPSTR FormatIPaddress(const char* sz_message, DWORD dwNetwork);
LPSTR FormatMessageWithNumber(const char* sz_message, DWORD number);
void InitLVColimn(LPLVCOLUMN column, LPSTR text, INT subitem);
LPSTR FormatLastError();
CHAR* FormatIPaddress(DWORD IPaddress);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmd)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}

BOOL DlgProc(HWND hwnd, UINT uMsg, WPARAM wParan, LPARAM lParam)
{
	DWORD dw_address = 0;
	DWORD dwIpMask = 0;
	const int size = 256;
	char sz_prefix[3]{};
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HWND hUpDown = GetDlgItem(hwnd, IDC_SPIN);
		SendMessage(hUpDown, UDM_SETRANGE, 0, MAKELPARAM(30, 1));
		//https://learn.microsoft.com/en-us/windows/win32/controls/udm-setrange
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		SendMessage(GetDlgItem(hwnd, IDC_IPADDRESS_MASK), IPM_GETADDRESS, 0, (LPARAM)&dwIpMask);
		std::cout << "WM_INITDIALOG:\n";
		std::cout << dwIpMask << std::endl;
	}
	break;
	case WM_COMMAND://Обрабатываются нажатия на кнопки, ввод текста и любые изменения состояния окна
	{

		switch (LOWORD(wParan))
		{
		case IDC_IPADDRESS_IP:
		{
			if (HIWORD(wParan) == EN_CHANGE)
			{
				SendMessage(GetDlgItem(hwnd, IDC_IPADDRESS_IP), IPM_GETADDRESS, 0, (LPARAM)&dw_address);
				byte first = FIRST_IPADDRESS(dw_address);
				if (first < 128)
					dwIpMask = MAKEIPADDRESS(255, 0, 0, 0);
				else if (first < 192)
					dwIpMask = MAKEIPADDRESS(255, 255, 0, 0);
				else if (first < 224)
					dwIpMask = MAKEIPADDRESS(255, 255, 255, 0);

				SendMessage(GetDlgItem(hwnd, IDC_IPADDRESS_MASK), IPM_SETADDRESS, 0, (LPARAM)dwIpMask);
			}
		}
		break;
		case IDC_IPADDRESS_MASK:
		{
			if (HIWORD(wParan) == EN_CHANGE)
			{
				SendMessage(GetDlgItem(hwnd, IDC_IPADDRESS_MASK), IPM_GETADDRESS, 0, (LPARAM)&dwIpMask);
				int i = 1;
				for (; dwIpMask <<= 1; i++);
				sprintf_s(sz_prefix, "%i", i);
				SendMessage(GetDlgItem(hwnd, IDC_EDIT_PREFIC), WM_SETTEXT, 0, (LPARAM)sz_prefix);
			}
		}
		break;
		case IDC_EDIT_PREFIC:
		{
			if (HIWORD(wParan) == EN_CHANGE)
			{
				SendMessage(GetDlgItem(hwnd, IDC_EDIT_PREFIC), WM_GETTEXT, 3, (LPARAM)sz_prefix);
				DWORD dwIPprefix = atoi(sz_prefix);
				UINT dwIPmask = UINT_MAX;
				dwIPmask <<= (32 - dwIPprefix);
				std::cout << "IDC_EDIT_PREFIX:\n";
				std::cout << std::hex << dwIPmask << "\n";
				if (dwIPprefix != 0)
					SendMessage(GetDlgItem(hwnd, IDC_IPADDRESS_MASK), IPM_SETADDRESS, 0, dwIPmask);
			}
		}
		break;
		case IDOK:
		{
			char sz_info[size] = "Info:\n infa";
			//char sz_network_address[size]{};
			//char sz_broadcast_address[size]{};
			char sz_number_of_hosts[size]{};
			SendMessage(GetDlgItem(hwnd, IDC_IPADDRESS_IP), IPM_GETADDRESS, 0, (LPARAM)&dw_address);
			SendMessage(GetDlgItem(hwnd, IDC_IPADDRESS_MASK), IPM_GETADDRESS, 0, (LPARAM)&dwIpMask);
			DWORD dwNetwork = dw_address & dwIpMask;
			DWORD dwBroadcastAddress = dwNetwork | ~dwIpMask;
			strcat(sz_info, FormatIPaddress("Net Address ", dwNetwork));
			strcat(sz_info, FormatIPaddress("Broadcast address ", dwBroadcastAddress));
			strcat(sz_info, FormatMessageWithNumber("Count IP-address: ", dwBroadcastAddress - dwNetwork + 1));
			strcat(sz_info, FormatMessageWithNumber("Count host: ", dwBroadcastAddress - dwNetwork - 1));
			//strcat(sz_info, "\nCount IP-address: ");
			//strcat(sz_info, _itoa(dwBroadcastAddress - dwNetwork+1, sz_number_of_hosts, 10));

			SendMessage(GetDlgItem(hwnd, IDC_STATIC_INFO), WM_SETTEXT, 0, (LPARAM)sz_info);

		}
		break;
		case IDC_BUTTON_SUBNETS:
		{
			DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_SUBNETS), hwnd, (DLGPROC)DlgProcSubnets, 0);
		}
		break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		}
	}
	break;

	case WM_CLOSE:
		FreeConsole();
		EndDialog(hwnd, 0);
		break;
	}
	return false;
}

BOOL DlgProcSubnets(HWND hwnd, UINT uMsg, WPARAM wParan, LPARAM lParam)
{
	static HWND hList = GetDlgItem(hwnd, IDC_LIST_SUBNETS);
	static LVCOLUMN lvcNetworkAddress;
	static LVCOLUMN lvcBroadCastAddress;
	static LVCOLUMN lvcNumberOfIpAddress;
	static LVCOLUMN lvcNumberOfHosts;

	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		hList = GetDlgItem(hwnd, IDC_LIST_SUBNETS);
		InitLVColimn(&lvcNetworkAddress, (LPSTR)"Addres net", 0); //IDC_COLUMN_NETWORK_ADDRESS);		
		InitLVColimn(&lvcBroadCastAddress, (LPSTR)"Broadcast", 1); //IDC_COLUMN_BROADCAST_ADDRESS);		
		InitLVColimn(&lvcNumberOfIpAddress, (LPSTR)"Number of IP-addres", 2); //IDC_COLUMN_NUMBER_OF_IP_ADDRESSED);	
		InitLVColimn(&lvcNumberOfHosts, (LPSTR)"Number of hosts", 3); //IDC_COLUMN_NUMBER_OF_HOSTS);		
		ListView_InsertColumn(hList, 0, &lvcNetworkAddress);
		ListView_InsertColumn(hList, 1, &lvcBroadCastAddress);
		ListView_InsertColumn(hList, 2, &lvcNumberOfIpAddress);
		ListView_InsertColumn(hList, 3, &lvcNumberOfHosts);
		//SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&lvcNetworkAddress);
		//SendMessage(hList, LVM_INSERTCOLUMN, 1, (LPARAM)&lvcBroadCastAddress);
		//SendMessage(hList, LVM_INSERTCOLUMN, 2, (LPARAM)&lvcNumberOfIpAddress);
		//SendMessage(hList, LVM_INSERTCOLUMN, 3, (LPARAM)&lvcNumberOfHosts);

		HWND hParent = GetParent(hwnd);
		HWND hIpAddres = GetDlgItem(hParent, IDC_IPADDRESS_IP);
		HWND hPrefix = GetDlgItem(hParent, IDC_EDIT_PREFIC);
		DWORD dwIPaddress;
		char sz_prefix[16];
		char sz_class[2];
		SendMessage(hIpAddres, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
		SendMessage(hPrefix, WM_GETTEXT, 16, (LPARAM)sz_prefix);
		DWORD dwPrefix = atoi(sz_prefix);
		if (dwIPaddress && dwPrefix)
		{
			DWORD dwDefoltPrefix = 0;
			if (FIRST_IPADDRESS(dwIPaddress) < 128)
			{
				strcpy(sz_class, "A");
				dwDefoltPrefix = 8;
			}
			else if (FIRST_IPADDRESS(dwIPaddress) < 192)
			{
				strcpy(sz_class, "B");
				dwDefoltPrefix = 16;
			}
			else if (FIRST_IPADDRESS(dwIPaddress) < 224)
			{
				strcpy(sz_class, "C");
				dwDefoltPrefix = 24;
			}
			DWORD dwSubnetsNumber = 1;
			DWORD dwDelta = dwPrefix - dwDefoltPrefix;
			for (;dwDelta > 0;dwDelta--)
				dwSubnetsNumber *= 2;
			if (dwSubnetsNumber == 1)
				dwSubnetsNumber = 0;
			std::cout << dwDelta << std::endl;
			std::cout << dwSubnetsNumber << std::endl;

			char sz_message[256]{};
			DWORD dwHostBits = 32 - dwPrefix;
			DWORD dwNetorkCapaciti = pow(2, dwHostBits);

			sprintf(sz_message, "Net class '%s' for %i subnets, %i IP-address", sz_class, dwSubnetsNumber, dwNetorkCapaciti);
			SendMessage(GetDlgItem(hwnd, IDC_STATIC_NUMBER_OF_SUBNETS), WM_SETTEXT, 0, (LPARAM)sz_message);

			for (DWORD i = 0, dwNetworkAddress = dwIPaddress; i < dwSubnetsNumber; i++, dwNetworkAddress += dwNetorkCapaciti)
			{
				CHAR szNetworkAddress[256];
				strcpy(szNetworkAddress, FormatIPaddress(dwNetworkAddress));
				LVITEM lvi;
				memset(&lvi, 0, sizeof(lvi));
				lvi.mask = LVIF_TEXT;   // Text Style
				lvi.iItem = i;      // choose item  
				ListView_InsertItem(hList, &lvi);
				ListView_SetItemText(hList, i, 0, (LPSTR)szNetworkAddress);
				ListView_SetItemText(hList, i, 1, (LPSTR)"Text test - 1");
				ListView_SetItemText(hList, i, 2, (LPSTR)"Text test - 2");
				ListView_SetItemText(hList, i, 3, (LPSTR)"Text test - 3");			
			}

		}

	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParan))
		{
		case IDOK:
			break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		default:
			break;
		}
	}
	break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	default:
		break;
	}
	return false;
}

void InitLVColimn(LPLVCOLUMN column, LPSTR text, INT subitem)
{
	ZeroMemory(column, sizeof(LVCOLUMN));
	column->mask = LVCF_WIDTH | LVCF_TEXT | LVCF_FMT | LVCF_SUBITEM;
	column->cx = 150;
	column->pszText = text;
	column->iSubItem = 5;
	column->fmt = LVCFMT_LEFT;
}

LPSTR FormatIPaddress(const char* sz_message, DWORD dwNetwork)
{
	char sz_buffer[256]{};
	sprintf(sz_buffer, "%s:\t %i.%i.%i.%i;\n", sz_message, FIRST_IPADDRESS(dwNetwork), SECOND_IPADDRESS(dwNetwork), THIRD_IPADDRESS(dwNetwork), FOURTH_IPADDRESS(dwNetwork));
	return sz_buffer;
}
LPSTR FormatMessageWithNumber(const char* sz_message, DWORD number)
{
	char sz_buffer[256]{};
	sprintf(sz_buffer, "%s:\t %i;\n ", sz_message, number);
	return sz_buffer;
}
LPSTR FormatLastError()
{
	DWORD dwErrorMessageID = GetLastError();
	LPSTR lpszMessageBuffer = NULL;
	DWORD dwSize = FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErrorMessageID,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_ENGLISH_US),
		(LPSTR)&lpszMessageBuffer,
		0,
		NULL
	);
	return lpszMessageBuffer;
}

CHAR* FormatIPaddress(DWORD IPaddress)
{
	CHAR sz_buffer[256]{};
	sprintf
	(
		sz_buffer,
		"%i.%i.%i.%i",
		FIRST_IPADDRESS(IPaddress),
		SECOND_IPADDRESS(IPaddress),
		THIRD_IPADDRESS(IPaddress),
		FOURTH_IPADDRESS(IPaddress)
	);
	return sz_buffer;
}
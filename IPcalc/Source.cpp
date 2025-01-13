#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "resource1.h"
#include<CommCtrl.h>
#include <cstdio>
#include<iostream>

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParan, LPARAM lParam);
LPSTR FormatIPaddress(const char* sz_message, DWORD dwNetwork);
LPSTR FormatMessageWithNumber(const char* sz_message, DWORD number);
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
		SendMessage(GetDlgItem(hwnd,IDC_IPADDRESS_MASK), IPM_GETADDRESS, 0, (LPARAM)&dwIpMask);
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

LPSTR FormatIPaddress(const char* sz_message, DWORD dwNetwork)
{
	char sz_buffer[256]{};
	sprintf(sz_buffer, "%s:\t %i.%i.%i.%i;\n", sz_message, FIRST_IPADDRESS(dwNetwork), SECOND_IPADDRESS(dwNetwork), THIRD_IPADDRESS(dwNetwork), FOURTH_IPADDRESS(dwNetwork));
	return sz_buffer; 
}
LPSTR FormatMessageWithNumber(const char* sz_message, DWORD number)
{
	char sz_buffer[256]{};
	sprintf(sz_buffer, "%s:\t %i;\n ",sz_message, number);
	return sz_buffer;
}
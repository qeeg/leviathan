#include "TraceView.h"
#include "resource.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "WindowsControls/WindowsControls.pkg"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
TraceView::TraceView(IUIManager& auiManager, TraceViewPresenter& apresenter, IProcessor& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	hwndDataGrid = NULL;
	hwndControlPanel = NULL;
	hfontHeader = NULL;
	hfontData = NULL;
	logLastModifiedToken = 0;
	SetWindowSettings(apresenter.GetUnqualifiedViewTitle(), 0, 0, 300, 500);
	SetDockableViewType(true, DockPos::Right);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT TraceView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementGiveFocusToChildWindowOnClick(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_CREATE:
		return msgWM_CREATE(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgWM_SIZE(hwnd, wparam, lparam);
	case WM_PAINT:
		return msgWM_PAINT(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
LRESULT TraceView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Register the DataGrid window class
	WC_DataGrid::RegisterWindowClass(GetAssemblyHandle());

	//Create the DataGrid child control
	hwndDataGrid = CreateWindowEx(WS_EX_CLIENTEDGE, WC_DataGrid::windowClassName, L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, 0, 0, 0, 0, hwnd, (HMENU)CTL_DATAGRID, GetAssemblyHandle(), NULL);

	//Insert our columns into the DataGrid control
	WC_DataGrid::Grid_InsertColumn addressColumn(L"Address", COLUMN_ADDRESS);
	WC_DataGrid::Grid_InsertColumn disassemblyColumn(L"Disassembly", COLUMN_DISASSEMBLY);
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&addressColumn);
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&disassemblyColumn);

	//Create the dialog control panel
	hwndControlPanel = CreateDialogParam(GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_TRACE_PANEL), hwnd, WndProcPanelStatic, (LPARAM)this);
	ShowWindow(hwndControlPanel, SW_SHOWNORMAL);
	UpdateWindow(hwndControlPanel);

	//Obtain the correct metrics for our custom font object
	int fontPointSize = 8;
	HDC hdc = GetDC(hwnd);
	int fontnHeight = -MulDiv(fontPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(hwnd, hdc);

	//Create the font for the header in the grid control
	std::wstring headerFontTypefaceName = L"MS Shell Dlg";
	hfontHeader = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &headerFontTypefaceName[0]);

	//Set the header font for the grid control
	SendMessage(hwndDataGrid, WM_SETFONT, (WPARAM)hfontHeader, (LPARAM)TRUE);

	//Create the font for the data region in the grid control
	std::wstring dataFontTypefaceName = L"Courier New";
	hfontData = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &dataFontTypefaceName[0]);

	//Set the data region font for the grid control
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::SetDataAreaFont, (WPARAM)hfontData, (LPARAM)TRUE);

	//Create a timer to trigger updates to the grid
	SetTimer(hwnd, 1, 200, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT TraceView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Delete our custom font objects
	SendMessage(hwndDataGrid, WM_SETFONT, (WPARAM)NULL, (LPARAM)FALSE);
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::SetDataAreaFont, (WPARAM)NULL, (LPARAM)FALSE);
	DeleteObject(hfontHeader);
	DeleteObject(hfontData);

	KillTimer(hwnd, 1);

	return DefWindowProc(hwnd, WM_DESTROY, wparam, lparam);
}

//----------------------------------------------------------------------------------------
LRESULT TraceView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Update the control panel
	SendMessage(hwndControlPanel, WM_TIMER, wparam, lparam);

	//If the trace log hasn't changed since the last refresh, abort any further
	//processing.
	unsigned int newLogLastModifiedToken = model.GetTraceLogLastModifiedToken();
	if(newLogLastModifiedToken == logLastModifiedToken)
	{
		return 0;
	}
	logLastModifiedToken = newLogLastModifiedToken;

	//Retrieve the latest trace log
	std::list<IProcessor::TraceLogEntry> traceList = model.GetTraceLog();

	//Delete any extra rows from the data grid that are no longer required
	unsigned int currentRowCount = (unsigned int)SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::GetRowCount, 0, 0);
	if((unsigned int)traceList.size() < currentRowCount)
	{
		unsigned int rowCountToRemove = currentRowCount - (unsigned int)traceList.size();
		WC_DataGrid::Grid_DeleteRows deleteRowsInfo;
		deleteRowsInfo.targetRowNo = currentRowCount - rowCountToRemove;
		deleteRowsInfo.rowCount = rowCountToRemove;
		SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::DeleteRows, 0, (LPARAM)&deleteRowsInfo);
	}

	//Update the data grid with the latest text
	std::map<unsigned int, std::map<unsigned int, std::wstring>> rowText;
	unsigned int pcLength = model.GetPCCharWidth();
	unsigned int currentRow = 0;
	for(std::list<IProcessor::TraceLogEntry>::const_iterator i = traceList.begin(); i != traceList.end(); ++i)
	{
		const IProcessor::TraceLogEntry& entry = *i;
		std::map<unsigned int, std::wstring>& columnText = rowText[currentRow++];
		std::wstring addressString;
		IntToStringBase16(entry.address, addressString, pcLength);
		columnText[COLUMN_ADDRESS] = addressString;
		columnText[COLUMN_DISASSEMBLY] = entry.disassembly;
	}
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::UpdateMultipleRowText, 0, (LPARAM)&rowText);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT TraceView::msgWM_SIZE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Read the new client size of the window
	RECT rect;
	GetClientRect(hwnd, &rect);
	int controlWidth = rect.right;
	int controlHeight = rect.bottom;
	GetClientRect(hwndControlPanel, &rect);
	int controlPanelWidth = rect.right;
	int controlPanelHeight = rect.bottom;

	//Global parameters defining how child windows are positioned
	int borderSize = 4;

	//Calculate the new position of the control panel
	int controlPanelPosX = borderSize;
	int controlPanelPosY = controlHeight - (borderSize + controlPanelHeight);
	MoveWindow(hwndControlPanel, controlPanelPosX, controlPanelPosY, controlPanelWidth, controlPanelHeight, TRUE);

	//Calculate the new size and position of the list
	int listBoxWidth = controlWidth - (borderSize * 2);
	int listBoxPosX = borderSize;
	int listBoxHeight = controlHeight - ((borderSize * 2) + controlPanelHeight);
	int listBoxPosY = borderSize;
	MoveWindow(hwndDataGrid, listBoxPosX, listBoxPosY, listBoxWidth, listBoxHeight, TRUE);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT TraceView::msgWM_PAINT(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Fill the background of the control with the dialog background colour
	HDC hdc = GetDC(hwnd);
	HBRUSH hbrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	HBRUSH hbrushOld = (HBRUSH)SelectObject(hdc, hbrush);

	RECT rect;
	GetClientRect(hwnd, &rect);
	FillRect(hdc, &rect, hbrush);

	SelectObject(hdc, hbrushOld);
	DeleteObject(hbrush);
	ReleaseDC(hwnd, hdc);

	return DefWindowProc(hwnd, WM_PAINT, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Panel dialog window procedure
//----------------------------------------------------------------------------------------
INT_PTR CALLBACK TraceView::WndProcPanelStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	TraceView* state = (TraceView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_INITDIALOG:
		//Set the object pointer
		state = (TraceView*)lparam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Pass this message on to the member window procedure function
		if(state != 0)
		{
			return state->WndProcPanel(hwnd, msg, wparam, lparam);
		}
		break;
	case WM_DESTROY:
		if(state != 0)
		{
			//Pass this message on to the member window procedure function
			INT_PTR result = state->WndProcPanel(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);

			//Return the result from processing the message
			return result;
		}
		break;
	}

	//Pass this message on to the member window procedure function
	INT_PTR result = FALSE;
	if(state != 0)
	{
		result = state->WndProcPanel(hwnd, msg, wparam, lparam);
	}
	return result;
}

//----------------------------------------------------------------------------------------
INT_PTR TraceView::WndProcPanel(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgPanelWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgPanelWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgPanelWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Panel dialog event handlers
//----------------------------------------------------------------------------------------
INT_PTR TraceView::msgPanelWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR TraceView::msgPanelWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;
	CheckDlgButton(hwnd, IDC_PROCESSOR_TRACE_ENABLED, (model.GetTraceEnabled())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_TRACE_DISASSEMBLE, (model.GetTraceDisassemble())? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_PROCESSOR_TRACE_LENGTH) UpdateDlgItemBin(hwnd, IDC_PROCESSOR_TRACE_LENGTH, model.GetTraceLength());

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR TraceView::msgPanelWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
	}
	else if((HIWORD(wparam) == EN_KILLFOCUS) && initializedDialog)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(newText != previousText)
		{
			switch(LOWORD(wparam))
			{
			case IDC_PROCESSOR_TRACE_LENGTH:
				model.SetTraceLength(GetDlgItemBin(hwnd, LOWORD(wparam)));
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_TRACE_ENABLED:{
			model.SetTraceEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_PROCESSOR_TRACE_DISASSEMBLE:{
			model.SetTraceDisassemble(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_PROCESSOR_TRACE_CLEAR:{
			model.ClearTraceLog();
			break;}
		}
	}

	return TRUE;
}

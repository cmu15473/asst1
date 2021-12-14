#include "WinForm.h"
#include "WinApp.h"
#include "WinButtons.h"
#include "WinMessage.h"
#include "gl/gl.h"
namespace CoreLib
{
	namespace WinForm
	{
		LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
		
			bool processed = false;
			if (message == WM_DESTROY && hWnd == Application::GetMainFormHandle())
			{
				Application::Terminate();
				PostQuitMessage(0);
				return 0;
			}
			else
				return DefWindowProc(hWnd, message, wParam, lParam);
			//else
			//{
			//	Component * comp;
			//	if (message != WM_DESTROY && (comp = Application::GetComponent(hWnd)))
			//		processed = comp->ProcessMessage(WinMessage(hWnd, message, wParam, lParam));
			//	if (!processed)
			//	{
			//		return DefWindowProc(hWnd, message, wParam, lParam);
			//	}
			//	else
			//		return 0;
			//}
			return 0;
		}

		BaseForm::BaseForm()
		{
			menu = 0;
			defButton = cancelButton = 0;
			ownerDraw = true;
			closed = true;
			firstShown = true;
			dlgResult = Cancel;
			accelTable = new AccelTable();
			dlgOwner = 0;
		}

		BaseForm::~BaseForm()
		{
			UnSubClass();
			if (menu)
			{
				SetMenu(handle, 0);
				delete menu;
				menu = 0;
			}
			Destroy();
			Application::UnRegisterComponent(this);
		}

		BaseForm::DialogResult BaseForm::ShowModal(BaseForm * owner)
		{
			MSG msg;
			dlgOwner = owner;
			dlgOwner->SetEnabled(false);
			dlgResult = BaseForm::Cancel;
			if (firstShown)
			{
				int l = (owner->GetWidth()-GetWidth())/2;
				int t = (owner->GetHeight()-GetHeight())/2;
				SetLeft(l+owner->GetLeft());
				SetTop(t+owner->GetTop());
				firstShown = false;
			}
			Show();

			while (GetMessage(&msg, NULL, 0,0)>0)
			{
				HWND hwnd = GetActiveWindow();
				if (!accelTable || !TranslateAccelerator(hwnd, 
					accelTable->GetHandle(), &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				if (closed)
					break;
			}
			dlgOwner->SetEnabled(true);
			//SetActiveWindow(dlgOwner->GetHandle());
			dlgOwner = NULL;
			return dlgResult;
		}

		void BaseForm::SetDialogResult(DialogResult result)
		{
			dlgResult = result;
			Close();
			closed = true;
		}

		void BaseForm::SetDefaultButton(Button * btn)
		{
			ApplyActionToComponentAndChildren(&ClearDefaultButtonAction());
			defButton = btn;
			btn->SetDefault(true);
		}

		Button * BaseForm::GetDefaultButton()
		{
			return defButton;
		}

		void BaseForm::SetMainMenu(MainMenu * _m)
		{
			menu = _m;
			SetMenu(handle, _m->GetHandle());
		}

		void BaseForm::Close()
		{
			WindowCloseEventArgs args;
			args.Cancel = false;
			OnClose.Invoke(this, args);
			if (!args.Cancel)
			{
				closed = true;
				if (dlgOwner)
				{
					dlgOwner->SetEnabled(true);
				}
				SetVisible(false);
				if (Application::GetMainFormHandle() == handle)
					Application::Terminate();
			}
		}

		void BaseForm::Show()
		{
			ShowWindow(handle, SW_SHOW);
			UpdateWindow(handle);
			closed = false;
		}

		int BaseForm::MessageBox(const String & msg, const String & title, unsigned int style)
		{
			return ::MessageBoxW(handle, msg.Buffer(), title.Buffer(), style);
		}

		void BaseForm::SetText(String text)
		{
			SetWindowTextW(handle, text.Buffer());
		}

		String BaseForm::GetText()
		{
			const int buflen = 1024;
			wchar_t buf[buflen];
			memset(buf,0,sizeof(wchar_t) * buflen);
			GetWindowTextW(handle, buf, buflen);
			String rs = buf;
			return rs;
		}

		void BaseForm::SetBorder(FormBorder border)
		{
			switch (border)
			{
			case fbNone:
				{
					SetStyle(GWL_STYLE, WS_BORDER, false);
					SetStyle(GWL_STYLE, WS_CAPTION, false);
					SetStyle(GWL_STYLE, WS_SIZEBOX, false);
					SetStyle(GWL_EXSTYLE, WS_EX_TOOLWINDOW, false);
				}
				break;
			case fbSingle:
				{
					SetStyle(GWL_STYLE, WS_BORDER, true);
					SetStyle(GWL_STYLE, WS_CAPTION, true);
					SetStyle(GWL_STYLE, WS_SIZEBOX, false);
					SetStyle(GWL_EXSTYLE, WS_EX_TOOLWINDOW, false);
				}
				break;
			case fbSizeable:
				{
					SetStyle(GWL_STYLE, WS_BORDER, true);
					SetStyle(GWL_STYLE, WS_CAPTION, true);
					SetStyle(GWL_STYLE, WS_SIZEBOX, true);
					SetStyle(GWL_EXSTYLE, WS_EX_TOOLWINDOW, false);
				}
				break;
			case fbFixedDialog:
				{
					SetStyle(GWL_STYLE, WS_BORDER, false);
					SetStyle(GWL_STYLE, WS_CAPTION, true);
					SetStyle(GWL_STYLE, WS_SIZEBOX, false);
					SetStyle(GWL_EXSTYLE, WS_EX_TOOLWINDOW, false);
				}
				break;
			case fbToolWindow:
				{
					SetStyle(GWL_STYLE, WS_BORDER, true);
					SetStyle(GWL_STYLE, WS_CAPTION, true);
					SetStyle(GWL_STYLE, WS_SIZEBOX, true);
					SetStyle(GWL_EXSTYLE, WS_EX_TOOLWINDOW, true);
				}
				break;
			case fbFixedToolWindow:
				{
					SetStyle(GWL_STYLE, WS_BORDER, true);
					SetStyle(GWL_STYLE, WS_CAPTION, true);
					SetStyle(GWL_STYLE, WS_SIZEBOX, false);
					SetStyle(GWL_EXSTYLE, WS_EX_TOOLWINDOW, true);
				}
				break;
			}
			RECT rect;
			GetWindowRect(handle ,&rect);
			MoveWindow(handle,rect.left,rect.top,rect.right-rect.left+1 ,rect.bottom-rect.top,true); 
			MoveWindow(handle,rect.left,rect.top,rect.right-rect.left ,rect.bottom-rect.top,true);
		}

		FormBorder BaseForm::GetBorder()
		{
			LONG_PTR style = GetWindowLongPtr(handle, GWL_STYLE);
			if (style == 0)
				return fbNone;
			else if (((style & (WS_SIZEBOX | WS_CAPTION))) == 
				(WS_SIZEBOX | WS_CAPTION) )
				return fbSizeable;
			else if ((style & (WS_BORDER | WS_CAPTION)) == (WS_BORDER | WS_CAPTION))
				return fbSingle;
			else if ((style & WS_POPUP) == WS_POPUP)
				return fbFixedDialog;
			else
			{
				LONG_PTR exstyle = GetWindowLongPtr(handle, GWL_EXSTYLE);
				if ((exstyle & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW)
				{
					if ((style & WS_SIZEBOX) == WS_SIZEBOX)
						return fbToolWindow;
					else
						return fbFixedToolWindow;
				}
			}
			return fbNone;
		}

		bool BaseForm::ProcessMessage(WinMessage msg)
		{
			bool rs = BaseControl::ProcessMessage(msg);
			switch (msg.message)
			{
			case WM_CLOSE:
				{
					WindowCloseEventArgs args;
					args.Cancel = false;
					OnClose.Invoke(this, args);
					if (!args.Cancel)
					{
						closed = true;
						SetVisible(false);
						if (Application::GetMainFormHandle() == handle)
						{
							Application::Terminate();
						}
					}
				}
				return true;
			case WM_SHOWWINDOW:
				{
					EventArgs e;
					if (msg.wParam == TRUE)
					{
						OnShow.Invoke(this, e);
						UpdateAccel();
					}
				}
				return false;
			case WM_COMMAND:
				{
					Object * obj = Application::GetObject(LOWORD(msg.wParam));
					MenuItem * mn = dynamic_cast<MenuItem *>(obj);
					if (mn)
					{
						mn->Clicked();
					}
				}
				return false;
			case WM_INITMENUPOPUP:
				{
					Object * obj = Application::GetComponent((HWND)msg.wParam);
					CustomMenu * mn = dynamic_cast<CustomMenu *>(obj);
					if (mn)
					{
						mn->OnPopup();
					}
				}
				return false;
			case WM_ACTIVATE:
				UpdateAccel();
				return false;
			default:
				return false;
			}

			return false;
		}

		void BaseForm::UpdateAccel()
		{
			accelTable->Update();
			Application::SetAccel(accelTable.operator ->());
		}

		void BaseForm::SetShowInTaskBar(bool val)
		{
			SetStyle(GWL_EXSTYLE, WS_EX_APPWINDOW, val);
		}

		bool BaseForm::GetShowInTaskBar()
		{
			return GetStyle(GWL_EXSTYLE, WS_EX_APPWINDOW);
		}

		bool BaseForm::GetControlBox()
		{
			return GetStyle(GWL_STYLE, WS_SYSMENU);
		}
		void BaseForm::SetControlBox(bool val)
		{
			SetStyle(GWL_STYLE,WS_SYSMENU, val);
		}
		bool BaseForm::GetTopMost()
		{
			return GetStyle(GWL_EXSTYLE, WS_EX_TOPMOST);
		}
		void BaseForm::SetTopMost(bool val)
		{
			SetStyle(GWL_EXSTYLE, WS_EX_TOPMOST, val);
		}
		bool BaseForm::GetMinimizeBox()
		{
			return GetStyle(GWL_STYLE, WS_MINIMIZEBOX);
		}
		void BaseForm::SetMinimizeBox(bool val)
		{
			SetStyle(GWL_STYLE, WS_MINIMIZEBOX, val);
		}
		bool BaseForm::GetMaximizeBox()
		{
			return GetStyle(GWL_STYLE, WS_MAXIMIZEBOX);
		}
		void BaseForm::SetMaximizeBox(bool val)
		{
			SetStyle(GWL_STYLE, WS_MAXIMIZEBOX, val);
		}
		void BaseForm::SetClientHeight(int h)
		{
			RECT cr;
			GetClientRect(handle, &cr);
			cr.bottom = cr.top + h;
			RECT wr;
			GetWindowRect(handle, &wr);
			AdjustWindowRect(&cr, GetWindowLongPtr(handle, GWL_STYLE), this->menu!=0);
			MoveWindow(handle, wr.left, wr.top, cr.right-cr.left, cr.bottom-cr.top, 1);
		}
		void BaseForm::RegisterAccel(Accelerator acc, MenuItem * item)
		{
			accelTable->RegisterAccel(acc, item);
		}
		void BaseForm::SetClientWidth(int w)
		{
			RECT cr;
			GetClientRect(handle, &cr);
			cr.right = cr.left + w;
			RECT wr;
			GetWindowRect(handle, &wr);
			AdjustWindowRect(&cr, GetWindowLongPtr(handle, GWL_STYLE), this->menu!=0);
			MoveWindow(handle, wr.left, wr.top, cr.right-cr.left, cr.bottom-cr.top, 1);
		}
		int BaseForm::GetClientHeight()
		{
			RECT cr;
			GetClientRect(handle, &cr);
			return cr.bottom - cr.top;
		}
		int BaseForm::GetClientWidth()
		{
			RECT cr;
			GetClientRect(handle, &cr);
			return cr.right - cr.left;
		}

		void BaseForm::SetCancelButton(Button * btn)
		{
			cancelButton = btn;
		}

		Button * BaseForm::GetCancelButton()
		{
			return cancelButton;
		}

		Form::Form()
		{
			Create();
		
		}

		Form::~Form()
		{
			Application::UnRegisterComponent(this);
		}

		void Form::Create()
		{
			handle = CreateWindowW(Application::FormClassName, 0, WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, Application::GetHandle(), NULL);
			if (!handle)
			{
				throw "Failed to create window.";
			}
			Application::RegisterComponent(this);
			SubClass();
		}
	}
}
#include "WinCtrls.h"
#include "WinMessage.h"
#include <windowsx.h>
#include "WinApp.h"
#include "commctrl.h"
#include "WinForm.h"
namespace CoreLib
{
	namespace WinForm
	{
		LRESULT CALLBACK SubClassWndProc(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,
			DWORD_PTR dwRefData)
		{
			int rs = 0;
			Component * comp = 0;
			if (message == WM_COMMAND)
			{
				if (lParam != 0)
				{
					comp = Application::GetComponent((HWND)lParam);
					WinNotification note;
					note.code = HIWORD(wParam);
					note.hWnd = (HWND)lParam;
					if (comp)
						rs = comp->ProcessNotification(note)?0:1;
				}
			}
			else if (message == WM_NOTIFY)
			{
				WinNotification note;
				note.code = ((LPNMHDR)lParam)->code;
				note.hWnd = ((LPNMHDR)lParam)->hwndFrom;
				comp = Application::GetComponent(note.hWnd);
				if (comp)
					rs = comp->ProcessNotification(note)?0:1;
			}
			WinMessage msg(hWnd, message, wParam, lParam);
			comp = Application::GetComponent(hWnd);
			if (comp)
				rs = comp->ProcessMessage(msg)?1:0;
			// Call Original WndProc
			if (!rs)
				return DefSubclassProc(hWnd, message, wParam, lParam);
			else
				return 0;
		}

		Component::Component()
		{
			handle = 0;
		}

		BaseForm* Component::GetOwnerForm()
		{
			HWND chandle = handle;
			while (chandle)
			{
				Component * comp = Application::GetComponent(chandle);
				if (dynamic_cast<BaseForm*>(comp))
					return (BaseForm*)comp;
				else
					chandle = ::GetParent(chandle);
			}
			return 0;
		}

		void Component::BroadcastMessage(WinMessage msg)
		{
			ProcessMessage(msg);
			for (int i=0; i<children.Count(); i++)
				children[i]->BroadcastMessage(msg);
		}

		void Component::AddChild(Component * comp)
		{
			children.Add(comp);
			comp->SetParent(this);
		}

		void Component::SubClass()
		{
			SetWindowSubclass(handle, SubClassWndProc, 0, (DWORD_PTR)this);
		}

		void Component::UnSubClass()
		{
			RemoveWindowSubclass(handle, SubClassWndProc, 0);
		}

		void Component::Destroy()
		{

		}

		bool Component::ProcessNotification(WinNotification note)
		{
			return false;
		}

		bool Component::ProcessMessage(WinMessage msg)
		{
			UserMessageArgs args;
			if (msg.message >= WM_USER)
			{
				args.Message = msg.message;
				args.LParam = msg.lParam;
				args.WParam = msg.wParam;
				OnUserEvent.Invoke(this, args);
				return true;
			}
			return false;
		}

		HWND Component::GetHandle() const
		{
			return handle;
		}

		void Component::ApplyActionToComponentAndChildren(Action * act)
		{
			act->Apply(this);
			for (int i=0; i<children.Count(); i++)
			{
				children[i]->ApplyActionToComponentAndChildren(act);
			}
		}

		void Component::SetParent(const Component * Parent)
		{
			::SetParent(handle, Parent->GetHandle());
		}

		Component * Component::GetParent()
		{
			HWND p = ::GetParent(handle);
			if (p)
			{
				return Application::GetComponent(p);
			}
			else
				return 0;
		}

		int Component::GetChildrenCount()
		{
			return children.Count();
		}

		Component * Component::GetChildren(int id)
		{
			return children[id].operator->();
		}

		bool BaseControl::ProcessMessage(WinMessage msg)
		{
			bool rs = Component::ProcessMessage(msg);
			switch (msg.message)
			{
			case WM_LBUTTONUP:
				{
					int mx = GET_X_LPARAM(msg.lParam);
					int my = GET_Y_LPARAM(msg.lParam);
					OnMouseUp.Invoke(this, MouseEventArgs(mbLeft, mx, my,0, GET_KEYSTATE_WPARAM(msg.wParam)));
					RECT rect;
					GetWindowRect(handle, &rect);
					if (captureClick && mouseDown && mx >= 0 && mx <= rect.right-rect.left && my >= 0 && my <= rect.bottom - rect.top)
					{
						OnClick.Invoke(this, EventArgs());
					}
					mouseDown = false;
				}
				return true;
			case WM_MBUTTONUP:
				{
					int mx = GET_X_LPARAM(msg.lParam);
					int my = GET_Y_LPARAM(msg.lParam);
					OnMouseUp.Invoke(this, MouseEventArgs(mbMiddle, mx, my,0, GET_KEYSTATE_WPARAM(msg.wParam)));
					RECT rect;
					GetWindowRect(handle, &rect);
					if (captureClick && mouseDown && mx >= 0 && mx <= rect.right-rect.left && my >= 0 && my <= rect.bottom - rect.top)
					{
						OnClick.Invoke(this, EventArgs());
					}
					mouseDown = false;
				}
				return true;
			case WM_RBUTTONUP:
				{
					int mx = GET_X_LPARAM(msg.lParam);
					int my = GET_Y_LPARAM(msg.lParam);
					OnMouseUp.Invoke(this, MouseEventArgs(mbRight, mx, my,0, GET_KEYSTATE_WPARAM(msg.wParam)));
					RECT rect;
					GetWindowRect(handle, &rect);
					if (captureClick && mouseDown && mx >= 0 && mx <= rect.right-rect.left && my >= 0 && my <= rect.bottom - rect.top)
					{
						OnClick.Invoke(this, EventArgs());
					}
					mouseDown = false;
				}
				return true;
			case WM_LBUTTONDOWN:
				{
					int mx = GET_X_LPARAM(msg.lParam);
					int my = GET_Y_LPARAM(msg.lParam);
					mouseDown = true;
					OnMouseDown.Invoke(this, MouseEventArgs(mbLeft, mx, my, 0, GET_KEYSTATE_WPARAM(msg.wParam)));
				}
				return true;
			case WM_MBUTTONDOWN:
				{
					int mx = GET_X_LPARAM(msg.lParam);
					int my = GET_Y_LPARAM(msg.lParam);
					mouseDown = true;
					OnMouseDown.Invoke(this, MouseEventArgs(mbMiddle, mx, my, 0, GET_KEYSTATE_WPARAM(msg.wParam)));
				}
				return true;
			case WM_RBUTTONDOWN:
				{
					int mx = GET_X_LPARAM(msg.lParam);
					int my = GET_Y_LPARAM(msg.lParam);
					mouseDown = true;
					OnMouseDown.Invoke(this, MouseEventArgs(mbRight, mx, my, 0, GET_KEYSTATE_WPARAM(msg.wParam)));
				}
				return true;
			case WM_MOUSEMOVE:
				{
					int mx = GET_X_LPARAM(msg.lParam);
					int my = GET_Y_LPARAM(msg.lParam);
					OnMouseMove.Invoke(this, MouseEventArgs(mbNone, mx, my, 0, GET_KEYSTATE_WPARAM(msg.wParam)));
				}
				return true;
			case WM_MOUSEWHEEL:
				{
					int delta = GET_WHEEL_DELTA_WPARAM(msg.wParam);
					OnMouseWheel.Invoke(this, MouseEventArgs(mbNone, 0,0, delta, GET_KEYSTATE_WPARAM(msg.wParam)));
				}
				return true;
			case WM_CHAR:
				{
					OnKeyPressed.Invoke(this, KeyEventArgs((wchar_t)(msg.wParam),0,false,false,false));
				}
				return true;
			case WM_KEYDOWN:
				{
					OnKeyDown.Invoke(this, KeyEventArgs(0,msg.wParam,false,false,false));
					if (msg.wParam == VK_TAB)
					{
						HandleTab();
					}
					BaseControl * ctrl = dynamic_cast<BaseControl *>(Application::GetComponent(msg.hWnd));
					if (ctrl)
					{
						WinMessage nmsg(msg.hWnd, gxMsgKeyBroadcast, msg.wParam, msg.lParam);
						BaseForm * f = ctrl->GetOwnerForm();
						if (f)
							f->BroadcastMessage(nmsg);
						else
							ctrl->BroadcastMessage(nmsg);
					}
				}
				return true;
			case WM_KEYUP:
				{
					OnKeyUp.Invoke(this, KeyEventArgs(0,msg.wParam,false,false,false));
				}
				return true;
			case WM_SETFOCUS:
				{
					OnFocus.Invoke(this, EventArgs());
				}
				return false;
			case WM_KILLFOCUS:
				{
					OnLostFocus.Invoke(this, EventArgs());
				}
				return false;
			case WM_MOUSEHOVER:
				{
					OnMouseEnter.Invoke(this, EventArgs());
				}
				return true;
			case WM_MOUSELEAVE:
				{
					OnMouseLeave.Invoke(this, EventArgs());
				}
				return true;
			case WM_SIZING:
				{
					ResizingEventArgs args;
					RECT * rect = (RECT *)msg.lParam;
					args.Width = rect->right - rect->left;
					args.Height = rect->bottom - rect->top;
					args.Left = rect->left;
					args.Top = rect->top;
					OnResizing.Invoke(this, args);
					rect->left = args.Left;
					rect->top = args.Top;
					rect->bottom = args.Top + args.Height;
					rect->right = args.Left + args.Width;
				}
				return false;
			case WM_SIZE:
				{
					OnResized.Invoke(this, EventArgs());
				}
				return false;
			case WM_PAINT:
				{
					if (ownerDraw)
					{
						PaintEventArgs args;
						RECT rect;
						int NeedDraw = GetUpdateRect(handle, &rect, true);
						if (NeedDraw)
						{
							PAINTSTRUCT s;
							args.Graphics = new Graphics(BeginPaint(handle, &s));
							args.UpdateRect.Left = rect.left;
							args.UpdateRect.Right = rect.right;
							args.UpdateRect.Bottom = rect.bottom;
							args.UpdateRect.Top = rect.top;
							OnPaint.Invoke(this, args);
							EndPaint(handle, &s);
							ReleaseDC(handle,s.hdc);
						}
					}
				}
				return false;
			default:
				return rs;
			}
		}

		void BaseControl::HandleTab()
		{
			int t;
			if (tabOrder >= 0)
			{
				t = tabOrder+1;
			}
			else
				t = 0;
			BaseForm * bf = GetOwnerForm();
			if (bf)
			{
				BaseControl * ctrl = bf->GetControlTabOrder(t);
				if (!ctrl)
				{
					t = 0;
					ctrl = bf->GetControlTabOrder(t);
				}
				if (ctrl)
					ctrl->SetFocus();
			}
		}

		bool BaseControl::ProcessNotification(WinNotification note)
		{
			switch (note.code)
			{
			case NM_SETFOCUS:
				_Focused();
				return true;
			case NM_KILLFOCUS:
				_LostFocus();
				return true;
			default:
				return false;
			}
		}

		void BaseControl::_Focused()
		{
			OnFocus.Invoke(this, EventArgs());
		}

		void BaseControl::_LostFocus()
		{
			OnLostFocus.Invoke(this, EventArgs());
		}

		void BaseControl::Destroy()
		{
			if (nativeFont)
				DeleteObject(nativeFont);
			DestroyWindow(handle);
		}

		void BaseControl::SetWidth(int w)
		{
			RECT r;
			GetWindowRect(handle, &r);
			MoveWindow(handle, r.left, r.top, w, r.bottom-r.top, true); 
		}

		void BaseControl::SetHeight(int h)
		{
			RECT r;
			GetWindowRect(handle, &r);
			MoveWindow(handle, r.left, r.top, r.right-r.left, h, true); 
		}

		void BaseControl::SetLeft(int l)
		{
			RECT r;
			GetWindowRect(handle, &r);
			MoveWindow(handle, l, r.top, r.right-r.left, r.bottom-r.top, true); 
		}

		void BaseControl::SetTop(int t)
		{
			RECT r;
			GetWindowRect(handle, &r);
			MoveWindow(handle, r.left, t, r.right-r.left, r.bottom-r.top, true); 
		}

		int BaseControl::GetWidth()
		{
			RECT r;
			GetWindowRect(handle, &r);
			return r.right-r.left;
		}

		int BaseControl::GetHeight()
		{
			RECT r;
			GetWindowRect(handle, &r);
			return r.bottom-r.top;
		}

		int BaseControl::GetLeft()
		{
			RECT r;
			GetWindowRect(handle, &r);
			return r.left;
		}

		int BaseControl::GetTop()
		{
			RECT r;
			GetWindowRect(handle, &r);
			return r.top;
		}

		void BaseControl::SetStyle(unsigned int StyleType, unsigned int Style, bool val)
		{
			ULONG style = GetWindowLongPtr(handle, StyleType);
			if (val)
				style = style | Style;
			else
				style = style & (~Style);
			SetWindowLongPtr(handle, StyleType, style);
		}

		bool BaseControl::GetStyle(unsigned int StyleType, unsigned int Style)
		{
			ULONG style = GetWindowLongPtr(handle, StyleType);
			return ((style & Style) == Style);
		}

		bool BaseControl::GetEnabled()
		{
			return IsWindowEnabled(handle)==TRUE; 
		}
		void BaseControl::SetEnabled(bool val)
		{
			EnableWindow(handle, val);
		}
		bool BaseControl::GetVisible()
		{
			return IsWindowVisible(handle)==TRUE;
		}
		void BaseControl::SetVisible(bool val)
		{
			ShowWindow(handle, val?SW_SHOW:SW_HIDE);
		}
		bool BaseControl::Focused()
		{
			return (GetFocus() == handle);
		}
		void BaseControl::SetFocus()
		{
			::SetFocus(handle);
		}
		void BaseControl::SetFont(Font * f)
		{
			font = f;
			LOGFONT fnt;
			f->GetLogFontW(&Graphics(handle), &fnt);
			if (nativeFont)
				DeleteObject(nativeFont);
			nativeFont = CreateFontIndirect(&fnt);
			SendMessage(handle,WM_SETFONT,(WPARAM)nativeFont,TRUE);
		}

		void BaseControl::SetPosition(int x, int y, int w, int h)
		{
			MoveWindow(handle, x,y,w,h, TRUE);
		}

		BaseControl::BaseControl()
			: tabStop(false), tabOrder(-1), ownerDraw(false), captureClick(true)
		{
			nativeFont = 0;
		}

		BaseControl::~BaseControl()
		{
			Destroy();
		}

		BaseControl * BaseControl::GetControlTabOrder(int t)
		{
			if (tabOrder == t)
				return this;
			for (int i=0; i<children.Count(); i++)
			{
				BaseControl * ctrl = dynamic_cast<BaseControl *>(children[i].operator->());
				if (ctrl)
				{
					BaseControl * rs = ctrl->GetControlTabOrder(t);
					if (rs)
						return rs;
				}
			}
			return 0;
		}

		void BaseControl::SetTabStop(bool ts)
		{
			tabStop = ts;
		}

		bool BaseControl::GetTabStop()
		{
			return tabStop;
		}

		void BaseControl::SetTabOrder(int t)
		{
			tabOrder = t;
		}

		int BaseControl::GetTabOrder()
		{
			return tabOrder;
		}

		void BaseControl::InitFont()
		{
			Font * f = new Font(Graphics(handle).GetHDC(), &Application::GetSysFont());
			SetFont(f);
		}

		void BaseControl::_Click()
		{
			OnClick.Invoke(this, EventArgs());
		}

		void Control::CreateWnd(Component * parent)
		{
			handle = CreateWindow(Application::ControlClassName, 0, WS_CHILD,
				0, 0, 50, 50, parent->GetHandle(), NULL, Application::GetHandle(), NULL);
			ownerDraw = true;
			if (!handle)
			{
				throw "Failed to create window.";
			}
		}

		void Control::Init(Component * parent)
		{
			CreateWnd(parent);
			InitFont();
			Application::RegisterComponent(this);
			SubClass();
			ShowWindow(handle, SW_SHOW); 
			UpdateWindow(handle); 
		}

		bool Control::ProcessNotification(WinNotification note)
		{
			return BaseControl::ProcessNotification(note);
		}

		void Control::Destroy()
		{
			UnSubClass();
			Application::UnRegisterComponent(this);
			BaseControl::Destroy();
		}

		Control::Control(Component * parent)
		{
			Init(parent);
		}

		Control::Control()
		{
		}

		Control::~Control()
		{
			Destroy();
		}
	}
}
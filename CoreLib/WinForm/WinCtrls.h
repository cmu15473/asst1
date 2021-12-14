#ifndef GX_WIN_CTRLS_H
#define GX_WIN_CTRLS_H

#include <Windows.h>
#include "../Basic.h"
#include "WinMessage.h"
#include "Gdiplus.h"

using namespace CoreLib;
using namespace CoreLib::Basic;
namespace CoreLib
{
	namespace WinForm
	{
		using namespace Gdiplus;

		class WinMessage;
		class BaseForm;

		class Component;

		class Action : public Object
		{
		public:
			virtual void Apply(Component * comp) = 0;
		};

		class Component : public Object
		{
			friend LRESULT CALLBACK SubClassWndProc(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,
				DWORD_PTR dwRefData);
			friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		protected:
			List<RefPtr<Component>> children;
			HWND handle;
			WNDPROC prevProc;
		protected:
			BaseForm * GetOwnerForm();
			virtual bool ProcessMessage(WinMessage msg);
			virtual bool ProcessNotification(WinNotification note);
			void BroadcastMessage(WinMessage msg);
			void SubClass();
			void UnSubClass();
		public:
			Component();
			HWND GetHandle() const;
			void SetParent(const Component * Parent);
			Component * GetParent();
			UserEvent OnUserEvent;
			virtual void Destroy();
			void AddChild(Component * comp);
			void RemoveChild(Component * comp);
			int GetChildrenCount();
			Component * GetChildren(int id);
			void ApplyActionToComponentAndChildren(Action * act);
		};

		class BaseControl abstract : public Component 
		{
		protected:
			RefPtr<Font> font;
			HFONT nativeFont;
			bool ownerDraw;
			bool tabStop;
			bool mouseDown;
			bool captureClick;
			int tabOrder;
			void SetStyle(unsigned int StyleType, unsigned int Style, bool val);
			bool GetStyle(unsigned int StyleType, unsigned int Style);
			bool ProcessMessage(WinMessage msg);
			bool ProcessNotification(WinNotification note);
			void InitFont();
			void _Click();
			void _Focused();
			void _LostFocus();
			virtual void HandleTab();
			BaseControl * GetControlTabOrder(int t);
		public:
			BaseControl();
			~BaseControl();
			virtual void Destroy();
			int GetWidth();
			int GetHeight();
			void SetWidth(int w);
			void SetHeight(int h);
			void SetPosition(int x, int y, int w, int h);
			int GetLeft();
			int GetTop();
			void SetLeft(int l);
			void SetTop(int t);
			bool GetEnabled();
			void SetEnabled(bool val);
			bool GetVisible();
			void SetVisible(bool val);
			bool Focused();
			void SetFocus();
			void SetFont(Font * font);
			void SetTabStop(bool ts);
			bool GetTabStop();
			void SetTabOrder(int t);
			int GetTabOrder();
		public:
			NotifyEvent OnClick;
			NotifyEvent OnDblClick;
			NotifyEvent OnFocus;
			NotifyEvent OnLostFocus;
			NotifyEvent OnMouseEnter;
			NotifyEvent OnMouseLeave;
			MouseEvent OnMouseDown;
			MouseEvent OnMouseUp;
			MouseEvent OnMouseMove;
			MouseEvent OnMouseWheel;
			KeyEvent OnKeyPressed;
			KeyEvent OnKeyUp;
			KeyEvent OnKeyDown;
			ResizingEvent OnResizing;
			NotifyEvent OnResized;
			PaintEvent OnPaint;
		};

		class Control : public BaseControl
		{
		protected:
			virtual void CreateWnd(Component * parent);
			virtual bool ProcessNotification(WinNotification note);
			void Init(Component * parent);
			Control();
		public:
			Control(Component * parent);
			~Control();
			void Destroy();
		};
	}
}

#endif
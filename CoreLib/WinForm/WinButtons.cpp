#include "WinButtons.h"
#include "WinApp.h"
#include "WinForm.h"

namespace CoreLib
{
	namespace WinForm
	{
		void ClearDefaultButtonAction::Apply(Component * comp)
		{
			Button * btn = dynamic_cast<Button*>(comp);
			if (btn)
				btn->SetDefault(false);
		}

		CustomButton::CustomButton(Component * parent)
			: Control()
		{
			tabStop = false;
			tabOrder = 0;
			Init(parent);
		}
		CustomButton::CustomButton()
		{
			tabStop = false;
			tabOrder = 0;
		}

		Button::Button(Component * comp)
			: CustomButton(comp)
		{
		}

		void Button::SetDefault(bool def)
		{
			bool ldef = GetStyle(GWL_STYLE, BS_DEFPUSHBUTTON);
			if (ldef != def)
			{
				if (GetStyle(GWL_STYLE, BS_DEFPUSHBUTTON) || GetStyle(GWL_STYLE, BS_PUSHBUTTON))
				{
					SetStyle(GWL_STYLE, BS_DEFPUSHBUTTON, def);
					SetStyle(GWL_STYLE, BS_PUSHBUTTON, !def);
				}
				UpdateWindow(handle);
			}
		
		}

		bool Button::ProcessMessage(WinMessage msg)
		{
			bool rs = CustomButton::ProcessMessage(msg);
			switch (msg.message)
			{
			case gxMsgKeyBroadcast:
				{
					if (msg.wParam == VK_RETURN && GetStyle(GWL_STYLE, BS_DEFPUSHBUTTON))
					{
						_Click();
					}
					else if (msg.wParam == VK_ESCAPE)
					{
						BaseForm * f = GetOwnerForm();
						if (f && f->GetCancelButton() == this)
						{
							_Click();
						}
					}
				}
				return true;
			default:
				return rs;
			}
		}

		bool Button::ProcessNotification(WinNotification note)
		{
			switch (note.code) 
			{
			case BN_SETFOCUS:
				{
					if (GetOwnerForm())
						GetOwnerForm()->ApplyActionToComponentAndChildren(&ClearDefaultButtonAction());
					SetDefault(true);
					UpdateWindow(handle);
				}
				return true;
			case BN_KILLFOCUS:
				{
					BaseForm * form = GetOwnerForm();
					if (form)
					{
						form->SetDefaultButton(form->GetDefaultButton());
					}
					UpdateWindow(handle);
				}
				return true;
			default:
				return CustomButton::ProcessNotification(note);
			}
		}

		void CustomButton::CreateWnd(Component * parent)
		{
			handle = CreateWindowEx
			(
				0,
				L"BUTTON",
				L"Button",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY ,
				10,
				10,
				100,
				100,
				parent->GetHandle(),
				NULL,
				Application::GetHandle(), 
				NULL
			); 
			captureClick = false;
			if (!handle)
			{
				throw "Failed to create window.";
			}
		}

		void CustomButton::SetText(const String & text)
		{
			SetWindowTextW(handle, text.Buffer());
		}

		bool CustomButton::ProcessNotification(WinNotification note)
		{
			switch (note.code)
			{
			case BN_CLICKED:
				{
					_Click();
				}
				return true;
			default:
				return Control::ProcessNotification(note);
			}
		}

		CheckBox::CheckBox(Component * parent)
			: CustomButton()
		{
			Init(parent);
		}

		void CheckBox::CreateWnd(Component * parent)
		{
			handle = CreateWindowEx
			(
				WS_EX_TRANSPARENT,
				L"BUTTON",
				L"CheckBox",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BS_TEXT | BS_NOTIFY ,
				10,
				10,
				100,
				100,
				parent->GetHandle(),
				NULL,
				Application::GetHandle(), 
				NULL
			); 
			captureClick = false;
			if (!handle)
			{
				throw "Failed to create window.";
			}
		}

		CheckBox::CheckBox()
		{
		}

		void CheckBox::SetChecked(bool chk)
		{
			SendMessage(handle, BM_SETCHECK, chk?BST_CHECKED:BST_UNCHECKED, 0);
		}

		bool CheckBox::GetChecked()
		{
			LRESULT rs = SendMessage(handle, BM_GETCHECK, 0,0);
			if (rs == BST_CHECKED)
				return true;
			else
				return false;
		}

		bool CheckBox::ProcessNotification(WinNotification note)
		{
		
			switch (note.code)
			{
			case BN_CLICKED:
				{
					SetChecked(!GetChecked());
				}
				return true;
			default:
				return CustomButton::ProcessNotification(note);
			}
		
		}

		RadioBox::RadioBox(Component * parent)
			: CheckBox()
		{
			Init(parent);
		}

		void RadioBox::CreateWnd(Component * parent)
		{
			handle = CreateWindowEx
			(
				WS_EX_TRANSPARENT,
				L"BUTTON",
				L"RadioBox",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON | BS_TEXT | BS_NOTIFY ,
				10,
				10,
				100,
				100,
				parent->GetHandle(),
				NULL,
				Application::GetHandle(), 
				NULL
			); 
			captureClick = false;
			if (!handle)
			{
				throw "Failed to create window.";
			}
		}

		bool RadioBox::ProcessNotification(WinNotification note)
		{
			switch (note.code)
			{
			case BN_CLICKED:
				{
					Component * comp = Application::GetComponent(::GetParent(handle));
					if (comp)
					{
						for (int i=0; i<comp->GetChildrenCount(); i++)
						{
							RadioBox * rd = dynamic_cast<RadioBox*>(comp->GetChildren(i));
							if (rd)
							{
								rd->SetChecked(false);
							}
						}
					}
					SetChecked(true);
				}
				return true;
			default:
				return CustomButton::ProcessNotification(note);
			}
		}

		GroupBox::GroupBox(Component * parent)
			: CustomButton()
		{
			Init(parent);
		}

		void GroupBox::CreateWnd(Component * parent)
		{
			handle = CreateWindowEx
			(
				0,
				L"BUTTON",
				L"GroupBox",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_GROUPBOX | BS_TEXT | BS_NOTIFY ,
				10,
				10,
				100,
				100,
				parent->GetHandle(),
				NULL,
				Application::GetHandle(), 
				NULL
			); 
			captureClick = false;
			if (!handle)
			{
				throw "Failed to create window.";
			}
		}
	}
}
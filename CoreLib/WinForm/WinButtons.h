#ifndef GX_WINBUTTONS_H
#define GX_WINBUTTONS_H

#include "WinCtrls.h"

namespace CoreLib
{
	namespace WinForm
	{
		class ClearDefaultButtonAction : public Action
		{
		public:
			void Apply(Component * comp);
		};

		class CustomButton : public Control
		{		
		protected:
			virtual void CreateWnd(Component * parent);
			virtual bool ProcessNotification(WinNotification note);
			CustomButton();
		public:
			CustomButton(Component * parent);
			void SetText(const String & text);
		
		};

		class Button : public CustomButton
		{
			friend class BaseForm;
			friend class ClearDefaultButtonAction;
		protected:
			void SetDefault(bool def);
			bool ProcessNotification(WinNotification note);
			bool ProcessMessage(WinMessage msg);
		public:
			Button(Component * parent);

		};

		class CheckBox : public CustomButton
		{
		protected:
			virtual void CreateWnd(Component * parent);
			virtual bool ProcessNotification(WinNotification note);
			CheckBox();
		public:
			CheckBox(Component * parent);
			void SetChecked(bool chk);
			bool GetChecked();
		};

		class RadioBox : public CheckBox
		{
		protected:
			virtual void CreateWnd(Component * parent);
			virtual bool ProcessNotification(WinNotification note);
		public:
			RadioBox(Component * parent);
		};

		class GroupBox : public CustomButton
		{
		protected:
			virtual void CreateWnd(Component * parent);
		public:
			GroupBox(Component * parent);
		};
	}
}
#endif
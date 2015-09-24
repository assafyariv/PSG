#pragma once

namespace AvocadoWFApp {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Windows::Forms;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			m_avoNativeWin = gcnew avocado::AvocadoManagedEngine (this,this->ClientSize.Width ,this->ClientSize.Height);
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: 
		System::Windows::Forms::Label^  Avocado3DLabel;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->Avocado3DLabel = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// Avocado3DLabel
			// 
			this->Avocado3DLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->Avocado3DLabel->AutoSize = true;
			this->Avocado3DLabel->BackColor = System::Drawing::Color::Transparent;
			this->Avocado3DLabel->Font = (gcnew System::Drawing::Font(L"Mistral", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Avocado3DLabel->ForeColor = System::Drawing::Color::DarkOliveGreen;
			this->Avocado3DLabel->Location = System::Drawing::Point(255, 320);
			this->Avocado3DLabel->Name = L"Avocado3DLabel";
			this->Avocado3DLabel->Size = System::Drawing::Size(117, 33);
			this->Avocado3DLabel->TabIndex = 0;
			this->Avocado3DLabel->Text = L"Avocado3D";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->ClientSize = System::Drawing::Size(384, 362);
			this->Controls->Add(this->Avocado3DLabel);
			this->Name = L"Form1";
			this->SizeGripStyle = System::Windows::Forms::SizeGripStyle::Show;
			this->Text = L"Form1";
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::Form1_Paint);
			this->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::Form1_MouseWheel);
			this->ResumeLayout(false);
			this->PerformLayout();
		}
#pragma endregion
		avocado::AvocadoManagedEngine ^m_avoNativeWin;
		private: System::Void Form1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
			{
				if (m_avoNativeWin)
				{
				   	m_avoNativeWin->SetAvocadoViewSize(this->ClientSize.Width ,this->ClientSize.Height);
				    m_avoNativeWin->CallPaint();
				}
			 }

		private: System::Void Form1_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
			{
			 
				 int zDelta = e->Delta ;
				 int x = e->X;
				 int y = e->Y;
				 if (m_avoNativeWin)
		     		m_avoNativeWin->MouseWheel( x,  y,  zDelta);
			}
};
}


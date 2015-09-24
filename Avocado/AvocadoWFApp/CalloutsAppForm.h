#pragma once
//#include "Form1.h"

namespace AvocadoWFApp {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for CalloutsAppForm
	/// </summary>
	public ref class CalloutsAppForm : System::Windows::Forms::Form
	{
	public:
		CalloutsAppForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			m_avoNativeWin = gcnew avocado::AvocadoManagedEngine (this,this->ClientSize.Width ,this->ClientSize.Height);
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CalloutsAppForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		System::Windows::Forms::Label^  Avocado3DLabel;
	private: System::Windows::Forms::Button^  LoadButton;
	private: System::Windows::Forms::Button^  TogHighLightButton;
	private: System::Windows::Forms::Button^  TogHLRButton;
	private: System::Windows::Forms::Button^  ToggleCellButton;
	private: System::Windows::Forms::Button^  OptixButton;
	private: System::Windows::Forms::Button^  PlayButton;
	private: System::Windows::Forms::Button^  PauseButton;
	private: System::Windows::Forms::Button^  StopButton;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::ComponentModel::IContainer^  components;


			 /// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->Avocado3DLabel = (gcnew System::Windows::Forms::Label());
			this->LoadButton = (gcnew System::Windows::Forms::Button());
			this->TogHighLightButton = (gcnew System::Windows::Forms::Button());
			this->TogHLRButton = (gcnew System::Windows::Forms::Button());
			this->ToggleCellButton = (gcnew System::Windows::Forms::Button());
			this->OptixButton = (gcnew System::Windows::Forms::Button());
			this->PlayButton = (gcnew System::Windows::Forms::Button());
			this->PauseButton = (gcnew System::Windows::Forms::Button());
			this->StopButton = (gcnew System::Windows::Forms::Button());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
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
			this->Avocado3DLabel->Location = System::Drawing::Point(297, 320);
			this->Avocado3DLabel->Name = L"Avocado3DLabel";
			this->Avocado3DLabel->Size = System::Drawing::Size(117, 33);
			this->Avocado3DLabel->TabIndex = 0;
			this->Avocado3DLabel->Text = L"Avocado3D";
			// 
			// LoadButton
			// 
			this->LoadButton->Location = System::Drawing::Point(13, 13);
			this->LoadButton->Name = L"LoadButton";
			this->LoadButton->Size = System::Drawing::Size(75, 23);
			this->LoadButton->TabIndex = 1;
			this->LoadButton->Text = L"Load File";
			this->LoadButton->UseVisualStyleBackColor = true;
			this->LoadButton->Click += gcnew System::EventHandler(this, &CalloutsAppForm::LoadButton_Click);
			// 
			// TogHighLightButton
			// 
			this->TogHighLightButton->Location = System::Drawing::Point(94, 13);
			this->TogHighLightButton->Name = L"TogHighLightButton";
			this->TogHighLightButton->Size = System::Drawing::Size(75, 23);
			this->TogHighLightButton->TabIndex = 2;
			this->TogHighLightButton->Text = L"HighLight";
			this->TogHighLightButton->UseVisualStyleBackColor = true;
			this->TogHighLightButton->Click += gcnew System::EventHandler(this, &CalloutsAppForm::TogHighLightButton_Click);
			// 
			// TogHLRButton
			// 
			this->TogHLRButton->Location = System::Drawing::Point(175, 13);
			this->TogHLRButton->Name = L"TogHLRButton";
			this->TogHLRButton->Size = System::Drawing::Size(75, 23);
			this->TogHLRButton->TabIndex = 3;
			this->TogHLRButton->Text = L"HLR";
			this->TogHLRButton->UseVisualStyleBackColor = true;
			this->TogHLRButton->Click += gcnew System::EventHandler(this, &CalloutsAppForm::TogHLRButton_Click);
			// 
			// ToggleCellButton
			// 
			this->ToggleCellButton->Location = System::Drawing::Point(256, 13);
			this->ToggleCellButton->Name = L"ToggleCellButton";
			this->ToggleCellButton->Size = System::Drawing::Size(75, 23);
			this->ToggleCellButton->TabIndex = 4;
			this->ToggleCellButton->Text = L"Cell Shading";
			this->ToggleCellButton->UseVisualStyleBackColor = true;
			this->ToggleCellButton->Click += gcnew System::EventHandler(this, &CalloutsAppForm::ToggleCellButton_Click);
			// 
			// OptixButton
			// 
			this->OptixButton->Location = System::Drawing::Point(337, 13);
			this->OptixButton->Name = L"OptixButton";
			this->OptixButton->Size = System::Drawing::Size(75, 23);
			this->OptixButton->TabIndex = 5;
			this->OptixButton->Text = L"Optix";
			this->OptixButton->UseVisualStyleBackColor = true;
			this->OptixButton->Click += gcnew System::EventHandler(this, &CalloutsAppForm::OptixButton_Click);
			// 
			// PlayButton
			// 
			this->PlayButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->PlayButton->Location = System::Drawing::Point(13, 330);
			this->PlayButton->Name = L"PlayButton";
			this->PlayButton->Size = System::Drawing::Size(75, 23);
			this->PlayButton->TabIndex = 6;
			this->PlayButton->Text = L"Play";
			this->PlayButton->UseVisualStyleBackColor = true;
			this->PlayButton->Click += gcnew System::EventHandler(this, &CalloutsAppForm::PlayButton_Click);
			// 
			// PauseButton
			// 
			this->PauseButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->PauseButton->Location = System::Drawing::Point(94, 330);
			this->PauseButton->Name = L"PauseButton";
			this->PauseButton->Size = System::Drawing::Size(75, 23);
			this->PauseButton->TabIndex = 7;
			this->PauseButton->Text = L"Pause";
			this->PauseButton->UseVisualStyleBackColor = true;
			this->PauseButton->Click += gcnew System::EventHandler(this, &CalloutsAppForm::PauseButton_Click);
			// 
			// StopButton
			// 
			this->StopButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->StopButton->Location = System::Drawing::Point(175, 330);
			this->StopButton->Name = L"StopButton";
			this->StopButton->Size = System::Drawing::Size(75, 23);
			this->StopButton->TabIndex = 8;
			this->StopButton->Text = L"Stop";
			this->StopButton->UseVisualStyleBackColor = true;
			this->StopButton->Click += gcnew System::EventHandler(this, &CalloutsAppForm::StopButton_Click);
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Interval = 1;
			this->timer1->Tick += gcnew System::EventHandler(this, &CalloutsAppForm::timer1_Tick);
			// 
			// CalloutsAppForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->ClientSize = System::Drawing::Size(426, 362);
			this->Controls->Add(this->StopButton);
			this->Controls->Add(this->PauseButton);
			this->Controls->Add(this->PlayButton);
			this->Controls->Add(this->OptixButton);
			this->Controls->Add(this->ToggleCellButton);
			this->Controls->Add(this->TogHLRButton);
			this->Controls->Add(this->TogHighLightButton);
			this->Controls->Add(this->LoadButton);
			this->Controls->Add(this->Avocado3DLabel);
			this->Name = L"CalloutsAppForm";
			this->SizeGripStyle = System::Windows::Forms::SizeGripStyle::Show;
			this->Text = L"CalloutsAppForm";
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &CalloutsAppForm::CalloutsAppForm_Paint);
			this->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &CalloutsAppForm::CalloutsAppForm_MouseWheel);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		avocado::AvocadoManagedEngine ^m_avoNativeWin;
		private: System::Void CalloutsAppForm_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
			{
				if (m_avoNativeWin)
				{
				   	m_avoNativeWin->SetAvocadoViewSize(this->ClientSize.Width ,this->ClientSize.Height);
				    m_avoNativeWin->CallPaint();
				}
			 }

		private: System::Void CalloutsAppForm_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
			{
			 
				 int zDelta = e->Delta ;
				 int x = e->X;
				 int y = e->Y;
				 if (m_avoNativeWin)
		     		m_avoNativeWin->MouseWheel( x,  y,  zDelta);
			}

		private: System::Void LoadButton_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 OpenFileDialog^ fileDialog		= gcnew OpenFileDialog; 
				 fileDialog->InitialDirectory	= "c:\\SceniX72\\media\\scenes\\";
				 fileDialog->Filter				= "Avocado files (*.avo)|*.txt|All files (*.*)|*.*";
				 fileDialog->FilterIndex		= 2;
				 fileDialog->RestoreDirectory	= true;

				 if ( fileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK ) 
				 {
					 String^ fName = fileDialog->FileName;
					 if (m_avoNativeWin)
		     			m_avoNativeWin->LoadFileDialog(fName);

				 }
			 }
		private: System::Void PlayButton_Click(System::Object^  sender, System::EventArgs^  e) 
			{
				if (m_avoNativeWin)
		     			m_avoNativeWin->PlayAnimation();
			}
		private: System::Void PauseButton_Click(System::Object^  sender, System::EventArgs^  e) 
			{
				if (m_avoNativeWin)
		     			m_avoNativeWin->PauseAnimation();
			}
		private: System::Void StopButton_Click(System::Object^  sender, System::EventArgs^  e) 
			{
				if (m_avoNativeWin)
		     			m_avoNativeWin->StopAnimation();
			}
		private: System::Void TogHighLightButton_Click(System::Object^  sender, System::EventArgs^  e) 
			{
				if (m_avoNativeWin)
		     			m_avoNativeWin->ToggleHighLight();
			}
		private: System::Void TogHLRButton_Click(System::Object^  sender, System::EventArgs^  e) 
			{
				if (m_avoNativeWin)
		     			m_avoNativeWin->ToggleHLR();
			}
		private: System::Void ToggleCellButton_Click(System::Object^  sender, System::EventArgs^  e) 
			{
				if (m_avoNativeWin)
		     			m_avoNativeWin->ToggleCellShading();
			}
		private: System::Void OptixButton_Click(System::Object^  sender, System::EventArgs^  e) 
			{
				if (m_avoNativeWin)
		     			m_avoNativeWin->ToggleOptix();
			}
		private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) 
			{
				if (m_avoNativeWin)
		     			m_avoNativeWin->TimerTick();
			}
};
}

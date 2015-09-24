
#include "StdAfx.h"
using namespace System::Windows::Forms;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;
using namespace avocado;
namespace AvocadoTests {
    using namespace System;
    ref class AvocadoManagedEngineTest;
    
    
    /// <summary>
///This is a test class for AvocadoManagedEngineTest and is intended
///to contain all AvocadoManagedEngineTest Unit Tests
///</summary>
	[TestClass]
	public ref class AvocadoManagedEngineTest
	{

	private: Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^  testContextInstance;
			 /// <summary>
			 ///Gets or sets the test context which provides
			 ///information about and functionality for the current test run.
			 ///</summary>
	public: property Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^  TestContext
			{
				Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^  get()
				{
					return testContextInstance;
				}
				System::Void set(Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^  value)
				{
					testContextInstance = value;
				}
			}

#pragma region Additional test attributes
			// 
			//You can use the following additional attributes as you write your tests:
			//
			//Use ClassInitialize to run code before running the first test in the class
			//public: [ClassInitialize]
			//static System::Void MyClassInitialize(TestContext^  testContext)
			//{
			//}
			//
			//Use ClassCleanup to run code after all tests in a class have run
			//public: [ClassCleanup]
			//static System::Void MyClassCleanup()
			//{
			//}
			//
			//Use TestInitialize to run code before running each test
			//public: [TestInitialize]
			//System::Void MyTestInitialize()
			//{
			//}
			//
			//Use TestCleanup to run code after each test has run
			//public: [TestCleanup]
			//System::Void MyTestCleanup()
			//{
			//}
			//
#pragma endregion
			/// <summary>
			///A test for AvocadoManagedEngine Constructor
			///</summary>
	public: [TestMethod]
			void AvocadoManagedEngineConstructorTest()
			{
				Form^  parentForm = nullptr; // TODO: Initialize to an appropriate value
				unsigned int iWidth = 0; // TODO: Initialize to an appropriate value
				unsigned int iHeight = 0; // TODO: Initialize to an appropriate value
				AvocadoManagedEngine^  target = (gcnew AvocadoManagedEngine(parentForm, iWidth, iHeight));
				Assert::Inconclusive(L"TODO: Implement code to verify target");
			}
			/// <summary>
			///A test for Dispose
			///</summary>
	public: [TestMethod]
			void DisposeTest()
			{
				Form^  parentForm = nullptr; // TODO: Initialize to an appropriate value
				unsigned int iWidth = 0; // TODO: Initialize to an appropriate value
				unsigned int iHeight = 0; // TODO: Initialize to an appropriate value
				AvocadoManagedEngine^  target = (gcnew AvocadoManagedEngine(parentForm, iWidth, iHeight)); // TODO: Initialize to an appropriate value
			//	target->Dispose();
				Assert::Inconclusive(L"A method that does not return a value cannot be verified.");
			}
			/// <summary>
			///A test for MouseLBDown
			///</summary>
	public: [TestMethod]
			void MouseLBDownTest()
			{
				Form^  parentForm = nullptr; // TODO: Initialize to an appropriate value
				unsigned int iWidth = 0; // TODO: Initialize to an appropriate value
				unsigned int iHeight = 0; // TODO: Initialize to an appropriate value
				AvocadoManagedEngine^  target = (gcnew AvocadoManagedEngine(parentForm, iWidth, iHeight)); // TODO: Initialize to an appropriate value
				int x = 0; // TODO: Initialize to an appropriate value
				int y = 0; // TODO: Initialize to an appropriate value
				target->MouseLBDown(x, y);
				Assert::Inconclusive(L"A method that does not return a value cannot be verified.");
			}
			/// <summary>
			///A test for MouseLBUp
			///</summary>
	public: [TestMethod]
			void MouseLBUpTest()
			{
				Form^  parentForm = nullptr; // TODO: Initialize to an appropriate value
				unsigned int iWidth = 0; // TODO: Initialize to an appropriate value
				unsigned int iHeight = 0; // TODO: Initialize to an appropriate value
				AvocadoManagedEngine^  target = (gcnew AvocadoManagedEngine(parentForm, iWidth, iHeight)); // TODO: Initialize to an appropriate value
				int x = 0; // TODO: Initialize to an appropriate value
				int y = 0; // TODO: Initialize to an appropriate value
				target->MouseLBUp(x, y);
				Assert::Inconclusive(L"A method that does not return a value cannot be verified.");
			}
			/// <summary>
			///A test for MouseMBDown
			///</summary>
	public: [TestMethod]
			void MouseMBDownTest()
			{
				Form^  parentForm = nullptr; // TODO: Initialize to an appropriate value
				unsigned int iWidth = 0; // TODO: Initialize to an appropriate value
				unsigned int iHeight = 0; // TODO: Initialize to an appropriate value
				AvocadoManagedEngine^  target = (gcnew AvocadoManagedEngine(parentForm, iWidth, iHeight)); // TODO: Initialize to an appropriate value
				int x = 0; // TODO: Initialize to an appropriate value
				int y = 0; // TODO: Initialize to an appropriate value
				target->MouseMBDown(x, y);
				Assert::Inconclusive(L"A method that does not return a value cannot be verified.");
			}
			/// <summary>
			///A test for MouseMBUp
			///</summary>
	public: [TestMethod]
			void MouseMBUpTest()
			{
				Form^  parentForm = nullptr; // TODO: Initialize to an appropriate value
				unsigned int iWidth = 0; // TODO: Initialize to an appropriate value
				unsigned int iHeight = 0; // TODO: Initialize to an appropriate value
				AvocadoManagedEngine^  target = (gcnew AvocadoManagedEngine(parentForm, iWidth, iHeight)); // TODO: Initialize to an appropriate value
				int x = 0; // TODO: Initialize to an appropriate value
				int y = 0; // TODO: Initialize to an appropriate value
				target->MouseMBUp(x, y);
				Assert::Inconclusive(L"A method that does not return a value cannot be verified.");
			}
			/// <summary>
			///A test for MouseMove
			///</summary>
	public: [TestMethod]
			void MouseMoveTest()
			{
				Form^  parentForm = nullptr; // TODO: Initialize to an appropriate value
				unsigned int iWidth = 0; // TODO: Initialize to an appropriate value
				unsigned int iHeight = 0; // TODO: Initialize to an appropriate value
				AvocadoManagedEngine^  target = (gcnew AvocadoManagedEngine(parentForm, iWidth, iHeight)); // TODO: Initialize to an appropriate value
				int x = 0; // TODO: Initialize to an appropriate value
				int y = 0; // TODO: Initialize to an appropriate value
				target->MouseMove(x, y);
				Assert::Inconclusive(L"A method that does not return a value cannot be verified.");
			}
			/// <summary>
			///A test for Render
			///</summary>
	public: [TestMethod]
			void RenderTest()
			{
				Form^  parentForm = nullptr; // TODO: Initialize to an appropriate value
				unsigned int iWidth = 0; // TODO: Initialize to an appropriate value
				unsigned int iHeight = 0; // TODO: Initialize to an appropriate value
				AvocadoManagedEngine^  target = (gcnew AvocadoManagedEngine(parentForm, iWidth, iHeight)); // TODO: Initialize to an appropriate value
				target->Render();
				Assert::Inconclusive(L"A method that does not return a value cannot be verified.");
			}
			/// <summary>
			///A test for SwapOpenGLBuffers
			///</summary>
	public: [TestMethod]
			void SwapOpenGLBuffersTest()
			{
				Form^  parentForm = nullptr; // TODO: Initialize to an appropriate value
				unsigned int iWidth = 0; // TODO: Initialize to an appropriate value
				unsigned int iHeight = 0; // TODO: Initialize to an appropriate value
				AvocadoManagedEngine^  target = (gcnew AvocadoManagedEngine(parentForm, iWidth, iHeight)); // TODO: Initialize to an appropriate value
				target->SwapOpenGLBuffers();
				Assert::Inconclusive(L"A method that does not return a value cannot be verified.");
			}
			/// <summary>
			///A test for AvocadoManagedEngine Constructor
			///</summary>
	public: [TestMethod]
			void AvocadoManagedEngineConstructorTest1()
			{
				UserControl^  parentForm = nullptr; // TODO: Initialize to an appropriate value
				unsigned int iWidth = 0; // TODO: Initialize to an appropriate value
				unsigned int iHeight = 0; // TODO: Initialize to an appropriate value
				AvocadoManagedEngine^  target = (gcnew AvocadoManagedEngine(parentForm, iWidth, iHeight));
				Assert::Inconclusive(L"TODO: Implement code to verify target");
			}
	};
}
namespace AvocadoTests {
    
}

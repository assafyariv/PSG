#include "AvocadoEngine.h"
#include "AvocadoScenixAdapter.h"
#include "AvocadoInternalModules.h"

#include <nvui/SceneRenderer.h>
#include <nvgl/SceneRendererGL2.h>
#include <nvgl/RenderTargetGLFBO.h>
#include <nvgl/RendererGLFSQ.h>
#include <nvutil\Trace.h>
//#include <nvutil/SmartPtr.h>
//using namespace avocado;
namespace avocado {
void AvocadoEngine::AvocadoInitDefaultOptions ()
	{
		size_t numOfPages = 5;
		int curPage = 0;
		AvocadoOptionPage *pages = new AvocadoOptionPage[numOfPages];
		pages[curPage].name = "Import options";
		{
			AvocadoOption opt;
			opt.Name = "flip_yz_on_import";
			opt.Label = "Flip YZ on import";
			opt.Description = "This is description";
			opt.valueBool = false;
			opt.Type = AvocadoOption::BOOL;
			opt.UIType = AvocadoOption::CHECKBOX;
			pages[curPage].options.push_back (opt);
		}
		{
			AvocadoOption opt;
			opt.Name = "convert_ffp_on_import";
			opt.Label = "Convert to FFP on import";
			opt.Description = "This is description";
			opt.valueBool = false;
			opt.Type = AvocadoOption::BOOL;
			opt.UIType = AvocadoOption::CHECKBOX;
			pages[curPage].options.push_back (opt);
		}
		
		// NEW PAGE -----------------------------
		curPage++;
		pages[curPage].name = "Export options";
		{
			AvocadoOption opt;
			opt.Name = "flip_yz_on_export";
			opt.Label = "Flip YZ on export";
			opt.Description = "This is description";
			opt.valueBool = false;
			opt.Type = AvocadoOption::BOOL;
			opt.UIType = AvocadoOption::CHECKBOX;
			pages[curPage].options.push_back (opt);
		}
		{
			AvocadoOption opt;
			opt.Name = "embed_models_on_export";
			opt.Label = "Embed model on export";
			opt.Description = "This is description";
			opt.valueBool = false;
			opt.Type = AvocadoOption::BOOL;
			opt.UIType = AvocadoOption::CHECKBOX;
			pages[curPage].options.push_back (opt);
		}
		{
			AvocadoOption opt;
			opt.Name = "use_optix_for_image_export";
			opt.Label = "Use Optix when saving image";
			opt.Description = "This is description";
			opt.valueBool = false;
			opt.Type = AvocadoOption::BOOL;
			opt.UIType = AvocadoOption::CHECKBOX;
			pages[curPage].options.push_back (opt);
		}
		{
			AvocadoOption opt;
			opt.Name = "default_image_width";
			opt.Label = "Sets the default width for exported images";
			opt.Description = "This is description";
			opt.valueInt = 3300;
			opt.Type = AvocadoOption::INT;
			opt.UIType = AvocadoOption::EDITBOX;
			pages[curPage].options.push_back (opt);
		}
		{
			AvocadoOption opt;
			opt.Name = "default_image_height";
			opt.Label = "Sets the default height for exported images";
			opt.Description = "This is description";
			opt.valueInt = 2200;
			opt.Type = AvocadoOption::INT;
			opt.UIType = AvocadoOption::EDITBOX;
			pages[curPage].options.push_back (opt);
		}
		// NEW PAGE -----------------------------
		curPage++;
		pages[curPage].name = "Publish options";
		{
			AvocadoOption opt;
			opt.Name = "smooth_on_publish";
			opt.Label = "Smooth model on publish";
			opt.Description = "model smoothing on publish";
			opt.valueBool = false;
			opt.Type = AvocadoOption::BOOL;
			opt.UIType = AvocadoOption::CHECKBOX;
			pages[curPage].options.push_back (opt);
		}
		{
			AvocadoOption opt;
			opt.Name = "smooth_on_publish_angle";
			opt.Label = "Smooth angle";
			opt.Description = "Set the smooth angle";
			opt.valueInt = 2;
			opt.Type = AvocadoOption::INT;
			opt.UIType = AvocadoOption::SPINBOX;
			opt.scrollMax = 100;
			opt.scrollMin = 0;
			pages[curPage].options.push_back (opt);
		}
		// NEW PAGE -----------------------------
		curPage++;
		pages[curPage].name = "Graphics options";
		{
			AvocadoOption opt;
			opt.Name = "backface_culling";
			opt.Label = "BF Culling";
			opt.Description = "This is description";
			opt.valueBool = false;
			opt.Type = AvocadoOption::BOOL;
			opt.UIType = AvocadoOption::CHECKBOX;
			pages[curPage].options.push_back (opt);
		}
		{
			AvocadoOption opt;
			opt.Name = "two_side_lightning";
			opt.Label = "Two side lightning";
			opt.Description = "This is description";
			opt.valueBool = true;
			opt.Type = AvocadoOption::BOOL;
			opt.UIType = AvocadoOption::CHECKBOX;
			pages[curPage].options.push_back (opt);
		}
		{
			AvocadoOption opt;
			opt.Name = "disable_raytracing";
			opt.Label = "Disable Ray Tracing";
			opt.Description = "Mostly used in emmbeded ActiveX";
			opt.valueBool = false;
			opt.Type = AvocadoOption::BOOL;
			opt.UIType = AvocadoOption::CHECKBOX;
			pages[curPage].options.push_back (opt);
		}
		// NEW PAGE -----------------------------
		curPage++;
		pages[curPage].name = "Navigation options";
		{
			AvocadoOption opt;
			opt.Name = "flip_mouse";
			opt.Label = "This is a label";
			opt.Description = "This is description";
			opt.valueBool = false;
			opt.Type = AvocadoOption::BOOL;
			opt.UIType = AvocadoOption::CHECKBOX;
			pages[curPage].options.push_back (opt);
		}
		{
			AvocadoOption opt;
			opt.Name = "flip_mouse_xx";
			opt.Label = "This is a label for combo";
			opt.Description = "This is description";
			opt.valueInt = 12;
			opt.Type = AvocadoOption::INT;
			opt.UIType = AvocadoOption::COMBOLIST;
			opt.possibleValuesInt.push_back (12);
			opt.possibleValuesInt.push_back (13);
			opt.possibleValuesInt.push_back (14);
			opt.listValueNames.push_back ("Low");
			opt.listValueNames.push_back ("High");
			opt.listValueNames.push_back ("Medium");
			pages[curPage].options.push_back (opt);
		}

		m_optionStructure.name = "Avocado Engine Options";
		for (size_t K=0;K < numOfPages;K++)
			m_optionStructure.pages.push_back (pages[K]);

		delete [] pages;


		m_defaultOptions = ParamList::createNew ();
		for (size_t jj = 0; jj < m_optionStructure.pages.size ();jj++)
			for (size_t op = 0; op < m_optionStructure.pages[jj].options.size();op++)
			{
				 AvocadoOption opp = m_optionStructure.pages[jj].options[op];
				 switch (opp.Type)
				 {
				 case AvocadoOption::BOOL:
					 m_defaultOptions->PushBool (opp.Name,opp.valueBool);
					 break;
				 case AvocadoOption::INT:
					 m_defaultOptions->PushInt (opp.Name,opp.valueInt);
					 break;
				 case AvocadoOption::FLOAT:
					 m_defaultOptions->PushFloat (opp.Name,opp.valueFloat);
					 break;
				 case AvocadoOption::STRING:
					 m_defaultOptions->PushString (opp.Name,opp.valueString);
					 break;
					 default:
						 // not good; assert here.
						 break;
				 }
			}
		}

		bool  AvocadoEngine::GetAvocadoOptionStructure ( AvocadoOptionStructure & opt) const
		{
			opt = m_optionStructure;
			return true;
		}

bool AvocadoEngine::GetAvocadoOption (std::string optionName,void *value, AvocadoOption::InternalType type)
	{
		Param *p = NULL;
		if (m_overideOptions)
		{
			p = m_overideOptions->GetParam (optionName);
		}
		if (!p)
		{
			p = m_defaultOptions->GetParam (optionName );
			if (!p)
				return false;
		}
		if (p->GetType () == "Integer")
		{
			if (type != AvocadoOption::INT)
				return false;
			int val;
			((IntParam*)p)->GetValue (val);
			(*((int*)value)) = val;
		} 
		else if (p->GetType () == "String")
		{
			if (type != AvocadoOption::STRING)
				return false;
			string val;
			((StringParam*)p)->GetValue (val);
			(*((string*)value)) = val;
		}  
		else if (p->GetType () == "Float")
		{
			if (type != AvocadoOption::FLOAT)
				return false;
			float val;
			((FloatParam*)p)->GetValue (val);
			(*((float*)value)) = val;
		} 
		else if (p->GetType () == "Boolean")
		{ 
			if (type != AvocadoOption::BOOL)
				return false;
			bool val;
			((BoolParam*)p)->GetValue (val);
			(*((bool*)value)) = val;
		} 
		return true;
	}

	
bool AvocadoEngine::SetAvocadoOption (std::string optionName,void *value, AvocadoOption::InternalType type)
	{
		// Check that options exists in deafults
		Param *dp = m_defaultOptions->GetParam (optionName);
		if (!dp)
			return false;
		ParamListSharedPtr plmsg = ParamList::createNew ();

		if (!m_overideOptions)
		{
			m_overideOptions = ParamList::createNew ();
		}
		Param *p = m_overideOptions->GetParam (optionName);
		if (p)
		{
			switch (type)
			{
			case AvocadoOption::INT:
				if (p->GetType () != "Integer")
					return false;
				((IntParam *)p)->SetValue (*((int*)value));
				plmsg->PushInt (optionName,*((int*)value));
				break;
			case AvocadoOption::BOOL:
				if (p->GetType () != "Boolean")
					return false;
				((BoolParam *)p)->SetValue (*((bool*)value));
				plmsg->PushBool (optionName,*((bool*)value));
				break;
			case AvocadoOption::FLOAT:
				if (p->GetType () != "Float")
					return false;
				((FloatParam *)p)->SetValue (*((float*)value));
				plmsg->PushFloat (optionName,*((float*)value));
				break;
			case AvocadoOption::STRING:
				if (p->GetType () != "String")
					return false;
				((StringParam *)p)->SetValue (*((std::string*)value));
				plmsg->PushString (optionName,*((string*)value));
				break;
			default:
				return false;

				
			}		
		} else
		{
			switch (type)
			{
			case AvocadoOption::INT:
				m_overideOptions->PushInt (optionName,*((int*)value));
				plmsg->PushInt (optionName,*((int*)value));
				break;
			case AvocadoOption::BOOL:
				m_overideOptions->PushBool (optionName,*((bool*)value));
				plmsg->PushBool (optionName,*((bool*)value));
				break;
			case AvocadoOption::FLOAT:
				m_overideOptions->PushFloat (optionName,*((float*)value));
				plmsg->PushFloat (optionName,*((float*)value));
				break;
			case AvocadoOption::STRING:
				m_overideOptions->PushString (optionName,*((string*)value));
				plmsg->PushString (optionName,*((string*)value));
				break;
			default:
				return false;
		   }
		}
		
		bool needRepaint;
		if (this->GetActiveDoc())
		  HandleAvocadoDocGeneralStringMessage ("AvocadoOptionChanged",this->GetActiveDoc()->GetID(),plmsg->SerializeList(),needRepaint);

		return true;
	}
	
	
}
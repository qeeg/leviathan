#include "LayerRemovalViewModel.h"
#include "LayerRemovalView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::LayerRemovalViewModel::LayerRemovalViewModel(const std::wstring& amenuHandlerName, int aviewModelID, S315_5313* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* S315_5313::LayerRemovalViewModel::CreateView()
{
	return new LayerRemovalView(device);
}

//----------------------------------------------------------------------------------------
void S315_5313::LayerRemovalViewModel::DeleteView(IView* aview)
{
	delete aview;
}

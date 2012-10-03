#include "inputX.h"
#include "inputX_i.h"

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED bool _INPXCheckError(HRESULT hr, bool displayMsg, const char *header)
{
	switch(hr)
	{
	case DI_OK:
		return false;
	case DI_BUFFEROVERFLOW:
		if(displayMsg) ASSERT_MSG(0,"The device buffer overflowed and some input was lost. This value is equal to the S_FALSE standard COM return value.",header);
		return true;
	case DI_DOWNLOADSKIPPED:
		if(displayMsg) ASSERT_MSG(0,"The parameters of the effect were successfully updated, but the effect could not be downloaded because the associated device was not acquired in exclusive mode.",header);
		return true;
	case DI_EFFECTRESTARTED:
		if(displayMsg) ASSERT_MSG(0,"The effect was stopped, the parameters were updated, and the effect was restarted.",header);
		return true;
	//case DI_NOEFFECT:
	//	if(displayMsg) ASSERT_MSG(0,"The operation had no effect. This value is equal to the S_FALSE standard COM return value.",header);
	//	return true;
	//case DI_NOTATTACHED:
	//	if(displayMsg) ASSERT_MSG(0,"The device exists but is not currently attached. This value is equal to the S_FALSE standard COM return value.",header);
	//	return true;
	case DI_POLLEDDEVICE:
		if(displayMsg) ASSERT_MSG(0,"The device is a polled device. As a result, device buffering does not collect any data and event notifications is not signaled until the IDirectInputDevice8::Poll method is called.",header);
		return true;
	//case DI_PROPNOEFFECT:
	//	if(displayMsg) ASSERT_MSG(0,"The change in device properties had no effect. This value is equal to the S_FALSE standard COM return value.",header);
	//	return true;
	case DI_SETTINGSNOTSAVED:
		if(displayMsg) ASSERT_MSG(0,"The action map was applied to the device, but the settings could not be saved.",header);
		return true;
	case DI_TRUNCATED:
		if(displayMsg) ASSERT_MSG(0,"The parameters of the effect were successfully updated, but some of them were beyond the capabilities of the device and were truncated to the nearest supported value.",header);
		return true;
	case DI_TRUNCATEDANDRESTARTED:
		if(displayMsg) ASSERT_MSG(0,"Equal to DI_EFFECTRESTARTED | DI_TRUNCATED.",header);
		return true;
	case DI_WRITEPROTECT:
		if(displayMsg) ASSERT_MSG(0,"A SUCCESS code indicating that settings cannot be modified.",header);
		return true;
	case DIERR_ACQUIRED:
		if(displayMsg) ASSERT_MSG(0,"The operation cannot be performed while the device is acquired.",header);
		return true;
	case DIERR_ALREADYINITIALIZED:
		if(displayMsg) ASSERT_MSG(0,"This object is already initialized",header);
		return true;
	case DIERR_BADDRIVERVER:
		if(displayMsg) ASSERT_MSG(0,"The object could not be created due to an incompatible driver version or mismatched or incomplete driver components.",header);
		return true;
	case DIERR_BETADIRECTINPUTVERSION:
		if(displayMsg) ASSERT_MSG(0,"The application was written for an unsupported prerelease version of DirectInput.",header);
		return true;
	case DIERR_DEVICEFULL:
		if(displayMsg) ASSERT_MSG(0,"The device is full.",header);
		return true;
	case DIERR_DEVICENOTREG:
		if(displayMsg) ASSERT_MSG(0,"The device or device instance is not registered with DirectInput. This value is equal to the REGDB_E_CLASSNOTREG standard COM return value.",header);
		return true;
	case DIERR_EFFECTPLAYING:
		if(displayMsg) ASSERT_MSG(0,"The parameters were updated in memory but were not downloaded to the device because the device does not support updating an effect while it is still playing.",header);
		return true;
	case DIERR_GENERIC:
		if(displayMsg) ASSERT_MSG(0,"An undetermined error occurred inside the DirectInput subsystem. This value is equal to the E_FAIL standard COM return value.",header);
		return true;
	case DIERR_HANDLEEXISTS:
		if(displayMsg) ASSERT_MSG(0,"The device already has an event notification associated with it. This value is equal to the E_ACCESSDENIED standard COM return value.",header);
		return true;
	case DIERR_HASEFFECTS:
		if(displayMsg) ASSERT_MSG(0,"The device cannot be reinitialized because effects are attached to it.",header);
		return true;
	case DIERR_INCOMPLETEEFFECT:
		if(displayMsg) ASSERT_MSG(0,"The effect could not be downloaded because essential information is missing. For example, no axes have been associated with the effect, or no type-specific information has been supplied.",header);
		return true;
	case DIERR_INPUTLOST:
		if(displayMsg) ASSERT_MSG(0,"Access to the input device has been lost. It must be reacquired.",header);
		return true;
	case DIERR_INVALIDPARAM:
		if(displayMsg) ASSERT_MSG(0,"An invalid parameter was passed to the returning function, or the object was not in a state that permitted the function to be called. This value is equal to the E_INVALIDARG standard COM return value.",header);
		return true;
	case DIERR_MAPFILEFAIL:
		if(displayMsg) ASSERT_MSG(0,"An error has occured either reading the vendor-supplied action-mapping file for the device or reading or writing the user configuration mapping file for the device.",header);
		return true;
	case DIERR_MOREDATA:
		if(displayMsg) ASSERT_MSG(0,"Not all the requested information fit into the buffer.",header);
		return true;
	case DIERR_NOAGGREGATION:
		if(displayMsg) ASSERT_MSG(0,"This object does not support aggregation.",header);
		return true;
	case DIERR_NOINTERFACE:
		if(displayMsg) ASSERT_MSG(0,"The object does not support the specified interface. This value is equal to the E_NOINTERFACE standard COM return value.",header);
		return true;
	case DIERR_NOTACQUIRED:
		if(displayMsg) ASSERT_MSG(0,"The operation cannot be performed unless the device is acquired.",header);
		return true;
	case DIERR_NOTBUFFERED:
		if(displayMsg) ASSERT_MSG(0,"The device is not buffered. Set the DIPROP_BUFFERSIZE property to enable buffering.",header);
		return true;
	case DIERR_NOTDOWNLOADED:
		if(displayMsg) ASSERT_MSG(0,"The effect is not downloaded.",header);
		return true;
	case DIERR_NOTEXCLUSIVEACQUIRED:
		if(displayMsg) ASSERT_MSG(0,"The operation cannot be performed unless the device is acquired in DISCL_EXCLUSIVE mode.",header);
		return true;
	case DIERR_NOTFOUND:
		if(displayMsg) ASSERT_MSG(0,"The requested object does not exist.",header);
		return true;
	case DIERR_NOTINITIALIZED:
		if(displayMsg) ASSERT_MSG(0,"This object has not been initialized.",header);
		return true;
	//case DIERR_OBJECTNOTFOUND:
	//	if(displayMsg) ASSERT_MSG(0,"The requested object does not exist.",header);
	//	return true;
	case DIERR_OLDDIRECTINPUTVERSION:
		if(displayMsg) ASSERT_MSG(0,"The application requires a newer version of DirectInput.",header);
		return true;
	//case DIERR_OTHERAPPHASPRIO:
	//	if(displayMsg) ASSERT_MSG(0,"Another application has a higher priority level, preventing this call from succeeding. This value is equal to the E_ACCESSDENIED standard COM return value. This error can be returned when an application has only foreground access to a device but is attempting to acquire the device while in the background.",header);
	//	return true;
	case DIERR_OUTOFMEMORY:
		if(displayMsg) ASSERT_MSG(0,"The DirectInput subsystem could not allocate sufficient memory to complete the call. This value is equal to the E_OUTOFMEMORY standard COM return value.",header);
		return true;
	//case DIERR_READONLY:
	//	if(displayMsg) ASSERT_MSG(0,"The specified property cannot be changed. This value is equal to the E_ACCESSDENIED standard COM return value.",header);
	//	return true;
	case DIERR_REPORTFULL:
		if(displayMsg) ASSERT_MSG(0,"More information was requested to be sent than can be sent to the device.",header);
		return true;
	case DIERR_UNPLUGGED:
		if(displayMsg) ASSERT_MSG(0,"The operation could not be completed because the device is not plugged in.",header);
		return true;
	case DIERR_UNSUPPORTED:
		if(displayMsg) ASSERT_MSG(0,"The function called is not supported at this time. This value is equal to the E_NOTIMPL standard COM return value.",header);
		return true;
	}

	return false;
}
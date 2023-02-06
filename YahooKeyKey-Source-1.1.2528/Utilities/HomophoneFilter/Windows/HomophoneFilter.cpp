// HomophoneFilter.cpp : main project file.

#include "stdafx.h"

#pragma unmanaged
#include "BIConfig.h"
#include "LFPlatform.h"
#include "OpenVanilla.h"

#pragma managed
#include <vcclr.h>

#include "BIServerRPCInterface.h"
#include "HomophoneForm.h"

using namespace OpenVanilla;
using namespace LFPlatform;
using namespace HomophoneFilter;

BISContext PMSharedContext = NULL;

[STAThreadAttribute] int main(array<System::String ^> ^ args) {
  RPCClientHelper::ServerHandle c_RPCServerInterfaceHandle;

  if (!(c_RPCServerInterfaceHandle = RPCClientHelper::ConnectServer(
            BASEIME_SERVER_IDENTIFIER, &BIServerRPCInterfaceGlobalHandle))) {
    return 1;
  }

  if (!BISOpenConnection(&PMSharedContext)) {
    return 1;
  }

  // Enabling Windows XP visual effects before any controls are created
  Application::EnableVisualStyles();
  Application::SetCompatibleTextRenderingDefault(false);

  // Create the main window and run it
  Application::Run(gcnew HomophoneForm());

  BISCloseConnection(&PMSharedContext);

  if (c_RPCServerInterfaceHandle)
    RPCClientHelper::Disconnect(c_RPCServerInterfaceHandle,
                                &BIServerRPCInterfaceGlobalHandle);

  return 0;
}

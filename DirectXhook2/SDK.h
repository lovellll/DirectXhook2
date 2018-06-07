#pragma once

#define NOMINMAX
#include <Windows.h>

#include "SDK\interfaces\IAppSystem.hpp"
#include "SDK\interfaces\IBaseClientDll.hpp"
#include "SDK\interfaces\IClientEntity.hpp"
#include "SDK\interfaces\IClientEntityList.hpp"
#include "SDK\interfaces\IClientMode.hpp"
#include "SDK\interfaces\IConVar.hpp"
#include "SDK\interfaces\ICvar.hpp"
#include "SDK\interfaces\IEngineTrace.hpp"
#include "SDK\interfaces\IVEngineClient.hpp"
#include "SDK\interfaces\IVDebugOverlay.hpp"
#include "SDK\interfaces\ISurface.hpp"
#include "SDK\interfaces\CInput.hpp"
#include "SDK\interfaces\IVModelInfoClient.hpp"
#include "SDK\interfaces\IVModelRender.hpp"
#include "SDK\interfaces\IRenderView.hpp"
#include "SDK\interfaces\IGameEventmanager.hpp"
#include "SDK\interfaces\IMaterialSystem.hpp"
#include "SDK\interfaces\IMoveHelper.hpp"
#include "SDK\interfaces\IMDLCache.hpp"
#include "SDK\interfaces\IPrediction.hpp"
#include "SDK\interfaces\IPanel.hpp"
#include "SDK\interfaces\IEngineSound.hpp"
#include "SDK\interfaces\IViewRender.hpp"
#include "SDK\interfaces\CClientState.hpp"
#include "SDK\interfaces\IPhysics.hpp"

#include "SDK\misc\Convar.hpp"
#include "SDK\misc\CUserCmd.hpp"
#include "SDK\misc\glow_outline_effect.hpp"

#include "SDK\netvars.hpp"

extern CGlowObjectManager*   g_GlowObjManager;
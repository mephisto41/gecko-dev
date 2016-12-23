/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ClientCanvasLayer.h"
#include "GLContext.h"                  // for GLContext
#include "GLScreenBuffer.h"             // for GLScreenBuffer
#include "GeckoProfiler.h"              // for PROFILER_LABEL
#include "SharedSurfaceEGL.h"           // for SurfaceFactory_EGLImage
#include "SharedSurfaceGL.h"            // for SurfaceFactory_GLTexture, etc
#include "ClientLayerManager.h"         // for ClientLayerManager, etc
#include "mozilla/gfx/Point.h"          // for IntSize
#include "mozilla/layers/AsyncCanvasRenderer.h"
#include "mozilla/layers/CompositorTypes.h"
#include "mozilla/layers/LayersTypes.h"
#include "nsCOMPtr.h"                   // for already_AddRefed
#include "nsISupportsImpl.h"            // for Layer::AddRef, etc
#include "nsRect.h"                     // for mozilla::gfx::IntRect
#include "nsXULAppAPI.h"                // for XRE_GetProcessType, etc
#include "gfxPrefs.h"                   // for WebGLForceLayersReadback
#include "gfxUtils.h"
#include "mozilla/layers/TextureClientSharedSurface.h"

using namespace mozilla::gfx;
using namespace mozilla::gl;

namespace mozilla {
namespace layers {

ClientCanvasLayer::~ClientCanvasLayer()
{
  MOZ_COUNT_DTOR(ClientCanvasLayer);
  if (mBufferProvider) {
    mBufferProvider->ClearCachedResources();
  }
  if (mCanvasClient) {
    mCanvasClient->OnDetach();
    mCanvasClient = nullptr;
  }
}

void
ClientCanvasLayer::RenderLayer()
{
  PROFILER_LABEL("ClientCanvasLayer", "RenderLayer",
    js::ProfileEntry::Category::GRAPHICS);

  RenderMaskLayers(this);

  UpdateCompositableClient();
  ClientManager()->Hold(this);
}

already_AddRefed<CanvasLayer>
ClientLayerManager::CreateCanvasLayer()
{
  NS_ASSERTION(InConstruction(), "Only allowed in construction phase");
  RefPtr<ClientCanvasLayer> layer =
    new ClientCanvasLayer(this);
  CREATE_SHADOW(Canvas);
  return layer.forget();
}

} // namespace layers
} // namespace mozilla

#include "AppLayer.hpp"

#include <algorithm>
#include <SDL3/SDL_log.h>

namespace sdlc
{

AppLayer::AppLayer(std::string_view name, int zIndex)
    : m_name(name), m_zIndex(zIndex)
{
}

void AppLayer::deleteLayer()
{
    if (m_container) {
        m_container->alive = false;
        m_container->enabled = false;
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Layer container not ready yet");
    }
}

void AppLayer::enable()
{
    if (m_container) {
        m_container->enabled = true;
    }
}

void AppLayer::disable()
{
    if (m_container) {
        m_container->enabled = false;
    } else {
        m_enabledInitState = false;
    }
}

LayerManager::LayerManager(AppState *appState)
    : m_appState(appState)
{
}

LayerManager::~LayerManager()
{
    for (auto& layerContainer : m_layers) {
        if (layerContainer.layer != nullptr) {
            delete layerContainer.layer;
        }
    }
}

void LayerManager::addLayer(AppLayer *layer)
{
    if (layer) {
        layer->m_appState = m_appState;
        m_layerAddStack.push(layer);
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Try to add null layer");
    }
}

void LayerManager::addLayersDeferred()
{
    bool needToSort = false;

    while (m_layerAddStack.size()) {
        AppLayer* candidate = m_layerAddStack.top();

        // Try to find abandoned layer container and re-use if possible
        bool found = false;
        for (size_t i = 0; i < m_layers.size(); ++i) {
            if (!m_layers[i].alive) {
                m_layers[i].layer = candidate;
                m_layers[i].zIndex = candidate->zIndex();
                m_layers[i].alive = true;
                m_layers[i].enabled = candidate->m_enabledInitState;
                candidate->m_container = &m_layers[i];
                found = true;
                break;
            }
        }

        // No un-used space. Add new one
        if (!found) {
            LayerContainer layerContainer {
                .layer = candidate,
                .alive = true,
                .enabled = candidate->m_enabledInitState,
                .zIndex = candidate->zIndex()
            };
            m_layers.emplace_back(layerContainer);
            candidate->m_container = &m_layers.back();
        }

        SDL_Log("Added layer: '%s' with zIndex: %d", candidate->name().data(), candidate->zIndex());

        m_layerAddStack.pop();
        needToSort = true;
    }

    // Sort layers by their z-index
    if (needToSort) {
        std::sort(m_layers.begin(), m_layers.end(), [] (const LayerContainer &a, const LayerContainer &b) {
           return a.zIndex < b.zIndex;
        });
    }
}

AppLayer * LayerManager::findLayerByName(std::string_view name)
{
    AppLayer *result = nullptr;
    for (auto& layerContainer : m_layers) {
        if (layerContainer.layer != nullptr && layerContainer.alive && layerContainer.layer->name() == name) {
            result = layerContainer.layer;
            break;
        }
    }
    return result;
}

void LayerManager::deleteLayerByName(std::string_view name)
{
    AppLayer *layer = findLayerByName(name);
    if (layer) {
        layer->deleteLayer();
    }
}

void LayerManager::disableLayerByName(std::string_view name)
{
    AppLayer *layer = findLayerByName(name);
    if (layer) {
        layer->disable();
    }
}

void LayerManager::enableLayerByName(std::string_view name)
{
    AppLayer *layer = findLayerByName(name);
    if (layer) {
        layer->enable();
    }
}

void LayerManager::handleEvent(SDL_Event &event)
{
    for (auto& layerContainer : m_layers) {
        if (layerContainer.layer != nullptr && layerContainer.alive && layerContainer.enabled) {
            layerContainer.layer->handleEvent(event);
        }
    }
}

void LayerManager::render(SDL_Renderer *renderer)
{
    for (auto& layerContainer : m_layers) {
        if (layerContainer.layer != nullptr && layerContainer.alive && layerContainer.enabled) {
            layerContainer.layer->render(renderer);
        }
    }
}

void LayerManager::update(float deltaTime)
{
    for (auto& layerContainer : m_layers) {
        if (!layerContainer.alive && layerContainer.layer != nullptr) {
            SDL_Log("Removed layer '%s'", layerContainer.layer->name().data());
            delete layerContainer.layer;
            layerContainer.layer = nullptr;
        } else {
            if (layerContainer.layer != nullptr && layerContainer.alive && layerContainer.enabled) {
                layerContainer.layer->update(deltaTime);
            }
        }
    }
}

} // namespace sdlc
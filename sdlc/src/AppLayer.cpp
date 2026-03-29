#include "AppLayer.hpp"

#include <algorithm>
#include <SDL3/SDL_log.h>

namespace sdlc
{

AppLayer::AppLayer(std::string_view name)
    : m_name(name)
{
}

LayerManager::LayerManager(AppState *appState)
    : m_appState(appState)
{
}

LayerManager::~LayerManager()
{
    for (auto layer : m_layers) {
        delete layer;
    }
}

void LayerManager::addLayer(AppLayer *layer)
{
    if (layer) {
        layer->m_appState = m_appState;
        m_layers.push_back(layer);
        std::sort(m_layers.begin(), m_layers.end(), [] (AppLayer *a, AppLayer *b) {
           return a->zIndex() < b->zIndex();
        });
        SDL_Log("Added layer: '%s' with zIndex: %d", layer->name().data(), layer->zIndex());
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Try to add null layer");
    }
}

AppLayer * LayerManager::findLayerByName(std::string_view name)
{
    AppLayer *result = nullptr;
    for (auto layer : m_layers) {
        if (layer->name() == name) {
            result = layer;
            break;
        }
    }
    return result;
}

void LayerManager::render(SDL_Renderer *renderer)
{
    for (auto layer : m_layers) {
        layer->render(renderer);
    }
}

void LayerManager::update(float deltaTime)
{
    for (auto layer : m_layers) {
        if (layer->toBeRemoved()) {
            m_layers.erase(std::ranges::remove(m_layers, layer).begin());
            SDL_Log("Removed layer '%s'", layer->name().data());
            delete layer;
        } else {
            layer->update(deltaTime);
        }
    }
}

} // namespace sdlc
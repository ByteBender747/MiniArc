#pragma once

#include <vector>
#include <stack>
#include <string>

#include <SDL3/SDL_render.h>

namespace sdlc
{

struct AppState;
class AppLayer;

struct LayerContainer
{
    AppLayer *layer{nullptr};
    bool alive{false};
    bool enabled{false};
    int zIndex{0};
};

class LayerManager
{
public:
    LayerManager(AppState *appState);
    ~LayerManager();

    void addLayer(AppLayer *layer);
    void render(SDL_Renderer *renderer);
    void update(float deltaTime);
    void handleEvent(SDL_Event &event);
    void addLayersDeferred();
    void deleteLayerByName(std::string_view name);
    void disableLayerByName(std::string_view name);
    void enableLayerByName(std::string_view name);
    AppLayer * findLayerByName(std::string_view name);

private:
    AppState *m_appState;
    std::stack<AppLayer*> m_layerAddStack;
    std::vector<LayerContainer> m_layers;
};

class AppLayer
{
public:
    AppLayer(std::string_view name, int zIndex);
    virtual ~AppLayer() = default;

    friend void LayerManager::addLayersDeferred();
    friend void LayerManager::addLayer(AppLayer *layer);

    virtual void render(SDL_Renderer *renderer) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void handleEvent(SDL_Event &event) { }

    [[nodiscard]] int zIndex() const { return m_zIndex; }
    [[nodiscard]] std::string_view name() const { return m_name; }
    void deleteLayer();
    void enable();
    void disable();

protected:
    [[nodiscard]] AppState * appState() const { return m_appState; }

private:
    int m_zIndex;
    bool m_preventFromAdding{false};
    bool m_enabledInitState{true};
    std::string_view m_name;
    LayerContainer *m_container{nullptr};
    AppState *m_appState{nullptr};
};

} // namespace sdlc
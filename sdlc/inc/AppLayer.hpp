#pragma once

#include <vector>
#include <string>

#include <SDL3/SDL_render.h>

namespace sdlc
{

struct AppState;
class AppLayer;

class LayerManager
{
public:
    LayerManager(AppState *appState);
    ~LayerManager();

    void addLayer(AppLayer *layer);
    void render(SDL_Renderer *renderer);
    void update(float deltaTime);
    AppLayer * findLayerByName(std::string_view name);

private:
    AppState *m_appState;
    std::vector<AppLayer*> m_layers;
};

class AppLayer
{
public:
    AppLayer(std::string_view name = "");
    virtual ~AppLayer() = default;
    friend void LayerManager::addLayer(AppLayer *layer);

    virtual void render(SDL_Renderer *renderer) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void handleEvent(SDL_Event &event) { }

    [[nodiscard]] bool toBeRemoved() const { return m_toBeRemoved; }
    [[nodiscard]] int zIndex() const { return m_zIndex; }
    [[nodiscard]] std::string_view name() const { return m_name; }
    void setZIndex(int zIndex) { m_zIndex = zIndex; }
    void deleteLayer() { m_toBeRemoved = false; }

protected:
    [[nodiscard]] AppState * appState() const { return m_appState; }

private:
    std::string m_name;
    AppState *m_appState{nullptr};
    int m_zIndex{0};
    bool m_toBeRemoved{false};
};

} // namespace sdlc
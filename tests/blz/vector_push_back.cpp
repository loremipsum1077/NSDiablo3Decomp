// Template checks for blz::vector: push_back(const T&) and _transfer.
#include "Contrib/Contrib/blz/include/blz/vector.h"
#include "vector_types.h"

template void blz::vector<SNOName>::push_back(const SNOName&);
template void blz::vector<ChromaEffect>::push_back(const ChromaEffect&);
template void blz::vector<DebugText>::push_back(const DebugText&);
template void blz::vector<UIID>::push_back(const UIID&);
template void blz::vector<VertexNoTexL>::push_back(const VertexNoTexL&);
template void blz::vector<Vertex1TexL>::push_back(const Vertex1TexL&);
template void blz::vector<MapTarget>::push_back(const MapTarget&);
template void blz::vector<WeatherVolume>::push_back(const WeatherVolume&);
template void blz::vector<Console::AnalogStickInputFilter::Rule>::push_back(const Console::AnalogStickInputFilter::Rule&);
template void blz::vector<CRefString>::push_back(const CRefString&);
template void blz::vector<UICharacterDetails::Console::StatListEntry>::push_back(const UICharacterDetails::Console::StatListEntry&);
template void blz::vector<UICharacterDetails::Console::StatListEntry>::_transfer(UICharacterDetails::Console::StatListEntry*);
template void blz::vector<UIConsoleDesignerMenu::ConsoleCommandOption>::push_back(const UIConsoleDesignerMenu::ConsoleCommandOption&);
template void blz::vector<UIConsoleDesignerMenu::QuestMenuOption>::push_back(const UIConsoleDesignerMenu::QuestMenuOption&);
template void blz::vector<UIConsoleDesignerMenu::QuestMenuOption>::_transfer(UIConsoleDesignerMenu::QuestMenuOption*);
template void blz::vector<UIEventQueue::Console::CriticalMessage>::push_back(const UIEventQueue::Console::CriticalMessage&);
template void blz::vector<UIEventQueue::Console::CriticalMessage>::_transfer(UIEventQueue::Console::CriticalMessage*);
template void blz::vector<UIOptions::Console::Selection>::push_back(const UIOptions::Console::Selection&);
template void blz::vector<UIOptions::Console::Selection>::_transfer(UIOptions::Console::Selection*);
template void blz::vector<UIPause::Console::PauseOption>::push_back(const UIPause::Console::PauseOption&);
template void blz::vector<UIEnchant::AffixInfo>::push_back(const UIEnchant::AffixInfo&);
template void blz::vector<OnlineService::GameTag>::push_back(const OnlineService::GameTag&);
template void blz::vector<DRLGTestEvent>::push_back(const DRLGTestEvent&);

#pragma once

#include "edm4hep/Track.h"
#include "edm4hep/TrackState.h"
#include <optional>

namespace k4RecTracker {
// Explicit location lookup also supports EDM4hep 1.0, before Track::getTrackState.
template <typename Location>
std::optional<edm4hep::TrackState> trackStateAt(const edm4hep::Track& track, Location location) {
  for (const auto& state : track.getTrackStates()) {
    if (state.location == static_cast<int>(location)) {
      return state;
    }
  }
  return std::nullopt;
}
} // namespace k4RecTracker

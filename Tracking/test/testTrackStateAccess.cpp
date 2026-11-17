#include "TrackStateAccess.h"
#include "edm4hep/TrackCollection.h"
#include <stdexcept>

int main() {
  edm4hep::TrackCollection tracks;
  auto track = tracks.create();
  edm4hep::TrackState first{};
  first.location = edm4hep::TrackState::AtFirstHit;
  first.phi = 2.0;
  track.addToTrackStates(first);
  edm4hep::TrackState ip{};
  ip.location = edm4hep::TrackState::AtIP;
  ip.phi = 0.25;
  track.addToTrackStates(ip);
  if (k4RecTracker::trackStateAt(track, edm4hep::TrackState::AtIP).value().phi != 0.25)
    throw std::runtime_error("IP state lookup did not preserve the measured state");
  if (k4RecTracker::trackStateAt(track, edm4hep::TrackState::AtCalorimeter).has_value())
    throw std::runtime_error("Missing states must remain missing");
}

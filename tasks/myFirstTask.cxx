#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Framework/AnalysisDataModel.h"

using namespace o2;
using namespace o2::framework;

struct MyFirstTask {
  void process(aod::Collision const& collision, aod::TracksIU const& tracks)
  {
    LOGF(info, "Collision index : %d", collision.index());
    LOGF(info, "Number of tracks: %d", tracks.size());
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<MyFirstTask>(cfgc),
  };
}

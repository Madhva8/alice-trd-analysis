/*
 * File: myFirstTask.cxx
 * Author: Madhva Fakare
 * 
 * A minimal O2Physics analysis task.
 * Loops over all collisions in a data file and prints basic info.
 *
 * What this teaches:
 *   - How an O2Physics task is structured (struct + process function)
 *   - How to access collision data via aod::Collision
 *   - How LOGF works (the framework's logging system)
 *   - How defineDataProcessing() acts as the entry point (like main())
 *
 * Run with:
 *   o2-analysis-my-first-task --aod-file path/to/AO2D.root --batch --run
 */

// The engine that starts and runs the framework
#include "Framework/runDataProcessing.h"
// Gives you the toolbox to write analysis tasks
#include "Framework/AnalysisTasks.h"
// Gives you access to ALICE data tables (Collision, Track, etc.)
#include "Framework/AnalysisDataModel.h"

struct MyFirstTask {

  // process() is the core function — the framework calls this once per collision.
  // aod::Collision  — the ALICE collision table (one row = one PbPb collision event)
  // const&          — read the data directly without copying it (fast and safe)
  void process(aod::Collision const& collision) {

    // LOGF is the framework's print function — like printf() or Python's print()
    // Severity levels: info, debug, warning, error, fatal
    // %d is a placeholder for an integer — like {} in Python f-strings
    // globalIndex() returns the unique ID number of this collision
    LOGF(info, "Hello from collision %d at vertex z = %.2f cm",
         collision.globalIndex(),
         collision.posZ());  // posZ() = z-position of the collision vertex
  }

};

// defineDataProcessing() is the entry point — like main() in normal C++.
// It tells the framework which tasks to run.
// WorkflowSpec = the list of tasks (like a Python list [])
// adaptAnalysisTask = wraps your struct so the framework can run it
WorkflowSpec defineDataProcessing(ConfigContext const& cfgc) {
  return WorkflowSpec{
    adaptAnalysisTask<MyFirstTask>(cfgc)
  };
}

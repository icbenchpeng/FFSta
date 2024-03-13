#include "TestFramework.hh"
#include "FastSta.hh"

#include "db_sta/dbNetwork.hh"
#include "ord/OpenRoad.hh"
#include "sta/Liberty.hh"
#include "sta/PortDirection.hh"
#include "sta/Sta.hh"

// #include "db_sta/dbNetwork.hh"
#include "db_sta/dbSta.hh"
// #include "sta/dbSta.hh"
#include "sta/Search.hh"
#include "sta/Network.hh"

namespace sta {

class compile_and_sched_arrival_test : public Test {
public:
  compile_and_sched_arrival_test() : Test(__FUNCTION__) {}
  int run() {
    auto pin = getPtr<sta::Pin>("pin_to_update_from");
    auto dbSta = ord::OpenRoad::openRoad()->getSta();
    // auto graph = dbSta->graph();
    auto network = dbSta->network();
    auto fastSta = network->search()->faststa();
    fastSta->findAllArrivals();
    auto pin_to_dbInst = [dbSta](Pin* pin) -> odb::dbInst* {
      odb::dbITerm* iterm;
      odb::dbBTerm* bterm;
      dbSta->getDbNetwork()->staToDb(pin, iterm, bterm);
      assert(iterm);
      return iterm->getInst();
    };

    auto inst = dbSta->getDbNetwork()->dbToSta(pin_to_dbInst(pin));
    auto cell = network->libertyCell(inst);

    sta::LibertyLibrarySeq libs;
    sta::LibertyLibraryIterator* lib_iter = network->libertyLibraryIterator();
    while (lib_iter->hasNext()) {
      sta::LibertyLibrary* lib = lib_iter->next();
      // massive kludge until makeEquivCells is fixed to only incldue link cells
      sta::LibertyCellIterator cell_iter(lib);
      if (cell_iter.hasNext()) {
        LibertyCell* cell = cell_iter.next();
        if (network->findLibertyCell(cell->name()) == cell) libs.push_back(lib);
      }
    }
    delete lib_iter;
    dbSta->makeEquivCells(&libs, nullptr);

    auto equiveCells = dbSta->equivCells(cell);
    for (auto to_cell : *equiveCells) {
      if (to_cell->area() > cell->area()) {
        logger()->warn("replace cell from %s to %s\n", cell->name(), to_cell->name());
        dbSta->replaceCell(inst, to_cell);
        break;
      }
    }
    logger()->warn("do nothing find\n");
    
    fastSta->update(network->direction(pin)->isInput() ? network->graph()->pinLoadVertex(pin) : network->graph()->pinDrvrVertex(pin));
    logger()->warn("after update do find arrivals\n");

    fastSta->findAllArrivals();
    
    logger()->warn("pin name %s\n", network->name(pin));
    // Laurel::getPtr<sta::Pin>("pin_obj");

    // sta::Sta().search()->faststa()->update();
	return 0;
  }
};

Test*
fsta_faststa_test() {
  TestGroup* group = new TestGroup("faststa");
  group->add(new compile_and_sched_arrival_test);
  return group;
}


}
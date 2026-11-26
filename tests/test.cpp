#include "ReliefHub.hpp"
#include <cassert>
#include <cstdio>
#include <iostream>

using namespace ReliefOps;
using namespace ReliefOps::Inventory;
using namespace ReliefOps::Logistics;

int main() {
    ReliefCargo a("MED", "Medical Kits", 10, 2.0, 5);
    ReliefCargo b("WATER", "Water", 20, 1.0, 3);
    assert(a.totalWeightKg() == 20.0);
    assert(!(a == b));

    DistributionDepot depot("TEST");
    depot.addInitialStock("MED", 50);
    depot -= a;
    assert(depot.stockOf("MED") == 40);
    depot += a;
    assert(depot.stockOf("MED") == 50);

    CargoManifest m;
    m.add(a);
    CargoManifest copy(m);
    assert(copy.size() == 1);
    assert(copy.totalWeightKg() == 20.0);

    PriorityDispatchQueue<ReliefCargo> q;
    q.push(b); q.push(a);
    assert(q.pop().urgency() == 5);

    HybridMedicalVehicle hybrid("HYB-T", 1000.0, 90.0);
    assert(hybrid.vehicleType() == "Hybrid Medical Vehicle");
    assert(hybrid.evaluateRouteKm(100.0, true) == 102.0);

    IncidentLogger logger("test_incident_log.bin");
    assert(logger.appendTransactional({"TEST-1", "HYB-T", "Unit test incident", 10.0}));
    assert(logger.readAll().size() == 1);
    std::remove("test_incident_log.bin");

    bool caught = false;
    try {
        GroundConvoy truck("TRUCK-T", 10.0, 80.0);
        CargoManifest heavy;
        heavy.add(ReliefCargo("BOX", "Heavy Box", 10, 5.0, 2));
        truck.loadManifest(heavy);
    } catch (const CargoOverweightException&) { caught = true; }
    assert(caught);

    std::cout << "All ERL-HUB tests passed.\n";
    return 0;
}

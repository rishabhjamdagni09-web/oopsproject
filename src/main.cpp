#include "ReliefHub.hpp"
#include <iostream>
#include <memory>

using namespace ReliefOps;
using namespace ReliefOps::Inventory;
using namespace ReliefOps::Logistics;

int main() {
    try {
        std::cout << "\n=== ERL-HUB: Emergency Relief Logistics Hub ===\n";
        DistributionDepot depot("MAIT-RELIEF-01");
        depot.addInitialStock("WATER", 500);
        depot.addInitialStock("MED", 120);
        depot.addInitialStock("RATION", 300);

        ReliefCargo water("WATER", "Clean Water Tank", 80, 10.0, 4);
        ReliefCargo medicine("MED", "Trauma Medical Kits", 30, 2.0, 5);
        ReliefCargo ration("RATION", "Ration Kits", 100, 1.5, 3);

        allocateSupplies(depot, water);
        allocateSupplies(depot, medicine);
        allocateSupplies(depot, ration);

        CargoManifest manifest(2);
        manifest.add(water);
        manifest.add(medicine);
        manifest.add(ration);

        // Deep-copy demonstration: copied manifest owns a separate dynamic array.
        CargoManifest backupManifest(manifest);

        GroundConvoy truck("TRK-104", 5000.0, 85.0);
        truck.setLocation(28.7041, 77.1025);
        truck.loadManifest(manifest);

        AerialTransport helicopter("AIR-207", 1500.0, 92.0);
        helicopter.loadManifest(manifest);

        HybridMedicalVehicle hybrid("HYB-301", 3000.0, 88.0);
        hybrid.loadManifest(manifest);

        std::vector<ReliefVehicle*> fleet;
        fleet.push_back(&truck);
        fleet.push_back(&helicopter);
        fleet.push_back(&hybrid);

        std::cout << "\n--- Dispatch Fleet (runtime polymorphism) ---\n";
        std::cout << tableHeader;
        for (ReliefVehicle* vehicle : fleet) {
            try {
                dispatchVehicle(*vehicle, 120.0, true);
            } catch (const DamagedCorridorException& ex) {
                std::cout << vehicle->id() << " NOT DISPATCHED: " << ex.what() << '\n';
            }
        }

        PriorityDispatchQueue<ReliefCargo> queue;
        queue.push(water);
        queue.push(medicine);
        queue.push(ration);
        std::cout << "\n--- Priority Supply Queue ---\n";
        while (!queue.empty()) {
            ReliefCargo next = queue.pop();
            std::cout << "Urgency " << next.urgency() << ": " << next.name() << '\n';
        }

        // Function-object sorting of urgent medical/supply requirements.
        std::vector<ReliefCargo> sortedNeeds{water, medicine, ration};
        std::sort(sortedNeeds.begin(), sortedNeeds.end(), CargoUrgencyFunctor());
        std::cout << "\n--- Sorted by Urgency ---\n";
        for (const auto& item : sortedNeeds) std::cout << item << '\n';

        IncidentLogger logger("data/incident_log.bin");
        IncidentRecord incident{"INC-2026-001", hybrid.id(),
            "Medical supplies dispatched to relief zone", 120.0};
        if (logger.appendTransactional(incident))
            std::cout << "\nIncident committed to binary log.\n";
        else
            std::cout << "\nIncident log write failed; transaction rolled back.\n";

        std::cout << "Stored incident records: " << logger.readAll().size() << '\n';
        std::cout << "\n--- Depot Inventory After Dispatch ---\n";
        depot.printStock(std::cout);
        std::cout << "\nOperational vehicles currently in scope: "
                  << ReliefVehicle::operationalVehicleCount() << '\n';
        std::cout << "Total dispatched tonnage: "
                  << ReliefVehicle::totalDispatchedTonnage() << " tonnes\n";
        std::cout << "Backup manifest items (deep copy): " << backupManifest.size() << '\n';
        std::cout << "Hybrid payload utilization: "
                  << payloadUtilizationPercent(hybrid.currentLoadKg(), hybrid.payloadCapacityKg()) << "%\n";
    } catch (const CargoOverweightException& ex) {
        std::cerr << "Cargo error: " << ex.what() << '\n';
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "System error: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}

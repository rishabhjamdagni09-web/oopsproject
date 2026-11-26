#pragma once
#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ReliefOps {
namespace Inventory {

class ReliefCargo {
    std::string code_;
    std::string name_;
    int quantity_;
    double unitWeightKg_;
    int urgency_; // 1 = routine, 5 = critical
public:
    ReliefCargo();
    ReliefCargo(const std::string& code, const std::string& name,
                int quantity, double unitWeightKg, int urgency);
    const std::string& code() const;
    const std::string& name() const;
    int quantity() const;
    double unitWeightKg() const;
    int urgency() const;
    double totalWeightKg() const;
    bool operator==(const ReliefCargo& other) const;
    friend std::ostream& operator<<(std::ostream& out, const ReliefCargo& cargo);
    friend std::istream& operator>>(std::istream& in, ReliefCargo& cargo);
};

class IncidentCommander;

class DistributionDepot {
    friend class IncidentCommander; // authorized critical-triage inventory override
    std::string depotCode_;
    std::unordered_map<std::string, int> stock_;
public:
    explicit DistributionDepot(const std::string& code = "DEPOT-01");
    void addInitialStock(const std::string& cargoCode, int quantity);
    int stockOf(const std::string& cargoCode) const;
    int& stockReference(const std::string& cargoCode); // return-by-reference inventory update
    DistributionDepot& operator+=(const ReliefCargo& cargo);
    DistributionDepot& operator-=(const ReliefCargo& cargo);
    bool operator==(const DistributionDepot& other) const;
    const std::string& code() const;
    void printStock(std::ostream& out) const;
};

class IncidentCommander {
public:
    void overrideStock(DistributionDepot& depot, const std::string& cargoCode, int emergencyQuantity);
};
void allocateSupplies(DistributionDepot& depot, const ReliefCargo& cargo);
void allocateSupplies(DistributionDepot& depot, const ReliefCargo& cargo, bool emergencyTriage);

class CargoManifest {
    ReliefCargo* items_;
    std::size_t size_;
    std::size_t capacity_;
    void grow();
public:
    CargoManifest();
    explicit CargoManifest(std::size_t initialCapacity);
    CargoManifest(const CargoManifest& other);              // deep copy
    CargoManifest& operator=(const CargoManifest& other);   // deep copy assignment
    ~CargoManifest();                                      // releases dynamic memory
    void add(const ReliefCargo& cargo);
    std::size_t size() const;
    const ReliefCargo& at(std::size_t index) const;
    double totalWeightKg() const;
};

template <typename T>
class PriorityDispatchQueue {
    std::vector<T> items_;
public:
    void push(const T& item) {
        items_.push_back(item);
        std::stable_sort(items_.begin(), items_.end(),
            [](const T& a, const T& b) { return a.urgency() > b.urgency(); });
    }
    bool empty() const { return items_.empty(); }
    std::size_t size() const { return items_.size(); }
    T pop() {
        if (items_.empty()) throw std::out_of_range("Dispatch queue is empty");
        T result = items_.front();
        items_.erase(items_.begin());
        return result;
    }
};

} // namespace Inventory

namespace Logistics {

class CargoOverweightException : public std::exception {
    std::string message_;
public:
    explicit CargoOverweightException(const std::string& message);
    const char* what() const noexcept override;
};
class FuelDepletionException : public std::exception {
    std::string message_;
public:
    explicit FuelDepletionException(const std::string& message);
    const char* what() const noexcept override;
};
class DamagedCorridorException : public std::exception {
    std::string message_;
public:
    explicit DamagedCorridorException(const std::string& message);
    const char* what() const noexcept override;
};

class GPSBeacon {
    double latitude_, longitude_;
public:
    GPSBeacon(double latitude = 0.0, double longitude = 0.0);
    void update(double latitude, double longitude);
    const double& latitude() const;
    const double& longitude() const;
};

class EmergencyLifeSupportUnit {
    int medicalKitCount_;
    int oxygenHours_;
public:
    EmergencyLifeSupportUnit(int kits = 0, int oxygenHours = 0);
    bool ready() const;
    void replenish(int kits, int oxygenHours);
};

inline double payloadUtilizationPercent(double loadKg, double capacityKg) {
    return capacityKg <= 0.0 ? 0.0 : (loadKg / capacityKg) * 100.0;
}

class ReliefVehicle {
protected:
    std::string vehicleId_;
    double payloadCapacityKg_;
    double fuelLevel_;
    GPSBeacon gps_;
    EmergencyLifeSupportUnit lifeSupport_;
    Inventory::CargoManifest manifest_;
    void validatePayload() const;
public:
    ReliefVehicle(const std::string& id = "VEH-0", double capacityKg = 1000.0,
                  double fuel = 100.0);
    virtual ~ReliefVehicle();
    virtual std::string vehicleType() const = 0;
    virtual double evaluateRouteKm(double distanceKm, bool corridorDamaged) const = 0;
    void loadManifest(const Inventory::CargoManifest& manifest);
    void setLocation(double latitude, double longitude);
    void setFuel(double fuel);
    const std::string& id() const;
    double payloadCapacityKg() const;
    double currentLoadKg() const;
    const GPSBeacon& gps() const;
    static int operationalVehicleCount();
    static double totalDispatchedTonnage();
    static void recordDispatch(double kilograms);
protected:
    static int activeVehicles_;
    static double dispatchedTonnageKg_;
};

class AerialTransport : virtual public ReliefVehicle {
public:
    AerialTransport(const std::string& id = "AIR-0", double capacityKg = 500.0,
                    double fuel = 100.0);
    std::string vehicleType() const override;
    double evaluateRouteKm(double distanceKm, bool corridorDamaged) const override;
};

class GroundConvoy : virtual public ReliefVehicle {
public:
    GroundConvoy(const std::string& id = "TRUCK-0", double capacityKg = 5000.0,
                 double fuel = 100.0);
    std::string vehicleType() const override;
    double evaluateRouteKm(double distanceKm, bool corridorDamaged) const override;
};

class AmphibiousCraft : virtual public ReliefVehicle {
public:
    AmphibiousCraft(const std::string& id = "BOAT-0", double capacityKg = 2500.0,
                    double fuel = 100.0);
    std::string vehicleType() const override;
    double evaluateRouteKm(double distanceKm, bool corridorDamaged) const override;
};

// Diamond inheritance is resolved because both parent branches virtually inherit ReliefVehicle.
class HybridMedicalVehicle : public GroundConvoy, public AmphibiousCraft {
public:
    HybridMedicalVehicle(const std::string& id = "HYBRID-0",
                         double capacityKg = 3000.0, double fuel = 100.0);
    std::string vehicleType() const override;
    double evaluateRouteKm(double distanceKm, bool corridorDamaged) const override;
};

struct IncidentRecord {
    std::string incidentId;
    std::string vehicleId;
    std::string description;
    double distanceKm;
};

class IncidentLogger {
    std::string filePath_;
    static void writeString(std::fstream& file, const std::string& value);
    static bool readString(std::fstream& file, std::string& value);
public:
    explicit IncidentLogger(const std::string& path = "data/incident_log.bin");
    bool appendTransactional(const IncidentRecord& record);
    std::vector<IncidentRecord> readAll() const;
};

struct CargoUrgencyFunctor {
    bool operator()(const Inventory::ReliefCargo& a,
                    const Inventory::ReliefCargo& b) const {
        return a.urgency() > b.urgency();
    }
};

std::ostream& tableHeader(std::ostream& out);
void dispatchVehicle(ReliefVehicle& vehicle, double distanceKm, bool corridorDamaged);

} // namespace Logistics
} // namespace ReliefOps

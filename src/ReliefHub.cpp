#include "ReliefHub.hpp"
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <sys/stat.h>

namespace ReliefOps {
namespace Inventory {

ReliefCargo::ReliefCargo() : code_("NA"), name_("Unspecified"), quantity_(0),
    unitWeightKg_(0.0), urgency_(1) {}
ReliefCargo::ReliefCargo(const std::string& code, const std::string& name,
    int quantity, double unitWeightKg, int urgency)
    : code_(code), name_(name), quantity_(quantity), unitWeightKg_(unitWeightKg),
      urgency_(urgency) {
    if (quantity < 0 || unitWeightKg < 0 || urgency < 1 || urgency > 5)
        throw std::invalid_argument("Invalid cargo quantity, weight, or urgency");
}
const std::string& ReliefCargo::code() const { return code_; }
const std::string& ReliefCargo::name() const { return name_; }
int ReliefCargo::quantity() const { return quantity_; }
double ReliefCargo::unitWeightKg() const { return unitWeightKg_; }
int ReliefCargo::urgency() const { return urgency_; }
double ReliefCargo::totalWeightKg() const { return quantity_ * unitWeightKg_; }
bool ReliefCargo::operator==(const ReliefCargo& other) const {
    return code_ == other.code_ && name_ == other.name_;
}
std::ostream& operator<<(std::ostream& out, const ReliefCargo& cargo) {
    out << cargo.code_ << ' ' << std::quoted(cargo.name_) << ' '
        << cargo.quantity_ << ' ' << cargo.unitWeightKg_ << ' ' << cargo.urgency_;
    return out;
}
std::istream& operator>>(std::istream& in, ReliefCargo& cargo) {
    std::string code, name;
    int quantity, urgency;
    double weight;
    if (in >> code >> std::quoted(name) >> quantity >> weight >> urgency)
        cargo = ReliefCargo(code, name, quantity, weight, urgency);
    return in;
}

DistributionDepot::DistributionDepot(const std::string& code) : depotCode_(code) {}
void DistributionDepot::addInitialStock(const std::string& cargoCode, int quantity) {
    if (quantity < 0) throw std::invalid_argument("Stock cannot be negative");
    stock_[cargoCode] = quantity;
}
int DistributionDepot::stockOf(const std::string& cargoCode) const {
    auto it = stock_.find(cargoCode);
    return it == stock_.end() ? 0 : it->second;
}
int& DistributionDepot::stockReference(const std::string& cargoCode) {
    return stock_[cargoCode];
}
DistributionDepot& DistributionDepot::operator+=(const ReliefCargo& cargo) {
    stock_[cargo.code()] += cargo.quantity();
    return *this;
}
DistributionDepot& DistributionDepot::operator-=(const ReliefCargo& cargo) {
    int current = stockOf(cargo.code());
    if (cargo.quantity() > current) throw std::runtime_error("Insufficient depot stock for " + cargo.code());
    stock_[cargo.code()] = current - cargo.quantity();
    return *this;
}
bool DistributionDepot::operator==(const DistributionDepot& other) const {
    return depotCode_ == other.depotCode_;
}
const std::string& DistributionDepot::code() const { return depotCode_; }
void IncidentCommander::overrideStock(DistributionDepot& depot, const std::string& cargoCode, int emergencyQuantity) {
    if (emergencyQuantity < 0) throw std::invalid_argument("Emergency stock cannot be negative");
    depot.stock_[cargoCode] += emergencyQuantity; // friend access for exceptional triage
}
void allocateSupplies(DistributionDepot& depot, const ReliefCargo& cargo) {
    depot -= cargo; // volume-based allocation
}
void allocateSupplies(DistributionDepot& depot, const ReliefCargo& cargo, bool emergencyTriage) {
    if (emergencyTriage) {
        IncidentCommander commander;
        commander.overrideStock(depot, cargo.code(), cargo.quantity());
    } else {
        depot -= cargo;
    }
}
void DistributionDepot::printStock(std::ostream& out) const {
    out << "Depot " << depotCode_ << " stock:\n";
    for (const auto& entry : stock_) out << "  " << entry.first << " : " << entry.second << '\n';
}

CargoManifest::CargoManifest() : items_(new ReliefCargo[2]), size_(0), capacity_(2) {}
CargoManifest::CargoManifest(std::size_t initialCapacity)
    : items_(new ReliefCargo[initialCapacity ? initialCapacity : 1]), size_(0),
      capacity_(initialCapacity ? initialCapacity : 1) {}
CargoManifest::CargoManifest(const CargoManifest& other)
    : items_(new ReliefCargo[other.capacity_]), size_(other.size_), capacity_(other.capacity_) {
    for (std::size_t i = 0; i < size_; ++i) items_[i] = other.items_[i];
}
CargoManifest& CargoManifest::operator=(const CargoManifest& other) {
    if (this != &other) {
        ReliefCargo* replacement = new ReliefCargo[other.capacity_];
        for (std::size_t i = 0; i < other.size_; ++i) replacement[i] = other.items_[i];
        delete[] items_;
        items_ = replacement; size_ = other.size_; capacity_ = other.capacity_;
    }
    return *this;
}
CargoManifest::~CargoManifest() { delete[] items_; }
void CargoManifest::grow() {
    std::size_t nextCapacity = capacity_ * 2;
    ReliefCargo* replacement = new ReliefCargo[nextCapacity];
    for (std::size_t i = 0; i < size_; ++i) replacement[i] = items_[i];
    delete[] items_; items_ = replacement; capacity_ = nextCapacity;
}
void CargoManifest::add(const ReliefCargo& cargo) {
    if (size_ == capacity_) grow();
    items_[size_++] = cargo;
}
std::size_t CargoManifest::size() const { return size_; }
const ReliefCargo& CargoManifest::at(std::size_t index) const {
    if (index >= size_) throw std::out_of_range("Manifest index out of range");
    return items_[index];
}
double CargoManifest::totalWeightKg() const {
    double total = 0.0;
    for (std::size_t i = 0; i < size_; ++i) total += items_[i].totalWeightKg();
    return total;
}

} // namespace Inventory

namespace Logistics {

CargoOverweightException::CargoOverweightException(const std::string& m) : message_(m) {}
const char* CargoOverweightException::what() const noexcept { return message_.c_str(); }
FuelDepletionException::FuelDepletionException(const std::string& m) : message_(m) {}
const char* FuelDepletionException::what() const noexcept { return message_.c_str(); }
DamagedCorridorException::DamagedCorridorException(const std::string& m) : message_(m) {}
const char* DamagedCorridorException::what() const noexcept { return message_.c_str(); }

GPSBeacon::GPSBeacon(double latitude, double longitude)
    : latitude_(latitude), longitude_(longitude) {}
void GPSBeacon::update(double latitude, double longitude) {
    latitude_ = latitude; longitude_ = longitude;
}
const double& GPSBeacon::latitude() const { return latitude_; }
const double& GPSBeacon::longitude() const { return longitude_; }

EmergencyLifeSupportUnit::EmergencyLifeSupportUnit(int kits, int oxygenHours)
    : medicalKitCount_(kits), oxygenHours_(oxygenHours) {}
bool EmergencyLifeSupportUnit::ready() const { return medicalKitCount_ > 0 && oxygenHours_ > 0; }
void EmergencyLifeSupportUnit::replenish(int kits, int oxygenHours) {
    medicalKitCount_ += kits; oxygenHours_ += oxygenHours;
}

int ReliefVehicle::activeVehicles_ = 0;
double ReliefVehicle::dispatchedTonnageKg_ = 0.0;
ReliefVehicle::ReliefVehicle(const std::string& id, double capacityKg, double fuel)
    : vehicleId_(id), payloadCapacityKg_(capacityKg), fuelLevel_(fuel),
      gps_(), lifeSupport_(2, 8), manifest_() { ++activeVehicles_; }
ReliefVehicle::~ReliefVehicle() { --activeVehicles_; }
void ReliefVehicle::validatePayload() const {
    if (manifest_.totalWeightKg() > payloadCapacityKg_)
        throw CargoOverweightException("Manifest exceeds payload capacity of " + vehicleId_);
}
void ReliefVehicle::loadManifest(const Inventory::CargoManifest& manifest) {
    manifest_ = manifest;
    validatePayload();
}
void ReliefVehicle::setLocation(double latitude, double longitude) { gps_.update(latitude, longitude); }
void ReliefVehicle::setFuel(double fuel) { fuelLevel_ = fuel; }
const std::string& ReliefVehicle::id() const { return vehicleId_; }
double ReliefVehicle::payloadCapacityKg() const { return payloadCapacityKg_; }
double ReliefVehicle::currentLoadKg() const { return manifest_.totalWeightKg(); }
const GPSBeacon& ReliefVehicle::gps() const { return gps_; }
int ReliefVehicle::operationalVehicleCount() { return activeVehicles_; }
double ReliefVehicle::totalDispatchedTonnage() { return dispatchedTonnageKg_ / 1000.0; }
void ReliefVehicle::recordDispatch(double kilograms) { dispatchedTonnageKg_ += kilograms; }

AerialTransport::AerialTransport(const std::string& id, double capacityKg, double fuel)
    : ReliefVehicle(id, capacityKg, fuel) {}
std::string AerialTransport::vehicleType() const { return "Aerial Transport"; }
double AerialTransport::evaluateRouteKm(double distanceKm, bool damaged) const {
    if (damaged) return distanceKm * 0.85; // aerial route bypasses damaged roads
    return distanceKm;
}
GroundConvoy::GroundConvoy(const std::string& id, double capacityKg, double fuel)
    : ReliefVehicle(id, capacityKg, fuel) {}
std::string GroundConvoy::vehicleType() const { return "Ground Convoy"; }
double GroundConvoy::evaluateRouteKm(double distanceKm, bool damaged) const {
    if (damaged) throw DamagedCorridorException("Ground corridor is marked damaged");
    return distanceKm * 1.10; // road detour allowance
}
AmphibiousCraft::AmphibiousCraft(const std::string& id, double capacityKg, double fuel)
    : ReliefVehicle(id, capacityKg, fuel) {}
std::string AmphibiousCraft::vehicleType() const { return "Amphibious Craft"; }
double AmphibiousCraft::evaluateRouteKm(double distanceKm, bool damaged) const {
    return damaged ? distanceKm * 1.05 : distanceKm * 1.15;
}
HybridMedicalVehicle::HybridMedicalVehicle(const std::string& id, double capacityKg, double fuel)
    : ReliefVehicle(id, capacityKg, fuel),
      GroundConvoy(id, capacityKg, fuel), AmphibiousCraft(id, capacityKg, fuel) {}
std::string HybridMedicalVehicle::vehicleType() const { return "Hybrid Medical Vehicle"; }
double HybridMedicalVehicle::evaluateRouteKm(double distanceKm, bool damaged) const {
    return damaged ? distanceKm * 1.02 : distanceKm * 1.08;
}

IncidentLogger::IncidentLogger(const std::string& path) : filePath_(path) {}
void IncidentLogger::writeString(std::fstream& file, const std::string& value) {
    std::uint32_t length = static_cast<std::uint32_t>(value.size());
    file.write(reinterpret_cast<const char*>(&length), sizeof(length));
    file.write(value.data(), length);
}
bool IncidentLogger::readString(std::fstream& file, std::string& value) {
    std::uint32_t length = 0;
    if (!file.read(reinterpret_cast<char*>(&length), sizeof(length))) return false;
    if (length > 1000000) return false;
    value.assign(length, '\0');
    return static_cast<bool>(file.read(&value[0], length));
}
bool IncidentLogger::appendTransactional(const IncidentRecord& record) {
    std::string tempPath = filePath_ + ".tmp";
    std::fstream out(tempPath.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    if (!out) return false;

    // Copy the old binary log into the temporary file, then append one complete record.
    std::ifstream old(filePath_.c_str(), std::ios::binary);
    if (old) {
        out << old.rdbuf();
        old.close();
    }
    std::streampos transactionStart = out.tellp();
    try {
        writeString(out, record.incidentId);
        writeString(out, record.vehicleId);
        writeString(out, record.description);
        out.write(reinterpret_cast<const char*>(&record.distanceKm), sizeof(record.distanceKm));
        if (!out) throw std::ios_base::failure("Unable to write incident record");
        out.flush();
        if (!out) throw std::ios_base::failure("Unable to flush incident record");
    } catch (...) {
        out.clear();
        out.seekp(transactionStart); // rollback to transaction boundary
        out.close();
        std::remove(tempPath.c_str());
        return false;
    }
    out.close();
    if (std::rename(tempPath.c_str(), filePath_.c_str()) != 0) {
        std::remove(tempPath.c_str());
        return false;
    }
    return true;
}
std::vector<IncidentRecord> IncidentLogger::readAll() const {
    std::vector<IncidentRecord> records;
    std::fstream in(filePath_.c_str(), std::ios::binary | std::ios::in);
    if (!in) return records;
    while (in.peek() != std::char_traits<char>::eof()) {
        IncidentRecord r;
        if (!readString(in, r.incidentId)) break;
        if (!readString(in, r.vehicleId)) break;
        if (!readString(in, r.description)) break;
        if (!in.read(reinterpret_cast<char*>(&r.distanceKm), sizeof(r.distanceKm))) break;
        records.push_back(r);
    }
    return records;
}

std::ostream& tableHeader(std::ostream& out) {
    out << std::left << std::setw(14) << "VEHICLE ID"
        << std::setw(27) << "VEHICLE TYPE" << std::setw(14) << "LOAD (KG)"
        << std::setw(14) << "CAPACITY" << "ROUTE (KM)\n";
    out << std::string(83, '-') << '\n';
    return out;
}
void dispatchVehicle(ReliefVehicle& vehicle, double distanceKm, bool corridorDamaged) {
    if (vehicle.currentLoadKg() > vehicle.payloadCapacityKg())
        throw CargoOverweightException("Cannot dispatch overloaded vehicle");
    if (distanceKm <= 0) throw std::invalid_argument("Distance must be positive");
    double adjustedDistance = vehicle.evaluateRouteKm(distanceKm, corridorDamaged);
    if (adjustedDistance > 0 && adjustedDistance > 10000.0)
        throw FuelDepletionException("Estimated route exceeds safe fuel planning limit");
    ReliefVehicle::recordDispatch(vehicle.currentLoadKg());
    std::cout << std::left << std::setw(14) << vehicle.id()
              << std::setw(27) << vehicle.vehicleType()
              << std::setw(14) << vehicle.currentLoadKg()
              << std::setw(14) << vehicle.payloadCapacityKg()
              << adjustedDistance << '\n';
}

} // namespace Logistics
} // namespace ReliefOps

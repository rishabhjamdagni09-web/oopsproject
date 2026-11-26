# ERL-HUB — Post-Earthquake Multi-Agency Emergency Relief Logistics Hub

**MAIT | Department of CSE | CIC-211: Object-Oriented Programming Using C++**  
**Project option:** 2  
**Language:** C++17

## Overview
ERL-HUB is a console-based disaster-relief logistics prototype. It manages relief cargo, depot inventory, transport vehicles, dispatch priorities and persistent incident records after an earthquake.

## Features mapped to the assignment

### Unit I — Core foundations and call semantics
- Namespaces: `ReliefOps::Logistics` and `ReliefOps::Inventory`.
- Overloaded operators for cargo/depot operations and stream I/O.
- Call-by-reference and return-by-reference inventory access (`stockReference`), plus const-reference GPS accessors.
- Overloaded `allocateSupplies` routines support standard volume allocation and emergency triage; `IncidentCommander` is a friend of `DistributionDepot`.
- Inline payload-utilization calculation.
- Generic `PriorityDispatchQueue<T>` and `CargoUrgencyFunctor` comparison behaviour.

### Unit II — Encapsulation, dynamic memory and operator overloading
- Encapsulated `ReliefCargo`, `DistributionDepot`, and `ReliefVehicle` classes.
- Static fleet count and dispatched-tonnage analytics.
- `CargoManifest` owns a dynamically allocated array and implements parameterized construction, deep copy construction, deep copy assignment and destruction.
- `+=`, `-=`, `==`, `<<`, and `>>` are implemented for depot/cargo operations.

### Unit III — Inheritance, containership and polymorphism
- Abstract `ReliefVehicle` base class with pure virtual `vehicleType()` and `evaluateRouteKm()`.
- `AerialTransport`, `GroundConvoy`, and `AmphibiousCraft` derive using virtual inheritance.
- `HybridMedicalVehicle` combines `GroundConvoy` and `AmphibiousCraft`; virtual inheritance resolves the diamond.
- `GPSBeacon`, `EmergencyLifeSupportUnit`, and `CargoManifest` are composed inside the vehicle base.
- `std::vector<ReliefVehicle*>` demonstrates runtime dispatch through base pointers and a virtual destructor.

### Unit IV — Streams, persistence, exceptions and STL
- Custom exceptions: `CargoOverweightException`, `FuelDepletionException`, and `DamagedCorridorException`.
- Binary incident log using `std::fstream`; updates are written to a temporary binary file and committed by rename.
- Uses `std::vector`, `std::unordered_map` for depot inventory, `std::sort`, and a custom functor.
- `PriorityDispatchQueue<T>` is a generic template.
- `tableHeader` is a user-defined stream manipulator.

## Build and run

### Linux / macOS
```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude src/ReliefHub.cpp src/main.cpp -o erl_hub
mkdir -p data
./erl_hub
```

### Windows (MinGW)
```powershell
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude src/ReliefHub.cpp src/main.cpp -o erl_hub.exe
mkdir data
.\\erl_hub.exe
```

### Run tests
```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude src/ReliefHub.cpp tests/test.cpp -o erl_hub_tests
./erl_hub_tests
```
Expected output ends with: `All ERL-HUB tests passed.`

## Project structure
```text
ERL_HUB_MAIT_OOPS_Project/
├── include/ReliefHub.hpp
├── src/ReliefHub.cpp
├── src/main.cpp
├── tests/test.cpp
├── docs/Project_Report.md
├── data/                 # runtime binary log is created here
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## Notes / limitations
- This is an academic simulation, not a certified emergency-dispatch platform. Route multipliers and sample stock figures are illustrative.
- The log stores native binary numeric representations; it is intended for the same compiler/platform family, not as a cross-platform interchange format.
- The application expects the `data` directory to exist before the first run; create it if needed.
- Review the report and add your name, enrollment number, section, faculty name and screenshots before submission.

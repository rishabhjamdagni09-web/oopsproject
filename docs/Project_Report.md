# ERL-HUB — Project Report
## Post-Earthquake Multi-Agency Emergency Relief Logistics Hub

**Institution:** Maharaja Agrasen Institute of Technology  
**Department:** Computer Science and Engineering  
**Subject:** Object-Oriented Programming Using C++  
**Paper Code:** CIC-211  
**Project Option:** 2  
**Student Name:** ____________________  
**Enrollment Number:** ____________________  
**Section:** ____________________  
**Submission Date:** ____________________

## 1. Abstract
ERL-HUB is a modular C++ console application designed to model relief logistics after a major earthquake. It supports cargo inventory, depot stock operations, transport fleet management, priority-based dispatch and binary incident history. The design demonstrates the four units of the CIC-211 assignment through encapsulation, dynamic memory, operator overloading, virtual inheritance, runtime polymorphism, templates, STL algorithms, exceptions and file streams.

## 2. Problem statement
After an earthquake, multiple agencies must distribute ration kits, medical supplies and clean water while operating different vehicle classes. A central logistics hub needs to track stock, prevent overloading, select urgent cargo first, evaluate routes and preserve dispatch records.

## 3. Objectives
- Maintain a depot inventory and deduct cargo when dispatching.
- Represent cargo and manifests with controlled access to data.
- Support ground, aerial, amphibious and hybrid medical transport.
- Prioritize critical supplies.
- Reject invalid dispatches through exception handling.
- Persist incident records in a binary file.

## 4. System modules
1. **Inventory module:** `ReliefCargo`, `DistributionDepot`, `CargoManifest`.
2. **Vehicle module:** `ReliefVehicle`, `AerialTransport`, `GroundConvoy`, `AmphibiousCraft`, `HybridMedicalVehicle`.
3. **Dispatch module:** generic `PriorityDispatchQueue<T>`, urgency functor and route evaluation.
4. **Persistence module:** `IncidentLogger` with binary transactional updates.
5. **Exception module:** cargo, fuel and damaged-corridor exception classes.

## 5. OOP concepts demonstrated
### Unit I
The code uses the required `ReliefOps::Logistics` and `ReliefOps::Inventory` namespaces. Overloaded `allocateSupplies` routines support standard allocation and emergency triage. `IncidentCommander` is a friend of `DistributionDepot` for authorized critical overrides. Inventory can be updated through a reference-returning accessor; GPS coordinates are exposed through const-reference accessors. Inline payload-utilization calculation and the generic queue provide reusable utility behaviour.

### Unit II
Private data members enforce information hiding. `ReliefVehicle` maintains static active-vehicle and dispatched-tonnage metrics. `CargoManifest` dynamically allocates an array, grows its capacity, deep-copies its contents and releases memory in its destructor. Depot operators `+=` and `-=` restock and deduct cargo; `==` compares depot identity.

### Unit III
`ReliefVehicle` is an abstract base class. The three transport branches virtually inherit it. `HybridMedicalVehicle` inherits from two branches, creating a diamond-shaped hierarchy resolved by virtual inheritance. GPS, life-support and manifest objects demonstrate composition. A vector of base-class pointers invokes derived route methods at runtime.

### Unit IV
Custom exception types report overweight cargo, fuel planning limits and damaged ground corridors. `IncidentLogger` uses binary file streams and a temporary-file commit strategy. It uses `seekp()` to return to the transaction boundary on write failure; reading uses binary stream operations. STL vectors, unordered maps, sorting and a generic template queue are included.

## 6. Workflow
1. Create depot stock.
2. Define cargo records and quantities.
3. Deduct dispatched quantities from depot stock.
4. Add cargo to a manifest.
5. Assign the manifest to a vehicle; payload validation prevents overload.
6. Evaluate each route using virtual dispatch.
7. Queue and process cargo according to urgency.
8. Append an incident record to the binary log.
9. Review fleet statistics and remaining inventory.

## 7. Testing
The test program verifies cargo weight and equality, depot restocking/deduction, manifest deep-copy behaviour, priority ordering, hybrid route evaluation, binary incident persistence and overweight-cargo exception handling. Build and run commands are provided in the README.

## 8. Sample expected behaviour
The main program prints a dispatch table for the fleet. A ground convoy is not dispatched if the corridor is damaged; aerial and hybrid vehicles can evaluate alternate routes. Critical medical cargo appears before lower-urgency cargo in the priority queue. The program reports the number of stored incidents and remaining depot stock.

## 9. Conclusion
ERL-HUB demonstrates a structured object-oriented design for a disaster-relief logistics scenario. The implementation covers the requested syllabus mechanisms and provides a foundation that could be extended with a graphical interface, real GPS feeds, role-based access and validated routing data.

## 10. Future enhancements
- Database-backed inventory and user authentication.
- Real map and road-closure integration.
- Multi-depot stock transfer.
- Role-based audit trails and signed incident records.
- A GUI dashboard for relief coordinators.

## 11. Academic integrity and finalization
This project is a working academic prototype. Before submission, the student should compile and run it on their own system, inspect the source, understand each demonstrated OOP concept, add their own identification details, and include genuine screenshots of their execution.

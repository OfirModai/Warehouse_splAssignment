#pragma once

#include <string>
#include <vector>
using std::string;
using std::vector;

class Volunteer;

enum class OrderStatus {
    PENDING,
    COLLECTING,
    DELIVERING,
    COMPLETED,
};

#define NO_VOLUNTEER -1

class Order {

    public:
        Order(int id, int customerId, int distance)
        : id(id), customerId(customerId), distance(distance), status(OrderStatus::PENDING),
         collectorId(NO_VOLUNTEER), driverId(NO_VOLUNTEER){}
        int getId() const {
            return id;
        }

        int getCustomerId() const {
            return customerId;
        }

        void setStatus(OrderStatus orderStatus) {
            status = orderStatus;
        }

        void setCollectorId(int orderCollectorId) {
            collectorId = orderCollectorId;
        }

        void setDriverId(int orderDriverId) {
            driverId = orderDriverId;
        }

        int getCollectorId() const {
            return collectorId;
        }

        int getDriverId() const {
            return driverId;
        }

        OrderStatus getStatus() const {
            return status;
        }

        // Function to convert Order information to string
        const std::string toString() const {
            std::string result = "Orderid: " + std::to_string(id) +
                                "\nOrderStatus: " + std::to_string(static_cast<int>(status)) +
                                "\nCustomerid: " + std::to_string(customerId) + "\nCollector: ";                                
            if (collectorId == NO_VOLUNTEER) result += "None ";
            else result += std::to_string(collectorId);
            result += "\nDriver: ";
            if (driverId == NO_VOLUNTEER) result += "None ";
            else result += std::to_string(driverId);
            return result;
        }

    private:
        const int id;
        const int customerId;
        const int distance;
        OrderStatus status;
        int collectorId; //Initialized to NO_VOLUNTEER if no collector has been assigned yet
        int driverId; //Initialized to NO_VOLUNTEER if no driver has been assigned yet
};
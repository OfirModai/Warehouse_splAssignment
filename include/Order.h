#pragma once

#include <string>
#include <vector>
using std::string;
using std::vector;

class Volunteer;

enum class OrderStatus
{
    PENDING,
    COLLECTING,
    DELIVERING,
    COMPLETED,
};

#define NO_VOLUNTEER -1

class Order
{

public:
    Order(int id, int customerId, int distance);
    int getId() const;

    int getCustomerId() const;

    void setStatus(OrderStatus orderStatus);

    void advanceStatus();

    void setCollectorId(int orderCollectorId);

    void setDriverId(int orderDriverId);

    int getCollectorId() const;

    int getDriverId() const;

    int getDistance() const;

    OrderStatus getStatus() const;

    const std::string toString() const;

    Order *clone();

    const std::string toString_close() const;

private:
    const int id;
    const int customerId;
    const int distance;
    OrderStatus status;
    int collectorId; // Initialized to NO_VOLUNTEER if no collector has been assigned yet
    int driverId;    // Initialized to NO_VOLUNTEER if no driver has been assigned yet
};
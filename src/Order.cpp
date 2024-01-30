#pragma once

#include <string>
#include <vector>
#include "../include/Order.h"
using std::string;
using std::vector;

Order::Order(int id, int customerId, int distance)
    : id(id), customerId(customerId), distance(distance), status(OrderStatus::PENDING),
      collectorId(NO_VOLUNTEER), driverId(NO_VOLUNTEER) {}

int Order::getId() const { return id; }

int Order::getCustomerId() const { return customerId; }

void Order::setStatus(OrderStatus orderStatus) { status = orderStatus; }

void Order::advanceStatus()
{
    int i = (int)status;
    i++;
    if (i == 4)
        i = 3;
    status = (OrderStatus)i;
}

void Order::setCollectorId(int orderCollectorId) { collectorId = orderCollectorId; }

void Order::setDriverId(int orderDriverId) { driverId = orderDriverId; }

int Order::getCollectorId() const { return collectorId; }

int Order::getDriverId() const { return driverId; }

int Order::getDistance() const { return distance; }

OrderStatus Order::getStatus() const { return status; }

const std::string Order::toString() const
{
    std::string result = "Orderid: " + std::to_string(id) +
                         "\nOrderStatus: " + std::to_string(static_cast<int>(status)) +
                         "\nCustomerid: " + std::to_string(customerId) + "\nCollector: ";
    if (collectorId == NO_VOLUNTEER)
        result += "None ";
    else
        result += std::to_string(collectorId);
    result += "\nDriver: ";
    if (driverId == NO_VOLUNTEER)
        result += "None ";
    else
        result += std::to_string(driverId);
    return result;
}

Order *Order::clone() { return new Order(*this); }

const std::string Order::toString_close() const
{
    return "OrderID: " + std::to_string(id) + " , CustomerID: " + std::to_string(customerId) + " , OrderStatus: " + std::to_string(static_cast<int>(status));
}
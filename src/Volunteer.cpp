#pragma once
#include <string>
#include <vector>
#include "../include/Order.h"
#include "../include/Volunteer.h"
#include <stdexcept>
using std::string;
using std::vector;

Volunteer::Volunteer(int id, const string &name)
    : id(id), name(name), completedOrderId(NO_ORDER), activeOrderId(NO_ORDER) {}
int Volunteer::getId() const { return id; };
const string &Volunteer::getName() const
{
    const string &ref_name = name;
    return ref_name;
}; // is it the same as to just return name?
int Volunteer::getActiveOrderId() const { return activeOrderId; }
int Volunteer::getCompletedOrderId() const { return completedOrderId; }
bool Volunteer::isBusy() const { return getActiveOrderId() != NO_ORDER; }
void Volunteer::resetActiveOrderId() { activeOrderId = NO_ORDER; }
void Volunteer::resetCompletedOrderId() { completedOrderId = NO_ORDER; }
void Volunteer::acceptOrder(const Order &order)
{
    activeOrderId = order.getId();
    completedOrderId = NO_ORDER;
}
string Volunteer::boolToString(bool value) const{
    return value ? "True" : "False";
}
string Volunteer::toString() const{
    std::string res = "VolunteerId: " + std::to_string(getId()) + "\nIsBusy: " + boolToString(isBusy()) + "\nOrderId: ";
    if (!isBusy())
    {
        res += "None";
    }
    else
    {
        res += std::to_string(getActiveOrderId());
    }
    return res;
}

CollectorVolunteer::CollectorVolunteer(int id, string name, int coolDown)
    : Volunteer(id, name), coolDown(coolDown), timeLeft(0) {}

CollectorVolunteer *CollectorVolunteer::clone() const
{
    return new CollectorVolunteer(getId(), getName(), coolDown);
}

void CollectorVolunteer::step()
{
    if (activeOrderId == NO_ORDER) return;
    if (decreaseCoolDown())
    {
        completedOrderId = activeOrderId;
        activeOrderId = NO_ORDER;
    }
}
bool CollectorVolunteer::isCollector() const { return true; }
int CollectorVolunteer::getCoolDown() const { return coolDown; };
int CollectorVolunteer::getTimeLeft() const { return timeLeft; };
bool CollectorVolunteer::decreaseCoolDown()
{
    if (timeLeft <= 0)
        throw std::runtime_error("already finished work");
    timeLeft--;
    return timeLeft == 0;
}
bool CollectorVolunteer::hasOrdersLeft() const { return true; }
bool CollectorVolunteer::canTakeOrder(const Order &order) const
{
    return activeOrderId == NO_ORDER & order.getStatus() == OrderStatus::PENDING;
}
void CollectorVolunteer::acceptOrder(const Order &order)
{
    if (!canTakeOrder(order))
        throw std::runtime_error("already working");
    Volunteer::acceptOrder(order);
    timeLeft = coolDown;
}

string CollectorVolunteer::toString() const{
    return collectorToString() + "\nordersLeft: No Limit";
}
string CollectorVolunteer::collectorToString() const
{
    string res = Volunteer::toString();
    res += "\ntimeLeft: ";
    if (timeLeft != 0) res += std::to_string(getTimeLeft());
    else res += "None";
    return res;
}

LimitedCollectorVolunteer::LimitedCollectorVolunteer(int id, string name, int coolDown, int maxOrders)
    : CollectorVolunteer(id, name, coolDown), maxOrders(maxOrders), ordersLeft(maxOrders){}
LimitedCollectorVolunteer *LimitedCollectorVolunteer::clone() const
{
    return new LimitedCollectorVolunteer(getId(), getName(), getCoolDown(), maxOrders);
}
bool LimitedCollectorVolunteer::hasOrdersLeft() const { return (ordersLeft > 0); }
bool LimitedCollectorVolunteer::canTakeOrder(const Order &order) const
{
    return CollectorVolunteer::canTakeOrder(order) & hasOrdersLeft();
}
void LimitedCollectorVolunteer::acceptOrder(const Order &order)
{
    if (!hasOrdersLeft())
        throw std::runtime_error("Volunterr has ran out of orders");
    CollectorVolunteer::acceptOrder(order);
    ordersLeft--;
}
int LimitedCollectorVolunteer::getMaxOrders() const { return maxOrders; }
int LimitedCollectorVolunteer::getNumOrdersLeft() const { return ordersLeft; }
string LimitedCollectorVolunteer::toString() const
{
    return CollectorVolunteer::collectorToString() + "\nordersLeft: " + std::to_string(ordersLeft);
}

DriverVolunteer::DriverVolunteer(int id, string name, int maxDistance, int distancePerStep)
    : Volunteer(id, name), maxDistance(maxDistance), distancePerStep(distancePerStep), distanceLeft(0) {}
DriverVolunteer *DriverVolunteer::clone() const
{
    return new DriverVolunteer(getId(), getName(), maxDistance, distancePerStep);
}
int DriverVolunteer::getDistanceLeft() const { return distanceLeft; }
int DriverVolunteer::getMaxDistance() const { return maxDistance; }
int DriverVolunteer::getDistancePerStep() const { return distancePerStep; }
bool DriverVolunteer::isCollector() const { return false; }
bool DriverVolunteer::decreaseDistanceLeft()
{
    if (distanceLeft <= distancePerStep)
    {
        distanceLeft = 0;
    }
    else
    {
        distanceLeft -= distancePerStep;
    }
    return (distanceLeft == 0);
}
bool DriverVolunteer::hasOrdersLeft() const { return true; }
bool DriverVolunteer::canTakeOrder(const Order &order) const
{
    return (activeOrderId == NO_ORDER & order.getDistance() <= maxDistance &
            order.getStatus() == OrderStatus::COLLECTING & order.getCollectorId() == NO_VOLUNTEER);
}
void DriverVolunteer::acceptOrder(const Order &order)
{
    if (!canTakeOrder(order))
        throw std::runtime_error("already working or order is too far");
    Volunteer::acceptOrder(order);
    distanceLeft = order.getDistance();
}
void DriverVolunteer::step()
{
    if (distanceLeft <= 0)
        return;
    distanceLeft -= distancePerStep;
    if (distanceLeft <= 0)
    {
        completedOrderId = activeOrderId;
        activeOrderId = NO_ORDER;
        distanceLeft = 0;
    }
}
string DriverVolunteer::driverToString() const{
    std::string res = Volunteer::toString() + "\nTimeLeft: ";
    if (distanceLeft != 0)
    {
        res += std::to_string(distanceLeft);
    }
    else
    {
        res += "None";
    }
    return res;
}
string DriverVolunteer::toString() const
{
    return driverToString() + "\nordersLeft: No Limit";
}

LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep, int maxOrders)
    : DriverVolunteer(id, name, maxDistance, distancePerStep), maxOrders(maxOrders), ordersLeft(maxOrders) {}
LimitedDriverVolunteer *LimitedDriverVolunteer::clone() const
{
    return new LimitedDriverVolunteer(getId(), getName(), getMaxDistance(), getDistanceLeft(), maxOrders);
}
int LimitedDriverVolunteer::getMaxOrders() const { return maxOrders; }
int LimitedDriverVolunteer::getNumOrdersLeft() const { return ordersLeft; }
bool LimitedDriverVolunteer::hasOrdersLeft() const { return ordersLeft > 0; }
bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const
{
    return (DriverVolunteer::canTakeOrder(order) & hasOrdersLeft());
}
void LimitedDriverVolunteer::acceptOrder(const Order &order)
{
    if (!hasOrdersLeft())
        throw std::runtime_error("Volunterr has ran out of orders");
    DriverVolunteer::acceptOrder(order);
    ordersLeft--;
}
string LimitedDriverVolunteer::toString() const
{
    return driverToString() + "\n ordersLeft: " + std::to_string(ordersLeft);
}
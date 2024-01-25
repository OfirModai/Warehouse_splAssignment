#pragma once
#include <string>
#include <vector>
#include "Order.h"
#include <stdexcept>
using std::string;
using std::vector;

#define NO_ORDER -1

class Volunteer
{
public:
    Volunteer(int id, const string &name);
    int getId() const;
    const string &getName() const;
    void resetActiveOrderId();
    void resetCompletedOrderId();
    int getActiveOrderId() const;
    int getCompletedOrderId() const;
    bool isBusy() const; // Signal whether the volunteer is currently processing an order
    virtual bool isCollector() const = 0;
    virtual bool hasOrdersLeft() const = 0;                  // Signal whether the volunteer didn't reach orders limit,Always true for CollectorVolunteer and DriverVolunteer
    virtual bool canTakeOrder(const Order &order) const = 0; // Signal if the volunteer can take the order.
    virtual void acceptOrder(const Order &order);            // Prepare for new order(Reset activeOrderId,TimeLeft,DistanceLeft,OrdersLeft depends on the volunteer type)
    virtual void step() = 0;                                 // Simulate volunteer step,if the volunteer finished the order, transfer activeOrderId to completedOrderId

    virtual string toString() const = 0;
    virtual Volunteer *clone() const = 0; // Return a copy of the volunteer

    Volunteer(int id, const string &name)
        : id(id), name(name), completedOrderId(NO_ORDER), activeOrderId(NO_ORDER) {}
    int getId() const { return id; };
    const string &getName() const
    {
        const string &ref_name = name;
        return ref_name;
    }; // is it the same as to just return name?
    int getActiveOrderId() const { return activeOrderId; }
    int getCompletedOrderId() const { return completedOrderId; }
    bool isBusy() const { return getActiveOrderId() != NO_ORDER; }
    void resetActiveOrderId() { activeOrderId = NO_ORDER; }
    void resetCompletedOrderId() { completedOrderId = NO_ORDER; }
    virtual void acceptOrder(const Order &order)
    {
        activeOrderId = order.getId();
        completedOrderId = NO_ORDER;
    }

protected:
    int completedOrderId; // Initialized to NO_ORDER if no order has been completed yet
    int activeOrderId;    // Initialized to NO_ORDER if no order is being processed

private:
    const int id;
    const string name;
};

class CollectorVolunteer : public Volunteer
{

public:
    CollectorVolunteer(int id, string name, int coolDown);
    CollectorVolunteer *clone() const override;
    void step() override;
    int getCoolDown() const;
    int getTimeLeft() const;
    bool decreaseCoolDown(); // Decrease timeLeft by 1,return true if timeLeft=0,false otherwise
    // we changed the signature
    bool hasOrdersLeft() const override;
    bool canTakeOrder(const Order &order) const override;
    void acceptOrder(const Order &order) override;
    string toString() const override;

    CollectorVolunteer(int id, string name, int coolDown)
        : Volunteer(id, name), coolDown(coolDown), timeLeft(0) {}

    CollectorVolunteer *clone() const override
    {
        return new CollectorVolunteer(getId(), getName(), coolDown);
    }

    void step()
    {
        if (decreaseCoolDown)
        {
            completedOrderId = activeOrderId;
            activeOrderId = NO_ORDER;
        }
    }
    bool isCollector() const override { return true; }
    int getCoolDown() const { return coolDown; };
    int getTimeLeft() const { return timeLeft; };
    bool decreaseCoolDown()
    {
        if (timeLeft <= 0)
            throw std::runtime_error("already finished work");
        timeLeft--;
        return timeLeft == 0;
    }
    bool hasOrdersLeft() const override { return true; }
    bool canTakeOrder(const Order &order) const override
    {
        return activeOrderId == NO_ORDER & order.getStatus() == OrderStatus::PENDING;
    }
    void acceptOrder(const Order &order) override
    {
        if (!canTakeOrder)
            throw std::runtime_error("already working");
        Volunteer::acceptOrder(order);
        timeLeft = coolDown;
    }
    string toString() const override
    {
        std::string res = "VolunteerId: " + std::to_string(getId()) + "\n IsBusy: " + std::to_string(isBusy()) + "\n OrderId: ";
        if (!isBusy())
        {
            res += "None";
        }
        else
        {
            res += std::to_string(getActiveOrderId()) + "\n timeLeft: ";
        }
        if (timeLeft != 0)
        {
            res += std::to_string(getTimeLeft());
        }
        else
        {
            res += "None";
        }
        res += "\n ordersLeft: No Limit";
        return res;
    }

private:
    const int coolDown; // The time it takes the volunteer to process an order
    int timeLeft;       // Time left until the volunteer finishes his current order
};

class LimitedCollectorVolunteer : public CollectorVolunteer
{

public:
    LimitedCollectorVolunteer(int id, string name, int coolDown, int maxOrders);
    LimitedCollectorVolunteer *clone() const override;
    bool hasOrdersLeft() const override;
    bool canTakeOrder(const Order &order) const override;
    void acceptOrder(const Order &order) override;

    int getMaxOrders() const;
    int getNumOrdersLeft() const;
    string toString() const override;

    LimitedCollectorVolunteer(int id, string name, int coolDown, int maxOrders)
        : CollectorVolunteer(id, name, coolDown), maxOrders(maxOrders) {}
    LimitedCollectorVolunteer *clone() const override
    {
        return new LimitedCollectorVolunteer(getId(), getName(), getCoolDown(), maxOrders);
    }
    bool hasOrdersLeft() const override { return ordersLeft > 0; }
    bool canTakeOrder(const Order &order) const override
    {
        return CollectorVolunteer::canTakeOrder(order) & hasOrdersLeft();
    }
    void acceptOrder(const Order &order) override
    {
        if (!hasOrdersLeft)
            throw std::runtime_error("Volunterr has ran out of orders");
        CollectorVolunteer::acceptOrder(order);
        ordersLeft--;
    }
    int getMaxOrders() const { return maxOrders; }
    int getNumOrdersLeft() const { return ordersLeft; }
    string toString() const override
    {
        std::string res = "VolunteerId: " + std::to_string(getId()) + "\n IsBusy: " + std::to_string(isBusy()) + "\n OrderId: ";
        if (!isBusy())
        {
            res += "None";
        }
        else
        {
            res += std::to_string(getActiveOrderId()) + "\n timeLeft: ";
        }
        if (getTimeLeft() != 0)
        {
            res += std::to_string(getTimeLeft());
        }
        else
        {
            res += "None";
        }
        res += "\n ordersLeft: " + ordersLeft;
        return res;
    }

private:
    const int maxOrders; // The number of orders the volunteer can process in the whole simulation
    int ordersLeft;      // The number of orders the volunteer can still take
};

class DriverVolunteer : public Volunteer
{

public:
    DriverVolunteer(int id, string name, int maxDistance, int distancePerStep);
    DriverVolunteer *clone() const override;

    int getDistanceLeft() const;
    int getMaxDistance() const;
    int getDistancePerStep() const;
    bool decreaseDistanceLeft(); // Decrease distanceLeft by distancePerStep,return true if distanceLeft<=0,false otherwise
    bool hasOrdersLeft() const override;
    bool canTakeOrder(const Order &order) const override; // Signal if the volunteer is not busy and the order is within the maxDistance
    void acceptOrder(const Order &order) override;        // Reset distanceLeft to order's distance
    void step() override;                                 // Decrease distanceLeft by distancePerStep
    string toString() const override;

    bool hasOrdersLeft() const override { return true; }
    bool canTakeOrder(const Order &order) const override { return activeOrderId == NO_ORDER; }

    DriverVolunteer(int id, string name, int maxDistance, int distancePerStep)
        : Volunteer(id, name), maxDistance(maxDistance), distancePerStep(distancePerStep), distanceLeft(0) {}
    DriverVolunteer *clone() const override
    {
        return new DriverVolunteer(getId(), getName(), maxDistance, distancePerStep);
    }
    int getDistanceLeft() const { return distanceLeft; }
    int getMaxDistance() const { return maxDistance; }
    int getDistancePerStep() const { return distancePerStep; }
    bool isCollector() const override { return false; }
    bool decreaseDistanceLeft()
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
    bool hasOrdersLeft() const override { return true; }
    bool canTakeOrder(const Order &order) const override
    {
        return (activeOrderId == NO_ORDER & order.getDistance() <= maxDistance &
                order.getStatus() == OrderStatus::COLLECTING & order.getCollectorId() == NO_VOLUNTEER);
    }
    void acceptOrder(const Order &order) override
    {
        if (!canTakeOrder)
            throw std::runtime_error("already working or order is too far");
        Volunteer::acceptOrder(order);
        distanceLeft = order.getDistance();
    }
    void step() override
    {
        if (distanceLeft <= 0)
            throw std::runtime_error("already finished work");
        distanceLeft -= distancePerStep;
        if (distanceLeft <= 0)
        {
            completedOrderId = activeOrderId;
            activeOrderId = NO_ORDER;
            distanceLeft = 0;
        }
    }
    string toString() const override
    {
        std::string res = "VolunteerId: " + std::to_string(getId()) + "\n IsBusy: " + std::to_string(isBusy()) + "\n OrderId: ";
        if (!isBusy())
        {
            res += "None";
        }
        else
        {
            res += std::to_string(getActiveOrderId()) + "\n timeLeft: ";
        }
        if (distanceLeft != 0)
        {
            res += std::to_string(distanceLeft);
        }
        else
        {
            res += "None";
        }
        res += "\n ordersLeft: No Limit";
        return res;
    }

private:
    const int maxDistance;     // The maximum distance of ANY order the volunteer can take
    const int distancePerStep; // The distance the volunteer does in one step
    int distanceLeft;          // Distance left until the volunteer finishes his current order
};

class LimitedDriverVolunteer : public DriverVolunteer
{

public:
    LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep, int maxOrders);
    LimitedDriverVolunteer *clone() const override;
    int getMaxOrders() const;
    int getNumOrdersLeft() const;
    bool hasOrdersLeft() const override;
    bool canTakeOrder(const Order &order) const override; // Signal if the volunteer is not busy, the order is within the maxDistance and have orders left
    void acceptOrder(const Order &order) override;        // Reset distanceLeft to maxDistance and decrease ordersLeft
    string toString() const override;

    bool hasOrdersLeft() const override { ordersLeft > 0; }
    bool canTakeOrder(const Order &order) const override
    {
        return DriverVolunteer::canTakeOrder(order) & hasOrdersLeft();
    }

    LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep, int maxOrders)
        : DriverVolunteer(id, name, maxDistance, distancePerStep), maxOrders(maxOrders) {}
    LimitedDriverVolunteer *clone() const override
    {
        return new LimitedDriverVolunteer(getId(), getName(), getMaxDistance(), getDistanceLeft(), maxOrders);
    }
    int getMaxOrders() const { return maxOrders; }
    int getNumOrdersLeft() const { return ordersLeft; }
    bool hasOrdersLeft() const override { return ordersLeft > 0; }
    bool canTakeOrder(const Order &order) const override
    {
        return (DriverVolunteer::canTakeOrder(order) & hasOrdersLeft());
    }
    void acceptOrder(const Order &order) override
    {
        if (!hasOrdersLeft)
            throw std::runtime_error("Volunterr has ran out of orders");
        DriverVolunteer::acceptOrder(order);
        ordersLeft--;
    }
    string toString() const override
    {
        // std::string res = Volunteer::toString();
        // if(isBusy()){ res += "\n distanceLeft: "; }
        // if (getDistanceLeft() != 0) { res += std::to_string(getDistanceLeft()) ; }
        // else { res += "None" ; }
        // res += "\n ordersLeft: " + ordersLeft;
        // return res;
        std::string res = "VolunteerId: " + std::to_string(getId()) + "\n IsBusy: " + std::to_string(isBusy()) + "\n OrderId: ";
        if (!isBusy())
        {
            res += "None";
        }
        else
        {
            res += std::to_string(getActiveOrderId()) + "\n timeLeft: ";
        }
        if (getDistanceLeft() != 0)
        {
            res += std::to_string(getDistanceLeft());
        }
        else
        {
            res += "None";
        }
        res += "\n ordersLeft: " + ordersLeft;
        return res;
    }

private:
    const int maxOrders; // The number of orders the volunteer can process in the whole simulation
    int ordersLeft;      // The number of orders the volunteer can still take
};
#pragma once
#include <string>
#include <vector>
#include <stdexcept>
using std::string;
using std::vector;

class Order;

class Customer
{
public:
    Customer(int id, const std::string &name, int locationDistance, int maxOrders)
        : id(id), name(name), locationDistance(locationDistance), maxOrders(maxOrders) {}
    const string &getName() const;
    int getId() const;
    int getCustomerDistance() const;
    int getMaxOrders() const;  // Returns maxOrders
    int getNumOrders() const;  // Returns num of orders the customer has made so far
    bool canMakeOrder() const; // Returns true if the customer didn't reach max orders
    vector<int> &getOrdersId() const;
    int addOrder(int orderId); // return OrderId if order was added successfully, -1 otherwise

    virtual Customer *clone() const = 0; // Return a copy of the customer

    const string &getName() const { return name; };
    int getId() const { return id; };
    int getCustomerDistance() const { return locationDistance; };
    int getMaxOrders() const { return maxOrders; };
    int getNumOrders() const { return ordersId.size(); };
    bool canMakeOrder() const
    {
        return maxOrders > ordersId.size();
    };
    int addOrder(int orderId)
    {
        if (canMakeOrder())
            ordersId.push_back(orderId);
        else
            throw std::runtime_error("too many orders");
    }

    vector<int> &getOrdersId() const
    {
        static vector<int> ref_ordersId = ordersId;
        return ref_ordersId;
    }
    virtual string getType() = 0;

private:
    const int id;
    const string name;
    const int locationDistance;
    const int maxOrders;
    vector<int> ordersId;
};

class SoldierCustomer : public Customer
{
public:
    SoldierCustomer(int id, string name, int locationDistance, int maxOrders)
        : Customer(id, name, locationDistance, maxOrders) {}
    SoldierCustomer *clone() const override
    {
        return new SoldierCustomer(*this);
    }
    string getType() override
    {
        return "Soldier";
    }

private:
};

class CivilianCustomer : public Customer
{
public:
    CivilianCustomer(int id, string name, int locationDistance, int maxOrders)
        : Customer(id, name, locationDistance, maxOrders) {}
    CivilianCustomer *clone() const override
    {
        return new CivilianCustomer(*this);
    }
    string getType() override
    {
        return "Civilian";
    }

private:
};
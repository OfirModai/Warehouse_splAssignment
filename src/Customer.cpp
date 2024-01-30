#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include "../include/Customer.h"
using std::string;
using std::vector;

Customer::Customer(int id, const std::string &name, int locationDistance, int maxOrders)
    : id(id), name(name), locationDistance(locationDistance), maxOrders(maxOrders) {}
const string &Customer::getName() const { return name; };
int Customer::getId() const { return id; };
int Customer::getCustomerDistance() const { return locationDistance; };
int Customer::getMaxOrders() const { return maxOrders; };
int Customer::getNumOrders() const { return ordersId.size(); };
bool Customer::canMakeOrder() const
{
    return maxOrders > ordersId.size();
};
int Customer::addOrder(int orderId)
{
    if (canMakeOrder())
        ordersId.push_back(orderId);
    else
        throw std::runtime_error("too many orders");
}

const vector<int> &Customer::getOrdersId() const
{
    static vector<int> ref_ordersId = ordersId;
    return ref_ordersId;
}

SoldierCustomer::SoldierCustomer(int id, string name, int locationDistance, int maxOrders)
    : Customer(id, name, locationDistance, maxOrders) {}
SoldierCustomer *SoldierCustomer::clone() const
{
    return new SoldierCustomer(*this);
}
string SoldierCustomer::getType()
{
    return "Soldier";
}

CivilianCustomer::CivilianCustomer(int id, string name, int locationDistance, int maxOrders)
    : Customer(id, name, locationDistance, maxOrders) {}
CivilianCustomer *CivilianCustomer::clone() const
{
    return new CivilianCustomer(*this);
}
string CivilianCustomer::getType()
{
    return "Civilian";
}

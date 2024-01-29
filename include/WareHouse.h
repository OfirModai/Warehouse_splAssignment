#pragma once
#include <string>
#include <vector>
using namespace std;

#include "Order.h"
#include "Customer.h"
#include "Volunteer.h"
#include "Action.h"

#include <fstream>
#include <iostream>
#include <sstream>

class BaseAction;
class Volunteer;

// Warehouse responsible for Volunteers, Customers Actions, and Orders.

class WareHouse
{

public:
    WareHouse(const string &configFilePath);
    WareHouse(const WareHouse &other);
    WareHouse& operator=(const WareHouse &other);
    WareHouse(WareHouse &&other) noexcept;
    WareHouse& operator=(WareHouse &&other) noexcept;
    ~WareHouse();
    void start(); // infinite loop which get inputs from the user
    void addOrder(Order *order);
    string addOrder(int customerId);
    void addAction(BaseAction *action);
    void addCustomer(const string &customerName, string customerType, int distance, int maxOrders);
    const vector<BaseAction *> &getActions() const;
    Customer &getCustomer(int customerId) const;
    Volunteer &getVolunteer(int volunteerId) const;
    Order &getOrder(int orderId) const;
    void close();
    void open();
    string getOrderStatus(int id);
    string getCustomerStatus(int id);
    void simulateStep();
    string getVolunteerStatus(int id);
    string getActionsLog();
    int orderCounter;

private:
    bool isOpen;
    vector<BaseAction *> actionsLog;
    vector<Volunteer *> volunteers;
    vector<Order *> pendingOrders;
    vector<Order *> inProcessOrders;
    vector<Order *> completedOrders;
    vector<Customer *> customers;
    int customerCounter;  // For assigning unique customer IDs
    int volunteerCounter; // For assigning unique volunteer IDs
    
    string orderStatusToString(OrderStatus status);
    void deleteVectors();
    void assignVectors(const WareHouse &other);
    void makeActionByString(string input);
};
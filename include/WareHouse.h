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
    WareHouse &operator=(const WareHouse &other);
    WareHouse::WareHouse(WareHouse &&other) noexcept;
    WareHouse &WareHouse::operator=(WareHouse &&other) noexcept;
    ~WareHouse();
    void start(); // infinite loop which get inputs from the user
    void addOrder(Order *order);
    string addOrder(int customerId);
    void addAction(BaseAction *action);
    void addCustomer(const string &customerName, CustomerType customerType, int distance, int maxOrders);
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
    int orderCounter;

    void deleteVectors()
    {
        while (actionsLog.size() > 0)
        {
            delete actionsLog.back();
            actionsLog.pop_back();
        }
        while (volunteers.size() > 0)
        {
            delete volunteers.back();
            volunteers.pop_back();
        }
        while (pendingOrders.size() > 0)
        {
            delete pendingOrders.back();
            pendingOrders.pop_back();
        }
        while (inProcessOrders.size() > 0)
        {
            delete inProcessOrders.back();
            inProcessOrders.pop_back();
        }
        while (completedOrders.size() > 0)
        {
            delete completedOrders.back();
            completedOrders.pop_back();
        }
        while (customers.size() > 0)
        {
            delete customers.back();
            customers.pop_back();
        }
    }
    void assignVectors(const WareHouse &other)
    {
        for (BaseAction *item : other.actionsLog)
        {
            actionsLog.push_back(item->clone());
        }
        for (Volunteer *item : other.volunteers)
        {
            volunteers.push_back(item->clone());
        }
        for (Order *item : other.pendingOrders)
        {
            pendingOrders.push_back(item->clone());
        }
        for (Order *item : other.inProcessOrders)
        {
            inProcessOrders.push_back(item->clone());
        }
        for (Order *item : other.completedOrders)
        {
            completedOrders.push_back(item->clone());
        }
        for (Customer *item : other.customers)
        {
            customers.push_back(item->clone());
        }
    }
    void do_it(string input)
    {
        BaseAction *action;
        if (input[0] == '0')
            action = new SimulateStep(std::stoi(input.substr(2)));
        else if (input[0] == '1')
            action = new AddOrder(std::stoi(input.substr(2)));
        else if (input[0] == '2')
        {
            std::istringstream iss(input.substr(2));
            std::string customerName, customerType;
            int distance, maxOrders;
            if (iss >> customerName >> customerType >> distance >> maxOrders)
                action = new AddCustomer(customerName, customerType, distance, maxOrders);
            else
            {
                cout << "Wrong input" << std::endl;
                return;
            }
        }
        else if (input[0] == '3')
            action = new PrintOrderStatus(std::stoi(input.substr(2)));
        else if (input[0] == '4')
            action = new PrintCustomerStatus(std::stoi(input.substr(2)));
        else if (input[0] == '5')
            action = new PrintVolunteerStatus(std::stoi(input.substr(2)));
        else if (input[0] == '6')
            action = new PrintActionsLog();
        else if (input[0] == '7')
            action = new Close();
        else if (input[0] == '8')
            action = new BackupWareHouse();
        else if (input[0] == '9')
            action = new RestoreWareHouse();
        else
        {
            cout << "Wrong input" << std::endl;
            return;
        }
        action->act(*this);
    }
};
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

    WareHouse(const string &configFilePath)
        : isOpen(false), customerCounter(0), volunteerCounter(0), orderCounter(0)
    {
        // Open the text file
        std::ifstream inputFile(configFilePath);
        // Check if the file is open
        if (!inputFile.is_open())
        {
            std::cerr << "Error opening file." << std::endl;
            throw runtime_error("can't open file"); // Return an error code
        }
        std::string line;
        // Read each line from the file
        while (std::getline(inputFile, line))
        {
            // Use a stringstream to parse the line
            std::istringstream iss(line);

            // Variables to store parsed values
            std::string type, name, advanced_type;
            int n1, n2, n3;

            // Parse the line using the stringstream
            if (iss >> type >> name >> advanced_type >> n1 >> n2 >> n3)
            {
                // this is a limited driver
                volunteers.push_back(new LimitedDriverVolunteer(volunteerCounter, name, n1, n2, n3));
                customerCounter++;
            }
            else if (iss >> type >> name >> advanced_type >> n1 >> n2)
            {
                // can be customer or driver or limited collector
                if (type == "customer")
                {
                    if (advanced_type == "soldier")
                        customers.push_back(new SoldierCustomer(customerCounter, name, n1, n2));
                    else
                        customers.push_back(new CivilianCustomer(customerCounter, name, n1, n2));
                    customerCounter++;
                }
                else if (type == "driver")
                {
                    volunteers.push_back(new DriverVolunteer(volunteerCounter, name, n1, n2));
                    volunteerCounter++;
                }
                else
                {
                    volunteers.push_back(new LimitedCollectorVolunteer(volunteerCounter, name, n1, n2));
                    volunteerCounter++;
                }
            }
            else if (iss >> type >> name >> advanced_type >> n1)
            {
                // this is a collector
                volunteers.push_back(new CollectorVolunteer(volunteerCounter, name, n1));
                volunteerCounter++;
            }
        }
        // Close the file
        inputFile.close();
    }
    WareHouse(const WareHouse &other)
        : isOpen(other.isOpen), customerCounter(other.customerCounter),
          volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter)
    {
        assignVectors(other);
    }
    WareHouse &operator=(const WareHouse &other)
    {
        if (&other == this)
            return *this;
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        deleteVectors();
        assignVectors(other);
        return *this;
    }
    ~WareHouse() { deleteVectors(); }
    void start()
    {
        isOpen = true;
        open();
    }
    void addOrder(Order *order)
    { // we got pointer, we are responsible to delete this object
        pendingOrders.push_back(order);
    }
    Customer &getCustomer(int customerId) const
    {
        for (int i = 0; i < customers.size(); i++)
        {
            if (customers[i]->getId() == customerId)
                return *customers[i];
        }
        throw runtime_error("no such customer");
    }
    Volunteer &getVolunteer(int volunteerId) const
    {
        for (int i = 0; i < volunteers.size(); i++)
        {
            if (volunteers[i]->getId() == volunteerId)
                return *volunteers[i];
        }
        throw runtime_error("no such volunteer");
    }
    Order &getOrder(int orderId) const
    {
        for (int i = 0; i < pendingOrders.size(); i++)
        {
            if (pendingOrders[i]->getId() == orderId)
                return *pendingOrders[i];
        }
        for (int i = 0; i < inProcessOrders.size(); i++)
        {
            if (inProcessOrders[i]->getId() == orderId)
                return *inProcessOrders[i];
        }
        for (int i = 0; i < completedOrders.size(); i++)
        {
            if (completedOrders[i]->getId() == orderId)
                return *completedOrders[i];
        }
        throw runtime_error("no such order");
    }
    void close()
    {
        for (Order *order : pendingOrders, inProcessOrders, completedOrders)
        {
            cout << order->toString_close() << std::endl;
        }
        delete this; // what happens??
    }
    void open()
    {
        std::string input;
        while (input != "7" & input != "9")
        {
            std::string menu = "You may enter an action as described: \n";
            menu += "SimulateStep: (<0> <numOfSteps>) \n";
            menu += "AddOrder: (<1> <customerID>) \n";
            menu += "AddCustomer: (<2> <customerName> <customerType> <distance> <maxOrders>) \n";
            menu += "PrintOrderStatus: (<3> <orderID>) \n";
            menu += "PrintCustomerStatus: (<4> <customerID>) \n";
            menu += "PrintVolunteerStatus: (<5> <volunteerID>) \n";
            menu += "PrintActionsLog: (<6>) \n";
            menu += "close: (<7>) \n";
            menu += "BackUpWareHouse: (<8>) \n";
            menu += "RestoreWareHouse: (<9>) \n";
            cout << menu << std::endl;

            cin >> input;
            do_it(input);
        }
    }

    string addOrder(int customerId)
    {
        int i;
        for (i = 0; i < customers.size(); i++)
        {
            if (customers[i]->getId() == customerId)
                break;
        }
        if (i == customers.size() || !customers[i]->canMakeOrder())
        {
            string message = "Wrong customer ID";
            if (i != customers.size())
                message = "Cannot place this order";
            return message;
        }
        addOrder(new Order(orderCounter, customerId, customers[i]->getCustomerDistance()));
        return "";
    }
    void addAction(BaseAction *action) { actionsLog.push_back(action); }
    void addCustomer(const string &customerName, CustomerType customerType, int distance, int maxOrders)
    {
        if (customerType == CustomerType::Civilian)
            customers.push_back(
                new CivilianCustomer(customerCounter, customerName, distance, maxOrders));
        else
            customers.push_back(
                new SoldierCustomer(customerCounter, customerName, distance, maxOrders));
    }
    void simulateStep()
    {
        // stage 1
        for (int i = 0; i < pendingOrders.size(); i++)
        {
            Order *order_i = pendingOrders[i];
            for (Volunteer *volunteer : volunteers)
            {
                if (volunteer->canTakeOrder(*order_i))
                {
                    volunteer->acceptOrder(*order_i);
                    OrderStatus status = order_i->getStatus();
                    if (status == OrderStatus::PENDING)
                        order_i->setCollectorId(volunteer->getId());
                    else
                        order_i->setDriverId(volunteer->getId());
                    order_i->advanceStatus();
                    inProcessOrders.push_back(order_i);
                    pendingOrders.erase(pendingOrders.begin() + i);
                    i--;
                }
            }
        }
        // stage 2
        for (int i = 0; i < volunteers.size(); i++)
        {
            volunteers[i]->step();
        }

        // stage 3
        for (Volunteer *volunteer : volunteers)
        {
            int id = volunteer->getCompletedOrderId();
            if (id != NO_ORDER)
            {
                for (int i = 0; i < inProcessOrders.size(); i++)
                {
                    if (id == inProcessOrders[i]->getId())
                    {
                        if (volunteer->isCollector())
                            pendingOrders.push_back(inProcessOrders[i]);
                        else
                            completedOrders.push_back(inProcessOrders[i]);
                        inProcessOrders.erase(inProcessOrders.begin() + i);
                        i--;
                    }
                }
            }
        }

        // stage 4
        for (int i = 0; i < volunteers.size(); i++)
        {
            if (!volunteers[i]->hasOrdersLeft() & volunteers[i]->getActiveOrderId() == NO_ORDER)
            {
                volunteers.erase(volunteers.begin() + i);
                i--;
            }
        }
    }
    string getOrderStatus(int id)
    {
        string res = "OrderId: " + std::to_string(id) + "\n";
        bool find = false;
        for (Order *order : pendingOrders, inProcessOrders, completedOrders)
        {
            if (order->getId() == id)
                return order->toString();
        }
        if (!find)
            return "Order doesn't exist";
    }
    string getCustomerStatus(int id)
    {
        string res = "CustomerID: " + std::to_string(id) + "\n";
        bool find = false;
        for (Customer *Customer : customers)
        {
            if (Customer->getId() == id)
            {
                find = true;
                vector<int> &ordersId = Customer->getOrdersId();
                for (int &ID : ordersId)
                {
                    res += "OrderID: " + std::to_string(ID) + "\n";
                    for (Order *order : pendingOrders, inProcessOrders, completedOrders)
                    {
                        if (order->getId() == ID)
                        {
                            res += "OrderStatus: " + std::to_string(static_cast<int>(order->getStatus())) + "\n";
                        }
                    }
                }
                res += "numOrdersLeft: " + Customer->getMaxOrders() - Customer->getNumOrders();
                break;
            }
        }
        if (!find)
            return "Customer doesn't exist";
    }
    string getVolunteerStatus(int id)
    {
        for (Volunteer *volunteer : volunteers)
        {
            if (volunteer->getId() == id)
            {
                return volunteer->toString();
            }
        }
        return "Volunteer doesn't exist";
    }
    string getActionsLog()
    {
        string res;
        for (BaseAction *action : actionsLog)
        {
            res += action->toString() + "\n";
        }
        return res;
    }

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
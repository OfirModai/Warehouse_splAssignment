#pragma once
#include <string>
#include <vector>
using namespace std;

#include "../include/Order.h"
#include "../include/Customer.h"
#include "../include/Volunteer.h"
#include "../include/Action.h"
#include "../include/WareHouse.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <tuple>

    WareHouse::WareHouse(const string &configFilePath)
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
            size_t pos = line.find('#');
            if(pos != std::string::npos) {
                line = line.substr(0, pos);
            }
            if(line.empty()) continue;
            // Variables to store parsed values
            std::string type, name, advanced_type;
            int n1=0, n2=0, n3=0;
            if (iss >> type >> name >> advanced_type >> n1 >> n2 >> n3){}
            if(n3!=0){
                // this is a limited driver
                volunteers.push_back(new LimitedDriverVolunteer(volunteerCounter, name, n1, n2, n3));
                volunteerCounter++;
            }
            else if (n2!=0)
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
                else if (advanced_type == "driver")
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
            else if (n1!=0)
            {
                // this is a collector
                volunteers.push_back(new CollectorVolunteer(volunteerCounter, name, n1));
                volunteerCounter++;
            }
        }
        // Close the file
        inputFile.close();
    }
    WareHouse::WareHouse(const WareHouse &other)
        : isOpen(other.isOpen), customerCounter(other.customerCounter),
          volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter)
    {
        assignVectors(other);
    }
    WareHouse &WareHouse::operator=(const WareHouse &other)
    {
        if (&other == this) return *this;
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        deleteVectors();
        assignVectors(other);
        return *this;
    }
    WareHouse::WareHouse(WareHouse &&other) noexcept
        : isOpen(other.isOpen), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter),
          actionsLog(std::move(other.actionsLog)), volunteers(std::move(other.volunteers)), pendingOrders(std::move(other.pendingOrders)),
          inProcessOrders(std::move(other.inProcessOrders)), completedOrders(std::move(other.completedOrders)), customers(std::move(other.customers)) {}
    WareHouse &WareHouse::operator=(WareHouse &&other) noexcept
    {
        if (this == &other)
            return *this;
        actionsLog.clear();
        volunteers.clear();
        pendingOrders.clear();
        inProcessOrders.clear();
        completedOrders.clear();
        customers.clear();

        actionsLog = std::move(other.actionsLog);
        volunteers = std::move(other.volunteers);
        pendingOrders = std::move(other.pendingOrders);
        inProcessOrders = std::move(other.inProcessOrders);
        completedOrders = std::move(other.completedOrders);
        customers = std::move(other.customers);
    }
    WareHouse::~WareHouse() { deleteVectors(); }

    void WareHouse::start()
    {
        isOpen = true;
        cout << "WareHouse is Open!" << std::endl;
        open();
    }
    void WareHouse::open()
    {
        std::string input;
        while (isOpen)
        {
            cout<<"You may enter action as described: "<< std::endl;
            std::getline(std::cin, input);
            makeActionByString(input);
        }
    }
    void WareHouse::addOrder(Order *order)
    { // we got pointer, we are responsible to delete this object
        pendingOrders.push_back(order);
        orderCounter++;
    }
    Customer &WareHouse::getCustomer(int customerId) const
    {
        for (int i = 0; i < customers.size(); i++)
        {
            if (customers[i]->getId() == customerId)
                return *customers[i];
        }
    }
    Volunteer &WareHouse::getVolunteer(int volunteerId) const
    {
        for (int i = 0; i < volunteers.size(); i++)
        {
            if (volunteers[i]->getId() == volunteerId)
                return *volunteers[i];
        }
    }
    Order &WareHouse::getOrder(int orderId) const
    {
        auto vectors = {pendingOrders, inProcessOrders, completedOrders};
        for (auto vector : vectors){
            for (Order *order : vector)
            {
                if (order->getId() == orderId) return *order;
            }
        }
        return -1;
    }
    void WareHouse::close()
    {
        auto vectors = {pendingOrders, inProcessOrders, completedOrders};
        for (auto vector : vectors){
            for (Order *order : vector)
            {
                cout << order->toString_close() << std::endl;
            }
        }
        isOpen = false;
    }
    
    string WareHouse::addOrder(int customerId)
    {
        int i;
        for (i = 0; i < customers.size(); i++)
        {
            if (customers[i]->getId() == customerId)
                break;
        }
        if (i == customers.size() || !customers[i]->canMakeOrder())
        {
            return "Cannot place this order";
        }
        customers[i]->addOrder(orderCounter);
        addOrder(new Order(orderCounter, customerId, customers[i]->getCustomerDistance()));
        return "";
    }
    void WareHouse::addAction(BaseAction *action) { actionsLog.push_back(action); }
    void WareHouse::addCustomer(const string &customerName, string customerType, int distance, int maxOrders)
    {
        if (customerType == "Civilian")
            customers.push_back(
                new CivilianCustomer(customerCounter, customerName, distance, maxOrders));
        else
            customers.push_back(
                new SoldierCustomer(customerCounter, customerName, distance, maxOrders));
        customerCounter++;
    }
    void WareHouse::simulateStep()
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
                    break;
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
                        if (volunteer->isCollector()){
                            pendingOrders.push_back(inProcessOrders[i]);
                        }
                        else{
                            completedOrders.push_back(inProcessOrders[i]);
                            inProcessOrders[i]->advanceStatus();
                        }
                        inProcessOrders.erase(inProcessOrders.begin() + i);
                        volunteer->resetCompletedOrderId();
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

    void WareHouse::deleteVectors()
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
    void WareHouse::assignVectors(const WareHouse &other)
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
    void WareHouse::makeActionByString(string& input)
    {
        BaseAction *action;
        std::istringstream iss(input);
        std::string firstWord;
        iss >> firstWord;
        if (firstWord == "step")
            action = new SimulateStep(std::stoi(input.substr(4)));
        else if (firstWord == "order")
            action = new AddOrder(std::stoi(input.substr(5)));
        else if (firstWord == "customer")
        {
            std::istringstream iss(input.substr(9));
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
        else if (firstWord == "orderStatus")
            action = new PrintOrderStatus(std::stoi(input.substr(12)));
        else if (firstWord == "customerStatus")
            action = new PrintCustomerStatus(std::stoi(input.substr(15)));
        else if (firstWord == "volunteerStatus")
            action = new PrintVolunteerStatus(std::stoi(input.substr(16)));
        else if (firstWord == "log")
            action = new PrintActionsLog();
        else if (firstWord == "close")
            action = new Close();
        else if (firstWord == "backup")
            action = new BackupWareHouse();
        else if (firstWord == "restore")
            action = new RestoreWareHouse();
        else
        {
            cout << "Wrong input" << std::endl;
            return;
        }
        action->act(*this);
        if(typeid(action) == typeid(Close)){
            // close is the only object which doesn't get deleted beacuse it 
        }
    }

    string WareHouse::getOrderStatus(int id)
    {
        auto vectors = {pendingOrders, inProcessOrders, completedOrders};
        for (auto vector : vectors){
            for (Order *order : vector)
            {
                if (order->getId() == id) return order->toString();
            }
        }
        return "Order doesn't exist";
    }
    string WareHouse::getCustomerStatus(int id)
    {
        string res = "CustomerID: " + std::to_string(id) + "\n";
        bool find = false;
        for (Customer *Customer : customers)
        {
            if (Customer->getId() == id)
            {
                find = true;
                const vector<int> &ordersId = Customer->getOrdersId();
                auto vectors = {pendingOrders, inProcessOrders, completedOrders};
                for (const int &ID : ordersId)
                {
                    res += "OrderID: " + std::to_string(ID) + "\n";
                    for (auto vector : vectors){
                        for (Order *order : vector)
                        {
                            if (order->getId() == ID)
                            {
                                res += "OrderStatus: " + order->getStringStatus() + "\n";
                            }
                        }
                    }
                }
                res += "numOrdersLeft: " + std::to_string(Customer->getMaxOrders() - Customer->getNumOrders());
                break;
            }
        }
        if (!find) return "Customer doesn't exist";
        return res;
    }
    string WareHouse::getVolunteerStatus(int id)
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
    string WareHouse::getActionsLog()
    {
        string res;
        for (BaseAction *action : actionsLog)
        {
            res += action->toString() + "\n";
        }
        return res;
    }
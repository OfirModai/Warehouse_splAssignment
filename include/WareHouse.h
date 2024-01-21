#pragma once
#include <string>
#include <vector>
using namespace std;

#include "Order.h"
#include "Customer.h"
#include "Volunteer.h"

#include <iostream>
#include <sstream>

class BaseAction;
class Volunteer;

// Warehouse responsible for Volunteers, Customers Actions, and Orders.


class WareHouse {

    public:
        WareHouse(const string &configFilePath);
        WareHouse(const WareHouse& other)
        :isOpen(other.isOpen), customerCounter(other.customerCounter),
         volunteerCounter(other.volunteerCounter){
            assignVectors(other);
        }
        WareHouse& operator=(const WareHouse& other){
            isOpen = other.isOpen;
            customerCounter = other.customerCounter;
            volunteerCounter = volunteerCounter;
            deleteVectors();
            assignVectors(other);
        }
        ~WareHouse(){deleteVectors();}
        void start();
        void addOrder(Order* order);
        void addAction(BaseAction* action);
        const vector<BaseAction*> &getActions() const;
        Customer &getCustomer(int customerId) const;
        Volunteer &getVolunteer(int volunteerId) const;
        Order &getOrder(int orderId) const;
        void close();
        void open(){isOpen=true;};

        WareHouse(const string& configFilePath)
        : isOpen(true), customerCounter(0), volunteerCounter(0)
        {
            istringstream iss(configFilePath); // this is a class which let me seperate the string
            string word;
            vector<vector<string>> lines;
            vector<string> curLine = {};
            while (iss >> word) {
                if (word == "\n"){
                    lines.push_back(curLine);
                    curLine = {};
                }
                else{
                    curLine.push_back(word);
                }
            }
            if (!curLine.empty())
            {
                lines.push_back(curLine);
            }
            string state = "customer";
            for(int i = 0;i < lines.size(); i++){
                if(lines[i][0] != state) state = lines[i][0];
                if(state == "customer"){
                    if(lines[i][2] == "soldier"){
                        customers.push_back(
                            new SoldierCustomer(customerCounter,lines[i][1],
                             std::stoi(lines[i][3]), std::stoi(lines[i][4])));
                    }
                    else{
                        customers.push_back(
                            new CivilianCustomer(customerCounter,lines[i][1],
                             std::stoi(lines[i][3]), std::stoi(lines[i][4])));
                    }
                    customerCounter++;
                }
                else{
                    // lines of volunteer
                    if(lines[i][2] == "collector"){
                        if(lines[i].size()==4){ // no limit given
                            volunteers.push_back(
                                new CollectorVolunteer(volunteerCounter, lines[i][1],
                                    std::stoi(lines[i][3])));
                        }
                        else volunteers.push_back(
                            new LimitedCollectorVolunteer(volunteerCounter, lines[i][1],
                                std::stoi(lines[i][3]), std::stoi(lines[i][4]))
                        );
                    }
                    else{
                        if(lines[i].size()==5){ // no limit given
                            volunteers.push_back(
                                new DriverVolunteer(volunteerCounter, lines[i][1],
                                    std::stoi(lines[i][3]), std::stoi(lines[i][4])));
                        }
                        else volunteers.push_back(
                            new LimitedDriverVolunteer(volunteerCounter, lines[i][1],
                                std::stoi(lines[i][3]), std::stoi(lines[i][4]),std::stoi(lines[i][5])));
                    }
                    volunteerCounter++;
                }
            }
        }
        
        void start() {isOpen=true;}
        void addOrder(Order* order){ //we got pointer, we are responsible to delete this object
            pendingOrders.push_back(order);
        }
        Customer& getCustomer(int customerId) const{
            for(int i = 0;i < customers.size();i++){
                if(customers[i]->getId() == customerId) return *customers[i];
            }
            throw runtime_error("no such customer");
        }
        Volunteer& getVolunteer(int volunteerId) const{
            for(int i = 0;i < volunteers.size();i++){
                if(volunteers[i]->getId() == volunteerId) return *volunteers[i];
            }
            throw runtime_error("no such volunteer");
        }
        Order &getOrder(int orderId) const{
            for(int i = 0;i < pendingOrders.size();i++){
                if(pendingOrders[i]->getId() == orderId) return *pendingOrders[i];
            }
            for(int i = 0;i < inProcessOrders.size();i++){
                if(inProcessOrders[i]->getId() == orderId) return *inProcessOrders[i];
            }
            for(int i = 0;i < completedOrders.size();i++){
                if(completedOrders[i]->getId() == orderId) return *completedOrders[i];
            }
            throw runtime_error("no such order");
        }

    private:
        bool isOpen;
        vector<BaseAction*> actionsLog;
        vector<Volunteer*> volunteers;
        vector<Order*> pendingOrders;
        vector<Order*> inProcessOrders;
        vector<Order*> completedOrders;
        vector<Customer*> customers;
        int customerCounter; //For assigning unique customer IDs
        int volunteerCounter; //For assigning unique volunteer IDs
        void deleteVectors(){
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
        void assignVectors(const WareHouse& other){
            for(BaseAction* item : other.actionsLog){
                actionsLog.push_back(item->clone());
            }
            for(Volunteer* item : other.volunteers){
                volunteers.push_back(item->clone());
            }
            for(Order* item : other.pendingOrders){
                pendingOrders.push_back(item->clone());
            }
            for(Order* item : other.inProcessOrders){
                inProcessOrders.push_back(item->clone());
            }
            for(Order* item : other.completedOrders){
                completedOrders.push_back(item->clone());
            }
            for(Customer* item : other.customers){
                customers.push_back(item->clone());
            }
        }
};
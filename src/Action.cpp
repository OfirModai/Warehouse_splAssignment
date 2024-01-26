#pragma once
#include <string>
#include <vector>
#include "WareHouse.h"
#include "main.h"
#include "Action.h"
using std::string;
using std::vector;

    ActionStatus BaseAction::getStatus() const { return status; };
    BaseAction::BaseAction() : errorMsg("didn't act yet"), status(ActionStatus::ERROR) {}
    void BaseAction::act(WareHouse &wareHouse)
    {
        wareHouse.addAction(this);
    }
    string BaseAction::toString() const
    {
        if (status == ActionStatus::COMPLETED)
            return "COMPLETED";
        return "ERROR";
    }

    void BaseAction::complete() { status = ActionStatus::COMPLETED; }
    void BaseAction::error(string errorMsg)
    {
        errorMsg = errorMsg;
        status = ActionStatus::ERROR;
    }
    string BaseAction::getErrorMsg() const { return errorMsg; }


    SimulateStep::SimulateStep(int numOfSteps) : numOfSteps(numOfSteps) {}
    void SimulateStep::act(WareHouse &warehouse)
    {
        BaseAction::act(warehouse);
        for (size_t i = 0; i < numOfSteps; i++)
            warehouse.simulateStep();
        complete();
    }
    SimulateStep *SimulateStep::clone() const { return new SimulateStep(*this); }
    std::string SimulateStep::toString() const
    {
        return "simulateStep " + std::to_string(numOfSteps) + " " + BaseAction::toString();
    }


    AddOrder::AddOrder(int id) : customerId(id) {}
    void AddOrder::act(WareHouse &wareHouse) 
    {
        BaseAction::act(wareHouse);
        string message = wareHouse.addOrder(customerId);
        if (message == "")
            complete();
        else
        {
            error(message);
            cout << this->toString();
        }
    }
    std::string AddOrder::toString() const 
    {
        return "order " + std::to_string(customerId) + " " + BaseAction::toString();
    }
    AddOrder *AddOrder::clone() const { return new AddOrder(*this); }

    AddCustomer::AddCustomer(string customerName, string customerType, int distance, int maxOrders)
        : customerName(customerName), customerType(convertStringToCustomerType(customerType)),
          distance(distance), maxOrders(maxOrders) {}
    void AddCustomer::act(WareHouse &wareHouse) 
    {
        BaseAction::act(wareHouse);
        wareHouse.addCustomer(customerName, std::to_string(static_cast<int>(customerType)), distance, maxOrders);
        complete();
    }
    AddCustomer *AddCustomer::clone() const 
    {
        return new AddCustomer(*this);
    }
    string AddCustomer::toString() const 
    {
        string annoying = "soldier";
        if (customerType == CustomerType::Civilian)
            annoying = "civilian";
        return "customer " + customerName + " " + annoying + " " +
               std::to_string(distance) + " " + std::to_string(maxOrders) + " " + BaseAction::toString();
    }

    CustomerType AddCustomer::convertStringToCustomerType(string s)
    {
        if (s == "civilian")
            return CustomerType::Civilian;
        else
            return CustomerType::Soldier;
    }
    PrintOrderStatus::PrintOrderStatus(int id) : orderId(id) {}
    void PrintOrderStatus::act(WareHouse &wareHouse) 
    {
        BaseAction::act(wareHouse);
        string s = wareHouse.getOrderStatus(orderId);
        if (s == "Order doesn't exist")
            error(s);
        else
            complete();
        std::cout << s << std::endl;
    }
    PrintOrderStatus *PrintOrderStatus::clone() const 
    {
        return new PrintOrderStatus(*this);
    }
    string PrintOrderStatus::toString() const 
    {
        return "PrintOrderStatus: " + BaseAction::toString();
    }


    PrintCustomerStatus::PrintCustomerStatus(int customerId) : customerId(customerId) {}
    void PrintCustomerStatus::act(WareHouse &wareHouse) 
    {
        BaseAction::act(wareHouse);
        string s = wareHouse.getCustomerStatus(customerId);
        if (s == "Customer doesn't exist")
            error(s);
        else
            complete();
        std::cout << s << std::endl;
    }
    PrintCustomerStatus *PrintCustomerStatus::clone() const 
    {
        return new PrintCustomerStatus(*this);
    }
    string PrintCustomerStatus::toString() const 
    {
        return "PrintCustomerStatus: " + BaseAction::toString();
    }

    PrintVolunteerStatus::PrintVolunteerStatus(int id) : VolunteerId(id) {}
    void PrintVolunteerStatus::act(WareHouse &wareHouse) 
    {
        BaseAction::act(wareHouse);
        string s = wareHouse.getVolunteerStatus(VolunteerId);
        if (s == "Volunteer doesn't exist")
            error(s);
        else
            complete();
        std::cout << s << std::endl;
    }
    PrintVolunteerStatus *PrintVolunteerStatus::clone() const 
    {
        return new PrintVolunteerStatus(*this);
    }
    string PrintVolunteerStatus::toString() const 
    {
        return "PrintVolunteerStatus: " + BaseAction::toString();
    }

    PrintActionsLog::PrintActionsLog(){};
    void PrintActionsLog::act(WareHouse &wareHouse) 
    {
        string s = wareHouse.getActionsLog();
        complete();
        std::cout << s << std::endl;
    }
    PrintActionsLog *PrintActionsLog::clone() const 
    {
        return new PrintActionsLog(*this);
    }
    string PrintActionsLog::toString() const 
    {
        return "PrintActionsLog: " + BaseAction::toString();
    }


    Close::Close(){};
    Close *Close::clone() const { return new Close(*this); }
    string Close::toString() const { return "Close: " + BaseAction::toString(); }
    void Close::act(WareHouse &wareHouse)
    {
        wareHouse.close();
        complete();
        std::cout << "end program" << std::endl;
    }


    BackupWareHouse::BackupWareHouse(){};
    void BackupWareHouse::act(WareHouse &wareHouse) 
    {
        if (backup != nullptr)
            delete backup;
        backup = new WareHouse(wareHouse);
        BaseAction::act(wareHouse);
        complete();
    }
    BackupWareHouse *BackupWareHouse::clone() const 
    {
        return new BackupWareHouse(*this);
    }
    string BackupWareHouse::toString() const 
    {
        return "BackupWarehouse: " + BaseAction::toString();
    }


    RestoreWareHouse::RestoreWareHouse(){};
    void RestoreWareHouse::act(WareHouse &wareHouse) 
    {
        BaseAction::act(wareHouse);
        if (backup != nullptr)
        {
            error("No Backup availble");
            std::cout << "No Backup availble" << std::endl;
        }
        else
        {
            to_restore = true;
            complete();
        }
    }
    RestoreWareHouse *RestoreWareHouse::clone() const 
    {
        return new RestoreWareHouse(*this);
    }
    string RestoreWareHouse::toString() const 
    {
        return "RestoreWareHouse: " + BaseAction::toString();
    }

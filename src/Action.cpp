#pragma once
#include <string>
#include <vector>
#include "WareHouse.h"
#include "main.cpp"
using std::string;
using std::vector;

enum class ActionStatus
{
    COMPLETED,
    ERROR
};

enum class CustomerType
{
    Soldier,
    Civilian
};

class Customer;

class BaseAction
{
public:
    BaseAction();
    ActionStatus getStatus() const;
    virtual void act(WareHouse &wareHouse);
    virtual string toString() const;
    virtual BaseAction *clone() const = 0;

    ActionStatus getStatus() const { return status; }
    BaseAction(): errorMsg("didn't act yet"), status(ActionStatus::ERROR){}
    virtual void act(WareHouse &wareHouse)
    {
        wareHouse.addAction(this);
    }
    virtual string toString() const{
        if(status==ActionStatus::COMPLETED) return "COMPLETED";
        return "ERROR";
    }
protected:
    void complete();
    void error(string errorMsg);
    string getErrorMsg() const;

    void complete() { status = ActionStatus::COMPLETED; }
    void error(string errorMsg)
    {
        errorMsg = errorMsg;
        status = ActionStatus::ERROR;
    }
    string getErrorMsg() const { return errorMsg; }

private:
    string errorMsg;
    ActionStatus status;
};

class SimulateStep : public BaseAction
{

public:
    SimulateStep(int numOfSteps);
    void act(WareHouse &wareHouse) override;
    std::string toString() const override;
    SimulateStep *clone() const override;

    SimulateStep(int numOfSteps) : numOfSteps(numOfSteps) {}
    void act(WareHouse &warehouse)
    {
        BaseAction::act(warehouse);
        for (size_t i = 0; i < numOfSteps; i++) warehouse.simulateStep();
        complete();
    }
    SimulateStep *clone() const override { return new SimulateStep(*this); }
    std::string toString() const override{
        return "simulateStep " + std::to_string(numOfSteps) + " " + BaseAction::toString();
    }

private:
    const int numOfSteps;
};

class AddOrder : public BaseAction
{
public:
    AddOrder(int id);
    void act(WareHouse &wareHouse) override;
    AddOrder *clone() const override;
    string toString() const override;

    AddOrder(int id) : customerId(id) {}
    void act(WareHouse &wareHouse) override
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
    std::string toString() const override{
        return "order " + std::to_string(customerId) + " " + BaseAction::toString();
    }
    AddOrder *clone() const override { return new AddOrder(*this); }

private:
    const int customerId;
};

class AddCustomer : public BaseAction
{
public:
    AddCustomer(string customerName, string customerType, int distance, int maxOrders);
    void act(WareHouse &wareHouse) override;
    AddCustomer *clone() const override;
    string toString() const override;

    AddCustomer(string customerName, string customerType, int distance, int maxOrders)
        : customerName(customerName), customerType(convertStringToCustomerType(customerType)),
          distance(distance), maxOrders(maxOrders) {}
    void act(WareHouse &wareHouse) override
    {
        BaseAction::act(wareHouse);
        wareHouse.addCustomer(customerName, customerType, distance, maxOrders);
        complete();
    }
    AddCustomer *clone() const override
    {
        return new AddCustomer(*this);
    }
    string toString() const override
    {
        string annoying = "soldier";
        if (customerType == CustomerType::Civilian)
            annoying = "civilian";
        return "customer " + customerName + " " + annoying + " " +
               std::to_string(distance) + " " + std::to_string(maxOrders) 
               + " " + BaseAction::toString();
    }

private:
    const string customerName;
    const CustomerType customerType;
    const int distance;
    const int maxOrders;

    CustomerType convertStringToCustomerType(string s)
    {
        if (s == "civilian")
            return CustomerType::Civilian;
        else
            return CustomerType::Soldier;
    }
};

class PrintOrderStatus : public BaseAction
{
public:
    PrintOrderStatus(int id);
    void act(WareHouse &wareHouse) override;
    PrintOrderStatus *clone() const override;
    string toString() const override;

    PrintOrderStatus(int id) : orderId(id) {}
    void act(WareHouse &wareHouse) override
    {
        BaseAction::act(wareHouse);
        string s = wareHouse.getOrderStatus(orderId);
        if(s=="Order doesn't exist") error(s);
        else complete();
        std::cout << s << std::endl;
    }
    PrintOrderStatus *clone() const override
    {
        return new PrintOrderStatus(*this);
    }
    string toString() const override
    {
        return "PrintOrderStatus: " + BaseAction::toString();
    }

private:
    const int orderId;
};

class PrintCustomerStatus : public BaseAction
{
public:
    PrintCustomerStatus(int customerId);
    void act(WareHouse &wareHouse) override;
    PrintCustomerStatus *clone() const override;
    string toString() const override;

    PrintCustomerStatus(int customerId) : customerId(customerId) {}
    void act(WareHouse &wareHouse) override
    {
        BaseAction::act(wareHouse);
        string s = wareHouse.getCustomerStatus(customerId);
        if(s=="Customer doesn't exist") error(s);
        else complete();
        std::cout << s << std::endl;
    }
    PrintCustomerStatus *clone() const override
    {
        return new PrintCustomerStatus(*this);
    }
    string toString() const override
    {
        return "PrintCustomerStatus: " + BaseAction::toString();
    }

private:
    const int customerId;
};

class PrintVolunteerStatus : public BaseAction
{
public:
    PrintVolunteerStatus(int id);
    void act(WareHouse &wareHouse) override;
    PrintVolunteerStatus *clone() const override;
    string toString() const override;

    PrintVolunteerStatus(int id) : VolunteerId(id) {}
    void act(WareHouse &wareHouse) override
    {
        BaseAction::act(wareHouse);
        string s = wareHouse.getVolunteerStatus(VolunteerId);
        if(s=="Volunteer doesn't exist") error(s);
        else complete();
        std::cout << s << std::endl;
    }
    PrintVolunteerStatus *clone() const override
    {
        return new PrintVolunteerStatus(*this);
    }
    string toString() const override
    {
        return "PrintVolunteerStatus: " + BaseAction::toString();
    }
private:
    const int VolunteerId;
};

class PrintActionsLog : public BaseAction
{
public:
    PrintActionsLog();
    void act(WareHouse &wareHouse) override;
    PrintActionsLog *clone() const override;
    string toString() const override;

    void act(WareHouse &wareHouse) override
    {
        string s = wareHouse.getActionsLog();
        complete();
        std::cout << s << std::endl;
    }
    PrintActionsLog *clone() const override
    {
        return new PrintActionsLog(*this);
    }
    string toString() const override
    {
        return "PrintActionsLog: " + BaseAction::toString();
    }
private:
};

class Close : public BaseAction
{
public:
    Close();
    Close *clone() const override;
    void act(WareHouse &wareHouse) override;
    string toString() const override;

    Close *clone() const override{return new Close(*this);}
    string toString() const override{return "Close: " + BaseAction::toString();}
    void act(WareHouse &wareHouse){
        wareHouse.close();
        complete();
        std::cout << "end program" << std::endl;
    }
private:
};

class BackupWareHouse : public BaseAction
{
public:
    BackupWareHouse();
    void act(WareHouse &wareHouse) override;
    BackupWareHouse *clone() const override;
    string toString() const override;

    void act(WareHouse &wareHouse) override{
        if(backup!=nullptr) delete backup;
        backup = new WareHouse(wareHouse);
        BaseAction::act(wareHouse);
        complete();
    }
    BackupWareHouse *clone() const override{
        return new BackupWareHouse(*this);
    }
    string toString() const override{
        return "BackupWarehouse: " + BaseAction::toString();
    }

private:
};

class RestoreWareHouse : public BaseAction
{
public:
    RestoreWareHouse();
    void act(WareHouse &wareHouse) override;
    RestoreWareHouse *clone() const override;
    string toString() const override;

    void act(WareHouse &wareHouse) override{
        BaseAction::act(wareHouse);
        if (backup != nullptr){
            error("No Backup availble");
            std::cout << "No Backup availble" << std::endl;
        }
        else{
            to_restore = true;
            complete();
        }
    }
    RestoreWareHouse *clone() const override{
        return new RestoreWareHouse(*this);
    }
    string toString() const override{
        return "RestoreWareHouse: " + BaseAction::toString();
    }

private:
};